[CmdletBinding()]
param(
    [string]$CondaEnvironment = "isaaclab",
    [string]$IsaacLabRoot = "D:\Software\Isaac Install\IsaacLab",
    [string]$ArtifactRoot = "",
    [int]$NumEnvs = 16,
    [int]$ProbeSteps = 640,
    [int]$MaxIterations = 150,
    [int]$MiniBatchSize = 512,
    [int]$Repetitions = 3,
    [string]$RunId = ""
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($ArtifactRoot)) {
    $ArtifactRoot = Join-Path $ProjectRoot "artifacts\r2dreamer\p1_isaaclab_official"
}
if ([string]::IsNullOrWhiteSpace($RunId)) {
    $RunId = Get-Date -Format "yyyyMMdd_HHmmss"
}
$ArtifactRoot = [System.IO.Path]::GetFullPath($ArtifactRoot)
$OfficialTrain = Join-Path $IsaacLabRoot "scripts\reinforcement_learning\rl_games\train.py"
$OfficialPlay = Join-Path $IsaacLabRoot "scripts\reinforcement_learning\rl_games\play.py"
$RunConfig = Join-Path $ArtifactRoot "configs\run_$RunId.json"

if (-not (Get-Command conda -ErrorAction SilentlyContinue)) {
    throw "conda is not available in this terminal."
}
if (-not (Test-Path -LiteralPath $OfficialTrain -PathType Leaf)) {
    throw "Official RL-Games training entry does not exist: $OfficialTrain"
}
if (-not (Test-Path -LiteralPath $OfficialPlay -PathType Leaf)) {
    throw "Official RL-Games play entry does not exist: $OfficialPlay"
}
if (Test-Path -LiteralPath $RunConfig) {
    throw "Run ID '$RunId' already exists. Formal evidence is never overwritten."
}
if (($NumEnvs * 32) % $MiniBatchSize -ne 0) {
    throw "NumEnvs * horizon_length (32) must be divisible by MiniBatchSize."
}

$directories = @(
    $ArtifactRoot,
    (Join-Path $ArtifactRoot "configs"),
    (Join-Path $ArtifactRoot "logs"),
    (Join-Path $ArtifactRoot "metrics"),
    (Join-Path $ArtifactRoot "plots"),
    (Join-Path $ArtifactRoot "tests"),
    (Join-Path $ArtifactRoot "videos"),
    (Join-Path $ArtifactRoot "checkpoints")
)
foreach ($directory in $directories) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

$env:PYTHONNOUSERSITE = "1"
$env:PYTHONDONTWRITEBYTECODE = "1"
$env:HYDRA_FULL_ERROR = "1"

function Invoke-CondaPythonLogged {
    param(
        [Parameter(Mandatory = $true)]
        [string]$LogPath,
        [Parameter(Mandatory = $true)]
        [string[]]$PythonArguments
    )

    $previousErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & conda run --name $CondaEnvironment python @PythonArguments 2>&1 |
        Tee-Object -FilePath $LogPath
    $nativeExitCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorAction
    $failureMarker = Select-String -LiteralPath $LogPath `
        -Pattern "ERROR conda.cli.main_run:execute", "Traceback \(most recent call last\)" `
        -Quiet
    if ($nativeExitCode -ne 0 -or $failureMarker) {
        throw "Command failed with exit code $nativeExitCode. See $LogPath"
    }
}

$runConfigData = [ordered]@{
    schema_version = 1
    run_id = $RunId
    started_at = (Get-Date -Format o)
    conda_environment = $CondaEnvironment
    python_no_user_site = $true
    isaaclab_root = $IsaacLabRoot
    official_train = $OfficialTrain
    official_play = $OfficialPlay
    task = "Isaac-Cartpole-Direct-v0"
    num_envs = $NumEnvs
    probe_steps = $ProbeSteps
    max_iterations = $MaxIterations
    minibatch_size = $MiniBatchSize
    repetitions = $Repetitions
    working_directory = $ArtifactRoot
    training_command = @(
        "conda", "run", "--name", $CondaEnvironment, "python", $OfficialTrain,
        "--task", "Isaac-Cartpole-Direct-v0",
        "--num_envs", "$NumEnvs",
        "--headless",
        "--max_iterations", "$MaxIterations",
        "agent.params.config.minibatch_size=$MiniBatchSize",
        "+agent.params.config.torch_compile=false"
    )
}
$runConfigData | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $RunConfig -Encoding UTF8

Set-Location -LiteralPath $ArtifactRoot

$upstreamAgentConfig = Join-Path $IsaacLabRoot `
    "source\isaaclab_tasks\isaaclab_tasks\direct\cartpole\agents\rl_games_ppo_cfg.yaml"
Copy-Item -LiteralPath $upstreamAgentConfig `
    -Destination (Join-Path $ArtifactRoot "configs\rl_games_ppo_cfg_${RunId}.upstream.yaml")

$probeOutput = Join-Path $ArtifactRoot "metrics\environment_probe_$RunId.json"
$probeLog = Join-Path $ArtifactRoot "logs\environment_probe_$RunId.txt"
Invoke-CondaPythonLogged -LogPath $probeLog -PythonArguments @(
    (Join-Path $PSScriptRoot "env_probe.py"),
    "--task", "Isaac-Cartpole-Direct-v0",
    "--num_envs", "$NumEnvs",
    "--steps", "$ProbeSteps",
    "--output", $probeOutput,
    "--headless"
)

$runsRoot = Join-Path $ArtifactRoot "logs\rl_games\cartpole_direct"
$beforeRuns = @()
if (Test-Path -LiteralPath $runsRoot) {
    $beforeRuns = @(Get-ChildItem -LiteralPath $runsRoot -Directory | ForEach-Object { $_.FullName })
}

