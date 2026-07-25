# 外部参考资料入口

> 用途：统一登记 AUBO 真机接口、厂商网站、SDK 文档、算法资料和用户提供的本地文件。
>
> 原则：入口只记录可追溯来源，不用未确认链接或版本指导真机实现。

## 1. AUBO 真机与 SDK 资料

| 资料 | 链接或文件 | 版本/访问日期 | 状态 | 用途 |
|---|---|---|---|---|
| AUBO 官方网站 | 待用户提供或确认 | 待确认 | 待接入 | 厂商信息与官方资料总入口 |
| AUBO E5 产品资料 | 待用户提供或确认 | 待确认 | 待接入 | 机械臂规格、限制与安全信息 |
| Windows AUBO SDK 工具包 | [SDK 目录](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/)；[版本文件](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/VERSION) | `0.27.1-rc.1+a798877` | 已存档，来源属性待确认 | C/C++ 库、头文件、Python/C++ 示例、RTDE 和错误码 |
| Python SDK wheel | [pyaubo_sdk wheel](./aubosdk/AuboController/sdk/pyaubo_sdk-0.24.1-cp311-cp311-win_amd64.whl) | `0.24.1`、CPython 3.11、Windows AMD64 | 已存档，兼容性待验证 | Python 真机适配候选依赖 |
| RTDE 配方中文说明 | [rtde_recipe_中文说明.md](<./aubo docs/rtde_recipe_中文说明 .md>) | 待确认 | 已存档 | 状态反馈、寄存器、时间戳和夹爪状态字段 |
| SDK RTDE 文档 | [rtde_recipe.md](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/share/doc/rtde_recipe.md) | 随 SDK `0.27.1-rc.1` | 已存档 | RTDE 字段与协议参考 |
| SDK 错误码 | [error_code.md](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/share/doc/error_code.md) | 随 SDK `0.27.1-rc.1` | 已存档 | 故障处理、日志和安全状态映射 |
| Python 运动控制示例 | [example_motion_control.py](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/share/example/python/example_motion_control.py) | 随 SDK `0.27.1-rc.1` | 已存档 | 基础运动控制接口 |
| Python 关节伺服示例 | [example_servoj2.py](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/share/example/python/example_servoj2.py) | 随 SDK `0.27.1-rc.1` | 已存档 | 关节位置/速度控制候选实现参考 |
| Python TCP 伺服示例 | [example_servo_cartesian.py](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/share/example/python/example_servo_cartesian.py) | 随 SDK `0.27.1-rc.1` | 已存档 | TCP 增量控制候选实现参考 |
| Python RTDE 示例 | [example_rtde.py](./aubosdk/AuboController/sdktool/aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877/share/example/python/example_rtde.py) | 随 SDK `0.27.1-rc.1` | 已存档 | 实时状态订阅与通信参考 |
| 夹爪接口资料 | 待用户提供或确认 | 待确认 | 待接入 | 夹爪开合、状态反馈与抓取判断 |

工具包与 Python wheel 的当前版本不同，不能假定二者 API 完全一致。确定真机实现前必须完成导入、连接、状态读取、无动作 shadow mode 和受限动作 smoke test，并冻结实际采用的版本组合。

接入 SDK 时还需记录：

- SDK 名称、版本、发布日期和获取来源；
- 支持的操作系统、Python/C++ 版本与通信方式；
- 关节位置、关节速度和 TCP 控制接口；
- 状态反馈字段、时间戳和推荐控制频率；
- 超时、断连、急停、故障码和恢复语义；
- 真机程序实际调用的接口与参考资料之间的对应关系。

## 2. 算法与仿真入口

| 资料 | 链接或文件 | 状态 | 用途 |
|---|---|---|---|
| R2-Dreamer 官方代码 | <https://github.com/NM512/r2dreamer> | 已登记 | 算法配置与复现基线 |
| R2-Dreamer 本地论文 | [R2Dreamer.pdf](./papers/R2Dreamer.pdf) | 已存档 | 论文原文 |
| DreamerV3 本地资料 | [DReamerV3：通过世界模型掌握多领域.pdf](<./papers/DReamerV3：通过世界模型掌握多领域.pdf>) | 已存档 | 背景参考，不属于当前对照实验 |

## 3. 本地文件管理约定

用户提供的 PDF、SDK 手册、接口说明、示例代码说明和网页存档直接放入本目录，并在本文件相应表格中登记。

每项资料至少记录：

- 原始文件名或外部链接；
- 厂商、作者或维护方；
- 版本或 commit；
- 获取日期；
- 对应的项目用途；
- 是否为官方资料；
- 已知许可或再分发限制。

若资料被新版本替换，保留旧版本记录，并明确当前实现实际采用的版本。外部链接失效时优先补充官方替代入口，不用来源不明的镜像替代。
