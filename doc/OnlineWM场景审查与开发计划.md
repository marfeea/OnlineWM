# OnlineWM 场景审查与开发实施文档

> 文档版本：v1.0  
> 基线日期：2026-07-18  
> 项目对象：双 AUBO E5 TCP-docking 场景、DreamerV3 世界模型与 Controller、R2-Dreamer 表征分支  
> 当前结论：场景的纯配置迁移已经完成；Isaac Lab 运行时、可训练环境和世界模型尚未完成。  
> 证据规则：`已完成` 必须附带可重复的命令或产物；`阻塞` 必须写明解除条件；没有运行证据的代码只能标记为 `待验证`。

## 1. 文档用途与维护规则

本文件是项目的开发主计划、状态面板和阶段验收依据。开发过程中不以“代码已经写完”作为完成标准，而以阶段 Gate 全部通过、证据归档完成作为完成标准。

### 1.1 范围

本计划覆盖：

1. 外部 USD 资产与双 AUBO 场景运行验证。
2. 单臂 TCP-docking 强化学习环境。
3. 世界模型序列数据层。
4. 状态版和视觉版 DreamerV3。
5. R2-Dreamer 无 decoder 表征分支。
6. 双臂、动态物体、抓取与多视角扩展。
7. 测试、性能记录、实验可复现性和阶段交付。

第一轮开发不包含真实机器人部署、sim-to-real、双臂 MARL、显式接触动力学或安全认证。这些内容只有在单臂视觉基线完成后才进入评估。

### 1.2 状态定义

| 状态 | 标记 | 使用条件 |
|---|---|---|
| 已完成 | ✅ | 实现、测试、验收 Gate 和证据均完成 |
| 进行中 | 🚧 | 已开始开发，且没有阻塞关键路径 |
| 待验证 | 🧪 | 已有实现或配置，但尚未在目标运行时验证 |
| 阻塞 | ⛔ | 无法继续，且有明确的外部依赖或环境问题 |
| 待开始 | ⬜ | 前置阶段未完成或尚未排入当前开发 |
| 暂缓 | ⏸ | 有意推迟，不属于当前关键路径 |

### 1.3 状态更新规则

每次状态更新必须同时修改：

- 里程碑总表中的状态、下一动作和证据。
- 对应阶段任务表中的任务状态。
- 阶段验收表中的实际结果。
- 文末变更记录中的日期、变更内容和未解决问题。

任何 Gate 失败时，当前阶段不得标记为已完成，下游关键路径任务不得提前标记为进行中。

## 2. 当前基线审查

### 2.1 已确认事实

| 基线项 | 当前状态 | 审查结果 | 证据 |
|---|---|---|---|
| 外部资产发现 | ✅ | 从 `D:\Project\S2R\Asset` 自动发现 4 项资产 | `resolve_asset_root()` 输出及文件存在性测试 |
| 必需 USD | ✅ | AUBO、工作站、样品瓶 3 项必需 USD 均存在 | `tests/test_scene_config.py` |
| 机器人 prim 契约 | ✅（纯配置） | 6 个臂关节、2 个夹爪关节、Flange 名称无歧义 | 配置回归测试 |
| 目标与相机配置 | ✅（纯配置） | 4 个目标状态、3 个诊断相机、640×480 配置稳定 | 配置回归测试 |
| Python 静态质量 | ✅ | `compileall` 和 Ruff 通过 | 本地命令输出 |
| 纯配置测试 | ✅ | `3 passed` | `python -m pytest tests/test_scene_config.py -q` |
| Isaac Lab 导入 | ⛔ | 当前可用 Python 均无法导入 `isaaclab` | smoke 命令失败输出 |
| 静态场景实例化 | 🧪 | 已有 smoke 脚本，尚未在 Isaac Lab 中通过 | `scripts/smoke_scene.py` |
| 动态与视觉实例化 | 🧪 | 已有配置，没有独立动态/视觉 smoke 证据 | 待新增测试 |
| RL 环境 | ⬜ | 尚无 TCP-docking 环境类和 Gym 注册 | 代码扫描 |
| Dreamer/R2-Dreamer | ⬜ | 尚无 replay、RSSM、actor-critic 和表征分支 | 代码扫描 |

### 2.2 已识别风险

