/** @file  hal_error.h
 *  @brief 定义硬件抽象层的错误码
 */
#ifndef AUBO_SDK_HAL_ERROR_H
#define AUBO_SDK_HAL_ERROR_H

// 缩写说明
// JNT: joint
// PDL: pedstral
// TP: teach pendant
// COMM: communication
// ENC: encoder
// CURR: current
// POS: position
// PKG: package
// PROG: program

// clang-format off
#define JOINT_ERRORS \
    _D(JOINT_ERR_OVER_CURRENET,  10001, "joint" _PH1_ " error: over current", "(a) Check for short circuit. (b) Do a Complete rebooting sequence. (c) If this happens more than two times in a row, replace joint") \
    _D(JOINT_ERR_OVER_VOLTAGE,  10002, "joint" _PH1_ " error: over voltage", "(a) Do a Complete rebooting sequence. (b) Check 48 V Power supply, current distributer, energy eater and Control Board for issues") \
    _D(JOINT_ERR_LOW_VOLTAGE,  10003, "joint" _PH1_ " error: low voltage", "(a) Do a Complete rebooting sequence. (b) Check for short circuit in robot arm. (c) Check 48 V Power supply, current distributer, energy eater and Control Board for issues") \
    _D(JOINT_ERR_OVER_TEMP,  10004, "joint" _PH1_ " error: over temperature", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_HALL,  10005, "joint" _PH1_ " error: hall", "suggest...") \
    _D(JOINT_ERR_ENCODER,  10006, "joint" _PH1_ " error: encoder", "Check encoder connections") \
    _D(JOINT_ERR_ABS_ENCODER,  10007, "joint" _PH1_ " error: abs encoder", "suggest...") \
    _D(JOINT_ERR_Q_CURRENT,  10008, "joint" _PH1_ " error: detect current", "suggest...") \
    _D(JOINT_ERR_ENC_POLL,  10009, "joint" _PH1_ " error: encoder pollustion", "suggest...") \
    _D(JOINT_ERR_ENC_Z_SIGNAL,  10010, "joint" _PH1_ " error: enocder z signal", "suggest...") \
    _D(JOINT_ERR_ENC_CAL,  10011, "joint" _PH1_ " error: encoder calibrate", "suggest...") \
    _D(JOINT_ERR_IMU_SENS, 10012, "joint" _PH1_ " error: IMU sensor", "suggest...") \
    _D(JOINT_ERR_TEMP_SENS, 10013, "joint" _PH1_ " error: TEMP sensor", "suggest...") \
    _D(JOINT_ERR_CAN_BUS, 10014, "joint" _PH1_ " error: CAN bus error", "suggest...") \
    _D(JOINT_ERR_SYS_CUR, 10015, "joint" _PH1_ " error: system current error", "suggest...") \
    _D(JOINT_ERR_SYS_POS, 10016, "joint" _PH1_ " error: system position error","suggest...") \
    _D(JOINT_ERR_OVER_SP, 10017, "joint" _PH1_ " error: over speed","suggest...") \
    _D(JOINT_ERR_OVER_ACC, 10018, "joint" _PH1_ " error: over accelerate", "suggest...") \
    _D(JOINT_ERR_TRACE, 10019, "joint" _PH1_ " error: trace accuracy", "suggest...") \
    _D(JOINT_ERR_TAG_POS_OVER, 10020, "joint" _PH1_ " error: target position out of range", "suggest...") \
    _D(JOINT_ERR_TAG_SP_OVER, 10021, "joint" _PH1_ " error: target speed out of range", "suggest...") \
    _D(JOINT_ERR_COLLISION, 10022, "joint" _PH1_ " error: collision", "suggest...") \
    _D(JOINT_ERR_COMMON, 10023, "joint" _PH1_ " error: unkown error. Check communication with joint.", "suggest...") \
    _D(JOINT_ERR_SWITCH_SERVO_MODE, 10024, "joint" _PH1_ " error: switch servo mode timeout.", "suggest...") \
    _D(JOINT_ERR_MOTOR_STUCK, 10025, "joint" _PH1_ " error: motor stucked.", "suggest...") \
    _D(JOINT_ERR_REDUCER_OVER_TEMP,  10026, "joint" _PH1_ " error: reducer over temperature", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_REDUCER_NTC,  10027, "joint" _PH1_ " error: reducer TEMP sensor failure", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_ABS_MULTITURN,  10028, "joint" _PH1_ " error: absolute encoder multiturn error", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_ADC_ZERO_OFFSET,  10029, "joint" _PH1_ " error: ADC zero offset failure", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_SHORT_CIRCUIT,  10030, "joint" _PH1_ " error: short circuit", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_PHASE_LOST,  10031, "joint" _PH1_ " error: motor phase lost", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_BRAKE,  10032, "joint" _PH1_ " error: brake failure", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_FIRMWARE_UPDATE,  10033, "joint" _PH1_ " error: firmware update failure", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_BATTERY_LOW,  10034, "joint" _PH1_ " error: battery low", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_PHASE_ALIGN,  10035, "joint" _PH1_ " error: phase align", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_CAN_HW_FAULT,  10036, "joint" _PH1_ " error: CAN bus hw fault", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_POS_DISCONTINUOUS,  10037, "joint" _PH1_ " error: target position discontinuous", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_POS_INIT,  10038, "joint" _PH1_ " error: position initiallization failure", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_TORQUE_SENSOR,  10039, "joint" _PH1_ " error: torqure sensor failure", "(a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence") \
    _D(JOINT_ERR_OFFLINE,  10040, "joint" _PH1_ " error: joint may be offline", "(a) Check joint's hardware. (b) Check joint's id.") \
    _D(JOINT_ERR_BOOTLOADER,  10041, "joint" _PH1_ " error: The joint is in bootloader mode. Retry firmware update. ", "suggest...") \
    _D(JOINT_ERR_SLAVE_OFFLINE,  10042, "slave joint" _PH1_ " error: slave joint may be offline", "(a) Check slave joint's hardware. (b) Check slave joint's id.") \
    _D(JOINT_ERR_SLAVE_BOOTLOADER,  10043, "slave joint" _PH1_ " error: The slave joint is in bootloader mode. Retry firmware update. ", "suggest...") \
    _D(JOINT_ERR_ETHERCAT_BUS, 10044, "joint" _PH1_ " error: ETHERCAT bus error", "suggest...")

