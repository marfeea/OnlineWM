# P3 已知问题与剩余风险

## 上游原生 AMP 数值不稳定

直接运行官方 `train.py env=isaaclab_vision model.rep_loss=r2dreamer` 时，进程能完成并保存
checkpoint，但短预检在 step 36、44、76 等记录出现 `loss/barlow=NaN` 或非有限梯度。
GradScaler 随后可能恢复，仍不满足 P3-G03/G04 的“持续有限”标准。Gate smoke 使用 float32
更新且禁用梯度缩放，初始/恢复共 38 次更新全部有限。正式默认配置训练前仍需决定是否固定
采用这一数值模式，并重新测量吞吐与显存。

## 上游连续动作未实际有界

官方原生预检记录的动作约落在 `[-2.5, 2.36]`，超出 Isaac Lab wrapper 声明的
`[-1,1]`。Gate 运行器在进入环境和 replay 前显式裁剪；固定策略视频中的动作也在合法范围。
这是冻结上游分布实现的兼容问题，P3 不直接修改外部 checkout。

## 正式预算未执行

本轮只完成 80-step 上游原生预检，以及 96-step 初始/80-step 恢复的缩小 Gate smoke。
路线图 20,000-step smoke 和 1.01M 完整预算仍为 PENDING。缩小模型的约 6.589 GB 显存峰值
不能直接外推到官方 size12M 配置。

## 可复现性提示

Isaac Lab 日志提示环境 seed 未显式设置。R2-Dreamer seed 已固定为 0，但正式多 seed 验收前
需确认 Isaac 环境侧随机源也被完整绑定。

