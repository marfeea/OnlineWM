# R2-Dreamer 单臂 AUBO 训练与真机部署开发路线

> 文档版本：v3.2
> 基线日期：2026-07-25
> 当前对象：官方 IsaacLab/R2-Dreamer 验证、本地 AUBO 单臂任务、仿真训练、sim-to-real 与真机部署
> 最终目标：训练一套可在单台 AUBO E5 上执行目标任务的 R2-Dreamer 策略，并形成可重复、可回退的真机验证证据
> 范围边界：双臂、多臂和多智能体世界模型属于未来计划，不进入当前路线

## 1. 文档定位

本文档是当前阶段的具体开发与验收依据，负责回答：

1. 如何运行 Isaac Lab 和 R2-Dreamer 官方示例，确认基础训练链路可用。
2. 如何冻结 AUBO 单臂任务、控制、观测、奖励和评价协议。
3. 如何将本地 AUBO 场景实现为可训练的 Isaac Lab 环境。
4. 如何训练并评估 AUBO 单臂 R2-Dreamer 策略。
5. 如何缩小仿真与真机差异，并安全地完成真机部署。

本文档服从宏观研究纲领：

- `doc/OnlineWM世界模型研究纲领.md`

论文背景、方法分析与研究价值参考：

- `doc/世界模型论文阅读与本地复现总结.md`

外部网站、SDK 文档与本地参考资料统一从以下入口管理：

- `reference/README.md`

## 2. 最终目标与当前范围

### 2.1 当前主线

```text
固化运行时与版本
  -> Isaac Lab 官方训练示例
  -> R2-Dreamer 官方状态训练
  -> R2-Dreamer 官方视觉训练
  -> AUBO 单臂任务冻结
  -> 本地单臂 Isaac Lab 环境
  -> 单臂状态链路验证
  -> 单臂视觉 R2-Dreamer 训练
  -> sim-to-real 准备
  -> 单臂 AUBO 真机部署与评估
```

### 2.2 当前范围

当前路线包括：

1. 官方 Isaac Lab 和 R2-Dreamer 训练链路验证。
2. 本地 AUBO 单臂任务设计。
3. 本地 AUBO 单臂训练环境实现与接入。
4. 单臂状态输入和视觉输入的训练闭环。
5. R2-Dreamer 单臂策略训练、恢复、评估与回放。
6. 仿真到真机的观测、动作、时序和坐标系对齐。
7. 传感器与机器人标定、系统辨识和必要的域随机化。
8. 真机推理接口、安全监督、分阶段部署和实验记录。

### 2.3 当前不纳入

当前阶段不包含：

- 第二台机械臂参与决策或执行；
- 双臂联合动作和联合状态；
- `DirectMARLEnv`、IPPO、MAPPO 或多智能体通信；
- 双臂闭链、内力分配和多主体信用分配；
- DreamerV3 与 R2-Dreamer 对照实验；
- 论文全部 benchmark 与全部随机种子复现。

如果仿真场景保留第二台 AUBO，它只能作为静态环境组成部分，不进入当前策略的观测、动作、奖励或成功定义。

## 3. 已确认事实、任务决策与待量化项

### 3.1 R2-Dreamer 官方配置

| 配置 | 官方任务 | 观测 | 并行环境 | 官方预算 | 当前用途 |
|---|---|---|---:|---:|---|
| `isaaclab_proprio` | `Isaac-Cartpole-Direct-v0` | `policy` 状态向量 | 16 | 510K steps | 验证环境、回放、RSSM 和 actor-critic |
| `isaaclab_vision` | `Isaac-Cartpole-RGB-Camera-Direct-v0` | 64×64 RGB `image` | 16 | 1.01M steps | 验证完整视觉 R2-Dreamer 链路 |

参考：

- <https://github.com/NM512/r2dreamer/blob/main/configs/env/isaaclab_proprio.yaml>
- <https://github.com/NM512/r2dreamer/blob/main/configs/env/isaaclab_vision.yaml>
- <https://github.com/NM512/r2dreamer/blob/main/configs/model/_base_.yaml>

官方基础模型配置默认使用 `rep_loss: r2dreamer`。实验命令仍应显式指定该参数。

### 3.2 本地环境现状

已确认：

- 本地场景包含两台 AUBO E5、工作站和样品瓶。
- 已有 AUBO articulation、关节、Flange 和 Jacobian 的静态检查。
- 已有四组目标位姿。
- 已有接触传感器和三路 640×480 诊断相机配置。
- 当前场景时间配置为 120 Hz 物理仿真、4 Hz 策略频率和 40 秒 episode。
- 当前动态场景中的样品瓶设置为 `kinematic_enabled=True`。
- 仓库尚无针对 AUBO 任务的 `DirectRLEnv`。

