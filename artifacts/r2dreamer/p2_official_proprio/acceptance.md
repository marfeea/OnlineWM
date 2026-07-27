# P2 R2-Dreamer 官方状态链路阶段验收

## 当前结论

**路线图 10,000-step 官方 smoke：PASS；缩小配置六项 Gate smoke：PASS。**

官方批次 `official_upstream_10000_20260725_run2` 使用冻结上游原生 `train.py`，配置为
`size12M`、`batch_size=16`、`batch_length=64`、`train_ratio=512`、4 个环境和
10,000 环境步。唯一覆盖是 Windows 无 Triton 所需的 `model.compile=false`；
`model.log_grads=false` 及其余模型/训练参数保持官方默认。

该批次从 2026-07-25 21:35 运行至 2026-07-26 10:19，耗时约 12 小时 44 分；
上游训练循环正常返回后生成 118,582,504 字节的 `latest.pt`。109 条 episode 记录中，
前 10 条平均 score 为 2.74，末 10 条为 266.96，最后一条为 292.92。日志、训练指标、
160 个 agent checkpoint 张量和 276 个 optimizer 张量均为有限值，无 traceback 或 CUDA OOM。

机器可读结论见
[official_smoke_acceptance_20260725_run2.json](tests/official_smoke_acceptance_20260725_run2.json)，
全部检查为 `true`；后台收尾结果见
[run_status.json](official_upstream_10000_20260725_run2/run_status.json)。上游 logger 不会强制写
最终步，因此最后一个事件步为 9919；但 `latest.pt` 位于训练循环之后，只有内部 step 达到
配置的 10,000 并返回后才会保存。

本轮已经打通并机器验证：

```text
Isaac Lab Cartpole
  -> IsaacLabVecEnv
  -> strict-episode sequence replay
  -> RSSM + reward/continue heads
  -> imagined rollout + actor-critic
  -> bounded action
  -> checkpoint restore + continued updates
```

通过批次为 `smoke_initial_20260725`（320 环境步）和
`smoke_resume_20260725`（从 checkpoint 恢复后 240 环境步）。两次均使用冻结
R2-Dreamer commit `546e4fab8146ea4b14e1d7726bbc1a8a1d50322f`、4 个
`Isaac-Cartpole-Direct-v0` 环境和 `model.rep_loss=r2dreamer`。

缩小批次用于证明六项 Gate 的观测/判定链路有效；官方 10K 批次则证明默认模型、batch、
sequence 与 train ratio 能在 RTX 4060 8GB 上完成训练和 checkpoint 保存。由于冻结上游
仍存在跨 reset replay、连续动作未真正绑定以及缺少恢复入口等问题，官方原生 smoke 的
PASS 不替代下方 Gate 适配验收，也不等同于 510K 完整预算训练完成。

## 已落地的兼容与 Gate 修复

- `StrictEpisodeTrainer` 为每个物理 episode 分配唯一 replay trajectory ID。冻结上游按
  env stream 固定 ID，会允许序列跨 reset，与 P2-G01 冲突。
- P2 验收使用 float32 更新。冻结上游 AMP 首次更新曾出现梯度溢出，虽然后续
  GradScaler 可恢复，但不满足 P2-G03 的全程有限要求。
- actor 送入环境及 replay 前裁剪至 `[-1,1]`。冻结上游 `bounded_normal` 当前实际返回
  普通 Normal，未应用其 `Bound` 包装。
- checkpoint 保存 agent、递归 optimizer state、resolved config 与诊断；恢复时先校验
  agent tensor digest，再继续收集 replay 并更新。
- 验收器同时扫描 `console.log` traceback；这是必要的，因为 Isaac App 在一次失败中把
  Python traceback 转成了进程退出码 0。

## Gate smoke 结果

| Gate | 状态 | 证据 |
|---|---|---|
| P2-G01 | PASS | 初始 70、恢复 50 个 replay batch 均为连续时间索引、单一 episode ID，且序列内部无 `is_first` |
| P2-G02 | PASS | 两批中 RSSM、reward、continue、actor、critic 参数均发生非零变化 |
| P2-G03 | PASS | 初始/恢复的 loss、metrics、梯度与 replay 潜状态均有限；梯度观测分别为 70/50 次 |
| P2-G04 | PASS | 初始动作 332 个，std 0.6643；恢复动作 252 个，std 0.6683；范围均为 `[-1,1]` |
| P2-G05 | PASS | checkpoint 加载前后 agent digest 同为 `c775a4e...a89f3d`，恢复后完成 50 次更新并生成新 checkpoint |
| P2-G06 | PASS | 初始记录 8 个完整 episode，恢复日志继续产生多条 episode score/length |

机器可读汇总见
[smoke_acceptance_20260725.json](tests/smoke_acceptance_20260725.json)，六个 Gate 均为
`true`。原始诊断见
[初始运行](smoke_initial_20260725/diagnostics.json)与
[恢复运行](smoke_resume_20260725/diagnostics.json)。

仓库级验证为 `9 passed`；P2 脚本 Ruff lint/format、PowerShell 语法、manifest YAML
解析与必需证据检查均通过。证据文件哈希见 [sha256.csv](tests/sha256.csv)。

## 复现

从项目根目录、允许 Isaac Sim 写入其用户/材质/OptiX 缓存的 PowerShell 执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File scripts\p2\collect_smoke.ps1 -RunId <UNIQUE_RUN_ID>
```

该收集器默认复现本次缩小 Gate smoke，并拒绝覆盖已有 Run ID。