// Joint extended error codes (ex_err_code), starting from 10401
#define JOINT_EX_ERRORS \
    _D(EX_JOINT_EC_SHORT_CIRCUIT, 10401, "joint" _PH1_ " error: short circuit protection", "suggest...", 0x2130) \
    _D(EX_JOINT_EC_SHORT_CURRENT, 10402, "joint" _PH1_ " error: over current", "suggest...", 0x2310) \
    _D(EX_JOINT_EC_PHASEA_CURRENT, 10403, "joint" _PH1_ " error: phase A over current", "suggest...", 0x2311) \
    _D(EX_JOINT_EC_PHASEB_CURRENT, 10404, "joint" _PH1_ " error: phase B over current", "suggest...", 0x2312) \
    _D(EX_JOINT_EC_PHASEC_CURRENT, 10405, "joint" _PH1_ " error: phase C over current", "suggest...", 0x2313) \
    _D(EX_JOINT_EC_PHASE_CURRENT, 10406, "joint" _PH1_ " error: phase over current", "suggest...", 0x2314) \
    _D(EX_JOINT_EC_MOTOR_PHASE_LOSE, 10407, "joint" _PH1_ " error: motor phase loss", "suggest...", 0x3331) \
    _D(EX_JOINT_EC_BUS_OVER_VOLTAGE, 10408, "joint" _PH1_ " error: bus over voltage", "suggest...", 0x3210) \
    _D(EX_JOINT_EC_BUS_LOW_VOLTAGE, 10409, "joint" _PH1_ " error: bus under voltage", "suggest...", 0x3220) \
    _D(EX_JOINT_EC_OVERLOAD, 10410, "joint" _PH1_ " error: overload", "suggest...", 0x3230) \
    _D(EX_JOINT_EC_IPM_OVER_TEMP, 10411, "joint" _PH1_ " error: IPM over temperature", "suggest...", 0x4310) \
    _D(EX_JOINT_EC_REDUCER_OVER_TEMP, 10412, "joint" _PH1_ " error: reducer over temperature", "suggest...", 0x4311) \
    _D(EX_JOINT_EC_ADC_ZERO_OFFSET, 10413, "joint" _PH1_ " error: ADC zero offset", "suggest...", 0x5101) \
    _D(EX_JOINT_EC_REDUCER_NTC, 10414, "joint" _PH1_ " error: reducer NTC fault", "suggest...", 0x5102) \
    _D(EX_JOINT_EC_IPM_NTC, 10415, "joint" _PH1_ " error: IPM NTC fault", "suggest...", 0x5103) \
    _D(EX_JOINT_EC_TORQUE_SENSOR, 10416, "joint" _PH1_ " error: torque sensor fault", "suggest...", 0x5104) \
    _D(EX_JOINT_EC_TORQUE_SENSOR_COMM, 10417, "joint" _PH1_ " error: torque sensor communication fault", "suggest...", 0x5105) \
    _D(EX_JOINT_EC_MOTOR_ABS_ENC_COMM, 10418, "joint" _PH1_ " error: motor-side absolute encoder communication fault", "suggest...", 0x5201) \
    _D(EX_JOINT_EC_REDUCER_ABS_ENC_COMM, 10419, "joint" _PH1_ " error: reducer-side absolute encoder communication fault", "suggest...", 0x5202) \
    _D(EX_JOINT_EC_REDUCER_ABS_ENC_DATA, 10420, "joint" _PH1_ " error: reducer-side absolute encoder data channel disabled warning", "suggest...", 0x5203) \
    _D(EX_JOINT_EC_REDUCER_ABS_ENC_CMD, 10421, "joint" _PH1_ " error: reducer-side absolute encoder command invalid warning", "suggest...", 0x5204) \
    _D(EX_JOINT_EC_REDUCER_ABS_ENC_ERR, 10422, "joint" _PH1_ " error: reducer-side absolute encoder fault", "suggest...", 0x5205) \
    _D(EX_JOINT_EC_REDUCER_ABS_ENC_WARNING, 10423, "joint" _PH1_ " error: reducer-side absolute encoder warning", "suggest...", 0x5206) \
    _D(EX_JOINT_EC_BRAKE, 10424, "joint" _PH1_ " error: brake fault", "suggest...", 0x5301) \
    _D(EX_JOINT_EC_COMM_HWL, 10425, "joint" _PH1_ " error: communication hardware layer error", "suggest...", 0x5302) \
    _D(EX_JOINT_EC_FIRMWARE_UPDATE, 10426, "joint" _PH1_ " error: firmware update failed", "suggest...", 0x6100) \
    _D(EX_JOINT_EC_FLASH_OP, 10427, "joint" _PH1_ " error: flash operation failed", "suggest...", 0x6101) \
    _D(EX_JOINT_EC_MU_SAVE, 10428, "joint" _PH1_ " error: multi-turn data error", "suggest...", 0x6102) \
    _D(EX_JOINT_EC_DEMADATA_LOST, 10429, "joint" _PH1_ " error: calibration zero point data lost", "suggest...", 0x6103) \
    _D(EX_JOINT_EC_PARAMETER, 10430, "joint" _PH1_ " error: parameter error", "suggest...", 0x6200) \
    _D(EX_JOINT_EC_UVW_LOGIC, 10431, "joint" _PH1_ " error: hall signal fault", "suggest...", 0x7001) \
    _D(EX_JOINT_EC_UVW_ABZ, 10432, "joint" _PH1_ " error: incremental encoder fault", "suggest...", 0x7002) \
    _D(EX_JOINT_EC_ENC_Z_LOST, 10433, "joint" _PH1_ " error: encoder Z signal lost", "suggest...", 0x7305) \
    _D(EX_JOINT_EC_ENC_POLLUTE, 10434, "joint" _PH1_ " error: encoder pollution", "suggest...", 0x7004) \
    _D(EX_JOINT_EC_ENC_CALI, 10435, "joint" _PH1_ " error: encoder calibration failed", "suggest...", 0x7005) \
    _D(EX_JOINT_EC_MT_ABS_DATA, 10436, "joint" _PH1_ " error: multi-turn absolute data error", "suggest...", 0x7006) \
    _D(EX_JOINT_EC_ENC_TYPE_INFO, 10437, "joint" _PH1_ " error: encoder type identified and saved", "suggest...", 0x7007) \
    _D(EX_JOINT_EC_ENC_TYPE_ERROR, 10438, "joint" _PH1_ " error: encoder type error", "suggest...", 0x7008) \
    _D(EX_JOINT_EC_ENC_VERIFY, 10439, "joint" _PH1_ " error: encoder verification failed", "suggest...", 0x7009) \
    _D(EX_JOINT_EC_DUAL_ENC_ERROR, 10440, "joint" _PH1_ " error: dual encoder deviation too large", "suggest...", 0x700A) \
    _D(EX_JOINT_EC_DUAL_ENC_EANGLE, 10441, "joint" _PH1_ " error: dual encoder electrical angle deviation too large", "suggest...", 0x700B) \
    _D(EX_JOINT_EC_OBJECT_DICT_ERROR, 10442, "joint" _PH1_ " error: object dictionary data error", "suggest...", 0x700C) \
    _D(EX_JOINT_EC_MOTOR_STALL, 10443, "joint" _PH1_ " error: motor stall protection", "suggest...", 0x7121) \
    _D(EX_JOINT_EC_ABS_ENC_LOW_VOLT, 10444, "joint" _PH1_ " error: absolute encoder low voltage", "suggest...", 0x7385) \
    _D(EX_JOINT_EC_MT_BATTERY_LOW, 10445, "joint" _PH1_ " error: multi-turn battery low voltage", "suggest...", 0x7386) \
    _D(EX_JOINT_EC_POS_CMD, 10446, "joint" _PH1_ " error: position command discontinuous", "suggest...", 0x8001) \
    _D(EX_JOINT_EC_POS_OVER_LIMIT, 10447, "joint" _PH1_ " error: position over limit", "suggest...", 0x8002) \
    _D(EX_JOINT_EC_COMM_PROTO, 10448, "joint" _PH1_ " error: communication protocol layer error", "suggest...", 0x8003) \
    _D(EX_JOINT_EC_GRAVITY_PARA_WARNING, 10449, "joint" _PH1_ " error: gravity compensation parameter invalid", "suggest...", 0x8004) \
    _D(EX_JOINT_EC_GRAVITY_COMPENSATE_ERROR, 10450, "joint" _PH1_ " error: gravity compensation value sudden change", "suggest...", 0x8005) \
    _D(EX_JOINT_EC_LOW_RIGIDITY, 10451, "joint" _PH1_ " error: collision soft float abnormal", "suggest...", 0x8006) \
    _D(EX_JOINT_EC_POS_CMD_WARNING, 10452, "joint" _PH1_ " error: position command unchanged during motion", "suggest...", 0x8007) \
    _D(EX_JOINT_EC_SLAVE_COMM, 10453, "joint" _PH1_ " error: master-slave MCU communication fault", "suggest...", 0x8008) \
    _D(EX_JOINT_EC_POS_ERR, 10454, "joint" _PH1_ " error: position following error too large", "suggest...", 0x8009) \
    _D(EX_JOINT_EC_DUAL_POS_ERR, 10455, "joint" _PH1_ " error: dual servo position sync error too large", "suggest...", 0x800A) \
    _D(EX_JOINT_EC_DUAL_COMM_ERR, 10456, "joint" _PH1_ " error: dual servo communication", "suggest...", 0x800B) \
    _D(EX_JOINT_EC_CAN_BUSOFF, 10457, "joint" _PH1_ " error: CAN bus-off warning", "suggest...", 0x800C) \
    _D(EX_JOINT_EC_SYNC_SNAKE, 10458, "joint" _PH1_ " error: sync frame jitter warning", "suggest...", 0x800D) \
    _D(EX_JOINT_EC_SYNC_DISCON, 10459, "joint" _PH1_ " error: sync frame discontinuous warning", "suggest...", 0x800E) \
    _D(EX_JOINT_EC_RPDO_LOST, 10460, "joint" _PH1_ " error: RPDO lost warning", "suggest...", 0x800F) \
    _D(EX_JOINT_EC_RPDO_MANY, 10461, "joint" _PH1_ " error: multiple RPDO in one sync cycle warning", "suggest...", 0x8010) \
    _D(EX_JOINT_EC_GRAVITY_LOST, 10462, "joint" _PH1_ " error: gravity compensation value lost", "suggest...", 0x8011) \
    _D(EX_JOINT_EC_MAININT_TIME_WARN, 10463, "joint" _PH1_ " error: servo main interrupt runtime warning", "suggest...", 0x8012) \
    _D(EX_JOINT_EC_MAININT_TIME_ERROR, 10464, "joint" _PH1_ " error: servo main interrupt runtime error", "suggest...", 0x8013) \
    _D(EX_JOINT_EC_SPDINT_TIME_WARN, 10465, "joint" _PH1_ " error: servo speed loop interrupt runtime warning", "suggest...", 0x8014) \
    _D(EX_JOINT_EC_SPDINT_TIME_ERROR, 10466, "joint" _PH1_ " error: servo speed loop interrupt runtime error", "suggest...", 0x8015) \
    _D(EX_JOINT_EC_POS_CMD_JUMP_WARNING, 10467, "joint" _PH1_ " error: position command sudden jump during motion", "suggest...", 0x8016) \
    _D(EX_JOINT_EC_SYNC_TIMCOARSE, 10468, "joint" _PH1_ " error: clock sync compensation status", "suggest...", 0x8017) \
    _D(EX_JOINT_EC_ENC_Z_CNTS_ERR, 10469, "joint" _PH1_ " error: incremental encoder Z signal or CNTS abnormal", "suggest...", 0x8018) \
    _D(EX_JOINT_EC_SLAVE_OVER_CURRENT, 10470, "joint" _PH1_ " error: slave MCU detected motor phase current over safe threshold", "suggest...", 0x8019) \
    _D(EX_JOINT_EC_SLAVE_OVER_VOLTAGE, 10471, "joint" _PH1_ " error: slave MCU detected DC bus voltage over upper threshold", "suggest...", 0x801A) \
    _D(EX_JOINT_EC_SLAVE_UNDER_VOLTAGE, 10472, "joint" _PH1_ " error: slave MCU detected DC bus voltage below lower threshold", "suggest...", 0x801B) \
    _D(EX_JOINT_EC_SLAVE_POS_ERR, 10473, "joint" _PH1_ " error: slave MCU detected master-slave motor position deviation out of range", "suggest...", 0x801C) \
    _D(EX_JOINT_EC_SLAVE_SPEED_ERR, 10474, "joint" _PH1_ " error: slave MCU detected master-slave motor speed deviation out of range", "suggest...", 0x801D) \
    _D(EX_JOINT_EC_SLAVE_TRACE_ERR, 10475, "joint" _PH1_ " error: slave MCU detected position following error out of control range", "suggest...", 0x801E) \
    _D(EX_JOINT_EC_SLAVE_ABS_ERR, 10476, "joint" _PH1_ " error: slave MCU detected absolute encoder feedback abnormal", "suggest...", 0x801F) \
    _D(EX_JOINT_EC_SLAVE_ADC_ZERO_OFFSET, 10477, "joint" _PH1_ " error: slave MCU detected current sampling zero offset out of calibration range", "suggest...", 0x8020) \
    _D(EX_JOINT_EC_SLAVE_ENC_POLLUTE, 10478, "joint" _PH1_ " error: slave MCU detected encoder signal quality degradation", "suggest...", 0x8021) \
    _D(EX_JOINT_EC_SLAVE_ENC_Z_LOST, 10479, "joint" _PH1_ " error: slave MCU detected encoder Z reference signal lost", "suggest...", 0x8022) \
    _D(EX_JOINT_EC_SLAVE_COMM_OVER_TM, 10480, "joint" _PH1_ " error: slave MCU detected master-slave communication timeout", "suggest...", 0x8023) \
    _D(EX_JOINT_EC_SLAVE_TRQ_ERR, 10481, "joint" _PH1_ " error: slave MCU detected master-slave motor torque deviation out of range", "suggest...", 0x8024) \
    _D(EX_JOINT_EC_BRAKE_TYPE_ERR, 10482, "joint" _PH1_ " error: brake type config mismatch with hardware", "suggest...", 0x8025) \
    _D(EX_JOINT_EC_PHASE_ALIGN, 10483, "joint" _PH1_ " error: phase alignment failed", "suggest...", 0xFF02) \
    _D(EX_JOINT_EC_POS_OVER_LIMIT_WARNING, 10484, "joint" _PH1_ " error: position over limit", "suggest...", 0x8026) \
    _D(EX_JOINT_EC_PHASE_ALIGN_WARNING, 10485, "joint" _PH1_ " error: phase alignment warning", "suggest...", 0xFF03) \
    _D(EX_JOINT_EC_TASK_STACK_SHORTAGE, 10486, "joint" _PH1_ " error: task stack insufficient", "suggest...", 0xFF04) \
    _D(EX_JOINT_EC_TASK_STACK_OVERFLOW, 10487, "joint" _PH1_ " error: task stack overflow", "suggest...", 0xFF05) \
    _D(EX_JOINT_EC_SERVO_STEP, 10488, "joint" _PH1_ " servo process step", "For information only. No action required.", 0x6105) \
    _D(EX_JOINT_EC_UNKNOWN, 10800, "joint" _PH1_ " error: unknown error", "suggest...", 0xFFFF)