### 3.3 已确认的单臂任务协议

| 维度 | 已确认决策 |
|---|---|
| 任务 | 使用夹爪抓取采样瓶并将其稳定抬离初始支撑面 |
| 受控机器人 | 使用第一台 AUBO，即场景实体 `AUBObot` |
| 选择依据 | `AUBObot` 与 `AUBObot_2` 在工作站局部平面的中心偏移分别约为 0.036 m 和 0.837 m；第一台明显更靠近工作站中心 |
| 操作对象 | 采样瓶是需要发生抓取、接触和抬升的交互物体；训练环境中必须由当前运动学物体改为受重力和接触作用的动态刚体 |
| 控制架构 | 保留关节空间控制与 TCP 增量控制两个可配置后端；默认优先采用关节位置/速度控制，TCP 增量作为备选 |
| 控制频率 | 策略保持 4 Hz；120 Hz 物理与低层控制循环负责命令插值，即每个策略动作对应 30 个低层周期 |
| 策略输入 | 采用视觉与本体感知联合输入 |
| 相机协议 | 仿真与真实环境使用统一相机视角，并保持相同的裁剪、颜色、缩放和归一化流程 |
| 真机资料 | AUBO SDK、RTDE 说明与控制示例已经存入 `reference/` 并由 `reference/README.md` 建立入口；官方网站链接仍待补充 |

控制后端可以同时保留在代码中，但单次训练、评估或部署只能选择一种动作语义。checkpoint 必须记录控制模式，禁止在恢复训练或部署时静默切换。

联合输入中的可部署本体信息至少包括关节位置、关节速度、夹爪状态和上一动作；仿真专有的对象真值位姿不得作为部署策略的必要输入。

### 3.4 仍待量化和冻结的参数

以下内容不再是路线选择问题，但必须在 P4 完成数值化：

- 采样瓶的训练初始分布、评估初始集合和重置方式；
- 抓取成立条件、抬升高度、保持时间和成功容差；
- 关节位置/速度动作的精确向量语义、范围、归一化和插值方法；
- TCP 增量控制的平移、旋转步长及逆运动学失败处理；
- 联合输入中本体字段的最终清单、时间同步和归一化；
- 统一相机的安装位姿、内外参、分辨率、裁剪区域和延迟预算；
- AUBO SDK 版本、命令接口、反馈频率、网络时延与故障语义；
- 关节、速度、工作空间、碰撞、掉瓶和急停阈值。

这些参数必须在正式训练前登记，不能依据训练或真机结果反复修改成功标准。

## 4. 总体里程碑

| 阶段 | 目标 | 前置 | 状态 | 完成证据 |
|---|---|---|---|---|
| P0 | 固化运行时与代码版本 | 无 | ✅ | 版本清单、commit、解释器与导入日志 |
| P1 | 运行 Isaac Lab 官方训练示例 | P0 | ⬜ | 官方任务曲线、checkpoint、重复运行日志 |
| P2 | 运行 R2-Dreamer 官方状态链路 | P1 | ⬜ | replay/RSSM/actor-critic 日志、checkpoint 恢复 |
| P3 | 运行 R2-Dreamer 官方视觉链路 | P2 | ⬜ | RGB、R2 loss、训练曲线、性能与回放证据 |
| P4 | 冻结 AUBO 单臂任务协议 | P3 | ⬜ | 任务规格、控制与观测协议、成功和安全定义 |
| P5 | 实现本地 AUBO 单臂 Isaac Lab 环境 | P4 | ⬜ | Gym 注册、random/zero smoke、环境测试 |
| P6 | 验证 AUBO 单臂状态训练链路 | P5 | ⬜ | 单臂状态训练日志、checkpoint、策略回放 |
| P7 | 训练 AUBO 单臂视觉 R2-Dreamer | P6 | ⬜ | 仿真策略、成功指标、性能与失败分析 |
| P8 | 完成 sim-to-real 与部署准备 | P7 | ⬜ | 标定、系统辨识、安全层、真机接口和演练 |
| P9 | 完成 AUBO 单臂真机部署 | P8 | ⬜ | 真机实验、干预记录、回退验证和部署报告 |
| QA | 测试、证据和文档维护 | 全阶段 | 🚧 | 配置快照、日志索引、风险与变更记录 |

任何阶段 Gate 未通过时，下游阶段不得标记为完成。

## 5. P0：固化运行时与版本

### 5.1 记录内容

