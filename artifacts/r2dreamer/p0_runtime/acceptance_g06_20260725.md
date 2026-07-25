# P0-G06 安装与导入补充验收

## 结论

**PASS。P0-G06 / P2 与 P0-G06 / P5 两个入口 Gate 均已满足。**

最终正式批次为 `20260725_g06c`。验收在 P0/P1 冻结的同一解释器
`D:\Anaconda\envs\isaaclab\python.exe` 内执行，保持 PyTorch 2.7.0+cu128、
CUDA 12.8、Isaac Sim 5.1.0.0 和 Isaac Lab commit 不变。

原始 [acceptance.md](acceptance.md) 是 18:22 完成的 P0 基线验收，不作同名覆盖；
本文件是 P0-G06 到期后的追加验收事实来源。

## 安装版本与策略

- R2-Dreamer：0.1.0，editable install，源码
  `D:\Software\R2-Dreamer`，commit
  `546e4fab8146ea4b14e1d7726bbc1a8a1d50322f`。
- TensorDict：0.8.3。
- TorchRL：0.8.1，原生扩展已加载。
- OnlineWM：0.1.0，editable install，源码
  `D:\Project\S2R\OnlineWM\OnlineWM\source\OnlineWM`。
- `packaging`：恢复并保持 Isaac Sim 要求的 23.0。
- 运行约束：`PYTHONNOUSERSITE=1`。

R2-Dreamer 上游 `pyproject.toml` 精确声明 Torch 2.8、TorchRL 0.9.2 和
TensorDict 0.9.1；直接解析依赖会破坏已验收的 P1 Torch/Isaac 运行时。因而本次采用
兼容覆盖：R2-Dreamer 与 OnlineWM 使用 `--no-deps` editable install，并选用支持
PyTorch 2.7 的 TorchRL 0.8.1 / TensorDict 0.8.3。该覆盖只证明 P0-G06 要求的安装与
导入链路，不替代 P2 的正式训练验收。

## Gate 检查

| Gate | 状态 | 证据与说明 |
|---|---|---|
| P0-G06 / P2 | PASS | [g06_probe_20260725_g06c.json](metrics/g06_probe_20260725_g06c.json)：R2-Dreamer distribution 为 editable；commit 匹配；`dreamer`、`rssm`、`buffer`、`networks`、`trainer`、`envs.isaaclab` 等核心模块均从冻结 checkout 导入；TensorDict 运算、TorchRL ReplayBuffer 和原生扩展均通过 |
| P0-G06 / P5 | PASS | 同一 probe 在不注入仓库 bootstrap 的情况下导入 OnlineWM distribution 与 `OnlineWM.tasks`；模块来自项目源码，`Template-Onlinewm-Direct-v0` 注册成功 |
| 冻结运行时保持 | PASS | probe 记录 Python 3.11.15、Torch 2.7.0+cu128、CUDA 12.8、RTX 4060；`packaging==23.0` 见 [最终环境快照](configs/requirements_g06_after_20260725_g06c.txt) |
| P1 回归 | PASS | [官方 Cartpole 5 epoch 回归日志](../p1_isaaclab_official/logs/g06_p1_regression_20260725_g06c.txt)：16 个 `cuda:0` 环境正常训练，生成 checkpoint，并到达 `MAX EPOCHS NUM!` |

## 验证

- 联合无头探针：[日志](logs/g06_import_smoke_20260725_g06c.txt)；
  [机器可读结果](metrics/g06_probe_20260725_g06c.json)，12/12 checks PASS。
- 安装记录：[g06_install_20260725_g06c.json](configs/g06_install_20260725_g06c.json)。
- Pytest：[pytest_g06_final_20260725_g06c.txt](tests/pytest_g06_final_20260725_g06c.txt)，
  `7 passed`。
- Ruff：[ruff_g06_final_20260725_g06c.txt](tests/ruff_g06_final_20260725_g06c.txt)。
- PowerShell 语法：
  [powershell_syntax_g06_20260725_g06c.txt](tests/powershell_syntax_g06_20260725_g06c.txt)。
- 补充验收物完整性：[sha256_g06_20260725.csv](tests/sha256_g06_20260725.csv)。

## 已知限制

- [pip check](logs/pip_check_g06_after_20260725_g06c.txt) 非零。原 P0 基线已有
  `torchaudio` 缺失、FastAPI/Starlette 与 wheel/packaging 三项冲突；本次新增的
  R2-Dreamer metadata 精确版本差异属于上述兼容覆盖。实际导入、原生扩展、replay
  操作及 P1 回归均已通过。
- `20260725_g06a` 使用 TorchRL 0.9.2，原生扩展加载失败且把 `packaging` 升至
  25.0，不计入通过；`20260725_g06b` 已通过联合探针，但仍残留首次尝试安装的
  `pyvers`；最终 `g06c` 已清理该遗留并重新验收。
- P2 必须独立验收 R2-Dreamer 的 replay、RSSM、actor-critic、checkpoint 与恢复链路；
  如 P2 用到 0.9 专有 API，应做最小兼容修复或建立独立且仍能加载 Isaac Lab 的运行时，
  不能无记录地升级当前 P1 冻结环境。

