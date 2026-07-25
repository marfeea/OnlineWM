/** @file  robot_algorithm.h
 *  @brief 机器人算法相关的对外接口
 */
#ifndef AUBO_SDK_ROBOT_ALGORITHM_INTERFACE_H
#define AUBO_SDK_ROBOT_ALGORITHM_INTERFACE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <aubo/global_config.h>
#include <aubo/type_def.h>

namespace arcs {
namespace common_interface {

// clang-format off

/**
 * \chinese
 * @defgroup RobotAlgorithm RobotAlgorithm (机器人算法工具)
 * @ingroup RobotInterface
 * \endchinese
 *
 * \english
 * @defgroup RobotAlgorithm Robot Algorithm Tool
 * @ingroup RobotInterface
 * \endenglish
 *
 * \chinese
 * 机器人算法相关的对外接口
 * \endchinese
 * \english
 * Interfaces related to robot algorithms
 * \endenglish
 */
class ARCS_ABI_EXPORT RobotAlgorithm
{
public:
    RobotAlgorithm();
    virtual ~RobotAlgorithm();

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 力传感器标定算法(三点标定法)
     *
     * @param force 力数据
     * @param q 关节角度
     * @return 标定结果
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * calibrateTcpForceSensor(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: List[List[float]]) -> Tuple[List[float],
     * List[float], float, List[float]]
     *
     * @par Lua函数原型
     * calibrateTcpForceSensor(force: table, q: table) -> table
     *
     * @par Lua示例
     * cal_table = calibrateTcpForceSensor({10.0,10.0,10.0,-1.2,-1.2,-1.2}, {3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * \endchinese
     * \english
     * Force sensor calibration algorithm (three-point calibration method)
     *
     * @param force Force data
     * @param q Joint angles
     * @return Calibration result
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * calibrateTcpForceSensor(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: List[List[float]]) -> Tuple[List[float],
     * List[float], float, List[float]]
     *
     * @par Lua function prototype
     * calibrateTcpForceSensor(force: table, q: table) -> table
     *
     * @par Lua example
     * cal_table = calibrateTcpForceSensor({10.0,10.0,10.0,-1.2,-1.2,-1.2}, {3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * \endenglish
      */
    ForceSensorCalibResult calibrateTcpForceSensor(
        const std::vector<std::vector<double>> &forces,
        const std::vector<std::vector<double>> &poses);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 力传感器标定算法(三点标定法)
     * @param forces
     * @param poses
     * @return force_offset, com, mass, angle error
     *
     * @throws arcs::common_interface::AuboException
     * \endchinese
     * \english
     * Force sensor calibration algorithm (three-point calibration method)
     * @param forces
     * @param poses
     * @return force_offset, com, mass, angle error
     *
     * @throws arcs::common_interface::AuboException
     * \endenglish
     */
    ForceSensorCalibResultWithError calibrateTcpForceSensor2(
        const std::vector<std::vector<double>> &forces,
        const std::vector<std::vector<double>> &poses);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 力传感器偏置标定算法
     *
     * @param force 力数据
     * @param poses 位姿
     * @param mass 质量, 单位: kg
     * @param cog 重心, 单位: m, 形式为(CoGx, CoGy, CoGz)
     * @return 标定结果
     *
     * @throws arcs::common_interface::AuboException
     * \endchinese
     * \english
     * Force sensor offset calibration algorithm
     *
     * @param forces Force data
     * @param poses
     * @param m Mass, unit: kg
     * @param cog Center of gravity, unit: m, format (CoGx, CoGy, CoGz)
     * @return Calibration result
     *
     * @throws arcs::common_interface::AuboException
     * \endenglish
      */
    ResultWithErrno calibrateTcpForceSensor3(
        const std::vector<std::vector<double>> &forces,
        const std::vector<std::vector<double>> &poses, const double &mass,
        const std::vector<double>&cog);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 基于电流的负载辨识算法接口
     *
     * 需要采集空载时运行激励轨迹的位置、速度、电流以及带负载时运行激励轨迹的位置、速度、电流
     *
     * @param data_file_no_payload
     * 空载时运行激励轨迹各关节数据的文件路径（.csv格式），共18列，依次为6个关节位置、6个关节速度、6个关节电流
     * @param data_file_with_payload
     * 带负载运行激励轨迹各关节数据的文件路径（.csv格式），共18列，依次为6个关节位置、6个关节速度、6个关节电流
     * @return 辨识的结果
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * payloadIdentify(self: pyaubo_sdk.RobotAlgorithm, arg0: List[List[float]],
     * arg1: List[List[float]]) -> Tuple[List[float], List[float], float,
     * List[float]]
     *
     * @par Lua函数原型
     * payloadIdentify(data_with_payload: table, data_with_payload: table) ->
     * table
     * \endchinese
     * \english
     * Payload identification algorithm interface based on current
     *
     * Requires collecting position, velocity, and current data when running the excitation trajectory without load, as well as with load.
     *
     * @param data_file_no_payload
     * File path of joint data when running the excitation trajectory without load (.csv format), 18 columns in total: 6 joint positions, 6 joint velocities, 6 joint currents
     * @param data_file_with_payload
     * File path of joint data when running the excitation trajectory with load (.csv format), 18 columns in total: 6 joint positions, 6 joint velocities, 6 joint currents
     * @return Identification result
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * payloadIdentify(self: pyaubo_sdk.RobotAlgorithm, arg0: List[List[float]],
     * arg1: List[List[float]]) -> Tuple[List[float], List[float], float,
     * List[float]]
     *
     * @par Lua function prototype
     * payloadIdentify(data_with_payload: table, data_with_payload: table) ->
     * table
     * \endenglish
     */
    int payloadIdentify(const std::string &data_file_no_payload,
                        const std::string &data_file_with_payload);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 新版基于电流的负载辨识算法接口
     *
     * 需要采集带载时运行最少三个点的位置、速度、加速度、电流、温度、末端传感器数据、底座数据
     *
     * @param data
     * 带负载的各关节数据的文件路径（.csv格式），共42列，末端传感器数据、底座数据默认为0
     * @return 辨识的结果
     * \endchinese
     * \english
     * New version of payload identification algorithm interface based on current
     *
     * Requires collecting at least three points of position, velocity, acceleration, current, temperature, end sensor data, and base data when running with load
     *
     * @param data
     * File path of joint data with load (.csv format), 42 columns in total, end sensor data and base data default to 0
     * @return Identification result
     * \endenglish
     */
    int payloadIdentify1(const std::string &file_name);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 负载辨识是否计算完成
     * @return 完成返回0; 正在进行中返回1; 计算失败返回<0;
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.payloadCalculateFinished","params":[],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":0}
     *
     * \endchinese
     * \english
     * Whether payload identification calculation is finished
     * @return 0 if finished; 1 if in progress; <0 if failed;
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.payloadCalculateFinished","params":[],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":0}
     *
     * \endenglish
     */
    int payloadCalculateFinished();

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 获取负载辨识结果
     * @return
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.getPayloadIdentifyResult","params":[],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[0.0,[],[],[]]}
     *
     * \endchinese
     * \english
     * Get the result of payload identification
     * @return
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.getPayloadIdentifyResult","params":[],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[0.0,[],[],[]]}
     *
     * \endenglish
     */
    Payload getPayloadIdentifyResult();

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 关节摩擦力模型辨识算法接口
     *
     * @param q 关节角度
     * @param qd 关节速度
     * @param qdd 关节加速度
     * @param temp 关节温度
     * @return 是否辨识成功
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * frictionModelIdentify(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: List[List[float]], arg2: List[List[float]],
     * arg3: List[List[float]]) -> bool
     *
     * @par Lua函数原型
     * frictionModelIdentify(q: table, qd: table, qdd: table, temp: table) ->
     * boolean
     * 
     * @par Lua示例
     * Identify_result = frictionModelIdentify({3.083,1.227,1.098,0.670,-1.870,-0.397},
     * {10.0,10.0,10.0,10.0,10.0,10.0},{20.0,20.0,20.0,20.0,20.0,20.0},{30.0,30.0,30.0,30.0,30.0,30.0})
     *
     * \endchinese
     * \english
     * Joint friction model identification algorithm interface
     *
     * @param q Joint positions
     * @param qd Joint velocities
     * @param qdd Joint accelerations
     * @param temp Joint temperatures
     * @return Whether identification succeeded
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * frictionModelIdentify(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: List[List[float]], arg2: List[List[float]],
     * arg3: List[List[float]]) -> bool
     *
     * @par Lua function prototype
     * frictionModelIdentify(q: table, qd: table, qdd: table, temp: table) ->
     * boolean
     *
     * @par Lua example
     * Identify_result = frictionModelIdentify({3.083,1.227,1.098,0.670,-1.870,-0.397},
     * {10.0,10.0,10.0,10.0,10.0,10.0},{20.0,20.0,20.0,20.0,20.0,20.0},{30.0,30.0,30.0,30.0,30.0,30.0})
     *
     * \endenglish
     */
    bool frictionModelIdentify(const std::vector<std::vector<double>> &q,
                               const std::vector<std::vector<double>> &qd,
                               const std::vector<std::vector<double>> &qdd,
                               const std::vector<std::vector<double>> &temp);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 工件坐标系标定算法接口(需要在调用之前正确的设置机器人的TCP偏移)
     * 输入多组关节角度和标定类型，输出工件坐标系位姿(相对于机器人基坐标系)
     *
     * @param q 关节角度
     * @param type 标定类型
     * @return 计算结果(工件坐标系位姿)以及错误代码
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * calibWorkpieceCoordinatePara(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: int) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * calibWorkpieceCoordinatePara(q: table, type: number) -> table, number
     *
     * @par Lua示例
     * coord_pose,coord_num = calibWorkpieceCoordinatePara({3.083,1.227,1.098,0.670,-1.870,-0.397},1)
     *
     * \endchinese
     * \english
     * Workpiece coordinate calibration algorithm interface (requires correct TCP offset set before calling)
     * Input multiple sets of joint angles and calibration type, output workpiece coordinate pose (relative to robot base)
     *
     * @param q Joint angles
     * @param type Calibration type
     * @return Calculation result (workpiece coordinate pose) and error code
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * calibWorkpieceCoordinatePara(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: int) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * calibWorkpieceCoordinatePara(q: table, type: number) -> table, number
     *
     * @par Lua example 
     * coord_pose,coord_num = calibWorkpieceCoordinatePara({3.083,1.227,1.098,0.670,-1.870,-0.397},1)
     *
     * \endenglish
     */
    ResultWithErrno calibWorkpieceCoordinatePara(
        const std::vector<std::vector<double>> &q, int type);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 动力学正解
     *
     * @param q 关节角
     * @param torqs
     * @return 计算结果以及错误代码
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * forwardDynamics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float], arg1:
     * List[float]) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * forwardDynamics(q: table, torqs: table) -> table, number
     *
     * @par Lua示例
     * Dynamics, fk_result = forwardDynamics({3.083,1.227,1.098,0.670,-1.870,-0.397},{0.0,0.0,0.0,0.0,0.0,0.0})
     *
     * \endchinese
     * \english
     * Forward dynamics
     *
     * @param q Joint angles
     * @param torqs
     * @return Calculation result and error code
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * forwardDynamics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float], arg1:
     * List[float]) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * forwardDynamics(q: table, torqs: table) -> table, number
     *
     * @par Lua example
     * Dynamics, fk_result = forwardDynamics({3.083,1.227,1.098,0.670,-1.870,-0.397},{0.0,0.0,0.0,0.0,0.0,0.0})
     *
     * \endenglish
     */
    ResultWithErrno forwardDynamics(const std::vector<double> &q,
                                    const std::vector<double> &torqs);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 动力学正解，基于给定的TCP偏移
     *
     * @param q 关节角
     * @param torqs
     * @param tcp_offset TCP偏移
     * @return 计算结果以及错误代码，同forwardDynamics
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * forwardDynamics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float], arg1:
     * List[float], arg2: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * forwardDynamics1(q: table, torqs: table, tcp_offset: table) -> table, number
     *
     * @par Lua示例
     * Dynamics, fk_result = forwardDynamics1({3.083,1.227,1.098,0.670,-1.870,-0.397},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.13201,0.03879,0,0,0})
     *
     * \endchinese
     * \english
     * Forward dynamics based on the given TCP offset
     *
     * @param q Joint angles
     * @param torqs
     * @param tcp_offset TCP offset
     * @return Calculation result and error code, same as forwardDynamics
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * forwardDynamics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float], arg1:
     * List[float], arg2: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * forwardDynamics1(q: table, torqs: table, tcp_offset: table) -> table, number
     *
     * @par Lua example
     * Dynamics, fk_result = forwardDynamics1({3.083,1.227,1.098,0.670,-1.870,-0.397},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.13201,0.03879,0,0,0})
     *
     * \endenglish
     */
    ResultWithErrno forwardDynamics1(const std::vector<double> &q,
                                     const std::vector<double> &torqs,
                                     const std::vector<double> &tcp_offset);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学正解, 基于激活的TCP偏移(最近的通过 setTcpOffset 设置的参数)
     * 输入关节角度，输出TCP位姿
     *
     * @param q 关节角
     * @return TCP位姿和正解结果是否有效
     * 返回值的第一个参数为正解结果，第二个为正解错误码，错误码返回值列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的关节角无效(维度错误)
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * forwardKinematics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float]) ->
     * Tuple[List[float], int]
     *
     * @par Lua函数原型
     * forwardKinematics(q: table) -> table, number
     *
     * @par Lua示例
     * pose, fk_result = forwardKinematics({3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardKinematics","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[0.7137448715395925,0.08416057568819092,0.6707994191515292,2.4599818776908724,0.4789772388601265,1.6189630435878408],0]}
     * \endchinese
     * \english
     * Forward kinematics, based on the activated TCP offset (the most recently set via setTcpOffset)
     * Input joint angles, output TCP pose
     *
     * @param q Joint angles
     * @return TCP pose and whether the result is valid
     * The first parameter of the return value is the forward kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input joint angles is invalid (dimension error)
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * forwardKinematics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float]) ->
     * Tuple[List[float], int]
     *
     * @par Lua function prototype
     * forwardKinematics(q: table) -> table, number
     *
     * @par Lua example
     * pose, fk_result = forwardKinematics({3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardKinematics","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[0.7137448715395925,0.08416057568819092,0.6707994191515292,2.4599818776908724,0.4789772388601265,1.6189630435878408],0]}
     * \endenglish
     */
    ResultWithErrno forwardKinematics(const std::vector<double> &q);
    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 外部轴轴组协同正解。
     *
     * @param q 机械臂关节角
     * @param axis_q 外部轴关节位置
     * @return TCP 在工件坐标系下的位姿和正解结果是否有效
     * \endchinese
     * \english
     * Coordinated forward kinematics with an external axis group.
     *
     * @param q Robot joint angles
     * @param axis_q External axis joint positions
     * @return TCP pose in workpiece frame and whether the result is valid
     * \endenglish
     */
    ResultWithErrno forwardKinematicsWithAxisGroup(
        const std::vector<double> &q, const std::vector<double> &axis_q);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学正解
     * 输入关节角度，输出TCP位姿
     * @param q 关节角
     * @param tcp_offset tcp偏移
     * @return TCP位姿和正解结果是否有效
     * 返回值的第一个参数为正解结果，第二个为正解错误码，错误码返回值列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的关节角或tcp偏移无效(维度错误)
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * forwardKinematics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * forwardKinematics1(q: table, tcp_offset: table) -> table, number
     *     
     * @par Lua示例
     * pose, fk_result = forwardKinematics1({3.083,1.227,1.098,0.670,-1.870,-0.397},{0.0,0.13201,0.03879,0,0,0})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardKinematics1","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],[0.0,
     * 0.13201,0.03879,0,0,0]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[0.7137636726659518,0.0837705432006433,0.6710022027216355,2.459981877690872,0.4789772388601267,1.6189630435878408],0]}
     *
     * @since 0.24.1
     * \endchinese
     * \english
     * Forward kinematics
     * Input joint angles, output TCP pose
     *
     * @param q Joint angles
     * @param tcp_offset TCP offset
     * @return TCP pose and whether the result is valid
     * The first parameter of the return value is the forward kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input joint angles or tcp offset is invalid (dimension error)
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * forwardKinematics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * forwardKinematics1(q: table, tcp_offset: table) -> table, number
     *     
     * @par Lua example
     * pose, fk_result = forwardKinematics1({3.083,1.227,1.098,0.670,-1.870,-0.397},{0.0,0.13201,0.03879,0,0,0})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardKinematics1","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],[0.0,
     * 0.13201,0.03879,0,0,0]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[0.7137636726659518,0.0837705432006433,0.6710022027216355,2.459981877690872,0.4789772388601267,1.6189630435878408],0]}
     *
     * @since 0.24.1
     * \endenglish
     */
    ResultWithErrno forwardKinematics1(const std::vector<double> &q,
                                       const std::vector<double> &tcp_offset);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学正解(忽略 TCP 偏移值)
     *
     * @param q 关节角
     * @return 法兰盘中心位姿和正解结果是否有效
     * 返回值的第一个参数为正解结果，第二个为正解错误码，错误码返回值列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的关节角无效(维度错误)
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Lua函数原型
     * forwardToolKinematics(q: table) -> table, number
     *     
     * @par Lua示例
     * pose, fk_result = forwardToolKinematics({3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardToolKinematics","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[0.5881351149440136,0.05323734739426938,0.623922550656701,2.4599818776908724,0.4789772388601265,1.6189630435878408],0]}
     * \endchinese
     * \english
     * Forward kinematics (ignoring TCP offset)
     *
     * @param q Joint angles
     * @return Flange center pose and whether the result is valid
     * The first parameter of the return value is the forward kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input joint angles is invalid (dimension error)
     *
     * @par Lua function prototype
     * forwardToolKinematics(q: table) -> table, number
     *     
     * @par Lua example
     * pose, fk_result = forwardToolKinematics({3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardToolKinematics","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[0.5881351149440136,0.05323734739426938,0.623922550656701,2.4599818776908724,0.4789772388601265,1.6189630435878408],0]}
     * \endenglish
     */
    ResultWithErrno forwardToolKinematics(const std::vector<double> &q);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学正解, 基于激活的TCP偏移(最近的通过 setTcpOffset 设置的参数)
     * 输入关节角度，输出各连杆位姿
     *
     * @param q 关节角
     * @return 各个连杆位姿和正解结果是否有效
     * 返回值的第一个参数为正解结果，第二个为正解错误码，错误码返回值列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的关节角无效(维度错误)
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * forwardKinematicsAll(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float]) ->
     * Tuple[List[List[float]], int]
     *
     * @par Lua函数原型
     * forwardKinematicsAll(q: table) -> table, number
     *
     * @par Lua示例
     * poses, fk_result = forwardKinematicsAll({3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardKinematicsAll","params":{"q":[-7.32945e-11,-0.261799,1.74533,0.436332,1.5708,-2.14136e-10]},"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[[0.0010004,0.0,0.122,0.0,0.0,3.141592653589793],
     * [0.0010003999910823934,-0.12166795404953117,0.12205392567412496,1.5690838552993878,-1.3089969602251492,0.0016541204887245322],
     * [0.10659809449330016,-0.12166124765791932,0.5161518260534821,-1.5718540206872664,0.43633111081725523,-0.002370419930605744],
     * [0.4482255342315581,-0.1226390142692,0.3568490758201224,-0.000788980482890453,1.5665204619271216,-1.5719618592940798],
     * [0.5519603828181741,-0.12282266347465487,0.35639753697117343,1.5707938201843654,0.0042721909279596635,1.569044569428874],
     * [0.5513243939877184,-0.12401224959696328,0.2615193425222568,-3.1349118101994096,0.004272190926529066,1.569044569643007],
     * [0.7494883076798231,-0.025647479212994567,-0.04023458911333461,-3.1349118101994096,0.004272190926529066,1.569044569643007]],0]}
     * \endchinese
     * \english
     * Forward kinematics, based on the activated TCP offset (the most recently set via setTcpOffset)
     * Input joint angles, output links poses
     *
     * @param q Joint angles
     * @return links poses and whether the result is valid
     * The first parameter of the return value is the forward kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input joint angles is invalid (dimension error)
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * forwardKinematicsAll(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float]) ->
     * Tuple[List[List[float]], int]
     *
     * @par Lua function prototype
     * forwardKinematicsAll(q: table) -> table, number
     *
     * @par Lua example
     * pose, fk_result = forwardKinematicsAll({3.083,1.227,1.098,0.670,-1.870,-0.397})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.forwardKinematicsAll","params":{"q":[-7.32945e-11,-0.261799,1.74533,0.436332,1.5708,-2.14136e-10]},"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[[0.0010004,0.0,0.122,0.0,0.0,3.141592653589793],
     * [0.0010003999910823934,-0.12166795404953117,0.12205392567412496,1.5690838552993878,-1.3089969602251492,0.0016541204887245322],
     * [0.10659809449330016,-0.12166124765791932,0.5161518260534821,-1.5718540206872664,0.43633111081725523,-0.002370419930605744],
     * [0.4482255342315581,-0.1226390142692,0.3568490758201224,-0.000788980482890453,1.5665204619271216,-1.5719618592940798],
     * [0.5519603828181741,-0.12282266347465487,0.35639753697117343,1.5707938201843654,0.0042721909279596635,1.569044569428874],
     * [0.5513243939877184,-0.12401224959696328,0.2615193425222568,-3.1349118101994096,0.004272190926529066,1.569044569643007],
     * [0.7494883076798231,-0.025647479212994567,-0.04023458911333461,-3.1349118101994096,0.004272190926529066,1.569044569643007]],0]}
     * \endenglish
     */
    ResultWithErrno1 forwardKinematicsAll(const std::vector<double> &q);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学逆解
     * 输入TCP位姿和参考关节角度，输出关节角度
     *
     * @param qnear 参考关节角
     * @param pose TCP位姿
     * @return 关节角和逆解结果是否有效
     * 返回值的第一个参数为逆解结果，第二个为逆解错误码，错误码返回列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的参考关节角或TCP位姿无效(维度错误)
     * -23 - 逆解计算不收敛，计算出错
     * -24 - 逆解计算超出机器人最大限制
     * -25 - 逆解输入配置存在错误
     * -26 - 逆解雅可比矩阵计算失败
     * -27 - 目标点存在解析解，但均不满足选解条件
     * -28 - 逆解返回未知类型错误
     * 若错误码非0,则返回值的第一个参数为输入参考关节角qnear
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * inverseKinematics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * inverseKinematics(qnear: table, pose: table) -> table, int
     *
     * @par Lua示例
     * joint,ik_result = inverseKinematics({0,0,0,0,0,0},{0.81665,-0.20419,0.43873,-3.135,0.004,1.569})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematics","params":[[0,0,0,0,0,0],[0.71374,0.08417,0.6708,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],0]}
     * \endchinese
     * \english
     * Inverse kinematics
     * Input TCP pose and reference joint angles, output joint angles
     *
     * @param qnear Reference joint angles
     * @param pose TCP pose
     * @return Joint angles and whether the inverse kinematics result is valid
     * The first parameter of the return value is the inverse kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input reference joint angles or tcp pose is invalid (dimension error)
     * -23 - Inverse kinematics calculation does not converge, calculation error
     * -24 - Inverse kinematics calculation exceeds robot limits
     * -25 - Inverse kinematics input configuration error
     * -26 - Inverse kinematics Jacobian calculation failed
     * -27 - Analytical solution exists but none satisfy the selection criteria
     * -28 - Unknown error in inverse kinematics
     * If the error code is not 0, the first parameter of the return value is the input reference joint angles qnear
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * inverseKinematics(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * inverseKinematics(qnear: table, pose: table) -> table, int
     *
     * @par Lua example
     * joint,ik_result = inverseKinematics({0,0,0,0,0,0},{0.81665,-0.20419,0.43873,-3.135,0.004,1.569})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematics","params":[[0,0,0,0,0,0],[0.71374,0.08417,0.6708,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],0]}
     * \endenglish
     */
    ResultWithErrno inverseKinematics(const std::vector<double> &qnear,
                                      const std::vector<double> &pose);
    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 外部轴轴组协同逆解。
     *
     * @param qnear 机械臂参考关节角
     * @param axis_q 外部轴关节位置
     * @param pose TCP 在工件坐标系下的目标位姿
     * @return 机械臂关节角和逆解结果是否有效
     * \endchinese
     * \english
     * Coordinated inverse kinematics with an external axis group.
     *
     * @param qnear Robot reference joint angles
     * @param axis_q External axis joint positions
     * @param pose Target TCP pose in workpiece frame
     * @return Robot joint angles and whether the result is valid
     * \endenglish
     */
    ResultWithErrno inverseKinematicsWithAxisGroup(
        const std::vector<double> &qnear, const std::vector<double> &axis_q,
        const std::vector<double> &pose);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学逆解
     * 输入TCP位姿和参考关节角度，输出关节角度
     *
     * @param qnear 参考关节角
     * @param pose TCP位姿
     * @param tcp_offset TCP偏移
     * @return 关节角和逆解结果是否有效，同 inverseKinematics
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * inverseKinematics1(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float], arg2: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * inverseKinematics1(qnear: table, pose: table, tcp_offset: table) -> table, int
     *
     * @par Lua示例
     * joint,ik_result = inverseKinematics1({0,0,0,0,0,0},{0.81665,-0.20419,0.43873,-3.135,0.004,1.569},{0.04,-0.035,0.1,0,0,0})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematics1","params":[[0,0,0,0,0,0],[0.71374,0.08417,0.6708,2.46,0.479,1.619],[0.0,
     * 0.13201,0.03879,0,0,0]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[3.084454549595208,1.2278265883747776,1.0986586440159576,0.6708221281915528,-1.8712459848518375,-0.3965111476861782],0]}
     * \endchinese
     * \english
     * Inverse kinematics
     * Input TCP pose and reference joint angles, output joint angles
     *
     * @param qnear Reference joint angles
     * @param pose TCP pose
     * @param tcp_offset TCP offset
     * @return Joint angles and whether the result is valid, same as inverseKinematics
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * inverseKinematics1(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float], arg2: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * inverseKinematics1(qnear: table, pose: table, tcp_offset: table) -> table, int
     *
     * @par Lua example
     * joint,ik_result = inverseKinematics1({0,0,0,0,0,0},{0.81665,-0.20419,0.43873,-3.135,0.004,1.569},{0.04,-0.035,0.1,0,0,0})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematics1","params":[[0,0,0,0,0,0],[0.71374,0.08417,0.6708,2.46,0.479,1.619],[0.0,
     * 0.13201,0.03879,0,0,0]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[3.084454549595208,1.2278265883747776,1.0986586440159576,0.6708221281915528,-1.8712459848518375,-0.3965111476861782],0]}
     * \endenglish
     */
    ResultWithErrno inverseKinematics1(const std::vector<double> &qnear,
                                       const std::vector<double> &pose,
                                       const std::vector<double> &tcp_offset);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 同构型最小关节变化逆解
     * 按照 qnear 的机械臂构型计算目标 TCP 位姿逆解，并在同构型逆解中选择相对 qnear 关节变化最小的一组
     * 基于激活的 TCP 偏移(最近的通过 setTcpOffset 设置的参数)
     *
     * @param qnear 参考关节角
     * @param pose TCP位姿，基于激活的 TCP 偏移(最近的通过 setTcpOffset 设置的参数)
     * @return 关节角和逆解结果是否有效
     * 返回值的第一个参数为逆解结果，第二个为逆解错误码，错误码返回列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的参考关节角或TCP位姿无效(维度错误)
     * -23 - 逆解计算不收敛，计算出错
     * -24 - 逆解计算超出机器人最大限制
     * -25 - 逆解输入配置存在错误
     * -26 - 逆解雅可比矩阵计算失败
     * -27 - 目标点存在解析解，但均不满足同构型最小关节变化选解条件
     * -28 - 逆解返回未知类型错误
     * 若错误码非0,则返回值的第一个参数为输入参考关节角qnear
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * inverseKinematics2(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * inverseKinematics2(qnear: table, pose: table) -> table, int
     *
     * @par Lua示例
     * joint,ik_result = inverseKinematics2({0,0,0,0,0,0},{0.81665,-0.20419,0.43873,-3.135,0.004,1.569})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematics2","params":[[0,0,0,0,0,0],[0.71374,0.08417,0.6708,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],0]}
     * \endchinese
     * \english
     * Inverse kinematics with the same robot configuration and minimum joint change
     * Calculate the inverse kinematics solution for the target TCP pose according to the robot configuration of qnear, and select the solution with the minimum joint change relative to qnear among solutions with the same configuration.
     * This interface is based on the activated TCP offset, which is the latest parameter set by setTcpOffset.
     *
     * @param qnear Reference joint angles
     * @param pose TCP pose, based on the activated TCP offset, which is the latest parameter set by setTcpOffset
     * @return Joint angles and whether the result is valid
     * The first parameter of the return value is the inverse kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input reference joint angles or tcp pose is invalid (dimension error)
     * -23 - Inverse kinematics calculation does not converge, calculation error
     * -24 - Inverse kinematics calculation exceeds robot limits
     * -25 - Inverse kinematics input configuration error
     * -26 - Inverse kinematics Jacobian calculation failed
     * -27 - Analytical solution exists but none satisfy the same-configuration minimum-joint-change selection criteria
     * -28 - Unknown error in inverse kinematics
     * If the error code is not 0, the first parameter of the return value is the input reference joint angles qnear
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * inverseKinematics2(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: List[float]) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * inverseKinematics2(qnear: table, pose: table) -> table, int
     *
     * @par Lua example
     * joint,ik_result = inverseKinematics2({0,0,0,0,0,0},{0.81665,-0.20419,0.43873,-3.135,0.004,1.569})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematics2","params":[[0,0,0,0,0,0],[0.71374,0.08417,0.6708,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],0]}
     * \endenglish
     */
    ResultWithErrno inverseKinematics2(const std::vector<double> &qnear,
                                       const std::vector<double> &pose);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 求出所有的逆解, 基于激活的 TCP 偏移
     *
     * @param pose TCP位姿
     * @return 关节角和逆解结果是否有效
     * 返回的错误码同inverseKinematics
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematicsAll","params":[[0.71374,0.08417,0.6708,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],
     * [3.081056801097411,0.17985038037652645,-1.0991717292664145,-0.4806460200109001,-1.869182975312333,-0.402066016835411],
     * [0.4090095277807992,-0.1623365054641728,1.081775890307679,0.26993250263224805,0.9738255833642309,0.000572556627720845],
     * [0.4116449425067969,-1.1931664523907126,-1.0822709833775688,-0.8665964106161371,0.9732141569888207,0.006484919654891586]],0]}
     * \endchinese
     * \english
     * Solve all inverse kinematics solutions based on the activated TCP offset
     *
     * @param pose TCP pose
     * @return Joint angles and whether the result is valid
     * The returned error code is the same as inverseKinematics
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematicsAll","params":[[0.71374,0.08417,0.6708,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627],
     * [3.081056801097411,0.17985038037652645,-1.0991717292664145,-0.4806460200109001,-1.869182975312333,-0.402066016835411],
     * [0.4090095277807992,-0.1623365054641728,1.081775890307679,0.26993250263224805,0.9738255833642309,0.000572556627720845],
     * [0.4116449425067969,-1.1931664523907126,-1.0822709833775688,-0.8665964106161371,0.9732141569888207,0.006484919654891586]],0]}
     * \endenglish
     */
    ResultWithErrno1 inverseKinematicsAll(const std::vector<double> &pose);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 求出所有的逆解, 基于提供的 TCP 偏移
     *
     * @param pose TCP位姿
     * @param tcp_offset TCP偏移
     * @return 关节角和逆解结果是否有效，同 inverseKinematicsAll
     * 返回的错误码同inverseKinematics
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematicsAll1","params":[[0.71374,0.08417,0.6708,2.46,0.479,1.619],[0.0,
     * 0.13201,0.03879,0,0,0]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[[3.084454549595208,1.2278265883747776,1.0986586440159576,0.6708221281915528,-1.8712459848518375,-0.3965111476861782],
     * [3.0818224058231602,0.17980369843203092,-1.0997576631122077,-0.48102131527371267,-1.8697135490338517,-0.40149459722060593],
     * [0.40972960018231047,-0.16226026285489026,1.0823403816496,0.2700204411869427,0.9734251963887868,0.0012903686498106507],
     * [0.41236549588802296,-1.193621392918341,-1.0828346680836718,-0.8671097369314354,0.972815367289568,0.007206851371073478]],0]}
     * \endchinese
     * \english
     * Solve all inverse kinematics solutions based on the provided TCP offset
     *
     * @param pose TCP pose
     * @param tcp_offset TCP offset
     * @return Joint angles and whether the result is valid, same as inverseKinematicsAll
     * The returned error code is the same as inverseKinematics
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseKinematicsAll1","params":[[0.71374,0.08417,0.6708,2.46,0.479,1.619],[0.0,
     * 0.13201,0.03879,0,0,0]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[[3.084454549595208,1.2278265883747776,1.0986586440159576,0.6708221281915528,-1.8712459848518375,-0.3965111476861782],
     * [3.0818224058231602,0.17980369843203092,-1.0997576631122077,-0.48102131527371267,-1.8697135490338517,-0.40149459722060593],
     * [0.40972960018231047,-0.16226026285489026,1.0823403816496,0.2700204411869427,0.9734251963887868,0.0012903686498106507],
     * [0.41236549588802296,-1.193621392918341,-1.0828346680836718,-0.8671097369314354,0.972815367289568,0.007206851371073478]],0]}
     * \endenglish
     */
    ResultWithErrno1 inverseKinematicsAll1(
        const std::vector<double> &pose, const std::vector<double> &tcp_offset);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学逆解(忽略 TCP 偏移值)
     *
     * @param qnear 参考关节角
     * @param pose 法兰盘中心的位姿
     * @return 关节角和逆解结果是否有效
     * 返回值的第一个参数为逆解结果，第二个为逆解错误码，错误码返回列表如下
     * 0 - 成功
     * -1 - 机械臂状态不对(未初始化完成，可尝试再次调用)
     * -5 - 输入的参考关节角或位姿无效(维度错误)
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Lua函数原型
     * inverseToolKinematics(qnear: table, pose: table) -> table, int
     *
     * @par Lua示例
     * joint, ik_result = inverseToolKinematics({0,0,0,0,0,0},{0.58815,0.0532,0.62391,2.46,0.479,1.619})
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseToolKinematics","params":[[0,0,0,0,0,0],[0.58815,0.0532,0.62391,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[3.083609363838651,1.22736129158332,1.098095443698268,0.6705395395487186,-1.8706605026855632,-0.39714507002376465],0]}
     * \endchinese
     * \english
     * Inverse kinematics (ignoring TCP offset)
     *
     * @param qnear Reference joint angles
     * @param pose Flange center pose
     * @return Joint angles and whether the result is valid
     * The first parameter of the return value is the inverse kinematics result, the second is the error code. Error codes are as follows:
     * 0 - Success
     * -1 - The status of the robot is incorrect (not initialized yet, you can try calling it again)
     * -5 - The input reference joint angles or pose is invalid (dimension error)
     * @throws arcs::common_interface::AuboException
     *
     * @par Lua function prototype
     * inverseToolKinematics(qnear: table, pose: table) -> table, int
     *
     * @par Lua example
     * joint, ik_result = inverseToolKinematics({0,0,0,0,0,0},{0.58815,0.0532,0.62391,2.46,0.479,1.619})
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseToolKinematics","params":[[0,0,0,0,0,0],[0.58815,0.0532,0.62391,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[3.083609363838651,1.22736129158332,1.098095443698268,0.6705395395487186,-1.8706605026855632,-0.39714507002376465],0]}
     * \endenglish
     */
    ResultWithErrno inverseToolKinematics(const std::vector<double> &qnear,
                                          const std::vector<double> &pose);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 运动学逆解(忽略 TCP 偏移值)
     *
     * @param qnear 参考关节角
     * @param pose 法兰盘中心的位姿
     * @return 关节角和逆解结果是否有效
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseToolKinematicsAll","params":[[0.58815,0.0532,0.62391,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[[3.083609363838651,1.22736129158332,1.098095443698268,0.6705395395487186,-1.8706605026855632,-0.39714507002376465],
     * [3.0809781797426523,0.17987122696706134,-1.0991932793263717,-0.4807053707530958,-1.8691282890274434,-0.40212516672751814],
     * [0.40892195618737215,-0.16235398607358653,1.081812753177426,0.27003586475871766,0.9738744130114284,0.00048462518316674287],
     * [0.41155633414333076,-1.1932173012004512,-1.082306542045813,-0.8665312056504818,0.9732632365861417,0.0063958311601771175]],0]}
     * \endchinese
     * \english
     * Inverse kinematics (ignoring TCP offset)
     *
     * @param qnear Reference joint angles
     * @param pose Flange center pose
     * @return Joint angles and whether the result is valid
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.inverseToolKinematicsAll","params":[[0.58815,0.0532,0.62391,2.46,0.479,1.619]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[[3.083609363838651,1.22736129158332,1.098095443698268,0.6705395395487186,-1.8706605026855632,-0.39714507002376465],
     * [3.0809781797426523,0.17987122696706134,-1.0991932793263717,-0.4807053707530958,-1.8691282890274434,-0.40212516672751814],
     * [0.40892195618737215,-0.16235398607358653,1.081812753177426,0.27003586475871766,0.9738744130114284,0.00048462518316674287],
     * [0.41155633414333076,-1.1932173012004512,-1.082306542045813,-0.8665312056504818,0.9732632365861417,0.0063958311601771175]],0]}
     * \endenglish
     */
    ResultWithErrno1 inverseToolKinematicsAll(const std::vector<double> &pose);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 根据输入的关节角计算并返回对应的机械臂构型
     *
     * 机械臂构型由三个维度的状态组合而成，各维度定义如下：
     * - 肩部方向：LEFT(肩部朝左) / RIGHT(肩部朝右)
     * - 肘部方向：UP(肘部朝上) / DOWN(肘部朝下)
     * - 腕部状态：FLIP(腕部翻转) / NOFLIP(腕部不翻转)
     *
     * 三个维度两两组合共形成8种基础构型（L/R + U/D + F/N），该接口会根据输入的关节角解析出当前对应的构型类型，
     * 并返回其枚举值对应的整数形式及错误码（注：接口返回的是组合构型值，如LUF对应0，而非单独的LEFT/UP/FLIP枚举值）。
     *
     * @param q 输入的关节角数组，6轴机械臂为6个元素，单位：弧度(rad)
     * @return 机械臂构型结果及错误码（类型为ResultWithErrno3，即std::tuple<int, int>）：
     *         - 第一个int：机械臂构型枚举(RobotConfiguration)对应的整数值，取值范围及含义：
     *           -1 (NONE)   - 无效构型
     *            0 (LUF)    - LEFT+UP+FLIP（左肩、肘上、腕翻转）
     *            1 (LUN)    - LEFT+UP+NOFLIP（左肩、肘上、腕不翻转）
     *            2 (LDF)    - LEFT+DOWN+FLIP（左肩、肘下、腕翻转）
     *            3 (LDN)    - LEFT+DOWN+NOFLIP（左肩、肘下、腕不翻转）
     *            4 (RUF)    - RIGHT+UP+FLIP（右肩、肘上、腕翻转）
     *            5 (RUN)    - RIGHT+UP+NOFLIP（右肩、肘上、腕不翻转）
     *            6 (RDF)    - RIGHT+DOWN+FLIP（右肩、肘下、腕翻转）
     *            7 (RDN)    - RIGHT+DOWN+NOFLIP（右肩、肘下、腕不翻转）
     *         - 第二个int：错误码，错误码含义如下：
     *           0  - 成功：构型计算完成，返回有效构型值
     *          -1  - 机械臂状态异常：未初始化完成，可尝试重新初始化后调用
     *          -5  - 输入参数无效：关节角数组维度错误（非6个元素）或数值超出合理范围
     *
     * @throws arcs::common_interface::AuboException 输入参数非法（如空数组、元素数量错误）时抛出
     *
     * @par Python函数原型
     * getRobotConfiguration(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float]) -> Tuple[int, int]
     *
     * @par Lua函数原型
     * getRobotConfiguration(q: table) -> number, number
     *
     * @par Lua示例
     * -- 输入6轴关节角（单位：rad），获取构型及错误码
     * local joint_angles = {3.083,1.227,1.098,0.670,-1.870,-0.397}
     * local config_val, err_code = getRobotConfiguration(joint_angles)
     * if err_code == 0 then
     *     print("机械臂构型值：", config_val)  -- 示例输出：4（对应RUF，右肩、肘上、腕翻转）
     * else
     *     print("获取构型失败，错误码：", err_code)
     * end
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.getRobotConfiguration","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[4,0]}
     * \endchinese
     * \english
     * Calculate and return the corresponding robot configuration based on input joint angles
     *
     * The robot configuration consists of three dimensions of states, with the following definitions for each dimension:
     * - Shoulder direction: LEFT (Shoulder to the left) / RIGHT (Shoulder to the right)
     * - Elbow direction: UP (Elbow up) / DOWN (Elbow down)
     * - Wrist state: FLIP (Wrist flipped) / NOFLIP (Wrist not flipped)
     *
     * The combination of the three dimensions forms 8 basic configurations (L/R + U/D + F/N). This interface parses the current
     * configuration type from the input joint angles and returns its integer value corresponding to the RobotConfiguration enumeration
     * and an error code (Note: The interface returns combined configuration values, e.g., LUF corresponds to 0, not the individual
     * enumeration values of LEFT/UP/FLIP).
     *
     * @param q Input joint angle array, 6 elements for 6-axis robot, unit: radians (rad)
     * @return Robot configuration result and error code (type: ResultWithErrno3, i.e., std::tuple<int, int>):
     *         - First int: Integer value corresponding to the RobotConfiguration enumeration, value range and meanings:
     *           -1 (NONE)   - Invalid configuration
     *            0 (LUF)    - LEFT+UP+FLIP (Left shoulder, elbow up, wrist flipped)
     *            1 (LUN)    - LEFT+UP+NOFLIP (Left shoulder, elbow up, wrist not flipped)
     *            2 (LDF)    - LEFT+DOWN+FLIP (Left shoulder, elbow down, wrist flipped)
     *            3 (LDN)    - LEFT+DOWN+NOFLIP (Left shoulder, elbow down, wrist not flipped)
     *            4 (RUF)    - RIGHT+UP+FLIP (Right shoulder, elbow up, wrist flipped)
     *            5 (RUN)    - RIGHT+UP+NOFLIP (Right shoulder, elbow up, wrist not flipped)
     *            6 (RDF)    - RIGHT+DOWN+FLIP (Right shoulder, elbow down, wrist flipped)
     *            7 (RDN)    - RIGHT+DOWN+NOFLIP (Right shoulder, elbow down, wrist not flipped)
     *         - Second int: Error code with the following meanings:
     *           0  - Success: Configuration calculation completed, valid configuration value returned
     *          -1  - Abnormal robot state: Not initialized completely, try reinitializing before calling
     *          -5  - Invalid input parameters: Incorrect dimension of joint angle array (not 6 elements) or values out of reasonable range
     *
     * @throws arcs::common_interface::AuboException Thrown when input parameters are illegal (e.g., empty array, wrong number of elements)
     *
     * @par Python function prototype
     * getRobotConfiguration(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float]) -> Tuple[int, int]
     *
     * @par Lua function prototype
     * getRobotConfiguration(q: table) -> number, number
     *
     * @par Lua example
     * -- Input 6-axis joint angles (unit: rad) to get configuration and error code
     * local joint_angles = {3.083,1.227,1.098,0.670,-1.870,-0.397}
     * local config_val, err_code = getRobotConfiguration(joint_angles)
     * if err_code == 0 then
     *     print("Robot configuration value: ", config_val)  -- Example output: 4 (corresponding to RUF, Right shoulder, elbow up, wrist flipped)
     * else
     *     print("Failed to get configuration, error code: ", err_code)
     * end
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.getRobotConfiguration","params":[[3.083688522170976,1.2273215976885394,1.098072739631141,0.6705738810610149,-1.870715392248607,-0.39708546603119627]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[4,0]}
     * \endenglish
     */
    ResultWithErrno3 getRobotConfiguration(const std::vector<double>& q);

    /**
     * \chinese
     * 求解movej之间的轨迹点
     *
     * @param q1 movej的起点
     * @param r1 在q1处的交融半径
     * @param q2 movej的终点
     * @param r2 在q2处的交融半径
     * @param d 采样距离
     * @return q1~q2 之间笛卡尔空间离散轨迹点(x,y,z,rx,ry,rz)集合
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * pathMovej(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float], arg1:
     * float, arg2: List[float], arg3: float, arg4: float) -> List[List[float]]
     *
     * @par Lua函数原型
     * pathMovej(q1: table, r1: number, q2: table, r2: number, d: number) ->
     * table, number
     *
     * @par Lua示例
     * path , num = pathMovej({0.0,-0.2618,1.7453,0.4364,1.5711,0.0},0.25,{0.3234,-0.5405,1.5403,0.5881,1.2962,0.7435},0.03,0.2)
     *
     * \endchinese
     * \english
     * Solve the trajectory points between movej
     *
     * @param q1 Start point of movej
     * @param r1 Blend radius at q1
     * @param q2 End point of movej
     * @param r2 Blend radius at q2
     * @param d Sampling distance
     * @return Discrete trajectory points (x, y, z, rx, ry, rz) between q1 and q2 in Cartesian space
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * pathMovej(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float], arg1:
     * float, arg2: List[float], arg3: float, arg4: float) -> List[List[float]]
     *
     * @par Lua function prototype
     * pathMovej(q1: table, r1: number, q2: table, r2: number, d: number) ->
     * table, number
     *
     * @par Lua example
     * path , num = pathMovej({0.0,-0.2618,1.7453,0.4364,1.5711,0.0},0.25,{0.3234,-0.5405,1.5403,0.5881,1.2962,0.7435},0.03,0.2)
     *
     * \endenglish
     */
    std::vector<std::vector<double>> pathMovej(const std::vector<double> &q1,
                                               double r1,
                                               const std::vector<double> &q2,
                                               double r2, double d);
    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 计算机械臂末端的雅克比矩阵
     *
     * @param q 关节角
     * @param base_or_end 参考坐标系为基坐标系（或者末端坐标系）
     *        true: 在 base 下描述
     *        false: 在 末端坐标系 下描述
     * @return 雅克比矩阵是否有效
     * 返回值的第一个参数为该构型下对应的雅克比矩阵，第二个为逆解错误码
     * 此接口的错误码返回值在0.28.1-rc.21 0.29.0-alpha.25版本之后做了修改。
     * 此前逆解错误时返回 30082 ，修改后错误码返回列表如下
     * 0 - 成功
     * -23 - 逆解计算不收敛，计算出错
     * -24 - 逆解计算超出机器人最大限制
     * -25 - 逆解输入配置存在错误
     * -26 - 逆解雅可比矩阵计算失败
     * -27 - 目标点存在解析解，但均不满足选解条件
     * -28 - 逆解返回未知类型错误
     * 若错误码非0,则返回值的第一个参数为输入参考关节角qnear
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * calJacobian(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: bool) -> Tuple[List[float], int]
     *
     * @par Lua函数原型
     * calJacobian(q: table, base_or_end: boolean) -> table
     *
     * @par Lua示例
     * calJ_result = calJacobian({0.58815,0.0532,0.62391,2.46,0.479,1.619},true)
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.calcJacobian","params":[[0.58815,0.0532,0.62391,2.46,0.479,1.619],true],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":[[0.20822779551242535,-0.5409416184208162,0.2019786999613013,0.061264982268770196,-0.026269884327316487,
     * 0.10131708699859962,0.26388933410019777,-0.36074292664199115,0.1346954733416397,0.04085636647597124,-0.07244204452918337,0.0708466286633346,
     * 0.0,0.10401808481666497,-0.12571344758923886,-0.07741290545882097,0.18818543519232858,0.04628646442706299,0.0,0.5548228314607867,
     * -0.5548228314607868,0.5548228314607868,-0.7901273140338193,0.37230961532208007,0.0,-0.8319685244586092,0.8319685244586091,-0.8319685244586091,
     * -0.5269197820578843,-0.8184088260676008,1.0,3.749399456654644e-33,-6.512048180336603e-18,1.0956823467534067e-16,-0.31313634553301894,
     * 0.43771285536682175],0]}
     * \endchinese
     * \english
     * Calculate the Jacobian matrix at the robot end-effector
     *
     * @param q Joint angles
     * @param base_or_end Reference frame: base (or end-effector)
     *        true: described in base frame
     *        false: described in end-effector frame
     * @return Whether the Jacobian matrix is valid
     * The first parameter of the return value is the Jacobian matrix for this configuration, the second is the error code.
     * The error code list was updated after versions 0.28.1-rc.21 and 0.29.0-alpha.25.
     * Previously, inverse kinematics errors returned 30082. The updated error codes are:
     * 0 - Success
     * -23 - Inverse kinematics calculation does not converge, calculation error
     * -24 - Inverse kinematics calculation exceeds robot limits
     * -25 - Inverse kinematics input configuration error
     * -26 - Inverse kinematics Jacobian calculation failed
     * -27 - Analytical solution exists but none satisfy the selection criteria
     * -28 - Unknown error in inverse kinematics
     * If the error code is not 0, the first parameter of the return value is the input reference joint angles qnear
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * calJacobian(self: pyaubo_sdk.RobotAlgorithm, arg0: List[float],
     * arg1: bool) -> Tuple[List[float], int]
     *
     * @par Lua function prototype
     * calJacobian(q: table, base_or_end: boolean) -> table
     *
     * @par Lua example
     * calJ_result = calJacobian({0.58815,0.0532,0.62391,2.46,0.479,1.619},true)
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.calcJacobian","params":[[0.58815,0.0532,0.62391,2.46,0.479,1.619],true],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":[[0.20822779551242535,-0.5409416184208162,0.2019786999613013,0.061264982268770196,-0.026269884327316487,
     * 0.10131708699859962,0.26388933410019777,-0.36074292664199115,0.1346954733416397,0.04085636647597124,-0.07244204452918337,0.0708466286633346,
     * 0.0,0.10401808481666497,-0.12571344758923886,-0.07741290545882097,0.18818543519232858,0.04628646442706299,0.0,0.5548228314607867,
     * -0.5548228314607868,0.5548228314607868,-0.7901273140338193,0.37230961532208007,0.0,-0.8319685244586092,0.8319685244586091,-0.8319685244586091,
     * -0.5269197820578843,-0.8184088260676008,1.0,3.749399456654644e-33,-6.512048180336603e-18,1.0956823467534067e-16,-0.31313634553301894,
     * 0.43771285536682175],0]}
     * \endenglish
     */
    ResultWithErrno calcJacobian(const std::vector<double> &q,
                                 bool base_or_end);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 求解交融的轨迹点
     *
     * @param type
     * 0-movej 和 movej
     * 1-movej 和 movel
     * 2-movel 和 movej
     * 3-movel 和 movel
     * @param q_start 交融前路径的起点
     * @param q_via 交融点
     * @param q_to 交融后路径的终点
     * @param r 在q_via处的交融半径
     * @param d 采样距离
     * @return q_via处的交融段笛卡尔空间离散轨迹点(x,y,z)集合
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * pathBlend3Points(self: pyaubo_sdk.RobotAlgorithm, arg0: int, arg1:
     * List[float], arg2: List[float], arg3: List[float], arg4: float, arg5:
     * float) -> List[List[float]]
     *
     * @par Lua函数原型
     * pathBlend3Points(type: number, q_start: table, q_via: table, q_to: table,
     * r: number, d: number) -> table, number
     *
     * @par Lua示例
     * q_via , num = pathBlend3Points(1,{0.58815,0.0532,0.62391,2.46,0.479,1.619},{0.0,-0.2618,1.7453,0.4364,1.5711,0.0},
     * {0.3234,-0.5405,1.5403,0.5881,1.2962,0.7435},0.25,0.02)
     *
     * \endchinese
     * \english
     * Solve the blended trajectory points
     *
     * @param type
     * 0-movej and movej
     * 1-movej and movel
     * 2-movel and movej
     * 3-movel and movel
     * @param q_start Start point before blending
     * @param q_via Blending point
     * @param q_to End point after blending
     * @param r Blend radius at q_via
     * @param d Sampling distance
     * @return Discrete trajectory points (x, y, z) of the blend segment at q_via in Cartesian space
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python function prototype
     * pathBlend3Points(self: pyaubo_sdk.RobotAlgorithm, arg0: int, arg1:
     * List[float], arg2: List[float], arg3: List[float], arg4: float, arg5:
     * float) -> List[List[float]]
     *
     * @par Lua function prototype
     * pathBlend3Points(type: number, q_start: table, q_via: table, q_to: table,
     * r: number, d: number) -> table, number
     *
     * @par Lua example
     * q_via , num = pathBlend3Points(1,{0.58815,0.0532,0.62391,2.46,0.479,1.619},{0.0,-0.2618,1.7453,0.4364,1.5711,0.0},
     * {0.3234,-0.5405,1.5403,0.5881,1.2962,0.7435},0.25,0.02)
     *
     * \endenglish
     */
    std::vector<std::vector<double>> pathBlend3Points(
        int type, const std::vector<double> &q_start,
        const std::vector<double> &q_via, const std::vector<double> &q_to,
        double r, double d);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 生成用于负载辨识的激励轨迹
     * 此接口内部调用pathBufferAppend
     * 将离线轨迹存入buffer中，后续可通过movePathBuffer运行离线轨迹
     * @param name 轨迹名字
     * @param traj_conf 各关节轨迹的限制条件
     * traj_conf.move_axis: 运动的轴
     * 由于实际用户现场可能不希望在负载辨识时控制机械臂多关节大幅度运动，故最好选用traj_conf.move_axis=LoadIdentifyMoveAxis::Joint_4_6;
     * traj_conf.init_joint:
     * 运动初始关节角,为了避免关节5接近零位时的奇异问题，应设置traj_conf.init_joint[4]的绝对值不小于0.3(rad)，接近1.57(rad)为宜。其余关节的关节角可任意设置
     * traj_conf.lower_joint_bound, traj_conf.upper_joint_bound:
     * 关节角上下限,维度应与config.move_axis维度保持一致,推荐设置upper_joint_bound为2，lower_joint_bound为-2
     * config.max_velocity, config.max_acceleration:
     * 关节角速度角加速度限制,维度应与config.move_axis维度保持一致,出于安全和驱动器跟随性能的考虑,推荐设置max_velocity=3,max_acceleration=5
     *
     * @return 成功返回0；失败返回错误码
     * AUBO_BUSY
     * AUBO_BAD_STATE
     * -AUBO_INVL_ARGUMENT
     * -AUBO_BAD_STATE
     *
     * @throws arcs::common_interface::AuboException
     * \endchinese
     * \english
     * Generate excitation trajectory for payload identification
     * This interface internally calls pathBufferAppend
     * The offline trajectory is stored in the buffer, and can be executed later via movePathBuffer
     * @param name Trajectory name
     * @param traj_conf Joint trajectory constraints
     * traj_conf.move_axis: Moving axes
     * Since users may not want large multi-joint movements during payload identification, it is recommended to use traj_conf.move_axis=LoadIdentifyMoveAxis::Joint_4_6;
     * traj_conf.init_joint:
     * Initial joint angles. To avoid singularity issues near joint 5 zero position, set abs(traj_conf.init_joint[4]) >= 0.3(rad), preferably close to 1.57(rad). Other joints can be set arbitrarily.
     * traj_conf.lower_joint_bound, traj_conf.upper_joint_bound:
     * Joint angle limits, dimensions should match config.move_axis. Recommended: upper_joint_bound=2, lower_joint_bound=-2
     * config.max_velocity, config.max_acceleration:
     * Joint velocity and acceleration limits, dimensions should match config.move_axis. For safety and driver performance, recommended: max_velocity=3, max_acceleration=5
     *
     * @return Returns 0 on success; error code on failure
     * AUBO_BUSY
     * AUBO_BAD_STATE
     * -AUBO_INVL_ARGUMENT
     * -AUBO_BAD_STATE
     *
     * @throws arcs::common_interface::AuboException
     * \endenglish
     */
    int generatePayloadIdentifyTraj(const std::string &name,
                                    const TrajConfig &traj_conf);
    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 负载辨识轨迹是否生成完成
     *
     * @return 完成返回0; 正在进行中返回1; 计算失败返回<0;
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.payloadIdentifyTrajGenFinished","params":[],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":0}
     *
     * \endchinese
     * \english
     * Whether payload identification trajectory generation is finished
     *
     * @return 0 if finished; 1 if in progress; <0 if failed;
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.payloadIdentifyTrajGenFinished","params":[],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":0}
     *
     * \endenglish
     */
    int payloadIdentifyTrajGenFinished();

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 求解 moveS 的轨迹点
     *
     * @brief pathMoveS
     * @param qs 样条轨迹生成点集合
     * @param d 采样距离
     * @return
     *
     * @throws arcs::common_interface::AuboException
     * \endchinese
     * \english
     * Solve the trajectory points for moveS
     *
     * @brief pathMoveS
     * @param qs Spline trajectory generation point set
     * @param d Sampling distance
     * @return
     *
     * @throws arcs::common_interface::AuboException
     * \endenglish
     */
    std::vector<std::vector<double>> pathMoveS(
        const std::vector<std::vector<double>> &qs, double d);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 振动抑制参数辨识算法接口
     *
     * @param q 当前关节角度
     * @param qd 当前关节速度
     * @param target_q 目标关节角度
     * @param target_qd 关节速度
     * @param target_qdd 关节加速度
     * @param tool_offset 工具TCP信息
     * @param omega 振动频率
     * @param zeta 振动阻尼比
     * @return 振动抑制参数和是否辨识成功
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * calibVibrationParams(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * List[List[float]], arg1: List[List[float]], arg2: List[List[float]], arg3: List[List[float]],
     * arg4: List[List[float]], arg5: List[float]) -> list[list[float]],int
     *
     * @par Lua函数原型
     * calibVibrationParams(q: table,qd: table, target_q: table, target_qd: table,
     * target_qdd: table, tool_offset: table, omega: table, zeta: table) -> table,number
     * \endchinese
     */
    ResultWithErrno1 calibVibrationParams(const std::vector<std::vector<double>> &q,
                             const std::vector<std::vector<double>> &qd,
                             const std::vector<std::vector<double>> &target_q,
                             const std::vector<std::vector<double>> &target_qd,
                             const std::vector<std::vector<double>> &target_qdd,
                             const std::vector<double> &tool_offset);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 振动抑制参数辨识算法接口1
     *
     * @param record_cache_name 目标缓存名称
     * @param tool_offset 工具TCP信息
     * @return 振动抑制参数和是否辨识成功
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Python函数原型
     * calibVibrationParams(self: pyaubo_sdk.RobotAlgorithm, arg0:
     * string, arg1: List[float]) -> list[list[float]],int
     *
     * @par Lua函数原型
     * calibVibrationParams(record_cache_name: string, tool_offset: table) -> table,number
     * \endchinese
     */
    ResultWithErrno1 calibVibrationParams1(const std::string &record_cache_name, const std::vector<double> &tool_offset);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 判断是否需要重新辨识振动参数
     *
     * @param param1 参考参数（如上一次辨识结果）
     * @param param2 当前参数（如新测量结果）
     * @param threshold 变化阈值（0~1），超过则需重新辨识
     * @return >0 需要重新辨识，=0 不需要，<0 出错
     *
     * @par Lua函数原型
     * needVibrationRecalib(param1: table, param2: table, threshold: number) -> number
     *
     * @par JSON-RPC示例
     * {"jsonrpc":"2.0","method":"VibrationController.needVibrationRecalib",
     *  "params":[param1_obj, param2_obj, 0.1],"id":1}
     * \endchinese
     */
    int needVibrationRecalib(const VibrationRecalibrationParameter &param1,
                             const VibrationRecalibrationParameter &param2,
                             double threshold);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 添加立方体碰撞对象
     *
     * 该接口用于向当前机器人算法场景中添加一个命名的立方体碰撞对象组。
     *
     * @param name 碰撞对象唯一标识
     * @param link_name 碰撞对象挂载的参考 link 名称。常用名称包括：
     * `world`、`base_link`、`shoulder_Link`、`upperArm_Link`、
     * `foreArm_Link`、`wrist1_Link`、`wrist2_Link`、`wrist3_Link`、
     * `end_effector`
     * @param sizes 立方体尺寸列表。每个元素格式为
     * `[length, width, height]`，单位：m
     * @param poses 碰撞对象位姿列表。每个元素表示相对参考 link 的位姿，
     * 格式为 `[x, y, z, rx, ry, rz]`
     * @return 0 表示成功，非 0 表示失败
     *
     * @par Lua函数原型
     * addCollisionBox(name, link_name, sizes, poses) -> integer
     *
     * @par Lua示例
     * -- 添加挂载到 world 的环境碰撞体
     * addCollisionBox("env_box", "world",
     *                 {{0.4, 0.4, 0.2}},
     *                 {{0.6, 0.0, 0.1, 0.0, 0.0, 0.0}})
     *
     * -- 添加挂载到 end_effector 的末端碰撞体
     * addCollisionBox("tool_box", "end_effector",
     *                 {{0.1, 0.08, 0.06}},
     *                 {{0.0, 0.0, 0.05, 0.0, 0.0, 0.0}})
     *
     * @par Python函数原型
     * addCollisionBox(name: str, link_name: str, sizes: list[list[float]],
     * poses: list[list[float]]) -> int
     *
     * @par Python示例
     * # 添加挂载到 world 的环境碰撞体
     * robot_algorithm.addCollisionBox(
     *     "env_box", "world",
     *     [[0.4, 0.4, 0.2]],
     *     [[0.6, 0.0, 0.1, 0.0, 0.0, 0.0]]
     * )
     *
     * # 添加挂载到 end_effector 的末端碰撞体
     * robot_algorithm.addCollisionBox(
     *     "tool_box", "end_effector",
     *     [[0.1, 0.08, 0.06]],
     *     [[0.0, 0.0, 0.05, 0.0, 0.0, 0.0]]
     * )
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.addCollisionBox",
     * "params":["env_box","world",[[0.4,0.4,0.2]],
     * [[0.6,0.0,0.1,0.0,0.0,0.0]]],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":0}
     * \endchinese
     *
     * \english
     * Add a box collision object
     *
     * This interface adds a named group of box collision objects into the
     * current robot algorithm scene.
     *
     * @param name Unique identifier of the collision object
     * @param link_name Name of the reference link the collision object is
     * attached to. Common names include: `world`, `base_link`,
     * `shoulder_Link`, `upperArm_Link`, `foreArm_Link`, `wrist1_Link`,
     * `wrist2_Link`, `wrist3_Link`, and `end_effector`
     * @param sizes List of box sizes. Each element is in the format
     * `[length, width, height]`, unit: m
     * @param poses List of collision object poses. Each element is a pose
     * relative to the reference link in the format `[x, y, z, rx, ry, rz]`
     * @return 0 on success, non-zero on failure
     *
     * @par Lua function prototype
     * addCollisionBox(name, link_name, sizes, poses) -> integer
     *
     * @par Lua example
     * -- Add an environment collision box attached to world
     * addCollisionBox("env_box", "world",
     *                 {{0.4, 0.4, 0.2}},
     *                 {{0.6, 0.0, 0.1, 0.0, 0.0, 0.0}})
     *
     * -- Add a tool collision box attached to end_effector
     * addCollisionBox("tool_box", "end_effector",
     *                 {{0.1, 0.08, 0.06}},
     *                 {{0.0, 0.0, 0.05, 0.0, 0.0, 0.0}})
     *
     * @par Python function prototype
     * addCollisionBox(name: str, link_name: str, sizes: list[list[float]],
     * poses: list[list[float]]) -> int
     *
     * @par Python example
     * # Add an environment collision box attached to world
     * robot_algorithm.addCollisionBox(
     *     "env_box", "world",
     *     [[0.4, 0.4, 0.2]],
     *     [[0.6, 0.0, 0.1, 0.0, 0.0, 0.0]]
     * )
     *
     * # Add a tool collision box attached to end_effector
     * robot_algorithm.addCollisionBox(
     *     "tool_box", "end_effector",
     *     [[0.1, 0.08, 0.06]],
     *     [[0.0, 0.0, 0.05, 0.0, 0.0, 0.0]]
     * )
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.addCollisionBox",
     * "params":["env_box","world",[[0.4,0.4,0.2]],
     * [[0.6,0.0,0.1,0.0,0.0,0.0]]],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":0}
     * \endenglish
    */
    int addCollisionBox(const std::string &name,
                        const std::string &link_name,
                        const std::vector<std::vector<double>> &sizes,
                        const std::vector<std::vector<double>> &poses);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 删除指定的碰撞对象
     *
     * @param name 需要删除的碰撞对象名称
     * @return 0 表示成功，非 0 表示失败
     *
     * @par Lua函数原型
     * removeCollisionObject(name) -> integer
     *
     * @par Lua示例
     * removeCollisionObject("tool_box")
     *
     * @par Python函数原型
     * removeCollisionObject(name: str) -> int
     *
     * @par Python示例
     * robot_algorithm.removeCollisionObject("tool_box")
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.removeCollisionObject",
     * "params":["tool_box"],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":0}
     * \endchinese
     *
     * \english
     * Remove a collision object by name
     *
     * @param name Name of the collision object to remove
     * @return 0 on success, non-zero on failure
     *
     * @par Lua function prototype
     * removeCollisionObject(name) -> integer
     *
     * @par Lua example
     * removeCollisionObject("tool_box")
     *
     * @par Python function prototype
     * removeCollisionObject(name: str) -> int
     *
     * @par Python example
     * robot_algorithm.removeCollisionObject("tool_box")
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.removeCollisionObject",
     * "params":["tool_box"],"id":1}
     *
     * @par JSON-RPC response example
     * {"id":1,"jsonrpc":"2.0","result":0}
     * \endenglish
     */
    int removeCollisionObject(const std::string &name);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 验证机器人运动路径从起点到终点的可达性，不启用外部碰撞检测。
     *
     * 该接口保留用于兼容旧版 SDK、Lua、JSON-RPC 和 ROS 服务调用。
     *
     * @param type 路径类型标识，用于指定起点和终点的数据类型：
     *        0 - 起点：关节角，终点：关节角
     *        1 - 起点：位姿，终点：位姿
     * @param start 路径起点数据
     * @param r1 起点交融半径，单位：m（米）
     * @param end 路径终点数据
     * @param r2 终点交融半径，单位：m（米）
     * @param d 路径采样间隔，单位：m（米），间隔越小验证精度越高，但耗时越长
     * @return 路径可达性结果码
     *
     * \endchinese
     *
     * \english
     * Validate the reachability of the robot's motion path from start point to
     * end point without external collision checks.
     *
     * This interface is kept for compatibility with existing SDK, Lua,
     * JSON-RPC, and ROS service calls.
     *
     * @param type Path type identifier, used to specify the data types of
     * start and end:
     *    0 - Start: Joint angles, End: Joint angles
     *    1 - Start: Pose, End: Pose
     * @param start Start point data of the path
     * @param r1 Start point blending radius, unit: m (meters)
     * @param end End point data of the path
     * @param r2 End point blending radius, unit: m (meters)
     * @param d Path sampling interval, unit: m (meters). A smaller interval
     * improves validation accuracy but takes more time.
     * @return Path reachability result code
     *
     * \endenglish
     */
    int validatePath(int type, const std::vector<double> &start, double r1,
                     const std::vector<double> &end, double r2, double d);

    /**
     * @ingroup RobotAlgorithm
     * \chinese
     * 验证机器人运动路径从起点到终点的可达性，可选择是否启用外部碰撞检测。
     *
     * 该接口通过采样的方式验证指定路径是否存在超限、自碰撞、外部碰撞、
     * 奇异等不可达情况，支持关节角<->关节角、位姿<->位姿两种路径类型验证。
     *
     * @param type 路径类型标识，用于指定起点和终点的数据类型：
     *        0 - 起点：关节角，终点：关节角
     *        1 - 起点：位姿，终点：位姿
     * @param start 路径起点数据
     * @param r1 起点交融半径，单位：m（米）
     * @param end 路径终点数据
     * @param r2 终点交融半径，单位：m（米）
     * @param d 路径采样间隔，单位：m（米），间隔越小验证精度越高，但耗时越长
     * @param check_external_collision 是否启用外部碰撞检测：
     *        false - 保持原有路径可达性与自碰撞校验
     *        true  - 在原有校验基础上增加与外部对象的碰撞检测
     * @return 路径可达性结果码：
     *        0  - 可达：路径无异常，可正常运动
     *       -18 - 路径中存在关节超限/笛卡尔空间超限
     *       -21 - 轨迹生成失败
     *       -22 - 路径中机器人本体发生自碰撞
     *       -24 - 路径中经过机器人奇异位形
     *       -27 - 目标点有解但超出关节限位
     *
     * @throws arcs::common_interface::AuboException
     *
     * @par Lua函数原型
     * validatePath1(type, start, r1, end, r2, d, check_external_collision) -> number
     *
     * @par Lua示例
     * local result =
     *     validatePath1(0, start_joint, 0.01, end_joint, 0.01, 0.05, true)
     *
     * @par Python函数原型
     * validatePath1(type: int, start: list[float], r1: float, end: list[float],
     * r2: float, d: float, check_external_collision: bool) -> int
     *
     * @par Python示例
     * result = validatePath1(1, start_joint, 0.01, end_pose, 0.01, 0.05, True)
     *
     * @par JSON-RPC请求示例
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.validatePath1","params":[0,
     * [0.0,0.0,1.57,0.0,1.57,0.0], 0.01, [0.52,0.0,1.57,0.0,1.57,0.0], 0.01,
     * 0.05, true],"id":1}
     *
     * @par JSON-RPC响应示例
     * {"id":1,"jsonrpc":"2.0","result":0}
     * \endchinese
     *
     * \english
     * Validate the reachability of the robot's motion path from start point to
     * end point, with optional external collision checks.
     *
     * This interface verifies whether the specified path has unreachable
     * conditions such as out-of-limit, self-collision, collision with external
     * objects, singularity, etc., by sampling. It supports two path type
     * validations: Joint-Joint and Pose-Pose.
     *
     * @param type Path type identifier, used to specify the data types of
     * start and end:
     *    0 - Start: Joint angles, End: Joint angles
     *    1 - Start: Pose, End: Pose
     * @param start Start point data of the path
     * @param r1 Start point blending radius, unit: m (meters)
     * @param end End point data of the path
     * @param r2 End point blending radius, unit: m (meters)
     * @param d Path sampling interval, unit: m (meters). A smaller interval
     * improves validation accuracy but takes more time.
     * @param check_external_collision Whether to enable external collision
     * checks:
     *    false - Keep the original reachability and self-collision checks
     *    true  - Add collision checks against external objects
     *
     * @return Path reachability result code:
     *    0  - Reachable: Path is normal, movement is possible
     *    -18 - Joint/Cartesian space limits exceeded in the path
     *    -21 - Trajectory generation failed
     *    -22 - Self-collision of the robot body in the path
     *    -24 - Path passing through robot singular configuration
     *    -27 - The target point has a solution but exceeds joint limits
     *
     * @throws arcs::common_interface::AuboException Thrown when parameters are
     * invalid.
     *
     * @par Lua function prototype
     * validatePath1(type, start, r1, end, r2, d, check_external_collision) -> integer
     *
     * @par Lua example
     * result = validatePath1(0, start_joint, 0.01, end_joint, 0.01, 0.05, true)
     *
     * @par Python function prototype
     * validatePath1(type: int, start: list[float], r1: float, end: list[float],
     * r2: float, d: float, check_external_collision: bool) -> int
     *
     * @par Python example
     * result = validatePath1(1, start_joint, 0.01, end_pose, 0.01, 0.005, True)
     *
     * @par JSON-RPC request example
     * {"jsonrpc":"2.0","method":"rob1.RobotAlgorithm.validatePath1","params":[0,
     * [0.0,0.0,1.57,0.0,1.57,0.0], 0.01, [0.52,0.0,1.57,0.0,1.57,0.0], 0.01,
     * 0.005, true],"id":1}
     *
     * @par JSON-RPC response example {"id":1,"jsonrpc":"2.0","result":0}
     * \endenglish
     */
    int validatePath1(int type, const std::vector<double> &start, double r1,
                      const std::vector<double> &end, double r2, double d,
                      bool check_external_collision);

     /**
      * @ingroup RobotAlgorithm
      * \chinese
      * 标定远端特征坐标系的位置和姿态在基坐标系的描述
      *
      * 通过三点法标定远端特征坐标系的位置和姿态，并返回该远端坐标系在基坐标系下的描述。
      * 三个标定点的含义由姿态标定方法 type 决定。
      *
      * 标定类型 type 说明：
      *  0 - oxy  原点 -> X轴正方向 -> XY平面（Y轴正方向）
      *  1 - oxz  原点 -> X轴正方向 -> XZ平面（Z轴正方向）
      *  2 - oyz  原点 -> Y轴正方向 -> YZ平面（Z轴正方向）
      *  3 - oyx  原点 -> Y轴正方向 -> YX平面（X轴正方向）
      *  4 - ozx  原点 -> Z轴正方向 -> ZX平面（X轴正方向）
      *  5 - ozy  原点 -> Z轴正方向 -> ZY平面（Y轴正方向）
      *
      * 以标定类型 type = 0（O_X_XY / oxy）为例说明三点含义：
      * 1) 运动到远端特征坐标系原点，记录末端在基坐标系下的位姿 → pose[0]
      * 2) 沿远端坐标系 X 轴正方向移动一段距离，记录末端位姿 → pose[1]
      * 3) 在远端坐标系 XY 平面内移动一段距离（确定Y轴方向），记录末端位姿 → pose[2]
      *
      * @param pose 输入：三个标定点处，末端坐标系在基坐标系下的位姿集合
      *                             每个位姿格式：[x, y, z, rx, ry, rz]
      * @param type 输入：姿态标定类型（0~5），决定三点的几何含义
      * @return 计算结果：包含远端特征坐标系在基坐标系下的位姿 + 错误码
      *         错误码 < 0 表示标定失败；>= 0 表示标定成功
      *
      * @par Lua 函数原型
      * calibRemoteFeatureFrame(pose: table, type: number) -> table, number
      *
      * @par Lua 示例
      * local p1 = {0.55462, 0.06219, 0.37175, -3.142, 0.0, 1.580}
      * local p2 = {0.63746, 0.11805, 0.37175, -3.142, 0.0, 1.580}
      * local p3 = {0.40441, 0.28489, 0.37174, -3.142, 0.0, 1.580}
      * local points = {p1, p2, p3}
      * local type = 0
      * local remote_pose, ret = calibRemoteFeatureFrame(points, type)
      *
      * \endchinese
      *
      * \english
      * Calibrate the pose of the remote feature frame in the base frame
      *
      * Calibrate the position and orientation of a remote feature frame using three points,
      * and return the pose of the remote feature frame represented in the base frame.
      * The definition of the three calibration points is determined by the calibration type.
      *
      * Calibration Type (type):
      *  0 - oxy  Origin -> Positive X -> XY plane (Positive Y)
      *  1 - oxz  Origin -> Positive X -> XZ plane (Positive Z)
      *  2 - oyz  Origin -> Positive Y -> YZ plane (Positive Z)
      *  3 - oyx  Origin -> Positive Y -> YX plane (Positive X)
      *  4 - ozx  Origin -> Positive Z -> ZX plane (Positive X)
      *  5 - ozy  Origin -> Positive Z -> ZY plane (Positive Y)
      *
      * Take type = 0 (O_X_XY / oxy) as an example:
      * 1) Move to the origin of the remote feature frame, record end-effector pose → pose[0]
      * 2) Move along positive X of the remote frame, record end-effector pose → pose[1]
      * 3) Move within XY plane of the remote frame, record end-effector pose → pose[2]
      *
      * @param pose Input: Three poses of the end-effector frame in the base frame
      *                             Each pose format: [x, y, z, rx, ry, rz]
      * @param type Input: Calibration type (0~5), defines the geometry of the three points
      * @return Result: Contains the remote feature frame pose in base frame + error code
      *         Error code < 0: calibration failed; >= 0: calibration succeeded
      *
      * @par Lua prototype
      * calibRemoteFeatureFrame(pose: table, type: number) -> table, number
      *
      * @par Lua example
      * local p1 = {0.55462, 0.06219, 0.37175, -3.142, 0.0, 1.580}
      * local p2 = {0.63746, 0.11805, 0.37175, -3.142, 0.0, 1.580}
      * local p3 = {0.40441, 0.28489, 0.37174, -3.142, 0.0, 1.580}
      * local points = {p1, p2, p3}
      * local type = 0
      * local remote_pose, ret = calibRemoteFeatureFrame(points, type)
      *
      * \endenglish
      */
     ResultWithErrno calibRemoteFeatureFrame(const std::vector<std::vector<double>>& pose, int type);

     /**
      * @ingroup RobotAlgorithm
      * \chinese
      * @brief 标定末端特征坐标系相对于法兰坐标系的姿态
      *
      * 通过三点法标定末端特征坐标系的姿态，求解其在**法兰坐标系**下的旋转矩阵。
      * 标定过程中需保持**末端特征坐标系姿态固定不变**，仅改变位置。
      * 三个标定点的几何含义由 type 标定类型决定。
      *
      * 标定类型 type 说明：
      *  0 - oxy  原点 -> X轴正方向 -> XY平面（Y轴正方向）
      *  1 - oxz  原点 -> X轴正方向 -> XZ平面（Z轴正方向）
      *  2 - oyz  原点 -> Y轴正方向 -> YZ平面（Z轴正方向）
      *  3 - oyx  原点 -> Y轴正方向 -> YX平面（X轴正方向）
      *  4 - ozx  原点 -> Z轴正方向 -> ZX平面（X轴正方向）
      *  5 - ozy  原点 -> Z轴正方向 -> ZY平面（Y轴正方向）
      *
      * 以标定类型 type = 0（O_X_XY / oxy）为例说明操作步骤：
      * 1) 移动到末端特征坐标系原点，保持姿态固定，记录法兰在基坐标系下位姿 → pose[0]
      * 2) 保持姿态固定，沿末端特征坐标系 X 轴正方向移动，记录法兰位姿 → pose[1]
      * 3) 保持姿态固定，沿末端特征坐标系 XY 平面移动，记录法兰位姿 → pose[2]
      *
      * @param pose 输入：3个位姿，每个位姿是法兰在基坐标系下的描述 [x,y,z,rx,ry,rz]
      * @param type 输入：姿态标定类型（0~5），定义三点几何关系
      * @return ResultWithErrno 包含：
      *         - 标定结果：末端特征坐标系在法兰坐标系下的姿态旋转矩阵
      *         - 错误码：<0 标定失败，>=0 标定成功
      *
      * @par Lua 函数原型
      * calibEndFeatureOriInFlange(pose: table, type: number) -> table, number
      *
      * \endchinese
      *
      * \english
      * @brief Calibrate the orientation of the end feature frame relative to the flange frame
      *
      * Calibrate the orientation of the end feature frame using three points,
      * and compute its rotation matrix with respect to the **flange frame**.
      * The orientation of the end feature frame must remain **fixed** during calibration.
      *
      * Calibration Type (type):
      *  0 - oxy  Origin -> Positive X -> XY plane (Positive Y)
      *  1 - oxz  Origin -> Positive X -> XZ plane (Positive Z)
      *  2 - oyz  Origin -> Positive Y -> YZ plane (Positive Z)
      *  3 - oyx  Origin -> Positive Y -> YX plane (Positive X)
      *  4 - ozx  Origin -> Positive Z -> ZX plane (Positive X)
      *  5 - ozy  Origin -> Positive Z -> ZY plane (Positive Y)
      *
      * Example for type = 0 (O_X_XY / oxy):
      * 1) Move to origin of end feature frame, record flange pose in base → pose[0]
      * 2) Keep orientation fixed, move along +X of end feature frame, record flange pose → pose[1]
      * 3) Keep orientation fixed, move in XY plane, record flange pose → pose[2]
      *
      * @param pose Input: 3 poses of flange frame in base frame [x,y,z,rx,ry,rz]
      * @param type Input: Calibration type (0~5)
      * @return ResultWithErrno Includes:
      *         - Calibrated orientation of end feature frame w.r.t. flange frame
      *         - Error code: <0 fail, >=0 success
      * \endenglish
      */
     ResultWithErrno calibEndFeatureOriInFlange(const std::vector<std::vector<double>>& pose, int type);


     /**
      * @ingroup RobotAlgorithm
      * \chinese
      * @brief 四点/多点法标定机器人末端特征坐标系原点在法兰坐标系下的位置
      *
      * 标定原理：
      * 保持末端特征坐标系原点在空间中固定不动，控制机器人以4种及以上不同姿态触碰该固定点，
      * 记录每个姿态下法兰在基坐标系中的位姿，通过球面拟合计算得到特征原点在法兰坐标系下的位置。
      *
      * 输入位姿格式：[x, y, z, rx, ry, rz]
      * 输入点数要求：>= 4
      *
      * @param pose 输入：多组法兰在基坐标系下的位姿
      * @return ResultWithErrno 标定结果与错误码
      *         第一个返回值：std::vector<double>，长度固定为4
      *                      [0] = x  末端特征原点在法兰坐标系下X坐标
      *                      [1] = y  末端特征原点在法兰坐标系下Y坐标
      *                      [2] = z  末端特征原点在法兰坐标系下Z坐标
      *                      [3] = 标定拟合误差
      *         第二个返回值：int 错误码，<0 标定失败，>=0 标定成功
      *
      * @par Lua 函数原型
      * calibEndFeaturePosInFlange(pose: table) -> result_table, number
      *
      * @par Lua 示例
      * local p1 = {0.5546, 0.0621, 0.3717, -3.142, 0.0, 1.580}
      * local p2 = {0.6374, 0.1180, 0.3717, -3.142, 0.0, 1.580}
      * local p3 = {0.4044, 0.2848, 0.3717, -3.142, 0.0, 1.580}
      * local p4 = {0.4800, 0.1900, 0.3717, -3.142, 0.0, 1.580}
      * local points = {p1, p2, p3, p4}
      * local res, err = calibEndFeaturePosInFlange(points)
      * -- res[1]=x, res[2]=y, res[3]=z, res[4]=error
      *
      * @par Python 函数原型
      * calibEndFeaturePosInFlange(pose: list[list[float]]) -> tuple[list[float], int]
      *
      * @par Python 示例
      * p1 = [0.5546, 0.0621, 0.3717, -3.142, 0.0, 1.580]
      * p2 = [0.6374, 0.1180, 0.3717, -3.142, 0.0, 1.580]
      * p3 = [0.4044, 0.2848, 0.3717, -3.142, 0.0, 1.580]
      * p4 = [0.4800, 0.1900, 0.3717, -3.142, 0.0, 1.580]
      * points = [p1, p2, p3, p4]
      * res, err = calibEndFeaturePosInFlange(points)
      * # res[0]=x, res[1]=y, res[2]=z, res[3]=error
      *
      * \endchinese
      *
      * \english
      * @brief Calibrate end feature frame origin position in flange frame (4-point / multi-point)
      *
      * Calibrate TCP position by fixing the end feature point and recording >=4 flange poses
      * in base frame with different robot orientations.
      *
      * Input pose format: [x, y, z, rx, ry, rz]
      * Input size: >=4 poses
      *
      * @param pose Input: flange poses in base frame
      * @return ResultWithErrno Result and error code
      *         First: vector<double> size=4
      *                [0]=x, [1]=y, [2]=z: feature origin in flange frame
      *                [3]: calibration error
      *         Second: int error code, <0 fail, >=0 success
      *
      * @par Lua prototype
      * calibEndFeaturePosInFlange(pose: table) -> result_table, number
      *
      * @par Python prototype
      * calibEndFeaturePosInFlange(pose: list[list[float]]) -> tuple[list[float], int]
      *
      * \endenglish
      */
     ResultWithErrno calibEndFeaturePosInFlange(const std::vector<std::vector<double>>& pose);


     /**
      * @ingroup RobotAlgorithm
      * \chinese
      * @brief 合并稠密路径点 / 剔除过短路径段，对输入路径点进行稀疏化处理
      *
      * 处理原理：
      * 根据指定的最小段长阈值，遍历输入路径点序列，剔除两点之间距离小于最小段长的稠密点，
      * 保留关键路径点，生成更平滑、点数更少的优化后路径，支持不同类型的路径点合并规则。
      *
      * 输入点格式：通用路径点格式 [x, y, z, rx, ry, rz, ...]
      *
      * @param type 输入：合并算法类型 / 模式选择
      * @param points 输入：原始稠密路径点序列
      * @param min_segment_length 输入：最小路径段长度阈值，小于该值的段会被合并/剔除
      * @return ResultWithErrno1 处理结果与错误码
      *         第一个返回值：std::vector<std::vector<double>>，处理后的稀疏路径点
      *         第二个返回值：int 错误码，<0 处理失败，>=0 处理成功
      *
      * @par Lua 函数原型
      * mergeDenseWaypoints(type: number, points: table, min_segment_length: number) -> result_table, number
      *
      * @par Lua 示例
      * local type = 0
      * local min_len = 0.01
      * local wp1 = {0.1, 0.2, 0.3, -3.142, 0.0, 0.0}
      * local wp2 = {0.1005, 0.2003, 0.3002, -3.142, 0.0, 0.0}
      * local wp3 = {0.12, 0.22, 0.32, -3.142, 0.0, 0.0}
      * local input_wps = {wp1, wp2, wp3}
      * local res, err = mergeDenseWaypoints(type, input_wps, min_len)
      * -- res 为剔除稠密点后的路径点表
      *
      * @par Python 函数原型
      * mergeDenseWaypoints(type: int, points: list[list[float]], min_segment_length: float) -> tuple[list[list[float]], int]
      *
      * @par Python 示例
      * type = 0
      * min_len = 0.01
      * wp1 = [0.1, 0.2, 0.3, -3.142, 0.0, 0.0]
      * wp2 = [0.1005, 0.2003, 0.3002, -3.142, 0.0, 0.0]
      * wp3 = [0.12, 0.22, 0.32, -3.142, 0.0, 0.0]
      * input_wps = [wp1, wp2, wp3]
      * res, err = mergeDenseWaypoints(type, input_wps, min_len)
      * # res 为处理后的稀疏路径点列表
      *
      * \endchinese
      *
      * \english
      * @brief Merge dense waypoints / remove short segments, sparsify input path points
      *
      * Processing principle:
      * Iterate the input waypoint sequence based on the specified minimum segment length threshold,
      * remove dense points with distance less than the threshold, keep key waypoints,
      * generate optimized path with fewer points and smoother trajectory, support different merge modes.
      *
      * Input point format: general path point [x, y, z, rx, ry, rz, ...]
      *
      * @param type Input: merge algorithm type / mode selection
      * @param points Input: original dense waypoint sequence
      * @param min_segment_length Input: minimum segment length threshold, segments shorter than this will be merged/removed
      * @return ResultWithErrno1 Processing result and error code
      *         First: vector<vector<double>>, sparsified waypoints after processing
      *         Second: int error code, <0 fail, >=0 success
      *
      * @par Lua prototype
      * mergeDenseWaypoints(type: number, points: table, min_segment_length: number) -> result_table, number
      *
      * @par Python prototype
      * mergeDenseWaypoints(type: int, points: list[list[float]], min_segment_length: float) -> tuple[list[list[float]], int]
      *
      * \endenglish
      */
     ResultWithErrno1 mergeDenseWaypoints(int type, const std::vector<std::vector<double>> &points, double min_segment_length);

protected:
    void *d_;
};
using RobotAlgorithmPtr = std::shared_ptr<RobotAlgorithm>;

} // namespace common_interface
} // namespace arcs

#endif // AUBO_SDK_ROBOT_ALGORITHM_INTERFACE_H