- Windows、GPU、显存和驱动；
- Isaac Sim 与 Isaac Lab 版本；
- R2-Dreamer 与 OnlineWM commit；
- Python、PyTorch、CUDA、Gymnasium 和 TensorDict 版本；
- 唯一推荐的 Python 启动入口；
- 依赖安装过程；
- 真机侧操作系统、控制 SDK 和通信依赖的待确认项。

R2-Dreamer 官方仓库当前主要在 Ubuntu 24.04、Python 3.11 环境测试。Windows 兼容性必须通过运行验证。

### 5.2 Gate

| Gate | 通过标准 |
|---|---|
| P0-G01 | P1 使用的唯一解释器能够导入 Isaac Lab、Isaac Lab Tasks 和选定的官方训练后端 |
| P0-G02 | CUDA 可用，PyTorch 和 Isaac Sim 指向预期 GPU |
| P0-G03 | 所有仓库 commit 和 dirty 状态已记录 |
| P0-G04 | 依赖安装可以从空终端重复执行 |
| P0-G05 | 版本信息写入 `doc/runtime_versions.md` |
| P0-G06 | 进入 P2 前，R2-Dreamer 能在与 Isaac Lab 兼容的解释器中导入；进入 P5 前，同一运行时能够导入 OnlineWM |

P1 不应被尚未安装的 R2-Dreamer 或 OnlineWM 阻塞，但必须先满足 P0-G01—P0-G05。P0-G06 是 P2 和 P5 的入口 Gate，不得推迟到相应阶段训练已经开始之后。

## 6. P1：Isaac Lab 官方训练示例

### 6.1 目标

验证 Isaac Lab 自身的仿真、并行环境、训练、日志和 checkpoint 链路。

### 6.2 推荐任务

优先选择 `Isaac-Cartpole-Direct-v0`。实际命令以固化版本的官方训练入口为准。

当前版本已确认 RL-Games 配置和官方训练脚本存在，可使用以下命令模板：

```powershell
conda run -n <ISAAC_ENV> python "<ISAACLAB_ROOT>\scripts\reinforcement_learning\rl_games\train.py" `
  --task Isaac-Cartpole-Direct-v0 `
  --num_envs 16 `
  --headless `
  --max_iterations 5
```

该命令只用于首次 smoke。通过环境创建、日志和 checkpoint 检查后，再恢复官方迭代预算。`<ISAAC_ENV>`、`<ISAACLAB_ROOT>`、工作目录和日志归档命令必须在 P0 中替换为冻结值。

### 6.3 Gate

| Gate | 通过标准 |
|---|---|
| P1-G01 | headless 创建并行官方环境成功 |
| P1-G02 | reset、step、reward、terminated、truncated 持续更新 |
| P1-G03 | 训练日志与 checkpoint 正常生成 |
| P1-G04 | episode return 相比初始阶段出现可辨识提升 |
| P1-G05 | 同一命令连续执行 3 次均正常退出 |

## 7. P2：R2-Dreamer 官方状态链路

### 7.1 目标

使用 `isaaclab_proprio` 验证：

```text
Isaac Lab
  -> IsaacLabVecEnv
  -> sequence replay
  -> RSSM
  -> imagined rollout
  -> actor-critic
  -> action
  -> Isaac Lab
```

本阶段只证明算法与环境集成正确。

### 7.2 Smoke 命令

```powershell
python train.py env=isaaclab_proprio model.rep_loss=r2dreamer env.steps=10000 env.env_num=4
```

Smoke 通过后恢复官方配置：

```powershell
python train.py env=isaaclab_proprio model.rep_loss=r2dreamer
```

### 7.3 Gate

| Gate | 通过标准 |
|---|---|
| P2-G01 | replay 能采样连续序列且不跨 episode |
| P2-G02 | RSSM、reward/continue heads、actor 和 critic 均发生更新 |
| P2-G03 | loss、梯度和潜状态无 NaN/Inf |
| P2-G04 | actor 动作不是恒定值且范围合法 |
| P2-G05 | checkpoint 保存后可恢复并继续训练 |
| P2-G06 | episode 指标持续写入日志 |

## 8. P3：R2-Dreamer 官方视觉链路

### 8.1 目标

使用 `isaaclab_vision` 验证相机、CNN encoder、RSSM、R2 冗余约简损失和 actor-critic 的完整视觉链路。

### 8.2 Smoke 命令

```powershell
python train.py env=isaaclab_vision model.rep_loss=r2dreamer env.steps=20000 env.env_num=4
```

Smoke 通过后恢复官方配置：

```powershell
python train.py env=isaaclab_vision model.rep_loss=r2dreamer
```

### 8.3 Gate

