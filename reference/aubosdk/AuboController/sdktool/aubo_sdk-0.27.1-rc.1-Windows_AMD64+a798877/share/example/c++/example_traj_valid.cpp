#include "aubo_sdk/rpc.h"
#include "math.h"
#include <chrono>
#include <thread>
#ifdef WIN32
#include <Windows.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace arcs::common_interface;
using namespace arcs::aubo_sdk;

bool isJointNaN(const std::vector<double> &q, int dof)
{
    for (int i = 0; i < dof; i++) {
        if (std::isnan(q[i])) {
            return true;
        }
    }
    return false;
}

bool isJointInRange(const std::vector<double> &q,
                    const std::vector<double> &upper,
                    const ::std::vector<double> &lower, int dof)
{
    for (int i = 0; i < dof; i++) {
        if (q[i] < (lower[i] - 1e-6) || q[i] > (upper[i] + 1e-6)) {
            return false;
        }
    }
    return true;
}

bool checkJoint(RpcClientPtr impl, const std::vector<double> &q)
{
    auto robot_name = impl->getRobotNames().front();
    auto interface = impl->getRobotInterface(robot_name);
    auto dof = interface->getRobotConfig()->getDof();
    auto upper = interface->getRobotConfig()->getJointMaxPositions();
    auto lower = interface->getRobotConfig()->getJointMinPositions();
    if ((int)q.size() != dof || isJointNaN(q, dof)) {
        return false;
    }

    // 检查关节是否超限
    if (!isJointInRange(q, upper, lower, dof)) {
        return false;
    }
    return true;
}

// 检查给定的轨迹是否有效，进行插值和逆解验证
bool exampleTrajectoryValid(RpcClientPtr impl, const std::vector<double> &p1,
                            const std::vector<double> &p2, int num_points)
{
    // 如果num_points小于2，则无法进行插值
    if (num_points < 2) {
        throw std::invalid_argument("num_points must be at least 2");
    }

    // 接口调用: 获取机器人的名字
    auto robot_name = impl->getRobotNames().front();

    // 接口调用: 设置 tcp 偏移
    std::vector<double> offset = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    impl->getRobotInterface(robot_name)->getRobotConfig()->setTcpOffset(offset);

    // 计算每个插值点的alpha值，并调用interpolatePose
    for (int i = 0; i < num_points; ++i) {
        double alpha =
            static_cast<double>(i) / (num_points - 1); // alpha从0到1均匀变化

        // 接口调用: 计算线性差值
        auto pose = impl->getMath()->interpolatePose(p1, p2, alpha);

        // 接口调用: 根据计算出的差值位姿，检查是否能找到有效的逆解
        auto result = impl->getRobotInterface(robot_name)
                          ->getRobotAlgorithm()
                          ->inverseKinematicsAll(pose);

        if (std::get<1>(result) != 0) {
            std::cout << "逆解失败, inverseKinematicsAll返回值:"
                      << std::get<1>(result) << std::endl;
            std::cout << "轨迹规划失败" << std::endl;
            return false;
        }
        auto qs = std::get<0>(result);
        for (size_t i = 0; i < qs.size(); ++i) {
            if (!checkJoint(impl, qs[i])) {
                std::cout << "轨迹规划失败" << std::endl;
                return false;
            }
        }
    }

    // 如果所有插值点均有效，表示轨迹规划成功
    std::cout << "轨迹规划成功" << std::endl;
    return true;
}

/**
 * 输入两个关节角，检测规划路径是否有效
 * @brief exampleTrajectoryValid3
 * @param impl
 * @param p1 位姿1
 * @param blend1 交融半径1
 * @param p2 位姿2
 * @param blend2 交融半径2
 * @param qnear
 * @return
 */
bool exampleTrajectoryValid2(RpcClientPtr impl, const std::vector<double> &q1,
                             double blend1, const std::vector<double> &q2,
                             double blend2)
{ // 接口调用: 获取机器人的名字
    auto robot_name = impl->getRobotNames().front();
    auto interface = impl->getRobotInterface(robot_name);
    // 接口调用: 设置 tcp 偏移
    std::vector<double> offset = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    interface->getRobotConfig()->setTcpOffset(offset);

    auto traj =
        interface->getRobotAlgorithm()->pathMovej(q1, blend1, q2, blend2, 0.05);

    for (size_t i = 0; i < traj.size(); ++i) {
        if (!checkJoint(impl, traj[i])) {
            std::cout << "轨迹规划失败" << std::endl;
            return false;
        }
    }
    std::cout << "轨迹规划成功" << std::endl;
    return true;
}

