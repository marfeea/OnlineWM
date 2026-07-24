"""Spawner for semantic Xform groups without geometry or physics schemas."""

from __future__ import annotations

from collections.abc import Callable
from typing import TYPE_CHECKING

from pxr import Usd

from isaaclab.sim.spawners.spawner_cfg import SpawnerCfg
from isaaclab.sim.utils import clone, create_prim, get_current_stage
from isaaclab.utils import configclass

if TYPE_CHECKING:
    from typing import Any


@clone
def spawn_xform(
    prim_path: str,
    cfg: Any,
    translation: tuple[float, float, float] | None = None,
    orientation: tuple[float, float, float, float] | None = None,
    **kwargs,
) -> Usd.Prim:
    """Create a pure Xform prim for the ``/station`` hierarchy."""

    del cfg, kwargs
    stage = get_current_stage()
    if stage.GetPrimAtPath(prim_path).IsValid():
        raise ValueError(f"Prim already exists: {prim_path}")
    return create_prim(
        prim_path,
        prim_type="Xform",
        translation=translation,
        orientation=orientation,
        stage=stage,
    )


@configclass
class XformSpawnerCfg(SpawnerCfg):
    """Isaac Lab spawner configuration for a pure Xform prim."""

    func: Callable = spawn_xform