| Gate | 通过标准 |
|---|---|
| P3-G01 | `image` 为预期的 `uint8` RGB 批量张量 |
| P3-G02 | reset 首帧与 terminal observation 语义正确 |
| P3-G03 | R2 冗余约简损失持续更新且数值有限 |
| P3-G04 | CNN encoder 与 RSSM 梯度有限 |
| P3-G05 | actor 能基于图像产生非恒定合法动作 |
| P3-G06 | 训练可保存、恢复和固定策略回放 |
| P3-G07 | 已记录环境 FPS、训练 FPS、显存峰值和墙钟时间 |

## 9. P4：AUBO 单臂任务协议冻结

### 9.1 目标

在实现环境前形成唯一、可测量、能够同时映射到仿真与真机的任务定义。

正式训练任务确定为：第一台 AUBO 使用夹爪抓取采样瓶，并将采样瓶稳定抬离初始支撑面。

### 9.2 已确定协议与待量化参数

| 维度 | 已确定协议 | P4 交付物 |
|---|---|---|
| 任务目标 | 抓取采样瓶并稳定抬升 | 抓取、抬升、保持和失败状态机 |
| 受控对象 | 第一台 AUBO `AUBObot` 的机械臂与夹爪 | 关节、夹爪和 TCP 控制范围 |
| 操作对象 | 采样瓶作为动态刚体参与接触、抓取和重力运动 | 质量、碰撞、摩擦、初始分布与重置协议 |
| 初始分布 | 围绕可抓取区域建立训练与评估集合 | 相互独立的训练和评估初始状态集合 |
| 动作 | 默认关节位置/速度控制；保留 TCP 增量后端 | 两个动作适配器及各自语义、范围、归一化 |
| 时序 | 4 Hz 策略控制，120 Hz 低层循环插值 | 30:1 时序、插值、命令保持与时间戳协议 |
| 状态观测 | 与视觉联合输入的可部署本体感知 | 关节、夹爪、上一动作等字段及归一化 |
| 视觉观测 | 仿真与真机采用同一相机视角和预处理 | 安装位姿、标定、分辨率、裁剪、色彩与帧率 |
| 奖励 | 哪些进展获得奖励，如何避免奖励捷径 | 奖励项及设计理由 |
| 终止 | 成功、失败、碰撞、越界和超时 | `terminated/truncated` 定义 |
| 成功 | 有效抓取后达到规定抬升高度并保持 | 高度、保持时间和容差 |
| 安全 | 仿真与真机的关节、工作空间和速度边界 | 安全约束清单 |
| 评价 | 使用多少固定初始条件和独立试验 | 预注册评估协议 |

所有阈值应在正式训练前登记。不得依据训练结果反复修改成功标准。

### 9.3 单臂边界

- 当前策略只控制第一台 AUBO `AUBObot`。
- 第二台 AUBO 若保留在场景中，应固定姿态并视为环境资产。
- 第二台 AUBO 的状态不得进入策略输入。
- 策略输出不得包含第二台 AUBO 的动作。
- 成功与奖励不得依赖第二台 AUBO 的主动行为。

### 9.4 Gate

| Gate | 通过标准 |
|---|---|
| P4-G01 | 任务目标能够用可测量状态描述 |
| P4-G02 | 仿真任务能够映射到真机设备和坐标系 |
| P4-G03 | 动作、观测、奖励、终止和成功协议已冻结 |
| P4-G04 | 训练集与评估集的初始条件已分离 |
| P4-G05 | 所有真机安全边界和停止条件已定义 |
| P4-G06 | 单臂边界明确，不包含多臂智能体内容 |
| P4-G07 | 两个控制后端可由配置显式选择，且 checkpoint 记录动作语义 |
| P4-G08 | 联合输入只依赖仿真和真机均可获得的观测 |

## 10. P5：本地 AUBO 单臂 Isaac Lab 环境

### 10.1 目标

把现有场景配置转化为一个可被 R2-Dreamer 使用的单智能体训练环境。

### 10.2 环境契约

| 字段 | 建议格式 | 说明 |
|---|---|---|
| `policy` | `float32 [N,P]` | 单臂本体与任务状态 |
| `image` | `uint8 [N,H,W,3]` | 与真机协议一致的 RGB |
| `action` | `float32 [N,A]` | 归一化策略动作 |
| `reward` | `float32 [N]` | P4 冻结的任务奖励 |
| `terminated` | `bool [N]` | 成功或任务失败 |
| `truncated` | `bool [N]` | 时间上限 |
| `success` | `bool [N]` | 独立成功指标 |
| extras/log | tensor 字典 | 奖励项、安全事件和诊断指标 |

R2-Dreamer 的策略观测由 `policy` 与 `image` 联合构成。`policy` 不得依赖真机无法直接获得的仿真特权信息。

### 10.3 开发任务

