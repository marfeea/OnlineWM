#ifndef TCP_AUTO_CALIB_H
#define TCP_AUTO_CALIB_H

#include <atomic>
#include <shared_mutex>

#include <aubo/aubo_api.h>

using namespace arcs::common_interface;

namespace arcs {
namespace aubo_sdk {

class RtdeClient;
class RpcClient;
using RtdeClientPtr = std::shared_ptr<RtdeClient>;
using RpcClientPtr = std::shared_ptr<RpcClient>;

using Vec3 = std::array<double, 3>;

class ARCS_ABI TcpAutoCalib
{
public:
    TcpAutoCalib(RpcClientPtr rpc, RtdeClientPtr rtde);

    /**
     * @brief setTcpAutoCalibParam
     * 设置TCP自动标定参数
     * @param circle_radius 圆周运动半径,单位:m,默认值:0.01m
     * @param second_plane_offset  第二个圆周运动相对于第一个圆周运动在当前TCP
     * Z+方向偏移的距离,单位:m,默认值:0.01m
     * @param move_line_speed 运动速度,单位:m/s,默认值:0.005m/s
     * @param move_line_r_speed 运动角速度,单位:rad/s,默认值:0.05rad/s
     * @param rotation_angle
     * TCP自动标定计算姿态角度,单位:rad,默认值:0.523599(30°)
     * @param depth_offset
     * TCP自动标定向下移动距离,单位:m,默认值:0.005m
     * @return
     */
    int setTcpAutoCalibParam(const double circle_radius,
                             const double second_plane_offset,
                             const double move_line_speed,
                             const double move_line_r_speed,
                             const double rotation_angle,
                             const double depth_offset);

    /**
     * @brief tcpAutoCalibrate
     * 基于交叉激光传感器的TCP位置自动标定
     * 要求：
     * 1. 调用该接口前需要设置估计的TCP位置
     * 2. 示教TCP位于传感器中心并且TCP Z方向垂直于传感器平面
     * @param io_index_1 DI索引
     * @param io_index_2 DI索引
     * @return 成功返回0和TCP位置（x,y,z），失败返回-1
     */
    ResultWithErrno tcpAutoCalibrate(const int io_index_1,
                                     const int io_index_2);

    /**
     * @brief tcpAutoCorrect
     * 基于交叉激光传感器的TCP自动纠偏
     * 要求：
     * 1. 调用该接口前需要示教TCP位于传感器中心并且TCP Z方向垂直于传感器平面
     * @param sensor_pose 传感器位姿
     * @param io_index_1 DI索引
     * @param io_index_2 DI索引
     * @return 成功返回0和纠偏后的TCP，失败返回-1
     */
    ResultWithErrno tcpAutoCorrect(const std::vector<double> &sensor_pose,
                                   const int io_index_1, const int io_index_2);

    /**
     * @brief calibrateSensorPoseWithTCP
     * 已知TCP标定传感器位姿
     * 要求：
     * 1. 调用该接口前需要示教TCP位于传感器中心并且TCP Z方向垂直于传感器平面
     * @param io_index_1 DI索引
     * @param io_index_2 DI索引
     * @return 成功返回0和传感器位姿，失败返回-1
     */
    ResultWithErrno calibrateSensorPoseWithTCP(const int io_index_1,
                                               const int io_index_2);

private:
    void subscribeRealtimeState();

    std::pair<std::vector<std::vector<double>>,
              std::vector<std::vector<double>>>
    runTwoCirclesCapture(const bool checkOccl = true);

    std::vector<std::vector<double>> runOneCircleAndCaptureFlange(
        const std::vector<double> &circle_center, const bool flag = true);

    std::vector<std::vector<double>> genCirclePoints(
        const std::vector<double> &center, double radius, int npts,
        bool is_offset = false);

    void moveTcpPoseAlignZ(const Vec3 &target_z);

    std::vector<double> alignTcpZToBaseZ();

    std::vector<double> probeAlongAxisFromPointWithZOffset(
        const Vec3 &axis_origin, const Vec3 &axis_z, double start_along_z);

    int waitArrival();

    int waitDIEdgeChanged(const int timeout);

private:
    double second_plane_offset_ = 0.01;
    double circle_radius_ = 0.01;
    double move_line_acc_ = 0.8;
    double move_line_speed_ = 0.005;
    double move_line_r_speed_ = 0.05;
    double rotation_angle_ = 0.523599; // TCP自动标定计算位姿角度，默认30°
    double depth_offset_ = 0.005; // TCP自动标定向下移动距离

    RpcClientPtr rpc_;
    RtdeClientPtr rtde_;
    RobotInterfacePtr robot_;
    int task_id_{ -1 };

    std::atomic_int io_index_1_{ 0 };
    std::atomic_int io_index_2_{ 0 };

    std::shared_mutex tool_poses_mutex_;
    std::vector<std::vector<double>> tool_poses_;
    std::atomic_bool io_1_state_{ false };
    std::atomic_bool io_2_state_{ false };

    bool offset_45_{ false };
};

} // namespace aubo_sdk
} // namespace arcs

#endif // TCP_AUTO_CALIB_H
