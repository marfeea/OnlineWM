---
layout: doc
title: "错误码"
---

# 错误码

最后更新时间: 2026-06-23

## 系统错误 (0 - 102)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 0 | `DEBUG` | Debug message {} |  |  |
| 1 | `POPUP` | Popup title: {}, msg: {}, mode: {} |  |  |
| 2 | `POPUP_DISMISS` | {} |  |  |
| 3 | `SYSTEM_HALT` | {} |  |  |
| 4 | `INV_ARGUMENTS` | Invalid arguments. |  |  |
| 5 | `USER_NOTIFY` | {} |  |  |
| 6 | `POPUP_DISMISS_BY_ID` | {} |  |  |
| 10 | `MODBUS_SIGNAL_CREATED` | Modbus signal {} created. |  |  |
| 11 | `MODBUS_SIGNAL_REMOVED` | Modbus signal {} removed. |  |  |
| 12 | `MODBUS_SIGNAL_VALUE_CHANGED` | Modbus signal {} value changed to {} |  |  |
| 13 | `RUNTIME_CONTEXT` | tid: {} lineno: {} index: {} comment: {} |  |  |
| 14 | `INTERP_CONTEXT` | tid: {} lineno: {} index: {} comment: {} |  |  |
| 15 | `PROGRAM_LOADED` | program loaded: {} |  |  |
| 16 | `TASK_DELETED` | tid: {} |  | was deleted |
| 20 | `MODBUS_SLAVE_BIT` | Modbus slave address: {} value {} |  |  |
| 21 | `MODBUS_SLAVE_REG` | Modbus slave address: {} value {} |  |  |
| 30 | `PNIO_SLAVE_SLOT_VALUE` | PNIO slot: {} subslot {} index {} value {} |  |  |
| 31 | `PNIO_CONNECT_STATUS` | PNIO connection status changed to {} |  |  |
| 32 | `PNIO_DEVICE_NAME` | PNIO device name changed to {} |  |  |
| 33 | `PNIO_IP` | PNIO ip {} mask {} gateway {} |  |  |
| 40 | `ICM_SERVER_STATUS` | ICM server status changed to {} |  |  |
| 50 | `EIP_SLAVE_VALUE` | EIP slave: trans_type {} index {} value {} |  |  |
| 51 | `EIP_SLAVE_CONNECT_STATUS` | EIP slave connection status changed to {} |  |  |
| 100 | `LOG_PROGRAM_SUCCESS` | [{}] Load program {} successful |  |  |
| 101 | `LOG_PROGRAM_FAILED` | [{}] Load program {} failed, file not found |  |  |
| 102 | `LOG_PROGRAM_FAILED2` | [{}] Load program {} failed, configuration file (.ins) does not match |  |  |

## 关节错误 (10001 - 10044)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 10001 | `JOINT_ERR_OVER_CURRENET` | joint{} error: over current |  | (a) Check for short circuit. (b) Do a Complete rebooting sequence. (c) If this happens more than two times in a row, replace joint |
| 10002 | `JOINT_ERR_OVER_VOLTAGE` | joint{} error: over voltage |  | (a) Do a Complete rebooting sequence. (b) Check 48 V Power supply, current distributer, energy eater and Control Board for issues |
| 10003 | `JOINT_ERR_LOW_VOLTAGE` | joint{} error: low voltage |  | (a) Do a Complete rebooting sequence. (b) Check for short circuit in robot arm. (c) Check 48 V Power supply, current distributer, energy eater and Control Board for issues |
| 10004 | `JOINT_ERR_OVER_TEMP` | joint{} error: over temperature |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10005 | `JOINT_ERR_HALL` | joint{} error: hall |  |  |
| 10006 | `JOINT_ERR_ENCODER` | joint{} error: encoder |  | Check encoder connections |
| 10007 | `JOINT_ERR_ABS_ENCODER` | joint{} error: abs encoder |  |  |
| 10008 | `JOINT_ERR_Q_CURRENT` | joint{} error: detect current |  |  |
| 10009 | `JOINT_ERR_ENC_POLL` | joint{} error: encoder pollustion |  |  |
| 10010 | `JOINT_ERR_ENC_Z_SIGNAL` | joint{} error: enocder z signal |  |  |
| 10011 | `JOINT_ERR_ENC_CAL` | joint{} error: encoder calibrate |  |  |
| 10012 | `JOINT_ERR_IMU_SENS` | joint{} error: IMU sensor |  |  |
| 10013 | `JOINT_ERR_TEMP_SENS` | joint{} error: TEMP sensor |  |  |
| 10014 | `JOINT_ERR_CAN_BUS` | joint{} error: CAN bus error |  |  |
| 10015 | `JOINT_ERR_SYS_CUR` | joint{} error: system current error |  |  |
| 10016 | `JOINT_ERR_SYS_POS` | joint{} error: system position error |  |  |
| 10017 | `JOINT_ERR_OVER_SP` | joint{} error: over speed |  |  |
| 10018 | `JOINT_ERR_OVER_ACC` | joint{} error: over accelerate |  |  |
| 10019 | `JOINT_ERR_TRACE` | joint{} error: trace accuracy |  |  |
| 10020 | `JOINT_ERR_TAG_POS_OVER` | joint{} error: target position out of range |  |  |
| 10021 | `JOINT_ERR_TAG_SP_OVER` | joint{} error: target speed out of range |  |  |
| 10022 | `JOINT_ERR_COLLISION` | joint{} error: collision |  |  |
| 10023 | `JOINT_ERR_COMMON` | joint{} error: unkown error. Check communication with joint. |  |  |
| 10024 | `JOINT_ERR_SWITCH_SERVO_MODE` | joint{} error: switch servo mode timeout. |  |  |
| 10025 | `JOINT_ERR_MOTOR_STUCK` | joint{} error: motor stucked. |  |  |
| 10026 | `JOINT_ERR_REDUCER_OVER_TEMP` | joint{} error: reducer over temperature |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10027 | `JOINT_ERR_REDUCER_NTC` | joint{} error: reducer TEMP sensor failure |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10028 | `JOINT_ERR_ABS_MULTITURN` | joint{} error: absolute encoder multiturn error |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10029 | `JOINT_ERR_ADC_ZERO_OFFSET` | joint{} error: ADC zero offset failure |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10030 | `JOINT_ERR_SHORT_CIRCUIT` | joint{} error: short circuit |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10031 | `JOINT_ERR_PHASE_LOST` | joint{} error: motor phase lost |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10032 | `JOINT_ERR_BRAKE` | joint{} error: brake failure |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10033 | `JOINT_ERR_FIRMWARE_UPDATE` | joint{} error: firmware update failure |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10034 | `JOINT_ERR_BATTERY_LOW` | joint{} error: battery low |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10035 | `JOINT_ERR_PHASE_ALIGN` | joint{} error: phase align |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10036 | `JOINT_ERR_CAN_HW_FAULT` | joint{} error: CAN bus hw fault |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10037 | `JOINT_ERR_POS_DISCONTINUOUS` | joint{} error: target position discontinuous |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10038 | `JOINT_ERR_POS_INIT` | joint{} error: position initiallization failure |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10039 | `JOINT_ERR_TORQUE_SENSOR` | joint{} error: torqure sensor failure |  | (a) Check robot’s environment and make sure the robot is operating within recommended limits. (b) Do a Complete rebooting sequence |
| 10040 | `JOINT_ERR_OFFLINE` | joint{} error: joint may be offline |  | (a) Check joint's hardware. (b) Check joint's id. |
| 10041 | `JOINT_ERR_BOOTLOADER` | joint{} error: The joint is in bootloader mode. Retry firmware update. |  |  |
| 10042 | `JOINT_ERR_SLAVE_OFFLINE` | slave joint{} error: slave joint may be offline |  | (a) Check slave joint's hardware. (b) Check slave joint's id. |
| 10043 | `JOINT_ERR_SLAVE_BOOTLOADER` | slave joint{} error: The slave joint is in bootloader mode. Retry firmware update. |  |  |
| 10044 | `JOINT_ERR_ETHERCAT_BUS` | joint{} error: ETHERCAT bus error |  |  |

