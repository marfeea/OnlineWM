"""External asset paths and the AUBO USD prim contract."""

from __future__ import annotations

import os
from dataclasses import dataclass
from pathlib import Path
from typing import Final

ASSET_ROOT_ENV_VAR: Final = "ONLINEWM_ASSET_ROOT"


class AssetRootError(RuntimeError):
    """Raised when the external asset root cannot be resolved."""


@dataclass(frozen=True)
class AssetSpec:
    """Path contract for one asset kept outside this repository."""

    key: str
    relative_path: Path
    required: bool = True


@dataclass(frozen=True)
class RobotPrimContract:
    """Stable prim, rigid-body, and joint names in the gripper AUBO USD."""

    usd_default_prim: str
    articulation_prim: str
    flange_body: str
    arm_joints: tuple[str, ...]
    gripper_joints: tuple[str, ...]
    ignored_contact_bodies: tuple[str, ...]


ASSETS: Final = (
    AssetSpec("aubo_with_gripper", Path("AUBO_E5/AUBO_E5_Withclaw.usd")),
    AssetSpec("workstation", Path("QKL-HX-300-II-00/Part/WorkStation/WorkStation.usd")),
    AssetSpec("sample_bottle", Path("QKL-HX-300-II-00/Part/Reagent_01/M_Reagent_01.usd")),
    AssetSpec("laboratory", Path("Laboratory/M_Laboratory.usd"), required=False),
)

ROBOT_PRIM_CONTRACT: Final = RobotPrimContract(
    usd_default_prim="Root",
    articulation_prim="AUBO_E5",
    flange_body="Flange",
    arm_joints=("Joint1", "Joint2", "Joint3", "Joint4", "Joint5", "Flange"),
    gripper_joints=("UpperFinger", "DownFinger"),
    ignored_contact_bodies=("Base_Link",),
)

SCENE_ENTITY_AUBO: Final = "AUBObot"
SCENE_ENTITY_AUBO_2: Final = "AUBObot_2"
SCENE_ENTITY_TARGET: Final = "ws_interactive_reagent_01_sample_bottle"
ROBOT_CONTACT_PRIM_EXPR: Final = "{ENV_REGEX_NS}/AUBObot/AUBO_E5/.*"


def _discover_workspace_asset_root() -> Path | None:
    """Find an ``Asset`` directory adjacent to an ancestor workspace."""

    module_path = Path(__file__).resolve()
    for parent in module_path.parents:
        candidate = parent.parent / "Asset"
        if candidate.is_dir():
            return candidate.resolve()
    return None


def resolve_asset_root(explicit_root: str | os.PathLike[str] | None = None) -> Path:
    """Resolve the asset root from an argument, environment, or workspace layout."""

    if explicit_root is not None:
        root = Path(explicit_root).expanduser().resolve()
        source = "explicit argument"
    elif configured_root := os.environ.get(ASSET_ROOT_ENV_VAR):
        root = Path(configured_root).expanduser().resolve()
        source = ASSET_ROOT_ENV_VAR
    elif discovered_root := _discover_workspace_asset_root():
        root = discovered_root
        source = "workspace sibling discovery"
    else:
        raise AssetRootError(f"Cannot resolve the asset root. Set {ASSET_ROOT_ENV_VAR} or pass an explicit path.")

    if not root.is_dir():
        raise AssetRootError(f"Asset root does not exist (source: {source}): {root}")
    return root


def asset_path(spec: AssetSpec, explicit_root: str | os.PathLike[str] | None = None) -> Path:
    """Return the absolute path for an asset specification."""

    return resolve_asset_root(explicit_root) / spec.relative_path


def asset_by_key(key: str) -> AssetSpec:
    """Look up an asset by its stable unique key."""

    matches = [spec for spec in ASSETS if spec.key == key]
    if len(matches) != 1:
        raise KeyError(f"Asset key must exist exactly once: {key!r}; matches={len(matches)}")
    return matches[0]