#define EXT_AXIS_ERRORS                                                                                                       \
    _D(EXT_AXIS_ERR_COMMON,              11001, "ext axis" _PH1_ " error: common", "Check communication with ext axis drive.") \
    _D(EXT_AXIS_ERR_OVER_CURRENT,        11002, "ext axis" _PH1_ " error: over current", "Check wiring/short circuit; reboot; if repeated replace drive/motor.") \
    _D(EXT_AXIS_ERR_OVER_VOLTAGE,        11003, "ext axis" _PH1_ " error: over voltage", "Check DC supply, regen, energy eater; reboot.") \
    _D(EXT_AXIS_ERR_LOW_VOLTAGE,         11004, "ext axis" _PH1_ " error: low voltage", "Check DC supply and cabling; reboot.") \
    _D(EXT_AXIS_ERR_OVER_TEMP,           11005, "ext axis" _PH1_ " error: over temperature", "Check environment/cooling; reboot.") \
    _D(EXT_AXIS_ERR_HALL,                11006, "ext axis" _PH1_ " error: hall fault", "Check hall sensor and motor cabling.") \
    _D(EXT_AXIS_ERR_ENCODER,             11007, "ext axis" _PH1_ " error: encoder fault", "Check encoder connection/cable/noise.") \
    _D(EXT_AXIS_ERR_ABS_ENCODER,         11008, "ext axis" _PH1_ " error: absolute encoder fault", "Check abs encoder power/cable; reboot.") \
    _D(EXT_AXIS_ERR_CUR_CALIB,           11009, "ext axis" _PH1_ " error: current calibration fault", "Reboot; check current sensing circuit.") \
    _D(EXT_AXIS_ERR_Q_CURRENT,           11010, "ext axis" _PH1_ " error: current detect fault", "Reboot; check current sensing circuit.") \
    _D(EXT_AXIS_ERR_ENC_POLL,            11011, "ext axis" _PH1_ " error: encoder pollution", "Check encoder contamination/noise; improve shielding.") \
    _D(EXT_AXIS_ERR_ENC_Z_SIGNAL,        11012, "ext axis" _PH1_ " error: encoder Z signal fault", "Check encoder Z channel and wiring.") \
    _D(EXT_AXIS_ERR_ENC_CAL,             11013, "ext axis" _PH1_ " error: encoder calibrate invalid", "Redo calibration; check encoder.") \
    _D(EXT_AXIS_ERR_IMU,                 11014, "ext axis" _PH1_ " error: IMU fault", "Check IMU sensor and connection.") \
    _D(EXT_AXIS_ERR_TEMP_SENSOR,         11015, "ext axis" _PH1_ " error: temperature sensor fault", "Check temp sensor wiring; reboot.") \
    _D(EXT_AXIS_ERR_ECAT_BUS,            11016, "ext axis" _PH1_ " error: EtherCAT bus error", "Check EtherCAT cabling/topology/sync; reboot master/drive.") \
    _D(EXT_AXIS_ERR_ECAT_CONFIG,         11017, "ext axis" _PH1_ " error: EtherCAT config/ESI/SM/PDO fault", "Check ESI, Mailbox/SM/PDO mapping, vendor/product/revision match.") \
    _D(EXT_AXIS_ERR_ECAT_SYNC,           11018, "ext axis" _PH1_ " error: EtherCAT sync/frame/period fault", "Check DC sync, cycle time, frame loss; verify NIC/IRQ affinity.") \
    _D(EXT_AXIS_ERR_SYS_CUR,             11019, "ext axis" _PH1_ " error: system current fault", "Check current loop and load; reboot.") \
    _D(EXT_AXIS_ERR_SYS_POS,             11020, "ext axis" _PH1_ " error: position out of range", "Check encoder/scale/limits; reboot.") \
    _D(EXT_AXIS_ERR_OVER_SPEED,          11021, "ext axis" _PH1_ " error: over speed", "Check command limits and tuning parameters.") \
    _D(EXT_AXIS_ERR_OVER_ACC,            11022, "ext axis" _PH1_ " error: over acceleration", "Reduce acceleration/jerk; check tuning.") \
    _D(EXT_AXIS_ERR_FOLLOW_ERROR,        11023, "ext axis" _PH1_ " error: following error", "Check gains, load, saturation; verify feedback.") \
    _D(EXT_AXIS_ERR_TAG_POS_OVER,        11024, "ext axis" _PH1_ " error: target position out of range", "Check target limits and homing.") \
    _D(EXT_AXIS_ERR_TAG_SPEED_OVER,      11025, "ext axis" _PH1_ " error: target speed out of range", "Clamp speed; check profile settings.") \
    _D(EXT_AXIS_ERR_TAG_CURRENT_OVER,    11026, "ext axis" _PH1_ " error: target current out of range", "Clamp current/torque; check load.") \
    _D(EXT_AXIS_ERR_COLLISION,           11027, "ext axis" _PH1_ " error: collision", "Remove obstruction; check torque/force limits.") \
    _D(EXT_AXIS_ERR_ADC_ZERO_OFFSET,     11028, "ext axis" _PH1_ " error: ADC zero offset", "Reboot; check ADC/current sensor offset.") \
    _D(EXT_AXIS_ERR_IPM_NTC,             11029, "ext axis" _PH1_ " error: IPM NTC fault", "Check power module temperature sensing.") \
    _D(EXT_AXIS_ERR_SHORT_CIRCUIT,       11030, "ext axis" _PH1_ " error: short circuit", "Check motor phase wiring; insulation test.") \
    _D(EXT_AXIS_ERR_MOTOR_STALL,         11031, "ext axis" _PH1_ " error: motor stall", "Check mechanical jam/load; reduce accel; reboot.") \
    _D(EXT_AXIS_ERR_ABS_MULTITURN,       11032, "ext axis" _PH1_ " error: abs encoder multiturn fault", "Check abs encoder battery/params; reboot.") \
    _D(EXT_AXIS_ERR_PHASE_LOST,          11033, "ext axis" _PH1_ " error: motor phase lost", "Check phase wiring/connector; measure continuity.") \
    _D(EXT_AXIS_ERR_BRAKE,               11034, "ext axis" _PH1_ " error: brake fault", "Check brake wiring/power; verify brake release.") \
    _D(EXT_AXIS_ERR_REDUCER_OVER_TEMP,   11035, "ext axis" _PH1_ " error: reducer over temperature", "Check reducer temperature/cooling.") \
    _D(EXT_AXIS_ERR_REDUCER_NTC,         11036, "ext axis" _PH1_ " error: reducer NTC fault", "Check reducer temperature sensor.") \
    _D(EXT_AXIS_ERR_FIRMWARE_UPDATE,     11037, "ext axis" _PH1_ " error: firmware update fault", "Retry update; check power stability.") \
    _D(EXT_AXIS_ERR_FLASH_OP,            11038, "ext axis" _PH1_ " error: flash operation fault", "Retry; if persistent replace drive.") \
    _D(EXT_AXIS_ERR_EXT_ABS_ENC,         11039, "ext axis" _PH1_ " error: motor-side abs encoder comm fault", "Check external abs encoder link/power.") \
    _D(EXT_AXIS_ERR_DRIVE_FAULT,         11040, "ext axis" _PH1_ " error: drive fault", "Check drive alarm code; reboot; replace if repeated.") \
    _D(EXT_AXIS_ERR_OVERLOAD,            11041, "ext axis" _PH1_ " error: overload", "Reduce load; check mechanics and tuning.") \
    _D(EXT_AXIS_ERR_HARDWARE_LIMIT,      11042, "ext axis" _PH1_ " error: hardware limit triggered", "Move away from limit; check limit switch.") \
    _D(EXT_AXIS_ERR_SERVO_MODE_TIMEOUT,  11043, "ext axis" _PH1_ " error: switch servo mode timeout", "Check mode transition and comm; reboot.") \
    _D(EXT_AXIS_ERR_UVW_ABZ,             11044, "ext axis" _PH1_ " error: UVW/ABZ fault", "Check phase/encoder signals wiring.") \
    _D(EXT_AXIS_ERR_BATTERY_LOW,         11045, "ext axis" _PH1_ " error: battery low", "Replace encoder battery; reboot.") \
    _D(EXT_AXIS_ERR_PHASE_ALIGN,         11046, "ext axis" _PH1_ " error: phase align fail", "Redo phase alignment; check motor params.") \
    _D(EXT_AXIS_ERR_POS_DISCONTINUOUS,   11047, "ext axis" _PH1_ " error: position command discontinuous", "Check trajectory generation and limits.") \
    _D(EXT_AXIS_ERR_POS_INIT,            11048, "ext axis" _PH1_ " error: position initialization failure", "Check encoder init/homing procedure.") \
    _D(EXT_AXIS_ERR_TORQUE_SENSOR,       11049, "ext axis" _PH1_ " error: torque sensor fault", "Check torque sensor wiring/calibration.") \
    _D(EXT_AXIS_ERR_ABS_ENC_LOW_VOLT,    11050, "ext axis" _PH1_ " error: abs encoder low voltage", "Check encoder supply voltage and cable.") \
    _D(EXT_AXIS_ERR_OFFLINE,             11051, "ext axis" _PH1_ " error: ext axis offline", "Check hardware and axis id; check EtherCAT state.") \
    _D(EXT_AXIS_ERR_BOOTLOADER,          11052, "ext axis" _PH1_ " error: ext axis in bootloader", "Retry firmware update.") \
    _D(EXT_AXIS_ERR_SLAVE_OFFLINE,       11053, "ext axis slave" _PH1_ " error: slave offline", "Check slave hardware and id.") \
    _D(EXT_AXIS_ERR_SLAVE_BOOTLOADER,    11054, "ext axis slave" _PH1_ " error: slave in bootloader", "Retry firmware update.") \
    _D(EXT_AXIS_ERR_EEPROM,              11055, "ext axis" _PH1_ " error: EEPROM/param store fault", "Check EEPROM/parameter storage; power cycle.") \
    _D(EXT_AXIS_ERR_PARAM_CONFIG,        11056, "ext axis" _PH1_ " error: parameter/config fault", "Verify parameter set; restore defaults if needed.") \
    _D(EXT_AXIS_ERR_STO,                 11057, "ext axis" _PH1_ " error: STO safety fault", "Check STO wiring/safety chain; reset safety.") \
    _D(EXT_AXIS_ERR_ENCRYPT_CHIP,        11058, "ext axis" _PH1_ " error: encrypt chip/key fault", "Check encryption chip/keys/firmware compatibility.") \
    _D(EXT_AXIS_ERR_BRAKE_RES_OVERLOAD,  11059, "ext axis" _PH1_ " error: brake resistor overload", "Check brake resistor/regen circuit; duty cycle.") \
    _D(EXT_AXIS_ERR_POWER_LINE_OPEN,     11060, "ext axis" _PH1_ " error: motor power line open", "Check motor power cable continuity/connector.") \
    _D(EXT_AXIS_ERR_HOMING,              11061, "ext axis" _PH1_ " error: homing fault", "Check homing sensor/origin procedure; retry.") \
    _D(EXT_AXIS_ERR_TUNING_FAIL,         11062, "ext axis" _PH1_ " error: tuning fail", "Redo tuning; reduce resonance; check mechanics.") \
    _D(EXT_AXIS_ERR_INERTIA_ID_FAIL,     11063, "ext axis" _PH1_ " error: inertia identification fail", "Check load; redo inertia ID; adjust conditions.") \
    _D(EXT_AXIS_ERR_FLYAWAY,             11064, "ext axis" _PH1_ " error: flyaway", "Emergency stop; check feedback polarity/scale; inspect drive params.") \
    _D(EXT_AXIS_ERR_SPEED_PULSE_OVER,    11065, "ext axis" _PH1_ " error: feedback pulse overspeed", "Check encoder feedback and scaling; reduce speed.") \
    _D(EXT_AXIS_ERR_CTRL_LOOP,           11066, "ext axis" _PH1_ " error: control loop/timeout fault", "Check sampling/current loop/comm timeout; reboot.")

