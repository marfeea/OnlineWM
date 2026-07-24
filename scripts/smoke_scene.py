"""Create the migrated static scene and validate both AUBO articulations."""

from __future__ import annotations

import argparse
import traceback

from _bootstrap import add_package_source

add_package_source()

try:
    from isaaclab.app import AppLauncher
except ModuleNotFoundError as error:
    raise SystemExit(f"Run this script with an Isaac Lab Python interpreter: {error}") from error

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--num-envs", type=int, default=1)
AppLauncher.add_app_launcher_args(parser)
args_cli = parser.parse_args()

app_launcher = AppLauncher(args_cli)
simulation_app = app_launcher.app

import torch  # noqa: E402
from OnlineWM.configs.assets import ROBOT_PRIM_CONTRACT  # noqa: E402
from OnlineWM.configs.training import SIMULATION_DT_S  # noqa: E402
from OnlineWM.runtime.scene_access import apply_robot_articulation_baseline, validate_contact_reporting  # noqa: E402
from OnlineWM.tasks.tcp_docking.static_scene_cfg import TcpDockingStaticSceneCfg  # noqa: E402

from omni.usd import get_context  # noqa: E402

import isaaclab.sim as sim_utils  # noqa: E402
from isaaclab.scene import InteractiveScene  # noqa: E402


def main() -> int:
    if args_cli.num_envs != 1:
        raise ValueError("The static scene smoke test currently requires --num-envs 1")

    sim = sim_utils.SimulationContext(sim_utils.SimulationCfg(dt=SIMULATION_DT_S, device=args_cli.device))
    scene = InteractiveScene(TcpDockingStaticSceneCfg(num_envs=1, env_spacing=4.0))

    stage = get_context().get_stage()
    if stage is None:
        raise RuntimeError("The current omni.usd stage is empty")
    articulation_paths = apply_robot_articulation_baseline(stage)
    for articulation_path in articulation_paths:
        validate_contact_reporting(stage, articulation_path)

    sim.reset()
    scene.reset()
    scene.write_data_to_sim()
    sim.step()
    scene.update(SIMULATION_DT_S)

    for entity_name in ("AUBObot", "AUBObot_2"):
        robot = scene[entity_name]
        arm_ids, arm_names = robot.find_joints(list(ROBOT_PRIM_CONTRACT.arm_joints), preserve_order=True)
        gripper_ids, gripper_names = robot.find_joints(list(ROBOT_PRIM_CONTRACT.gripper_joints), preserve_order=True)
        flange_ids, flange_names = robot.find_bodies([ROBOT_PRIM_CONTRACT.flange_body], preserve_order=True)

        if tuple(arm_names) != ROBOT_PRIM_CONTRACT.arm_joints:
            raise RuntimeError(f"{entity_name}: unexpected arm joints: {arm_names}")
        if tuple(gripper_names) != ROBOT_PRIM_CONTRACT.gripper_joints:
            raise RuntimeError(f"{entity_name}: unexpected gripper joints: {gripper_names}")
        if tuple(flange_names) != (ROBOT_PRIM_CONTRACT.flange_body,):
            raise RuntimeError(f"{entity_name}: unexpected flange body: {flange_names}")
        if set(arm_ids).intersection(gripper_ids):
            raise RuntimeError(f"{entity_name}: arm and gripper joint indices overlap")

        flange_id = flange_ids[0]
        flange_state = torch.cat(
            (
                robot.data.body_pos_w[:, flange_id],
                robot.data.body_quat_w[:, flange_id],
                robot.data.body_lin_vel_w[:, flange_id],
                robot.data.body_ang_vel_w[:, flange_id],
            ),
            dim=-1,
        )
        if not torch.isfinite(flange_state).all():
            raise RuntimeError(f"{entity_name}: flange state contains NaN or Inf")

        jacobians = robot.root_physx_view.get_jacobians()
        if jacobians.ndim != 4 or jacobians.shape[0] != 1:
            raise RuntimeError(f"{entity_name}: unexpected Jacobian shape: {tuple(jacobians.shape)}")
        print(
            f"{entity_name}: arm={list(arm_names)}, gripper={list(gripper_names)}, "
            f"flange_body_id={flange_id}, jacobian_shape={tuple(jacobians.shape)}",
            flush=True,
        )

    sim.clear_instance()
    print("Migrated static scene smoke test passed.", flush=True)
    return 0


if __name__ == "__main__":
    try:
        exit_code = main()
    except BaseException:
        traceback.print_exc()
        raise
    finally:
        simulation_app.close()
    raise SystemExit(exit_code)
