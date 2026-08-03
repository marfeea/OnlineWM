"""Run the frozen R2-Dreamer vision chain with P3 diagnostics and resume support."""

from __future__ import annotations

import argparse
import atexit
import importlib
import subprocess
import sys
import time
from contextlib import nullcontext
from datetime import datetime
from pathlib import Path
from types import MethodType
from typing import Any

import imageio.v2 as imageio
import torch
from hydra import compose, initialize_config_dir
from omegaconf import OmegaConf

P2_SCRIPT_ROOT = Path(__file__).resolve().parents[1] / "p2"
sys.path.insert(0, str(P2_SCRIPT_ROOT))

from p2_runtime import StrictEpisodeTrainer, tensor_state_digest  # noqa: E402
from p3_runtime import VisionDiagnostics, write_json  # noqa: E402


def git_value(repository: Path, *arguments: str) -> str:
    """Run a read-only Git query against the frozen checkout."""
    return subprocess.run(
        ["git", "-c", f"safe.directory={repository.as_posix()}", "-C", str(repository), *arguments],
        check=True,
        capture_output=True,
        text=True,
    ).stdout.strip()


def parse_args() -> argparse.Namespace:
    """Parse wrapper arguments; remaining values are Hydra overrides."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--r2-dreamer-root", type=Path, required=True)
    parser.add_argument("--diagnostics", type=Path, required=True)
    parser.add_argument("--checkpoint-out", type=Path, required=True)
    parser.add_argument("--samples-out", type=Path, required=True)
    parser.add_argument("--policy-video", type=Path, required=True)
    parser.add_argument("--policy-steps", type=int, default=64)
    parser.add_argument("--resume", type=Path)
    parser.add_argument("overrides", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.overrides[:1] == ["--"]:
        args.overrides = args.overrides[1:]
    return args


def save_rgb_samples(samples: dict[str, torch.Tensor], output: Path) -> list[str]:
    """Save representative raw RGB tensors as lossless PNG evidence."""
    output.mkdir(parents=True, exist_ok=True)
    saved = []
    for name, image in samples.items():
        path = output / f"{name}.png"
        imageio.imwrite(path, image.detach().cpu().numpy())
        saved.append(str(path.resolve()))
    return saved


@torch.no_grad()
def record_fixed_policy(agent: Any, envs: Any, steps: int, output: Path) -> dict[str, Any]:
    """Record one deterministic eval-mode rollout from the trained checkpoint state."""
    agent.eval()
    done = torch.ones(envs.env_num, dtype=torch.bool, device=agent.device)
    state = agent.get_initial_state(envs.env_num)
    action = state["prev_action"].clone()
    frames = []
    actions = []
    for _ in range(steps):
        transition, done = envs.step(action.detach(), done)
        transition = transition.to(agent.device, non_blocking=True)
        action, state = agent.act(transition.clone(), state, eval=True)
        action = action.clamp(-1.0, 1.0)
        state["prev_action"] = action
        frames.append(transition["image"][0].detach().cpu().numpy())
        actions.append(action.detach().cpu())
    output.parent.mkdir(parents=True, exist_ok=True)
    imageio.mimwrite(output, frames, fps=30, codec="libx264", macro_block_size=None)
    action_tensor = torch.stack(actions)
    agent.train()
    return {
        "path": str(output.resolve()),
        "frames": len(frames),
        "bytes": output.stat().st_size,
        "action_min": float(action_tensor.min()),
        "action_max": float(action_tensor.max()),
        "action_std": float(action_tensor.float().std(unbiased=False)),
    }


def main() -> int:
    args = parse_args()
    started = time.perf_counter()
    r2_root = args.r2_dreamer_root.resolve()
    sys.path.insert(0, str(r2_root))
    with initialize_config_dir(version_base=None, config_dir=str(r2_root / "configs")):
        config = compose(config_name="configs", overrides=args.overrides)
    logdir = Path(str(config.logdir)).expanduser().resolve()
    logdir.mkdir(parents=True, exist_ok=True)

    r2_tools = importlib.import_module("tools")
    buffer_module = importlib.import_module("buffer")
    dreamer_module = importlib.import_module("dreamer")
    envs_module = importlib.import_module("envs")
    trainer_module = importlib.import_module("trainer")
    trainer_module.OnlineTrainer.tools = r2_tools
    # The upstream AMP warm-up produces non-finite Barlow gradients on this RTX 4060.
    # Gate runs use full precision so every observed P3-G03/G04 update must be finite.
    dreamer_module.autocast = lambda **_kwargs: nullcontext()

    r2_tools.set_seed_everywhere(config.seed)
    if config.deterministic_run:
        r2_tools.enable_deterministic_run()
    console_handle = r2_tools.setup_console_log(logdir, filename="console.log")
    atexit.register(console_handle.close)
    logger = r2_tools.Logger(logdir)
    logger.log_hydra_config(config)
    (logdir / "resolved_config.yaml").write_text(OmegaConf.to_yaml(config, resolve=True), encoding="utf-8")

    print("Logdir", logdir)
    print("Create envs.")
    replay_buffer = buffer_module.Buffer(config.buffer)
    train_envs, eval_envs, obs_space, act_space = envs_module.make_envs(config.env)
    if "image" not in obs_space.spaces:
        raise RuntimeError("P3 requires an official vision environment exposing the 'image' observation.")
    print("Create agent.")
    agent = dreamer_module.Dreamer(config.model, obs_space, act_space).to(config.device)
    agent._scaler = torch.amp.GradScaler(enabled=False)
    if torch.cuda.is_available():
        torch.cuda.reset_peak_memory_stats(agent.device)

    resume_info: dict[str, Any] = {"requested": args.resume is not None, "loaded": False}
    if args.resume is not None:
        checkpoint = torch.load(args.resume.resolve(), map_location=config.device, weights_only=False)
        expected_digest = tensor_state_digest(checkpoint["agent_state_dict"])
        agent.load_state_dict(checkpoint["agent_state_dict"], strict=True)
        r2_tools.recursively_load_optim_state_dict(agent, checkpoint["optims_state_dict"])
        actual_digest = tensor_state_digest(agent.state_dict())
        if actual_digest != expected_digest:
            raise RuntimeError("Loaded agent state does not match checkpoint tensor digest.")
        resume_info = {
            "requested": True,
            "loaded": True,
            "source": str(args.resume.resolve()),
            "source_agent_digest": expected_digest,
            "loaded_agent_digest": actual_digest,
            "source_completed_env_steps": checkpoint.get("completed_env_steps"),
        }
        print("Loaded checkpoint", args.resume.resolve())

    diagnostics = VisionDiagnostics(agent)
    rgb_samples: dict[str, torch.Tensor] = {}
    original_env_step = train_envs.step

    def observed_env_step(action: torch.Tensor, previous_done: torch.Tensor):
        step_started = time.perf_counter()
        transition, step_done = original_env_step(action, previous_done)
        diagnostics.observe_env_step(time.perf_counter() - step_started, int((~previous_done).sum().cpu()))
        diagnostics.observe_transition(transition, previous_done, step_done)
        image = transition["image"]
        if "initial" not in rgb_samples:
            rgb_samples["initial"] = image[0].detach().cpu().clone()
        terminal_ids = step_done.nonzero(as_tuple=False).squeeze(-1)
        if "terminal" not in rgb_samples and terminal_ids.numel():
            rgb_samples["terminal"] = image[int(terminal_ids[0])].detach().cpu().clone()
        reset_ids = previous_done.nonzero(as_tuple=False).squeeze(-1)
        if "reset" not in rgb_samples and diagnostics.semantic_observations > 1 and reset_ids.numel():
            rgb_samples["reset"] = image[int(reset_ids[0])].detach().cpu().clone()
        return transition, step_done

    train_envs.step = observed_env_step
    original_agc = agent._agc

    def observed_agc(parameters: Any) -> None:
        diagnostics.observe_module_gradients(agent)
        original_agc(list(parameters))

    agent._agc = observed_agc
    original_update = agent.update

    def observed_update(self: Any, buffer: Any) -> dict[str, Any]:
        update_started = time.perf_counter()
        metrics = original_update(buffer)
        diagnostics.observe_train_update(time.perf_counter() - update_started)
        diagnostics.observe_update(metrics)
        return metrics

    agent.update = MethodType(observed_update, agent)
    upstream_trainer = trainer_module.OnlineTrainer(
        config.trainer,
        replay_buffer,
        logger,
        logdir,
        train_envs,
        eval_envs,
    )
    strict_trainer = StrictEpisodeTrainer(upstream_trainer, diagnostics)
    strict_trainer.begin(agent)
    fixed_policy = record_fixed_policy(agent, train_envs, args.policy_steps, args.policy_video.resolve())
    sample_paths = save_rgb_samples(rgb_samples, args.samples_out.resolve())
    peak_cuda_bytes = torch.cuda.max_memory_allocated(agent.device) if torch.cuda.is_available() else None
    diagnostics.set_runtime(
        time.perf_counter() - started,
        peak_cuda_bytes,
        training_batch_steps=int(config.batch_size * config.batch_length),
    )

    result = {
        "schema_version": 1,
        "completed_at": datetime.now().astimezone().isoformat(),
        "r2dreamer": {
            "root": str(r2_root),
            "commit": git_value(r2_root, "rev-parse", "HEAD"),
            "dirty": bool(git_value(r2_root, "status", "--short")),
        },
        "config": OmegaConf.to_container(config, resolve=True),
        "numerical_mode": "float32_updates_no_grad_scaling",
        "resume": resume_info,
        "completed_env_steps": strict_trainer.final_step,
        "episode_count": strict_trainer.episode_count,
        "skipped_updates_no_sequence": strict_trainer.skipped_updates_no_sequence,
        "rgb_samples": sample_paths,
        "fixed_policy": fixed_policy,
        **diagnostics.result(agent),
    }
    write_json(args.diagnostics.resolve(), result)
    checkpoint_payload = {
        "schema_version": 1,
        "saved_at": datetime.now().astimezone().isoformat(),
        "r2dreamer_commit": result["r2dreamer"]["commit"],
        "completed_env_steps": strict_trainer.final_step,
        "agent_state_dict": agent.state_dict(),
        "optims_state_dict": r2_tools.recursively_collect_optim_state_dict(agent),
        "diagnostics": result,
        "config": OmegaConf.to_container(config, resolve=True),
    }
    args.checkpoint_out.resolve().parent.mkdir(parents=True, exist_ok=True)
    torch.save(checkpoint_payload, args.checkpoint_out.resolve())
    print("Saved checkpoint", args.checkpoint_out.resolve())
    print("Saved diagnostics", args.diagnostics.resolve())
    print("Saved fixed-policy video", args.policy_video.resolve())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
