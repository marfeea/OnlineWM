# OnlineWM Anaconda 环境配置

## 1. 推荐基线

本配置面向当前 Windows 项目和现有 AUBO 场景，采用一套固定兼容基线，而不是追逐各组件的最新版本。

| 组件 | 推荐版本 | 依据 |
|---|---:|---|
| Python | 3.11 | Isaac Sim 5.x 要求 Python 3.11；项目自身要求 Python >= 3.10 |
| NVIDIA 驱动 | 支持 CUDA 12.8 的版本 | PyTorch 使用 cu128；本机 591.86 已满足 |
| PyTorch | 2.7.0+cu128 | Isaac Lab 2.3.x / Isaac Sim 5.1 官方安装基线 |
| torchvision | 0.22.0+cu128 | 与 PyTorch 2.7.0 配套 |
| Isaac Sim | 5.1.0 | 项目扩展声明支持 5.1.0；本机二进制也是 5.1.0 |
| Isaac Lab | v2.3.2 | 本机源码的稳定标签，面向 Isaac Sim 5.1 |
| OnlineWM | editable install | 便于直接运行和调试当前仓库代码 |

当前仓库还没有 DreamerV3/R2-Dreamer 的算法主体，因此本阶段不加入 JAX、额外世界模型库或论文仓库依赖。待 S4-S6 开始实现后，再根据最终采用的 PyTorch 实现追加依赖，避免现在引入冲突。

## 2. 创建基础环境

在项目根目录打开 Anaconda PowerShell Prompt：

```powershell
conda env create -f environment.yml
conda activate onlinewm
```

`environment.yml` 会设置 `PYTHONNOUSERSITE=1`。请保留该设置：本机用户级 Python 目录中存在另一套 Torch 和 Isaac Lab，它们会覆盖虚拟环境内的正确版本，并可能触发 DLL 初始化失败。

如果创建时在清华镜像的 `repodata.json` 阶段出现连接重置，本机的用户级 `.condarc` 中存在过期/重复镜像。可绕过全局 channel 配置创建同等基础环境：

```powershell
conda create --name onlinewm --override-channels `
  --channel conda-forge --channel defaults `
  python=3.11 pip "importlib_metadata"
conda activate onlinewm
python -m pip install "setuptools<82.0.0" wheel toml==0.10.2 `
  "psutil>=5.9,<8" "pytest>=8,<10" ruff==0.14.10 "pre-commit>=4,<5"
conda env config vars set PYTHONNOUSERSITE=1
conda deactivate
conda activate onlinewm
```

可用以下命令检查设置是否生效：

```powershell
python -c "import site; print(site.ENABLE_USER_SITE)"
```

期望输出为 `False`。

## 3. 安装仿真与训练依赖

安装顺序很重要。以下命令均在已激活的 `onlinewm` 环境中执行。

### 3.1 安装 CUDA 版 PyTorch

```powershell
python -m pip install torch==2.7.0 torchvision==0.22.0 `
  --index-url https://download.pytorch.org/whl/cu128
```

不要安装 Conda 的 `cuda-toolkit`，也不要按 `nvidia-smi` 显示的最高 CUDA 版本选择 PyTorch。此项目使用 PyTorch wheel 自带的 CUDA 12.8 运行时；只需主机驱动向下兼容。

### 3.2 安装 Isaac Sim 5.1

推荐使用与 Conda 环境直接集成的 pip 包：

```powershell
python -m pip install "isaacsim[all,extscache]==5.1.0" `
  --extra-index-url https://pypi.nvidia.com
```

如果希望复用本机 `C:\isaac-sim` 二进制安装，也可以不执行本步骤，但随后应始终通过该安装自带的 Python/环境脚本启动。对于本项目，pip 安装更容易保证 `python` 始终指向同一个 Conda 解释器。

### 3.3 安装 Isaac Lab

建议把 Isaac Lab 放在项目仓库之外，并锁定标签：

```powershell
git clone --branch v2.3.2 --depth 1 https://github.com/isaac-sim/IsaacLab.git D:\Software\IsaacLab-2.3.2
& "D:\Software\IsaacLab-2.3.2\isaaclab.bat" -i
```

`-i` 会以 editable 模式安装 Isaac Lab 的核心扩展、任务包以及项目脚本涉及的 RL 后端（RSL-RL、RL-Games、SKRL、Stable-Baselines3）。若只运行当前 TCP-docking 场景 smoke test，可先安装核心扩展；但完整 `-i` 与仓库中现有训练脚本最一致。

本机已有源码目录 `D:\Software\Isaac Install\IsaacLab`。如果确认继续使用它，应先把该仓库固定到计划采用的 commit/tag，再执行其中的 `isaaclab.bat -i`；不要在可复现实验中长期跟随 `main`。

### 3.4 安装 OnlineWM

回到本项目根目录执行：

```powershell
python -m pip install -e source\OnlineWM
```

外部 USD 资产默认可从当前目录结构自动发现。若资产放在其他位置，再显式设置：

```powershell
conda env config vars set ONLINEWM_ASSET_ROOT="D:/Project/S2R/Asset"
conda deactivate
conda activate onlinewm
```

## 4. 验证环境

先验证解释器、版本、CUDA 和包来源：

```powershell
python -c "import sys, site, torch, isaaclab, OnlineWM; print(sys.executable); print('user-site:', site.ENABLE_USER_SITE); print('torch:', torch.__version__); print('torch-cuda:', torch.version.cuda); print('cuda:', torch.cuda.is_available()); print('isaaclab:', isaaclab.__file__); print('OnlineWM:', OnlineWM.__file__)"
```

应满足：

- 解释器路径位于 `...\envs\onlinewm\python.exe`；
- `user-site` 为 `False`；
- Torch 为 `2.7.0+cu128`，`torch.version.cuda` 为 `12.8`；
- `cuda` 为 `True`；
- `isaaclab` 来自锁定的 Isaac Lab 源码目录；
- `OnlineWM` 来自当前项目的 `source\OnlineWM`。

然后依次运行：

```powershell
python -m pytest tests\test_scene_config.py -q
python -m compileall -q source\OnlineWM\OnlineWM scripts tests
python -m ruff check source scripts tests
python scripts\smoke_scene.py --headless
```

前三项用于检查纯 Python 配置，最后一项才会启动 Isaac Sim 并验证静态场景。首次启动可能需要较长时间生成缓存。

本配置制定时已在本机等价基线上验证：纯配置测试为 `3 passed`，headless 静态场景成功识别两台 AUBO 的关节、夹爪、flange 和 Jacobian，并输出 `Migrated static scene smoke test passed.`。

## 5. 不建议的组合

- Python 3.10 + Isaac Sim 5.1：Python ABI 不匹配。
- Python 3.12：当前 Isaac Sim 5.1 不支持。
- 最新版 Torch/CUDA 随意组合：可能与 Isaac Sim 扩展的二进制依赖冲突。
- 同时混用 `C:\isaac-sim\kit\python.bat` 和 Conda 环境的 `python`：容易让包安装位置和运行解释器分离。
- 允许用户级 `site-packages`：本机已观察到它会覆盖环境中的 Torch/Isaac Lab。

## 6. 导出最终锁定结果

完成安装并通过 smoke test 后，保存两份清单：

```powershell
conda env export --from-history > environment.history.yml
python -m pip freeze > requirements.lock.txt
```

同时记录 Isaac Lab commit：

```powershell
git -C "D:\Software\IsaacLab-2.3.2" rev-parse HEAD
```

正式 DreamerV3/R2-Dreamer 实验还应额外记录 GPU、驱动、种子、配置文件、checkpoint 和训练日志。