## 关节扩展错误 (10401 - 10800)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 10401 | `EX_JOINT_EC_SHORT_CIRCUIT` | joint{} error: short circuit protection |  |  |
| 10402 | `EX_JOINT_EC_SHORT_CURRENT` | joint{} error: over current |  |  |
| 10403 | `EX_JOINT_EC_PHASEA_CURRENT` | joint{} error: phase A over current |  |  |
| 10404 | `EX_JOINT_EC_PHASEB_CURRENT` | joint{} error: phase B over current |  |  |
| 10405 | `EX_JOINT_EC_PHASEC_CURRENT` | joint{} error: phase C over current |  |  |
| 10406 | `EX_JOINT_EC_PHASE_CURRENT` | joint{} error: phase over current |  |  |
| 10407 | `EX_JOINT_EC_MOTOR_PHASE_LOSE` | joint{} error: motor phase loss |  |  |
| 10408 | `EX_JOINT_EC_BUS_OVER_VOLTAGE` | joint{} error: bus over voltage |  |  |
| 10409 | `EX_JOINT_EC_BUS_LOW_VOLTAGE` | joint{} error: bus under voltage |  |  |
| 10410 | `EX_JOINT_EC_OVERLOAD` | joint{} error: overload |  |  |
| 10411 | `EX_JOINT_EC_IPM_OVER_TEMP` | joint{} error: IPM over temperature |  |  |
| 10412 | `EX_JOINT_EC_REDUCER_OVER_TEMP` | joint{} error: reducer over temperature |  |  |
| 10413 | `EX_JOINT_EC_ADC_ZERO_OFFSET` | joint{} error: ADC zero offset |  |  |
| 10414 | `EX_JOINT_EC_REDUCER_NTC` | joint{} error: reducer NTC fault |  |  |
| 10415 | `EX_JOINT_EC_IPM_NTC` | joint{} error: IPM NTC fault |  |  |
| 10416 | `EX_JOINT_EC_TORQUE_SENSOR` | joint{} error: torque sensor fault |  |  |
| 10417 | `EX_JOINT_EC_TORQUE_SENSOR_COMM` | joint{} error: torque sensor communication fault |  |  |
| 10418 | `EX_JOINT_EC_MOTOR_ABS_ENC_COMM` | joint{} error: motor-side absolute encoder communication fault |  |  |
| 10419 | `EX_JOINT_EC_REDUCER_ABS_ENC_COMM` | joint{} error: reducer-side absolute encoder communication fault |  |  |
| 10420 | `EX_JOINT_EC_REDUCER_ABS_ENC_DATA` | joint{} error: reducer-side absolute encoder data channel disabled warning |  |  |
| 10421 | `EX_JOINT_EC_REDUCER_ABS_ENC_CMD` | joint{} error: reducer-side absolute encoder command invalid warning |  |  |
| 10422 | `EX_JOINT_EC_REDUCER_ABS_ENC_ERR` | joint{} error: reducer-side absolute encoder fault |  |  |
| 10423 | `EX_JOINT_EC_REDUCER_ABS_ENC_WARNING` | joint{} error: reducer-side absolute encoder warning |  |  |
| 10424 | `EX_JOINT_EC_BRAKE` | joint{} error: brake fault |  |  |
| 10425 | `EX_JOINT_EC_COMM_HWL` | joint{} error: communication hardware layer error |  |  |
| 10426 | `EX_JOINT_EC_FIRMWARE_UPDATE` | joint{} error: firmware update failed |  |  |
| 10427 | `EX_JOINT_EC_FLASH_OP` | joint{} error: flash operation failed |  |  |
| 10428 | `EX_JOINT_EC_MU_SAVE` | joint{} error: multi-turn data error |  |  |
| 10429 | `EX_JOINT_EC_DEMADATA_LOST` | joint{} error: calibration zero point data lost |  |  |
| 10430 | `EX_JOINT_EC_PARAMETER` | joint{} error: parameter error |  |  |
| 10431 | `EX_JOINT_EC_UVW_LOGIC` | joint{} error: hall signal fault |  |  |
| 10432 | `EX_JOINT_EC_UVW_ABZ` | joint{} error: incremental encoder fault |  |  |
| 10433 | `EX_JOINT_EC_ENC_Z_LOST` | joint{} error: encoder Z signal lost |  |  |
| 10434 | `EX_JOINT_EC_ENC_POLLUTE` | joint{} error: encoder pollution |  |  |
| 10435 | `EX_JOINT_EC_ENC_CALI` | joint{} error: encoder calibration failed |  |  |
| 10436 | `EX_JOINT_EC_MT_ABS_DATA` | joint{} error: multi-turn absolute data error |  |  |
| 10437 | `EX_JOINT_EC_ENC_TYPE_INFO` | joint{} error: encoder type identified and saved |  |  |
| 10438 | `EX_JOINT_EC_ENC_TYPE_ERROR` | joint{} error: encoder type error |  |  |
| 10439 | `EX_JOINT_EC_ENC_VERIFY` | joint{} error: encoder verification failed |  |  |
| 10440 | `EX_JOINT_EC_DUAL_ENC_ERROR` | joint{} error: dual encoder deviation too large |  |  |
| 10441 | `EX_JOINT_EC_DUAL_ENC_EANGLE` | joint{} error: dual encoder electrical angle deviation too large |  |  |
| 10442 | `EX_JOINT_EC_OBJECT_DICT_ERROR` | joint{} error: object dictionary data error |  |  |
| 10443 | `EX_JOINT_EC_MOTOR_STALL` | joint{} error: motor stall protection |  |  |
| 10444 | `EX_JOINT_EC_ABS_ENC_LOW_VOLT` | joint{} error: absolute encoder low voltage |  |  |
| 10445 | `EX_JOINT_EC_MT_BATTERY_LOW` | joint{} error: multi-turn battery low voltage |  |  |
| 10446 | `EX_JOINT_EC_POS_CMD` | joint{} error: position command discontinuous |  |  |
| 10447 | `EX_JOINT_EC_POS_OVER_LIMIT` | joint{} error: position over limit |  |  |
| 10448 | `EX_JOINT_EC_COMM_PROTO` | joint{} error: communication protocol layer error |  |  |
| 10449 | `EX_JOINT_EC_GRAVITY_PARA_WARNING` | joint{} error: gravity compensation parameter invalid |  |  |
| 10450 | `EX_JOINT_EC_GRAVITY_COMPENSATE_ERROR` | joint{} error: gravity compensation value sudden change |  |  |
| 10451 | `EX_JOINT_EC_LOW_RIGIDITY` | joint{} error: collision soft float abnormal |  |  |
| 10452 | `EX_JOINT_EC_POS_CMD_WARNING` | joint{} error: position command unchanged during motion |  |  |
| 10453 | `EX_JOINT_EC_SLAVE_COMM` | joint{} error: master-slave MCU communication fault |  |  |
| 10454 | `EX_JOINT_EC_POS_ERR` | joint{} error: position following error too large |  |  |
| 10455 | `EX_JOINT_EC_DUAL_POS_ERR` | joint{} error: dual servo position sync error too large |  |  |
| 10456 | `EX_JOINT_EC_DUAL_COMM_ERR` | joint{} error: dual servo communication |  |  |
| 10457 | `EX_JOINT_EC_CAN_BUSOFF` | joint{} error: CAN bus-off warning |  |  |
| 10458 | `EX_JOINT_EC_SYNC_SNAKE` | joint{} error: sync frame jitter warning |  |  |
| 10459 | `EX_JOINT_EC_SYNC_DISCON` | joint{} error: sync frame discontinuous warning |  |  |
| 10460 | `EX_JOINT_EC_RPDO_LOST` | joint{} error: RPDO lost warning |  |  |
| 10461 | `EX_JOINT_EC_RPDO_MANY` | joint{} error: multiple RPDO in one sync cycle warning |  |  |
| 10462 | `EX_JOINT_EC_GRAVITY_LOST` | joint{} error: gravity compensation value lost |  |  |
| 10463 | `EX_JOINT_EC_MAININT_TIME_WARN` | joint{} error: servo main interrupt runtime warning |  |  |
| 10464 | `EX_JOINT_EC_MAININT_TIME_ERROR` | joint{} error: servo main interrupt runtime error |  |  |
| 10465 | `EX_JOINT_EC_SPDINT_TIME_WARN` | joint{} error: servo speed loop interrupt runtime warning |  |  |
| 10466 | `EX_JOINT_EC_SPDINT_TIME_ERROR` | joint{} error: servo speed loop interrupt runtime error |  |  |
| 10467 | `EX_JOINT_EC_POS_CMD_JUMP_WARNING` | joint{} error: position command sudden jump during motion |  |  |
| 10468 | `EX_JOINT_EC_SYNC_TIMCOARSE` | joint{} error: clock sync compensation status |  |  |
| 10469 | `EX_JOINT_EC_ENC_Z_CNTS_ERR` | joint{} error: incremental encoder Z signal or CNTS abnormal |  |  |
| 10470 | `EX_JOINT_EC_SLAVE_OVER_CURRENT` | joint{} error: slave MCU detected motor phase current over safe threshold |  |  |
| 10471 | `EX_JOINT_EC_SLAVE_OVER_VOLTAGE` | joint{} error: slave MCU detected DC bus voltage over upper threshold |  |  |
| 10472 | `EX_JOINT_EC_SLAVE_UNDER_VOLTAGE` | joint{} error: slave MCU detected DC bus voltage below lower threshold |  |  |
| 10473 | `EX_JOINT_EC_SLAVE_POS_ERR` | joint{} error: slave MCU detected master-slave motor position deviation out of range |  |  |
| 10474 | `EX_JOINT_EC_SLAVE_SPEED_ERR` | joint{} error: slave MCU detected master-slave motor speed deviation out of range |  |  |
| 10475 | `EX_JOINT_EC_SLAVE_TRACE_ERR` | joint{} error: slave MCU detected position following error out of control range |  |  |
| 10476 | `EX_JOINT_EC_SLAVE_ABS_ERR` | joint{} error: slave MCU detected absolute encoder feedback abnormal |  |  |
| 10477 | `EX_JOINT_EC_SLAVE_ADC_ZERO_OFFSET` | joint{} error: slave MCU detected current sampling zero offset out of calibration range |  |  |
| 10478 | `EX_JOINT_EC_SLAVE_ENC_POLLUTE` | joint{} error: slave MCU detected encoder signal quality degradation |  |  |
| 10479 | `EX_JOINT_EC_SLAVE_ENC_Z_LOST` | joint{} error: slave MCU detected encoder Z reference signal lost |  |  |
| 10480 | `EX_JOINT_EC_SLAVE_COMM_OVER_TM` | joint{} error: slave MCU detected master-slave communication timeout |  |  |
| 10481 | `EX_JOINT_EC_SLAVE_TRQ_ERR` | joint{} error: slave MCU detected master-slave motor torque deviation out of range |  |  |
| 10482 | `EX_JOINT_EC_BRAKE_TYPE_ERR` | joint{} error: brake type config mismatch with hardware |  |  |
| 10483 | `EX_JOINT_EC_PHASE_ALIGN` | joint{} error: phase alignment failed |  |  |
| 10484 | `EX_JOINT_EC_POS_OVER_LIMIT_WARNING` | joint{} error: position over limit |  |  |
| 10485 | `EX_JOINT_EC_PHASE_ALIGN_WARNING` | joint{} error: phase alignment warning |  |  |
| 10486 | `EX_JOINT_EC_TASK_STACK_SHORTAGE` | joint{} error: task stack insufficient |  |  |
| 10487 | `EX_JOINT_EC_TASK_STACK_OVERFLOW` | joint{} error: task stack overflow |  |  |
| 10488 | `EX_JOINT_EC_SERVO_STEP` | joint{} servo process step |  | For information only. No action required. |
| 10800 | `EX_JOINT_EC_UNKNOWN` | joint{} error: unknown error |  |  |