| 风险 ID | 优先级 | 当前事实 | 影响 | 计划处理阶段 |
|---|---:|---|---|---|
| R-01 | P0 | Isaac Lab 无法导入 | 所有仿真运行验证和环境开发均被阻塞 | S0 |
| R-02 | P0 | 样品瓶为 `kinematic_enabled=True` | 适合 TCP 对接，不可用于真实抓取/搬运物理验证 | S2 定义范围，S7 扩展 |
| R-03 | P1 | ContactSensorCfg 只查询第一台 AUBO | 双臂接触观测不完整 | S1 |
| R-04 | P1 | 3 台 640×480 相机、6 类输出、每仿真步更新 | 视觉 WM 吞吐和显存风险极高 | S1、S5 |
| R-05 | P1 | 策略频率当前为 4 Hz | 对增量控制可能过低 | S2 控制频率实验 |
| R-06 | P1 | 目标四元数声明为 wxyz，但来自迁移项目 | 可能出现姿态顺序错误 | S1 可视化验证 |
| R-07 | P1 | 父 USD 和子 articulation 均设置初始位姿 | 可能重复应用父子变换 | S0/S1 smoke 验证 |
| R-08 | P2 | 论文机器人证据以单臂、单智能体为主 | 不能直接外推双臂协作结论 | S7 单独验证 |

## 3. 里程碑状态总表

工作量使用相对等级：S 为小型、M 为中型、L 为大型、XL 为跨模块大型工作包，不表示固定工期。

| 顺序 | 阶段 | 目标 | 前置阶段 | 工作量 | 当前状态 | 下一动作 | 完成证据 |
|---:|---|---|---|---|---|---|---|
| 0 | BASE | 场景纯配置迁移与静态检查 | 无 | M | ✅ | 保持回归测试 | 3 tests、compileall、Ruff |
| 1 | S0 | 打通 Isaac Lab 运行时和静态场景 | BASE | M | ⛔ | 找到或安装匹配的 Isaac Lab 解释器 | static smoke 日志、版本清单 |
| 2 | S1 | 动态、接触、视觉和并行场景验收 | S0 | M | ⬜ | S0 通过后增加两个 smoke 脚本 | 动态/视觉日志和截图 |
| 3 | S2 | 单臂 TCP-docking RL 环境闭环 | S1 | L | ⬜ | 冻结动作与奖励契约 | 环境测试、random/zero/PPO 曲线 |
| 4 | S3 | 世界模型序列数据层 | S2 | L | ⬜ | 冻结 transition schema | replay 测试和吞吐报告 |
| 5 | S4 | 状态版 DreamerV3 | S3 | XL | ⬜ | 先完成模块接口设计 | loss 日志、状态控制曲线 |
| 6 | S5 | 视觉版 DreamerV3 | S4 | XL | ⬜ | 新增训练专用相机 | 视觉训练曲线、profile、视频 |
| 7 | S6 | R2-Dreamer 表征分支与公平对照 | S5 | L | ⬜ | 固定 Dreamer 对照协议 | 多种子对照表、资源报告 |
| 8 | S7 | 双臂、抓取、多视角扩展 | S6 | XL | ⏸ | 等待单臂论文基线完成 | 双臂专项报告 |
| 持续 | QA | 测试、文档、性能和可复现性 | 全阶段 | M | 🚧 | 每阶段更新证据 | CI/日志/配置快照 |

## 4. 目标系统与数据契约

### 4.1 分层目标

```text
Isaac Lab Scene
  -> TcpDockingEnv（reset/action/observation/reward/done）
  -> Sequence Collector + Replay Buffer
  -> RSSM World Model
  -> Imagined Rollout
  -> Actor + Critic Controller
  -> Applied Action
  -> Isaac Lab Scene
```

DreamerV3 与 R2-Dreamer 共用 RSSM、reward/continue heads、actor、critic、replay 和训练预算。两者只在视觉表征学习处切换：

```text
DreamerV3: CNN Encoder + Image Decoder + Reconstruction Loss
R2-Dreamer: CNN Encoder + Linear Projector + Redundancy Reduction Loss
```

### 4.2 环境输出契约

第一版单臂环境应稳定输出：

| 字段 | dtype | 形状 | 说明 |
|---|---|---|---|
| `proprio` | float32 | `[N, P]` | q、qd、TCP pose/velocity、目标相对位姿、上一动作 |
| `rgb` | uint8 | `[N, H, W, 3]` | S5 前可选；S5 起必须存在 |
| `action_requested` | float32 | `[N, A]` | actor 原始归一化动作 |
| `action_applied` | float32 | `[N, A]` | 限幅和控制器处理后真正施加的动作 |
| `reward` | float32 | `[N]` | 总奖励 |
| `reward_terms` | float32 | `[N, K]` | 调试和消融使用，不必进入 policy |
| `terminated` | bool | `[N]` | 物理失败或任务终止 |
| `truncated` | bool | `[N]` | 时间上限 |
| `success` | bool | `[N]` | 成功状态 |
| `is_first` | bool | `[N]` | reset 后第一帧 |

Replay 必须保存连续序列，不能把不同 episode 的 transition 拼接为同一训练序列。