1. 新建 AUBO 单臂 `DirectRLEnv`。
2. 将第一台 `AUBObot` 注册为唯一受控机器人，固定或排除第二台机械臂。
3. 将采样瓶从 `kinematic_enabled=True` 改为可受重力、碰撞和夹爪接触作用的动态刚体。
4. 实现可配置的关节空间与 TCP 增量动作适配器，默认启用关节位置/速度控制。
5. 在 4 Hz 策略循环与 120 Hz 物理循环之间实现并测试 30 个低层周期的命令插值。
6. 实现抓取、抬升进展、保持、掉瓶、碰撞、越界和超时对应的奖励与终止逻辑。
7. 为训练建立单路相机，并使视角和预处理可映射到真机相机，不直接使用三路多模态诊断配置。
8. 输出与 R2-Dreamer 一致的 `policy`/`image` 联合观测。
9. 注册独立 Gym task，并确保 R2-Dreamer 创建环境前导入 `OnlineWM.tasks`。
10. 保留 terminal observation，正确生成 episode 边界。
11. 增加 zero、random、scripted grasp/reset 和异步 reset 测试。
12. 对相机渲染、环境步进和训练数据搬运进行 profile。

### 10.4 Gate

| Gate | 通过标准 |
|---|---|
| P5-G01 | Gym task 可被发现和创建 |
| P5-G02 | 单环境与并行环境均可连续运行 |
| P5-G03 | 观测、动作、奖励和 done 的 shape、dtype、device 稳定 |
| P5-G04 | 各环境可异步结束并独立 reset |
| P5-G05 | terminal observation 在 reset 前可见 |
| P5-G06 | `is_first/is_last/is_terminal` 与真实边界一致 |
| P5-G07 | zero/random agent 不产生未定义行为或数值异常 |
| P5-G08 | 环境与相机吞吐满足后续训练的最低要求 |
| P5-G09 | 采样瓶可被夹爪接触、抓取、抬升和释放，且 reset 后状态一致 |
| P5-G10 | 4 Hz 策略动作经 120 Hz 低层插值后连续、有限且无明显振荡 |

## 11. P6：AUBO 单臂状态训练链路

### 11.1 目标

先使用本体与任务状态验证本地任务、奖励、控制和 R2-Dreamer 训练器接入，避免一开始同时排查视觉和控制问题。

状态链路通过只证明本地训练闭环正确，不代表 R2-Dreamer 视觉表征目标已经验证。

### 11.2 Gate

| Gate | 通过标准 |
|---|---|
| P6-G01 | replay 中单臂轨迹连续且 episode 边界正确 |
| P6-G02 | reward/continue 预测和 RSSM loss 有限 |
| P6-G03 | actor 动作满足单臂控制范围 |
| P6-G04 | 任务指标相对 zero/random 策略出现明确进展 |
| P6-G05 | checkpoint 能够恢复并复现行为 |
| P6-G06 | 典型失败能够归类为任务、环境、模型或控制问题 |

## 12. P7：AUBO 单臂视觉 R2-Dreamer 训练

### 12.1 目标

使用视觉与本体感知联合输入训练单臂 R2-Dreamer 策略。仿真和真机必须采用相同的输入字段、相机视角、预处理、归一化和时间同步语义。

### 12.2 训练顺序

1. 短程 smoke：创建环境、填充 replay、执行完整更新。
2. 稳定性运行：检查序列、loss、梯度、显存和 checkpoint。
3. 学习性运行：确认策略在固定评估条件上产生任务进展。
4. 正式训练：锁定配置和随机种子，不再更改任务协议。
5. 固定策略评估：禁用探索噪声，执行预注册评估协议。

### 12.3 Gate

| Gate | 通过标准 |
|---|---|
| P7-G01 | 图像、状态、动作和 episode 边界符合最终协议 |
| P7-G02 | R2 loss、RSSM、actor 和 critic 训练稳定 |
| P7-G03 | 固定评估集上的任务指标达到 P4 预注册门槛 |
| P7-G04 | 训练结果可由 checkpoint 和配置复现 |
| P7-G05 | 已完成失败类型、视觉捷径和分布外条件分析 |
| P7-G06 | 已选定进入真机准备的冻结策略版本 |

## 13. P8：sim-to-real 与部署准备

### 13.1 目标

保证真机接收到的观测和策略输出与仿真训练语义一致，并在策略外建立独立安全约束。

### 13.2 对齐工作

#### 机器人与坐标系

- 关节顺序、零位、方向和单位；
- 基座、法兰、TCP、相机和任务目标坐标系；
- 末端执行器几何、负载和工具中心点；
- 控制周期、命令保持和反馈时间戳。

#### 视觉与观测

