[CmdletBinding()]
param(
    [string]$PythonExecutable = "D:\Anaconda\envs\isaaclab\python.exe",
    [string]$R2DreamerRoot = "D:\Software\R2-Dreamer",
    [string]$ArtifactRoot = "",
    [string]$RunId = "",
    [int]$InitialSteps = 320,
    [int]$ResumeSteps = 240,
    [int]$NumEnvs = 4,
    [int]$BatchSize = 2,
    [int]$BatchLength = 8,
    [int]$TrainRatio = 4
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($ArtifactRoot)) {
    $ArtifactRoot = Join-Path $ProjectRoot "artifacts\r2dreamer\p2_official_proprio"
}
if ([string]::IsNullOrWhiteSpace($RunId)) {
    $RunId = Get-Date -Format "yyyyMMdd_HHmmss"
}
$ArtifactRoot = [System.IO.Path]::GetFullPath($ArtifactRoot)
$RunConfig = Join-Path $ArtifactRoot "configs\run_$RunId.json"
$InitialRoot = Join-Path $ArtifactRoot "smoke_initial_$RunId"
$ResumeRoot = Join-Path $ArtifactRoot "smoke_resume_$RunId"
$InitialCheckpoint = Join-Path $ArtifactRoot "checkpoints\smoke_initial_$RunId.pt"
$ResumeCheckpoint = Join-Path $ArtifactRoot "checkpoints\smoke_resume_$RunId.pt"
$Validation = Join-Path $ArtifactRoot "tests\smoke_acceptance_$RunId.json"

if (-not (Test-Path -LiteralPath $PythonExecutable -PathType Leaf)) {
    throw "Python executable does not exist: $PythonExecutable"
}
if (-not (Test-Path -LiteralPath (Join-Path $R2DreamerRoot "train.py") -PathType Leaf)) {
    throw "R2-Dreamer checkout does not exist: $R2DreamerRoot"
}
if (Test-Path -LiteralPath $RunConfig) {
    throw "Run ID '$RunId' already exists. Evidence is never overwritten."
}

foreach ($directory in @(
    $ArtifactRoot,
    (Join-Path $ArtifactRoot "configs"),
    (Join-Path $ArtifactRoot "checkpoints"),
    (Join-Path $ArtifactRoot "tests"),
    $InitialRoot,
    $ResumeRoot
)) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

$env:PYTHONNOUSERSITE = "1"
$env:PYTHONDONTWRITEBYTECODE = "1"
$env:HYDRA_FULL_ERROR = "1"

function Invoke-P2Run {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments,
        [Parameter(Mandatory = $true)]
        [string]$ConsolePath
    )
    $previousErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $PythonExecutable @Arguments 2>&1 | Tee-Object -FilePath $ConsolePath
    $nativeExitCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorAction
    $failureMarker = Select-String -LiteralPath $ConsolePath `
        -Pattern "Traceback \(most recent call last\)" -Quiet
    if ($nativeExitCode -ne 0 -or $failureMarker) {
        throw "P2 command failed. Exit=$nativeExitCode; traceback=$failureMarker; see $ConsolePath"
    }
}

$CommonOverrides = @(
    "env=isaaclab_proprio",
    "model.rep_loss=r2dreamer",
    "env.env_num=$NumEnvs",
    "batch_size=$BatchSize",
    "batch_length=$BatchLength",
    "env.train_ratio=$TrainRatio",
    "trainer.update_log_every=64",
    "model.compile=false",
    "model.log_grads=true",
    "model.deter=128",
    "model.hidden=64",
    "model.discrete=8",
    "model.units=64",
    "model.rssm.blocks=8",
    "model.imag_horizon=5"
)
$TrainScript = Join-Path $PSScriptRoot "train_state_chain.py"

$initialArgs = @(
    $TrainScript,
    "--r2-dreamer-root", $R2DreamerRoot,
    "--diagnostics", (Join-Path $InitialRoot "diagnostics.json"),
    "--checkpoint-out", $InitialCheckpoint,
    "--"
) + $CommonOverrides + @(
    "env.steps=$InitialSteps",
    "logdir=$InitialRoot"
)
Invoke-P2Run -Arguments $initialArgs -ConsolePath (Join-Path $InitialRoot "launcher.log")

$resumeArgs = @(
    $TrainScript,
    "--r2-dreamer-root", $R2DreamerRoot,
    "--resume", $InitialCheckpoint,
    "--diagnostics", (Join-Path $ResumeRoot "diagnostics.json"),
    "--checkpoint-out", $ResumeCheckpoint,
    "--"
) + $CommonOverrides + @(
    "env.steps=$ResumeSteps",
    "logdir=$ResumeRoot"
)
Invoke-P2Run -Arguments $resumeArgs -ConsolePath (Join-Path $ResumeRoot "launcher.log")

& $PythonExecutable (Join-Path $PSScriptRoot "verify_acceptance.py") `
    --initial-diagnostics (Join-Path $InitialRoot "diagnostics.json") `
    --initial-metrics (Join-Path $InitialRoot "metrics.jsonl") `
    --initial-console (Join-Path $InitialRoot "console.log") `
    --resume-diagnostics (Join-Path $ResumeRoot "diagnostics.json") `
    --resume-metrics (Join-Path $ResumeRoot "metrics.jsonl") `
    --resume-console (Join-Path $ResumeRoot "console.log") `
    --initial-checkpoint $InitialCheckpoint `
    --resume-checkpoint $ResumeCheckpoint `
    --output $Validation
if ($LASTEXITCODE -ne 0) {
    throw "P2 smoke acceptance failed. See $Validation"
}

$runData = [ordered]@{
    schema_version = 1
    run_id = $RunId
    completed_at = Get-Date -Format o
    profile = "reduced_gate_smoke"
    python_executable = $PythonExecutable
    python_no_user_site = $true
    r2dreamer_root = $R2DreamerRoot
    initial_steps = $InitialSteps
    resume_steps = $ResumeSteps
    num_envs = $NumEnvs
    batch_size = $BatchSize
    batch_length = $BatchLength
    train_ratio = $TrainRatio
    numerical_mode = "float32_updates_no_grad_scaling"
    strict_episode_replay = $true
    action_clipping = @(-1.0, 1.0)
    initial_checkpoint = $InitialCheckpoint
    resume_checkpoint = $ResumeCheckpoint
    validation = $Validation
}
$runData | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $RunConfig -Encoding UTF8
Write-Host "P2 reduced smoke evidence written to $ArtifactRoot"
