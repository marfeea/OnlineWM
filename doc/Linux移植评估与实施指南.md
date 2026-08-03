# OnlineWM Linux 移植：智能体执行手册

> 文档版本：2026-08-03
> Windows 金标准仓库基线：`45079b0`（`master`）
> 默认目标：Ubuntu 22.04 LTS x86_64、Python 3.11、Isaac Sim 5.1.0.0、Isaac Lab `f4aa17f...`、R2-Dreamer `546e4f...`
> 默认执行档：`smoke`，完成环境重建、跨平台改造、P0/P1 和仓库内已存在的 P2/P3 缩减验收；不自动执行完整预算训练和真机控制。

## 0. 给智能体的强制执行指令

当智能体收到“执行本文件”的请求时，必须把本文视为 Runbook，而不是参考资料：

1. 完整阅读本文，然后开始执行；不要只输出摘要、建议或计划。
2. 第一条命令必须确认仓库根目录和 `git status --short`。
3. 保留所有既有修改和未跟踪文件；不得执行 `git reset --hard`、`git clean`、覆盖已有环境或删除已有外部仓库。
4. 默认使用 `smoke` 档。只有用户明确指定时，才进入 `acceptance`、`full-budget` 或 `robot` 档。
5. 所有命令、退出码、版本、失败原因和验收结果必须写入本次独立运行目录。
6. 每完成一个 Gate 立即更新状态文件；允许从状态文件断点续跑，不重复已经 PASS 的破坏性或耗时步骤。
7. 遇到系统级安装、NVIDIA 驱动、EULA、sudo、外部凭据、真机动作或大于 20 GB 的下载时，先向用户请求授权。
8. 遇到缺失资产时，继续完成不依赖资产的 Gate，并把资产 Gate 标为 `BLOCKED`，不要伪造资产或修改测试跳过检查。
9. 遇到版本冲突时优先保持本文冻结版本，不得擅自升级 Python、Torch、Isaac Sim/Lab 或 R2-Dreamer。
10. 结束时必须给出：完成的 Gate、未完成项、改动文件、运行证据路径、当前 `git status` 和下一条精确命令。

可直接交给 Codex 的启动提示词：

```text
读取并严格执行 doc/Linux移植评估与实施指南.md。使用默认 smoke 档，不要只总结。
保留工作区现有修改；按 Gate 产生证据并持续执行，只有在本文规定的授权点或真实阻塞点暂停。
```

推荐从新克隆的仓库根目录启动：

```bash
git clone <ONLINEWM_GIT_URL> OnlineWM
cd OnlineWM
codex
```

这份文件只有被提交并推送到 Git 远端后，新的 clone 才能读取它。

## 1. 执行档与完成定义

| 档位 | 自动执行范围 | 不包含 |
|---|---|---|
| `smoke`（默认） | 主机预检、冻结环境、跨平台入口、纯测试、P0 导入、P1 5 iteration、P2/P3 reduced smoke | 长训练、AUBO 真机、驱动自动安装 |
| `acceptance` | `smoke` 全部 + P1 三次正式回归、P2 10K、P3 20K（若代码存在） | 510K/完整预算、真机 |
| `full-budget` | `acceptance` 全部 + 当前验收索引要求的完整训练预算 | 真机；执行前必须确认时间、显存和存储预算 |
| `robot` | 仿真 Gate 全部 + Linux AUBO SDK、只读状态、低速空载和现场验收 | 未经人工监护的运动；必须单独授权 |

默认完成定义：

- Linux 专用解释器可重复创建；
- OnlineWM、Isaac Lab、R2-Dreamer 版本和来源可审计；
- Windows 专属路径不再阻塞 Linux 主入口；
- P0/P1 通过；仓库中存在 P2/P3 时，其 reduced smoke 通过；
- 外部 USD 缺失时，除资产相关 Gate 外的结果仍完整保存；
- Linux 证据与 Windows 证据分目录保存，不覆盖原验收材料。

## 2. 固定版本与不可擅改项

第一轮只做操作系统等价迁移，不同时做框架大版本升级。

| 组件 | 冻结值 |
|---|---|
| OS | Ubuntu 22.04 LTS x86_64 |
| Python | 3.11 |
| PyTorch | 2.7.0+cu128 |
| torchvision | 0.22.0+cu128 |
| Isaac Sim | 5.1.0.0 |
| Isaac Lab | commit `f4aa17f87e2e5db5484f0b5974918573e8918ce2` |
| RL-Games | 1.6.5 |
| TensorDict | 0.8.3 |
| TorchRL | 0.8.1 |
| R2-Dreamer | `https://github.com/NM512/r2dreamer.git`，commit `546e4fab8146ea4b14e1d7726bbc1a8a1d50322f` |
| OnlineWM | 当前 clone 的 commit；`source/OnlineWM` editable install |
| packaging | 23.0 |
| Ruff | 0.14.10 |

R2-Dreamer 上游元数据要求 Torch 2.8.0、TorchRL 0.9.2 和 TensorDict 0.9.1，但当前 Windows 金标准为了兼容 Isaac Sim 5.1，使用 Torch 2.7.0、TorchRL 0.8.1 和 TensorDict 0.8.3，并对 R2-Dreamer/OnlineWM 执行 `--no-deps` editable install。这是有意的兼容覆盖，不得由智能体“修复”为上游版本。

当前 `pip check` 也因此可能非零。验收方式是：保存完整输出，只允许已知冲突清单，并以 P0 导入、TorchRL 原生扩展操作、P2/P3 训练 Gate 为最终运行证据。出现新的、清单外的冲突才算失败。

Windows 金标准中已知的 `pip check` allowlist 如下；Linux 结果可以少于此列表，不得无条件扩大：

