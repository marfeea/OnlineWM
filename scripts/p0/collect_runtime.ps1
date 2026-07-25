[CmdletBinding()]
param(
    [string]$CondaEnvironment = "isaaclab",
    [string]$IsaacLabRoot = "D:\Software\Isaac Install\IsaacLab",
    [string]$ArtifactRoot = "",
    [switch]$SkipIsaacProbe
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($ArtifactRoot)) {
    $ArtifactRoot = Join-Path $ProjectRoot "artifacts\r2dreamer\p0_runtime"
}
$ArtifactRoot = [System.IO.Path]::GetFullPath($ArtifactRoot)
$LogsRoot = Join-Path $ArtifactRoot "logs"
$MetricsRoot = Join-Path $ArtifactRoot "metrics"
$ConfigsRoot = Join-Path $ArtifactRoot "configs"
$TestsRoot = Join-Path $ArtifactRoot "tests"

foreach ($directory in @($ArtifactRoot, $LogsRoot, $MetricsRoot, $ConfigsRoot, $TestsRoot)) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

if (-not (Get-Command conda -ErrorAction SilentlyContinue)) {
    throw "conda is not available in this terminal."
}
if (-not (Test-Path -LiteralPath $IsaacLabRoot -PathType Container)) {
    throw "Isaac Lab root does not exist: $IsaacLabRoot"
}

$env:PYTHONNOUSERSITE = "1"
$env:PYTHONDONTWRITEBYTECODE = "1"

function Invoke-NativeLogged {
    param(
        [Parameter(Mandatory = $true)]
        [string]$LogName,
        [Parameter(Mandatory = $true)]
        [scriptblock]$Command
    )

    $logPath = Join-Path $LogsRoot $LogName
    & $Command 2>&1 | Tee-Object -FilePath $logPath
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE. See $logPath"
    }
}

$hostLines = @(
    "collected_at=$(Get-Date -Format o)"
    "project_root=$ProjectRoot"
    "conda_environment=$CondaEnvironment"
    "isaaclab_root=$IsaacLabRoot"
    "powershell=$($PSVersionTable.PSVersion)"
)
$hostLines += cmd /c ver
$hostLines | Set-Content -Encoding UTF8 (Join-Path $LogsRoot "host.txt")

Invoke-NativeLogged "gpu.txt" {
    nvidia-smi --query-gpu=index,name,uuid,driver_version,memory.total --format=csv,noheader
}

Invoke-NativeLogged "onlinewm_git.txt" {
    git -C $ProjectRoot status --short --branch
    git -C $ProjectRoot rev-parse HEAD
    git -C $ProjectRoot remote -v
}

$safeIsaacLabRoot = $IsaacLabRoot.Replace("\", "/")
Invoke-NativeLogged "isaaclab_git.txt" {
    git -c "safe.directory=$safeIsaacLabRoot" -C $IsaacLabRoot status --short --branch
    git -c "safe.directory=$safeIsaacLabRoot" -C $IsaacLabRoot rev-parse HEAD
    git -c "safe.directory=$safeIsaacLabRoot" -C $IsaacLabRoot describe --tags --always --dirty
    git -c "safe.directory=$safeIsaacLabRoot" -C $IsaacLabRoot remote -v
}

Invoke-NativeLogged "conda_info.txt" {
    conda info
    conda env list
}
conda env export --name $CondaEnvironment --from-history |
    Set-Content -Encoding UTF8 (Join-Path $ConfigsRoot "conda-history.yml")
if ($LASTEXITCODE -ne 0) {
    throw "Could not export Conda history."
}
conda run --name $CondaEnvironment python -m pip freeze |
    Set-Content -Encoding UTF8 (Join-Path $ConfigsRoot "requirements.lock.txt")
if ($LASTEXITCODE -ne 0) {
    throw "Could not export pip requirements."
}

if (-not $SkipIsaacProbe) {
    $probeScript = Join-Path $ProjectRoot "scripts\p0\runtime_probe.py"
    $probeOutput = Join-Path $MetricsRoot "runtime_probe.json"
    Invoke-NativeLogged "isaac_import_smoke.txt" {
        conda run --name $CondaEnvironment python $probeScript --headless --output $probeOutput
    }
}

$hashRows = Get-ChildItem -LiteralPath $ArtifactRoot -File -Recurse |
    Where-Object { $_.Name -ne "sha256.csv" } |
    Sort-Object FullName |
    ForEach-Object {
        $relativePath = $_.FullName.Substring($ArtifactRoot.Length)
        $relativePath = $relativePath.TrimStart([char[]]@("\", "/")).Replace("\", "/")
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant()
        [PSCustomObject]@{ path = $relativePath; sha256 = $hash; bytes = $_.Length }
    }
$hashRows | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $TestsRoot "sha256.csv")

Write-Host "P0 runtime evidence written to $ArtifactRoot"
