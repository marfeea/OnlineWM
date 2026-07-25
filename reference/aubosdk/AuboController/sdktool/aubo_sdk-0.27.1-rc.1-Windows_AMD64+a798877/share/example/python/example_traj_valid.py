#! /usr/bin/env python
# coding=utf-8

"""
validatePath1 碰撞体示例

本示例演示五种路径校验场景：
1. 不添加任何碰撞体，运动到环境碰撞目标点，预期通过。
2. 仅添加环境碰撞平面，运动到环境碰撞目标点，预期碰撞失败。
3. 仅添加环境碰撞平面，运动到工具碰撞目标点，预期通过。
4. 添加环境碰撞平面和末端碰撞体，运动到工具碰撞目标点，预期碰撞失败。
5. 仅添加末端碰撞体，不添加环境碰撞平面，运动到工具碰撞目标点，预期通过。
"""

import pyaubo_sdk


LOCAL_IP = "127.0.0.1"
RPC_PORT = 30004


def print_validate_path_result(case_name, ret, expect_success):
    print(
        "{} validatePath1 返回值: {}, 预期: {}, 实际: {}".format(
            case_name,
            ret,
            "通过" if expect_success else "碰撞失败",
            "通过" if ret == 0 else "失败",
        )
    )


def example_trajectory_valid3(
    rpc_cli, start, target_env_collision, target_tool_collision
):
    robot_name = rpc_cli.getRobotNames()[0]
    robot_interface = rpc_cli.getRobotInterface(robot_name)
    algorithm = robot_interface.getRobotAlgorithm()

    # 接口调用: 设置 TCP 偏移
    robot_interface.getRobotConfig().setTcpOffset([0.0, 0.0, 0.0, 0.0, 0.0, 0.0])

    # 清理可能残留的同名碰撞体，避免重复添加影响示例结果。
    algorithm.removeCollisionObject("tool_box_demo")
    algorithm.removeCollisionObject("collision_plane_demo")

    pose_path_type = 1
    blend_radius = 0.0
    step = 0.05

    print("开始 validatePath1 碰撞体示例")
    print("返回值说明: 0 表示通过，非 0 表示失败；-22 通常表示路径碰撞")

    def add_collision_plane():
        return algorithm.addCollisionBox(
            "collision_plane_demo",
            "world",
            [[0.12, 0.12, 0.10]],
            [[start[0], start[1], -0.05, 0.0, 0.0, 0.0]],
        )

    def add_tool_box():
        return algorithm.addCollisionBox(
            "tool_box_demo",
            "end_effector",
            [[0.08, 0.08, 0.08]],
            [[0.0, 0.0, 0.04, 0.0, 0.0, 0.0]],
        )

    # case0: 不添加任何碰撞体，向下运动到环境碰撞目标点，作为基线。
    case0_ret = algorithm.validatePath1(
        pose_path_type,
        start,
        blend_radius,
        target_env_collision,
        blend_radius,
        step,
        True,
    )
    print_validate_path_result("[case0 无碰撞体]", case0_ret, True)

    # 添加一个较小的环境碰撞平面，并放到起点正下方附近，
    # 避免尺寸过大时仍然与底座区域重叠。
    ret = add_collision_plane()
    if ret != 0:
        print("添加环境碰撞平面失败, ret = {}".format(ret))
        return False

    # case1: 仅添加环境碰撞平面，向下运动到环境碰撞目标点，预期碰撞失败。
    case1_ret = algorithm.validatePath1(
        pose_path_type,
        start,
        blend_radius,
        target_env_collision,
        blend_radius,
        step,
        True,
    )
    print_validate_path_result("[case1 仅环境碰撞平面]", case1_ret, False)

    # case2: 仅添加环境碰撞平面，向下运动到工具碰撞目标点，预期仍可通过。
    case2_ret = algorithm.validatePath1(
        pose_path_type,
        start,
        blend_radius,
        target_tool_collision,
        blend_radius,
        step,
        True,
    )
    print_validate_path_result("[case2 仅环境碰撞平面]", case2_ret, True)

    # 在末端挂一个立方体，用于演示工具与环境的碰撞检测。
    ret = add_tool_box()
    if ret != 0:
        print("添加末端立方体失败, ret = {}".format(ret))
        algorithm.removeCollisionObject("collision_plane_demo")
        return False

    # case3: 添加环境碰撞平面和末端碰撞体，运动到同一个工具碰撞目标点，
    # 预期末端碰撞体先碰到环境碰撞平面。
    case3_ret = algorithm.validatePath1(
        pose_path_type,
        start,
        blend_radius,
        target_tool_collision,
        blend_radius,
        step,
        True,
    )
    print_validate_path_result(
        "[case3 环境碰撞平面+末端碰撞体]", case3_ret, False
    )

    algorithm.removeCollisionObject("collision_plane_demo")

    # case4: 移除环境碰撞平面，仅保留末端碰撞体，运动到工具碰撞目标点，
    # 预期仍可通过。
    case4_ret = algorithm.validatePath1(
        pose_path_type,
        start,
        blend_radius,
        target_tool_collision,
        blend_radius,
        step,
        True,
    )
    print_validate_path_result("[case4 仅末端碰撞体]", case4_ret, True)

    algorithm.removeCollisionObject("tool_box_demo")

    return (
        case0_ret == 0
        and case1_ret != 0
        and case2_ret == 0
        and case3_ret != 0
        and case4_ret == 0
    )


if __name__ == "__main__":
    rpc_cli = pyaubo_sdk.RpcClient()
    rpc_cli.setRequestTimeout(1000)
    rpc_cli.connect(LOCAL_IP, RPC_PORT)
    rpc_cli.login("aubo", "123456")

    # 使用示教器读到的位姿作为起点，分别演示五种碰撞校验场景。
    # env_collision_drop: 下压量超过 start_z，末端 TCP 本体会碰到环境碰撞平面。
    # tool_collision_drop: 下压量小于 start_z，仅添加末端碰撞体时才会碰撞。
    pose_start = [1.27114, -0.24923, 0.80691, -3.135, 0.004, 1.569]
    start_z = pose_start[2]
    env_collision_drop = start_z + 0.02
    tool_collision_drop = start_z - 0.06

    pose_env_collision = [
        1.27114,
        -0.24923,
        start_z - env_collision_drop,
        -3.135,
        0.004,
        1.569,
    ]
    pose_tool_collision = [
        1.27114,
        -0.24923,
        start_z - tool_collision_drop,
        -3.135,
        0.004,
        1.569,
    ]

    example_trajectory_valid3(
        rpc_cli, pose_start, pose_env_collision, pose_tool_collision
    )

    rpc_cli.logout()
    rpc_cli.disconnect()