- `r2dreamer 0.1.0` 对 Torch 2.8.0、TorchRL 0.9.2、TensorDict 0.9.1 的声明与冻结兼容覆盖不一致；
- `r2dreamer 0.1.0` 对 einops、gymnasium、moviepy、setuptools 的精确声明可能与 Isaac 环境中已验证版本不一致；
- `wheel` 要求较新 `packaging`，而 Isaac Sim 5.1 冻结 `packaging==23.0`；
- `isaacsim-core` 可能报告缺少当前工作流未使用的 `torchaudio`；
- Windows 基线曾出现 FastAPI/Starlette 版本声明冲突。

智能体必须逐条保存实际输出并解释为何不影响已执行 Gate；不能使用“在 allowlist 中”替代运行验证。

## 3. 目录、状态和证据协议

### 3.1 默认目录

智能体必须从 Git 推导路径，不得写死用户名：

```bash
set -euo pipefail

export ONLINEWM_ROOT="$(git rev-parse --show-toplevel)"
export MIGRATION_WORKSPACE="$(dirname "$ONLINEWM_ROOT")"
export ISAACLAB_ROOT="${ISAACLAB_ROOT:-$MIGRATION_WORKSPACE/IsaacLab-OnlineWM}"
export R2_DREAMER_ROOT="${R2_DREAMER_ROOT:-$MIGRATION_WORKSPACE/R2-Dreamer}"
export ONLINEWM_ASSET_ROOT="${ONLINEWM_ASSET_ROOT:-$MIGRATION_WORKSPACE/Asset}"
export CONDA_ENV="${CONDA_ENV:-onlinewm-linux}"
export MIGRATION_PROFILE="${MIGRATION_PROFILE:-smoke}"
export PYTHONNOUSERSITE=1
export PYTHONDONTWRITEBYTECODE=1
export HYDRA_FULL_ERROR=1

case "$MIGRATION_PROFILE" in
  smoke|acceptance|full-budget|robot) ;;
  *) echo "Unsupported MIGRATION_PROFILE: $MIGRATION_PROFILE" >&2; exit 2 ;;
esac
```

每个新的 Bash 会话都必须先启用 `set -euo pipefail`。本文大量使用 `tee` 保存日志；没有 `pipefail` 时，前面的训练命令失败而 `tee` 成功会产生错误的 PASS。

若默认外部目录已经存在：

- 若来源、commit 和状态符合本文，复用并记录；
- 若目录 dirty 或来源不匹配，不 checkout、不清理，改用带时间戳的新目录；
- 不得把外部 clone 放进 OnlineWM 仓库内部；
- 不得复制 Windows 的 Conda 环境、`site-packages`、`.runtime` 或缓存。

### 3.2 本次运行目录

每次运行创建唯一目录：

```bash
export RUN_ID="$(date -u +%Y%m%dT%H%M%SZ)_$(git rev-parse --short HEAD)"
export RUN_ROOT="$ONLINEWM_ROOT/artifacts/linux_migration/$RUN_ID"
mkdir -p "$RUN_ROOT"/{logs,metrics,configs,tests,checkpoints,p2,p3}

{
  printf 'export ONLINEWM_ROOT=%q\n' "$ONLINEWM_ROOT"
  printf 'export MIGRATION_WORKSPACE=%q\n' "$MIGRATION_WORKSPACE"
  printf 'export ISAACLAB_ROOT=%q\n' "$ISAACLAB_ROOT"
  printf 'export R2_DREAMER_ROOT=%q\n' "$R2_DREAMER_ROOT"
  printf 'export ONLINEWM_ASSET_ROOT=%q\n' "$ONLINEWM_ASSET_ROOT"
  printf 'export CONDA_ENV=%q\n' "$CONDA_ENV"
  printf 'export MIGRATION_PROFILE=%q\n' "$MIGRATION_PROFILE"
  printf 'export RUN_ID=%q\n' "$RUN_ID"
  printf 'export RUN_ROOT=%q\n' "$RUN_ROOT"
  printf 'export PYTHONNOUSERSITE=1\n'
  printf 'export PYTHONDONTWRITEBYTECODE=1\n'
  printf 'export HYDRA_FULL_ERROR=1\n'
} > "$RUN_ROOT/run.env"

python3 - "$RUN_ROOT/migration_state.json" <<'PY'
import datetime
import json
import os
import pathlib
import subprocess
import sys

output = pathlib.Path(sys.argv[1])
if output.exists():
    raise SystemExit(f"Refusing to overwrite state: {output}")
commit = subprocess.run(
    ["git", "-C", os.environ["ONLINEWM_ROOT"], "rev-parse", "HEAD"],
    check=True,
    capture_output=True,
    text=True,
).stdout.strip()
gate_names = ("HOST", "ENV", "PORT", "L0", "L1", "L2", "L3", "L4_P2", "L4_P3")
state = {
    "schema_version": 1,
    "run_id": os.environ["RUN_ID"],
    "profile": os.environ["MIGRATION_PROFILE"],
    "onlinewm_commit": commit,
    "started_at": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "gates": {name: {"status": "PENDING", "evidence": []} for name in gate_names},
}
output.write_text(json.dumps(state, indent=2) + "\n", encoding="utf-8")
PY
```

禁止复用已存在的 `RUN_ROOT`。至少生成：

```text
artifacts/linux_migration/<RUN_ID>/
├── migration_state.json
├── run.env
├── git_before.txt
├── git_after.txt
├── logs/
├── metrics/
├── configs/
├── tests/
├── checkpoints/
├── p2/
└── p3/
```

`migration_state.json` 使用以下结构并在每个 Gate 后更新：

```json
{
  "schema_version": 1,
  "run_id": "<RUN_ID>",
  "profile": "smoke",
  "onlinewm_commit": "<SHA>",
  "started_at": "<ISO-8601>",
  "gates": {
    "HOST": {"status": "PENDING", "evidence": []},
    "ENV": {"status": "PENDING", "evidence": []},
    "PORT": {"status": "PENDING", "evidence": []},
    "L0": {"status": "PENDING", "evidence": []},
    "L1": {"status": "PENDING", "evidence": []},
    "L2": {"status": "PENDING", "evidence": []},
    "L3": {"status": "PENDING", "evidence": []},
    "L4_P2": {"status": "PENDING", "evidence": []},
    "L4_P3": {"status": "PENDING", "evidence": []}
  }
}
```

