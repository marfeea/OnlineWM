"""Regression tests for the migrated scene's pure configuration contract."""

import sys
from pathlib import Path

PACKAGE_SOURCE = Path(__file__).resolve().parents[1] / "source" / "OnlineWM"
sys.path.insert(0, str(PACKAGE_SOURCE))

from OnlineWM.configs.assets import (  # noqa: E402
    ASSETS,
    ROBOT_PRIM_CONTRACT,
    asset_by_key,
    asset_path,
    resolve_asset_root,
)
from OnlineWM.configs.task import TARGET_STATES  # noqa: E402
from OnlineWM.configs.vision_diagnostics import (  # noqa: E402
    DIAGNOSTIC_CAMERA_DATA_TYPES,
    DIAGNOSTIC_CAMERA_MODEL,
    DIAGNOSTIC_CAMERA_POSES,
)


def test_required_external_assets_exist() -> None:
    root = resolve_asset_root()
    assert root.name == "Asset"
    assert all(asset_path(spec, root).is_file() for spec in ASSETS if spec.required)


def test_asset_and_robot_contracts_are_unambiguous() -> None:
    keys = [spec.key for spec in ASSETS]
    assert len(keys) == len(set(keys))
    assert asset_by_key("aubo_with_gripper").relative_path == Path("AUBO_E5/AUBO_E5_Withclaw.usd")
    assert len(ROBOT_PRIM_CONTRACT.arm_joints) == 6
    assert len(ROBOT_PRIM_CONTRACT.gripper_joints) == 2
    assert set(ROBOT_PRIM_CONTRACT.arm_joints).isdisjoint(ROBOT_PRIM_CONTRACT.gripper_joints)


def test_target_states_and_camera_views_are_stable() -> None:
    assert len(TARGET_STATES) == 4
    assert len({state.name for state in TARGET_STATES}) == 4
    assert len(DIAGNOSTIC_CAMERA_POSES) == 3
    assert len({pose.scene_name for pose in DIAGNOSTIC_CAMERA_POSES}) == 3
    assert DIAGNOSTIC_CAMERA_MODEL.width == 640
    assert DIAGNOSTIC_CAMERA_MODEL.height == 480
    assert "rgb" in DIAGNOSTIC_CAMERA_DATA_TYPES
    assert "instance_id_segmentation_fast" in DIAGNOSTIC_CAMERA_DATA_TYPES
