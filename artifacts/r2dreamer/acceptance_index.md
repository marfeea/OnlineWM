# R2-Dreamer / AUBO 阶段验收索引

| 阶段 | 状态 | 验收时间 | OnlineWM commit | 验收报告 | 遗留问题 |
|---|---|---|---|---|---|
| P0 运行时 | PASS | 2026-07-25 | `b15613ac8907188ad1b51c38170c190b51abfe25`（dirty） | [p0_runtime/acceptance.md](p0_runtime/acceptance.md) | P2 前安装并冻结 R2-Dreamer/TensorDict；P5 前完成 OnlineWM editable install Gate |
| P1 Isaac Lab 官方训练 | 未开始 | — | — | — | 依赖 P0 |
| P2 官方状态链路 | 未开始 | — | — | — | 依赖 P1 和 P0-G06 的 R2-Dreamer 子 Gate |
| P3 官方视觉链路 | 未开始 | — | — | — | 依赖 P2 |
| P4 AUBO 任务协议 | 未开始 | — | — | — | 依赖 P3 |
| P5 本地 AUBO 环境 | 未开始 | — | — | — | 依赖 P4 和 P0-G06 的 OnlineWM 子 Gate |
| P6 AUBO 状态训练 | 未开始 | — | — | — | 依赖 P5 |
| P7 AUBO 视觉训练 | 未开始 | — | — | — | 依赖 P6 |
| P8 sim-to-real | 未开始 | — | — | — | 依赖 P7 |
| P9 AUBO 真机 | 未开始 | — | — | — | 依赖 P8 |