$trainingArgs = @(
    $OfficialTrain,
    "--task", "Isaac-Cartpole-Direct-v0",
    "--num_envs", "$NumEnvs",
    "--headless",
    "--max_iterations", "$MaxIterations",
    "agent.params.config.minibatch_size=$MiniBatchSize",
    "+agent.params.config.torch_compile=false"
)
for ($index = 1; $index -le $Repetitions; $index++) {
    $trainingLog = Join-Path $ArtifactRoot ("logs\train_{0}_run{1}.txt" -f $RunId, $index)
    Invoke-CondaPythonLogged -LogPath $trainingLog -PythonArguments $trainingArgs
}

$newRuns = @(
    Get-ChildItem -LiteralPath $runsRoot -Directory |
        Where-Object { $beforeRuns -notcontains $_.FullName } |
        Sort-Object Name
)
if ($newRuns.Count -ne $Repetitions) {
    throw "Expected $Repetitions new RL-Games runs, found $($newRuns.Count)."
}
for ($index = 0; $index -lt $newRuns.Count; $index++) {
    $snapshotPrefix = "actual_{0}_run{1}" -f $RunId, ($index + 1)
    Copy-Item -LiteralPath (Join-Path $newRuns[$index].FullName "params\env.yaml") `
        -Destination (Join-Path $ArtifactRoot "configs\${snapshotPrefix}_env.yaml")
    Copy-Item -LiteralPath (Join-Path $newRuns[$index].FullName "params\agent.yaml") `
        -Destination (Join-Path $ArtifactRoot "configs\${snapshotPrefix}_agent.yaml")
}

$analysisLog = Join-Path $ArtifactRoot "logs\analysis_$RunId.txt"
$analysisArguments = @(
    (Join-Path $PSScriptRoot "analyze_training.py"),
    "--runs"
)
$analysisArguments += @($newRuns | ForEach-Object { $_.FullName })
$analysisArguments += @(
    "--metrics-dir", (Join-Path $ArtifactRoot "metrics"),
    "--plots-dir", (Join-Path $ArtifactRoot "plots"),
    "--expected-runs", "$Repetitions",
    "--label", $RunId
)
Invoke-CondaPythonLogged -LogPath $analysisLog -PythonArguments $analysisArguments

$lastRun = $newRuns[-1]
$checkpoint = Get-ChildItem -LiteralPath (Join-Path $lastRun.FullName "nn") -Filter "*.pth" -File |
    Sort-Object LastWriteTime |
    Select-Object -Last 1
if ($null -eq $checkpoint) {
    throw "No checkpoint found in $($lastRun.FullName)."
}
$acceptedCheckpoint = Join-Path $ArtifactRoot ("checkpoints\cartpole_{0}.pth" -f $RunId)
Copy-Item -LiteralPath $checkpoint.FullName -Destination $acceptedCheckpoint

$resumeIterations = $MaxIterations + 5
$resumeLog = Join-Path $ArtifactRoot "logs\resume_$RunId.txt"
Invoke-CondaPythonLogged -LogPath $resumeLog -PythonArguments @(
    $OfficialTrain,
    "--task", "Isaac-Cartpole-Direct-v0",
    "--num_envs", "$NumEnvs",
    "--headless",
    "--max_iterations", "$resumeIterations",
    "--checkpoint", $acceptedCheckpoint,
    "agent.params.config.minibatch_size=$MiniBatchSize",
    "+agent.params.config.torch_compile=false"
)

$playLog = Join-Path $ArtifactRoot "logs\play_$RunId.txt"
$playVideoRoot = Join-Path $ArtifactRoot "videos\play"
$beforeVideos = @()
if (Test-Path -LiteralPath $playVideoRoot) {
    $beforeVideos = @(Get-ChildItem -LiteralPath $playVideoRoot -Filter "*.mp4" -File -Recurse |
        ForEach-Object { $_.FullName })
}
Invoke-CondaPythonLogged -LogPath $playLog -PythonArguments @(
    $OfficialPlay,
    "--task", "Isaac-Cartpole-Direct-v0",
    "--num_envs", "1",
    "--headless",
    "--video",
    "--video_length", "300",
    "--checkpoint", $acceptedCheckpoint
)
$recordedVideo = Get-ChildItem -LiteralPath $playVideoRoot -Filter "*.mp4" -File -Recurse |
    Where-Object { $beforeVideos -notcontains $_.FullName } |
    Sort-Object LastWriteTime |
    Select-Object -Last 1
if ($null -eq $recordedVideo) {
    throw "Fixed-policy video was not created under $($lastRun.FullName)."
}
Copy-Item -LiteralPath $recordedVideo.FullName `
    -Destination (Join-Path $ArtifactRoot ("videos\fixed_policy_{0}.mp4" -f $RunId))

$hydraOutput = Join-Path $ArtifactRoot "outputs"
if (Test-Path -LiteralPath $hydraOutput) {
    Move-Item -LiteralPath $hydraOutput `
        -Destination (Join-Path $ArtifactRoot "logs\hydra_$RunId")
}

$runConfigData["completed_at"] = Get-Date -Format o
$runConfigData["run_directories"] = @($newRuns | ForEach-Object { $_.FullName })
$runConfigData["checkpoint_source"] = $checkpoint.FullName
$runConfigData["accepted_checkpoint"] = $acceptedCheckpoint
$runConfigData["video_source"] = $recordedVideo.FullName
$runConfigData | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $RunConfig -Encoding UTF8

Write-Host "P1 official evidence written to $ArtifactRoot"
