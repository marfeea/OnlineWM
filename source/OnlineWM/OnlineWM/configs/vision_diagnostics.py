"""Reproducible camera poses and imaging settings for scene diagnostics."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Final

from .scene import WORKSTATION_POSE_E

Vec3 = tuple[float, float, float]
QuatWxyz = tuple[float, float, float, float]


@dataclass(frozen=True)
class DiagnosticCameraPoseCfg:
    """Camera pose aligned with environment axes and offset from the station."""

    scene_name: str
    prim_name: str
    workstation_offset_e: Vec3
    rotation_wxyz: QuatWxyz
    pose_convention: str
    source_reference: str

    @property
    def position_e(self) -> Vec3:
        return tuple(
            origin + offset
            for origin, offset in zip(WORKSTATION_POSE_E.position, self.workstation_offset_e, strict=True)
        )


@dataclass(frozen=True)
class DiagnosticCameraModelCfg:
    """Pinhole camera model migrated from the Test project."""

    width: int = 640
    height: int = 480
    focal_length_mm: float = 24.0
    focus_distance_mm: float = 400.0
    horizontal_aperture_mm: float = 20.955
    clipping_range_m: tuple[float, float] = (0.1, 1.0e5)


DIAGNOSTIC_CAMERA_MODEL: Final = DiagnosticCameraModelCfg()
DIAGNOSTIC_CAMERA_DATA_TYPES: Final = (
    "rgb",
    "distance_to_image_plane",
    "normals",
    "semantic_segmentation",
    "instance_segmentation_fast",
    "instance_id_segmentation_fast",
)

DIAGNOSTIC_CAMERA_POSES: Final = (
    DiagnosticCameraPoseCfg(
        scene_name="camera_cfg",
        prim_name="CameraSensor",
        workstation_offset_e=(1.4, 0.0, 1.3),
        rotation_wxyz=(0.5, 0.5, 0.5, 0.5),
        pose_convention="opengl",
        source_reference="Test current camera_cfg",
    ),
    DiagnosticCameraPoseCfg(
        scene_name="camera_cfg_2",
        prim_name="CameraSensor_2",
        workstation_offset_e=(0.0, -0.8, 2.0),
        rotation_wxyz=(0.86603, 0.5, 0.0, 0.0),
        pose_convention="opengl",
        source_reference="Test current camera_cfg_2",
    ),
    DiagnosticCameraPoseCfg(
        scene_name="camera_cfg_3",
        prim_name="CameraSensor_3",
        workstation_offset_e=(-0.5, 0.0, 0.9),
        rotation_wxyz=(0.70711, 0.0, 0.70711, 0.0),
        pose_convention="opengl",
        source_reference="Test history 7c52956 camera_cfg",
    ),
)
