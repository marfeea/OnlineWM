"""Validate P3 official-vision, resume, fixed-policy, and performance evidence."""

from __future__ import annotations

import argparse
import json
from datetime import datetime
from pathlib import Path
from typing import Any


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def clean_console(path: Path) -> bool:
    """Reject tracebacks because Isaac App can turn Python failures into exit code zero."""
    text = path.read_text(encoding="utf-8", errors="replace")
    return "Traceback (most recent call last)" not in text and "CUDA out of memory" not in text


def _image_contract(run: dict[str, Any]) -> bool:
    images = run["images"]
    shape = images["shape"]
    return (
        images["observations"] > 0
        and isinstance(shape, list)
        and len(shape) == 4
        and shape[-1] == 3
        and images["shape_stable"]
        and images["dtype"] == "torch.uint8"
        and images["dtype_stable"]
        and images["rgb_batch"]
        and images["range_valid"]
    )


def _episode_semantics(run: dict[str, Any]) -> bool:
    semantics = run["episode_semantics"]
    return (
        semantics["observations"] > 0
        and semantics["is_first_matches_previous_done"]
        and semantics["is_last_matches_step_done"]
        and semantics["terminal_implies_last"]
        and semantics["reset_reward_zero"]
        and semantics["reset_frames"] > 0
        and semantics["terminal_frames"] > 0
    )


def _r2_loss(run: dict[str, Any]) -> bool:
    r2_loss = run["r2_loss"]
    return (
        run["updates"]["count"] > 0
        and run["updates"]["metrics_finite"]
        and r2_loss["observations"] > 1
        and r2_loss["finite"]
        and r2_loss["min"] is not None
        and r2_loss["max"] is not None
    )


def _vision_gradients(run: dict[str, Any]) -> bool:
    gradients = run["gradients"]
    modules = run["updates"]["modules"]
    return all(
        gradients[name]["observations"] > 0 and gradients[name]["finite"] and modules[name]["changed"]
        for name in ("encoder", "rssm")
    )


def _fixed_policy(run: dict[str, Any]) -> bool:
    policy = run["fixed_policy"]
    return (
        policy["frames"] > 0
        and policy["bytes"] > 0
        and policy["action_std"] > 1e-6
        and policy["action_min"] >= -1.000001
        and policy["action_max"] <= 1.000001
    )


def _performance(run: dict[str, Any]) -> bool:
    performance = run["performance"]
    return (
        performance["environment_fps"] is not None
        and performance["environment_fps"] > 0
        and performance["training_updates_per_second"] is not None
        and performance["training_updates_per_second"] > 0
        and (performance.get("training_fps", performance["training_updates_per_second"])) > 0
        and performance["wall_seconds"] is not None
        and performance["wall_seconds"] > 0
        and performance["peak_cuda_bytes"] is not None
        and performance["peak_cuda_bytes"] > 0
    )


def evaluate_gates(
    initial: dict[str, Any],
    resumed: dict[str, Any],
    initial_checkpoint: Path,
    resume_checkpoint: Path,
) -> dict[str, bool]:
    """Evaluate P3-G01 through P3-G07 from two gate runs."""
    resume = resumed["resume"]
    resume_valid = (
        resume["requested"]
        and resume["loaded"]
        and resume["source_agent_digest"] == resume["loaded_agent_digest"]
        and initial_checkpoint.is_file()
        and initial_checkpoint.stat().st_size > 0
        and resume_checkpoint.is_file()
        and resume_checkpoint.stat().st_size > 0
        and resumed["updates"]["count"] > 0
    )
    return {
        "P3-G01": _image_contract(initial) and _image_contract(resumed),
        "P3-G02": _episode_semantics(initial) and _episode_semantics(resumed),
        "P3-G03": _r2_loss(initial) and _r2_loss(resumed),
        "P3-G04": _vision_gradients(initial) and _vision_gradients(resumed),
        "P3-G05": _fixed_policy(initial) and _fixed_policy(resumed),
        "P3-G06": resume_valid and _fixed_policy(resumed),
        "P3-G07": _performance(initial) and _performance(resumed),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--initial-diagnostics", type=Path, required=True)
    parser.add_argument("--initial-console", type=Path, required=True)
    parser.add_argument("--resume-diagnostics", type=Path, required=True)
    parser.add_argument("--resume-console", type=Path, required=True)
    parser.add_argument("--initial-checkpoint", type=Path, required=True)
    parser.add_argument("--resume-checkpoint", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    initial = read_json(args.initial_diagnostics)
    resumed = read_json(args.resume_diagnostics)
    gates = evaluate_gates(initial, resumed, args.initial_checkpoint, args.resume_checkpoint)
    console_checks = {
        "initial_console_clean": clean_console(args.initial_console),
        "resume_console_clean": clean_console(args.resume_console),
    }
    result = {
        "schema_version": 1,
        "verified_at": datetime.now().astimezone().isoformat(),
        "gates": gates,
        "console_checks": console_checks,
        "pass": all(gates.values()) and all(console_checks.values()),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    return 0 if result["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
