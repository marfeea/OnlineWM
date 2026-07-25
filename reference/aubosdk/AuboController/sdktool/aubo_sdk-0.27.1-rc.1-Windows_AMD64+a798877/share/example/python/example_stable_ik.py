#! /usr/bin/env python
# coding=utf-8

"""
功能：解决 TCP 位姿的欧拉角接近极限位置时，直接逆解后 moveJoint
      容易选择到另一组等价姿态逆解而导致关节翻转的问题。

      示例包含运动对照逻辑：
      1. unsafe 路径: 对同一个目标 TCP 位姿，给 inverseKinematics
         一个错误分支上的 qnear，使普通逆解选到翻转分支并执行 moveJoint；
      2. safe 路径: 使用 inverseKinematics2(reference_q, target_pose)
         按 reference_q 的构型计算逆解，并选择同构型内关节变化最小的
         一组，最后使用 moveJoint 运动。

步骤:
第一步: 设置 RPC 超时、连接 RPC 服务、机械臂登录
第二步: 获取当前关节角和当前 TCP 位姿
第三步: 对目标 TCP 位姿使用错误 qnear 调普通 inverseKinematics，
        先故意选择翻转解运动
第四步: 回到起点后，再使用 inverseKinematics2 计算稳定逆解并运动
第五步: RPC 退出登录、断开连接
"""

import math
import time

import pyaubo_sdk


robot_ip = "127.0.0.1"  # 机械臂 IP 地址
robot_port = 30004  # 端口号
DEMO_MOVE_SPEED = 20 * (math.pi / 180)
DEMO_MOVE_ACCELERATION = 20 * (math.pi / 180)
RUN_UNSAFE_DEMO = False
MIN_FLIP_DISTANCE_DEG = 60.0
MAX_JOINT_ABS_DEG = 350.0


def wait_done(robot_interface):
    max_retry_count = 5
    cnt = 0

    # 接口调用: 获取当前的运动指令 ID
    exec_id = robot_interface.getMotionControl().getExecId()

    # 等待机械臂开始运动
    while exec_id == -1:
        if cnt > max_retry_count:
            return -1
        time.sleep(0.05)
        cnt += 1
        exec_id = robot_interface.getMotionControl().getExecId()

    # 等待机械臂运动完成
    while robot_interface.getMotionControl().getExecId() != -1:
        time.sleep(0.05)

    return 0


def joint_gap(q1, q2):
    return sum((a - b) * (a - b) for a, b in zip(q1, q2))


def joint_gap_deg(q1, q2):
    return math.sqrt(joint_gap(q1, q2)) * 180 / math.pi


def within_demo_limit(q):
    return all(abs(v * 180 / math.pi) <= MAX_JOINT_ABS_DEG for v in q)


def show_vec(name, values):
    print(name + ": [" + ", ".join("{:.6f}".format(v) for v in values) + "]")


def show_joint(name, q):
    print(name + ": [" + ", ".join("{:.2f}".format(v * 180 / math.pi)
                                    for v in q) + "] deg")


def all_ik(robot_algorithm, target_pose):
    ik_result = robot_algorithm.inverseKinematicsAll(target_pose)
    ik_solutions = ik_result[0]
    ik_ret = ik_result[1]

    if ik_ret != 0 or len(ik_solutions) == 0:
        print("inverseKinematicsAll 失败，返回值:", ik_ret)
        return []

    return ik_solutions


def config_of(robot_algorithm, q):
    config_result = robot_algorithm.getRobotConfiguration(q)
    config = config_result[0]
    config_ret = config_result[1]

    if config_ret != 0:
        return None

    return config


def show_all_ik(robot_algorithm, target_pose, reference_q):
    ik_solutions = all_ik(robot_algorithm, target_pose)
    print("\ninverseKinematicsAll 全部逆解，共 {} 组".format(
        len(ik_solutions)))
    for index, q in enumerate(ik_solutions):
        config = config_of(robot_algorithm, q)
        config_text = config if config is not None else "获取失败"
        print("  解 {}，构型: {}，关节变化 {:.3f} deg".format(
            index + 1, config_text, joint_gap_deg(q, reference_q)))
        show_joint("    关节角", q)