## 5. S0：Isaac Lab 运行时与静态场景

### 5.1 目标

获得唯一、可重复的项目解释器，使现有 static scene 能在 headless 模式实例化，并验证两台 AUBO 的 articulation、关节、Flange、Jacobian 和接触报告契约。

### 5.2 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S0-T01 | 确定目标解释器 | 找到与当前 Isaac Sim 匹配的 Isaac Lab 安装/源码；确认 Python 路径 | `doc/runtime_versions.md` 初稿 | 无 | ⛔ |
| S0-T02 | 安装项目包 | 用目标解释器 editable 安装 `source/OnlineWM`，确认所有子包可发现 | 安装日志 | S0-T01 | ⬜ |
| S0-T03 | 基础导入检查 | 导入 `isaaclab`、`OnlineWM`、scene config | 导入日志 | S0-T02 | ⬜ |
| S0-T04 | 运行 static smoke | headless 创建 1 个环境并执行现有检查 | `artifacts/smoke/static_scene.log` | S0-T03 | 🧪 |
| S0-T05 | 固化版本 | 记录 Isaac Sim、Isaac Lab、PyTorch、CUDA、驱动、GPU、commit | 版本清单 | S0-T04 | ⬜ |
| S0-T06 | 固化启动入口 | 在 README 给出唯一推荐的 Windows 启动命令 | README 更新 | S0-T04 | ⬜ |

### 5.3 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S0-G01 | 运行时可导入 | `{ISAAC_PYTHON} -c "import isaaclab, OnlineWM"` | 退出码 0 | 未通过 |
| S0-G02 | 两台 articulation 被识别 | `scripts/smoke_scene.py --headless` | 精确识别 2 台 AUBO | 未执行 |
| S0-G03 | 关节/刚体契约正确 | smoke 输出 | 每臂 6 个 arm joints、2 个 gripper joints、1 个 Flange body，顺序一致 | 未执行 |
| S0-G04 | Jacobian 有效 | smoke 输出 | rank 为 4，batch 为 1，所有值有限 | 未执行 |
| S0-G05 | 接触报告有效 | `validate_contact_reporting` | 两台 articulation 下均发现 ContactReport API | 未执行 |
| S0-G06 | 生命周期正常 | 连续运行 smoke 3 次 | 3/3 退出码 0，无残留进程或关闭异常 | 未执行 |
| S0-G07 | 版本可追溯 | 检查版本文档 | 所有版本、解释器路径和 Git commit 非空 | 未执行 |

S0 只有在 S0-G01 至 S0-G07 全部通过后才能标记为完成。

## 6. S1：动态、接触、视觉与并行场景验收

### 6.1 目标

证明迁移场景不仅能加载，而且能够稳定更新、正确重置目标、输出双臂接触数据和批量相机数据。

### 6.2 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S1-T01 | 双臂接触配置 | 为 `AUBObot` 与 `AUBObot_2` 建立明确传感器配置 | scene config + test | S0 | ⬜ |
| S1-T02 | 动态 smoke | 实例化 `TcpDockingDynamicSceneCfg`，循环设置 4 个目标位姿 | `scripts/smoke_dynamic_scene.py` | S1-T01 | ⬜ |
| S1-T03 | 视觉 smoke | 实例化三相机，读取所有配置通道并保存诊断帧 | `scripts/smoke_vision_scene.py` | S0 | ⬜ |
| S1-T04 | 四元数核验 | 在 GUI 或诊断图中确认 4 个目标和 3 个相机方向 | 审查图 + 结论 | S1-T02/T03 | ⬜ |
| S1-T05 | 父子位姿核验 | 比较期望和实际 robot root/world pose | pose 日志 | S1-T02 | ⬜ |
| S1-T06 | 静置稳定性 | 无动作运行 1000 物理步，检查位姿、速度和 NaN | stability 日志 | S1-T02 | ⬜ |
| S1-T07 | 并行环境核验 | 运行 N=1、4、16，验证 prim 数、batch 维度和显存 | scaling 表 | S1-T02/T03 | ⬜ |
| S1-T08 | 训练相机配置 | 新增单 RGB、64×64 或 96×96、策略频率更新的配置 | training camera cfg | S1-T03 | ⬜ |