状态只允许：`PENDING`、`RUNNING`、`PASS`、`FAIL`、`BLOCKED`、`SKIPPED_NOT_PRESENT`。每条命令应保存命令文本、stdout/stderr 和退出码；不得只记录智能体的自然语言结论。

## 4. Gate HOST：主机预检

### 4.1 只读检查

先执行并保存输出：

```bash
cd "$ONLINEWM_ROOT"
git status --short | tee "$RUN_ROOT/git_before.txt"
git rev-parse HEAD | tee "$RUN_ROOT/configs/onlinewm_commit.txt"
uname -a | tee "$RUN_ROOT/logs/uname.txt"
cat /etc/os-release | tee "$RUN_ROOT/logs/os-release.txt"
uname -m | tee "$RUN_ROOT/logs/architecture.txt"
ldd --version 2>&1 | head -n 1 | tee "$RUN_ROOT/logs/glibc.txt"
free -h | tee "$RUN_ROOT/logs/memory.txt"
df -h "$ONLINEWM_ROOT" | tee "$RUN_ROOT/logs/disk.txt"
nvidia-smi | tee "$RUN_ROOT/logs/nvidia-smi.txt"
git lfs version | tee "$RUN_ROOT/logs/git-lfs.txt"
```

通过条件：

- 原生 Linux x86_64；默认拒绝把 WSL 当作正式 Isaac 验收主机；
- Ubuntu 22.04 为首选；24.04 只有用户确认后才继续本冻结组合；
- NVIDIA GPU 和驱动可见；Isaac Sim 5.1 Linux 验证基线为 580.65.06，使用该版本或更新的 production branch；
- RAM 至少 32 GB；可用磁盘至少 100 GB；
- 目标训练若需要正式视觉负载，VRAM 至少 16 GB。

当前 Windows 基线 RTX 4060 约 8 GB，低于 Isaac Sim 5.1 官方 16 GB 最低配置。8 GB 可以继续 `smoke`，但必须记录容量警告；不得承诺完整三相机双臂训练。

若 `nvidia-smi` 不可用或驱动版本不满足要求：将 HOST 标为 `BLOCKED`，向用户报告精确型号/错误并请求驱动安装授权。智能体不得自行替换内核或 NVIDIA 驱动。

### 4.2 系统依赖

仅在检查缺失且用户批准后安装：

```bash
sudo apt-get update
sudo apt-get install -y \
  git git-lfs curl wget unzip ripgrep ffmpeg build-essential cmake \
  libgl1 libegl1 libx11-6 libxext6 libxrandr2 \
  libxinerama1 libxcursor1 libxi6 libglib2.0-0 libvulkan1
git lfs install
git lfs ls-files --size
```

根据 `git lfs ls-files --size` 估算下载量；可能超过 20 GB 时先请求授权，再执行 `git lfs pull`。

不得为了“方便”执行发行版升级或安装桌面环境。

## 5. Gate ENV：建立冻结运行时

### 5.1 Conda/Miniforge

优先复用已有 Conda/Miniforge。若不存在，向用户请求下载授权后，将 Miniforge 安装到用户目录；不要用 sudo 安装 Python 环境。

用户批准后，x86_64 主机可执行：

```bash
test "$(uname -m)" = "x86_64"
export MINIFORGE_INSTALLER="Miniforge3-Linux-x86_64.sh"
export MINIFORGE_TMP="$(mktemp -d)"
curl -fL \
  "https://github.com/conda-forge/miniforge/releases/latest/download/$MINIFORGE_INSTALLER" \
  -o "$MINIFORGE_TMP/$MINIFORGE_INSTALLER"
curl -fL \
  "https://github.com/conda-forge/miniforge/releases/latest/download/$MINIFORGE_INSTALLER.sha256" \
  -o "$MINIFORGE_TMP/$MINIFORGE_INSTALLER.sha256"
(
  cd "$MINIFORGE_TMP"
  sha256sum --check "$MINIFORGE_INSTALLER.sha256"
)
bash "$MINIFORGE_TMP/$MINIFORGE_INSTALLER" -b -p "$HOME/miniforge3"
source "$HOME/miniforge3/etc/profile.d/conda.sh"
conda --version
```

校验和失败必须停止；不得绕过校验或改用非官方镜像。

若 `CONDA_ENV` 已存在：

1. 读取 Python、Torch、Isaac、环境路径和既有状态文件；
2. 只有证据表明它由同一迁移运行创建且版本符合本文时才续用；
3. 否则使用新名称 `onlinewm-linux-<短SHA或时间戳>`；
4. 不删除、不原地修复未知环境。

创建环境：

```bash
conda create --yes --name "$CONDA_ENV" \
  --override-channels --channel conda-forge --channel defaults \
  python=3.11 pip importlib_metadata

conda env config vars set --name "$CONDA_ENV" \
  PYTHONNOUSERSITE=1 PYTHONDONTWRITEBYTECODE=1 HYDRA_FULL_ERROR=1

conda run --name "$CONDA_ENV" python -m pip install \
  "setuptools<82.0.0" wheel toml==0.10.2 \
  "psutil>=5.9,<8" "pytest>=8,<10" ruff==0.14.10 "pre-commit>=4,<5"
```

### 5.2 Torch 与 Isaac Sim

这些命令会产生大下载；执行前请求网络/下载授权：

```bash
conda run --name "$CONDA_ENV" python -m pip install \
  torch==2.7.0 torchvision==0.22.0 \
  --index-url https://download.pytorch.org/whl/cu128

conda run --name "$CONDA_ENV" python -m pip install \
  "isaacsim[all,extscache]==5.1.0.0" \
  --extra-index-url https://pypi.nvidia.com
```

如果 Isaac Sim 要求接受 EULA，暂停并让用户明确接受；不得替用户自动同意法律条款。

立即验证 Torch 未被解析器改写：

