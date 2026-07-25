<div align='center'>
<h1 align='center'>Aubo SDK RTDE 配方说明 </h1>
</div>

## 输入菜单
| 名称 | 数据类型 | 说明 |
| :----:| :----: | :----: |
|set_recipe|RtdeRecipe|设置配方 |
|input_bit_registers0_to_31|int|通用位 该范围的布尔输入寄存器保留用于现场总线/PLC 接口。|
|input_bit_registers32_to_63|int|通用位 该范围的布尔输入寄存器保留用于现场总线/PLC 接口。|
|input_bit_registers64_to_127|int64_t|64 个通用位 X: [64..127] - 布尔输入寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_int_registers_0|int|48 个通用整数寄存器 X: [0..23] - 整数输入寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 整数输入寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_float_registers_0|float|48 个通用整数寄存器 X: [0..23] - 整数输入寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 整数输入寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_double_registers_0|double|48 个通用双精度浮点寄存器X: [0..23]  - 双精度浮点输入寄存器的低位范围保留用于现场总线/PLC 接口。X: [24..47] - 双精度浮点输入寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_int16_registers_0|int16_t||
|input_int16_registers0_to_63|std::vector<int16_t>||
|pn_input_int16_registers_256_to_287|std::vector<int16_t>|32 个 PN 专用 int16 寄存器|
|pn_input_int16_registers_288_to_319|std::vector<int16_t>|32 个 PN 专用 int16 寄存器|
|eip_input_int16_registers_320_to_351|std::vector<int16_t>|32 个 EIP 专用 int16 寄存器|
|eip_input_int16_registers_352_to_383|std::vector<int16_t>|32 个 EIP 专用 int16 寄存器|
|R1_speed_slider_mask|double|0 = 不通过该输入修改速度滑块；<br>非 0 = 使用 speed_slider_fraction 设置速度滑块值|
|R1_speed_slider_fraction|double|新的速度滑块值 |
|R1_standard_digital_output_mask|uint32_t|标准数字输出 |
|R1_configurable_digital_output_mask|uint32_t|可配置数字输出|
|R1_standard_digital_output|uint32_t|标准数字输出 |
|R1_configurable_digital_output|uint32_t|可配置数字输出|
|R1_tool_digital_output|uint32_t|工具数字输出。<br>位 0～1 表示输出状态，其余位保留供未来使用|
|R1_standard_analog_output_type|std::vector<int>|输出类型：{0=电流[A]，1=电压[V]}。<br>位 0～1：standard_analog_output_0 | standard_analog_output_1|
|R1_standard_analog_output_mask|uint32_t|标准模拟输出 0（比例值）[0..1] |
|R1_standard_analog_output|std::vector<double>|标准模拟输出 1（比例值）[0..1] |
|R1_debug|uint32_t|内部调试使用 |
|R1_tool_digital_output_mask|uint32_t|工具数字输出掩码|
|R1_rtde_input_max|int||
|R2_speed_slider_mask|double|0 = 不通过该输入修改速度滑块；<br>非 0 = 使用 speed_slider_fraction 设置速度滑块值|
|R2_speed_slider_fraction|double|新的速度滑块值 |
|R2_standard_digital_output_mask|uint32_t|标准数字输出 |
|R2_configurable_digital_output_mask|uint32_t|可配置数字输出|
|R2_standard_digital_output|uint32_t|标准数字输出 |
|R2_configurable_digital_output|uint32_t|可配置数字输出|
|R2_tool_digital_output|uint32_t|工具数字输出。<br>位 0～1 表示输出状态，其余位保留供未来使用|
|R2_standard_analog_output_type|std::vector<int>|输出类型：{0=电流[A]，1=电压[V]}。<br>位 0～1：standard_analog_output_0 | standard_analog_output_1|
|R2_standard_analog_output_mask|uint32_t|标准模拟输出 0（比例值）[0..1] |
|R2_standard_analog_output|std::vector<double>|标准模拟输出 1（比例值）[0..1] |
|R2_debug|uint32_t|内部调试使用 |
|R2_tool_digital_output_mask|uint32_t|工具数字输出掩码|
|R2_rtde_input_max|int||
|R3_speed_slider_mask|double|0 = 不通过该输入修改速度滑块；<br>非 0 = 使用 speed_slider_fraction 设置速度滑块值|
|R3_speed_slider_fraction|double|新的速度滑块值 |
|R3_standard_digital_output_mask|uint32_t|标准数字输出 |
|R3_configurable_digital_output_mask|uint32_t|可配置数字输出|
|R3_standard_digital_output|uint32_t|标准数字输出 |
|R3_configurable_digital_output|uint32_t|可配置数字输出|
|R3_tool_digital_output|uint32_t|工具数字输出。<br>位 0～1 表示输出状态，其余位保留供未来使用|
|R3_standard_analog_output_type|std::vector<int>|输出类型：{0=电流[A]，1=电压[V]}。<br>位 0～1：standard_analog_output_0 | standard_analog_output_1|
|R3_standard_analog_output_mask|uint32_t|标准模拟输出 0（比例值）[0..1] |
|R3_standard_analog_output|std::vector<double>|标准模拟输出 1（比例值）[0..1] |
|R3_debug|uint32_t|内部调试使用 |
|R3_tool_digital_output_mask|uint32_t|工具数字输出掩码|
|R3_rtde_input_max|int||
|R4_speed_slider_mask|double|0 = 不通过该输入修改速度滑块；<br>非 0 = 使用 speed_slider_fraction 设置速度滑块值|
|R4_speed_slider_fraction|double|新的速度滑块值 |
|R4_standard_digital_output_mask|uint32_t|标准数字输出 |
|R4_configurable_digital_output_mask|uint32_t|可配置数字输出|
|R4_standard_digital_output|uint32_t|标准数字输出 |
|R4_configurable_digital_output|uint32_t|可配置数字输出|
|R4_tool_digital_output|uint32_t|工具数字输出。<br>位 0～1 表示输出状态，其余位保留供未来使用|
|R4_standard_analog_output_type|std::vector<int>|输出类型：{0=电流[A]，1=电压[V]}。<br>位 0～1：standard_analog_output_0 | standard_analog_output_1|
|R4_standard_analog_output_mask|uint32_t|标准模拟输出 0（比例值）[0..1] |
|R4_standard_analog_output|std::vector<double>|标准模拟输出 1（比例值）[0..1] |
|R4_debug|uint32_t|内部调试使用 |
|R4_tool_digital_output_mask|uint32_t|工具数字输出掩码|
|R4_rtde_input_max|int||
## 输出菜单
| 名称 | 数据类型 | 说明 |
| :----:| :----: | :----: |
|timestamp|double|自控制器启动以来经过的时间 [s]|
|line_number|int|由 setPlanContext 设置的行号|
|runtime_state|RuntimeState|程序状态|
|output_bit_registers_0_to_63|int64_t|64 个 [000..063] 通用位|
|output_bit_registers_64_to_127|int64_t|64 个 [064..127] 通用位|
|output_int_registers_0|int|48 个通用整数寄存器X: [0..23] - 整数输出寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 整数输出寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|output_float_registers_0|int|48 个通用整数寄存器X: [0..23] - 整数输出寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 整数输出寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|output_double_registers_0|double|48 个通用双精度浮点寄存器X: [0..23] - 双精度浮点输出寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 双精度浮点输出寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_bit_registers_r0_to_63|int64_t|[0..63] 通用位。 该范围的布尔输出寄存器保留用于现场总线/PLC 接口。|
|input_bit_registers_r64_to_127|int64_t|64 个 [64..127] 通用位|
|input_int_registers_r0|int|([0..48]) 共 48 个通用整数寄存器X: [0..23] - 整数输入寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 整数输入寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_float_registers_r0|int|([0..48]) 共 48 个通用整数寄存器X: [0..23] - 整数输入寄存器的低位范围保留用于现场总线/PLC 接口。 X: [24..47] - 整数输入寄存器的高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|input_double_registers_r0|double|([0..48]) 共 48 个通用双精度浮点寄存器。X：[0..23]——低位范围保留用于现场总线/PLC 接口；X：[24..47]——高位范围可供外部 RTDE 客户端使用（例如 aubo_studio 插件）。|
|modbus_signals|std::vector<int>|来自已连接 Modbus 从站的信号|
|modbus_signals_errors|std::vector<int>|来自已连接 Modbus 从站的信号请求状态|
|input_int16_registers_r0|int16_t|input_int16_registers_r0|
|input_int16_registers_0_to_63|std::vector<int16_t>| |
|pn_input_int16_registers256_to_287|std::vector<int16_t>|32 个 PN 专用 int16 寄存器|
|pn_input_int16_registers288_to_319|std::vector<int16_t>|32 个 PN 专用 int16 寄存器|
|eip_input_int16_registers320_to_351|std::vector<int16_t>|32 个 EIP 专用 int16 寄存器|
|eip_input_int16_registers352_to_383|std::vector<int16_t>|32 个 EIP 专用 int16 寄存器|
|runtime_context|std::vector<int>| |
|gripper_status|GripperStatus| |
|axis_actual_positions|std::vector<double>|实际轴位置|
|axis_actual_velocities|std::vector<double>|实际轴速度|
|axis_actual_accelerations|std::vector<double>|实际轴加速度|
|axis_actual_currents|std::vector<double>|实际轴电流|
|R1_message|RobotMsg|来自控制器的机器人消息|
|R1_target_q|std::vector<double>|目标关节位置|
|R1_target_qd|std::vector<double>|目标关节速度|
|R1_target_qdd|std::vector<double>|目标关节加速度|
|R1_target_current|std::vector<double>|目标关节电流|
|R1_target_moment|std::vector<double>|目标关节力矩（转矩）|
|R1_actual_q|std::vector<double>|实际关节位置|
|R1_actual_qd|std::vector<double>|实际关节速度|
|R1_actual_current|std::vector<double>|实际关节电流|
|R1_joint_control_output|std::vector<double>|关节控制电流|
|R1_joint_temperatures|std::vector<double>|各关节温度，单位为摄氏度|
|R1_actual_joint_voltage|std::vector<double>|实际关节电压|
|R1_joint_mode|std::vector<JointStateType>|关节控制模式，详见《Remote Control Via TCP/IP - 16496》|
|R1_actual_execution_time|double|控制器实时线程执行时间|
|R1_robot_mode|RobotModeType|机器人模式，详见《Remote Control Via TCP/IP - 16496》|
|R1_safety_mode|SafetyModeType|安全模式，详见《Remote Control Via TCP/IP - 16496》|
|R1_safety_status|unknown|安全状态|
|R1_robot_status_bits|unknown|位 0～3：是否已上电 | 程序是否正在运行 | 示教按钮是否按下 | 电源按钮是否按下|
|R1_safety_status_bits|unknown|位 0～10：是否为正常模式 | 是否为降级模式 | 是否保护性停止 | 是否为恢复模式 | 是否安全防护停止 | 是否系统急停 | 是否机器人急停 | 是否急停 | 是否违规 | 是否故障 | 是否因安全原因停止|
|R1_speed_scaling|double|轨迹限制器的速度缩放比例|
|R1_target_speed_fraction|double|目标速度比例|
|R1_actual_TCP_pose|std::vector<double>|工具的实际笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R1_actual_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的实际速度|
|R1_actual_TCP_force|std::vector<double>|TCP 处的广义力|
|R1_target_TCP_pose|std::vector<double>|工具的目标笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R1_target_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的目标速度|
|R1_elbow_position|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的位置|
|R1_elbow_velocity|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的速度|
|R1_actual_momentum|std::vector<double>|笛卡尔线动量的范数|
|R1_tcp_force_scalar|std::vector<double>|TCP 力标量 [N]|
|R1_future_path_points|std::vector<std::vector<double>>|获取未来路径的关节点|
|R1_actual_main_voltage|unknown|安全控制板：主电压|
|R1_actual_robot_voltage|unknown|安全控制板：机器人电压（48V）|
|R1_actual_robot_current|unknown|安全控制板：机器人电流|
|R1_joint_torque_sensor|std::vector<double>|关节力矩传感器|
|R1_operationalModeSelectorInput|OperationalModeType|操作模式选择器输入的当前状态|
|R1_threePositionEnablingDeviceInput|unknown||
|R1_masterboard_temperature|unknown||
|R1_standard_digital_input_bits|uint64_t|标准数字输入的当前状态。|
|R1_tool_digital_input_bits|uint64_t|工具数字输入与输出的当前状态。|
|R1_configurable_digital_input_bits|uint64_t|安全输入的当前状态。|
|R1_link_digital_input_bits|uint64_t|链路数字输入的当前状态。|
|R1_standard_digital_output_bits|uint64_t|标准数字输出的当前状态。|
|R1_tool_digital_output_bits|uint64_t|工具数字输入与输出的当前状态。|
|R1_configurable_digital_output_bits|uint64_t|安全输出的当前状态。|
|R1_link_digital_output_bits|uint64_t|链路数字输出的当前状态。|
|R1_standard_analog_input_values|std::vector<double>|标准模拟输入的当前值。|
|R1_tool_analog_input_values|std::vector<double>|工具模拟输入的当前值。|
|R1_standard_analog_output_values|std::vector<double>|标准模拟输出的当前值。|
|R1_tool_analog_output_values|std::vector<double>|工具模拟输出的当前值。|
|R1_is_simulation_enabled|bool||
|R1_collision_level|int||
|R1_master_io_current|unknown|I/O 电流 [A]|
|R1_euromap67_input_bits|unknown|Euromap67 输入位|
|R1_euromap67_output_bits|unknown|Euromap67 输出位|
|R1_euromap67_24V_voltage|unknown|Euromap 24V 电压 [V]|
|R1_euromap67_24V_current|unknown|Euromap 24V 电流 [A]|
|R1_tool_mode|unknown|工具模式，详见《Remote Control Via TCP/IP - 16496》|
|R1_tool_output_mode|unknown|当前输出模式|
|R1_tool_output_voltage|unknown|工具输出电压 [V]|
|R1_tool_output_current|unknown|工具电流 [A]|
|R1_tool_voltage_48V|unknown||
|R1_tool_current|unknown||
|R1_tool_temperature|unknown|工具温度，单位为摄氏度|
|R1_actual_tool_accelerometer|unknown|工具 x、y、z 方向的加速度计数值|
|R1_motion_progress|unknown|轨迹运行进度|
|R1_actual_qdd|unknown|实际关节加速度|
|R1_controlbox_humidity|double|控制箱湿度|
|R1_actual_tool_pose|std::vector<double>|工具的实际笛卡尔坐标（不包含 TCP 偏置）|
|R1_rtde_output_max|int||
|R1_actual_TCP_force_sensor|std::vector<double>|TCP 力传感器|
|R1_fc_cond_fullfiled|bool||
|R1_actual_payload|Payload|实际负载|
|R1_tool_button_status|bool|工具按钮状态|
|R1_handle_status|uint64_t|手柄按钮 I/O 状态|
|R1_enc_tick_count|std::vector<int>|编码器计数值|
|R1_weave_direction|int|获取当前摆动轨迹方向|
|R1_handle_dev_state|int|获取手柄设备状态|
|R1_handle_dev_type|int|获取手柄设备类型|
|R2_message|RobotMsg|来自控制器的机器人消息|
|R2_target_q|std::vector<double>|目标关节位置|
|R2_target_qd|std::vector<double>|目标关节速度|
|R2_target_qdd|std::vector<double>|目标关节加速度|
|R2_target_current|std::vector<double>|目标关节电流|
|R2_target_moment|std::vector<double>|目标关节力矩（转矩）|
|R2_actual_q|std::vector<double>|实际关节位置|
|R2_actual_qd|std::vector<double>|实际关节速度|
|R2_actual_current|std::vector<double>|实际关节电流|
|R2_joint_control_output|std::vector<double>|关节控制电流|
|R2_joint_temperatures|std::vector<double>|各关节温度，单位为摄氏度|
|R2_actual_joint_voltage|std::vector<double>|实际关节电压|
|R2_joint_mode|std::vector<JointStateType>|关节控制模式，详见《Remote Control Via TCP/IP - 16496》|
|R2_actual_execution_time|double|控制器实时线程执行时间|
|R2_robot_mode|RobotModeType|机器人模式，详见《Remote Control Via TCP/IP - 16496》|
|R2_safety_mode|SafetyModeType|安全模式，详见《Remote Control Via TCP/IP - 16496》|
|R2_safety_status|unknown|安全状态|
|R2_robot_status_bits|unknown|位 0～3：是否已上电 | 程序是否正在运行 | 示教按钮是否按下 | 电源按钮是否按下|
|R2_safety_status_bits|unknown|位 0～10：是否为正常模式 | 是否为降级模式 | 是否保护性停止 | 是否为恢复模式 | 是否安全防护停止 | 是否系统急停 | 是否机器人急停 | 是否急停 | 是否违规 | 是否故障 | 是否因安全原因停止|
|R2_speed_scaling|double|轨迹限制器的速度缩放比例|
|R2_target_speed_fraction|double|目标速度比例|
|R2_actual_TCP_pose|std::vector<double>|工具的实际笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R2_actual_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的实际速度|
|R2_actual_TCP_force|std::vector<double>|TCP 处的广义力|
|R2_target_TCP_pose|std::vector<double>|工具的目标笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R2_target_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的目标速度|
|R2_elbow_position|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的位置|
|R2_elbow_velocity|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的速度|
|R2_actual_momentum|std::vector<double>|笛卡尔线动量的范数|
|R2_tcp_force_scalar|std::vector<double>|TCP 力标量 [N]|
|R2_future_path_points|std::vector<std::vector<double>>|获取未来路径的关节点|
|R2_actual_main_voltage|unknown|安全控制板：主电压|
|R2_actual_robot_voltage|unknown|安全控制板：机器人电压（48V）|
|R2_actual_robot_current|unknown|安全控制板：机器人电流|
|R2_joint_torque_sensor|std::vector<double>|关节力矩传感器|
|R2_operationalModeSelectorInput|OperationalModeType|操作模式选择器输入的当前状态|
|R2_threePositionEnablingDeviceInput|unknown||
|R2_masterboard_temperature|unknown||
|R2_standard_digital_input_bits|uint64_t|标准数字输入的当前状态。|
|R2_tool_digital_input_bits|uint64_t|工具数字输入与输出的当前状态。|
|R2_configurable_digital_input_bits|uint64_t|安全输入的当前状态。|
|R2_link_digital_input_bits|uint64_t|链路数字输入的当前状态。|
|R2_standard_digital_output_bits|uint64_t|标准数字输出的当前状态。|
|R2_tool_digital_output_bits|uint64_t|工具数字输入与输出的当前状态。|
|R2_configurable_digital_output_bits|uint64_t|安全输出的当前状态。|
|R2_link_digital_output_bits|uint64_t|链路数字输出的当前状态。|
|R2_standard_analog_input_values|std::vector<double>|标准模拟输入的当前值。|
|R2_tool_analog_input_values|std::vector<double>|工具模拟输入的当前值。|
|R2_standard_analog_output_values|std::vector<double>|标准模拟输出的当前值。|
|R2_tool_analog_output_values|std::vector<double>|工具模拟输出的当前值。|
|R2_is_simulation_enabled|bool||
|R2_collision_level|int||
|R2_master_io_current|unknown|I/O 电流 [A]|
|R2_euromap67_input_bits|unknown|Euromap67 输入位|
|R2_euromap67_output_bits|unknown|Euromap67 输出位|
|R2_euromap67_24V_voltage|unknown|Euromap 24V 电压 [V]|
|R2_euromap67_24V_current|unknown|Euromap 24V 电流 [A]|
|R2_tool_mode|unknown|工具模式，详见《Remote Control Via TCP/IP - 16496》|
|R2_tool_output_mode|unknown|当前输出模式|
|R2_tool_output_voltage|unknown|工具输出电压 [V]|
|R2_tool_output_current|unknown|工具电流 [A]|
|R2_tool_voltage_48V|unknown||
|R2_tool_current|unknown||
|R2_tool_temperature|unknown|工具温度，单位为摄氏度|
|R2_actual_tool_accelerometer|unknown|工具 x、y、z 方向的加速度计数值|
|R2_motion_progress|unknown|轨迹运行进度|
|R2_actual_qdd|unknown|实际关节加速度|
|R2_controlbox_humidity|double|控制箱湿度|
|R2_actual_tool_pose|std::vector<double>|工具的实际笛卡尔坐标（不包含 TCP 偏置）|
|R2_rtde_output_max|int||
|R2_actual_TCP_force_sensor|std::vector<double>|TCP 力传感器|
|R2_fc_cond_fullfiled|bool||
|R2_actual_payload|Payload|实际负载|
|R2_tool_button_status|bool|工具按钮状态|
|R2_handle_status|uint64_t|手柄按钮 I/O 状态|
|R2_enc_tick_count|std::vector<int>|编码器计数值|
|R2_weave_direction|int|获取当前摆动轨迹方向|
|R2_handle_dev_state|int|获取手柄设备状态|
|R2_handle_dev_type|int|获取手柄设备类型|
|R3_message|RobotMsg|来自控制器的机器人消息|
|R3_target_q|std::vector<double>|目标关节位置|
|R3_target_qd|std::vector<double>|目标关节速度|
|R3_target_qdd|std::vector<double>|目标关节加速度|
|R3_target_current|std::vector<double>|目标关节电流|
|R3_target_moment|std::vector<double>|目标关节力矩（转矩）|
|R3_actual_q|std::vector<double>|实际关节位置|
|R3_actual_qd|std::vector<double>|实际关节速度|
|R3_actual_current|std::vector<double>|实际关节电流|
|R3_joint_control_output|std::vector<double>|关节控制电流|
|R3_joint_temperatures|std::vector<double>|各关节温度，单位为摄氏度|
|R3_actual_joint_voltage|std::vector<double>|实际关节电压|
|R3_joint_mode|std::vector<JointStateType>|关节控制模式，详见《Remote Control Via TCP/IP - 16496》|
|R3_actual_execution_time|double|控制器实时线程执行时间|
|R3_robot_mode|RobotModeType|机器人模式，详见《Remote Control Via TCP/IP - 16496》|
|R3_safety_mode|SafetyModeType|安全模式，详见《Remote Control Via TCP/IP - 16496》|
|R3_safety_status|unknown|安全状态|
|R3_robot_status_bits|unknown|位 0～3：是否已上电 | 程序是否正在运行 | 示教按钮是否按下 | 电源按钮是否按下|
|R3_safety_status_bits|unknown|位 0～10：是否为正常模式 | 是否为降级模式 | 是否保护性停止 | 是否为恢复模式 | 是否安全防护停止 | 是否系统急停 | 是否机器人急停 | 是否急停 | 是否违规 | 是否故障 | 是否因安全原因停止|
|R3_speed_scaling|double|轨迹限制器的速度缩放比例|
|R3_target_speed_fraction|double|目标速度比例|
|R3_actual_TCP_pose|std::vector<double>|工具的实际笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R3_actual_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的实际速度|
|R3_actual_TCP_force|std::vector<double>|TCP 处的广义力|
|R3_target_TCP_pose|std::vector<double>|工具的目标笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R3_target_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的目标速度|
|R3_elbow_position|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的位置|
|R3_elbow_velocity|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的速度|
|R3_actual_momentum|std::vector<double>|笛卡尔线动量的范数|
|R3_tcp_force_scalar|std::vector<double>|TCP 力标量 [N]|
|R3_future_path_points|std::vector<std::vector<double>>|获取未来路径的关节点|
|R3_actual_main_voltage|unknown|安全控制板：主电压|
|R3_actual_robot_voltage|unknown|安全控制板：机器人电压（48V）|
|R3_actual_robot_current|unknown|安全控制板：机器人电流|
|R3_joint_torque_sensor|std::vector<double>|关节力矩传感器|
|R3_operationalModeSelectorInput|OperationalModeType|操作模式选择器输入的当前状态|
|R3_threePositionEnablingDeviceInput|unknown||
|R3_masterboard_temperature|unknown||
|R3_standard_digital_input_bits|uint64_t|标准数字输入的当前状态。|
|R3_tool_digital_input_bits|uint64_t|工具数字输入与输出的当前状态。|
|R3_configurable_digital_input_bits|uint64_t|安全输入的当前状态。|
|R3_link_digital_input_bits|uint64_t|链路数字输入的当前状态。|
|R3_standard_digital_output_bits|uint64_t|标准数字输出的当前状态。|
|R3_tool_digital_output_bits|uint64_t|工具数字输入与输出的当前状态。|
|R3_configurable_digital_output_bits|uint64_t|安全输出的当前状态。|
|R3_link_digital_output_bits|uint64_t|链路数字输出的当前状态。|
|R3_standard_analog_input_values|std::vector<double>|标准模拟输入的当前值。|
|R3_tool_analog_input_values|std::vector<double>|工具模拟输入的当前值。|
|R3_standard_analog_output_values|std::vector<double>|标准模拟输出的当前值。|
|R3_tool_analog_output_values|std::vector<double>|工具模拟输出的当前值。|
|R3_is_simulation_enabled|bool||
|R3_collision_level|int||
|R3_master_io_current|unknown|I/O 电流 [A]|
|R3_euromap67_input_bits|unknown|Euromap67 输入位|
|R3_euromap67_output_bits|unknown|Euromap67 输出位|
|R3_euromap67_24V_voltage|unknown|Euromap 24V 电压 [V]|
|R3_euromap67_24V_current|unknown|Euromap 24V 电流 [A]|
|R3_tool_mode|unknown|工具模式，详见《Remote Control Via TCP/IP - 16496》|
|R3_tool_output_mode|unknown|当前输出模式|
|R3_tool_output_voltage|unknown|工具输出电压 [V]|
|R3_tool_output_current|unknown|工具电流 [A]|
|R3_tool_voltage_48V|unknown||
|R3_tool_current|unknown||
|R3_tool_temperature|unknown|工具温度，单位为摄氏度|
|R3_actual_tool_accelerometer|unknown|工具 x、y、z 方向的加速度计数值|
|R3_motion_progress|unknown|轨迹运行进度|
|R3_actual_qdd|unknown|实际关节加速度|
|R3_controlbox_humidity|double|控制箱湿度|
|R3_actual_tool_pose|std::vector<double>|工具的实际笛卡尔坐标（不包含 TCP 偏置）|
|R3_rtde_output_max|int||
|R3_actual_TCP_force_sensor|std::vector<double>|TCP 力传感器|
|R3_fc_cond_fullfiled|bool||
|R3_actual_payload|Payload|实际负载|
|R3_tool_button_status|bool|工具按钮状态|
|R3_handle_status|uint64_t|手柄按钮 I/O 状态|
|R3_enc_tick_count|std::vector<int>|编码器计数值|
|R3_weave_direction|int|获取当前摆动轨迹方向|
|R3_handle_dev_state|int|获取手柄设备状态|
|R3_handle_dev_type|int|获取手柄设备类型|
|R4_message|RobotMsg|来自控制器的机器人消息|
|R4_target_q|std::vector<double>|目标关节位置|
|R4_target_qd|std::vector<double>|目标关节速度|
|R4_target_qdd|std::vector<double>|目标关节加速度|
|R4_target_current|std::vector<double>|目标关节电流|
|R4_target_moment|std::vector<double>|目标关节力矩（转矩）|
|R4_actual_q|std::vector<double>|实际关节位置|
|R4_actual_qd|std::vector<double>|实际关节速度|
|R4_actual_current|std::vector<double>|实际关节电流|
|R4_joint_control_output|std::vector<double>|关节控制电流|
|R4_joint_temperatures|std::vector<double>|各关节温度，单位为摄氏度|
|R4_actual_joint_voltage|std::vector<double>|实际关节电压|
|R4_joint_mode|std::vector<JointStateType>|关节控制模式，详见《Remote Control Via TCP/IP - 16496》|
|R4_actual_execution_time|double|控制器实时线程执行时间|
|R4_robot_mode|RobotModeType|机器人模式，详见《Remote Control Via TCP/IP - 16496》|
|R4_safety_mode|SafetyModeType|安全模式，详见《Remote Control Via TCP/IP - 16496》|
|R4_safety_status|unknown|安全状态|
|R4_robot_status_bits|unknown|位 0～3：是否已上电 | 程序是否正在运行 | 示教按钮是否按下 | 电源按钮是否按下|
|R4_safety_status_bits|unknown|位 0～10：是否为正常模式 | 是否为降级模式 | 是否保护性停止 | 是否为恢复模式 | 是否安全防护停止 | 是否系统急停 | 是否机器人急停 | 是否急停 | 是否违规 | 是否故障 | 是否因安全原因停止|
|R4_speed_scaling|double|轨迹限制器的速度缩放比例|
|R4_target_speed_fraction|double|目标速度比例|
|R4_actual_TCP_pose|std::vector<double>|工具的实际笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R4_actual_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的实际速度|
|R4_actual_TCP_force|std::vector<double>|TCP 处的广义力|
|R4_target_TCP_pose|std::vector<double>|工具的目标笛卡尔坐标：(x, y, z, rx, ry, rz)，其中 rx、ry、rz 以旋转向量表示工具姿态|
|R4_target_TCP_speed|std::vector<double>|工具在笛卡尔坐标系下的目标速度|
|R4_elbow_position|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的位置|
|R4_elbow_velocity|std::vector<double>|机器人肘部在基座笛卡尔坐标系中的速度|
|R4_actual_momentum|std::vector<double>|笛卡尔线动量的范数|
|R4_tcp_force_scalar|std::vector<double>|TCP 力标量 [N]|
|R4_future_path_points|std::vector<std::vector<double>>|获取未来路径的关节点|
|R4_actual_main_voltage|unknown|安全控制板：主电压|
|R4_actual_robot_voltage|unknown|安全控制板：机器人电压（48V）|
|R4_actual_robot_current|unknown|安全控制板：机器人电流|
|R4_joint_torque_sensor|std::vector<double>|关节力矩传感器|
|R4_operationalModeSelectorInput|OperationalModeType|操作模式选择器输入的当前状态|
|R4_threePositionEnablingDeviceInput|unknown||
|R4_masterboard_temperature|unknown||
|R4_standard_digital_input_bits|uint64_t|标准数字输入的当前状态。|
|R4_tool_digital_input_bits|uint64_t|工具数字输入与输出的当前状态。|
|R4_configurable_digital_input_bits|uint64_t|安全输入的当前状态。|
|R4_link_digital_input_bits|uint64_t|链路数字输入的当前状态。|
|R4_standard_digital_output_bits|uint64_t|标准数字输出的当前状态。|
|R4_tool_digital_output_bits|uint64_t|工具数字输入与输出的当前状态。|
|R4_configurable_digital_output_bits|uint64_t|安全输出的当前状态。|
|R4_link_digital_output_bits|uint64_t|链路数字输出的当前状态。|
|R4_standard_analog_input_values|std::vector<double>|标准模拟输入的当前值。|
|R4_tool_analog_input_values|std::vector<double>|工具模拟输入的当前值。|
|R4_standard_analog_output_values|std::vector<double>|标准模拟输出的当前值。|
|R4_tool_analog_output_values|std::vector<double>|工具模拟输出的当前值。|
|R4_is_simulation_enabled|bool||
|R4_collision_level|int||
|R4_master_io_current|unknown|I/O 电流 [A]|
|R4_euromap67_input_bits|unknown|Euromap67 输入位|
|R4_euromap67_output_bits|unknown|Euromap67 输出位|
|R4_euromap67_24V_voltage|unknown|Euromap 24V 电压 [V]|
|R4_euromap67_24V_current|unknown|Euromap 24V 电流 [A]|
|R4_tool_mode|unknown|工具模式，详见《Remote Control Via TCP/IP - 16496》|
|R4_tool_output_mode|unknown|当前输出模式|
|R4_tool_output_voltage|unknown|工具输出电压 [V]|
|R4_tool_output_current|unknown|工具电流 [A]|
|R4_tool_voltage_48V|unknown||
|R4_tool_current|unknown||
|R4_tool_temperature|unknown|工具温度，单位为摄氏度|
|R4_actual_tool_accelerometer|unknown|工具 x、y、z 方向的加速度计数值|
|R4_motion_progress|unknown|轨迹运行进度|
|R4_actual_qdd|unknown|实际关节加速度|
|R4_controlbox_humidity|double|控制箱湿度|
|R4_actual_tool_pose|std::vector<double>|工具的实际笛卡尔坐标（不包含 TCP 偏置）|
|R4_rtde_output_max|int||
|R4_actual_TCP_force_sensor|std::vector<double>|TCP 力传感器|
|R4_fc_cond_fullfiled|bool||
|R4_actual_payload|Payload|实际负载|
|R4_tool_button_status|bool|工具按钮状态|
|R4_handle_status|uint64_t|手柄按钮 I/O 状态|
|R4_enc_tick_count|std::vector<int>|编码器计数值|
|R4_weave_direction|int|获取当前摆动轨迹方向|
|R4_handle_dev_state|int|获取手柄设备状态|
|R4_handle_dev_type|int|获取手柄设备类型|