#define TOOL_ERRORS \
    _D(TOOL_FLASH_VERIFY_FAILED, 40001, "Flash write verify failed", "suggest...") \
    _D(TOOL_PROGRAM_CRC_FAILED, 40002, "Program flash checksum failed during bootloading", "suggest...") \
    _D(TOOL_PROGRAM_CRC_FAILED2, 40003, "Program flash checksum failed at runtime", "suggest...") \
    _D(TOOL_ID_UNDIFINED, 40004, "Tool ID is undefined", "suggest...") \
    _D(TOOL_ILLEGAL_BL_CMD, 40005, "Illegal bootloader command", "suggest...") \
    _D(TOOL_FW_WRONG, 40006, "Wrong firmware at the joint", "suggest...") \
    _D(TOOL_HW_INVALID, 40007, "Invalid hardware revision", "suggest...") \
    _D(TOOL_SHORT_CURCUIT_H, 40011, "Short circuit detected on Digital Output: " _PH1_ " high side", "suggest...") \
    _D(TOOL_SHORT_CURCUIT_L, 40012, "Short circuit detected on Digital Output: " _PH1_ " low side", "suggest...") \
    _D(TOOL_AVERAGE_CURR_HIGH, 40013, "10 second Average tool IO Current of " _PH1_ " A is outside of the allowed range.", "suggest...") \
    _D(TOOL_POWER_PIN_OVER_CURR, 40014, "Current of " _PH1_ " A on the POWER pin is outside of the allowed range.", "suggest...") \
    _D(TOOL_DOUT_PIN_OVER_CURR, 40015, "Current of " _PH1_ " A on the Digital Output pins is outside of the allowed range.", "suggest...") \
    _D(TOOL_GROUND_PIN_OVER_CURR, 40016, "Current of " _PH1_ " A on the ground pin is outside of the allowed range.", "suggest...") \
    _D(TOOL_RX_FRAMING, 40021, "RX framing error", "suggest...") \
    _D(TOOL_RX_PARITY, 40022, "RX Parity error", "suggest...") \
    _D(TOOL_48V_LOW, 40031, "48V input is too low", "suggest...") \
    _D(TOOL_48V_HIGH, 40032, "48V input is too high", "suggest...") \
    _D(TOOL_ERR_OFFLINE,  40033, "tool error: tool may be offline", "(a) Check tool's hardware. (b) Check joint's id.") \
    _D(TOOL_ERR_BOOTLOADER,  40034, "tool error: The tool is in bootloader mode. Retry firmware update. ", "suggest...")