```bash
conda run --name "$CONDA_ENV" python -c \
  "import site,sys,torch; print(sys.executable); print(site.ENABLE_USER_SITE); print(torch.__version__); print(torch.version.cuda); print(torch.cuda.is_available())" \
  | tee "$RUN_ROOT/logs/python_torch_after_isaac.txt"
```

必须满足：Python 3.11、`site.ENABLE_USER_SITE == False`、Torch 为 `2.7.0+cu128`、CUDA 可用。

### 5.3 Isaac Lab

```bash
if [ -d "$ISAACLAB_ROOT/.git" ]; then
  test -z "$(git -C "$ISAACLAB_ROOT" status --porcelain)" || {
    echo "Existing Isaac Lab checkout is dirty; choose a new ISAACLAB_ROOT." >&2
    exit 3
  }
  case "$(git -C "$ISAACLAB_ROOT" remote get-url origin)" in
    https://github.com/isaac-sim/IsaacLab|https://github.com/isaac-sim/IsaacLab.git|git@github.com:isaac-sim/IsaacLab.git) ;;
    *) echo "Existing Isaac Lab origin mismatch; choose a new ISAACLAB_ROOT." >&2; exit 3 ;;
  esac
elif [ -e "$ISAACLAB_ROOT" ]; then
  echo "ISAACLAB_ROOT exists but is not a Git checkout; choose a new path." >&2
  exit 3
else
  git clone https://github.com/isaac-sim/IsaacLab.git "$ISAACLAB_ROOT"
fi

git -C "$ISAACLAB_ROOT" status --short
git -C "$ISAACLAB_ROOT" remote -v
git -C "$ISAACLAB_ROOT" checkout --detach f4aa17f87e2e5db5484f0b5974918573e8918ce2

conda run --name "$CONDA_ENV" \
  bash "$ISAACLAB_ROOT/isaaclab.sh" -i rl_games
```

如果已有 Isaac Lab 目录 dirty 或 remote/commit 不匹配，不执行 checkout；选择新的 sibling 目录后再 clone。

### 5.4 R2-Dreamer 与 OnlineWM 兼容覆盖

```bash
if [ -d "$R2_DREAMER_ROOT/.git" ]; then
  test -z "$(git -C "$R2_DREAMER_ROOT" status --porcelain)" || {
    echo "Existing R2-Dreamer checkout is dirty; choose a new R2_DREAMER_ROOT." >&2
    exit 3
  }
  case "$(git -C "$R2_DREAMER_ROOT" remote get-url origin)" in
    https://github.com/NM512/r2dreamer|https://github.com/NM512/r2dreamer.git|git@github.com:NM512/r2dreamer.git) ;;
    *) echo "Existing R2-Dreamer origin mismatch; choose a new R2_DREAMER_ROOT." >&2; exit 3 ;;
  esac
elif [ -e "$R2_DREAMER_ROOT" ]; then
  echo "R2_DREAMER_ROOT exists but is not a Git checkout; choose a new path." >&2
  exit 3
else
  git clone https://github.com/NM512/r2dreamer.git "$R2_DREAMER_ROOT"
fi

git -C "$R2_DREAMER_ROOT" status --short
git -C "$R2_DREAMER_ROOT" remote -v
git -C "$R2_DREAMER_ROOT" checkout --detach 546e4fab8146ea4b14e1d7726bbc1a8a1d50322f

conda run --name "$CONDA_ENV" python -m pip install \
  tensordict==0.8.3 torchrl==0.8.1 ruamel.yaml==0.17.4 \
  einops==0.8.2 gymnasium==1.2.1 hydra-core==1.3.2 \
  moviepy==2.2.1 numpy==1.26.0 tensorboard==2.20.0 \
  imageio==2.37.0 imageio-ffmpeg==0.6.0

conda run --name "$CONDA_ENV" python -m pip uninstall --yes pyvers
conda run --name "$CONDA_ENV" python -m pip install packaging==23.0

conda run --name "$CONDA_ENV" python -m pip install \
  --no-deps --no-build-isolation -e "$R2_DREAMER_ROOT"

conda run --name "$CONDA_ENV" python -m pip install \
  --no-deps --no-build-isolation -e "$ONLINEWM_ROOT/source/OnlineWM"
```

如果已有 R2-Dreamer 目录 dirty，不 checkout、不覆盖，改用新的 sibling clone。

保存环境证据：

```bash
conda run --name "$CONDA_ENV" python -m pip freeze \
  > "$RUN_ROOT/configs/requirements-linux.txt"

test "$(git -C "$ISAACLAB_ROOT" rev-parse HEAD)" = \
  "f4aa17f87e2e5db5484f0b5974918573e8918ce2"
test "$(git -C "$R2_DREAMER_ROOT" rev-parse HEAD)" = \
  "546e4fab8146ea4b14e1d7726bbc1a8a1d50322f"

cat > "$RUN_ROOT/configs/check_frozen_versions.py" <<'PY'
import importlib.metadata as metadata

expected = {
    "torch": "2.7.0+cu128",
    "torchvision": "0.22.0+cu128",
    "isaacsim": "5.1.0.0",
    "rl-games": "1.6.5",
    "tensordict": "0.8.3",
    "torchrl": "0.8.1",
    "packaging": "23.0",
    "gymnasium": "1.2.1",
    "hydra-core": "1.3.2",
    "numpy": "1.26.0",
    "ruamel.yaml": "0.17.4",
    "moviepy": "2.2.1",
    "r2dreamer": "0.1.0",
    "OnlineWM": "0.1.0",
}
actual = {name: metadata.version(name) for name in expected}
for name in expected:
    print(f"{name}={actual[name]}")
mismatches = {
    name: {"expected": expected[name], "actual": actual[name]}
    for name in expected
    if actual[name] != expected[name]
}
if mismatches:
    raise SystemExit(f"Frozen version mismatch: {mismatches}")
PY

conda run --name "$CONDA_ENV" python \
  "$RUN_ROOT/configs/check_frozen_versions.py" \
  | tee "$RUN_ROOT/logs/frozen-version-check.txt"

set +e
conda run --name "$CONDA_ENV" python -m pip check \
  > "$RUN_ROOT/logs/pip-check.txt" 2>&1
echo $? > "$RUN_ROOT/logs/pip-check.exitcode"
set -e
```