## 扩展轴错误 (11001 - 11066)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 11001 | `EXT_AXIS_ERR_COMMON` | ext axis{} error: common |  | Check communication with ext axis drive. |
| 11002 | `EXT_AXIS_ERR_OVER_CURRENT` | ext axis{} error: over current |  | Check wiring/short circuit; reboot; if repeated replace drive/motor. |
| 11003 | `EXT_AXIS_ERR_OVER_VOLTAGE` | ext axis{} error: over voltage |  | Check DC supply, regen, energy eater; reboot. |
| 11004 | `EXT_AXIS_ERR_LOW_VOLTAGE` | ext axis{} error: low voltage |  | Check DC supply and cabling; reboot. |
| 11005 | `EXT_AXIS_ERR_OVER_TEMP` | ext axis{} error: over temperature |  | Check environment/cooling; reboot. |
| 11006 | `EXT_AXIS_ERR_HALL` | ext axis{} error: hall fault |  | Check hall sensor and motor cabling. |
| 11007 | `EXT_AXIS_ERR_ENCODER` | ext axis{} error: encoder fault |  | Check encoder connection/cable/noise. |
| 11008 | `EXT_AXIS_ERR_ABS_ENCODER` | ext axis{} error: absolute encoder fault |  | Check abs encoder power/cable; reboot. |
| 11009 | `EXT_AXIS_ERR_CUR_CALIB` | ext axis{} error: current calibration fault |  | Reboot; check current sensing circuit. |
| 11010 | `EXT_AXIS_ERR_Q_CURRENT` | ext axis{} error: current detect fault |  | Reboot; check current sensing circuit. |
| 11011 | `EXT_AXIS_ERR_ENC_POLL` | ext axis{} error: encoder pollution |  | Check encoder contamination/noise; improve shielding. |
| 11012 | `EXT_AXIS_ERR_ENC_Z_SIGNAL` | ext axis{} error: encoder Z signal fault |  | Check encoder Z channel and wiring. |
| 11013 | `EXT_AXIS_ERR_ENC_CAL` | ext axis{} error: encoder calibrate invalid |  | Redo calibration; check encoder. |
| 11014 | `EXT_AXIS_ERR_IMU` | ext axis{} error: IMU fault |  | Check IMU sensor and connection. |
| 11015 | `EXT_AXIS_ERR_TEMP_SENSOR` | ext axis{} error: temperature sensor fault |  | Check temp sensor wiring; reboot. |
| 11016 | `EXT_AXIS_ERR_ECAT_BUS` | ext axis{} error: EtherCAT bus error |  | Check EtherCAT cabling/topology/sync; reboot master/drive. |
| 11017 | `EXT_AXIS_ERR_ECAT_CONFIG` | ext axis{} error: EtherCAT config/ESI/SM/PDO fault |  | Check ESI, Mailbox/SM/PDO mapping, vendor/product/revision match. |
| 11018 | `EXT_AXIS_ERR_ECAT_SYNC` | ext axis{} error: EtherCAT sync/frame/period fault |  | Check DC sync, cycle time, frame loss; verify NIC/IRQ affinity. |
| 11019 | `EXT_AXIS_ERR_SYS_CUR` | ext axis{} error: system current fault |  | Check current loop and load; reboot. |
| 11020 | `EXT_AXIS_ERR_SYS_POS` | ext axis{} error: position out of range |  | Check encoder/scale/limits; reboot. |
| 11021 | `EXT_AXIS_ERR_OVER_SPEED` | ext axis{} error: over speed |  | Check command limits and tuning parameters. |
| 11022 | `EXT_AXIS_ERR_OVER_ACC` | ext axis{} error: over acceleration |  | Reduce acceleration/jerk; check tuning. |
| 11023 | `EXT_AXIS_ERR_FOLLOW_ERROR` | ext axis{} error: following error |  | Check gains, load, saturation; verify feedback. |
| 11024 | `EXT_AXIS_ERR_TAG_POS_OVER` | ext axis{} error: target position out of range |  | Check target limits and homing. |
| 11025 | `EXT_AXIS_ERR_TAG_SPEED_OVER` | ext axis{} error: target speed out of range |  | Clamp speed; check profile settings. |
| 11026 | `EXT_AXIS_ERR_TAG_CURRENT_OVER` | ext axis{} error: target current out of range |  | Clamp current/torque; check load. |
| 11027 | `EXT_AXIS_ERR_COLLISION` | ext axis{} error: collision |  | Remove obstruction; check torque/force limits. |
| 11028 | `EXT_AXIS_ERR_ADC_ZERO_OFFSET` | ext axis{} error: ADC zero offset |  | Reboot; check ADC/current sensor offset. |
| 11029 | `EXT_AXIS_ERR_IPM_NTC` | ext axis{} error: IPM NTC fault |  | Check power module temperature sensing. |
| 11030 | `EXT_AXIS_ERR_SHORT_CIRCUIT` | ext axis{} error: short circuit |  | Check motor phase wiring; insulation test. |
| 11031 | `EXT_AXIS_ERR_MOTOR_STALL` | ext axis{} error: motor stall |  | Check mechanical jam/load; reduce accel; reboot. |
| 11032 | `EXT_AXIS_ERR_ABS_MULTITURN` | ext axis{} error: abs encoder multiturn fault |  | Check abs encoder battery/params; reboot. |
| 11033 | `EXT_AXIS_ERR_PHASE_LOST` | ext axis{} error: motor phase lost |  | Check phase wiring/connector; measure continuity. |
| 11034 | `EXT_AXIS_ERR_BRAKE` | ext axis{} error: brake fault |  | Check brake wiring/power; verify brake release. |
| 11035 | `EXT_AXIS_ERR_REDUCER_OVER_TEMP` | ext axis{} error: reducer over temperature |  | Check reducer temperature/cooling. |
| 11036 | `EXT_AXIS_ERR_REDUCER_NTC` | ext axis{} error: reducer NTC fault |  | Check reducer temperature sensor. |
| 11037 | `EXT_AXIS_ERR_FIRMWARE_UPDATE` | ext axis{} error: firmware update fault |  | Retry update; check power stability. |
| 11038 | `EXT_AXIS_ERR_FLASH_OP` | ext axis{} error: flash operation fault |  | Retry; if persistent replace drive. |
| 11039 | `EXT_AXIS_ERR_EXT_ABS_ENC` | ext axis{} error: motor-side abs encoder comm fault |  | Check external abs encoder link/power. |
| 11040 | `EXT_AXIS_ERR_DRIVE_FAULT` | ext axis{} error: drive fault |  | Check drive alarm code; reboot; replace if repeated. |
| 11041 | `EXT_AXIS_ERR_OVERLOAD` | ext axis{} error: overload |  | Reduce load; check mechanics and tuning. |
| 11042 | `EXT_AXIS_ERR_HARDWARE_LIMIT` | ext axis{} error: hardware limit triggered |  | Move away from limit; check limit switch. |
| 11043 | `EXT_AXIS_ERR_SERVO_MODE_TIMEOUT` | ext axis{} error: switch servo mode timeout |  | Check mode transition and comm; reboot. |
| 11044 | `EXT_AXIS_ERR_UVW_ABZ` | ext axis{} error: UVW/ABZ fault |  | Check phase/encoder signals wiring. |
| 11045 | `EXT_AXIS_ERR_BATTERY_LOW` | ext axis{} error: battery low |  | Replace encoder battery; reboot. |
| 11046 | `EXT_AXIS_ERR_PHASE_ALIGN` | ext axis{} error: phase align fail |  | Redo phase alignment; check motor params. |
| 11047 | `EXT_AXIS_ERR_POS_DISCONTINUOUS` | ext axis{} error: position command discontinuous |  | Check trajectory generation and limits. |
| 11048 | `EXT_AXIS_ERR_POS_INIT` | ext axis{} error: position initialization failure |  | Check encoder init/homing procedure. |
| 11049 | `EXT_AXIS_ERR_TORQUE_SENSOR` | ext axis{} error: torque sensor fault |  | Check torque sensor wiring/calibration. |
| 11050 | `EXT_AXIS_ERR_ABS_ENC_LOW_VOLT` | ext axis{} error: abs encoder low voltage |  | Check encoder supply voltage and cable. |
| 11051 | `EXT_AXIS_ERR_OFFLINE` | ext axis{} error: ext axis offline |  | Check hardware and axis id; check EtherCAT state. |
| 11052 | `EXT_AXIS_ERR_BOOTLOADER` | ext axis{} error: ext axis in bootloader |  | Retry firmware update. |
| 11053 | `EXT_AXIS_ERR_SLAVE_OFFLINE` | ext axis slave{} error: slave offline |  | Check slave hardware and id. |
| 11054 | `EXT_AXIS_ERR_SLAVE_BOOTLOADER` | ext axis slave{} error: slave in bootloader |  | Retry firmware update. |
| 11055 | `EXT_AXIS_ERR_EEPROM` | ext axis{} error: EEPROM/param store fault |  | Check EEPROM/parameter storage; power cycle. |
| 11056 | `EXT_AXIS_ERR_PARAM_CONFIG` | ext axis{} error: parameter/config fault |  | Verify parameter set; restore defaults if needed. |
| 11057 | `EXT_AXIS_ERR_STO` | ext axis{} error: STO safety fault |  | Check STO wiring/safety chain; reset safety. |
| 11058 | `EXT_AXIS_ERR_ENCRYPT_CHIP` | ext axis{} error: encrypt chip/key fault |  | Check encryption chip/keys/firmware compatibility. |
| 11059 | `EXT_AXIS_ERR_BRAKE_RES_OVERLOAD` | ext axis{} error: brake resistor overload |  | Check brake resistor/regen circuit; duty cycle. |
| 11060 | `EXT_AXIS_ERR_POWER_LINE_OPEN` | ext axis{} error: motor power line open |  | Check motor power cable continuity/connector. |
| 11061 | `EXT_AXIS_ERR_HOMING` | ext axis{} error: homing fault |  | Check homing sensor/origin procedure; retry. |
| 11062 | `EXT_AXIS_ERR_TUNING_FAIL` | ext axis{} error: tuning fail |  | Redo tuning; reduce resonance; check mechanics. |
| 11063 | `EXT_AXIS_ERR_INERTIA_ID_FAIL` | ext axis{} error: inertia identification fail |  | Check load; redo inertia ID; adjust conditions. |
| 11064 | `EXT_AXIS_ERR_FLYAWAY` | ext axis{} error: flyaway |  | Emergency stop; check feedback polarity/scale; inspect drive params. |
| 11065 | `EXT_AXIS_ERR_SPEED_PULSE_OVER` | ext axis{} error: feedback pulse overspeed |  | Check encoder feedback and scaling; reduce speed. |
| 11066 | `EXT_AXIS_ERR_CTRL_LOOP` | ext axis{} error: control loop/timeout fault |  | Check sampling/current loop/comm timeout; reboot. |

