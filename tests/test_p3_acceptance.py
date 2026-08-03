from __future__ import annotations

import importlib.util
from pathlib import Path

MODULE_PATH = Path(__file__).parents[1] / "scripts" / "p3" / "verify_acceptance.py"
SPEC = importlib.util.spec_from_file_location("p3_verify_acceptance", MODULE_PATH)
assert SPEC and SPEC.loader
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


def passing_run() -> dict:
    return {
        "images": {
            "observations": 3,
            "shape": [4, 64, 64, 3],
            "shape_stable": True,
            "dtype": "torch.uint8",
            "dtype_stable": True,
            "rgb_batch": True,
            "range_valid": True,
        },
        "episode_semantics": {
            "observations": 3,
            "is_first_matches_previous_done": True,
            "is_last_matches_step_done": True,
            "terminal_implies_last": True,
            "reset_reward_zero": True,
            "reset_frames": 4,
            "terminal_frames": 1,
        },
        "r2_loss": {"observations": 2, "finite": True, "min": 1.0, "max": 2.0, "last": 1.5},
        "gradients": {
            "encoder": {"observations": 2, "finite": True},
            "rssm": {"observations": 2, "finite": True},
        },
        "updates": {
            "count": 2,
            "metrics_finite": True,
            "modules": {"encoder": {"changed": True}, "rssm": {"changed": True}},
        },
        "actions": {"count": 8, "min": -0.5, "max": 0.5, "std": 0.2, "finite": True},
        "fixed_policy": {"frames": 16, "bytes": 1024, "action_min": -0.4, "action_max": 0.4, "action_std": 0.1},
        "performance": {
            "environment_fps": 10.0,
            "training_updates_per_second": 2.0,
            "wall_seconds": 4.0,
            "peak_cuda_bytes": 2048,
        },
        "resume": {"requested": False, "loaded": False},
    }


def test_evaluate_gates_accepts_complete_initial_and_resume_evidence(tmp_path):
    initial = passing_run()
    resumed = passing_run()
    resumed["resume"] = {
        "requested": True,
        "loaded": True,
        "source_agent_digest": "abc",
        "loaded_agent_digest": "abc",
    }
    initial_checkpoint = tmp_path / "initial.pt"
    resume_checkpoint = tmp_path / "resume.pt"
    initial_checkpoint.write_bytes(b"initial")
    resume_checkpoint.write_bytes(b"resume")

    gates = MODULE.evaluate_gates(initial, resumed, initial_checkpoint, resume_checkpoint)

    assert all(gates.values())


def test_evaluate_gates_rejects_nonfinite_r2_loss(tmp_path):
    initial = passing_run()
    resumed = passing_run()
    resumed["resume"] = {
        "requested": True,
        "loaded": True,
        "source_agent_digest": "abc",
        "loaded_agent_digest": "abc",
    }
    resumed["r2_loss"]["finite"] = False
    initial_checkpoint = tmp_path / "initial.pt"
    resume_checkpoint = tmp_path / "resume.pt"
    initial_checkpoint.write_bytes(b"initial")
    resume_checkpoint.write_bytes(b"resume")

    gates = MODULE.evaluate_gates(initial, resumed, initial_checkpoint, resume_checkpoint)

    assert not gates["P3-G03"]
