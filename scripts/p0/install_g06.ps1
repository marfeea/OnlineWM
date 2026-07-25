[CmdletBinding()]
param(
    [string]$PythonExecutable = "D:\Anaconda\envs\isaaclab\python.exe",
    [string]$R2DreamerRoot = "D:\Software\R2-Dreamer",
    [string]$R2DreamerCommit = "546e4fab8146ea4b14e1d7726bbc1a8a1d50322f",
    [string]$ArtifactRoot = "",
    [string]$RunId = ""
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($ArtifactRoot)) {
    $ArtifactRoot = Join-Path $ProjectRoot "artifacts\r2dreamer\p0_runtime"
}
if ([string]::IsNullOrWhiteSpace($RunId)) {
    $RunId = Get-Date -Format "yyyyMMdd_HHmmss"
}
$ArtifactRoot = [System.IO.Path]::GetFullPath($ArtifactRoot)
$OnlineWMRoot = Join-Path $ProjectRoot "source\OnlineWM"
$ConfigRoot = Join-Path $ArtifactRoot "configs"
$LogRoot = Join-Path $ArtifactRoot "logs"
$MetricsRoot = Join-Path $ArtifactRoot "metrics"
$RunRecord = Join-Path $ConfigRoot "g06_install_$RunId.json"

foreach ($path in @($PythonExecutable, $R2DreamerRoot, $OnlineWMRoot)) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required path does not exist: $path"
    }
}
if (Test-Path -LiteralPath $RunRecord) {
    throw "Run ID '$RunId' already exists. Formal evidence is never overwritten."
}
foreach ($directory in @($ConfigRoot, $LogRoot, $MetricsRoot)) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

$safeR2DreamerRoot = $R2DreamerRoot.Replace("\", "/")
$r2DreamerStatus = git -c "safe.directory=$safeR2DreamerRoot" `
    -C $R2DreamerRoot status --porcelain
if ($LASTEXITCODE -ne 0) {
    throw "Could not inspect the R2-Dreamer repository."
}
if ($r2DreamerStatus) {
    throw "Refusing to install from a dirty R2-Dreamer checkout."
}
git -c "safe.directory=$safeR2DreamerRoot" -C $R2DreamerRoot checkout --detach $R2DreamerCommit
if ($LASTEXITCODE -ne 0) {
    throw "Could not detach R2-Dreamer at $R2DreamerCommit."
}
$actualCommit = git -c "safe.directory=$safeR2DreamerRoot" -C $R2DreamerRoot rev-parse HEAD
if ($LASTEXITCODE -ne 0) {
    throw "Could not inspect the R2-Dreamer repository."
}
if ($actualCommit.Trim() -ne $R2DreamerCommit) {
    throw "R2-Dreamer commit mismatch: expected $R2DreamerCommit, got $actualCommit"
}

$env:PYTHONNOUSERSITE = "1"
$env:PYTHONDONTWRITEBYTECODE = "1"

function Invoke-PythonLogged {
    param(
        [Parameter(Mandatory = $true)]
        [string]$LogPath,
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments
    )

    $previousErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $PythonExecutable @Arguments 2>&1 | Tee-Object -FilePath $LogPath | Out-Host
    $nativeExitCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorAction
    if ($nativeExitCode -ne 0) {
        throw "Command failed with exit code $nativeExitCode. See $LogPath"
    }
}

function Invoke-PipCheckSnapshot {
    param([Parameter(Mandatory = $true)][string]$LogPath)

    $previousErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $PythonExecutable -m pip check 2>&1 | Tee-Object -FilePath $LogPath | Out-Host
    $exitCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorAction
    return $exitCode
}

Invoke-PythonLogged -LogPath (Join-Path $LogRoot "g06_python_before_$RunId.txt") `
    -Arguments @("-c", "import site,sys,torch; print(sys.executable); print(site.ENABLE_USER_SITE); print(torch.__version__); print(torch.version.cuda)")
Invoke-PythonLogged -LogPath (Join-Path $ConfigRoot "requirements_g06_before_$RunId.txt") `
    -Arguments @("-m", "pip", "freeze")
$pipCheckBefore = Invoke-PipCheckSnapshot -LogPath (Join-Path $LogRoot "pip_check_g06_before_$RunId.txt")

Invoke-PythonLogged -LogPath (Join-Path $LogRoot "install_g06_dependencies_$RunId.txt") `
    -Arguments @(
        "-m", "pip", "install",
        "tensordict==0.8.3",
        "torchrl==0.8.1",
        "ruamel.yaml==0.17.4"
    )
Invoke-PythonLogged -LogPath (Join-Path $LogRoot "remove_g06_obsolete_pyvers_$RunId.txt") `
    -Arguments @("-m", "pip", "uninstall", "--yes", "pyvers")
Invoke-PythonLogged -LogPath (Join-Path $LogRoot "restore_g06_packaging_$RunId.txt") `
    -Arguments @("-m", "pip", "install", "packaging==23.0")
Invoke-PythonLogged -LogPath (Join-Path $LogRoot "install_g06_r2dreamer_$RunId.txt") `
    -Arguments @(
        "-m", "pip", "install", "--no-deps", "--no-build-isolation", "-e", $R2DreamerRoot
    )
Invoke-PythonLogged -LogPath (Join-Path $LogRoot "install_g06_onlinewm_$RunId.txt") `
    -Arguments @(
        "-m", "pip", "install", "--no-deps", "--no-build-isolation", "-e", $OnlineWMRoot
    )

$probeOutput = Join-Path $MetricsRoot "g06_probe_$RunId.json"
Invoke-PythonLogged -LogPath (Join-Path $LogRoot "g06_import_smoke_$RunId.txt") `
    -Arguments @(
        (Join-Path $PSScriptRoot "g06_probe.py"),
        "--r2-dreamer-root", $R2DreamerRoot,
        "--expected-r2-commit", $R2DreamerCommit,
        "--output", $probeOutput,
        "--headless"
    )
if (-not (Test-Path -LiteralPath $probeOutput)) {
    throw "The G06 probe did not produce its required JSON result: $probeOutput"
}
$probeResult = Get-Content -LiteralPath $probeOutput -Raw | ConvertFrom-Json
if ($probeResult.pass -ne $true) {
    throw "The G06 probe reported failure. See $probeOutput"
}
Invoke-PythonLogged -LogPath (Join-Path $ConfigRoot "requirements_g06_after_$RunId.txt") `
    -Arguments @("-m", "pip", "freeze")
$pipCheckAfter = Invoke-PipCheckSnapshot -LogPath (Join-Path $LogRoot "pip_check_g06_after_$RunId.txt")

$record = [ordered]@{
    schema_version = 1
    run_id = $RunId
    completed_at = Get-Date -Format o
    python_executable = $PythonExecutable
    python_no_user_site = $true
    r2dreamer_root = $R2DreamerRoot
    r2dreamer_commit = $R2DreamerCommit
    onlinewm_root = $OnlineWMRoot
    dependency_strategy = "Use the PyTorch-2.7-compatible TensorDict 0.8.3/TorchRL 0.8.1 pair, restore Isaac Sim's packaging 23.0 pin, and editable-install R2-Dreamer plus OnlineWM with --no-deps to preserve the frozen Torch/Isaac runtime."
    pip_check_before_exit_code = $pipCheckBefore
    pip_check_after_exit_code = $pipCheckAfter
    probe = $probeOutput
}
$record | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $RunRecord -Encoding UTF8

Write-Host "P0-G06 installation and import evidence written to $ArtifactRoot"
