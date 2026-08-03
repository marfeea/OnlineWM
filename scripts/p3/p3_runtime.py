"""Runtime diagnostics for the R2-Dreamer official vision chain."""

from __future__ import annotations

import json
import math
from collections.abc import Iterable, Mapping
from pathlib import Path
from typing import Any

import torch


def values_are_finite(values: Iterable[Any]) -> bool:
    """Return whether all numeric values are finite."""
    for value in values:
        if isinstance(value, torch.Tensor):
            if not bool(torch.isfinite(value).all()):
                return False
        elif isinstance(value, int | float) and not math.isfinite(float(value)):
            return False
    return True


def _module_state(module: torch.nn.Module) -> dict[str, torch.Tensor]:
    return {name: value.detach().cpu().clone() for name, value in module.state_dict().items()}


def _module_delta(before: Mapping[str, torch.Tensor], module: torch.nn.Module) -> dict[str, float | int | bool]:
    total_square = 0.0
    max_abs = 0.0
    changed_tensors = 0
    for name, previous in before.items():
        delta = module.state_dict()[name].detach().cpu().float() - previous.float()
        if bool((delta != 0).any()):
            changed_tensors += 1
        total_square += float(torch.sum(delta**2))
        max_abs = max(max_abs, float(delta.abs().max()))
    return {
        "changed": changed_tensors > 0,
        "changed_tensors": changed_tensors,
        "delta_l2": math.sqrt(total_square),
        "delta_max_abs": max_abs,
    }


def _gradient_summary(module: torch.nn.Module) -> tuple[bool, float | None]:
    gradients = [parameter.grad for parameter in module.parameters() if parameter.grad is not None]
    if not gradients:
        return False, None
    finite = values_are_finite(gradients)
    norm = math.sqrt(sum(float(torch.sum(gradient.detach().float() ** 2).cpu()) for gradient in gradients))
    return finite and math.isfinite(norm), norm if math.isfinite(norm) else None