### 6.3 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S1-G01 | 4 个目标位姿正确 | 自动 pose 对比 + 人工截图 | 平移误差 ≤ 1e-5 m；四元数归一化；视觉方向符合任务定义 | 未执行 |
| S1-G02 | 父子变换无重复 | 读取两臂 root pose | 与配置期望平移误差 ≤ 1e-4 m、旋转角误差 ≤ 0.1° | 未执行 |
| S1-G03 | 场景静置稳定 | 1000 physics steps | 无 NaN/Inf；固定资产无漂移；非控制关节无爆炸速度 | 未执行 |
| S1-G04 | 双臂接触可读 | 人工触发或受控碰撞 | 两臂 sensor tensor 均存在，接触/无接触状态可区分 | 未执行 |
| S1-G05 | 诊断相机数据正确 | vision smoke | 3 相机均输出 640×480；RGB/深度/法线/分割 shape 与 dtype 正确 | 未执行 |
| S1-G06 | 训练相机可用 | training camera smoke | RGB 为 uint8；分辨率符合配置；每个策略步恰好一帧 | 未执行 |
| S1-G07 | 并行克隆正确 | N=1、4、16 | batch 第一维等于 N；环境间目标和状态无串扰 | 未执行 |
| S1-G08 | 资源记录完整 | profile | 每种 N 记录 FPS、GPU 显存、相机耗时；无 OOM | 未执行 |

## 7. S2：单臂 TCP-docking RL 环境

### 7.1 第一版任务冻结

- 主体：先使用 `AUBObot`，第二台机械臂保持固定。
- 目标：末端 Flange 从初始位姿移动到样品瓶 pre-position/target pose。
- 目标物：保持 kinematic；该阶段是 TCP 对接，不宣称完成抓取搬运。
- 动作：默认 6 维 TCP 位姿增量，归一化到 `[-1, 1]`。
- 低层控制：differential IK + joint position/PD；低层按物理频率或控制子步执行。
- 观测：q、qd、TCP pose/velocity、目标相对 pose、上一动作、碰撞/接触标志。
- 成功：位置和旋转误差同时进入阈值并连续保持若干策略步。

动作维度、频率和成功阈值可在 S2 内进行一次有记录的调整；S3 开始后必须冻结，避免数据和模型协议漂移。

### 7.2 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S2-T01 | 冻结任务规范 | 明确坐标系、动作、成功、失败、超时和目标采样 | `doc/tcp_docking_task_spec.md` | S1 | ⬜ |
| S2-T02 | 环境配置 | 新建 `TcpDockingEnvCfg`，定义 sim/scene/action/observation | env cfg | S2-T01 | ⬜ |
| S2-T03 | 环境实现与注册 | 实现 `DirectRLEnv` 和 Gym task 注册 | env + `__init__.py` | S2-T02 | ⬜ |
| S2-T04 | 低层控制器 | 实现 IK、动作限幅、关节目标和 applied action 回传 | controller module | S2-T03 | ⬜ |
| S2-T05 | 观测实现 | 输出统一 proprio、目标相对 pose、接触和上一动作 | observation code/test | S2-T03 | ⬜ |
| S2-T06 | reward 实现 | 分项位置、姿态、动作变化、碰撞、成功奖励 | reward code/test | S2-T03 | ⬜ |
| S2-T07 | reset/done 实现 | 4 目标采样、机器人复位、success/terminated/truncated | reset/done tests | S2-T03 | ⬜ |
| S2-T08 | 控制频率实验 | 比较 4/10/20 Hz，选择稳定且吞吐合适的频率 | frequency report | S2-T04 | ⬜ |
| S2-T09 | dummy agents | zero/random 连续运行并记录失败原因 | smoke logs | S2-T04–T07 | ⬜ |
| S2-T10 | PPO 基线 | 使用现有训练框架建立环境可学习性基线 | config + curves | S2-T09 | ⬜ |

### 7.3 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S2-G01 | 接口 shape/dtype 稳定 | 单元测试 N=1/16 | 与第 4.2 节契约完全一致 | 未执行 |
| S2-G02 | 动作安全 | 边界动作测试 | requested/action_applied 可区分；所有关节目标和速度均受限 | 未执行 |
| S2-G03 | reset 正确 | 连续 reset 1000 次 | 无非法状态；4 个目标均被采样；环境间无串扰 | 未执行 |
| S2-G04 | reward 方向正确 | 构造近/远/碰撞/成功状态 | 接近目标时 reward 单调改善；碰撞和越界惩罚符号正确 | 未执行 |
| S2-G05 | done 语义正确 | 构造终止与超时 | success/terminated/truncated 不混淆，timeout 不伪装成物理失败 | 未执行 |
| S2-G06 | 长运行稳定 | random agent 10,000 policy steps | 无 NaN/Inf、无卡死、无内存持续增长 | 未执行 |
| S2-G07 | 控制频率确定 | 4/10/20 Hz 对比 | 选定频率有跟踪误差、FPS 和稳定性依据 | 未执行 |
| S2-G08 | 环境可学习 | PPO，3 seeds | 固定单目标成功率 ≥80%；4 目标平均成功率 ≥60%；随机策略 ≤10% | 未执行 |
| S2-G09 | 结果可重复 | PPO，3 seeds | 保存每 seed 配置、曲线和 checkpoint；报告均值与标准差 | 未执行 |

