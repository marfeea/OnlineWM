"""Dynamic scene view with target reset support and robot contacts."""

from isaaclab import sim as sim_utils
from isaaclab.assets import RigidObjectCfg
from isaaclab.sensors import ContactSensorCfg
from isaaclab.utils import configclass

from ...configs.assets import ROBOT_CONTACT_PRIM_EXPR, asset_by_key, asset_path
from ...configs.task import TARGET_STATES
from .static_scene_cfg import TcpDockingStaticSceneCfg


@configclass
class TcpDockingDynamicSceneCfg(TcpDockingStaticSceneCfg):
    """Add a kinematic target rigid-body view and contact sensor."""

    ws_interactive_reagent_01_sample_bottle = RigidObjectCfg(
        prim_path="{ENV_REGEX_NS}/station/interactive/ws_interactive_reagent_01_sample_bottle",
        spawn=sim_utils.UsdFileCfg(
            usd_path=str(asset_path(asset_by_key("sample_bottle"))),
            rigid_props=sim_utils.RigidBodyPropertiesCfg(kinematic_enabled=True),
        ),
        init_state=RigidObjectCfg.InitialStateCfg(
            pos=TARGET_STATES[0].position_e,
            rot=TARGET_STATES[0].rotation_wxyz,
            lin_vel=(0.0, 0.0, 0.0),
            ang_vel=(0.0, 0.0, 0.0),
        ),
    )
    robot_contact = ContactSensorCfg(
        prim_path=ROBOT_CONTACT_PRIM_EXPR,
        update_period=0.0,
        history_length=1,
        debug_vis=False,
    )
