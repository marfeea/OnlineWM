#include <math.h>
#include <iostream>

#include "research_interface/robot.h"
#include "trajectory_io.h"
#include <csignal>

static volatile std::sig_atomic_t g_stop = 0;

static void on_sigint(int)
{
    g_stop = 1;
}

template <typename T, std::size_t N>
inline std::ostream &operator<<(std::ostream &os, const std::array<T, N> &list)
{
    for (size_t i = 0; i < list.size(); i++) {
        os << list.at(i);
        if (i != (list.size() - 1)) {
            os << ",";
        }
    }
    return os;
}

template <typename T, std::size_t N>
inline auto operator*(const std::array<T, N> &l, T x)
{
    std::array<T, N> temp;
    for (size_t i = 0; i < l.size(); i++) {
        temp[i] = l[i] * x;
    }
    return temp;
}

using namespace arcs::research_interface;
int main(int argc, char *argv[])
{
    std::signal(SIGINT, on_sigint);

#ifdef SIGTERM
    // Linux
    std::signal(SIGTERM, on_sigint);
#endif

#ifdef SIGBREAK
    // MSVC
    std::signal(SIGBREAK, on_sigint);
#endif

    auto robot = Robot("127.0.0.1");

    // 设置控制周期，可选1000和5000
    if (robot.setControlPeriod(5000) < 0) {
        std::cout << "set Control Period failed" << std::endl;
        return -1;
    }

    // record6为5ms轨迹,record7为1ms轨迹
    TrajectoryIo traj_io("../trajs/record6.offt");
    auto trajs = traj_io.parse();
    int index = 0;

    // 移动到第一个点
    JointPositions jnt_pos;
    for (int i = 0; i < 6; i++) {
        jnt_pos.q[i] = trajs[index][i];
    }
    std::cout << "movej: " << jnt_pos.q * (180. / M_PI) << std::endl;
    robot.movej(jnt_pos);

    robot.read([](const RobotState &robot_state) {
        std::cout << "q: " << robot_state.q * (180. / M_PI) << std::endl;
        return true;
    });

    // 开始记录(储存至 arcs_ws/log/.trace/ 下)
    auto csv_file = "research_control.csv";
    robot.startRecord(csv_file);

    robot.control([&](const RobotState &robot_state, double duration) {
        JointPositions jnt_pos;
        // Ctrl+C 请求退出时，让 control 自然收尾退出
        if (g_stop || robot_state.error_code) {
            jnt_pos.finished = true;
            return jnt_pos;
        }
        for (int i = 0; i < 6; i++) {
            jnt_pos.q[i] = trajs[index][i];
        }
        index++;
        jnt_pos.finished = (index >= trajs.size());
        return jnt_pos;
    });

    // 停止记录
    robot.stopRecord();

    return g_stop ? 130 : 0;
}