ENV 只有在两个 Git commit 检查和 `frozen-version-check.txt` 对应命令均返回 0 时才能标记 `PASS`。`pip check` 非零时还必须确认每一项都属于第 2 节 allowlist，并在状态文件中记录解释。

不要把 Windows 的 `artifacts/r2dreamer/p0_runtime/configs/requirements.lock.txt` 当作 Linux 安装输入；其中包含 `pywin32`、Windows editable 路径和平台专属 wheel。

## 6. Gate PORT：智能体必须完成的跨平台改造

环境创建后，智能体应修改项目，使后续 Linux 执行不依赖 PowerShell。只修改与移植有关的文件，不碰用户的其他 P3/研究改动。

### 6.1 必需改造

1. 将 `scripts/p0/g06_probe.py` 中写死的 Windows Python 路径改为参数 `--expected-python`；未传时只检查解释器属于当前专用环境，不与某台机器绝对路径比较。
2. 动态发现 `scripts/p*/collect*.ps1`、`scripts/p*/watch*.ps1` 和安装脚本；为每个仍在使用的阶段提供跨平台 Python 主入口。
3. 复用已有 Python 训练/验收模块，避免把 PowerShell 逻辑逐行翻译成 Bash。
4. 保留原 `.ps1` 供 Windows 回归，不删除历史入口。
5. 新入口所有路径使用 `pathlib.Path`，命令使用参数数组和 `subprocess.run(..., check=True)`，不得使用 `shell=True`。
6. 外部解释器、Isaac Lab、R2-Dreamer、Asset 和 artifact root 必须由参数或环境变量给出。
7. 日志统一 UTF-8；仅在读取旧 PowerShell 证据时兼容 UTF-16。
8. 新入口拒绝覆盖既有 `run_id`，并保留当前“证据不覆盖”约束。
9. `.gitattributes` 增加 `*.sh text eol=lf`；若新增 `.sh`，提交可执行位。
10. `.vscode/tasks.json` 保留 Windows/Linux 两套命令，不共享 Windows 默认绝对路径。

### 6.2 推荐稳定接口

新建 `scripts/platform/`，最少提供：

```text
scripts/platform/
├── collect_runtime.py
├── collect_official.py
├── collect_smoke.py
└── watch_run.py
```

建议命令契约：

```bash
conda run --name "$CONDA_ENV" python scripts/platform/collect_runtime.py \
  --output-root "$RUN_ROOT" \
  --isaaclab-root "$ISAACLAB_ROOT" \
  --r2-dreamer-root "$R2_DREAMER_ROOT"

conda run --name "$CONDA_ENV" python scripts/platform/collect_official.py \
  --output-root "$RUN_ROOT/p1" \
  --isaaclab-root "$ISAACLAB_ROOT" \
  --profile smoke

conda run --name "$CONDA_ENV" python scripts/platform/collect_smoke.py \
  --stage p2 \
  --output-root "$RUN_ROOT/p2" \
  --r2-dreamer-root "$R2_DREAMER_ROOT" \
  --profile smoke
```

如果 P3 或后续 `scripts/pN` 不存在，标记 `SKIPPED_NOT_PRESENT`；如果存在，智能体必须将它纳入动态阶段发现，不能把支持范围写死为 P0～P2。

### 6.3 PORT 通过条件

- 所有新增入口有单元测试；
- Linux 路径测试通过；
- Windows 现有测试不回归；
- `rg` 扫描显示 Windows 绝对路径只存在于历史文档、`.ps1` 默认参数或 Windows 专用分支；
- 新的主执行路径不调用 `.bat`、`cmd /c`、Windows DLL 或 PowerShell。

## 7. Gate L0：纯 Python、格式与路径

```bash
cd "$ONLINEWM_ROOT"

conda run --name "$CONDA_ENV" python -m pytest -q tests \
  --basetemp "$RUN_ROOT/tests/pytest-temp" \
  | tee "$RUN_ROOT/tests/pytest.txt"

conda run --name "$CONDA_ENV" python -m ruff check scripts source tests \
  | tee "$RUN_ROOT/tests/ruff-check.txt"

conda run --name "$CONDA_ENV" python -m ruff format --check scripts source tests \
  | tee "$RUN_ROOT/tests/ruff-format.txt"

if rg -n -i --glob '*.py' --glob '*.sh' \
  '(D:\\|C:\\|cmd /c|isaaclab\.bat|python\.bat|\.dll)' \
  scripts/platform source tests \
  > "$RUN_ROOT/tests/windows-coupling-scan.txt"
then
  cat "$RUN_ROOT/tests/windows-coupling-scan.txt"
  echo "Unexpected Windows coupling in the Linux main path." >&2
  exit 4
else
  scan_exit=$?
  test "$scan_exit" -eq 1 || exit "$scan_exit"
fi
```

通过条件：

- pytest、Ruff check、Ruff format 全部返回 0；
- `ONLINEWM_ASSET_ROOT` 使用 Linux 路径时能解析；
- 新平台入口不存在 Windows 盘符、反斜杠拼接或 Windows 可执行文件依赖；
- Linux 大小写敏感路径检查通过。

## 8. Gate L1/P0：运行时导入与操作探针

先执行通用探针：

```bash
conda run --name "$CONDA_ENV" python -c \
  "import site,sys,torch; import isaaclab,isaaclab_tasks,OnlineWM; from tensordict import TensorDict; from torchrl.data.replay_buffers import ReplayBuffer,LazyTensorStorage; print(sys.executable); print(site.ENABLE_USER_SITE); print(torch.__version__,torch.version.cuda,torch.cuda.get_device_name(0)); print(OnlineWM.__file__); print('IMPORT_OPERATION_PASS')" \
  | tee "$RUN_ROOT/logs/l1-import-operation.txt"
```

