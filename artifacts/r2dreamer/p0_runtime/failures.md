# P0 已知问题与风险

| 项目 | 影响 | 当前处置 | 重新检查时点 |
|---|---|---|---|
| R2-Dreamer 与 TensorDict 未安装 | 阻塞 P2，不阻塞 P1 | P2 前在同一兼容运行时安装、冻结 commit 并补导入 smoke | P2 入口 |
| OnlineWM 未作为 distribution editable-install | 严格的 P5 导入 Gate 尚未通过 | 当前 P0 probe 使用仓库 `source/OnlineWM` bootstrap；P5 前执行 editable install 和无 bootstrap 导入测试 | P5 入口 |
| Isaac Lab 为 `v2.3.2-13`，不是干净的 `v2.3.2` tag | 复现实验必须依赖精确 commit，不能只写版本号 | 已冻结 commit `f4aa17f...`；重建脚本检出该 commit | P1 前及任何源码变更后 |
| Isaac Lab 有未跟踪文件 `2.7.0+cu128` | dirty 状态可能掩盖后续非预期改动 | 不擅自删除；已记录在原始 Git 日志 | P1 前人工确认 |
| 环境中存在外部 editable package `D:\project\IsaacTest\Test\source\test` | 可能造成命名或任务注册污染 | 不擅自卸载；P1 仅运行官方任务，并保留完整 `pip freeze` | P1 smoke |
| 用户级 Conda 配置含过期/重复镜像 | 从空环境安装可能受镜像影响 | 重建脚本使用 `--override-channels` 固定 conda-forge/defaults | 重新建环境 |
| Kit 日志含弃用、集显跳过和 KVDB lock warning | 当前未影响导入、CUDA 选择或任务注册 | 保留完整 smoke 日志；若 P1 出现启动/缓存问题再升级处理 | P1 smoke |
