"""Run the frozen R2-Dreamer state chain with P2 diagnostics and resume support."""

from __future__ import annotations

import argparse
import atexit
import importlib
import subprocess
import sys
from contextlib import nullcontext
from datetime import datetime
from pathlib import Path
from types import MethodType
from typing import Any

import torch
from hydra import compose, initialize_config_dir
from omegaconf import OmegaConf
from p2_runtime import (
    ChainDiagnostics,
    StrictEpisodeTrainer,
    tensor_state_digest,
    write_json,
)


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
    parser.add_argument("--resume", type=Path)
    parser.add_argument("overrides", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.overrides[:1] == ["--"]:
        args.overrides = args.overrides[1:]
    return args


def main() -> int:
    args = parse_args()
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
    # StrictEpisodeTrainer calls the same helper through the upstream trainer instance.
    trainer_module.OnlineTrainer.tools = r2_tools
    # P2-G03 requires every observed gradient to be finite. The frozen upstream
    # AMP path overflows on its first state-chain update before GradScaler backs
    # off. Use full precision for acceptance rather than hiding that event.
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
    print("Create agent.")
    agent = dreamer_module.Dreamer(config.model, obs_space, act_space).to(config.device)
    agent._scaler = torch.amp.GradScaler(enabled=False)

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
            "source_update_count": checkpoint.get("diagnostics", {}).get("updates", {}).get("count"),
        }
        print("Loaded checkpoint", args.resume.resolve())

    diagnostics = ChainDiagnostics(agent)
    original_sample = replay_buffer.sample

    def observed_sample() -> Any:
        result = original_sample()
        diagnostics.observe_replay(*result)
        return result

    replay_buffer.sample = observed_sample
    original_agc = agent._agc

    def observed_agc(parameters: Any) -> None:
        parameters = list(parameters)
        diagnostics.observe_gradients(parameters)
        original_agc(parameters)

    agent._agc = observed_agc
    original_update = agent.update

    def observed_update(self: Any, buffer: Any) -> dict[str, Any]:
        metrics = original_update(buffer)
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
