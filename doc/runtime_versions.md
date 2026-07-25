# P0 运行时与版本冻结

> 冻结时间：2026-07-25 18:22:25 +08:00  
> P0 验收包：`artifacts/r2dreamer/p0_runtime/`  
> 适用范围：P1 Isaac Lab 官方训练 smoke；P2/P5 仍受下文入口 Gate 限制。

## 1. 唯一运行入口

P1 统一使用以下解释器，不混用系统 Python、`onlinewm` 环境或
`C:\isaac-sim\kit\python.bat`：

```text
D:\Anaconda\envs\isaaclab\python.exe
```

从空 PowerShell 终端启动时统一使用：

```powershell
$env:PYTHONNOUSERSITE = "1"
conda run --name isaaclab python <SCRIPT> <ARGS>
```

`PYTHONNOUSERSITE=1` 是运行契约的一部分。未设置时，该环境会启用用户级
`site-packages`，可能加载另一套 Torch 或 Isaac Lab。

## 2. 主机与 GPU

| 项目 | 冻结值 |
|---|---|
| 操作系统 | Windows 11 Pro 25H2，build 26200 |
| PowerShell | 5.1.26100.8655 |
| GPU | NVIDIA GeForce RTX 4060 |
| GPU UUID | `GPU-e04a936b-14ad-b9f5-396a-51d4a215a5e8` |
| 显存 | 8188 MiB（`nvidia-smi`）；Isaac Kit 报告可用设备显存 7956 MB |
| NVIDIA 驱动 | 591.86 |
| PyTorch CUDA runtime | 12.8 |
| CUDA device | `cuda:0`，compute capability 8.9 |

原始证据见
`artifacts/r2dreamer/p0_runtime/logs/host.txt`、
`artifacts/r2dreamer/p0_runtime/logs/gpu.txt` 和
`artifacts/r2dreamer/p0_runtime/logs/isaac_import_smoke.txt`。

## 3. Python 与依赖

| 组件 | 冻结值 |
|---|---|
| Conda 环境 | `isaaclab` |
| Python | 3.11.15 |
| Python executable | `D:\Anaconda\envs\isaaclab\python.exe` |
| user site | 禁用 |
| PyTorch | 2.7.0+cu128 |
| torchvision | 0.22.0+cu128 |
| Isaac Sim | 5.1.0.0 |
| Isaac Lab `VERSION` | 2.3.2 |
| Isaac Lab Python distribution | 0.54.3 |
| Isaac Lab Tasks | 0.11.14 |
| RL-Games | 1.6.5 |
| Gymnasium | 1.2.1 |
| TensorDict | 未安装；P2 前补齐 |
| R2-Dreamer | 未安装；P2 前补齐并冻结 commit |
| OnlineWM distribution | 未 editable-install；仓库脚本通过 `source/OnlineWM` bootstrap 导入 |

完整环境快照：

- `artifacts/r2dreamer/p0_runtime/configs/conda-history.yml`
- `artifacts/r2dreamer/p0_runtime/configs/requirements.lock.txt`
- `artifacts/r2dreamer/p0_runtime/metrics/runtime_probe.json`

## 4. 代码版本与 dirty 状态

| 仓库 | 路径 | commit | 状态 |
|---|---|---|---|
| OnlineWM | `D:\Project\S2R\OnlineWM\OnlineWM` | `b15613ac8907188ad1b51c38170c190b51abfe25` | dirty；包含路线文档的既有修改和本次 P0 文件 |
| Isaac Lab | `D:\Software\Isaac Install\IsaacLab` | `f4aa17f87e2e5db5484f0b5974918573e8918ce2` | `v2.3.2-13-gf4aa17f87e2`；dirty，存在未跟踪文件 `2.7.0+cu128` |
| R2-Dreamer | 未安装 | 未冻结 | P2 入口阻塞项 |

Git 原始输出见 `logs/onlinewm_git.txt` 和 `logs/isaaclab_git.txt`。
P1 的运行必须继续使用上表 Isaac Lab commit；升级、清理或切换分支后必须重新执行 P0
采集器。

## 5. P1 冻结 smoke 命令

从空 PowerShell 终端执行：

```powershell
$env:PYTHONNOUSERSITE = "1"
Set-Location "D:\Project\S2R\OnlineWM\OnlineWM\artifacts\r2dreamer\p1_isaaclab_official"
conda run --name isaaclab python `
  "D:\Software\Isaac Install\IsaacLab\scripts\reinforcement_learning\rl_games\train.py" `
  --task Isaac-Cartpole-Direct-v0 `
  --num_envs 16 `
  --headless `
  --max_iterations 5
```

该工作目录使 RL-Games 输出落入
`artifacts/r2dreamer/p1_isaaclab_official/logs/rl_games/`。P1 首次运行前创建标准验收目录，
并把完整终端输出另存为带时间戳的文本日志。

## 6. 重建与复核

全新环境安装脚本：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File scripts\p0\install_runtime.ps1 -PlanOnly
```

移除 `-PlanOnly` 后，脚本会创建独立的 `onlinewm-p0` 环境，绕过用户级 Conda channel，
安装固定 Torch/Isaac Sim 版本、检出固定 Isaac Lab commit，并 editable-install OnlineWM。
脚本拒绝覆盖同名环境或已有 Isaac Lab 目录。

重新采集本机证据：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File scripts\p0\collect_runtime.ps1
```

## 7. 真机侧待确认项

本地参考资料包含 `aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877`，但这不等同于已经冻结
真机控制机的实际运行时。P8 前仍需在真机控制机确认并记录：

- 操作系统版本；
- 实际安装的 AUBO SDK build；
- RTDE/控制接口及依赖；
- 网络、反馈频率、时延和故障语义。