若 S2-G08 未达到初始阈值，可在不改变任务定义的前提下调整 reward scale 或控制器；任何任务定义变化必须重新通过 S2-G01 至 S2-G07。

## 8. S3：世界模型序列数据层

### 8.1 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S3-T01 | Transition schema | 定义 observation/action/reward/done/is_first 数据类或 TensorDict | schema + test | S2 | ⬜ |
| S3-T02 | 多环境 collector | 正确处理异步 reset 和 applied action | collector | S3-T01 | ⬜ |
| S3-T03 | Sequence replay | 实现 episode-aware 存储和定长序列采样 | replay module | S3-T01 | ⬜ |
| S3-T04 | 持久化 | 保存/加载 replay 元数据与 episode | serialization code | S3-T03 | ⬜ |
| S3-T05 | 确定性测试 | 固定种子验证采样和 reset 可复现 | tests | S3-T02/T03 | ⬜ |
| S3-T06 | 吞吐 profile | 分离 env、copy、replay insert/sample 耗时 | profile report | S3-T02/T03 | ⬜ |
| S3-T07 | 离线检查工具 | 统计 episode、reward、done、动作分布和坏帧 | diagnostics script | S3-T04 | ⬜ |

### 8.2 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S3-G01 | 序列不跨 episode | 构造异步 reset 数据 | 10,000 个采样序列中跨 episode 数为 0 | 未执行 |
| S3-G02 | done 标志正确 | schema 单元测试 | `is_first/is_last/is_terminal/terminated/truncated` 逻辑 100% 通过 | 未执行 |
| S3-G03 | shape/dtype 正确 | N、T、batch 参数化测试 | 所有字段符合 schema，设备迁移不改变值 | 未执行 |
| S3-G04 | 持久化无损 | save/load round trip | 整数/布尔完全相等，float 在容差内一致 | 未执行 |
| S3-G05 | 固定种子可重复 | 重复采样 | 序列索引和数据完全一致 | 未执行 |
| S3-G06 | 数据质量 | diagnostics | 无 NaN/Inf；RGB 无空帧；动作分布未被意外常量化 | 未执行 |
| S3-G07 | 性能可接受 | profile | replay 插入/采样不成为主瓶颈；各模块耗时有独立记录 | 未执行 |

## 9. S4：状态版 DreamerV3

### 9.1 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S4-T01 | 模型接口设计 | 定义 encoder、RSSM、heads、actor、critic API | design doc | S3 | ⬜ |
| S4-T02 | 状态 encoder/RSSM | MLP encoder、确定/随机状态、prior/posterior | modules + tests | S4-T01 | ⬜ |
| S4-T03 | WM heads | reward、continue、状态重建或辅助预测 | modules + tests | S4-T02 | ⬜ |
| S4-T04 | 稳定化组件 | symlog、two-hot、KL balance、free bits、unimix | modules + numerical tests | S4-T02 | ⬜ |
| S4-T05 | imagined rollout | latent rollout、lambda-return、discount | module + tests | S4-T02/T03 | ⬜ |
| S4-T06 | actor-critic | actor、distributional critic、entropy 和 return normalization | modules + tests | S4-T05 | ⬜ |
| S4-T07 | 训练循环 | env collect、replay sample、WM/actor/critic 更新和 checkpoint | train script | S4-T02–T06 | ⬜ |
| S4-T08 | 数值监控 | loss、KL、entropy、grad norm、latent usage、finite check | dashboard/logging | S4-T07 | ⬜ |
| S4-T09 | 状态控制实验 | 单目标和 4 目标、3 seeds | curves/checkpoints | S4-T07/T08 | ⬜ |

### 9.2 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S4-G01 | 数学组件正确 | 手算小样本和梯度测试 | symlog/two-hot/KL/free bits 与参考公式一致 | 未执行 |
| S4-G02 | 序列推理正确 | posterior/prior shape 测试 | batch/time 维度正确，reset 时隐藏状态正确清零 | 未执行 |
| S4-G03 | imagined rollout 正确 | 人工短轨迹 | discount、continue、lambda-return 与参考实现一致 | 未执行 |
| S4-G04 | 数值稳定 | ≥10,000 model updates | loss、grad、latent、概率无 NaN/Inf；无概率越界 | 未执行 |
| S4-G05 | 潜状态未明显塌缩 | latent diagnostics | 多数随机变量保持非平凡类别使用；KL 不长期贴零或持续爆炸 | 未执行 |
| S4-G06 | WM 预测有信息 | hold-out replay | reward 误差较常数基线降低 ≥20%；continue balanced accuracy ≥0.8 | 未执行 |
| S4-G07 | 控制优于随机 | 3 seeds | 4 目标平均成功率显著高于 random，且至少达到 50% | 未执行 |
| S4-G08 | checkpoint 可恢复 | 中断恢复测试 | 恢复后 step/replay/optimizer 一致，曲线无异常跳变 | 未执行 |