## 安全接口板错误 (20001 - 20027)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 20001 | `IFB_ERR_ROBOTTYPE` | Robot error type! |  |  |
| 20002 | `IFB_ERR_ADXL_SENS` | Base Acceleration sensor error! |  |  |
| 20003 | `IFB_ERR_EN_LINE` | Encoder line error! |  |  |
| 20004 | `IFB_ERR_ENTER_HDG_MODE` | Robot enter handguide mode! |  |  |
| 20005 | `IFB_ERR_EXIT_HDG_MODE` | Robot exit handguide mode! |  |  |
| 20006 | `IFB_ERR_MAC_DATA_BREAK` | MAC data break! |  |  |
| 20007 | `IFB_ERR_DRV_FIRMWARE_VERSION` | Motor driver firmware version error! |  |  |
| 20008 | `INIT_ERR_EN_DRV` | Motor driver enable failed! |  |  |
| 20009 | `INIT_ERR_EN_AUTO_BACK` | Motor driver enable auto back failed! |  |  |
| 20010 | `INIT_ERR_EN_CUR_LOOP` | Motor driver enable current loop failed! |  |  |
| 20011 | `INIT_ERR_SET_TAG_CUR` | Motor driver set target current failed! |  |  |
| 20012 | `INIT_ERR_RELEASE_BRAKE` | Motor driver release brake failed! |  |  |
| 20013 | `INIT_ERR_EN_POS_LOOP` | Motor driver enable postion loop failed! |  |  |
| 20014 | `INIT_ERR_SET_MAX_ACC` | Motor set max accelerate failed! |  |  |
| 20015 | `SAFETY_ERR_PROTECTION_STOP_TIMEOUT` | Protective stop timeout! |  |  |
| 20016 | `SAFETY_ERR_REDUCED_MODE_TIMEOUT` | Reduced mode timeout! |  |  |
| 20017 | `SYS_ERR_MCU_COM` | Robot system error: mcu communication error! |  |  |
| 20018 | `SYS_ERR_RS485_COM` | Robot system error: RS485 communication error! |  |  |
| 20019 | `IFB_ERR_DISCONNECTED` | Interface board may be disconnected. Please check connection between IPC and Interface board. |  |  |
| 20020 | `IFB_ERR_PAYLOAD_ERROR` | Payload error. |  |  |
| 20021 | `IFB_OFFLINE` | ifaceboard error: ifaceboard may be offline |  | (a) Check ifaceboard's hardware. (b) Check ifaceboard's id. |
| 20022 | `IFB_ERR_BOOTLOADER` | ifaceboard error: The ifaceboard is in bootloader mode. Retry firmware update. |  |  |
| 20023 | `IFB_SLAVE_OFFLINE` | interface slave board error: interface slave board may be offline |  | (a) Check interface slave board's hardware. (b) Check interface slave board's id. |
| 20024 | `IFB_SLAVE_ERR_BOOTLOADER` | interface slave board error: The interface slave board is in bootloader mode. Retry firmware update. |  |  |
| 20025 | `IFB_TOOL_ERR_ADXL_SENS` | Tool Acceleration sensor error! |  |  |
| 20026 | `HANDLE_OFFLINE` | handle error: handle may be offline |  | (a) Check handle's hardware. (b) Check handle's id. |
| 20027 | `HANDLE_ERR_BOOTLOADER` | handle error: The handle is in bootloader mode. Retry firmware update. |  |  |