void printValidatePathResult(const std::string &case_name, int ret,
                             bool expect_success)
{
    std::cout << case_name << " validatePath1 返回值: " << ret
              << ", 预期: " << (expect_success ? "通过" : "碰撞失败")
              << ", 实际: " << (ret == 0 ? "通过" : "失败") << std::endl;
}

/**
 * 演示 validatePath1 在不同碰撞体配置下的校验结果。
 *
 * 说明：
 * 1. case0: 不添加任何碰撞体，运动到 target_env_collision，预期通过。
 * 2. case1: 仅添加环境碰撞平面，运动到 target_env_collision，预期碰撞失败。
 * 3. case2: 仅添加环境碰撞平面，运动到 target_tool_collision，预期通过。
 * 4. case3: 添加环境碰撞平面和末端碰撞体，运动到 target_tool_collision，
 *    预期碰撞失败。
 * 5. case4: 仅添加末端碰撞体，不添加环境碰撞平面，运动到
 * target_tool_collision， 预期通过。
 */
bool exampleTrajectoryValid3(RpcClientPtr impl,
                             const std::vector<double> &start,
                             const std::vector<double> &target_env_collision,
                             const std::vector<double> &target_tool_collision)
{
    auto robot_name = impl->getRobotNames().front();
    auto interface = impl->getRobotInterface(robot_name);
    auto algorithm = interface->getRobotAlgorithm();

    std::vector<double> offset = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    interface->getRobotConfig()->setTcpOffset(offset);

    // 清理可能残留的同名碰撞体，避免重复添加影响示例结果。
    algorithm->removeCollisionObject("tool_box_demo");
    algorithm->removeCollisionObject("collision_plane_demo");

    const int pose_path_type = 1;
    const double blend_radius = 0.0;
    const double step = 0.05;

    std::cout << "开始 validatePath1 碰撞体示例" << std::endl;
    std::cout << "返回值说明: 0 表示通过，非 0 表示失败；-22 通常表示路径碰撞"
              << std::endl;

    // case0: 不添加任何碰撞体，向下运动到环境碰撞目标点，作为基线。
    int case0_ret = algorithm->validatePath1(pose_path_type, start,
                                             blend_radius, target_env_collision,
                                             blend_radius, step, true);
    printValidatePathResult("[case0 无碰撞体]", case0_ret, true);

    auto add_collision_plane = [&]() {
        return algorithm->addCollisionBox(
            "collision_plane_demo", "world", { { 0.12, 0.12, 0.10 } },
            { { start[0], start[1], -0.05, 0.0, 0.0, 0.0 } });
    };

    auto add_tool_box = [&]() {
        return algorithm->addCollisionBox(
            "tool_box_demo", "end_effector", { { 0.08, 0.08, 0.08 } },
            { { 0.0, 0.0, 0.04, 0.0, 0.0, 0.0 } });
    };

    // 添加一个较小的环境碰撞平面，并放到起点正下方附近，
    // 避免尺寸过大时仍然与底座区域重叠。
    int ret = add_collision_plane();
    if (ret != 0) {
        std::cout << "添加环境碰撞平面失败, ret = " << ret << std::endl;
        return false;
    }

    // case1: 仅添加环境碰撞平面，向下运动到环境碰撞目标点，预期碰撞失败。
    int case1_ret = algorithm->validatePath1(pose_path_type, start,
                                             blend_radius, target_env_collision,
                                             blend_radius, step, true);
    printValidatePathResult("[case1 仅环境碰撞平面]", case1_ret, false);

    // case2: 仅添加环境碰撞平面，向下运动到工具碰撞目标点，预期仍可通过。
    int case2_ret = algorithm->validatePath1(
        pose_path_type, start, blend_radius, target_tool_collision,
        blend_radius, step, true);
    printValidatePathResult("[case2 仅环境碰撞平面]", case2_ret, true);

    // 在末端挂一个立方体，用于演示工具与环境的碰撞检测。
    ret = add_tool_box();
    if (ret != 0) {
        std::cout << "添加末端立方体失败, ret = " << ret << std::endl;
        algorithm->removeCollisionObject("collision_plane_demo");
        return false;
    }

    // case3: 添加环境碰撞平面和末端碰撞体，运动到同一个工具碰撞目标点，
    // 预期末端碰撞体先碰到环境碰撞平面。
    int case3_ret = algorithm->validatePath1(
        pose_path_type, start, blend_radius, target_tool_collision,
        blend_radius, step, true);
    printValidatePathResult("[case3 环境碰撞平面+末端碰撞体]", case3_ret,
                            false);

    algorithm->removeCollisionObject("collision_plane_demo");

    // case4: 移除环境碰撞平面，仅保留末端碰撞体，运动到工具碰撞目标点，
    // 预期仍可通过。
    int case4_ret = algorithm->validatePath1(
        pose_path_type, start, blend_radius, target_tool_collision,
        blend_radius, step, true);
    printValidatePathResult("[case4 仅末端碰撞体]", case4_ret, true);

    algorithm->removeCollisionObject("tool_box_demo");

    return case0_ret == 0 && case1_ret != 0 && case2_ret == 0 &&
           case3_ret != 0 && case4_ret == 0;
}