状态版结果只证明 WM/Controller 工程闭环，不作为 R2-Dreamer 视觉表征结论。

## 10. S5：视觉 DreamerV3

### 10.1 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S5-T01 | 训练相机接入 | 单视角 RGB、64×64/96×96、策略步更新 | vision env cfg | S1-T08/S4 | ⬜ |
| S5-T02 | 时间同步 | 为 image/state/action 记录 env step 与 episode ID | sync code/test | S5-T01 | ⬜ |
| S5-T03 | CNN encoder | 规范化 RGB 并编码 | module + tests | S5-T01 | ⬜ |
| S5-T04 | 图像 decoder | 从 latent 重建观测，支持 Dreamer loss | module + tests | S5-T03 | ⬜ |
| S5-T05 | 多模态融合 | RGB 与 proprioception 融合 | module + ablation | S5-T03 | ⬜ |
| S5-T06 | 视觉训练 | 固定协议完成 3 seeds | curves/checkpoints | S5-T04/T05 | ⬜ |
| S5-T07 | 性能 profile | 仿真、渲染、replay、encoder、decoder、更新分别计时 | profile report | S5-T06 | ⬜ |
| S5-T08 | 可视化诊断 | 重建、open-loop rollout、任务视频 | artifacts | S5-T06 | ⬜ |

### 10.2 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S5-G01 | 图像契约正确 | N=1/16 tests | uint8、目标分辨率、RGB 通道顺序和 batch 维度正确 | 未执行 |
| S5-G02 | 时序严格对齐 | 注入 step ID 的测试 | image/state/action/reward 无一帧偏移 | 未执行 |
| S5-G03 | 无坏帧 | 采集 ≥100k frames | 全黑、全零、NaN 深度或重复卡帧比例为 0 | 未执行 |
| S5-G04 | 视觉目标可辨 | 人工诊断图 | TCP、样品瓶和目标关系在训练分辨率下可辨识 | 未执行 |
| S5-G05 | 训练稳定 | ≥10,000 updates | decoder、WM、actor/critic loss 和梯度均有限 | 未执行 |
| S5-G06 | 重建保留任务信息 | hold-out 可视化/指标 | 目标与末端位置可辨，不能只重建大面积背景 | 未执行 |
| S5-G07 | 视觉控制有效 | 3 seeds | 平均成功率 ≥ 状态版成功率的 70%，并显著高于 random | 未执行 |
| S5-G08 | 资源可执行 | 完整 profile | 无 OOM；记录最大并行环境数、FPS、显存和墙钟时间 | 未执行 |

## 11. S6：R2-Dreamer 表征分支

### 11.1 公平对照约束

DreamerV3 与 R2-Dreamer 必须共享：

- 环境和相机帧。
- replay 数据与序列采样。
- CNN encoder、RSSM、reward/continue heads。
- actor、critic、优化步数、batch、随机种子和评估协议。

允许差异仅为：DreamerV3 使用 image decoder/reconstruction loss；R2-Dreamer 使用 linear projector/redundancy-reduction loss。

### 11.2 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S6-T01 | 表征接口抽象 | 配置化 `rep_loss`，统一训练入口 | interface + tests | S5 | ⬜ |
| S6-T02 | R2 projector | latent state 映射到 image embedding 维度 | module + tests | S6-T01 | ⬜ |
| S6-T03 | 冗余约简损失 | B×T 标准化、cross-correlation、diag/offdiag loss、stop-gradient | module + formula tests | S6-T02 | ⬜ |
| S6-T04 | decoder 移除核验 | R2 配置不构造 decoder、不更新 decoder 参数 | architecture test | S6-T01 | ⬜ |
| S6-T05 | 公平协议测试 | 对比两分支配置 diff 和数据 ID | parity report | S6-T01–T04 | ⬜ |
| S6-T06 | 多种子实验 | Dreamer/R2 各至少 3 seeds，正式报告建议 5 | curves/checkpoints | S6-T05 | ⬜ |
| S6-T07 | 资源对比 | 墙钟、FPS、显存、参数量、训练模块耗时 | comparison table | S6-T06 | ⬜ |
| S6-T08 | 小目标压力测试 | 缩小瓶体/目标视觉面积或构造遮挡但不改变动力学 | benchmark variant | S6-T06 | ⬜ |

