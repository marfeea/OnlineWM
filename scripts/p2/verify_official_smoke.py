"""Verify a completed upstream R2-Dreamer official state-chain smoke run."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
from datetime import datetime
from pathlib import Path
from typing import Any

import torch
import yaml


def tensor_values(value: Any):
    """Yield tensors recursively from checkpoint containers."""
    if isinstance(value, torch.Tensor):
        yield value
    elif isinstance(value, dict):
        for item in value.values():
            yield from tensor_values(item)
    elif isinstance(value, list | tuple):
        for item in value:
            yield from tensor_values(item)


def state_digest(state_dict: dict[str, torch.Tensor]) -> str:
    """Return a stable digest for an agent state dict."""
    digest = hashlib.sha256()
    for name, value in sorted(state_dict.items()):
        tensor = value.detach().cpu().contiguous()
        digest.update(name.encode("utf-8"))
        digest.update(str(tensor.dtype).encode("ascii"))
        digest.update(str(tuple(tensor.shape)).encode("ascii"))
        digest.update(tensor.numpy().tobytes())
    return digest.hexdigest()


def numeric_metrics_are_finite(rows: list[dict[str, Any]]) -> bool:
    """Check every numeric JSONL metric."""
    return all(math.isfinite(float(value)) for row in rows for value in row.values() if isinstance(value, int | float))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run-root", type=Path, required=True)
    parser.add_argument("--expected-steps", type=int, default=10000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    root = args.run_root.resolve()
    config = yaml.safe_load((root / ".hydra" / "config.yaml").read_text(encoding="utf-8"))
    metrics = [
        json.loads(line) for line in (root / "metrics.jsonl").read_text(encoding="utf-8").splitlines() if line.strip()
    ]
    checkpoint_path = root / "latest.pt"
    checkpoint = torch.load(checkpoint_path, map_location="cpu", weights_only=False)
    agent_tensors = list(tensor_values(checkpoint["agent_state_dict"]))
    optimizer_tensors = list(tensor_values(checkpoint["optims_state_dict"]))
    training_rows = [row for row in metrics if "train/opt/loss" in row]
    episode_rows = [row for row in metrics if "episode/score" in row]
    log_text = "\n".join(
        path.read_text(encoding="utf-8", errors="replace")
        for path in (
            root / "console.log",
            root / "launcher.stdout.log",
            root / "launcher.stderr.log",
        )
        if path.exists()
    )
    model = config["model"]
    env = config["env"]
    checks = {
        "official_state_config": (
            env["steps"] == args.expected_steps
            and env["env_num"] == 4
            and env["train_ratio"] == 512
            and config["batch_size"] == 16
            and config["batch_length"] == 64
            and model["rep_loss"] == "r2dreamer"
            and model["deter"] == 2048
            and model["hidden"] == 256
            and model["discrete"] == 16
            and model["units"] == 256
        ),
        "windows_compile_override_only": model["compile"] is False and model["log_grads"] is False,
        # Frozen upstream train.py saves latest.pt only after begin() returns;
        # begin() returns once its internal step counter reaches env.steps.
        "training_loop_completed": checkpoint_path.stat().st_size > 0,
        "training_metrics_present": bool(training_rows) and training_rows[-1]["train/opt/updates"] > 0,
        "episode_metrics_present": len(episode_rows) > 1,
        "metrics_finite": numeric_metrics_are_finite(metrics),
        "agent_checkpoint_finite": bool(agent_tensors)
        and all(bool(torch.isfinite(tensor).all()) for tensor in agent_tensors),
        "optimizer_checkpoint_finite": bool(optimizer_tensors)
        and all(bool(torch.isfinite(tensor).all()) for tensor in optimizer_tensors),
        "traceback_absent": "Traceback (most recent call last)" not in log_text,
        "cuda_oom_absent": "CUDA out of memory" not in log_text,
    }
    initial_scores = [float(row["episode/score"]) for row in episode_rows[:10]]
    final_scores = [float(row["episode/score"]) for row in episode_rows[-10:]]
    result = {
        "schema_version": 1,
        "verified_at": datetime.now().astimezone().isoformat(),
        "run_root": str(root),
        "completion_semantics": (
            "latest.pt is written by frozen upstream train.py only after "
            "OnlineTrainer.begin returns at configured env.steps"
        ),
        "config": {
            "steps": env["steps"],
            "env_num": env["env_num"],
            "train_ratio": env["train_ratio"],
            "batch_size": config["batch_size"],
            "batch_length": config["batch_length"],
            "rep_loss": model["rep_loss"],
            "compile": model["compile"],
            "log_grads": model["log_grads"],
            "deter": model["deter"],
            "hidden": model["hidden"],
            "discrete": model["discrete"],
            "units": model["units"],
        },
        "metrics": {
            "rows": len(metrics),
            "max_logged_step": max(row["step"] for row in metrics),
            "note": "Upstream logger has no unconditional final-step write.",
            "training_rows": len(training_rows),
            "episode_rows": len(episode_rows),
            "last_logged_updates": training_rows[-1]["train/opt/updates"],
            "last_logged_loss": training_rows[-1]["train/opt/loss"],
            "initial_episode_score_mean": sum(initial_scores) / len(initial_scores),
            "final_episode_score_mean": sum(final_scores) / len(final_scores),
            "last_episode_score": episode_rows[-1]["episode/score"],
        },
        "checkpoint": {
            "path": str(checkpoint_path),
            "bytes": checkpoint_path.stat().st_size,
            "agent_tensors": len(agent_tensors),
            "optimizer_tensors": len(optimizer_tensors),
            "agent_digest": state_digest(checkpoint["agent_state_dict"]),
        },
        "checks": checks,
        "pass": all(checks.values()),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    return 0 if result["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