void printVector(const std::vector<double> &vec)
{
    std::cout << "[ ";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i != vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << " ]" << std::endl;
}

void exampleTrajectoryValid4(RpcClientPtr impl)
{
    auto robot_name = impl->getRobotNames().front();
    auto interface = impl->getRobotInterface(robot_name);
    auto algorithm = interface->getRobotAlgorithm();

    std::vector<double> offset = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    interface->getRobotConfig()->setTcpOffset(offset);

    // 角度转弧度常量
    const double PI = acos(-1.0);
    const double DEG2RAD = PI / 180.0;

    // 你的关节角数组（完全对应 Lua 代码）
    std::vector<std::vector<double>> joint_positions = {
        { 86.3200 * DEG2RAD, 20.0000 * DEG2RAD, -126.9900 * DEG2RAD,
          -40.0000 * DEG2RAD, -87.1600 * DEG2RAD, 0.0 * DEG2RAD },
        { 112.7723 * DEG2RAD, 53.3426 * DEG2RAD, -56.2858 * DEG2RAD,
          -19.5524 * DEG2RAD, -90.1026 * DEG2RAD, 22.835 * DEG2RAD },
        { 112.7723 * DEG2RAD, 53.3426 * DEG2RAD, -56.2858 * DEG2RAD,
          -19.5524 * DEG2RAD, -90.1026 * DEG2RAD, 22.835 * DEG2RAD },
        { 121.6682 * DEG2RAD, 29.9312 * DEG2RAD, -98.1711 * DEG2RAD,
          -38.0263 * DEG2RAD, -90.0785 * DEG2RAD, 31.7343 * DEG2RAD },
        { 121.9946 * DEG2RAD, 29.4157 * DEG2RAD, -99.0255 * DEG2RAD,
          -38.3652 * DEG2RAD, -90.0779 * DEG2RAD, 32.0608 * DEG2RAD },
        { 121.9946 * DEG2RAD, 29.4157 * DEG2RAD, -99.0255 * DEG2RAD,
          -38.3652 * DEG2RAD, -90.0779 * DEG2RAD, 32.0608 * DEG2RAD },
        { 90.0000 * DEG2RAD, -45.0000 * DEG2RAD, -150.0000 * DEG2RAD,
          -90.0000 * DEG2RAD, -90.0000 * DEG2RAD, 0.0 * DEG2RAD }
    };
    auto ret = interface->getRobotAlgorithm()->mergeDenseWaypoints(
        0, joint_positions, 0.005);
    if (std::get<1>(ret) == 0) {
        auto out = std::get<0>(ret);
        for (int i = 0; i < out.size(); ++i) {
            printVector(out[i]);
        }
    }
}

/**
 * 演示通过 mergeDenseWaypoints 判断相邻点是否过密，过密则忽略点位。
 */