def movej(robot_interface, q, title):
    print(title)
    ret = robot_interface.getMotionControl().moveJoint(
        q, DEMO_MOVE_ACCELERATION, DEMO_MOVE_SPEED, 0, 0)
    if ret != 0:
        print("moveJoint 下发失败，返回值:", ret)
        return False

    ret = wait_done(robot_interface)
    if ret != 0:
        print(title, "失败")
        return False

    print(title, "成功")
    return True


def bad_seed(robot_algorithm, target_pose, reference_q, reference_config):
    """
    为 unsafe 对照构造一个错误分支上的 qnear。

    为了稳定展示翻转，优先选择与起点不同构型、角度不超出演示限制、
    且关节变化足够明显的逆解；如果有多组候选，则选择变化最小的一组，
    避免挑到本来就不可执行的极端解。这里只用 inverseKinematicsAll
    找到错误 qnear，真正用于 unsafe 运动的目标关节角仍由普通
    inverseKinematics(qnear, pose) 产生。
    """
    ik_solutions = all_ik(robot_algorithm, target_pose)
    if len(ik_solutions) == 0:
        return None

    different_config_solutions = []
    valid_solutions = []
    for q in ik_solutions:
        config = config_of(robot_algorithm, q)
        if config is None:
            continue

        if not within_demo_limit(q):
            continue

        valid_solutions.append(q)
        if config != reference_config:
            different_config_solutions.append(q)

    if len(valid_solutions) == 0:
        return None

    candidates = different_config_solutions or valid_solutions
    candidates = [(q, joint_gap_deg(q, reference_q)) for q in candidates]
    candidates = [item for item in candidates
                  if item[1] >= MIN_FLIP_DISTANCE_DEG]
    if len(candidates) == 0:
        print("未找到角度合法且足够明显的错误 qnear")
        return None

    bad_qnear, _ = min(candidates, key=lambda item: item[1])
    return bad_qnear


def unsafe_ik(robot_algorithm, target_pose, reference_q, reference_config):
    bad_qnear = bad_seed(robot_algorithm, target_pose, reference_q,
                         reference_config)
    if bad_qnear is None:
        return None

    ik_result = robot_algorithm.inverseKinematics(bad_qnear, target_pose)
    unsafe_q = ik_result[0]
    ik_ret = ik_result[1]

    if ik_ret != 0:
        print("unsafe inverseKinematics 失败，返回值:", ik_ret)
        return None

    if not within_demo_limit(unsafe_q):
        print("unsafe 目标关节角超出演示限制，跳过该翻转解")
        return None

    unsafe_distance = joint_gap_deg(unsafe_q, reference_q)
    if unsafe_distance < MIN_FLIP_DISTANCE_DEG:
        print("普通 inverseKinematics 未选到明显翻转解，关节距离仅 {:.3f} deg".format(
            unsafe_distance))
        return None

    return unsafe_q


def stable_ik(robot_algorithm, target_pose, reference_q):
    """
    面向 moveJoint 的防翻转逆解选解策略。

    欧拉角在极限位置附近可能出现多种等价姿态表示。如果只用单个
    inverseKinematics 或随便取 inverseKinematicsAll 的某个解，机械臂
    可能切到另一组构型，表现为 moveJoint 过程中某些关节突然大幅翻转。

    选解步骤:
    1. 将 reference_q 作为参考关节角传入 inverseKinematics2；
    2. inverseKinematics2 按 reference_q 的构型计算逆解；
    3. inverseKinematics2 在同构型逆解中选择相对 reference_q
       关节变化最小的一组。
    """
    ik_result = robot_algorithm.inverseKinematics2(reference_q, target_pose)
    q = ik_result[0]
    ik_ret = ik_result[1]
    if ik_ret != 0:
        print("inverseKinematics2 失败，返回值:", ik_ret)
        return None

    if len(q) != len(reference_q):
        print("inverseKinematics2 返回关节数量异常: {}，期望: {}".format(
            len(q), len(reference_q)))
        return None

    print("stable_ik 选解结果")
    print("  inverseKinematics2 已按参考构型选择同构型最近解")
    print("  关节变化 {:.3f} deg".format(joint_gap_deg(q, reference_q)))
    return q


