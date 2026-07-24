# Copyright (c) 2022-2025, The Isaac Lab Project Developers (https://github.com/isaac-sim/IsaacLab/blob/main/CONTRIBUTORS.md).
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause

"""OnlineWM Isaac Lab extension package."""


def _missing_optional_runtime(error: ModuleNotFoundError) -> bool:
    """Return whether an import failed only because Isaac is not running."""

    module_root = (error.name or "").split(".", maxsplit=1)[0]
    return module_root in {"carb", "gymnasium", "isaaclab", "isaaclab_tasks", "omni", "pxr"}


# Keep task and UI auto-registration inside Isaac while allowing pure scene
# configuration modules to be imported by lightweight validation tools.
try:
    from .tasks import *  # noqa: F403
except ModuleNotFoundError as error:
    if not _missing_optional_runtime(error):
        raise

try:
    from .ui_extension_example import *  # noqa: F403
except ModuleNotFoundError as error:
    if not _missing_optional_runtime(error):
        raise
