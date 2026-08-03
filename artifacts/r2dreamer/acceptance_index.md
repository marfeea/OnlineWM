# R2-Dreamer / AUBO 阶段验收索引

| 阶段 | 状态 | 验收时间 | OnlineWM commit | 验收报告 | 遗留问题 |
|---|---|---|---|---|---|
| P0 运行时 | PASS（G06 已补齐） | 2026-07-25 | 基线 `b15613a`；G06 `01bd31b`（dirty） | [基线](p0_runtime/acceptance.md)；[G06 补充](p0_runtime/acceptance_g06_20260725.md) | R2-Dreamer 使用 Torch 2.7 兼容覆盖；P2 需独立训练验收 |
| P1 Isaac Lab 官方训练 | PASS | 2026-07-25 | `01bd31b15c986737e9f5b123af9f7699faee1737`（dirty） | [正式验收](p1_isaaclab_official/acceptance.md)；[G06 后回归](p1_isaaclab_official/p0_g06_addendum_20260725.md) | 官方 USD 需联网；Windows 禁用 `torch.compile` |
| P2 官方状态链路 | 施工中（官方 10K smoke PASS） | 2026-07-27 | `2d64fb6`（dirty） | [阶段验收](p2_official_proprio/acceptance.md)；[官方 10K 结果](p2_official_proprio/tests/official_smoke_acceptance_20260725_run2.json) | 默认模型/batch 的 10K smoke 与缩小六 Gate 验证均 PASS；510K 完整预算训练尚未执行 |
| P3 官方视觉链路 | 施工中（原生预检完成、缩小七 Gate PASS） | 2026-07-28 | 基线 `45079b0`（dirty） | [施工中验收](p3_official_vision/acceptance.md) | 官方 20K smoke 与完整预算待执行；原生 AMP/动作边界需适配 |
| P4 AUBO 任务协议 | 未开始 | — | — | — | 依赖 P3 |
| P5 本地 AUBO 环境 | 未开始（入口已满足） | — | — | — | 依赖 P4；P0-G06 / P5 已 PASS |
| P6 AUBO 状态训练 | 未开始 | — | — | — | 依赖 P5 |
| P7 AUBO 视觉训练 | 未开始 | — | — | — | 依赖 P6 |
| P8 sim-to-real | 未开始 | — | — | — | 依赖 P7 |
| P9 AUBO 真机 | 未开始 | — | — | — | 依赖 P8 |