### 11.3 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S6-G01 | 损失公式正确 | 手算矩阵 + autograd test | diag/offdiag、标准化、stop-gradient 与论文公式一致 | 未执行 |
| S6-G02 | 架构差异受控 | 自动 config/model diff | 除 decoder/projector/rep loss 外无未声明差异 | 未执行 |
| S6-G03 | 数据完全一致 | replay sample ID 日志 | 同 seed 对照实验读取相同训练序列 | 未执行 |
| S6-G04 | 表征未塌缩 | correlation/variance 监控 | embedding 方差非零；diag 相关持续上升；offdiag 不发散 | 未执行 |
| S6-G05 | 训练稳定 | ≥10,000 updates | 所有 loss、相关矩阵、梯度无 NaN/Inf | 未执行 |
| S6-G06 | 控制具有竞争力 | ≥3 seeds | R2 平均成功率不低于 Dreamer 超过 10 个百分点，报告置信区间 | 未执行 |
| S6-G07 | 资源收益可量化 | 统一硬件 profile | 参数、显存或模型更新墙钟至少一项有明确下降；如无下降需定位渲染瓶颈 | 未执行 |
| S6-G08 | 论文命题可检验 | 小目标任务 | 报告标准任务与小目标任务的独立结果，不用状态版代替视觉验证 | 未执行 |

S6-G07 不要求复现论文的固定加速倍数；Isaac Sim 渲染可能成为主瓶颈，但必须把渲染耗时与模型更新耗时分开报告。

## 12. S7：双臂、动态物体与多视角扩展

S7 默认暂缓，只有 S6 完成后才进入开发。

### 12.1 任务状态表

| 任务 ID | 任务 | 实施内容 | 交付物 | 依赖 | 状态 |
|---|---|---|---|---|---|
| S7-T01 | 双臂集中式环境 | 联合状态、联合动作、两臂控制和接触 | dual-arm env | S6 | ⏸ |
| S7-T02 | 协同任务定义 | 明确角色、同步、成功和冲突条件 | task spec | S7-T01 | ⏸ |
| S7-T03 | 动态样品瓶 | 关闭 kinematic，配置质量、惯量、碰撞、摩擦 | object cfg + tests | S7-T01 | ⏸ |
| S7-T04 | 抓取/搬运 reward | grasp、lift、transport、place 分阶段指标 | reward tests | S7-T03 | ⏸ |
| S7-T05 | 多视角融合 | 比较单视角、拼接、共享 encoder/attention 融合 | ablation | S7-T01 | ⏸ |
| S7-T06 | WM 架构比较 | centralized RSSM、per-arm RSSM、通信结构 | experiment report | S7-T02 | ⏸ |

### 12.2 验收 Gate

| Gate | 验收门槛 | 验证方法 | 通过标准 | 实际结果 |
|---|---|---|---|---|
| S7-G01 | 双臂动作同步 | 受控联合动作 | 同一 policy step 两臂动作时间戳一致 | 未执行 |
| S7-G02 | 双臂接触完整 | 接触测试 | 两臂、物体和环境接触均可区分 | 未执行 |
| S7-G03 | 动态物体物理合理 | 静置/抓取/跌落测试 | 无漂移、穿透和非物理爆炸；参数有来源记录 | 未执行 |
| S7-G04 | 协同成功可判定 | 构造成功/失败轨迹 | success、冲突、超时和失败原因分类正确 | 未执行 |
| S7-G05 | 双臂控制优于随机 | ≥3 seeds | 预定义任务成功率显著高于 random，报告相对单臂新增失败类型 | 未执行 |
| S7-G06 | 结论边界明确 | 实验报告审查 | 不把单臂论文结果直接表述为双臂论文复现 | 未执行 |

## 13. QA：持续工程任务

### 13.1 测试矩阵

| 测试层 | 执行频率 | 必须覆盖 | 失败处理 |
|---|---|---|---|
| 纯配置单元测试 | 每次配置变更 | 资产、prim、目标、相机、常量 | 禁止合入 |
| Python 编译/Ruff | 每次代码变更 | `source/`、`scripts/`、`tests/` | 禁止合入 |
| Isaac static smoke | 每次 USD/robot config 变更 | 两臂、关节、Flange、Jacobian、contact API | 回退或修复配置 |
| Dynamic/vision smoke | 每次传感器/相机/目标变更 | reset、contact、camera shape/dtype | 禁止进入训练 |
| Env smoke | 每次 env/reward/control 变更 | zero/random、reset、done、10k steps | 禁止训练 |
| Replay tests | 每次 schema/buffer 变更 | episode 边界、save/load、determinism | 禁止 WM 训练 |
| Model numerical tests | 每次 loss/RSSM 变更 | 公式、shape、gradient、finite | 禁止长训练 |
| 小规模 overfit | 每次主要模型变更 | 固定小 replay 上降低 loss | 禁止长训练 |
| 多种子评估 | 每个阶段发布点 | success/return/资源指标 | 不得形成结论 |

