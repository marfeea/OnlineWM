# P3 R2-Dreamer 官方视觉链路施工中验收

## 当前结论

**官方原生视觉预检已完成；缩小配置 P3-G01 至 P3-G07 全部 PASS；P3 仍为施工中。**

测试顺序遵循“官方优先”：首先直接运行冻结上游的 `train.py`，选择
`env=isaaclab_vision model.rep_loss=r2dreamer`。原生 80-step 预检成功启动官方
`Isaac-Cartpole-RGB-Camera-Direct-v0`、构造 CNN encoder/RSSM、执行 16 次更新并保存
3,122,637 字节 checkpoint。不过，上游 AMP warm-up 在若干更新产生非有限 Barlow loss/
梯度，连续动作也超出 `[-1,1]`，因此只判定为“入口完成、Gate 未通过”。原始指标位于
`official_preflight_20260728_run2/metrics.jsonl`。

随后使用不修改上游仓库的 P3 Gate 运行器执行初始与恢复训练。运行器复用 P2 已验证的
严格 episode replay，使用 float32 更新并在送入环境/回放前裁剪动作。机器判定见
`tests/smoke_acceptance_gate_20260728.json`，七项 Gate 及两份 console 扫描均为 `true`。

## Gate smoke 结果

| Gate | 状态 | 核心证据 |
|---|---|---|
| P3-G01 | PASS | 初始/恢复分别观测 98/90 批 `uint8 [B,64,64,3]` RGB，范围稳定在 `[0,255]` |
| P3-G02 | PASS | `is_first`、`is_last`、`is_terminal` 与前后 done 语义一致；reset reward 为 0；捕获 terminal/reset PNG |
| P3-G03 | PASS | 初始 21、恢复 17 次 `loss/barlow` 全部有限，范围分别为 553.91–614.27 与 548.24–611.11 |
| P3-G04 | PASS | encoder/RSSM 每次更新前梯度均有限，且两模块参数在初始和恢复批次均发生非零变化 |
| P3-G05 | PASS | 训练动作裁剪到 `[-1,1]`；两段 48 帧 eval-mode 固定策略动作标准差约 0.0042/0.0043 |
| P3-G06 | PASS | checkpoint digest 加载前后完全一致，恢复后继续 17 次更新并生成新 checkpoint 与固定策略视频 |
| P3-G07 | PASS | 环境 FPS 85.24/79.35，训练 FPS 62.23/66.37，显存峰值约 6.589 GB，墙钟 27.87/26.08 秒 |

固定策略视频存放于 `videos/`。该目录受仓库 `.gitignore` 约束，文件保留在本机验收目录，
其大小与 SHA256 已写入 `tests/sha256.csv`。RGB 初始帧、terminal 帧和 reset 帧均以无损 PNG
保存在 `rgb_samples/`。

## 已落地施工内容

- `p3_runtime.py`：图像契约、reset/terminal 语义、R2 loss、encoder/RSSM 梯度、动作与性能诊断。
- `train_vision_chain.py`：冻结上游视觉链路、全精度 Gate 更新、checkpoint 恢复、RGB 采样和固定策略 MP4。
- `verify_acceptance.py`：P3-G01 至 P3-G07 的机器判定及 traceback/CUDA OOM 扫描。
- `collect_smoke.ps1`：不可覆盖的初始/恢复两阶段证据采集器。
- 两组测试遵循测试先行，先观察缺失模块失败，再实现并转绿。

## 未完成项

路线图指定的 20,000-step 官方 smoke 尚未执行，官方默认模型/批大小/训练比率下的正式视觉
性能与稳定性尚未验收，1.01M 完整预算训练也未开始。因此当前结论不允许将 P3 标为完成，
也不解除 P4 对 P3 的依赖。

## 复现

从项目根目录、允许 Isaac Sim 写入用户/材质/OptiX 缓存的 PowerShell 执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File scripts\p3\collect_smoke.ps1 -RunId <UNIQUE_RUN_ID>
```