bool exampleTrajectoryValid5(RpcClientPtr impl)
{
    auto robot_name = impl->getRobotNames().front();
    auto interface = impl->getRobotInterface(robot_name);
    auto algorithm = interface->getRobotAlgorithm();

    std::vector<double> offset = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    interface->getRobotConfig()->setTcpOffset(offset);

    const double PI = acos(-1.0);
    const double DEG2RAD = PI / 180.0;
    const int joint_path_type = 0;
    const double min_segment_length = 0.005;
    const double blend_radius = 0.005;
    const double max_acc = 80 * DEG2RAD;
    const double max_vel = 60 * DEG2RAD;

    std::vector<std::vector<double>> joint_positions = {
        { 86.3200 * DEG2RAD, 20.0000 * DEG2RAD, -126.9900 * DEG2RAD,
          -40.0000 * DEG2RAD, -87.1600 * DEG2RAD, 0.0 * DEG2RAD },
        { 112.7723 * DEG2RAD, 53.3426 * DEG2RAD, -56.2858 * DEG2RAD,
          -19.5524 * DEG2RAD, -90.1026 * DEG2RAD, 22.835 * DEG2RAD },
        { 112.7723 * DEG2RAD, 53.3426 * DEG2RAD, -56.2858 * DEG2RAD,
          -19.5524 * DEG2RAD, -90.1026 * DEG2RAD, 22.835 * DEG2RAD },
        { 121.6682 * DEG2RAD, 29.9312 * DEG2RAD, -98.1711 * DEG2RAD,
          -38.0263 * DEG2RAD, -90.0785 * DEG2RAD, 31.7343 * DEG2RAD },
        { 121.9946 * DEG2RAD, 29.4157 * DEG2RAD, -99.0255 * DEG2RAD,
          -38.3652 * DEG2RAD, -90.0779 * DEG2RAD, 32.0608 * DEG2RAD },
        { 121.9946 * DEG2RAD, 29.4157 * DEG2RAD, -99.0255 * DEG2RAD,
          -38.3652 * DEG2RAD, -90.0779 * DEG2RAD, 32.0608 * DEG2RAD },
        { 90.0000 * DEG2RAD, -45.0000 * DEG2RAD, -150.0000 * DEG2RAD,
          -90.0000 * DEG2RAD, -90.0000 * DEG2RAD, 0.0 * DEG2RAD }
    };

    // 从当前点开始比较。
    std::vector<double> last_executed =
        interface->getRobotState()->getJointPositions();

    // 开启运行时。
    auto runtime = impl->getRuntimeMachine();
    auto start_time = std::chrono::steady_clock::now();
    int task_id = -1;
    int start_ret = runtime->start();
    if (start_ret != AUBO_OK) {
        std::cerr << "开启运行时失败, 返回值: " << start_ret << std::endl;
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    while (task_id == -1) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_time)
                .count() > 500) {
            std::cerr << "等待运行时任务创建超时" << std::endl;
            runtime->abort();
            return false;
        }
        auto context_result = runtime->getPlanContext();
        task_id = std::get<0>(context_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    bool success = true;
    int line_number = 0;
    for (size_t i = 0; i < joint_positions.size(); ++i) {
        // 判断目标点是否过近。
        auto merge_ret = algorithm->mergeDenseWaypoints(
            joint_path_type, { last_executed, joint_positions[i] },
            min_segment_length);
        if (std::get<1>(merge_ret) != AUBO_OK) {
            std::cout << "[失败] 点位[" << i
                      << "] mergeDenseWaypoints 返回值: "
                      << std::get<1>(merge_ret) << std::endl;
            std::cout << "  点位值: ";
            printVector(joint_positions[i]);
            success = false;
            break;
        }

        if (std::get<0>(merge_ret).size() < 2) {
            std::cout << "[跳过] 点位[" << i
                      << "], 判定过近, 未下发 moveJoint" << std::endl;
            std::cout << "  点位值: ";
            printVector(joint_positions[i]);
            continue;
        }

        const double current_blend_radius =
            (line_number == 0 || i + 1 == joint_positions.size())
                ? 0.0
                : blend_radius;

        // 带交融执行点位。
        std::cout << "[执行] 点位[" << i << "]" << std::endl;
        std::cout << "  点位值: ";
        printVector(joint_positions[i]);
        int current_line = line_number + 1;
        runtime->setLabel(current_line, "moveJoint");
        int move_ret = interface->getMotionControl()->moveJoint(
            joint_positions[i], max_acc, max_vel, current_blend_radius, 0);
        if (move_ret != AUBO_OK) {
            std::cout << "[失败] 点位[" << i
                      << "]失败, moveJoint 返回值: " << move_ret << std::endl;
            std::cout << "  点位值: ";
            printVector(joint_positions[i]);
            success = false;
            break;
        }
        line_number = current_line;
        last_executed = joint_positions[i];
    }

    // 等待最后一个执行点到位。
    while (success && line_number > 0) {
        if (runtime->getRuntimeState() != RuntimeState::Running) {
            break;
        }
        auto context_result = runtime->getPlanContext();
        int current_line = std::get<1>(context_result);
        if (current_line >= line_number &&
            interface->getMotionControl()->getExecId() == -1) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // 终止运行时。
    runtime->abort();
    return success;
}

#define LOCAL_IP "127.0.0.1"

int main(int argc, char **argv)
{
#ifdef WIN32
    // 将Windows控制台输出代码页设置为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
    auto rpc_cli = std::make_shared<RpcClient>();
    // 接口调用: 设置 RPC 超时, 单位: ms
    rpc_cli->setRequestTimeout(1000);
    // 接口调用: 连接到 RPC 服务
    rpc_cli->connect(LOCAL_IP, 30004);
    // 接口调用: 登录
    rpc_cli->login("aubo", "123456");

    // 起始位姿和目标位姿
    std::vector<double> pose1 = { 0.551, -0.295, 0.261, -3.135, 0.0, 1.569 };
    std::vector<double> pose2 = { 0.551, 0.295, 0.261, -3.135, 0.0, 0 };

    // 插值点数量
    int num_points = 30;

    // 检查给定的轨迹是否有效，进行插值和逆解验证
    // exampleTrajectoryValid(rpc_cli, pose1, pose2, num_points);

    // exampleTrajectoryValid3
    // 使用示教器读到的位姿作为起点，分别演示五种碰撞校验场景。
    // env_collision_drop: 下压量超过 start_z，末端 TCP 本体会碰到环境碰撞平面。
    // tool_collision_drop: 下压量小于 start_z，仅添加末端碰撞体时才会碰撞。

    //    std::vector<double> pose_start = { 1.27114, -0.24923, 0.80691,
    //                                       -3.135,  0.004,    1.569 };
    //    const double start_z = pose_start[2];
    //    const double env_collision_drop = start_z + 0.02;
    //    const double tool_collision_drop = start_z - 0.06;
    //    std::vector<double> pose_env_collision = {
    //        1.27114, -0.24923, start_z - env_collision_drop, -3.135,
    //        0.004, 1.569
    //    };
    //    std::vector<double> pose_tool_collision = {
    //        1.27114, -0.24923, start_z - tool_collision_drop, -3.135,
    //        0.004, 1.569
    //    };

    //    exampleTrajectoryValid3(rpc_cli, pose_start, pose_env_collision,
    //                            pose_tool_collision);

    // exampleTrajectoryValid4(rpc_cli);
    exampleTrajectoryValid5(rpc_cli);

    // exampleTrajectoryValid2
    // 关节角，单位: 弧度
    //    std::vector<double> q1 = {
    //        0.0 * (M_PI / 180),  -15.0 * (M_PI / 180), 100.0 * (M_PI / 180),
    //        25.0 * (M_PI / 180), 90.0 * (M_PI / 180),  0.0 * (M_PI / 180)
    //    };

    //    std::vector<double> q2 = {
    //        35.92 * (M_PI / 180),  -11.28 * (M_PI / 180), 59.96 * (M_PI /
    //        180), -18.76 * (M_PI / 180), 90.0 * (M_PI / 180),   35.92 * (M_PI
    //        / 180)
    //    };
    // exampleTrajectoryValid2(rpc_cli, q1, 0, q2, 0);
    // 接口调用: 退出登录
    rpc_cli->logout();
    // 接口调用: 断开连接
    rpc_cli->disconnect();

    return 0;
}
