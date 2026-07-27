"""Validate P2 state-chain and resume evidence."""

from __future__ import annotations

import argparse
import json
import math
from datetime import datetime
from pathlib import Path
from typing import Any


def read_json(path: Path) -> dict[str, Any]:
    """Read one UTF-8 JSON object."""
    return json.loads(path.read_text(encoding="utf-8"))


def read_metrics(path: Path) -> list[dict[str, Any]]:
    """Read the upstream JSONL scalar log."""
    return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines() if line.strip()]


def clean_console(path: Path) -> bool:
    """Reject tracebacks because Isaac App can turn Python failures into exit code zero."""
    text = path.read_text(encoding="utf-8", errors="replace")
    return "Traceback (most recent call last)" not in text


def finite_tree(value: Any) -> bool:
    """Recursively validate numeric finiteness."""
    if isinstance(value, dict):
        return all(finite_tree(item) for item in value.values())
    if isinstance(value, list):
        return all(finite_tree(item) for item in value)
    if isinstance(value, int | float):
        return math.isfinite(float(value))
    return True


def validate_run(diagnostics: dict[str, Any], metrics: list[dict[str, Any]]) -> dict[str, bool]:
    """Evaluate per-run portions of P2 gates."""
    modules = diagnostics["updates"]["modules"]
    episode_rows = [row for row in metrics if "episode/score" in row and "episode/length" in row]
    return {
        "replay_continuous": (
            diagnostics["replay"]["samples"] > 0
            and diagnostics["replay"]["episode_constant_per_sequence"]
            and diagnostics["replay"]["no_internal_is_first"]
            and diagnostics["replay"]["time_index_contiguous"]
        ),
        "all_modules_updated": all(modules[name]["changed"] for name in ("rssm", "reward", "cont", "actor", "value")),
        "finite_numerics": (
            diagnostics["updates"]["count"] > 0
            and diagnostics["updates"]["metrics_finite"]
            and diagnostics["gradients"]["observations"] > 0
            and diagnostics["gradients"]["finite"]
            and diagnostics["latents"]["observations"] > 0
            and diagnostics["latents"]["finite"]
            and finite_tree(metrics)
        ),
        "actions_valid": (
            diagnostics["actions"]["count"] > 1
            and diagnostics["actions"]["finite"]
            and diagnostics["actions"]["std"] > 1e-6
            and diagnostics["actions"]["min"] >= -1.000001
            and diagnostics["actions"]["max"] <= 1.000001
        ),
        "episode_metrics": len(episode_rows) >= 2,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--initial-diagnostics", type=Path, required=True)
    parser.add_argument("--initial-metrics", type=Path, required=True)
    parser.add_argument("--initial-console", type=Path, required=True)
    parser.add_argument("--resume-diagnostics", type=Path, required=True)
    parser.add_argument("--resume-metrics", type=Path, required=True)
    parser.add_argument("--resume-console", type=Path, required=True)
    parser.add_argument("--initial-checkpoint", type=Path, required=True)
    parser.add_argument("--resume-checkpoint", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    initial = read_json(args.initial_diagnostics)
    resumed = read_json(args.resume_diagnostics)
    initial_checks = validate_run(initial, read_metrics(args.initial_metrics))
    resume_checks = validate_run(resumed, read_metrics(args.resume_metrics))
    initial_checks["console_clean"] = clean_console(args.initial_console)
    resume_checks["console_clean"] = clean_console(args.resume_console)
    resume_valid = (
        resumed["resume"]["loaded"]
        and resumed["resume"]["source_agent_digest"] == resumed["resume"]["loaded_agent_digest"]
        and resumed["updates"]["count"] > 0
        and all(item["changed"] for item in resumed["updates"]["modules"].values())
        and args.initial_checkpoint.stat().st_size > 0
        and args.resume_checkpoint.stat().st_size > 0
    )
    gates = {
        "P2-G01": initial_checks["replay_continuous"]
        and resume_checks["replay_continuous"]
        and initial_checks["console_clean"]
        and resume_checks["console_clean"],
        "P2-G02": initial_checks["all_modules_updated"] and resume_checks["all_modules_updated"],
        "P2-G03": initial_checks["finite_numerics"] and resume_checks["finite_numerics"],
        "P2-G04": initial_checks["actions_valid"] and resume_checks["actions_valid"],
        "P2-G05": resume_valid,
        "P2-G06": initial_checks["episode_metrics"] and resume_checks["episode_metrics"],
    }
    result = {
        "schema_version": 1,
        "verified_at": datetime.now().astimezone().isoformat(),
        "initial": initial_checks,
        "resume": resume_checks,
        "gates": gates,
        "pass": all(gates.values()),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    return 0 if result["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
