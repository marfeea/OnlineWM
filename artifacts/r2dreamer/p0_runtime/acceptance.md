# P0 运行时验收

## 结论

**PASS。允许进入 P1。**

本次验收冻结了 P1 的唯一解释器、Isaac Lab 源码、RL-Games 后端和 GPU 设备，并在
headless Isaac App 内完成官方 Cartpole 任务注册与导入 smoke。P0-G06 是 P2/P5 的延后入口
Gate，不阻塞 P1；其未满足部分在本报告中单独列出。

## 输入版本

- OnlineWM：`b15613ac8907188ad1b51c38170c190b51abfe25`，dirty。
- Isaac Lab：`f4aa17f87e2e5db5484f0b5974918573e8918ce2`，
  `v2.3.2-13-gf4aa17f87e2`，dirty。
- 解释器：`D:\Anaconda\envs\isaaclab\python.exe`，Python 3.11.15。
- 后端：RL-Games 1.6.5。
- GPU：NVIDIA GeForce RTX 4060，驱动 591.86，Torch CUDA 12.8。
- 完整清单：[manifest.yaml](manifest.yaml) 和
  [runtime_probe.json](metrics/runtime_probe.json)。

## Gate 检查

| Gate | 状态 | 证据与说明 |
|---|---|---|
| P0-G01 | PASS | [isaac_import_smoke.txt](logs/isaac_import_smoke.txt)：先启动 headless Isaac App，再导入 `isaaclab`、`isaaclab_tasks`、RL-Games；`Isaac-Cartpole-Direct-v0` 注册成功 |
| P0-G02 | PASS | [gpu.txt](logs/gpu.txt) 和 [runtime_probe.json](metrics/runtime_probe.json)：Torch CUDA 可用并选中 RTX 4060 `cuda:0` |
| P0-G03 | PASS | [onlinewm_git.txt](logs/onlinewm_git.txt) 与 [isaaclab_git.txt](logs/isaaclab_git.txt)：commit、分支、remote 和 dirty 状态均已记录 |
| P0-G04 | PASS | `scripts/p0/install_runtime.ps1` 提供固定版本的空终端重建入口；[install_plan.txt](tests/install_plan.txt) 记录无副作用计划验证 |
| P0-G05 | PASS | [doc/runtime_versions.md](../../../doc/runtime_versions.md) 已写入版本、入口、P1 命令和待确认项 |
| P0-G06 / P2 | BLOCKED（尚未到期） | R2-Dreamer 和 TensorDict 尚未安装；进入 P2 前必须完成导入 smoke 并冻结 R2-Dreamer commit |
| P0-G06 / P5 | BLOCKED（尚未到期） | OnlineWM 可由仓库 bootstrap 导入，但尚未作为 distribution editable-install；进入 P5 前必须补做严格导入验证 |

## 验证

- 纯 Python 测试：[pytest.txt](tests/pytest.txt)。
- Ruff：[ruff.txt](tests/ruff.txt)。
- PowerShell 5.1 语法和安装计划：[powershell_syntax.txt](tests/powershell_syntax.txt)、
  [install_plan.txt](tests/install_plan.txt)。
- 验收包文件完整性：[sha256.csv](tests/sha256.csv)。

## 已知限制与重新验收条件

详见 [failures.md](failures.md)。发生以下任一变化时必须重新执行 P0：

- P1 解释器、Isaac Sim、Isaac Lab、PyTorch、CUDA 或 RL-Games 版本变化；
- Isaac Lab commit 或工作树状态变化；
- GPU/驱动变化；
- 启动时不再强制 `PYTHONNOUSERSITE=1`；
- 从现有 `isaaclab` 环境切换到重建的 `onlinewm-p0` 环境。
