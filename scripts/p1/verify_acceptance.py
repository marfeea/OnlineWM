"""Validate the P1 evidence package and write machine-readable QA evidence."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import subprocess
from datetime import datetime
from pathlib import Path

import cv2


def sha256(path: Path) -> str:
    """Calculate a file SHA256."""
    digest = hashlib.sha256()
    with path.open("rb") as file:
        for chunk in iter(lambda: file.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_value(repository: Path, *arguments: str) -> str:
    """Run a read-only Git query."""
    result = subprocess.run(
        ["git", "-c", f"safe.directory={repository.as_posix()}", "-C", str(repository), *arguments],
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def inspect_video(video_path: Path) -> dict[str, object]:
    """Read the accepted video and verify that frames can be decoded."""
    capture = cv2.VideoCapture(str(video_path))
    if not capture.isOpened():
        raise ValueError(f"Could not open video: {video_path}")
    declared_frames = int(capture.get(cv2.CAP_PROP_FRAME_COUNT))
    width = int(capture.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(capture.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = float(capture.get(cv2.CAP_PROP_FPS))
    decoded_frames = 0
    while True:
        success, _ = capture.read()
        if not success:
            break
        decoded_frames += 1
    capture.release()
    return {
        "path": video_path.as_posix(),
        "bytes": video_path.stat().st_size,
        "sha256": sha256(video_path),
        "codec_frame_count": declared_frames,
        "decoded_frames": decoded_frames,
        "width": width,
        "height": height,
        "fps": fps,
        "pass": (
            decoded_frames >= 299
            and declared_frames == decoded_frames
            and width > 0
            and height > 0
            and fps > 0.0
        ),
    }


def read_log(path: Path) -> str:
    """Read PowerShell 5.1 UTF-16 logs and regular UTF-8 logs."""
    data = path.read_bytes()
    if data.startswith((b"\xff\xfe", b"\xfe\xff")):
        return data.decode("utf-16")
    return data.decode("utf-8", errors="replace")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--artifact-root", type=Path, required=True)
    parser.add_argument("--project-root", type=Path, required=True)
    parser.add_argument("--isaaclab-root", type=Path, required=True)
    parser.add_argument("--run-id", required=True)
    parser.add_argument("--training-runs", nargs=3, required=True)
    args = parser.parse_args()

    artifact_root = args.artifact_root.resolve()
    tests_root = artifact_root / "tests"
    tests_root.mkdir(parents=True, exist_ok=True)
    probe_path = artifact_root / "metrics" / f"environment_probe_{args.run_id}.json"
    training_summary_path = artifact_root / "metrics" / f"training_summary_{args.run_id}.json"
    checkpoint_path = artifact_root / "checkpoints" / f"cartpole_{args.run_id}.pth"
    video_path = artifact_root / "videos" / f"fixed_policy_{args.run_id}.mp4"
    resume_log = artifact_root / "logs" / f"resume_{args.run_id}.txt"
    play_log = artifact_root / "logs" / f"play_{args.run_id}.txt"
    training_logs = [
        artifact_root / "logs" / f"train_{args.run_id}_run{index}.txt" for index in range(1, 4)
    ]

    probe = json.loads(probe_path.read_text(encoding="utf-8"))
    training_summary = json.loads(training_summary_path.read_text(encoding="utf-8"))
    training_log_checks = []
    for path in training_logs:
        text = read_log(path)
        training_log_checks.append(
            {
                "path": path.relative_to(artifact_root).as_posix(),
                "max_epochs_reached": "MAX EPOCHS NUM!" in text,
                "training_time_reported": "Training time:" in text,
                "traceback_absent": "Traceback (most recent call last)" not in text,
            }
        )
    resume_text = read_log(resume_log)
    play_text = read_log(play_log)
    video = inspect_video(video_path)
    checkpoint = {
        "path": checkpoint_path.relative_to(artifact_root).as_posix(),
        "bytes": checkpoint_path.stat().st_size,
        "sha256": sha256(checkpoint_path),
        "pass": checkpoint_path.stat().st_size > 0,
    }
    repositories = {
        "onlinewm": {
            "path": str(args.project_root.resolve()),
            "commit": git_value(args.project_root, "rev-parse", "HEAD"),
            "dirty": bool(git_value(args.project_root, "status", "--short")),
        },
        "isaaclab": {
            "path": str(args.isaaclab_root.resolve()),
            "commit": git_value(args.isaaclab_root, "rev-parse", "HEAD"),
            "dirty": bool(git_value(args.isaaclab_root, "status", "--short")),
        },
    }
    checks = {
        "P1-G01": bool(probe["checks"]["parallel_environment_created"]),
        "P1-G02": bool(probe["pass"]),
        "P1-G03": all(
            check["max_epochs_reached"] and check["training_time_reported"] and check["traceback_absent"]
            for check in training_log_checks
        )
        and checkpoint["pass"],
        "P1-G04": bool(training_summary["pass"]),
        "P1-G05": len(args.training_runs) == 3
        and all(check["traceback_absent"] and check["max_epochs_reached"] for check in training_log_checks),
        "checkpoint_resume": (
            "loading checkpoint" in resume_text.lower()
            and "epoch: 155/155" in resume_text
            and "Traceback (most recent call last)" not in resume_text
        ),
        "fixed_policy_video": bool(video["pass"])
        and "loading checkpoint" in play_text.lower()
        and "Traceback (most recent call last)" not in play_text,
    }
    result = {
        "schema_version": 1,
        "verified_at": datetime.now().astimezone().isoformat(),
        "run_id": args.run_id,
        "training_runs": args.training_runs,
        "repositories": repositories,
        "training_logs": training_log_checks,
        "checkpoint": checkpoint,
        "video": video,
        "checks": checks,
        "pass": all(checks.values()),
    }
    validation_path = tests_root / f"acceptance_validation_{args.run_id}.json"
    validation_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")

    hashes_path = tests_root / "sha256.csv"
    with hashes_path.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=("path", "sha256", "bytes"))
        writer.writeheader()
        for path in sorted(artifact_root.rglob("*")):
            if path.is_file() and path != hashes_path:
                writer.writerow(
                    {
                        "path": path.relative_to(artifact_root).as_posix(),
                        "sha256": sha256(path),
                        "bytes": path.stat().st_size,
                    }
                )
    print(json.dumps(result, indent=2))
    return 0 if result["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