然后运行改造后的 G06 探针：

```bash
conda run --name "$CONDA_ENV" python scripts/p0/g06_probe.py \
  --r2-dreamer-root "$R2_DREAMER_ROOT" \
  --expected-r2-commit 546e4fab8146ea4b14e1d7726bbc1a8a1d50322f \
  --expected-python "$(conda run --name "$CONDA_ENV" python -c 'import sys; print(sys.executable)' | tail -n 1)" \
  --output "$RUN_ROOT/metrics/g06_probe.json" \
  --headless \
  | tee "$RUN_ROOT/logs/g06_probe.txt"
```

通过条件：

- `PYTHONNOUSERSITE=1` 生效；
- Torch 仍是 2.7.0+cu128，CUDA 可用；
- TensorDict/TorchRL 原生扩展和 ReplayBuffer 操作成功；
- R2-Dreamer 模块来自固定 checkout；
- OnlineWM 来自当前 clone 的 `source/OnlineWM`；
- OnlineWM 任务注册成功；
- `g06_probe.json` 的 `pass` 为 `true`。

## 9. Gate L2/P1：Isaac Lab 官方 smoke

默认 `smoke` 档运行：

```bash
mkdir -p "$RUN_ROOT/p1"
cd "$RUN_ROOT/p1"

conda run --name "$CONDA_ENV" python \
  "$ISAACLAB_ROOT/scripts/reinforcement_learning/rl_games/train.py" \
  --task Isaac-Cartpole-Direct-v0 \
  --num_envs 16 \
  --headless \
  --max_iterations 5 \
  agent.params.config.minibatch_size=512 \
  +agent.params.config.torch_compile=false \
  2>&1 | tee "$RUN_ROOT/logs/p1-smoke.txt"
```

通过条件：退出码 0；无 traceback、CUDA OOM 和非有限数；产生 RL-Games 运行目录和 checkpoint。

`acceptance` 档必须使用当前 P1 验收语义运行 150 iteration、连续三次、checkpoint resume 和固定策略视频；不得只把 5 iteration smoke 当作正式验收。

## 10. Gate L3：OnlineWM 外部资产与场景

检查资产：

```bash
for p in \
  "$ONLINEWM_ASSET_ROOT/AUBO_E5/AUBO_E5_Withclaw.usd" \
  "$ONLINEWM_ASSET_ROOT/QKL-HX-300-II-00/Part/WorkStation/WorkStation.usd" \
  "$ONLINEWM_ASSET_ROOT/QKL-HX-300-II-00/Part/Reagent_01/M_Reagent_01.usd"
do
  test -f "$p" || echo "MISSING: $p"
done | tee "$RUN_ROOT/logs/asset-check.txt"
```

三个必需 USD 都存在时运行：

```bash
cd "$ONLINEWM_ROOT"
conda run --name "$CONDA_ENV" python scripts/list_envs.py \
  | tee "$RUN_ROOT/logs/list-envs.txt"

conda run --name "$CONDA_ENV" python scripts/smoke_scene.py --headless \
  | tee "$RUN_ROOT/logs/onlinewm-scene-smoke.txt"
```

若缺失资产，将 L3 标为 `BLOCKED` 并列出精确文件；不得从互联网猜测或下载许可证不明的 USD。Linux 区分大小写，`AUBO_E5`、`WorkStation` 和 `Reagent_01` 必须与代码完全一致。

## 11. Gate L4_P2：R2-Dreamer 状态链 reduced smoke

仅在 `scripts/p2/train_state_chain.py` 存在时执行；否则标记 `SKIPPED_NOT_PRESENT`。

```bash
export P2_INITIAL="$RUN_ROOT/p2/initial"
export P2_RESUME="$RUN_ROOT/p2/resume"
mkdir -p "$P2_INITIAL" "$P2_RESUME" "$RUN_ROOT/checkpoints"

cd "$ONLINEWM_ROOT"
conda run --name "$CONDA_ENV" python scripts/p2/train_state_chain.py \
  --r2-dreamer-root "$R2_DREAMER_ROOT" \
  --diagnostics "$P2_INITIAL/diagnostics.json" \
  --checkpoint-out "$RUN_ROOT/checkpoints/p2-initial.pt" \
  -- \
  env=isaaclab_proprio model.rep_loss=r2dreamer \
  env.env_num=4 batch_size=2 batch_length=8 env.train_ratio=4 \
  trainer.update_log_every=64 model.compile=false model.log_grads=true \
  model.deter=128 model.hidden=64 model.discrete=8 model.units=64 \
  model.rssm.blocks=8 model.imag_horizon=5 \
  env.steps=320 logdir="$P2_INITIAL" \
  2>&1 | tee "$RUN_ROOT/logs/p2-initial.txt"

conda run --name "$CONDA_ENV" python scripts/p2/train_state_chain.py \
  --r2-dreamer-root "$R2_DREAMER_ROOT" \
  --resume "$RUN_ROOT/checkpoints/p2-initial.pt" \
  --diagnostics "$P2_RESUME/diagnostics.json" \
  --checkpoint-out "$RUN_ROOT/checkpoints/p2-resume.pt" \
  -- \
  env=isaaclab_proprio model.rep_loss=r2dreamer \
  env.env_num=4 batch_size=2 batch_length=8 env.train_ratio=4 \
  trainer.update_log_every=64 model.compile=false model.log_grads=true \
  model.deter=128 model.hidden=64 model.discrete=8 model.units=64 \
  model.rssm.blocks=8 model.imag_horizon=5 \
  env.steps=240 logdir="$P2_RESUME" \
  2>&1 | tee "$RUN_ROOT/logs/p2-resume.txt"

conda run --name "$CONDA_ENV" python scripts/p2/verify_acceptance.py \
  --initial-diagnostics "$P2_INITIAL/diagnostics.json" \
  --initial-metrics "$P2_INITIAL/metrics.jsonl" \
  --initial-console "$P2_INITIAL/console.log" \
  --resume-diagnostics "$P2_RESUME/diagnostics.json" \
  --resume-metrics "$P2_RESUME/metrics.jsonl" \
  --resume-console "$P2_RESUME/console.log" \
  --initial-checkpoint "$RUN_ROOT/checkpoints/p2-initial.pt" \
  --resume-checkpoint "$RUN_ROOT/checkpoints/p2-resume.pt" \
  --output "$RUN_ROOT/tests/p2-acceptance.json"
```

