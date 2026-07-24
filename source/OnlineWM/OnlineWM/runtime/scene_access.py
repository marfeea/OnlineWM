"""Minimal USD runtime adaptation for the migrated AUBO scene."""

from __future__ import annotations

from typing import Any

from ..configs.assets import ROBOT_PRIM_CONTRACT
from ..configs.scene import ENABLE_SELF_COLLISIONS, SOLVER_POSITION_ITERATIONS, SOLVER_VELOCITY_ITERATIONS


def apply_robot_articulation_baseline(stage: Any, expected_num_envs: int = 1) -> tuple[str, ...]:
    """Apply the migrated solver baseline to both robots in each environment."""

    from pxr import PhysxSchema

    suffixes = (
        f"/AUBObot/{ROBOT_PRIM_CONTRACT.articulation_prim}",
        f"/AUBObot_2/{ROBOT_PRIM_CONTRACT.articulation_prim}",
    )
    matched_paths: list[str] = []
    for prim in stage.Traverse():
        path = str(prim.GetPath())
        if not path.endswith(suffixes):
            continue
        api = PhysxSchema.PhysxArticulationAPI.Apply(prim)
        api.GetEnabledSelfCollisionsAttr().Set(ENABLE_SELF_COLLISIONS)
        api.GetSolverPositionIterationCountAttr().Set(SOLVER_POSITION_ITERATIONS)
        api.GetSolverVelocityIterationCountAttr().Set(SOLVER_VELOCITY_ITERATIONS)
        matched_paths.append(path)

    expected_count = 2 * expected_num_envs
    if len(matched_paths) != expected_count:
        raise RuntimeError(
            f"Expected {expected_count} AUBO articulations for {expected_num_envs} environments; got {matched_paths}"
        )
    return tuple(matched_paths)


def validate_contact_reporting(stage: Any, articulation_path: str) -> None:
    """Verify that contact reporting is active below an articulation."""

    contact_prims = [
        str(prim.GetPath())
        for prim in stage.Traverse()
        if str(prim.GetPath()).startswith(f"{articulation_path}/")
        and "PhysxContactReportAPI" in prim.GetAppliedSchemas()
    ]
    if not contact_prims:
        raise RuntimeError(f"No PhysxContactReportAPI found below {articulation_path}")
