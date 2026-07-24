"""Three-camera scene for RGB, depth, normal, and segmentation capture."""

from __future__ import annotations

from isaaclab import sim as sim_utils
from isaaclab.sensors.camera import CameraCfg
from isaaclab.utils import configclass

from ...configs.vision_diagnostics import (
    DIAGNOSTIC_CAMERA_DATA_TYPES,
    DIAGNOSTIC_CAMERA_MODEL,
    DIAGNOSTIC_CAMERA_POSES,
    DiagnosticCameraPoseCfg,
)
from .dynamic_scene_cfg import TcpDockingDynamicSceneCfg


def make_diagnostic_camera_cfg(pose_cfg: DiagnosticCameraPoseCfg) -> CameraCfg:
    """Create a diagnostic camera while preserving raw segmentation IDs."""

    return CameraCfg(
        prim_path=f"{{ENV_REGEX_NS}}/{pose_cfg.prim_name}",
        update_period=0.0,
        height=DIAGNOSTIC_CAMERA_MODEL.height,
        width=DIAGNOSTIC_CAMERA_MODEL.width,
        data_types=list(DIAGNOSTIC_CAMERA_DATA_TYPES),
        colorize_semantic_segmentation=False,
        colorize_instance_id_segmentation=False,
        colorize_instance_segmentation=False,
        offset=CameraCfg.OffsetCfg(
            pos=pose_cfg.position_e,
            rot=pose_cfg.rotation_wxyz,
            convention=pose_cfg.pose_convention,
        ),
        spawn=sim_utils.PinholeCameraCfg(
            focal_length=DIAGNOSTIC_CAMERA_MODEL.focal_length_mm,
            focus_distance=DIAGNOSTIC_CAMERA_MODEL.focus_distance_mm,
            horizontal_aperture=DIAGNOSTIC_CAMERA_MODEL.horizontal_aperture_mm,
            clipping_range=DIAGNOSTIC_CAMERA_MODEL.clipping_range_m,
        ),
    )


@configclass
class TcpDockingVisionSceneCfg(TcpDockingDynamicSceneCfg):
    """Visual scene intended for diagnostics and world-model data capture."""

    camera_cfg = make_diagnostic_camera_cfg(DIAGNOSTIC_CAMERA_POSES[0])
    camera_cfg_2 = make_diagnostic_camera_cfg(DIAGNOSTIC_CAMERA_POSES[1])
    camera_cfg_3 = make_diagnostic_camera_cfg(DIAGNOSTIC_CAMERA_POSES[2])