通过条件：验证器退出 0，训练更新/梯度/指标有限，checkpoint 存在，resume digest 匹配，episode replay 语义通过。

## 12. Gate L4_P3：R2-Dreamer 视觉链 reduced smoke

仅在 `scripts/p3/train_vision_chain.py` 和 `scripts/p3/verify_acceptance.py` 同时存在时执行；否则标记 `SKIPPED_NOT_PRESENT`。

```bash
export P3_INITIAL="$RUN_ROOT/p3/initial"
export P3_RESUME="$RUN_ROOT/p3/resume"
mkdir -p "$P3_INITIAL" "$P3_RESUME" \
  "$RUN_ROOT/p3/samples-initial" "$RUN_ROOT/p3/samples-resume"

cd "$ONLINEWM_ROOT"
conda run --name "$CONDA_ENV" python scripts/p3/train_vision_chain.py \
  --r2-dreamer-root "$R2_DREAMER_ROOT" \
  --diagnostics "$P3_INITIAL/diagnostics.json" \
  --checkpoint-out "$RUN_ROOT/checkpoints/p3-initial.pt" \
  --samples-out "$RUN_ROOT/p3/samples-initial" \
  --policy-video "$RUN_ROOT/p3/policy-initial.mp4" \
  --policy-steps 48 \
  -- \
  env=isaaclab_vision model.rep_loss=r2dreamer \
  env.env_num=2 batch_size=2 batch_length=4 env.train_ratio=2 \
  trainer.update_log_every=8 model.compile=false model.log_grads=true \
  model.deter=64 model.hidden=32 model.discrete=4 model.units=32 \
  model.depth=8 model.rssm.blocks=4 model.imag_horizon=3 \
  env.steps=96 logdir="$P3_INITIAL" \
  2>&1 | tee "$RUN_ROOT/logs/p3-initial.txt"

conda run --name "$CONDA_ENV" python scripts/p3/train_vision_chain.py \
  --r2-dreamer-root "$R2_DREAMER_ROOT" \
  --resume "$RUN_ROOT/checkpoints/p3-initial.pt" \
  --diagnostics "$P3_RESUME/diagnostics.json" \
  --checkpoint-out "$RUN_ROOT/checkpoints/p3-resume.pt" \
  --samples-out "$RUN_ROOT/p3/samples-resume" \
  --policy-video "$RUN_ROOT/p3/policy-resume.mp4" \
  --policy-steps 48 \
  -- \
  env=isaaclab_vision model.rep_loss=r2dreamer \
  env.env_num=2 batch_size=2 batch_length=4 env.train_ratio=2 \
  trainer.update_log_every=8 model.compile=false model.log_grads=true \
  model.deter=64 model.hidden=32 model.discrete=4 model.units=32 \
  model.depth=8 model.rssm.blocks=4 model.imag_horizon=3 \
  env.steps=80 logdir="$P3_RESUME" \
  2>&1 | tee "$RUN_ROOT/logs/p3-resume.txt"

conda run --name "$CONDA_ENV" python scripts/p3/verify_acceptance.py \
  --initial-diagnostics "$P3_INITIAL/diagnostics.json" \
  --initial-console "$P3_INITIAL/console.log" \
  --resume-diagnostics "$P3_RESUME/diagnostics.json" \
  --resume-console "$P3_RESUME/console.log" \
  --initial-checkpoint "$RUN_ROOT/checkpoints/p3-initial.pt" \
  --resume-checkpoint "$RUN_ROOT/checkpoints/p3-resume.pt" \
  --output "$RUN_ROOT/tests/p3-acceptance.json"
```

通过条件：P3 验证器退出 0；RGB batch 的形状、dtype、范围稳定；episode 语义、R2 loss、encoder/RSSM 梯度、checkpoint/resume 和固定策略视频全部通过。

## 13. acceptance 与 full-budget 扩展

只有用户选择相应档位才执行：

1. 读取 `artifacts/r2dreamer/acceptance_index.md` 获取仓库当前阶段和遗留项；
2. P1 运行三次正式训练、resume 和固定策略视频；
3. P2 运行当前官方 10K 状态链；
4. P3 存在时运行官方 20K 视觉链；
5. `full-budget` 执行前先估算 wall time、VRAM、磁盘和日志增长，并让用户确认；
6. 每个阶段沿用当前验证器，不得以“进程退出 0”替代 Gate JSON；
7. GPU 浮点轨迹不要求逐位一致，但完成步数、有限性、checkpoint/resume 和验收语义必须一致。

容量扩展顺序固定为：

```text
无渲染单环境
→ 16 环境 Cartpole
→ 双臂无相机
→ 单相机
→ 三相机 640×480
→ 增加并行环境数
→ 正式 batch/model 配置
```

发生 OOM 时记录当前上限并回退一级；`smoke` 档可以缩减负载，`acceptance` 档不得静默修改既定配置。

## 14. AUBO 真机：默认不执行

当前仓库内参考 SDK 是 `aubo_sdk-0.27.1-rc.1-Windows_AMD64+a798877`，只有 DLL/LIB/EXE，不能在 Linux 原生链接。

只有 `robot` 档并获得现场授权后才继续：

1. 从 AUBO 官方渠道获得匹配固件/API 的 Linux x86_64 SDK；
2. 核对版本、build、许可证和 C++ ABI；
3. 使用 `.so` 和头文件重新构建 C++/pybind 接口；
4. 使用 `ldd` 检查依赖，优先 RPATH，不长期依赖全局 `LD_LIBRARY_PATH`；
5. 检查控制器 IP、RTDE/控制端口、防火墙、网卡和 watchdog；
6. 先仿真，再只读状态，再低速空载，最后带负载；
7. 全程配置急停、软限位、速度限制和人工监护；
8. 智能体不得在无人确认时发送真实机械臂运动命令。