### 13.2 推荐验证命令

```powershell
python -m pytest tests/test_scene_config.py -q
python -m compileall -q source/OnlineWM/OnlineWM scripts tests
python -m ruff check source scripts tests

{ISAAC_PYTHON} scripts/smoke_scene.py --headless
{ISAAC_PYTHON} scripts/smoke_dynamic_scene.py --headless
{ISAAC_PYTHON} scripts/smoke_vision_scene.py --headless
```

`{ISAAC_PYTHON}` 必须在 S0 固化为项目唯一推荐入口，不允许不同阶段临时切换解释器而不记录。

### 13.3 证据目录约定

以下目录为规划约定，创建时应同时加入合适的 `.gitignore` 规则：

```text
artifacts/
|-- smoke/          # static/dynamic/vision/env smoke 日志
|-- diagnostics/    # 相机图、位姿图、接触诊断
|-- profiles/       # FPS、显存、模块耗时
`-- evaluations/    # 汇总表和评估曲线

outputs/
|-- checkpoints/
|-- replay/
`-- runs/
```

大型 checkpoint、replay 和原始视频不提交 Git；配置、汇总指标和必要诊断图应可追溯。

## 14. 实验记录最低要求

每次可用于比较的训练必须记录：

- Git commit 与 dirty 状态。
- Isaac Sim、Isaac Lab、Python、PyTorch、CUDA、驱动和 GPU。
- 完整配置快照、随机种子和环境数量。
- 相机分辨率、相机数量、更新频率和数据通道。
- 环境步数、模型更新数、batch、sequence length 和 replay ratio。
- success rate、return、episode length、失败原因分布。
- WM loss、KL、latent usage、reward/continue 指标。
- actor/critic loss、entropy、return scale 和 gradient norm。
- 环境 FPS、渲染耗时、模型更新时间、总墙钟和峰值显存。

缺少上述任一核心项的运行只能作为调试，不进入 DreamerV3/R2-Dreamer 正式对照。

## 15. 当前执行队列

### 15.1 立即执行（关键路径）

| 排序 | 任务 ID | 动作 | 状态 | 完成后解锁 |
|---:|---|---|---|---|
| 1 | S0-T01 | 找到/安装匹配的 Isaac Lab 解释器 | ⛔ | S0 全部任务 |
| 2 | S0-T02 | editable 安装项目 | ⬜ | 基础导入与 smoke |
| 3 | S0-T03 | 运行基础导入检查 | ⬜ | static smoke |
| 4 | S0-T04 | 让 static smoke 连续通过 3 次 | 🧪 | S1 动态/视觉验收 |
| 5 | S0-T05/T06 | 固化版本和启动命令 | ⬜ | 可重复开发环境 |

### 15.2 后续排队

| 优先级 | 任务范围 | 进入条件 | 状态 |
|---:|---|---|---|
| P0 | S1 动态/视觉 smoke 与训练相机 | S0 完成 | ⬜ |
| P0 | S2 单臂 TCP-docking Env + PPO | S1 完成 | ⬜ |
| P1 | S3 sequence replay | S2 环境协议冻结 | ⬜ |
| P1 | S4 状态 DreamerV3 | S3 完成 | ⬜ |
| P1 | S5 视觉 DreamerV3 | S4 完成 | ⬜ |
| P1 | S6 R2-Dreamer | S5 完成 | ⬜ |
| P2 | S7 双臂/抓取/多视角 | S6 完成 | ⏸ |

## 16. 阶段完成检查单

每个阶段关闭前必须逐项确认：

- [ ] 任务表中的必需任务全部为 ✅。
- [ ] 所有 Gate 均有实际结果，不保留“未执行”。
- [ ] 单元测试、smoke、Ruff 和编译检查通过。
- [ ] 配置、日志、曲线、截图或 checkpoint 证据可定位。
- [ ] 已知失败和偏差写入阶段报告。
- [ ] README 或相关开发文档已更新。
- [ ] 里程碑总表和当前执行队列已更新。
- [ ] 下游阶段所依赖的接口已经冻结或版本化。

## 17. 变更记录

| 日期 | 版本 | 变更 | 当前阻塞 |
|---|---|---|---|
| 2026-07-18 | v1.0 | 将场景审查扩展为任务级开发文档；加入状态表、依赖、Gate、阈值、证据与执行队列 | Isaac Lab 无法从当前解释器导入 |