## 运行时错误 (30001 - 30444)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 30001 | `ROBOT_BE_PULLING` | Something is pulling the robot. |  | Please check TCP configuration,payload and mounting settings |
| 30002 | `PSTOP_ELBOW_POS` | Protective Stop: Elbow position close to safety plane limits. |  | Please move robot Elbow joint away from the safety plane |
| 30003 | `PSTOP_STOP_TIME` | Protective Stop: Exceeding user safety settings for stopping time. |  | (a) Check speeds and accelerations in the program (b) Check usage of TCP,payload and CoG correctly (c) Check external equipmentactivation if correctly set |
| 30004 | `PSTOP_STOP_DISTANCE` | Protective Stop: Exceeding user safety settings for stopping distance. |  | (a) Check speeds and accelerations in the program (b) Check usage of TCP,payload and CoG correctly (c) Check external equipmentactivation if correctly set |
| 30005 | `PSTOP_CLAMP` | Protective Stop: Danger of clamping between the Robot’s lower arm and tool. |  | (a) Check speeds and accelerations in the program (b) Check usage of TCP,payload and CoG correctly (c) Check external equipmentactivation if correctly set |
| 30006 | `PSTOP_POS_LIMIT` | Protective Stop: Position close to joint limits |  |  |
| 30007 | `PSTOP_ORI_LIMIT` | Protective Stop: Tool orientation close to limits |  |  |
| 30008 | `PSTOP_PLANE_LIMIT` | Protective Stop: Position close to safety plane limits |  |  |
| 30009 | `PSTOP_POS_DEVIATE` | Protective Stop: Position deviates from path |  | Check payload, center of gravity and acceleration settings. |
| 30010 | `JOINT_CHK_PAYLOAD` | Joint {}: Check payload, center of gravity and acceleration settings. Log screen may contain additional information. |  |  |
| 30011 | `PSTOP_SINGULARITY` | Protective Stop: Position in singularity. |  | Please use MoveJ or change the motion |
| 30012 | `PSTOP_CANNOT_MAINTAIN` | Protective Stop: Robot cannot maintain its position, check if payload is correct |  |  |
| 30013 | `PSTOP_WRONG_PAYLOAD` | Protective Stop: Wrong payload or mounting detected, or something is pushing the robot when entering Freedrive mode |  | Verify that the TCP configuration and mounting in the used installation is correct |
| 30014 | `PSTOP_JOINT_COLLISION` | Protective Stop: Collision detected by joint {} |  | Make sure no objects are in the path of the robot and resume the program |
| 30015 | `PSTOP_POS_DISAGREE` | Protective stop: The robot was powered off last time due to a joint position disagreement. |  | (a) Verify that the robot position in the 3D graphics matches the real robot, to ensure that the encoders function before releasing the brakes. Stand back and monitor the robot performing its first program cycle as expected. (b) If the position is not correct, the robot must be repaired. In this case, click Power Off Robot. (c) If the position is correct, please tick the check box below the 3D graphics and click Robot Position Verified |
| 30016 | `TARGET_JOINT_SPEED_EXCEED` | Target joint speed exceed limits |  |  |
| 30017 | `TARGET_POS_SUDDEN_CHG` | Sudden change in target position |  |  |
| 30018 | `SUDDEN_STOP` | Sudden stop. |  | To abort a motion, use "stopj" or "stopl" script commands to generate a smooth deceleration before using "wait". Avoid aborting motions between waypoints with blend” |
| 30019 | `ROBOT_STOP_ABNORMAL` | Robot has not stopped in the allowed reaction and braking time |  |  |
| 30020 | `PROG_INVALID_SETP` | Robot program resulted in invalid setpoint. |  | Please review waypoints in the program |
| 30021 | `BLEND_INVALID_SETP` | Blending failed and resulted in an invalid setpoint. |  | Try changing the blend radius or contact technical support |
| 30022 | `APPROACH_SINGULARITY` | Robot approaching singularity – Acceleration threshold failed. |  | Review waypoints in the program, try using MoveJ instead of MoveL in the position close to singularity |
| 30023 | `TSPEED_UNMATCH_POS` | Target speed does not match target position |  |  |
| 30024 | `INCONSIS_TPOS_SPD` | Inconsistency between target position and speed |  |  |
| 30025 | `JOINT_TSPD_UNMATCH_POS` | Target joint speed does not match target joint position change – Joint {} |  |  |
| 30026 | `FIELDBUS_INPUT_DISCONN` | Fieldbus input disconnected. |  | Please check fieldbus connections (RTDE, ModBus, EtherNet/IP and Profinet) or disable the fieldbus in the installation. Check RTDE watchdog feature. Check if a URCap is using this feature. |
| 30027 | `OPMODE_CHANGED` | Operational mode changed: {} |  |  |
| 30028 | `NO_KIN_CALIB` | No Kinematic Calibration found (calibration.conf file is either corrupt or missing). |  | A new kinematics calibration may be needed if the robot needs to improve its kinematics, otherwise, ignore this message) |
| 30029 | `KIN_CALIB_UNMATCH_JOINT` | Kinematic Calibration for the robot does not match the joint(s). |  | If moving a program from a different robot to this one, rekinematic calibrate the second robot to improve kinematics, otherwise ignore this message. |
| 30030 | `KIN_CALIB_UNMATCH_ROBOT` | Kinematic Calibration does not match the robot. |  | Please check if the serial number of the robot arm matches the Control Box |
| 30031 | `JOINT_OFFSET_CHANGED` | Large movement of the robot detected while it was powered off. The joints were moved while it was powered off, or the encoders do not function |  |  |
| 30032 | `OFFSET_CHANGE_HIGH` | Change in offset is too high |  |  |
| 30033 | `JOINT_SPEED_LIMIT` | Close to joint speed safety limit. |  | Review program speed and acceleration |
| 30034 | `TOOL_SPEED_LIMIT` | Close to tool speed safety limit. |  | Review program speed and acceleration |
| 30035 | `MOMENTUM_LIMIT` | Close to momentum safety limit. |  | Review program speed and acceleration |
| 30036 | `ROBOT_MV_STOP` | Robot is moving when in Stop Mode |  |  |
| 30037 | `HAND_PROTECTION` | Hand protection: Tool is too close to the lower arm: {} meter. |  | (a) Check wrist position. (b) Verify mounting (c) Do a Complete rebooting sequence (d) Update software (e) Contact your local AUBO Robots service provider for assistance |
| 30038 | `WRONG_SAFETYMODE` | Wrong safety mode: {} |  |  |
| 30039 | `SAFETYMODE_CHANGED` | Safety mode changed: {} |  |  |
| 30040 | `JOINT_ACC_LIMIT` | Close to joint acceleration safety limit |  |  |
| 30041 | `TOOL_ACC_LIMIT` | Close to tool acceleration safety limit |  |  |
| 30042 | `JOINT_TEMPERATURE_LIMIT` | Joint {} temperature too high(>{}℃) |  |  |
| 30043 | `CONTROL_BOX_TEMPERATURE_LIMIT` | Control box temperature too high(>{}℃) |  |  |
| 30044 | `ROBOT_EMERGENCY_STOP` | Robot emergency stop |  |  |
| 30045 | `ROBOTMODE_CHANGED` | Robot mode changed: {} |  |  |
| 30046 | `ROBOTMODE_ERROR` | Wrong robot mode: {} |  |  |
| 30047 | `POSE_OUT_OF_REACH` | Target pose [{}] out of reach |  |  |
| 30048 | `TP_PLAN_FAILED` | Trajectory plan FAILED. |  |  |
| 30049 | `START_FORCE_FAILED` | Start force control failed, because force sensor does not exist. |  |  |
| 30050 | `OVER_SAFE_PLANE_LIMIT` | {} axis exceeds the safety plane limit (Move_type:{} id:{}). |  | Please move the robot to the safety plane range. |
| 30051 | `POWERON_FAIL_VIOLATION` | Failed to power on because the robot safety mode is in violation |  |  |
| 30052 | `POWERON_FAIL_SYSTEMEMERGENCYSTOP` | Failed to power on because the robot safety mode is in system emergency stop |  |  |
| 30053 | `POWERON_FAIL_ROBOTEMERGENCYSTOP` | Failed to power on because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30054 | `POWERON_FAIL_FAULT` | Failed to power on because the robot safety mode is in fault |  |  |
| 30055 | `STARTUP_FAIL_VIOLATION` | Failed to startup because the robot safety mode is in violation |  |  |
| 30056 | `STARTUP_FAIL_SYSTEMEMERGENCYSTOP` | Failed to startup because the robot safety mode is in system emergency stop |  |  |
| 30057 | `STARTUP_FAIL_ROBOTEMERGENCYSTOP` | Failed to startup because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30058 | `STARTUP_FAIL_FAULT` | Failed to startup because the robot safety mode is in fault |  |  |
| 30059 | `BACKDRIVE_FAIL_VIOLATION` | Failed to backdrive because the robot safety mode is in violation |  |  |
| 30060 | `BACKDRIVE_FAIL_SYSTEMEMERGENCYSTOP` | Failed to backdrive because the robot safety mode is in system emergency stop |  |  |
| 30061 | `BACKDRIVE_FAIL_ROBOTEMERGENCYSTOP` | Failed to backdrive because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30062 | `BACKDRIVE_FAIL_FAULT` | Failed to backdrive because the robot safety mode is in fault |  |  |
| 30063 | `SETSIM_FAIL_VIOLATION` | Switch sim mode failed because the robot safety mode is in violation |  |  |
| 30064 | `SETSIM_FAIL_SYSTEMEMERGENCYSTOP` | Switch sim mode failed because the robot safety mode is in system emergency stop |  |  |
| 30065 | `SETSIM_FAIL_ROBOTEMERGENCYSTOP` | Switch sim mode failed because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30066 | `SETSIM_FAIL_FAULT` | Switch sim mode failed because the robot safety mode is in fault |  |  |
| 30067 | `FREEDRIVE_FAIL_VIOLATION` | Enable handguide mode failed because the robot safety mode is in violation |  |  |
| 30068 | `FREEDRIVE_FAIL_SYSTEMEMERGENCYSTOP` | Enable handguide mode failed because the robot safety mode is in system emergency stop |  |  |
| 30069 | `FREEDRIVE_FAIL_ROBOTEMERGENCYSTOP` | Enable handguide mode failed because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30070 | `FREEDRIVE_FAIL_FAULT` | Enable handguide mode failed because the robot safety mode is in fault |  |  |
| 30071 | `UPFIRMWARE_FAIL_VIOLATION` | Firmware update failed because the robot safety mode is in violation |  |  |
| 30072 | `UPFIRMWARE_FAIL_SYSTEMEMERGENCYSTOP` | Firmware update failed because the robot safety mode is in system emergency stop |  |  |
| 30073 | `UPFIRMWARE_FAIL_ROBOTEMERGENCYSTOP` | Firmware update failed because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30074 | `UPFIRMWARE_FAIL_FAULT` | Firmware update failed because the robot safety mode is in fault |  |  |
| 30075 | `SETPERSOSTENT_FAIL_VIOLATION` | Set persistent parameter failed because the robot safety mode is in violation |  |  |
| 30076 | `SETPERSOSTENT_FAIL_SYSTEMEMERGENCYSTOP` | Set persistent parameter failed because the robot safety mode is in system emergency stop |  |  |
| 30077 | `SETPERSOSTENT_FAIL_ROBOTEMERGENCYSTOP` | Set persistent parameter failed because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30078 | `SETPERSOSTENT_FAIL_FAULT` | Set persistent parameter failed because the robot safety mode is in fault |  |  |
| 30079 | `SETPERSOSTENT_FAIL_PARAM_ERR` | Set persistent parameter failed |  | (a) Check the parameter format, whether all are floating point numbers |
| 30080 | `ROBOT_CABLE_DISCONN` | Robot cable not connected |  | (a) Make sure the cable between Control Box and Robot Arm is correctly connected and it has no damage. (b) Check for loose connections (c) Do a Complete rebooting sequence (d) Update software (e) Contact your local AUBO Robots service provider for assistance Contact your local AUBO Robots service provider for assistance. |
| 30081 | `TP_TOO_SHORT` | The generated trajectory is ignored because it is too short |  | (a) Please check if the added waypoints are coincident (b) If it is an arc movement, please check whether the three points are collinear |
| 30082 | `INV_KIN_FAIL` | Inverse kinematics solution failed. The target pose may be in a singular position or exceed the joint limits |  | (a) Change the target pose and try moving again |
| 30083 | `FREEDRIVE_ENABLED` | Freedrive status changed to {} |  |  |
| 30084 | `TP_INV_FAIL_REFERENCE_JOINT_OUT_OF_LIMIT` | Inverse kinematics solution failed. Reference angle [{}] exceeds joint limit [{}]. |  |  |
| 30085 | `TP_INV_FAIL_NO_SOLUTION` | Inverse kinematics solution failed. The reference angle [{}] and the target angle [{}] are used as parameters. there is no solution in the calculation of the inverse solution process. |  |  |
| 30086 | `SERVO_FAIL_VIOLATION` | Switch servo mode failed because the robot safety mode is in violation |  |  |
| 30087 | `SERVO_FAIL_SYSTEMEMERGENCYSTOP` | Switch servo mode failed because the robot safety mode is in system emergency stop |  |  |
| 30088 | `SERVO_FAIL_ROBOTEMERGENCYSTOP` | Switch servo mode failed because the robot safety mode is in robot emergency stop |  | Pop up the red emergency stop button on the teach pendant when the robot is in a safe range of motion |
| 30089 | `SERVO_FAIL_FAULT` | Switch servo mode failed because the robot safety mode is in fault |  |  |
| 30090 | `FREEDRIVE_FAIL_NO_RUNNING` | Enable handguide mode failed because the robot mode type is {}(not running) |  |  |
| 30091 | `RUNTIME_MACHINE_ERROR` | The state of the running machine is {}, not {}. {} function execution failed because the state is wrong. |  |  |
| 30092 | `RESUME_FAR_PAUSE_PT` | Cannot resume from joint position [{}].\nToo far away from paused point [{}]. |  |  |
| 30093 | `PAYLOAD_LIGHTER_ERROR` | The payload setting is too small! |  |  |
| 30094 | `PAYLOAD_OVERLOAD_ERROR` | The payload setting is too large! |  |  |
| 30095 | `PAUSE_FAIL_NOT_POSITION_PLAN_MODE` | This motion does not support the pause function. The motion is stopping. |  |  |
| 30096 | `TP_PLAN_FAILED_CIRCULAR_WAYPOINTS_COINCIDE` | The planning failed because the three waypoints of the arc were determined to coincide. |  | Check the circular waypoints to make sure they are different. |
| 30097 | `SERVO_WRONG_SAFETYMODE` | Switch servo mode failed because the robot safety mode is in {}. |  | Check the circular waypoints to make sure they are different. |
| 30098 | `SET_PERSTPARAM_WRONG_SAFETYMODE` | Set persistent parameter failed because the robot safety mode is in {} |  |  |
| 30099 | `SET_KINPARAM_WRONG_SAFETYMODE` | Set Kinematics Compensate parameters failed because the robot safety mode is in {} |  |  |
| 30100 | `SET_ROBOT_ZERO_WRONG_SAFETYMODE` | Set current joint angles to zero failed because the robot safety mode is in {} |  |  |
| 30101 | `UPFIRMWARE_WRONG_SAFETYMODE` | Firmware update failed because the robot safety mode is in {} |  |  |
| 30102 | `POWERON_WRONG_SAFETYMODE` | Failed to power on because the robot safety mode is in {} |  |  |
| 30103 | `STARTUP_WRONG_SAFETYMODE` | Failed to startup because the robot safety mode is in {} |  |  |
| 30104 | `BACKDRIVE_WRONG_SAFETYMODE` | Failed to backdrive because the robot safety mode is in system emergency stop |  |  |
| 30105 | `SETSIM_WRONG_SAFETYMODE` | Switch sim mode failed because the robot safety mode is in violation |  |  |
| 30106 | `FREEDRIVE_WRONG_SAFETYMODE` | Enable handguide mode failed because the robot safety mode is in wrong safety mode: {} |  |  |
| 30107 | `TP_PLAN_FAILED_JOINT_JUMP_BIGGER` | Inverse kinematics solution failed. The target point and the current point are in different robot configuration spaces. |  | Add a few more points between the target point and the current point. |
| 30108 | `RUN_PROGRAM_FAILED` | Run program {} failed. |  |  |
| 30109 | `FREEDRIVE_FAIL_WRONG_RTMSTATE` | Unable to enter the HandGuide mode as the robot is not currently in a stopped or paused state. |  |  |
| 30110 | `SAFEGUARDSTOP_CONFIGURABLE_INPUT` | Configurable safety input is triggered. |  |  |
| 30111 | `SAFEGUARDSTOP_3PE` | 3PE is triggered. |  |  |
| 30112 | `SAFEGUARDSTOP_SI` | SI0/SI1 is triggered. |  |  |
| 30200 | `ROBOT_TYPE_CHANGED` | Robot type changed to '{}', and robot subtype changed to '{}' |  |  |
| 30201 | `LINKMODE_CHANGED` | Link mode changed to {} |  |  |
| 30301 | `ROBOT_SELF_COLLISION` | Detect risk of robot self collision |  |  |
| 30302 | `CONSTANT_INVALID` | Joint torque constants are invalid. HandGuide will be disabled, and the collision protection may be triggered by mistake. |  |  |
| 30303 | `GRAVITY_INVALID` | Abnormal value of gravity acceleration sensor. HandGuide will be disabled, and the collision protection may be triggered by mistake. |  |  |
| 30304 | `DYNAMICS_INVALID` | Robot dynamics parameters are invalid. HandGuide will be disabled, and the collision protection may be triggered by mistake. |  |  |
| 30305 | `FRICTION_INVALID` | Joint friction parameters are invalid. HandGuide will be disabled, and the collision protection may be triggered by mistake. |  |  |
| 30306 | `HANDGUIDE_UNDER_DEVELOP` | Robot type of {} function under development. HandGuide will be disabled, and the collision protection may be triggered by mistake. |  |  |
| 30307 | `SLOW_DOWN_INFO` | Slow down level changed to {}({}%) |  |  |
| 30308 | `WRONG_JOINT_DESIGNED_LIMIT` | Joint designed ranges exceeds ranges read from hardware interface. |  |  |
| 30309 | `FREEDRIVE_IN_SIMULATION` | Enable handguide mode failed because the robot is in simulation mode. |  |  |
| 30310 | `ROBOT_STOPPING_TIMEOUT` | Robot stopping timeout. |  |  |
| 30311 | `PSTOP_INCORRECT_FORCE_OFFSET` | Protective Stop: Sudden change in force control target position. Force sensor offset may be incorrect or force sensor fault. |  |  |
| 30312 | `WRONG_JOINT_SAFETY_LIMIT` | Joint safety ranges exceeds designed ranges. |  |  |
| 30401 | `PSTOP_TCP_PLANE_VIOLATION` | Protective Stop: TCP position close to safety plane limits. |  |  |
| 30402 | `PSTOP_ELBOW_PLANE_VIOLATION` | Protective Stop: elbow position close to safety plane limits. |  |  |
| 30403 | `PSTOP_JOINT_TORQUE_VIOLATION` | Protective Stop: joint{} exceeds torque limit. |  |  |
| 30404 | `PSTOP_JOINT_POSITION_VIOLATION` | Protective Stop: joint{} exceeds position limit. |  |  |
| 30405 | `PSTOP_JOINT_SPEED_VIOLATION` | Protective Stop: joint{} exceeds speed limit. |  |  |
| 30406 | `PSTOP_TCP_SPEED_VIOLATION` | Protective Stop: TCP speed close to safety limits. |  |  |
| 30407 | `PSTOP_ELBOW_SPEED_VIOLATION` | Protective Stop: elbow speed close to safety limits. |  |  |
| 30408 | `PSTOP_TCP_FORCE_VIOLATION` | Protective Stop: TCP foece close to safety limits. |  |  |
| 30409 | `PSTOP_ELBOW_TORQUE_VIOLATION` | Protective Stop: elbow torque close to safety limits. |  |  |
| 30410 | `PSTOP_POWER_VIOLATION` | Protective Stop: robot power close to safety limits. |  |  |
| 30411 | `PSTOP_MOMENTUM_VIOLATION` | Protective Stop: robot momentum close to safety limits. |  |  |
| 30412 | `PSTOP_TCP_CUBE_VIOLATION` | Protective Stop: TCP position close to safety cube. |  |  |
| 30413 | `PSTOP_ELBOW_CUBE_VIOLATION` | Protective Stop: TCP position close to safety cube. |  |  |
| 30414 | `REDUCE_ELBOW_PLANE_TRIGGER` | Reduce mode: elbow close to safety plane triggers reduction mode. |  |  |
| 30415 | `REDUCE_TCP_PLANE_TRIGGER` | Reduce mode: TCP close to safety plane triggers reduction mode. |  |  |
| 30416 | `PSTOP_MOVE_OUT_RANGE` | Joint {} has exceeded the limit, please do not continue to move out of the range |  |  |
| 30417 | `RESUME_PAUSE_FAILED` | Resume Failed: Safety mode type is {} |  |  |
| 30418 | `FIRMWARE_UPDATE_FAIL_EMERGENCYSTOP` | Failed to firmware update because the robot safety mode is in {} |  | Release emergency stop when the robot is in a safe range of motion |
| 30419 | `TOOL_SENSOR_CHANGED` | Tool sensor type changed to {} |  |  |
| 30420 | `TOOL_SENSOR_REMOVED` | Tool sensor is removed. |  |  |
| 30421 | `CAL_TARGET_CURRENT_ERR` | The calculation of the target current failed. Please try again later. |  |  |
| 30422 | `CONVEYOR_MODE_CHANGED` | Conveyor{}: track mode changed to {}, track item id is {} |  |  |
| 30423 | `CONVEYOR_ENQUEUE` | Conveyor{}: the queue has been changed, item{} is enqueue |  |  |
| 30424 | `CONVEYOR_DEQUEUE_FINISH` | Conveyor{}: the queue has been changed, item{} dequeue due to track finished |  |  |
| 30425 | `CONVEYOR_DEQUEUE_STARTWINDOW` | Conveyor{}: the queue has been changed, item{} dequeue due to exceeds startwindow |  |  |
| 30426 | `CONVEYOR_DEQUEUE_LIMIT` | Conveyor{}: the queue has been changed, item{} dequeue due to exceed limit area |  |  |
| 30427 | `CONVEYOR_DEQUEUE_CLEAR` | Conveyor{}: item queue is cleared |  |  |
| 30428 | `CONVEYOR_NEXT_TRACK` | Conveyor{}: item{} inside the start window that can be tracked |  |  |
| 30429 | `CONVEYOR_EXCEED_LIMIT` | Conveyor{}: item{} exceeds the limit area during tracking |  |  |
| 30430 | `WRONG_POWER_SAFETY_LIMIT` | Robot power safety value exceeds designed value. |  |  |
| 30431 | `WRONG_POWER_DESIGNED_LIMIT` | Power designed value exceeds value read from hardware interface. |  |  |
| 30432 | `TOOL_SENSOR_STATUS_CHANGED` | Tool sensor status changed to {} |  |  |
| 30433 | `COLLISION_THRESHOLD_INVALID` | Robot collision threshold parameters are invalid.Please reidentify the threshold or modify the configuration to ensure that it does not cause accidental collisions. |  |  |
| 30434 | `GRIPPER_DISCONNECT` | The gripper {} is disconnected. |  |  |
| 30435 | `GRIPPER_UNKNOWN_FAULT` | There is an unknown fault with the gripper {} |  |  |
| 30436 | `GRIPPER_CURRENT_ANOMALY_FAULT` | There is an abnormal current fault with the gripper {} |  |  |
| 30437 | `GRIPPER_VOLTAGE_ANOMALY_FAULT` | There is an abnormal voltage fault with the gripper {} |  |  |
| 30438 | `GRIPPER_OVER_TEMPERATURE_FAULT` | There is an over-temperature fault with the gripper {} |  |  |
| 30439 | `GRIPPER_INTERNAL_FAULT` | There is an internal fault with the gripper {} |  |  |
| 30440 | `GRIPPER_COMMUNICATION_FAULT` | There is an communication fault with the gripper {} |  |  |
| 30441 | `GRIPPER_CONTROL_COMMAND_FAULT` | There is an control command fault with the gripper {} |  |  |
| 30442 | `GRIPPER_ENABLE_FAULT` | There is an enable fault with the gripper {} |  |  |
| 30443 | `WRIST_SINGULARITY_RISK` | Wrist singularity detected. Linear motion may cause excessive joint speed.Adjust robot posture to avoid J5 near 0° or 180°, or use joint motion instead of linear motion. |  |  |
| 30444 | `PSTOP_PATH_OFFSET_OVER_LIMIT` | Protective Stop: the offset of the robot has exceeded the limit. |  |  |

