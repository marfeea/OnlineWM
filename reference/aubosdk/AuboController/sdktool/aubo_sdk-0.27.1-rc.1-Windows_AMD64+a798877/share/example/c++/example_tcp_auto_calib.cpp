#include "aubo_sdk/rpc.h"
#include "aubo_sdk/rtde.h"

#include <robot_math/robot_math.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "skill_interface/tcp_auto_calib.h"

#ifdef WIN32
#include <Windows.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace arcs::common_interface;
using namespace arcs::aubo_sdk;

constexpr const char *kRobotIp = "172.17.41.84";
constexpr int kRobotRpcPort = 30004;
constexpr int kRobotRtdePort = 30010;
constexpr const char *kUser = "aubo";
constexpr const char *kPassword = "123456";

void printPose(const std::vector<double> &v)
{
    std::cout << "[ ";
    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i] << ", ";
    }
    std::cout << " ]" << std::endl;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    auto rpc = std::make_shared<RpcClient>();
    auto rtde_cli = std::make_shared<RtdeClient>();
    int task_id = -1;

    try {
        rpc->setRequestTimeout(5000);
        rpc->connect(kRobotIp, kRobotRpcPort);
        if (!rpc->hasConnected()) {
            std::cerr << "RPC connect failed" << std::endl;
            return 1;
        }
        rpc->login(kUser, kPassword);
        if (!rpc->hasLogined()) {
            std::cerr << "RPC login failed" << std::endl;
            return 1;
        }

        // 接口调用: 连接到 RTDE 服务
        rtde_cli->connect(kRobotIp, kRobotRtdePort);
        // 接口调用: 登录
        rtde_cli->login(kUser, kPassword);

        TcpAutoCalib tcp_auto_calib(rpc, rtde_cli);

        //        auto result = tcp_auto_calib.calibrateSensorPoseWithTCP(1, 3);
        //        if (std::get<1>(result) != 0) {
        //            std::cout << "通过TCP标定传感器位置失败." << std::endl;
        //        } else {
        //            std::cout << "sensor_pose:";
        //            printPose(std::get<0>(result));
        //        }

        std::vector sensor_pose = { -0.564623, -0.0267271,  0.302365,
                                    -3.13986,  1.81117e-05, -1.26507 };
        auto result = tcp_auto_calib.tcpAutoCorrect(sensor_pose, 1, 3);
        if (std::get<1>(result) != 0) {
            std::cout << "TCP纠偏失败." << std::endl;
        } else {
            std::cout << "新的TCP:";
            printPose(std::get<0>(result));
        }

        //        auto result = tcp_auto_calib.tcpAutoCalibrate(1, 3);
        //        if (std::get<1>(result) != 0) {
        //            std::cout << "TCP自动标定失败." << std::endl;
        //        } else {
        //            std::cout << "TCP标定结果:";
        //            printPose(std::get<0>(result));
        //        }

        rtde_cli->logout();
        rtde_cli->disconnect();

        rpc->logout();
        rpc->disconnect();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