class VisionDiagnostics:
    """Collect machine-readable evidence for P3-G01 through P3-G07."""

    MODULES = ("encoder", "rssm")

    def __init__(self, agent: torch.nn.Module):
        self._initial = {name: _module_state(getattr(agent, name)) for name in self.MODULES}
        self.image_observations = 0
        self.image_shape: list[int] | None = None
        self.image_shape_stable = True
        self.image_dtype: str | None = None
        self.image_dtype_stable = True
        self.image_rgb_batch = True
        self.image_range_valid = True
        self.image_min = math.inf
        self.image_max = -math.inf

        self.semantic_observations = 0
        self.is_first_matches_previous_done = True
        self.is_last_matches_step_done = True
        self.terminal_implies_last = True
        self.reset_reward_zero = True
        self.reset_frames = 0
        self.terminal_frames = 0

        self.update_count = 0
        self.metrics_finite = True
        self.metric_names: set[str] = set()
        self.r2_values: list[float] = []
        self.r2_finite = True

        self.gradient_observations = {name: 0 for name in self.MODULES}
        self.gradient_finite = {name: True for name in self.MODULES}
        self.gradient_norm_min: dict[str, float | None] = {name: None for name in self.MODULES}
        self.gradient_norm_max: dict[str, float | None] = {name: None for name in self.MODULES}

        self.action_count = 0
        self.action_min = math.inf
        self.action_max = -math.inf
        self._action_sum = 0.0
        self._action_square_sum = 0.0

        self.env_steps = 0
        self.env_seconds = 0.0
        self.training_updates = 0
        self.training_seconds = 0.0
        self.wall_seconds: float | None = None
        self.peak_cuda_bytes: int | None = None
        self.training_batch_steps: int | None = None

    def observe_transition(
        self,
        transition: Mapping[str, torch.Tensor],
        previous_done: torch.Tensor,
        step_done: torch.Tensor,
    ) -> None:
        """Inspect an official IsaacLabVecEnv transition without changing it."""
        image = transition.get("image")
        if image is not None:
            self.image_observations += 1
            shape = list(image.shape)
            dtype = str(image.dtype)
            if self.image_shape is None:
                self.image_shape = shape
            else:
                self.image_shape_stable &= shape == self.image_shape
            if self.image_dtype is None:
                self.image_dtype = dtype
            else:
                self.image_dtype_stable &= dtype == self.image_dtype
            self.image_rgb_batch &= image.ndim == 4 and image.shape[-1] == 3 and image.dtype == torch.uint8
            self.image_range_valid &= image.dtype == torch.uint8
            if image.numel():
                image_min = float(image.min().cpu())
                image_max = float(image.max().cpu())
                self.image_min = min(self.image_min, image_min)
                self.image_max = max(self.image_max, image_max)
                self.image_range_valid &= image_min >= 0 and image_max <= 255

        previous_done = previous_done.bool().reshape(-1)
        step_done = step_done.bool().reshape(-1)
        is_first = transition["is_first"].bool().reshape(-1)
        is_last = transition["is_last"].bool().reshape(-1)
        is_terminal = transition["is_terminal"].bool().reshape(-1)
        reward = transition["reward"].reshape(-1)
        self.semantic_observations += 1
        self.is_first_matches_previous_done &= bool(torch.equal(is_first.cpu(), previous_done.cpu()))
        self.is_last_matches_step_done &= bool(torch.equal(is_last.cpu(), step_done.cpu()))
        self.terminal_implies_last &= not bool((is_terminal & ~is_last).any())
        if previous_done.any():
            self.reset_reward_zero &= bool((reward[previous_done] == 0).all())
        self.reset_frames += int(previous_done.sum().cpu())
        self.terminal_frames += int(step_done.sum().cpu())

    def observe_module_gradients(self, agent: torch.nn.Module) -> None:
        """Inspect encoder and RSSM gradients before clipping."""
        for name in self.MODULES:
            finite, norm = _gradient_summary(getattr(agent, name))
            self.gradient_observations[name] += 1
            self.gradient_finite[name] &= finite
            if norm is None:
                continue
            current_min = self.gradient_norm_min[name]
            current_max = self.gradient_norm_max[name]
            self.gradient_norm_min[name] = norm if current_min is None else min(current_min, norm)
            self.gradient_norm_max[name] = norm if current_max is None else max(current_max, norm)

    def observe_update(self, metrics: Mapping[str, Any]) -> None:
        """Inspect one model update and the R2 redundancy-reduction loss."""
        self.update_count += 1
        self.metric_names.update(metrics)
        self.metrics_finite &= values_are_finite(metrics.values())
        value = metrics.get("loss/barlow")
        if value is None:
            self.r2_finite = False
            return
        scalar = float(value.detach().cpu()) if isinstance(value, torch.Tensor) else float(value)
        self.r2_values.append(scalar)
        self.r2_finite &= math.isfinite(scalar)

    def observe_action(self, action: torch.Tensor) -> None:
        """Accumulate exact fixed-policy/action validity statistics."""
        values = action.detach().float()
        self.action_count += values.numel()
        self.action_min = min(self.action_min, float(values.min().cpu()))
        self.action_max = max(self.action_max, float(values.max().cpu()))
        self._action_sum += float(values.sum().cpu())
        self._action_square_sum += float((values**2).sum().cpu())

    def observe_env_step(self, seconds: float, env_count: int) -> None:
        self.env_seconds += max(0.0, float(seconds))
        self.env_steps += int(env_count)

    def observe_train_update(self, seconds: float) -> None:
        self.training_seconds += max(0.0, float(seconds))
        self.training_updates += 1

    def set_runtime(
        self,
        wall_seconds: float,
        peak_cuda_bytes: int | None,
        training_batch_steps: int | None = None,
    ) -> None:
        self.wall_seconds = float(wall_seconds)
        self.peak_cuda_bytes = int(peak_cuda_bytes) if peak_cuda_bytes is not None else None
        self.training_batch_steps = int(training_batch_steps) if training_batch_steps is not None else None

    def result(self, agent: torch.nn.Module) -> dict[str, Any]:
        """Build a JSON-safe diagnostics result."""
        if self.action_count:
            mean = self._action_sum / self.action_count
            variance = max(0.0, self._action_square_sum / self.action_count - mean**2)
            action_std = math.sqrt(variance)
            action_min: float | None = self.action_min
            action_max: float | None = self.action_max
        else:
            mean = action_std = action_min = action_max = None
        gradient_result = {
            name: {
                "observations": self.gradient_observations[name],
                "finite": self.gradient_finite[name],
                "norm_min": self.gradient_norm_min[name],
                "norm_max": self.gradient_norm_max[name],
            }
            for name in self.MODULES
        }
        return {
            "images": {
                "observations": self.image_observations,
                "shape": self.image_shape,
                "shape_stable": self.image_shape_stable,
                "dtype": self.image_dtype,
                "dtype_stable": self.image_dtype_stable,
                "rgb_batch": self.image_rgb_batch,
                "range_valid": self.image_range_valid,
                "min": self.image_min if self.image_observations else None,
                "max": self.image_max if self.image_observations else None,
            },
            "episode_semantics": {
                "observations": self.semantic_observations,
                "is_first_matches_previous_done": self.is_first_matches_previous_done,
                "is_last_matches_step_done": self.is_last_matches_step_done,
                "terminal_implies_last": self.terminal_implies_last,
                "reset_reward_zero": self.reset_reward_zero,
                "reset_frames": self.reset_frames,
                "terminal_frames": self.terminal_frames,
            },
            "r2_loss": {
                "observations": len(self.r2_values),
                "finite": self.r2_finite,
                "min": min(self.r2_values) if self.r2_values else None,
                "max": max(self.r2_values) if self.r2_values else None,
                "last": self.r2_values[-1] if self.r2_values else None,
            },
            "gradients": gradient_result,
            "updates": {
                "count": self.update_count,
                "metrics_finite": self.metrics_finite,
                "metric_names": sorted(self.metric_names),
                "modules": {name: _module_delta(self._initial[name], getattr(agent, name)) for name in self.MODULES},
            },
            "actions": {
                "count": self.action_count,
                "min": action_min,
                "max": action_max,
                "mean": mean,
                "std": action_std,
                "finite": values_are_finite(
                    value for value in (action_min, action_max, mean, action_std) if value is not None
                ),
            },
            "performance": {
                "environment_steps": self.env_steps,
                "environment_seconds": self.env_seconds,
                "environment_fps": self.env_steps / self.env_seconds if self.env_seconds > 0 else None,
                "training_updates": self.training_updates,
                "training_seconds": self.training_seconds,
                "training_updates_per_second": (
                    self.training_updates / self.training_seconds if self.training_seconds > 0 else None
                ),
                "training_fps": (
                    self.training_updates * self.training_batch_steps / self.training_seconds
                    if self.training_seconds > 0 and self.training_batch_steps is not None
                    else None
                ),
                "wall_seconds": self.wall_seconds,
                "peak_cuda_bytes": self.peak_cuda_bytes,
            },
        }


def write_json(path: Path, value: dict[str, Any]) -> None:
    """Write an indented UTF-8 JSON object without non-standard NaN values."""
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, allow_nan=False) + "\n", encoding="utf-8")