#define EX_TOOL_ERRORS \
    _D(EX_TOOL_EC_LOW_VOLTAGE, 40101, "tool error: low voltage", "check tool power supply voltage") \
    _D(EX_TOOL_EC_FORCESENSOR_COMM, 40102, "tool error: external force sensor communication error", "check external force sensor communication connection") \
    _D(EX_TOOL_485_SENDFULL_COMM, 40103, "tool error: 485 transparent transmission buffer full", "check 485 communication load and transmission frequency") \
    _D(EX_TOOL_FORCESENSOR_FILTER_ZERO, 40104, "tool error: force sensor filter parameter is zero", "check force sensor filter parameter configuration") \
    _D(EX_TOOL_EC_UNKNOWN, 40200, "tool error: unknown error", "check controller logs and hardware status")

#define PEDSTRAL_ERRORS \
    _D(PKG_LOST, 50001, "Lost package from pedestal", "suggest...") \
    _D(PEDSTRAL_OFFLINE,  50002, "pedestal error: pedestal may be offline", "(a) Check pedestal's hardware. (b) Check pedestal's id.") \
    _D(PEDESTAL_ERR_BOOTLOADER,  50003, "pedestal error: The pedestal is in bootloader mode. Retry firmware update. ", "suggest...")

#define EX_PEDSTRAL_ERRORS \
    _D(EX_BASE_EC_LOW_VOLTAGE, 50101, "pedstral error: low voltage", "check power supply voltage and battery status") \
    _D(EX_BASE_EC_OVER_TEMPERATURE_RES, 50102, "pedstral error: resistor over temperature", "check braking resistor temperature and cooling condition") \
    _D(EX_BASE_EC_OVER_TARGET_BRAKE_OPEN_VOLT, 50103, "pedstral error: input voltage close to or exceeds regenerative brake activation voltage", "check input power voltage and regenerative braking configuration") \
    _D(EX_BASE_EC_RES_BREAKAGE, 50104, "pedstral error: brake resistor breakage", "check whether the brake resistor is disconnected or damaged") \
    _D(EX_BASE_EC_IMU_CALIBRATE, 50105, "pedstral error: IMU calibration required", "perform IMU calibration according to maintenance procedure") \
    _D(EX_BASE_EC_TEMP_SENSOR_SHORT, 50106, "pedstral error: temperature sensor short circuit", "check temperature sensor wiring and solder joints for short circuit") \
    _D(EX_BASE_EC_TEMP_SENSOR_BREAK, 50107, "pedstral error: temperature sensor open circuit", "check temperature sensor connection and cable continuity") \
    _D(EX_BASE_EC_96V_INSTANT_OVER_VOLTAGE, 50108, "pedstral error: 96V instantaneous over voltage after regenerative braking", "check regenerative braking behavior and power bus voltage") \
    _D(EX_BASE_EC_UNKNOWN, 50200, "pedstral error: unknown error", "check controller logs and hardware status")