- 相机内参、外参和畸变；
- 分辨率、裁剪、颜色通道和归一化；
- 曝光、光照、背景和遮挡；
- 图像与关节状态的时间同步；
- 缺帧、延迟和异常值处理。

#### 动力学与控制

- 关节限制、速度、加速度和控制带宽；
- 摩擦、负载、时延和执行误差；
- 仿真中需要辨识或随机化的参数；
- 关节空间与 TCP 增量适配器到真机命令的分别映射；
- 部署时依据 checkpoint 元数据选择唯一控制后端。

#### SDK 与外部资料

- AUBO SDK 接口、官方网站、版本和访问日期登记在 `reference/README.md`；
- 用户提供的 SDK 手册、接口说明和其他原始文件存放在 `reference/`；
- 真机适配代码必须记录所依据的 SDK 版本和具体资料入口；
- 外部资料尚未接入或版本未确认时，不得将真机接口标记为完成。

### 13.3 安全与部署组件

- 独立于策略的关节限位和工作空间限制；
- 速度、加速度、动作增量和必要的力/力矩限制；
- 命令超时、通信看门狗和安全停止；
- 急停、人工接管和故障复位流程；
- 策略输出限幅、平滑和异常检测；
- 全量观测、原始策略动作、实际执行命令和安全干预日志；
- 可切换回验证过的安全控制器或零动作模式。

### 13.4 部署演练顺序

1. 离线真机数据回放，不向机器人发送命令。
2. Shadow mode：策略推理并记录动作，但不执行。
3. 仿真与真机观测分布比较。
4. 低速、无接触、受限工作空间测试。
5. 人工监护下执行简化任务。
6. 达到安全 Gate 后执行完整任务。

首次真机部署默认使用冻结策略，不在真机上进行无约束在线探索。

### 13.5 Gate

| Gate | 通过标准 |
|---|---|
| P8-G01 | 仿真与真机的观测、动作、单位和坐标系契约一致 |
| P8-G02 | 相机、TCP 和任务坐标完成标定并有误差记录 |
| P8-G03 | 控制周期和端到端时延满足 P4 协议 |
| P8-G04 | 所有策略动作都经过独立安全层 |
| P8-G05 | 急停、看门狗、人工接管和回退控制均已演练 |
| P8-G06 | Shadow mode 无持续越界动作或未处理异常 |
| P8-G07 | 已形成允许进入真机任务实验的书面检查单 |
| P8-G08 | AUBO SDK、官方网站和接口版本均可从 `reference/README.md` 追溯 |

## 14. P9：AUBO 单臂真机部署与评估

### 14.1 目标

在受控条件下验证冻结策略能否完成 P4 定义的单臂任务，并记录成功、失败、安全干预和 sim-to-real 偏差。

### 14.2 分阶段实验

1. 机器人上电、回零和传感器健康检查。
2. 低速空载轨迹与静态目标测试。
3. 简化初始条件测试。
4. 完整预注册评估条件测试。
5. 对失败案例进行离线回放和根因分析。

每次扩大速度、工作空间、对象变化或任务难度，都视为新的风险级别，必须重新通过安全检查。

### 14.3 记录指标

- 任务成功率和完成时间；
- 轨迹误差、最终误差和动作平滑性；
- 安全层限幅、拒绝和急停次数；
- 人工接管次数与原因；
- 相机、状态和命令时延；
- 仿真与真机观测分布差异；
- 失败类型与可恢复性；
- 每个试验对应的策略、配置、标定和日志版本。

具体阈值和试验次数由 P4 预注册，不在结果产生后修改。

### 14.4 Gate

| Gate | 通过标准 |
|---|---|
| P9-G01 | 冻结策略可在真机控制循环中持续运行 |
| P9-G02 | 所有动作均通过安全层且日志完整 |
| P9-G03 | 在预注册条件下完成规定数量的真机试验 |
| P9-G04 | 任务指标达到 P4 预注册门槛，或形成明确失败结论 |
| P9-G05 | 急停、人工接管和失败均可追溯 |
| P9-G06 | 已形成真机部署、风险和 sim-to-real 偏差报告 |

## 15. 失败诊断顺序

出现问题时按以下顺序定位：

1. 运行时与依赖。
2. 官方环境与官方 R2-Dreamer 链路。
3. 本地任务定义是否自洽。
4. 本地环境 reset、reward、done 和动作映射。
5. sequence replay 与 episode 边界。
6. 状态训练闭环。
7. 图像、相机和视觉训练。
8. 仿真评估协议。
9. 真机标定、时序和通信。
10. 安全层与实际执行动作。
11. sim-to-real 差异和策略泛化。

