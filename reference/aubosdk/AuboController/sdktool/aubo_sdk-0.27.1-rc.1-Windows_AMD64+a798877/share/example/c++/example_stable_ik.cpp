#include "aubo_sdk/rpc.h"

/*
 * 功能：解决 TCP 位姿的欧拉角接近极限位置时，直接逆解后 moveJoint
 *       容易选择到另一组等价姿态逆解而导致关节翻转的问题。
 *
 * 示例默认只执行 safe 路径：
 * 1. 获取当前关节角 reference_q，并以它作为逆解参考；
 * 2. 对目标 TCP 位姿调用 stableIk()；
 * 3. stableIk() 内部调用 inverseKinematics2(reference_q, pose)；
 * 4. inverseKinematics2 按 reference_q 的构型计算逆解，并选择同构型
 *    内关节变化最小的一组；
 * 5. 使用选中的关节角执行 moveJoint。
 *
 * 如果需要观察翻转对照，可将 RUN_UNSAFE_DEMO 改为 true。
 */

#include <cmath>
#include <iomanip>
#include <limits>
#include <optional>
#include <tuple>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

using namespace arcs::common_interface;
using namespace arcs::aubo_sdk;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

constexpr double DEMO_MOVE_SPEED = 20 * (M_PI / 180);
constexpr double DEMO_MOVE_ACCELERATION = 20 * (M_PI / 180);
constexpr bool RUN_UNSAFE_DEMO = false;
constexpr double MIN_FLIP_DISTANCE_DEG = 60.0;
constexpr double MAX_JOINT_ABS_DEG = 350.0;