#define SAFETY_INTERFACE_BOARD_ERRORS \
    _D(IFB_ERR_ROBOTTYPE, 20001, "Robot error type!", "suggest...") \
    _D(IFB_ERR_ADXL_SENS, 20002, "Base Acceleration sensor error!", "suggest...") \
    _D(IFB_ERR_EN_LINE, 20003, "Encoder line error!", "suggest...") \
    _D(IFB_ERR_ENTER_HDG_MODE, 20004, "Robot enter handguide mode!", "suggest...") \
    _D(IFB_ERR_EXIT_HDG_MODE, 20005, "Robot exit handguide mode!", "suggest...") \
    _D(IFB_ERR_MAC_DATA_BREAK, 20006, "MAC data break!", "suggest...") \
    _D(IFB_ERR_DRV_FIRMWARE_VERSION, 20007, "Motor driver firmware version error!", "suggest...") \
    _D(INIT_ERR_EN_DRV, 20008, "Motor driver enable failed!", "suggest...") \
    _D(INIT_ERR_EN_AUTO_BACK, 20009, "Motor driver enable auto back failed!", "suggest...") \
    _D(INIT_ERR_EN_CUR_LOOP, 20010, "Motor driver enable current loop failed!", "suggest...") \
    _D(INIT_ERR_SET_TAG_CUR, 20011, "Motor driver set target current failed!", "suggest...") \
    _D(INIT_ERR_RELEASE_BRAKE, 20012, "Motor driver release brake failed!", "suggest...") \
    _D(INIT_ERR_EN_POS_LOOP, 20013, "Motor driver enable postion loop failed!", "suggest...") \
    _D(INIT_ERR_SET_MAX_ACC, 20014, "Motor set max accelerate failed!", "suggest...") \
    _D(SAFETY_ERR_PROTECTION_STOP_TIMEOUT, 20015, "Protective stop timeout!", "suggest...") \
    _D(SAFETY_ERR_REDUCED_MODE_TIMEOUT, 20016, "Reduced mode timeout!", "suggest...") \
    _D(SYS_ERR_MCU_COM, 20017, "Robot system error: mcu communication error!", "suggest...") \
    _D(SYS_ERR_RS485_COM, 20018, "Robot system error: RS485 communication error!", "suggest...") \
    _D(IFB_ERR_DISCONNECTED, 20019, "Interface board may be disconnected. Please check connection between IPC and Interface board.", "suggest...")\
    _D(IFB_ERR_PAYLOAD_ERROR, 20020, "Payload error.", "suggest...") \
    _D(IFB_OFFLINE,  20021, "ifaceboard error: ifaceboard may be offline", "(a) Check ifaceboard's hardware. (b) Check ifaceboard's id.") \
    _D(IFB_ERR_BOOTLOADER,  20022, "ifaceboard error: The ifaceboard is in bootloader mode. Retry firmware update. ", "suggest...") \
    _D(IFB_SLAVE_OFFLINE,  20023, "interface slave board error: interface slave board may be offline", "(a) Check interface slave board's hardware. (b) Check interface slave board's id.") \
    _D(IFB_SLAVE_ERR_BOOTLOADER,  20024, "interface slave board error: The interface slave board is in bootloader mode. Retry firmware update. ", "suggest...") \
    _D(IFB_TOOL_ERR_ADXL_SENS, 20025, "Tool Acceleration sensor error!", "suggest...") \
    _D(HANDLE_OFFLINE,  20026, "handle error: handle may be offline", "(a) Check handle's hardware. (b) Check handle's id.") \
    _D(HANDLE_ERR_BOOTLOADER,  20027, "handle error: The handle is in bootloader mode. Retry firmware update. ", "suggest...") \
    _D(IFB_POWERLOSS_OFFLINE,  20028, "interface powerloss board error: interface powerloss board may be offline", "(a) Check interface powerloss board's hardware. (b) Check interface powerloss board's id.") \
    _D(IFB_POWERLOSS_ERR_BOOTLOADER,  20029, "interface powerloss board error: The interface powerloss board is in bootloader mode. Retry firmware update. ", "suggest...") \
    _D(HANDLE_COMM_ERROR,  20030, "handle error: handle comm error", "(a) Check handle's hardware. (b) Check handle's id.")

