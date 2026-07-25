# P1 Isaac Lab 官方训练验收

## 结论

**PASS。P1 全部强制 Gate 已通过。**

允许结束 P1；进入 P2 前仍必须按 P0 约定补齐 P0-G06 / P2（在兼容解释器中安装、
冻结并导入 R2-Dreamer 和 TensorDict）。本次验收只覆盖 Isaac Lab 官方 Cartpole 的环境、
训练、日志、checkpoint、恢复和固定策略视频链路，不对 R2-Dreamer 作通过结论。

正式验收批次为 `20260725_p1e`，三次训练运行目录分别为：

- `2026-07-25_19-13-51`
- `2026-07-25_19-15-27`
- `2026-07-25_19-17-00`

## 输入版本与实际配置

- OnlineWM：`01bd31b15c986737e9f5b123af9f7699faee1737`，验收时 dirty（P1 新增代码与证据）。
- Isaac Lab：`f4aa17f87e2e5db5484f0b5974918573e8918ce2`，
  `v2.3.2-13-gf4aa17f87e2`，dirty 状态与 P0 一致。
- 解释器：`D:\Anaconda\envs\isaaclab\python.exe`，Python 3.11.15，
  强制 `PYTHONNOUSERSITE=1`。
- 后端：RL-Games 1.6.5；GPU：NVIDIA GeForce RTX 4060。
- 官方任务：`Isaac-Cartpole-Direct-v0`，16 个并行环境，150 个训练 epoch。
- 实测必须覆盖：
  - `agent.params.config.minibatch_size=512`，使 `16 × 32` rollout batch 与 minibatch 相容；
  - `+agent.params.config.torch_compile=false`，避免 Windows 冻结运行时缺少 Triton。

完整命令、工作目录和配置索引见 [manifest.yaml](manifest.yaml)；
三次实际环境/agent 快照见 `configs/actual_20260725_p1e_run*_*.yaml`。

## Gate 检查

| Gate | 状态 | 证据与说明 |
|---|---|---|
| P1-G01 | PASS | [environment_probe_20260725_p1e.json](metrics/environment_probe_20260725_p1e.json)：headless 在 `cuda:0` 创建 16 个官方 Cartpole 环境 |
| P1-G02 | PASS | 同一 probe 连续执行 640 步；观测每步变化、reward 有变化、数值均有限，并观测到 terminated/truncated 事件 |
| P1-G03 | PASS | [run 1](logs/train_20260725_p1e_run1.txt)、[run 2](logs/train_20260725_p1e_run2.txt)、[run 3](logs/train_20260725_p1e_run3.txt) 均跑满 150 epoch；每次生成 TensorBoard event 和 8 个 checkpoint |
| P1-G04 | PASS | [training_summary_20260725_p1e.json](metrics/training_summary_20260725_p1e.json)：三次运行早期 return 中位数 6.235，末期 255.220，绝对提升 248.984；[曲线](plots/episode_returns_20260725_p1e.svg) |
| P1-G05 | PASS | 完全相同的正式训练命令连续执行 3 次，训练耗时分别为 75.99 s、73.73 s、76.78 s，均无 traceback 并正常到达 `MAX EPOCHS NUM!` |

机器可读的 Gate 汇总见
[acceptance_validation_20260725_p1e.json](tests/acceptance_validation_20260725_p1e.json)。

## Checkpoint 恢复与固定策略

- 验收 checkpoint：
  [cartpole_20260725_p1e.pth](checkpoints/cartpole_20260725_p1e.pth)，31,253 bytes，
  SHA256 `c37ae4de00bc0df8fb6d5ce702f317b08e6c15ff0ca584e830f8391d30597f7c`。
- [恢复日志](logs/resume_20260725_p1e.txt)明确加载上述 checkpoint，并从 epoch 151
  连续运行到 epoch 155，生成恢复后的 checkpoint。
- [固定策略视频](videos/fixed_policy_20260725_p1e.mp4)可解码 299 帧，
  1280×720、60 FPS、439,584 bytes，SHA256
  `5343e0fc39416147ca5a2bd16e85c481a87f2afc4672cdbd1898a751eb01fa3b`。

## 代码与证据包验证

- Pytest：[pytest_final_20260725_p1e.txt](tests/pytest_final_20260725_p1e.txt)，`7 passed`。
- Ruff：[ruff_final_20260725_p1e.txt](tests/ruff_final_20260725_p1e.txt)，全部通过。
- PowerShell 5.1 语法：
  [powershell_syntax_20260725_p1e.txt](tests/powershell_syntax_20260725_p1e.txt)。
- 全包文件完整性：[sha256.csv](tests/sha256.csv)。

## 已知限制与重新验收条件

已修复失败与风险见 [failures.md](failures.md)。以下任一条件发生变化时必须重新验收 P1：

- P0 冻结的解释器、Isaac Sim/Lab、PyTorch、CUDA、RL-Games、GPU 或驱动发生变化；
- Isaac Lab commit、官方 Cartpole 配置、minibatch 或 `torch_compile` 覆盖发生变化；
- NVIDIA 官方 USD 资产地址不可访问，且没有受控的本地资产镜像；
- 训练命令、环境数量、epoch 预算或 checkpoint 格式发生变化。

三次正式训练使用相同 seed 42，曲线一致；这满足“同一命令连续执行 3 次”的可重复性
Gate，但不等同于跨随机种子的鲁棒性评估。跨 seed 统计留待后续策略评估阶段。