## 工具错误 (40001 - 40034)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 40001 | `TOOL_FLASH_VERIFY_FAILED` | Flash write verify failed |  |  |
| 40002 | `TOOL_PROGRAM_CRC_FAILED` | Program flash checksum failed during bootloading |  |  |
| 40003 | `TOOL_PROGRAM_CRC_FAILED2` | Program flash checksum failed at runtime |  |  |
| 40004 | `TOOL_ID_UNDIFINED` | Tool ID is undefined |  |  |
| 40005 | `TOOL_ILLEGAL_BL_CMD` | Illegal bootloader command |  |  |
| 40006 | `TOOL_FW_WRONG` | Wrong firmware at the joint |  |  |
| 40007 | `TOOL_HW_INVALID` | Invalid hardware revision |  |  |
| 40011 | `TOOL_SHORT_CURCUIT_H` | Short circuit detected on Digital Output: {} high side |  |  |
| 40012 | `TOOL_SHORT_CURCUIT_L` | Short circuit detected on Digital Output: {} low side |  |  |
| 40013 | `TOOL_AVERAGE_CURR_HIGH` | 10 second Average tool IO Current of {} A is outside of the allowed range. |  |  |
| 40014 | `TOOL_POWER_PIN_OVER_CURR` | Current of {} A on the POWER pin is outside of the allowed range. |  |  |
| 40015 | `TOOL_DOUT_PIN_OVER_CURR` | Current of {} A on the Digital Output pins is outside of the allowed range. |  |  |
| 40016 | `TOOL_GROUND_PIN_OVER_CURR` | Current of {} A on the ground pin is outside of the allowed range. |  |  |
| 40021 | `TOOL_RX_FRAMING` | RX framing error |  |  |
| 40022 | `TOOL_RX_PARITY` | RX Parity error |  |  |
| 40031 | `TOOL_48V_LOW` | 48V input is too low |  |  |
| 40032 | `TOOL_48V_HIGH` | 48V input is too high |  |  |
| 40033 | `TOOL_ERR_OFFLINE` | tool error: tool may be offline |  | (a) Check tool's hardware. (b) Check joint's id. |
| 40034 | `TOOL_ERR_BOOTLOADER` | tool error: The tool is in bootloader mode. Retry firmware update. |  |  |

