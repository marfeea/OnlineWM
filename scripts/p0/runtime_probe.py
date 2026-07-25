"""Probe the P0 Isaac runtime after launching Isaac Sim in headless mode."""

from __future__ import annotations

import argparse
import importlib.metadata
import json
import platform
import site
import sys
from pathlib import Path
from typing import Any

PROJECT_ROOT = Path(__file__).resolve().parents[2]
PACKAGE_SOURCE = PROJECT_ROOT / "source" / "OnlineWM"
if str(PACKAGE_SOURCE) not in sys.path:
    sys.path.insert(0, str(PACKAGE_SOURCE))


def package_version(distribution: str) -> str | None:
    """Return an installed distribution version without importing it."""

    try:
        return importlib.metadata.version(distribution)
    except importlib.metadata.PackageNotFoundError:
        return None


def module_path(module: Any) -> str | None:
    """Return a stable absolute module path when one is available."""

    path = getattr(module, "__file__", None)
    return str(Path(path).resolve()) if path else None


def build_parser(app_launcher_type: Any) -> argparse.ArgumentParser:
    """Build the probe CLI before Isaac Sim is launched."""

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, help="Optional path for the machine-readable JSON result.")
    app_launcher_type.add_app_launcher_args(parser)
    return parser


def collect_runtime() -> dict[str, Any]:
    """Import the P1 stack and collect its versions and CUDA identity."""

    import gymnasium

    # OnlineWM is loaded from the working tree, not from user site-packages.
    import OnlineWM
    import rl_games
    import torch

    import isaaclab

    import isaaclab_tasks

    task_id = "Isaac-Cartpole-Direct-v0"
    task_spec = gymnasium.spec(task_id)
    cuda_available = torch.cuda.is_available()
    gpu_name = torch.cuda.get_device_name(0) if cuda_available else None
    gpu_capability = list(torch.cuda.get_device_capability(0)) if cuda_available else None

    return {
        "status": "PASS" if cuda_available else "FAIL",
        "python": {
            "executable": sys.executable,
            "version": sys.version,
            "platform": platform.platform(),
            "user_site_enabled": site.ENABLE_USER_SITE,
        },
        "packages": {
            "torch": package_version("torch"),
            "torchvision": package_version("torchvision"),
            "isaac_sim": package_version("isaacsim"),
            "isaac_lab_distribution": package_version("isaaclab"),
            "isaac_lab_tasks_distribution": package_version("isaaclab_tasks"),
            "rl_games": package_version("rl-games"),
            "gymnasium": package_version("gymnasium"),
            "tensordict": package_version("tensordict"),
            "onlinewm": package_version("OnlineWM"),
            "r2dreamer": package_version("r2dreamer"),
        },
        "modules": {
            "isaaclab": module_path(isaaclab),
            "isaaclab_tasks": module_path(isaaclab_tasks),
            "rl_games": module_path(rl_games),
            "onlinewm": module_path(OnlineWM),
        },
        "cuda": {
            "available": cuda_available,
            "torch_runtime": torch.version.cuda,
            "device_index": 0 if cuda_available else None,
            "device_name": gpu_name,
            "device_capability": gpu_capability,
        },
        "official_task": {
            "id": task_id,
            "registered": task_spec is not None,
            "entry_point": str(task_spec.entry_point),
        },
    }


def main() -> int:
    """Launch Isaac Sim, run the probe, and emit a delimited JSON record."""

    from isaaclab.app import AppLauncher

    parser = build_parser(AppLauncher)
    args = parser.parse_args()
    app_launcher = AppLauncher(args)
    simulation_app = app_launcher.app

    try:
        result = collect_runtime()
        payload = json.dumps(result, ensure_ascii=False, indent=2)
        if args.output:
            output_path = args.output.resolve()
            output_path.parent.mkdir(parents=True, exist_ok=True)
            output_path.write_text(payload + "\n", encoding="utf-8")
        print("P0_RUNTIME_JSON_BEGIN", flush=True)
        print(payload, flush=True)
        print("P0_RUNTIME_JSON_END", flush=True)
        return 0 if result["status"] == "PASS" else 1
    finally:
        simulation_app.close()


if __name__ == "__main__":
    raise SystemExit(main())
