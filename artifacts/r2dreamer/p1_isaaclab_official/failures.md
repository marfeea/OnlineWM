# P1 失败样本、修复与剩余风险

## 已修复失败

| 失败 | 复现条件与证据 | 原因 | 修复 | 状态 |
|---|---|---|---|---|
| 官方 USD 不可达 | 首次沙箱 smoke；[日志](logs/smoke_missing_asset_20260725.txt) | Cartpole USD 使用 NVIDIA 远程资产 URL，受限网络中无法解析为有效文件 | 正式验收在获准联网的进程中读取官方资产；不修改 USD 或任务 | 已修复 |
| rollout batch 与 minibatch 不整除 | 16 env、32 horizon、官方 minibatch 16384；[日志](logs/smoke_batch_mismatch_20260725.txt) | batch 为 512，RL-Games 要求可被 minibatch 整除 | 显式覆盖 `agent.params.config.minibatch_size=512` | 已修复 |
| Windows 缺少 Triton | 首次应用 minibatch 修复；`logs/train_20260725_p1c_run1.txt` | RL-Games 1.6.5 默认启用 `torch.compile`，冻结环境无可用 Triton | 使用官方配置入口新增 `torch_compile=false`，不改变 P0 依赖 | 已修复 |
| Hydra 拒绝新增键 | `logs/train_20260725_p1d_run1.txt` | `torch_compile` 不在上游 YAML 的结构化键集合中 | 改用 Hydra 新增键语法 `+agent.params.config.torch_compile=false` | 已修复 |
| TensorBoard 分析器未发现 scalar | [分析失败日志](logs/analysis_20260725_p1e.txt) | RL-Games event 位于运行目录的 `summaries/` 子目录 | 分析器递归定位唯一 event 目录；三次既有训练无需重跑 | 已修复 |
| PowerShell 将 Gym 提示视为 stderr | 恢复/播放工具层显示非零，但日志无 traceback 且产物完整 | 旧 `gym` 包向 stderr 打印迁移提示，PowerShell 5.1 包装为 `NativeCommandError` | 验收器以 traceback、完成标记和产物联合判定；采集器保留完整 stderr | 已缓解 |

`p1a`—`p1d` 是采集器/兼容性调试尝试，不纳入 P1-G05 的三次正式运行。
正式统计只使用 `20260725_p1e` 下明确列入 `manifest.yaml` 的三个运行目录。

## 剩余风险

| 风险 | 影响 | 当前处置 | 重新检查时点 |
|---|---|---|---|
| 官方 Cartpole USD 是远程资产 | 离线或服务不可用时无法从空缓存重现 | manifest 固定 URL 来源对应的 Isaac Lab commit；正式命令要求联网 | 重新执行 P1 或构建受控本地资产镜像时 |
| `torch.compile` 在 Windows 冻结运行时禁用 | 性能路径与默认 RL-Games 行为不同 | 训练吞吐仍约 900–1200 FPS，Gate 不受影响 | 升级 RL-Games/Torch 或安装受支持 Triton 后 |
| 三次运行使用相同 seed 42 | 证明命令级重复性，不证明跨 seed 稳健性 | 明确限制，不扩大 P1 结论 | 后续策略评估 |
| 300-step 录像编码为 299 帧 | Gym RecordVideo 的触发步不计入编码帧 | 已逐帧解码验证 299 帧、1280×720、60 FPS | 更换 Gymnasium/录像包装器后 |
