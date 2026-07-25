"""Extract P1 RL-Games TensorBoard metrics and generate acceptance summaries."""

from __future__ import annotations

import argparse
import csv
import json
import statistics
from datetime import datetime
from pathlib import Path

from tensorboard.backend.event_processing.event_accumulator import EventAccumulator


def load_reward_series(run_dir: Path) -> tuple[str, list[tuple[int, float]]]:
    """Load the episode-return-per-iteration scalar from one RL-Games run."""
    event_files = sorted(run_dir.rglob("events.out.tfevents.*"))
    if not event_files:
        raise ValueError(f"No TensorBoard event file found below {run_dir}")
    event_directories = {event_file.parent for event_file in event_files}
    if len(event_directories) != 1:
        raise ValueError(f"Expected one TensorBoard event directory below {run_dir}, found {event_directories}")
    accumulator = EventAccumulator(str(event_directories.pop()), size_guidance={"scalars": 0})
    accumulator.Reload()
    scalar_tags = accumulator.Tags().get("scalars", [])
    candidates = [
        tag for tag in scalar_tags if tag.endswith("/iter") and ("reward" in tag.lower() or "return" in tag.lower())
    ]
    if not candidates:
        raise ValueError(f"No episode reward /iter scalar found in {run_dir}; tags={scalar_tags}")
    tag = "rewards/iter" if "rewards/iter" in candidates else sorted(candidates)[0]
    series = [(event.step, float(event.value)) for event in accumulator.Scalars(tag)]
    if not series:
        raise ValueError(f"Scalar {tag!r} has no samples in {run_dir}")
    return tag, series


def summarize_series(series: list[tuple[int, float]]) -> dict[str, float | int]:
    """Compare robust early and late windows of one learning curve."""
    window = max(1, min(10, len(series) // 5))
    initial = statistics.median(value for _, value in series[:window])
    final = statistics.median(value for _, value in series[-window:])
    return {
        "samples": len(series),
        "first_step": series[0][0],
        "last_step": series[-1][0],
        "window": window,
        "initial_median": initial,
        "final_median": final,
        "absolute_improvement": final - initial,
        "relative_improvement": (final - initial) / max(abs(initial), 1.0),
        "peak_return": max(value for _, value in series),
    }


def write_svg(curves: list[tuple[str, list[tuple[int, float]]]], output: Path) -> None:
    """Write a dependency-free SVG learning-curve plot."""
    width, height = 960, 540
    left, right, top, bottom = 80, 30, 35, 65
    all_points = [point for _, curve in curves for point in curve]
    min_x = min(step for step, _ in all_points)
    max_x = max(step for step, _ in all_points)
    min_y = min(value for _, value in all_points)
    max_y = max(value for _, value in all_points)
    if max_x == min_x:
        max_x += 1
    if max_y == min_y:
        max_y += 1.0

    def x_pixel(value: float) -> float:
        return left + (value - min_x) / (max_x - min_x) * (width - left - right)

    def y_pixel(value: float) -> float:
        return top + (max_y - value) / (max_y - min_y) * (height - top - bottom)

    colors = ("#2563eb", "#dc2626", "#059669", "#7c3aed")
    lines = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        f'<line x1="{left}" y1="{height - bottom}" x2="{width - right}" y2="{height - bottom}" stroke="#111827"/>',
        f'<line x1="{left}" y1="{top}" x2="{left}" y2="{height - bottom}" stroke="#111827"/>',
        (
            f'<text x="{width / 2}" y="{height - 18}" text-anchor="middle" '
            'font-family="sans-serif">RL-Games iteration</text>'
        ),
        (
            f'<text x="20" y="{height / 2}" text-anchor="middle" font-family="sans-serif" '
            'transform="rotate(-90 20 270)">episode return</text>'
        ),
        f'<text x="{left}" y="{height - bottom + 22}" font-family="sans-serif" font-size="12">{min_x}</text>',
        (
            f'<text x="{width - right}" y="{height - bottom + 22}" text-anchor="end" '
            f'font-family="sans-serif" font-size="12">{max_x}</text>'
        ),
        (
            f'<text x="{left - 8}" y="{y_pixel(min_y) + 4}" text-anchor="end" '
            f'font-family="sans-serif" font-size="12">{min_y:.2f}</text>'
        ),
        (
            f'<text x="{left - 8}" y="{y_pixel(max_y) + 4}" text-anchor="end" '
            f'font-family="sans-serif" font-size="12">{max_y:.2f}</text>'
        ),
    ]
    for index, (name, curve) in enumerate(curves):
        color = colors[index % len(colors)]
        points = " ".join(f"{x_pixel(step):.2f},{y_pixel(value):.2f}" for step, value in curve)
        lines.append(f'<polyline fill="none" stroke="{color}" stroke-width="2" points="{points}"/>')
        legend_y = top + 18 * index
        lines.append(
            f'<line x1="{width - 210}" y1="{legend_y}" x2="{width - 190}" '
            f'y2="{legend_y}" stroke="{color}" stroke-width="3"/>'
        )
        lines.append(
            f'<text x="{width - 184}" y="{legend_y + 4}" font-family="sans-serif" font-size="12">{name}</text>'
        )
    lines.append("</svg>")
    output.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runs", nargs="+", type=Path, required=True)
    parser.add_argument("--metrics-dir", type=Path, required=True)
    parser.add_argument("--plots-dir", type=Path, required=True)
    parser.add_argument("--expected-runs", type=int, default=3)
    parser.add_argument("--label", required=True, help="Immutable evidence label used in output filenames.")
    args = parser.parse_args()

    args.metrics_dir.mkdir(parents=True, exist_ok=True)
    args.plots_dir.mkdir(parents=True, exist_ok=True)
    curves: list[tuple[str, list[tuple[int, float]]]] = []
    run_summaries: list[dict[str, object]] = []

    csv_path = args.metrics_dir / f"episode_returns_{args.label}.csv"
    with csv_path.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=("run", "tag", "iteration", "episode_return"))
        writer.writeheader()
        for run_dir in args.runs:
            tag, series = load_reward_series(run_dir)
            curves.append((run_dir.name, series))
            for step, value in series:
                writer.writerow({"run": run_dir.name, "tag": tag, "iteration": step, "episode_return": value})
            run_summaries.append(
                {"run": run_dir.name, "path": str(run_dir.resolve()), "tag": tag, **summarize_series(series)}
            )

    improvements = [float(run["relative_improvement"]) for run in run_summaries]
    summary = {
        "schema_version": 1,
        "generated_at": datetime.now().astimezone().isoformat(),
        "expected_runs": args.expected_runs,
        "completed_runs": len(run_summaries),
        "runs": run_summaries,
        "aggregate": {
            "median_relative_improvement": statistics.median(improvements),
            "runs_with_positive_improvement": sum(value > 0.0 for value in improvements),
        },
    }
    summary["pass"] = (
        len(run_summaries) == args.expected_runs
        and summary["aggregate"]["median_relative_improvement"] >= 0.10
        and summary["aggregate"]["runs_with_positive_improvement"] >= 2
    )
    (args.metrics_dir / f"training_summary_{args.label}.json").write_text(
        json.dumps(summary, indent=2) + "\n", encoding="utf-8"
    )
    write_svg(curves, args.plots_dir / f"episode_returns_{args.label}.svg")
    print(json.dumps(summary, indent=2))
    return 0 if summary["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