## 工具扩展错误 (40101 - 40200)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 40101 | `EX_TOOL_EC_LOW_VOLTAGE` | tool error: low voltage |  | check tool power supply voltage |
| 40102 | `EX_TOOL_EC_FORCESENSOR_COMM` | tool error: external force sensor communication error |  | check external force sensor communication connection |
| 40103 | `EX_TOOL_485_SENDFULL_COMM` | tool error: 485 transparent transmission buffer full |  | check 485 communication load and transmission frequency |
| 40104 | `EX_TOOL_FORCESENSOR_FILTER_ZERO` | tool error: force sensor filter parameter is zero |  | check force sensor filter parameter configuration |
| 40200 | `EX_TOOL_EC_UNKNOWN` | tool error: unknown error |  | check controller logs and hardware status |

## 基座错误 (50001 - 50003)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 50001 | `PKG_LOST` | Lost package from pedestal |  |  |
| 50002 | `PEDSTRAL_OFFLINE` | pedestal error: pedestal may be offline |  | (a) Check pedestal's hardware. (b) Check pedestal's id. |
| 50003 | `PEDESTAL_ERR_BOOTLOADER` | pedestal error: The pedestal is in bootloader mode. Retry firmware update. |  |  |

## 基座扩展错误 (50101 - 50200)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 50101 | `EX_BASE_EC_LOW_VOLTAGE` | pedstral error: low voltage |  | check power supply voltage and battery status |
| 50102 | `EX_BASE_EC_OVER_TEMPERATURE_RES` | pedstral error: resistor over temperature |  | check braking resistor temperature and cooling condition |
| 50103 | `EX_BASE_EC_OVER_TARGET_BRAKE_OPEN_VOLT` | pedstral error: input voltage close to or exceeds regenerative brake activation voltage |  | check input power voltage and regenerative braking configuration |
| 50104 | `EX_BASE_EC_RES_BREAKAGE` | pedstral error: brake resistor breakage |  | check whether the brake resistor is disconnected or damaged |
| 50105 | `EX_BASE_EC_IMU_CALIBRATE` | pedstral error: IMU calibration required |  | perform IMU calibration according to maintenance procedure |
| 50106 | `EX_BASE_EC_TEMP_SENSOR_SHORT` | pedstral error: temperature sensor short circuit |  | check temperature sensor wiring and solder joints for short circuit |
| 50107 | `EX_BASE_EC_TEMP_SENSOR_BREAK` | pedstral error: temperature sensor open circuit |  | check temperature sensor connection and cable continuity |
| 50108 | `EX_BASE_EC_96V_INSTANT_OVER_VOLTAGE` | pedstral error: 96V instantaneous over voltage after regenerative braking |  | check regenerative braking behavior and power bus voltage |
| 50200 | `EX_BASE_EC_UNKNOWN` | pedstral error: unknown error |  | check controller logs and hardware status |