#define HARDWARE_INTERFACE_ERRORS \
    _D(HW_SCB_SETUP_FAILED, 60001, "Setup of Interface Board failed", "suggest...") \
    _D(HW_PKG_CNT_DISAGEE, 60002, "Packet counter disagreements", "suggest...") \
    _D(HW_SCB_DISCONNECT, 60003, "Connection to Interface Board lost", "suggest...") \
    _D(HW_SCB_PKG_LOST, 60004, "Package lost from Interface Board", "suggest...") \
    _D(HW_SCB_CONN_INIT_FAILED, 60005, "Ethernet connection initialization with Interface Board failed", "suggest...") \
    _D(HW_LOST_JOINT_PKG, 60006, "Lost package from joint  " _PH1_ "", "suggest...") \
    _D(HW_LOST_TOOL_PKG, 60007, "Lost package from tool", "suggest...") \
    _D(HW_JOINT_PKG_CNT_DISAGREE, 60008, "Packet counter disagreement in packet from joint " _PH1_ "", "suggest...") \
    _D(HW_TOOL_PKG_CNT_DISAGREE, 60009, "Packet counter disagreement in packet from tool", "suggest...") \
    _D(HW_JOINTS_FAULT, 60011, "" _PH1_ " joint entered the Fault State", "suggest...") \
    _D(HW_JOINTS_VIOLATION, 60012, "" _PH1_ " joint entered the Violation State", "suggest...") \
    _D(HW_TP_FAULT, 60013, "Teach Pendant entered the Fault State", "suggest...") \
    _D(HW_TP_VIOLATION, 60014, "Teach Pendant entered the Violation State", "suggest...") \
    _D(HW_JOINT_MV_TOO_FAR, 60021, "" _PH1_ " joint moved too far before robot entered RUNNING State", "suggest...") \
    _D(HW_JOINT_STOP_NOT_FAST, 60022, "Joint Not stopping fast enough", "suggest...") \
    _D(HW_JOINT_MV_LIMIT, 60023, "Joint moved more than allowable limit", "suggest...") \
    _D(HW_FT_SENSOR_DATA_INVALID, 60024, "Force-Torque Sensor data invalid", "suggest...") \
    _D(HW_NO_FT_SENSOR, 60025, "Force-Torque sensor is expected, but it cannot be detected", "suggest...") \
    _D(HW_FT_SENSOR_NOT_CALIB, 60026, "Force-Torque sensor is detected but not calibrated", "suggest...") \
    _D(HW_RELEASE_BRAKE_FAILED, 60030, "Robot was not able to brake release, see log for details", "suggest...") \
    _D(HW_OVERCURR_SHUTDOWN, 60040, "Overcurrent shutdown", "suggest...") \
    _D(HW_ENERGEY_SURPLUS, 60050, "Energy surplus shutdown", "suggest...") \
    _D(HW_IDLE_POWER_HIGH, 60060, "Idle power consumption to high", "suggest...") \
    _D(HW_ENTER_COLLISION_TIMEOUT, 60071, "Enter collision stop procedure timeout", "suggest...") \
    _D(HW_POWERON_TIMEOUT, 60072, "Poweron robot timeout", "suggest...") \
    _D(HW_NO_NIC_FOUND, 60073, "No network cards found.", "suggest...") \
    _D(HW_IFB_NOT_FOUND, 60074, "No Interface Board found.", "suggest...") \
    _D(HW_IFB_BOOTLOAD, 60075, "The Interface Board is in bootloader mode. Update firmware firstly.", "suggest...") \
    _D(HW_TOOL_NOT_FOUND, 60076, "No Tool Board found.", "suggest...") \
    _D(HW_BASE_NOT_FOUND, 60077, "No Base Board found.", "suggest...") \
    _D(HW_BRINGUP_TIMEOUT, 60078, "Poweron robot timeout", "suggest...") \
    _D(HW_COLLISION_RECOVERY_FAILED, 60079, "Collision recovery failed", "suggest...") \
    _D(HW_TP_ENABLED, 60080, "Teach pendant enabled status changed to " _PH1_, "suggest...")

// clang-format on

// 定义硬件抽象层的错误代码
#define HAL_ERRORS                \
    JOINT_ERRORS                  \
    JOINT_EX_ERRORS               \
    EXT_AXIS_ERRORS               \
    SAFETY_INTERFACE_BOARD_ERRORS \
    TOOL_ERRORS                   \
    EX_TOOL_ERRORS                \
    PEDSTRAL_ERRORS               \
    EX_PEDSTRAL_ERRORS            \
    HARDWARE_INTERFACE_ERRORS

#endif // AUBO_SDK_JOINT_ERROR_H
