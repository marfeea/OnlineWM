"""Target poses required to assemble the migrated scene."""

from dataclasses import dataclass
from typing import Final


@dataclass(frozen=True)
class TargetState:
    """Discrete target pose in the per-environment frame."""

    name: str
    position_e: tuple[float, float, float]
    rotation_wxyz: tuple[float, float, float, float]
    preposition_e: tuple[float, float, float]


TARGET_STATES: Final = (
    TargetState("sample_bottle_state_01", (1.537, 0.203, 0.94), (0.0, 0.0, 0.0, 1.0), (1.537, 0.083, 0.94)),
    TargetState(
        "sample_bottle_state_02",
        (0.91167, 0.1753, 0.96789),
        (0.70710678, 0.0, 0.0, -0.70710678),
        (1.03167, 0.1753, 0.96789),
    ),
    TargetState(
        "sample_bottle_state_03",
        (0.91167, 0.03036, 0.96676),
        (0.70710678, 0.0, 0.0, -0.70710678),
        (1.03167, 0.03036, 0.96676),
    ),
    TargetState(
        "sample_bottle_state_04",
        (0.91235, -0.18557, 0.99091),
        (0.70710678, 0.0, 0.0, -0.70710678),
        (1.03235, -0.18557, 0.99091),
    ),
)
