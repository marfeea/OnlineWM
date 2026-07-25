"""Validate both P0-G06 installation gates in the frozen Isaac Lab runtime."""

from __future__ import annotations

import argparse
import importlib
import importlib.metadata
import json
import subprocess
import sys
from datetime import datetime
from pathlib import Path

from isaaclab.app import AppLauncher

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--r2-dreamer-root", type=Path, required=True)
parser.add_argument("--expected-r2-commit", required=True)
parser.add_argument("--output", type=Path, required=True)
AppLauncher.add_app_launcher_args(parser)
args_cli = parser.parse_args()

app_launcher = AppLauncher(args_cli)
simulation_app = app_launcher.app

import gymnasium as gym
import torch
from tensordict import TensorDict
from torchrl._torchrl import (
    MinSegmentTreeFp32,
    MinSegmentTreeFp64,
    SumSegmentTreeFp32,
    SumSegmentTreeFp64,
)
from torchrl.data.replay_buffers import LazyTensorStorage, ReplayBuffer


def _distribution_details(name: str) -> dict[str, object]:
    distribution = importlib.metadata.distribution(name)
    direct_url_text = distribution.read_text("direct_url.json")
    direct_url = json.loads(direct_url_text) if direct_url_text else {}
    return {
        "name": distribution.metadata["Name"],
        "version": distribution.version,
        "location": str(distribution.locate_file("")),
        "direct_url": direct_url,
        "editable": bool(direct_url.get("dir_info", {}).get("editable")),
    }


def _is_below(path: Path, root: Path) -> bool:
    try:
        path.resolve().relative_to(root.resolve())
    except ValueError:
        return False
    return True


def main() -> int:
    r2_root = args_cli.r2_dreamer_root.resolve()
    git_commit = subprocess.run(
        [
            "git",
            "-c",
            f"safe.directory={r2_root.as_posix()}",
            "-C",
            str(r2_root),
            "rev-parse",
            "HEAD",
        ],
        check=True,
        capture_output=True,
        text=True,
    ).stdout.strip()

    # The upstream distribution only packages envs/ and optim/; its executable imports
    # top-level modules from the checkout. Mirror the official train.py launch semantics.
    sys.path.insert(0, str(r2_root))
    r2_modules = {}
    for module_name in (
        "buffer",
        "distributions",
        "dreamer",
        "envs",
        "envs.isaaclab",
        "networks",
        "optim",
        "rssm",
        "tools",
        "trainer",
    ):
        module = importlib.import_module(module_name)
        r2_modules[module_name] = str(Path(module.__file__).resolve())

    tensor_dict = TensorDict(
        {"value": torch.arange(6, dtype=torch.float32).reshape(2, 3)},
        batch_size=(2,),
    )
    replay = ReplayBuffer(storage=LazyTensorStorage(max_size=8))
    replay.extend(TensorDict({"value": torch.arange(4).unsqueeze(-1)}, batch_size=(4,)))
    replay_sample = replay.sample(2)
    torchrl_extension_symbols = (
        MinSegmentTreeFp32,
        MinSegmentTreeFp64,
        SumSegmentTreeFp32,
        SumSegmentTreeFp64,
    )

    import OnlineWM
    import OnlineWM.tasks  # noqa: F401

    onlinewm_file = Path(OnlineWM.__file__).resolve()
    onlinewm_expected_root = Path(__file__).resolve().parents[2] / "source" / "OnlineWM"
    registered_task = gym.spec("Template-Onlinewm-Direct-v0")

    r2_distribution = _distribution_details("r2dreamer")
    onlinewm_distribution = _distribution_details("OnlineWM")
    checks = {
        "python_is_frozen_entry": Path(sys.executable).resolve()
        == Path(r"D:\Anaconda\envs\isaaclab\python.exe").resolve(),
        "torch_preserved": torch.__version__ == "2.7.0+cu128",
        "cuda_available": torch.cuda.is_available(),
        "r2_commit_matches": git_commit == args_cli.expected_r2_commit,
        "r2_distribution_editable": bool(r2_distribution["editable"]),
        "r2_core_modules_from_frozen_checkout": all(
            _is_below(Path(module_path), r2_root) for module_path in r2_modules.values()
        ),
        "tensordict_operation": tensor_dict.batch_size == torch.Size([2]) and float(tensor_dict["value"].sum()) == 15.0,
        "torchrl_replay_operation": replay_sample.batch_size == torch.Size([2]),
        "torchrl_native_extension_loaded": all(symbol is not None for symbol in torchrl_extension_symbols),
        "onlinewm_distribution_editable": bool(onlinewm_distribution["editable"]),
        "onlinewm_import_from_project": _is_below(onlinewm_file, onlinewm_expected_root),
        "onlinewm_task_registered": registered_task.id == "Template-Onlinewm-Direct-v0",
    }
    result = {
        "schema_version": 1,
        "collected_at": datetime.now().astimezone().isoformat(),
        "python": {
            "executable": sys.executable,
            "version": sys.version,
        },
        "torch": {
            "version": torch.__version__,
            "cuda": torch.version.cuda,
            "cuda_available": torch.cuda.is_available(),
            "device": torch.cuda.get_device_name(0) if torch.cuda.is_available() else None,
        },
        "dependencies": {
            name: importlib.metadata.version(name)
            for name in (
                "gymnasium",
                "hydra-core",
                "numpy",
                "ruamel.yaml",
                "tensordict",
                "torchrl",
            )
        },
        "r2dreamer": {
            "root": str(r2_root),
            "commit": git_commit,
            "distribution": r2_distribution,
            "modules": r2_modules,
        },
        "onlinewm": {
            "module_file": str(onlinewm_file),
            "distribution": onlinewm_distribution,
            "registered_task": registered_task.id,
        },
        "checks": checks,
        "pass": all(checks.values()),
    }
    args_cli.output.parent.mkdir(parents=True, exist_ok=True)
    args_cli.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    return 0 if result["pass"] else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    finally:
        simulation_app.close()