若 Linux 版 SDK 不可得，正式结论应为：Linux 承担开发/仿真/训练，Windows 控制机暂时承担真机接口；这不阻塞 `smoke` 或 `acceptance` 档。

## 15. 失败处理与断点续跑

| 失败 | 必须动作 | 禁止动作 |
|---|---|---|
| 工作区 dirty | 保存 `git_before.txt`，只改移植范围 | reset、clean、覆盖用户文件 |
| 外部 repo dirty | 使用新的 sibling clone | checkout/清理原目录 |
| NVIDIA 驱动缺失 | 标记 HOST `BLOCKED` 并请求授权 | 自动换内核/驱动 |
| EULA 未接受 | 暂停让用户确认 | 自动设置接受标志 |
| pip 解析升级 Torch | 恢复冻结版本并记录解析过程 | 接受 Torch 2.8 继续跑 Isaac 5.1 |
| `pip check` 已知冲突 | 保存并与 allowlist 对比 | 把非零直接当作唯一失败依据 |
| 新的 pip 冲突 | 标记 ENV `FAIL`，定位引入包 | 无证据地批量升级/降级 |
| USD 缺失 | L3 `BLOCKED`，继续其他 Gate | 下载来源不明资产、跳过测试 |
| CUDA OOM | 保存峰值，smoke 回退一级 | 修改 acceptance 配置后仍声称等价 |
| 测试失败 | 先复现最小失败，再做局部修复 | 跳过/删除测试 |
| 网络失败 | 记录 URL、错误和重试时间 | 无限重试或更换不可信镜像 |

断点续跑时：

1. 读取 `migration_state.json`；
2. 验证本机、commit、环境路径与原运行一致；
3. 只从第一个非 PASS Gate 继续；
4. 对下载中断可以安全重试；
5. 对安装/checkout 等变更先检查当前状态，保持幂等；
6. 不覆盖原日志，追加带时间戳的新尝试文件。

## 16. 最终验收与交付格式

默认 `smoke` 档满足以下条件才可声明完成：

- [ ] HOST 通过，或仅有已明确记录的 8 GB VRAM 容量警告；
- [ ] ENV 通过，唯一 Python 3.11 且 user site 禁用；
- [ ] Torch/Isaac/Isaac Lab/R2-Dreamer commit 与冻结值一致；
- [ ] PORT 通过，新主入口不依赖 Windows shell；
- [ ] L0 pytest、Ruff 和路径扫描通过；
- [ ] L1/P0 导入、TorchRL 操作和任务注册通过；
- [ ] L2/P1 官方 Cartpole smoke 通过；
- [ ] L3 通过，或因明确列出的外部 USD 缺失而 `BLOCKED`；
- [ ] P2 存在时 reduced smoke、checkpoint 和 resume 通过；
- [ ] P3 存在时 reduced vision smoke 和七 Gate 通过；
- [ ] 所有证据位于独立 `RUN_ROOT`；
- [ ] 没有覆盖 Windows 验收包和用户原有修改。

智能体最终回复必须使用以下格式：

```text
Linux 移植结果：PASS | PARTIAL | BLOCKED | FAIL
执行档：smoke | acceptance | full-budget | robot
OnlineWM commit：<SHA>
运行证据：<RUN_ROOT>

Gate：
- HOST: ...
- ENV: ...
- PORT: ...
- L0: ...
- L1/P0: ...
- L2/P1: ...
- L3: ...
- L4/P2: ...
- L4/P3: ...

改动文件：
- <path>: <reason>

未完成/风险：
- <exact blocker>

当前 git status：
<git status --short>

下一条命令：
<one exact command, or “none”>
```

最后执行：

```bash
cd "$ONLINEWM_ROOT"
git diff --check
git status --short | tee "$RUN_ROOT/git_after.txt"
```

## 17. 成本、适配性与官方依据

### 17.1 成本

| 范围 | 难度 | 预计单人工期 |
|---|---:|---:|
| 核心 Python、配置、静态测试 | 低 | 1～2 人日 |
| Isaac Sim/Lab + R2-Dreamer 仿真等价 | 中 | 4～7 人日 |
| 完整跨平台自动化与验收证据 | 中高 | 7～12 人日 |
| AUBO 真机 | 高 | 上述基础上增加 3～10 人日 |

最大不确定性是 8 GB 显存、Linux AUBO SDK、TorchRL 原生扩展组合，以及 Isaac/资产首次下载时间。

### 17.2 Codex Linux 环境

Linux 原生支持 Codex CLI。CLI 在本机仓库中读写文件并调用本机命令，但模型服务仍需网络和 ChatGPT 登录或 API 凭据。安装入口：

```bash
curl -fsSL https://chatgpt.com/codex/install.sh | sh
codex
```

官方参考：[Codex CLI](https://learn.chatgpt.com/docs/codex/cli) 和 [openai/codex](https://github.com/openai/codex)。

### 17.3 Isaac 官方依据

- [conda-forge Miniforge](https://github.com/conda-forge/miniforge)
- [Isaac Sim 5.1 System Requirements](https://docs.isaacsim.omniverse.nvidia.com/5.1.0/installation/requirements.html)
- [Isaac Sim 5.1 Installation](https://docs.isaacsim.omniverse.nvidia.com/5.1.0/installation/index.html)
- [Isaac Lab 2.3 Local Installation](https://isaac-sim.github.io/IsaacLab/v2.3.0/source/setup/installation/index.html)

Isaac Sim 5.1 支持 Ubuntu 22.04/24.04，但本项目 Isaac Lab 2.3.x/Python 3.11 冻结组合优先 Ubuntu 22.04。更新到 Isaac Sim 6.x/Python 3.12 必须另立升级任务，不属于本 Runbook 的第一次 Linux 等价迁移。