不得通过扩大训练预算掩盖环境、任务或真机接口错误。

## 16. 阶段验收物与验收目录

### 16.1 验收形式

采用“一个阶段一个验收包”的方式。目录中的原始证据是验收事实来源；聊天记录、口头结论和孤立截图不能单独作为验收依据。

每个阶段目录必须包含 `acceptance.md`，并采用以下结论之一：

- `PASS`：本阶段全部强制 Gate 已通过，可以进入下一阶段；
- `FAIL`：已完成验收但存在未通过 Gate，修复后必须重新验收；
- `BLOCKED`：缺少外部资料、设备或前置条件，尚不能形成通过/失败结论。

项目级 `acceptance_index.md` 汇总所有阶段的状态、时间、代码版本、验收报告和遗留问题。

### 16.2 总体目录

```text
doc/
  OnlineWM世界模型研究纲领.md
  R2Dreamer单臂AUBO训练与真机部署开发路线.md
  runtime_versions.md
reference/
  README.md
  aubo docs/
  aubosdk/
  papers/
artifacts/
  r2dreamer/
    acceptance_index.md
    p0_runtime/
    p1_isaaclab_official/
    p2_official_proprio/
    p3_official_vision/
    p4_aubo_task/
    p5_aubo_env/
    p6_aubo_proprio/
    p7_aubo_vision/
    p8_sim2real/
    p9_real_robot/
    qa/
```

现有开发目录若尚不存在，应在对应阶段首次执行前建立。任何阶段不得把证据散落在未登记的临时目录中。

### 16.3 每个阶段的标准结构

```text
pX_stage/
  acceptance.md
  manifest.yaml
  configs/
  logs/
  metrics/
  plots/
  tests/
  videos/
  checkpoints/
  failures.md
```

各文件和目录含义如下：

| 验收物 | 形式 | 要求 |
|---|---|---|
| `acceptance.md` | Markdown | 阶段目标、范围、Gate 检查表、证据链接、结论和遗留问题 |
| `manifest.yaml` | YAML | OnlineWM/R2-Dreamer/Isaac Lab commit、dirty 状态、解释器、配置、数据、模型和运行命令索引 |
| `configs/` | YAML/JSON/TOML | 实际运行配置快照，不只保存默认配置 |
| `logs/` | TXT/JSONL | 完整终端、训练、评估和异常日志 |
| `metrics/` | JSON/CSV/NPZ | 机器可读的原始指标和统计结果 |
| `plots/` | PNG/SVG | 由 `metrics/` 生成的曲线、分布和对比图 |
| `tests/` | TXT/XML/JSON | 测试命令、测试报告和随机/边界条件记录 |
| `videos/` | MP4 | 固定策略、环境交互或真机试验录像 |
| `checkpoints/` | 模型或索引文件 | checkpoint 本体，或存储路径、大小、格式和 SHA256 |
| `failures.md` | Markdown | 失败样本、复现条件、原因分类、修复状态和剩余风险 |

`acceptance.md` 至少记录：

1. 阶段目标与验收范围；
2. 输入代码、依赖、配置、数据和模型版本；
3. 每项 Gate 的 `PASS/FAIL/BLOCKED` 状态；
4. 每项 Gate 对应的具体证据文件；
5. 阶段总评和是否允许进入下一阶段；
6. 已知限制、未解决问题和重新验收条件。

大型 checkpoint 和真机原始数据可以不提交到 Git，但必须在 `manifest.yaml` 中记录可访问位置、文件大小和 SHA256。不得用同名文件覆盖既有正式验收物。

### 16.4 各阶段核心验收物

| 阶段 | 验收目录 | 核心验收物 |
|---|---|---|
| P0 运行时 | `p0_runtime/` | Windows/GPU/驱动、Python/PyTorch/CUDA、Isaac Sim/Lab、OnlineWM/R2-Dreamer 版本；Git 状态；依赖快照；导入 smoke 日志 |
| P1 Isaac Lab 官方训练 | `p1_isaaclab_official/` | 官方任务配置、训练日志、episode 曲线、checkpoint、恢复记录、连续三次运行记录和固定策略视频 |
| P2 官方状态链路 | `p2_official_proprio/` | replay 连续性检查、RSSM/actor/critic 更新证据、loss 曲线、动作统计和 checkpoint 恢复测试 |
| P3 官方视觉链路 | `p3_official_vision/` | RGB 样本、shape/dtype 检查、R2 loss、梯度、显存/FPS、checkpoint 和固定策略视频 |
| P4 AUBO 任务协议 | `p4_aubo_task/` | 抓取抬升状态机、动作后端、联合观测、奖励、终止、成功、初始分布、相机和安全协议 |
| P5 本地 AUBO 环境 | `p5_aubo_env/` | Gym 注册、zero/random/scripted 测试、动态采样瓶交互、异步 reset、4 Hz 插值、吞吐和环境视频 |
| P6 AUBO 状态训练 | `p6_aubo_proprio/` | 训练配置、checkpoint、学习曲线、zero/random 对照、评估轨迹和失败分类 |
| P7 AUBO 视觉训练 | `p7_aubo_vision/` | 最终联合输入配置、冻结 checkpoint、成功率、训练曲线、评估视频、视觉捷径和分布外分析 |
| P8 sim-to-real | `p8_sim2real/` | 相机/TCP 标定、SDK 版本与接口映射、时延测试、shadow mode、安全检查表和回退演练 |
| P9 AUBO 真机 | `p9_real_robot/` | 每次试验的同步视频、观测、机器人状态、策略动作、实际命令、安全事件、汇总指标和部署报告 |
| QA 全阶段 | `qa/` | 测试矩阵、Gate—证据追踪表、配置完整性、失效链接、文件哈希和跨阶段回归检查 |

