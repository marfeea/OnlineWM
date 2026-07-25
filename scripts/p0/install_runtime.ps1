[CmdletBinding(SupportsShouldProcess)]
param(
    [string]$CondaEnvironment = "onlinewm-p0",
    [string]$IsaacLabRoot = "D:\Software\IsaacLab-OnlineWM",
    [string]$IsaacLabCommit = "f4aa17f87e2e5db5484f0b5974918573e8918ce2",
    [switch]$PlanOnly
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

foreach ($command in @("conda", "git")) {
    if (-not (Get-Command $command -ErrorAction SilentlyContinue)) {
        throw "$command is not available in this terminal."
    }
}
if (Test-Path -LiteralPath $IsaacLabRoot) {
    throw "Refusing to overwrite the existing Isaac Lab path: $IsaacLabRoot"
}
$existingEnvironments = (conda env list --json | ConvertFrom-Json).envs
if ($existingEnvironments | Where-Object { (Split-Path $_ -Leaf) -eq $CondaEnvironment }) {
    throw "Refusing to modify the existing Conda environment: $CondaEnvironment"
}

$steps = @(
    "conda create --yes --name $CondaEnvironment --override-channels --channel conda-forge --channel defaults python=3.11 pip importlib_metadata"
    "conda run --name $CondaEnvironment python -m pip install setuptools<82.0.0 wheel toml==0.10.2 psutil>=5.9,<8 pytest>=8,<10 ruff==0.14.10 pre-commit>=4,<5"
    "conda env config vars set --name $CondaEnvironment PYTHONNOUSERSITE=1"
    "conda run --name $CondaEnvironment python -m pip install torch==2.7.0 torchvision==0.22.0 --index-url https://download.pytorch.org/whl/cu128"
    "conda run --name $CondaEnvironment python -m pip install isaacsim[all,extscache]==5.1.0 --extra-index-url https://pypi.nvidia.com"
    "git clone https://github.com/isaac-sim/IsaacLab.git $IsaacLabRoot"
    "git -C $IsaacLabRoot checkout --detach $IsaacLabCommit"
    "conda run --name $CondaEnvironment cmd /c $IsaacLabRoot\isaaclab.bat -i rl_games"
    "conda run --name $CondaEnvironment python -m pip install -e $ProjectRoot\source\OnlineWM"
)

if ($PlanOnly) {
    $steps | ForEach-Object { Write-Host $_ }
    exit 0
}

if ($PSCmdlet.ShouldProcess($CondaEnvironment, "create the pinned OnlineWM P0 runtime")) {
    conda create --yes --name $CondaEnvironment --override-channels --channel conda-forge --channel defaults `
        python=3.11 pip importlib_metadata
    if ($LASTEXITCODE -ne 0) { throw "Conda environment creation failed." }
    conda run --name $CondaEnvironment python -m pip install "setuptools<82.0.0" wheel toml==0.10.2 `
        "psutil>=5.9,<8" "pytest>=8,<10" ruff==0.14.10 "pre-commit>=4,<5"
    if ($LASTEXITCODE -ne 0) { throw "Base dependency installation failed." }
    conda env config vars set --name $CondaEnvironment PYTHONNOUSERSITE=1
    if ($LASTEXITCODE -ne 0) { throw "Conda environment variable configuration failed." }

    conda run --name $CondaEnvironment python -m pip install torch==2.7.0 torchvision==0.22.0 `
        --index-url https://download.pytorch.org/whl/cu128
    if ($LASTEXITCODE -ne 0) { throw "PyTorch installation failed." }

    conda run --name $CondaEnvironment python -m pip install "isaacsim[all,extscache]==5.1.0" `
        --extra-index-url https://pypi.nvidia.com
    if ($LASTEXITCODE -ne 0) { throw "Isaac Sim installation failed." }

    git clone https://github.com/isaac-sim/IsaacLab.git $IsaacLabRoot
    if ($LASTEXITCODE -ne 0) { throw "Isaac Lab clone failed." }
    git -C $IsaacLabRoot checkout --detach $IsaacLabCommit
    if ($LASTEXITCODE -ne 0) { throw "Isaac Lab checkout failed." }

    conda run --name $CondaEnvironment cmd /c (Join-Path $IsaacLabRoot "isaaclab.bat") -i rl_games
    if ($LASTEXITCODE -ne 0) { throw "Isaac Lab installation failed." }
    conda run --name $CondaEnvironment python -m pip install -e (Join-Path $ProjectRoot "source\OnlineWM")
    if ($LASTEXITCODE -ne 0) { throw "OnlineWM installation failed." }
}
