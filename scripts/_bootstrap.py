"""Local source-path bootstrap for repository scripts."""

import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
PACKAGE_SOURCE = PROJECT_ROOT / "source" / "OnlineWM"


def add_package_source() -> None:
    """Make the local ``OnlineWM`` package importable without installation."""

    source = str(PACKAGE_SOURCE)
    if source not in sys.path:
        sys.path.insert(0, source)