### 16.5 真机单次试验目录

P9 中每次试验必须使用唯一编号，禁止覆盖：

```text
p9_real_robot/
  trials/
    trial_0001/
      trial.yaml
      camera.mp4
      observations.npz
      robot_state.csv
      policy_actions.csv
      executed_commands.csv
      safety_events.csv
      result.md
```

其中：

- `trial.yaml` 记录策略、控制后端、SDK、相机标定、初始条件、操作者和时间；
- 观测、机器人状态、策略动作、实际命令和安全事件必须包含可对齐的时间戳；
- `result.md` 记录成功/失败、完成时间、人工接管、急停、异常和关联视频；
- 汇总结果必须能够反查到任意一次试验的原始输入和实际执行命令。

### 16.6 最低归档要求

训练与部署至少归档：

- 完整配置、commit 和运行时版本；
- 训练日志、曲线和 checkpoint；
- episode、reward、success 和失败类型；
- 模型 loss、梯度和潜状态诊断；
- FPS、时延、显存和墙钟时间；
- 相机、TCP 和坐标系标定；
- 真机原始观测、策略动作、实际命令和安全干预；
- 固定策略回放与真机实验视频；
- 风险评估、回退和部署报告。

## 17. 当前路线完成定义

只有同时满足以下条件，当前路线才算完成：

1. Isaac Lab 官方训练示例通过。
2. R2-Dreamer 官方状态和视觉训练链路通过。
3. AUBO 单臂任务协议完成冻结。
4. 本地单臂 Isaac Lab 环境通过压力与边界测试。
5. AUBO 单臂状态和视觉 R2-Dreamer 策略能够稳定训练、保存和恢复。
6. 冻结策略达到 P4 预注册的仿真评估门槛。
7. sim-to-real 标定、真机接口和独立安全层完成验收。
8. 完成预注册的 AUBO 单臂真机试验，或形成可追溯的失败结论。
9. 所有版本、配置、日志、曲线、标定、安全和实验资料可追溯。

## 18. 未来计划：多臂与多智能体

只有多臂智能体相关内容属于未来计划，包括：

- 第二台 AUBO 参与策略观测与动作；
- 双臂联合状态、联合动作和协同奖励；
- 集中式、分布式或分层多臂世界模型；
- 多智能体通信、角色分工和信用分配；
- 闭链约束、内力分配和双臂接触协调；
- 双臂/多机器人 benchmark 与评价协议。

这些内容不作为当前单臂策略训练与真机部署的完成条件。进入该阶段前必须单独形成多臂研究问题和开发文档。

## 19. 变更记录

| 日期 | 版本 | 变更 |
|---|---|---|
| 2026-07-25 | v1.0 | 建立官方 Isaac Lab、官方 R2-Dreamer 与本地双 AUBO 接入路线 |
| 2026-07-25 | v2.0 | 曾将本地机器人和真机接入移入未来计划 |
| 2026-07-25 | v3.0 | 纠正范围：当前目标改为 AUBO 单臂策略训练与真机部署；仅多臂/多智能体保留为未来计划 |
| 2026-07-25 | v3.1 | 冻结采样瓶抓取抬升任务、第一台 AUBO、联合输入、统一相机与 4 Hz 控制基线；保留关节空间和 TCP 增量控制后端，并建立外部资料入口 |
| 2026-07-25 | v3.2 | 增加统一阶段验收包、P0—P9/QA 验收目录、阶段核心验收物和真机单次试验证据规范；明确 P1 与后续 R2-Dreamer/OnlineWM 运行时 Gate，并补充 RL-Games smoke 命令模板 |
