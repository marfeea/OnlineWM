[CmdletBinding()]
param(
    [string]$PythonExecutable = "D:\Anaconda\envs\isaaclab\python.exe",
    [string]$R2DreamerRoot = "D:\Software\R2-Dreamer",
    [string]$ArtifactRoot = "",
    [string]$RunId = "",
    [int]$InitialSteps = 96,
    [int]$ResumeSteps = 80,
    [int]$NumEnvs = 2,
    [int]$BatchSize = 2,
    [int]$BatchLength = 4,
    [int]$TrainRatio = 2,
    [int]$PolicySteps = 48
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($ArtifactRoot)) {
    $ArtifactRoot = Join-Path $ProjectRoot "artifacts\r2dreamer\p3_official_vision"
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
$InitialSamples = Join-Path $ArtifactRoot "rgb_samples\initial_$RunId"
$ResumeSamples = Join-Path $ArtifactRoot "rgb_samples\resume_$RunId"
$InitialVideo = Join-Path $ArtifactRoot "videos\fixed_policy_initial_$RunId.mp4"
$ResumeVideo = Join-Path $ArtifactRoot "videos\fixed_policy_resume_$RunId.mp4"
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
    (Join-Path $ArtifactRoot "rgb_samples"),
    (Join-Path $ArtifactRoot "tests"),
    (Join-Path $ArtifactRoot "videos"),
    $InitialRoot,
    $ResumeRoot
)) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

$env:PYTHONNOUSERSITE = "1"
$env:PYTHONDONTWRITEBYTECODE = "1"
$env:HYDRA_FULL_ERROR = "1"

function Invoke-P3Run {
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
        -Pattern "Traceback \(most recent call last\)|CUDA out of memory" -Quiet
    if ($nativeExitCode -ne 0 -or $failureMarker) {
        throw "P3 command failed. Exit=$nativeExitCode; failure_marker=$failureMarker; see $ConsolePath"
    }
}

$CommonOverrides = @(
    "env=isaaclab_vision",
    "model.rep_loss=r2dreamer",
    "env.env_num=$NumEnvs",
    "batch_size=$BatchSize",
    "batch_length=$BatchLength",
    "env.train_ratio=$TrainRatio",
    "trainer.update_log_every=8",
    "model.compile=false",
    "model.log_grads=true",
    "model.deter=64",
    "model.hidden=32",
    "model.discrete=4",
    "model.units=32",
    "model.depth=8",
    "model.rssm.blocks=4",
    "model.imag_horizon=3"
)
$TrainScript = Join-Path $PSScriptRoot "train_vision_chain.py"

$initialArgs = @(
    $TrainScript,
    "--r2-dreamer-root", $R2DreamerRoot,
    "--diagnostics", (Join-Path $InitialRoot "diagnostics.json"),
    "--checkpoint-out", $InitialCheckpoint,
    "--samples-out", $InitialSamples,
    "--policy-video", $InitialVideo,
    "--policy-steps", $PolicySteps,
    "--"
) + $CommonOverrides + @(
    "env.steps=$InitialSteps",
    "logdir=$InitialRoot"
)
Invoke-P3Run -Arguments $initialArgs -ConsolePath (Join-Path $InitialRoot "launcher.log")

$resumeArgs = @(
    $TrainScript,
    "--r2-dreamer-root", $R2DreamerRoot,
    "--resume", $InitialCheckpoint,
    "--diagnostics", (Join-Path $ResumeRoot "diagnostics.json"),
    "--checkpoint-out", $ResumeCheckpoint,
    "--samples-out", $ResumeSamples,
    "--policy-video", $ResumeVideo,
    "--policy-steps", $PolicySteps,
    "--"
) + $CommonOverrides + @(
    "env.steps=$ResumeSteps",
    "logdir=$ResumeRoot"
)
Invoke-P3Run -Arguments $resumeArgs -ConsolePath (Join-Path $ResumeRoot "launcher.log")

& $PythonExecutable (Join-Path $PSScriptRoot "verify_acceptance.py") `
    --initial-diagnostics (Join-Path $InitialRoot "diagnostics.json") `
    --initial-console (Join-Path $InitialRoot "console.log") `
    --resume-diagnostics (Join-Path $ResumeRoot "diagnostics.json") `
    --resume-console (Join-Path $ResumeRoot "console.log") `
    --initial-checkpoint $InitialCheckpoint `
    --resume-checkpoint $ResumeCheckpoint `
    --output $Validation
if ($LASTEXITCODE -ne 0) {
    throw "P3 smoke acceptance failed. See $Validation"
}

$runData = [ordered]@{
    schema_version = 1
    run_id = $RunId
    completed_at = Get-Date -Format o
    profile = "reduced_gate_smoke"
    official_environment = "isaaclab_vision"
    official_task = "isaaclab_Isaac-Cartpole-RGB-Camera-Direct-v0"
    representation_loss = "r2dreamer"
    python_executable = $PythonExecutable
    python_no_user_site = $true
    r2dreamer_root = $R2DreamerRoot
    initial_steps = $InitialSteps
    resume_steps = $ResumeSteps
    num_envs = $NumEnvs
    batch_size = $BatchSize
    batch_length = $BatchLength
    train_ratio = $TrainRatio
    policy_steps = $PolicySteps
    numerical_mode = "float32_updates_no_grad_scaling"
    strict_episode_replay = $true
    action_clipping = @(-1.0, 1.0)
    initial_checkpoint = $InitialCheckpoint
    resume_checkpoint = $ResumeCheckpoint
    initial_policy_video = $InitialVideo
    resume_policy_video = $ResumeVideo
    validation = $Validation
}
$runData | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $RunConfig -Encoding UTF8
Write-Host "P3 reduced smoke evidence written to $ArtifactRoot"
