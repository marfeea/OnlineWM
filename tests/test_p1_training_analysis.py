from __future__ import annotations

import importlib.util
from pathlib import Path

MODULE_PATH = Path(__file__).parents[1] / "scripts" / "p1" / "analyze_training.py"
SPEC = importlib.util.spec_from_file_location("p1_analyze_training", MODULE_PATH)
assert SPEC and SPEC.loader
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


def test_summarize_series_reports_robust_improvement():
    series = [(index, float(index)) for index in range(1, 51)]

    result = MODULE.summarize_series(series)

    assert result["samples"] == 50
    assert result["window"] == 10
    assert result["initial_median"] == 5.5
    assert result["final_median"] == 45.5
    assert result["absolute_improvement"] == 40.0
    assert result["relative_improvement"] > 7.0


def test_write_svg_contains_all_curve_names(tmp_path):
    output = tmp_path / "curve.svg"

    MODULE.write_svg(
        [
            ("run_one", [(1, 0.0), (2, 1.0)]),
            ("run_two", [(1, -1.0), (2, 2.0)]),
        ],
        output,
    )

    svg = output.read_text(encoding="utf-8")
    assert "<svg" in svg
    assert "run_one" in svg
    assert "run_two" in svg