def demo(rpc_client):
    # 接口调用: 获取机器人的名字
    robot_name = rpc_client.getRobotNames()[0]
    robot_interface = rpc_client.getRobotInterface(robot_name)
    robot_algorithm = robot_interface.getRobotAlgorithm()
    motion_control = robot_interface.getMotionControl()

    # 接口调用: 设置机械臂的速度比率
    motion_control.setSpeedFraction(0.3)

    # 获取当前关节角，并以当前构型作为后续选解约束。
    reference_q = robot_interface.getRobotState().getJointPositions()
    current_tcp_pose = robot_interface.getRobotState().getTcpPose()
    show_joint("当前关节角", reference_q)
    reference_config_result = robot_algorithm.getRobotConfiguration(reference_q)
    reference_config = reference_config_result[0]
    reference_config_ret = reference_config_result[1]
    if reference_config_ret != 0:
        print("获取参考点构型失败，返回值:", reference_config_ret)
        return
    print("参考点构型:", reference_config)

    # 目标 TCP 位姿，单位: m, rad
    # 这里从当前 TCP 位姿做较明显的位置偏移。两段演示使用完全相同的目标 TCP，
    # 只改变选解策略，从而对比“翻转”和“防翻转”的运动差异。
    target_tcp_pose = current_tcp_pose[:]
    target_tcp_pose[0] += 0.060
    target_tcp_pose[1] += 0.040
    target_tcp_pose[2] += 0.030
    show_vec("目标 TCP 位姿", target_tcp_pose)
    # 调试全部逆解时取消下一行注释。
    # show_all_ik(robot_algorithm, target_tcp_pose, reference_q)

    if RUN_UNSAFE_DEMO:
        current_config_result = robot_algorithm.getRobotConfiguration(
            reference_q)
        current_config = current_config_result[0]
        current_config_ret = current_config_result[1]

        if current_config_ret != 0:
            print("获取当前构型失败，返回值:", current_config_ret)
            return

        print("当前机械臂构型:", current_config)

        print("\n1. unsafe 运动对照: 错误 qnear + 普通 inverseKinematics")
        flip_q = unsafe_ik(robot_algorithm, target_tcp_pose, reference_q,
                           current_config)
        if flip_q is None:
            print("未执行 unsafe 运动对照")
            return

        show_joint("unsafe 目标关节角", flip_q)
        if not movej(robot_interface, flip_q, "unsafe moveJoint 翻转运动"):
            return

        print("\n回到演示起点")
        if not movej(robot_interface, reference_q, "moveJoint 回到起点"):
            return

    print("\nsafe 运动示例: inverseKinematics2 同构型最近解，防止翻转")
    safe_q = stable_ik(robot_algorithm, target_tcp_pose, reference_q)
    if safe_q is None:
        print("未执行 safe 运动示例")
        return

    show_joint("safe 目标关节角", safe_q)
    target_config_result = robot_algorithm.getRobotConfiguration(safe_q)
    target_config = target_config_result[0]
    target_config_ret = target_config_result[1]
    if target_config_ret != 0:
        print("获取目标点构型失败，返回值:", target_config_ret)
        return
    print("目标点构型:", target_config)
    movej(robot_interface, safe_q, "safe moveJoint 防翻转运动")


if __name__ == '__main__':
    robot_rpc_client = pyaubo_sdk.RpcClient()
    robot_rpc_client.setRequestTimeout(1000)  # 接口调用: 设置 RPC 超时
    robot_rpc_client.connect(robot_ip, robot_port)  # 接口调用: 连接 RPC 服务
    if robot_rpc_client.hasConnected():
        print("RPC客户端连接成功!")
        robot_rpc_client.login("aubo", "123456")  # 接口调用: 登录
        if robot_rpc_client.hasLogined():
            print("RPC客户端登录成功!")
            demo(robot_rpc_client)
            robot_rpc_client.logout()  # 退出登录
            robot_rpc_client.disconnect()  # 断开连接
