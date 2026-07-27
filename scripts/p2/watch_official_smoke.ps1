[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [int]$ProcessId,
    [Parameter(Mandatory = $true)]
    [string]$RunRoot,
    [int]$ExpectedSteps = 10000,
    [int]$PollSeconds = 30
)

$ErrorActionPreference = "Stop"
$RunRoot = [System.IO.Path]::GetFullPath($RunRoot)
$StatusPath = Join-Path $RunRoot "run_status.json"

while (Get-Process -Id $ProcessId -ErrorAction SilentlyContinue) {
    Start-Sleep -Seconds $PollSeconds
}

$metricsPath = Join-Path $RunRoot "metrics.jsonl"
$checkpointPath = Join-Path $RunRoot "latest.pt"
$configPath = Join-Path $RunRoot ".hydra\config.yaml"
$logPaths = @(
    (Join-Path $RunRoot "console.log"),
    (Join-Path $RunRoot "launcher.stdout.log"),
    (Join-Path $RunRoot "launcher.stderr.log")
)
$existingLogs = @($logPaths | Where-Object { Test-Path -LiteralPath $_ -PathType Leaf })
$combinedLog = ($existingLogs | ForEach-Object {
    Get-Content -LiteralPath $_ -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
}) -join "`n"

$metricRows = @()
if (Test-Path -LiteralPath $metricsPath -PathType Leaf) {
    $metricRows = @(Get-Content -LiteralPath $metricsPath -Encoding UTF8 | ForEach-Object {
        if (-not [string]::IsNullOrWhiteSpace($_)) {
            $_ | ConvertFrom-Json
        }
    })
}
$maxStep = if ($metricRows.Count) {
    ($metricRows | Measure-Object -Property step -Maximum).Maximum
} else {
    0
}
$trainingRows = @($metricRows | Where-Object { $null -ne $_.'train/opt/loss' })
$episodeRows = @($metricRows | Where-Object {
    $null -ne $_.'episode/score' -and $null -ne $_.'episode/length'
})
$tracebackAbsent = $combinedLog -notmatch "Traceback \(most recent call last\)"
$nonFiniteAbsent = $combinedLog -notmatch "(?i)(^|[^a-z])(nan|inf|infinity)([^a-z]|$)"
$checkpointPresent = (
    (Test-Path -LiteralPath $checkpointPath -PathType Leaf) -and
    (Get-Item -LiteralPath $checkpointPath).Length -gt 0
)
$configText = if (Test-Path -LiteralPath $configPath -PathType Leaf) {
    Get-Content -LiteralPath $configPath -Raw -Encoding UTF8
} else {
    ""
}
$expectedStepsConfigured = $configText -match "(?m)^  steps: $ExpectedSteps\s*$"

# The frozen upstream train.py saves latest.pt only after OnlineTrainer.begin()
# returns. That loop returns when its internal step counter reaches env.steps.
# Metrics are event-driven and do not include an unconditional final-step row,
# so max_logged_step can legitimately be below ExpectedSteps.
$trainingLoopCompleted = $checkpointPresent -and $expectedStepsConfigured

$checks = [ordered]@{
    expected_steps_configured = $expectedStepsConfigured
    training_loop_completed = $trainingLoopCompleted
    training_metrics_present = $trainingRows.Count -gt 0
    episode_metrics_present = $episodeRows.Count -gt 1
    checkpoint_present = $checkpointPresent
    traceback_absent = $tracebackAbsent
    non_finite_log_marker_absent = $nonFiniteAbsent
}
$passed = -not ($checks.Values -contains $false)
$result = [ordered]@{
    schema_version = 1
    checked_at = Get-Date -Format o
    process_id = $ProcessId
    expected_steps = $ExpectedSteps
    max_logged_step = [int64]$maxStep
    completion_evidence = "latest.pt is saved only after OnlineTrainer.begin returns at env.steps"
    training_metric_rows = $trainingRows.Count
    episode_metric_rows = $episodeRows.Count
    checkpoint = $checkpointPath
    checks = $checks
    pass = $passed
    status = if ($passed) { "PASS" } else { "FAIL" }
}
$result | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $StatusPath -Encoding UTF8