// 实现阻塞功能: 当机械臂运动到目标路点时，程序再往下执行
int waitDone(RobotInterfacePtr robot)
{
    const int max_retry_count = 5;
    int cnt = 0;

    int exec_id = robot->getMotionControl()->getExecId();

    while (exec_id == -1) {
        if (cnt++ > max_retry_count) {
            return -1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        exec_id = robot->getMotionControl()->getExecId();
    }

    while (robot->getMotionControl()->getExecId() != -1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}

double jointGap(const std::vector<double> &q1, const std::vector<double> &q2)
{
    double gap = 0;
    for (size_t i = 0; i < q1.size() && i < q2.size(); i++) {
        const double diff = q1.at(i) - q2.at(i);
        gap += diff * diff;
    }
    return gap;
}

double jointGapDeg(const std::vector<double> &q1,
                   const std::vector<double> &q2)
{
    return std::sqrt(jointGap(q1, q2)) * 180 / M_PI;
}

bool inLimit(const std::vector<double> &q)
{
    for (auto value : q) {
        if (std::abs(value * 180 / M_PI) > MAX_JOINT_ABS_DEG) {
            return false;
        }
    }
    return true;
}

void showVec(const std::string &name, const std::vector<double> &values)
{
    std::cout << name << ": [";
    for (size_t i = 0; i < values.size(); i++) {
        std::cout << std::fixed << std::setprecision(6) << values.at(i);
        if (i + 1 != values.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

void showJoint(const std::string &name, const std::vector<double> &q)
{
    std::cout << name << ": [";
    for (size_t i = 0; i < q.size(); i++) {
        std::cout << std::fixed << std::setprecision(2)
                  << q.at(i) * 180 / M_PI;
        if (i + 1 != q.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "] deg" << std::endl;
}

template <typename RobotAlgorithmPtrT>
std::vector<std::vector<double>> allIk(RobotAlgorithmPtrT algorithm,
                                       const std::vector<double> &pose)
{
    auto result = algorithm->inverseKinematicsAll(pose);
    auto solutions = std::get<0>(result);
    auto ret = std::get<1>(result);

    if (ret != 0 || solutions.empty()) {
        std::cout << "inverseKinematicsAll 失败，返回值: " << ret << std::endl;
        return {};
    }

    return solutions;
}

template <typename RobotAlgorithmPtrT>
std::optional<int> configOf(RobotAlgorithmPtrT algorithm,
                            const std::vector<double> &q)
{
    auto result = algorithm->getRobotConfiguration(q);
    auto config = std::get<0>(result);
    auto ret = std::get<1>(result);

    if (ret != 0) {
        return std::nullopt;
    }

    return config;
}

template <typename RobotAlgorithmPtrT>
void showAllIk(RobotAlgorithmPtrT algorithm, const std::vector<double> &pose,
               const std::vector<double> &reference_q)
{
    auto solutions = allIk(algorithm, pose);
    std::cout << "\ninverseKinematicsAll 全部逆解，共 " << solutions.size()
              << " 组" << std::endl;
    for (size_t i = 0; i < solutions.size(); i++) {
        const auto &q = solutions.at(i);
        auto config = configOf(algorithm, q);
        std::cout << "  解 " << i + 1;
        if (config) {
            std::cout << "，构型: " << *config;
        } else {
            std::cout << "，构型: 获取失败";
        }
        std::cout << "，关节变化 " << std::fixed << std::setprecision(3)
                  << jointGapDeg(q, reference_q) << " deg" << std::endl;
        showJoint("    关节角", q);
    }
}

bool moveJ(RobotInterfacePtr robot, const std::vector<double> &q,
           const std::string &title)
{
    std::cout << title << std::endl;
    int ret = robot->getMotionControl()->moveJoint(
        q, DEMO_MOVE_ACCELERATION, DEMO_MOVE_SPEED, 0, 0);
    if (ret != 0) {
        std::cout << "moveJoint 下发失败，返回值: " << ret << std::endl;
        return false;
    }

    ret = waitDone(robot);
    if (ret != 0) {
        std::cout << title << "失败" << std::endl;
        return false;
    }

    std::cout << title << "成功" << std::endl;
    return true;
}

template <typename RobotAlgorithmPtrT>
std::optional<std::vector<double>> badSeed(
    RobotAlgorithmPtrT algorithm, const std::vector<double> &pose,
    const std::vector<double> &reference_q, int reference_config)
{
    // unsafe 对照专用：构造一个错误分支上的 qnear。
    // 这里只用于演示普通 inverseKinematics 被错误 qnear 引导后可能翻转；
    // 默认不会执行该路径。
    auto solutions = allIk(algorithm, pose);
    if (solutions.empty()) {
        return std::nullopt;
    }

    std::vector<std::vector<double>> different_config_solutions;
    std::vector<std::vector<double>> valid_solutions;
    for (const auto &q : solutions) {
        auto config = configOf(algorithm, q);
        if (!config || !inLimit(q)) {
            continue;
        }

        valid_solutions.push_back(q);
        if (*config != reference_config) {
            different_config_solutions.push_back(q);
        }
    }

    if (valid_solutions.empty()) {
        return std::nullopt;
    }

    const auto &raw_candidates = different_config_solutions.empty()
        ? valid_solutions
        : different_config_solutions;

    std::optional<std::vector<double>> best_q;
    double best_gap = std::numeric_limits<double>::max();
    for (const auto &q : raw_candidates) {
        const double gap = jointGapDeg(q, reference_q);
        if (gap >= MIN_FLIP_DISTANCE_DEG && gap < best_gap) {
            best_q = q;
            best_gap = gap;
        }
    }

    if (!best_q) {
        std::cout << "未找到角度合法且足够明显的错误 qnear" << std::endl;
    }

    return best_q;
}

template <typename RobotAlgorithmPtrT>
std::optional<std::vector<double>> unsafeIk(
    RobotAlgorithmPtrT algorithm, const std::vector<double> &pose,
    const std::vector<double> &reference_q, int reference_config)
{
    // unsafe 对照专用：最终运动目标仍由普通 inverseKinematics 生成。
    auto seed = badSeed(algorithm, pose, reference_q, reference_config);
    if (!seed) {
        return std::nullopt;
    }

    auto result = algorithm->inverseKinematics(*seed, pose);
    auto q = std::get<0>(result);
    auto ret = std::get<1>(result);

    if (ret != 0) {
        std::cout << "unsafe inverseKinematics 失败，返回值: " << ret
                  << std::endl;
        return std::nullopt;
    }

    if (!inLimit(q)) {
        std::cout << "unsafe 目标关节角超出演示限制，跳过该翻转解"
                  << std::endl;
        return std::nullopt;
    }

    const double gap = jointGapDeg(q, reference_q);
    if (gap < MIN_FLIP_DISTANCE_DEG) {
        std::cout << "普通 inverseKinematics 未选到明显翻转解，关节距离仅 "
                  << std::fixed << std::setprecision(3) << gap << " deg"
                  << std::endl;
        return std::nullopt;
    }

    return q;
}

template <typename RobotAlgorithmPtrT>
std::optional<std::vector<double>> stableIk(
    RobotAlgorithmPtrT algorithm, const std::vector<double> &pose,
    const std::vector<double> &reference_q)
{
    /*
     * 面向 moveJoint 的防翻转逆解选解策略。
     *
     * 这个函数是自包含的，用户可以直接复制使用，不依赖本文件中的
     * allIk/configOf/jointGap 等辅助函数。
     *
     * 参数：
     * - algorithm: robot->getRobotAlgorithm() 返回的对象
     * - pose: 目标 TCP 位姿，单位 m, rad
     * - reference_q: 参考关节角，通常传当前关节角或上一段运动的目标关节角
     *
     * 返回：
     * - 成功：同构型且相对 reference_q 关节变化最小的逆解
     * - 失败：std::nullopt
     *
     * 选解步骤：
     * 1. 将 reference_q 作为参考关节角传入 inverseKinematics2；
     * 2. inverseKinematics2 按 reference_q 的构型计算逆解；
     * 3. inverseKinematics2 在同构型逆解中选择相对 reference_q
     *    关节变化最小的一组。
     */
    auto ik_result = algorithm->inverseKinematics2(reference_q, pose);
    auto q = std::get<0>(ik_result);
    auto ik_ret = std::get<1>(ik_result);
    if (ik_ret != 0) {
        std::cout << "inverseKinematics2 失败，返回值: " << ik_ret
                  << std::endl;
        return std::nullopt;
    }
    if (q.size() != reference_q.size()) {
        std::cout << "inverseKinematics2 返回关节数量异常: " << q.size()
                  << "，期望: " << reference_q.size() << std::endl;
        return std::nullopt;
    }

    std::cout << "stable_ik 选解结果" << std::endl;
    std::cout << "  inverseKinematics2 已按参考构型选择同构型最近解"
              << std::endl;
    std::cout << "  关节变化 " << std::fixed << std::setprecision(3)
              << jointGapDeg(q, reference_q) << " deg"
              << std::endl;
    return q;
}

void demo(RpcClientPtr rpc_cli)
{
    // 接口调用: 获取机器人的名字
    auto robot_name = rpc_cli->getRobotNames().front();
    auto robot = rpc_cli->getRobotInterface(robot_name);
    auto algorithm = robot->getRobotAlgorithm();
    auto motion = robot->getMotionControl();

    // 接口调用: 设置机械臂的速度比率
    motion->setSpeedFraction(0.3);

    // 获取当前关节角和当前 TCP 位姿
    auto reference_q = robot->getRobotState()->getJointPositions();
    auto current_tcp_pose = robot->getRobotState()->getTcpPose();
    showJoint("当前关节角", reference_q);
    auto reference_config_result = algorithm->getRobotConfiguration(reference_q);
    auto reference_config = std::get<0>(reference_config_result);
    auto reference_config_ret = std::get<1>(reference_config_result);
    if (reference_config_ret != 0) {
        std::cout << "获取参考点构型失败，返回值: " << reference_config_ret
                  << std::endl;
        return;
    }
    std::cout << "参考点构型: " << reference_config << std::endl;

    // 目标 TCP 位姿，单位: m, rad
    // 这里从当前 TCP 位姿做较明显的位置偏移。safe 和 unsafe 对照
    // 使用完全相同的目标 TCP，只改变选解策略。
    auto target_tcp_pose = current_tcp_pose;
    target_tcp_pose.at(0) += 0.060;
    target_tcp_pose.at(1) += 0.040;
    target_tcp_pose.at(2) += 0.030;
    showVec("目标 TCP 位姿", target_tcp_pose);
    // 调试全部逆解时取消下一行注释。
    // showAllIk(algorithm, target_tcp_pose, reference_q);

    if (RUN_UNSAFE_DEMO) {
        auto config_result = algorithm->getRobotConfiguration(reference_q);
        auto current_config = std::get<0>(config_result);
        auto config_ret = std::get<1>(config_result);
        if (config_ret != 0) {
            std::cout << "获取当前构型失败，返回值: " << config_ret
                      << std::endl;
            return;
        }

        std::cout << "当前机械臂构型: " << current_config << std::endl;

        // unsafe 对照默认关闭。打开后会故意给 inverseKinematics 一个错误
        // qnear，用于观察普通逆解选到翻转分支时的运动效果。
        std::cout << "\n1. unsafe 运动对照: 错误 qnear + 普通 "
                     "inverseKinematics"
                  << std::endl;
        auto flip_q = unsafeIk(algorithm, target_tcp_pose, reference_q,
                               current_config);
        if (!flip_q) {
            std::cout << "未执行 unsafe 运动对照" << std::endl;
            return;
        }

        showJoint("unsafe 目标关节角", *flip_q);
        if (!moveJ(robot, *flip_q, "unsafe moveJoint 翻转运动")) {
            return;
        }

        std::cout << "\n回到演示起点" << std::endl;
        if (!moveJ(robot, reference_q, "moveJoint 回到起点")) {
            return;
        }
    }

    // safe 路径：使用 inverseKinematics2 选出同构型且关节变化最小的目标关节角
    std::cout << "\nsafe 运动示例: inverseKinematics2 同构型最近解，防止翻转"
              << std::endl;
    auto safe_q = stableIk(algorithm, target_tcp_pose, reference_q);
    if (!safe_q) {
        std::cout << "未执行 safe 运动示例" << std::endl;
        return;
    }

    showJoint("safe 目标关节角", *safe_q);
    auto target_config_result = algorithm->getRobotConfiguration(*safe_q);
    auto target_config = std::get<0>(target_config_result);
    auto target_config_ret = std::get<1>(target_config_result);
    if (target_config_ret != 0) {
        std::cout << "获取目标点构型失败，返回值: " << target_config_ret
                  << std::endl;
        return;
    }
    std::cout << "目标点构型: " << target_config << std::endl;
    moveJ(robot, *safe_q, "safe moveJoint 防翻转运动");
}

#define LOCAL_IP "127.0.0.1"

int main(int argc, char **argv)
{
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    auto rpc_cli = std::make_shared<RpcClient>();
    // 接口调用: 设置 RPC 超时
    rpc_cli->setRequestTimeout(1000);
    // 接口调用: 连接到 RPC 服务
    rpc_cli->connect(LOCAL_IP, 30004);
    // 接口调用: 登录
    rpc_cli->login("aubo", "123456");

    demo(rpc_cli);

    // 接口调用: 退出登录、断开连接
    rpc_cli->logout();
    rpc_cli->disconnect();

    return 0;
}
