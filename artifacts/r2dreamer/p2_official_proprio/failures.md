# P2 施工失败记录

| 尝试 | 现象 | 原因 | 处置 |
|---|---|---|---|
| `preflight_20260725` | `conda run` 120 秒无可见进展 | 子进程输出缓冲，无法定位初始化阶段 | 改用冻结解释器直接启动 |
| `preflight2/3_20260725` | Isaac Sim 初始化极慢并在 5 分钟超时 | 沙箱阻止写入 `user.config.json`、材质缓存和 OptiX cache；日志出现 `PermissionError`/readonly database | 经授权在沙箱外运行；初始化恢复到几十秒 |
| `preflight4_20260725` | 首次模型更新断言失败 | 缩小 two-hot head 到 31，但分布仍固定 255 bins，是错误的预检覆盖 | 撤销 reward/critic shape 覆盖 |
| `preflight5_20260725` | 558 次更新后诊断 JSON 拒绝 NaN | 上游 AMP 首次梯度溢出；同时上游 `bounded_normal` 实际未绑定动作 | P2 验收使用 float32 更新；送环境动作裁剪到 `[-1,1]`；诊断保留非有限判定 |

上述失败批次不计入通过结论。通过批次为 `smoke_initial_20260725` 与
`smoke_resume_20260725`。