## 硬件接口错误 (60001 - 60080)

| 错误码 | 名称 | 描述 | 描述(中文) | 建议 |
|--------|------|------|------------|------|
| 60001 | `HW_SCB_SETUP_FAILED` | Setup of Interface Board failed |  |  |
| 60002 | `HW_PKG_CNT_DISAGEE` | Packet counter disagreements |  |  |
| 60003 | `HW_SCB_DISCONNECT` | Connection to Interface Board lost |  |  |
| 60004 | `HW_SCB_PKG_LOST` | Package lost from Interface Board |  |  |
| 60005 | `HW_SCB_CONN_INIT_FAILED` | Ethernet connection initialization with Interface Board failed |  |  |
| 60006 | `HW_LOST_JOINT_PKG` | Lost package from joint  {} |  |  |
| 60007 | `HW_LOST_TOOL_PKG` | Lost package from tool |  |  |
| 60008 | `HW_JOINT_PKG_CNT_DISAGREE` | Packet counter disagreement in packet from joint {} |  |  |
| 60009 | `HW_TOOL_PKG_CNT_DISAGREE` | Packet counter disagreement in packet from tool |  |  |
| 60011 | `HW_JOINTS_FAULT` | {} joint entered the Fault State |  |  |
| 60012 | `HW_JOINTS_VIOLATION` | {} joint entered the Violation State |  |  |
| 60013 | `HW_TP_FAULT` | Teach Pendant entered the Fault State |  |  |
| 60014 | `HW_TP_VIOLATION` | Teach Pendant entered the Violation State |  |  |
| 60021 | `HW_JOINT_MV_TOO_FAR` | {} joint moved too far before robot entered RUNNING State |  |  |
| 60022 | `HW_JOINT_STOP_NOT_FAST` | Joint Not stopping fast enough |  |  |
| 60023 | `HW_JOINT_MV_LIMIT` | Joint moved more than allowable limit |  |  |
| 60024 | `HW_FT_SENSOR_DATA_INVALID` | Force-Torque Sensor data invalid |  |  |
| 60025 | `HW_NO_FT_SENSOR` | Force-Torque sensor is expected, but it cannot be detected |  |  |
| 60026 | `HW_FT_SENSOR_NOT_CALIB` | Force-Torque sensor is detected but not calibrated |  |  |
| 60030 | `HW_RELEASE_BRAKE_FAILED` | Robot was not able to brake release, see log for details |  |  |
| 60040 | `HW_OVERCURR_SHUTDOWN` | Overcurrent shutdown |  |  |
| 60050 | `HW_ENERGEY_SURPLUS` | Energy surplus shutdown |  |  |
| 60060 | `HW_IDLE_POWER_HIGH` | Idle power consumption to high |  |  |
| 60071 | `HW_ENTER_COLLISION_TIMEOUT` | Enter collision stop procedure timeout |  |  |
| 60072 | `HW_POWERON_TIMEOUT` | Poweron robot timeout |  |  |
| 60073 | `HW_NO_NIC_FOUND` | No network cards found. |  |  |
| 60074 | `HW_IFB_NOT_FOUND` | No Interface Board found. |  |  |
| 60075 | `HW_IFB_BOOTLOAD` | The Interface Board is in bootloader mode. Update firmware firstly. |  |  |
| 60076 | `HW_TOOL_NOT_FOUND` | No Tool Board found. |  |  |
| 60077 | `HW_BASE_NOT_FOUND` | No Base Board found. |  |  |
| 60078 | `HW_BRINGUP_TIMEOUT` | Poweron robot timeout |  |  |
| 60079 | `HW_COLLISION_RECOVERY_FAILED` | Collision recovery failed |  |  |
| 60080 | `HW_TP_ENABLED` | Teach pendant enabled status changed to {} |  |  |

