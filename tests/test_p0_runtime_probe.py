"""Pure-Python checks for the P0 runtime probe helpers."""

from __future__ import annotations

import importlib.util
from pathlib import Path

PROBE_PATH = Path(__file__).resolve().parents[1] / "scripts" / "p0" / "runtime_probe.py"


def load_probe_module():
    """Load the probe without importing the optional Isaac runtime."""

    spec = importlib.util.spec_from_file_location("p0_runtime_probe_for_test", PROBE_PATH)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_package_version_reports_python() -> None:
    probe = load_probe_module()
    assert probe.package_version("pip")
    assert probe.package_version("definitely-not-an-installed-distribution") is None


def test_module_path_is_absolute() -> None:
    probe = load_probe_module()
    path = probe.module_path(probe)
    assert path is not None
    assert Path(path).is_absolute()
