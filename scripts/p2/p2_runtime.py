"""P2 runtime adapters for the frozen R2-Dreamer checkout."""

from __future__ import annotations

import hashlib
import json
import math
from collections.abc import Iterable
from pathlib import Path
from typing import Any

import torch


def tensor_state_digest(state_dict: dict[str, torch.Tensor]) -> str:
    """Return a stable SHA256 for a tensor state dict."""
    digest = hashlib.sha256()
    for name, value in sorted(state_dict.items()):
        tensor = value.detach().cpu().contiguous()
        digest.update(name.encode("utf-8"))
        digest.update(str(tensor.dtype).encode("ascii"))
        digest.update(str(tuple(tensor.shape)).encode("ascii"))
        digest.update(tensor.numpy().tobytes())
    return digest.hexdigest()


def values_are_finite(values: Iterable[Any]) -> bool:
    """Return whether all numeric scalar/tensor values are finite."""
    for value in values:
        if isinstance(value, torch.Tensor):
            if not bool(torch.isfinite(value).all()):
                return False
        elif isinstance(value, int | float) and not math.isfinite(float(value)):
            return False
    return True


class ChainDiagnostics:
    """Collect machine-readable evidence for the P2 gates."""

    MODULES = ("rssm", "reward", "cont", "actor", "value")

    def __init__(self, agent: torch.nn.Module):
        self._initial = {
            name: {key: value.detach().cpu().clone() for key, value in getattr(agent, name).state_dict().items()}
            for name in self.MODULES
        }
        self.replay_samples = 0
        self.replay_episode_constant = True
        self.replay_no_internal_first = True
        self.replay_time_contiguous = True
        self.replay_shapes: list[list[int]] = []
        self.latent_observations = 0
        self.latent_finite = True
        self.gradient_observations = 0
        self.gradient_finite = True
        self.gradient_norm_min: float | None = None
        self.gradient_norm_max: float | None = None
        self.update_count = 0
        self.metrics_finite = True
        self.metric_names: set[str] = set()
        self.action_count = 0
        self.action_min = math.inf
        self.action_max = -math.inf
        self._action_sum = 0.0
        self._action_square_sum = 0.0

    def observe_replay(self, data: Any, index: list[torch.Tensor], initial: tuple[torch.Tensor, torch.Tensor]) -> None:
        """Inspect one replay sample without changing it."""
        self.replay_samples += 1
        episode = data["episode"]
        self.replay_shapes.append(list(episode.shape))
        if episode.ndim >= 2 and episode.shape[1] > 1:
            self.replay_episode_constant &= bool((episode[:, 1:] == episode[:, :-1]).all())
        is_first = data.get("is_first")
        if is_first is not None and is_first.ndim >= 2 and is_first.shape[1] > 1:
            self.replay_no_internal_first &= not bool(is_first[:, 1:].any())

        contiguous_dimension_found = False
        for component in index:
            if component.ndim < 2 or component.shape[1] < 2:
                continue
            delta = component[:, 1:] - component[:, :-1]
            if bool((delta == 1).all()):
                contiguous_dimension_found = True
        self.replay_time_contiguous &= contiguous_dimension_found

        latent_values = [*initial]
        for key in ("stoch", "deter"):
            if key in data:
                latent_values.append(data[key])
        self.latent_observations += 1
        self.latent_finite &= values_are_finite(latent_values)

    def observe_gradients(self, parameters: Iterable[torch.nn.Parameter]) -> None:
        """Inspect gradients immediately before clipping and optimizer step."""
        grads = [parameter.grad for parameter in parameters if parameter.grad is not None]
        self.gradient_observations += 1
        self.gradient_finite &= bool(grads) and values_are_finite(grads)
        if not grads:
            return
        norm = math.sqrt(sum(float(torch.sum(grad.detach().float() ** 2).cpu()) for grad in grads))
        if not math.isfinite(norm):
            return
        self.gradient_norm_min = norm if self.gradient_norm_min is None else min(self.gradient_norm_min, norm)
        self.gradient_norm_max = norm if self.gradient_norm_max is None else max(self.gradient_norm_max, norm)

    def observe_update(self, metrics: dict[str, Any]) -> None:
        """Inspect update metrics."""
        self.update_count += 1
        self.metric_names.update(metrics)
        self.metrics_finite &= values_are_finite(metrics.values())

    def observe_action(self, action: torch.Tensor) -> None:
        """Accumulate exact action range and variance statistics."""
        values = action.detach().float()
        self.action_count += values.numel()
        self.action_min = min(self.action_min, float(values.min().cpu()))
        self.action_max = max(self.action_max, float(values.max().cpu()))
        self._action_sum += float(values.sum().cpu())
        self._action_square_sum += float((values**2).sum().cpu())

    def module_deltas(self, agent: torch.nn.Module) -> dict[str, dict[str, float | bool]]:
        """Compare final module parameters with the post-load initial state."""
        result = {}
        for name in self.MODULES:
            total_square = 0.0
            max_abs = 0.0
            changed_tensors = 0
            current = getattr(agent, name).state_dict()
            for key, before in self._initial[name].items():
                delta = current[key].detach().cpu().float() - before.float()
                if bool((delta != 0).any()):
                    changed_tensors += 1
                total_square += float(torch.sum(delta**2))
                max_abs = max(max_abs, float(delta.abs().max()))
            result[name] = {
                "changed": changed_tensors > 0,
                "changed_tensors": changed_tensors,
                "delta_l2": math.sqrt(total_square),
                "delta_max_abs": max_abs,
            }
        return result

    def result(self, agent: torch.nn.Module) -> dict[str, Any]:
        """Build the serializable diagnostics result."""
        if self.action_count:
            mean = self._action_sum / self.action_count
            variance = max(0.0, self._action_square_sum / self.action_count - mean**2)
            action_std = math.sqrt(variance)
            action_min = self.action_min
            action_max = self.action_max
        else:
            mean = action_std = action_min = action_max = math.nan
        return {
            "replay": {
                "samples": self.replay_samples,
                "sample_shapes": self.replay_shapes,
                "episode_constant_per_sequence": self.replay_episode_constant,
                "no_internal_is_first": self.replay_no_internal_first,
                "time_index_contiguous": self.replay_time_contiguous,
            },
            "latents": {
                "observations": self.latent_observations,
                "finite": self.latent_finite,
            },
            "gradients": {
                "observations": self.gradient_observations,
                "finite": self.gradient_finite,
                "norm_min": self.gradient_norm_min,
                "norm_max": self.gradient_norm_max,
            },
            "updates": {
                "count": self.update_count,
                "metrics_finite": self.metrics_finite,
                "metric_names": sorted(self.metric_names),
                "modules": self.module_deltas(agent),
            },
            "actions": {
                "count": self.action_count,
                "min": action_min,
                "max": action_max,
                "mean": mean,
                "std": action_std,
                "finite": values_are_finite((action_min, action_max, mean, action_std)),
            },
        }


class StrictEpisodeTrainer:
    """Online trainer that gives every physical episode a unique replay trajectory ID.

    R2-Dreamer's upstream trainer deliberately uses one trajectory ID per
    environment stream. That permits sampled sequences to cross resets. P2-G01
    explicitly forbids this, so this adapter keeps the upstream update/logging
    behavior while assigning a fresh ID to every post-reset transition.
    """

    def __init__(self, upstream_trainer: Any, diagnostics: ChainDiagnostics):
        self._trainer = upstream_trainer
        self.diagnostics = diagnostics
        self.final_step = 0
        self.skipped_updates_no_sequence = 0
        self.episode_count = 0

    def begin(self, agent: Any) -> None:
        """Run the upstream online loop with strict episode segmentation."""
        trainer = self._trainer
        envs = trainer.train_envs
        replay_buffer = trainer.replay_buffer
        logger = trainer.logger
        video_cache = []
        step = replay_buffer.count() * trainer._action_repeat
        update_count = 0
        done = torch.ones(envs.env_num, dtype=torch.bool, device=agent.device)
        returns = torch.zeros(envs.env_num, dtype=torch.float32, device=agent.device)
        lengths = torch.zeros(envs.env_num, dtype=torch.int32, device=agent.device)
        episode_ids = torch.arange(envs.env_num, dtype=torch.int64, device=agent.device)
        next_episode_id = envs.env_num
        first_step = True
        train_metrics: dict[str, Any] = {}
        agent_state = agent.get_initial_state(envs.env_num)
        act = agent_state["prev_action"].clone()

        while step < trainer.steps:
            if trainer._should_eval(step) and trainer.eval_episode_num > 0 and trainer.eval_envs is not None:
                trainer.eval(agent, step)
            if done.any():
                for index, is_done in enumerate(done):
                    if is_done and lengths[index] > 0:
                        if index == 0 and video_cache:
                            video = torch.stack(video_cache, axis=0)
                            logger.video("train_video", trainer.tools.to_np(video[None]))
                            video_cache = []
                        logger.scalar("episode/score", returns[index])
                        logger.scalar("episode/length", lengths[index])
                        logger.write(step + index)
                        returns[index] = lengths[index] = 0
                        self.episode_count += 1

            step += int((~done).sum()) * trainer._action_repeat
            lengths += ~done
            reset_mask = done.clone()
            trans, step_done = envs.step(act.detach(), done)
            trans = trans.to(agent.device, non_blocking=True)
            done = step_done.to(agent.device)
            act, agent_state = agent.act(trans.clone(), agent_state, eval=False)
            act = act.clamp(-1.0, 1.0)
            agent_state["prev_action"] = act
            self.diagnostics.observe_action(act)

            if not first_step and reset_mask.any():
                reset_indices = reset_mask.nonzero(as_tuple=False).squeeze(-1)
                new_ids = torch.arange(
                    next_episode_id,
                    next_episode_id + reset_indices.numel(),
                    dtype=episode_ids.dtype,
                    device=episode_ids.device,
                )
                episode_ids[reset_indices] = new_ids
                next_episode_id += reset_indices.numel()
            first_step = False

            trans["action"] = act * ~done.unsqueeze(-1)
            trans["stoch"] = agent_state["stoch"]
            trans["deter"] = agent_state["deter"]
            trans["episode"] = episode_ids
            if "image" in trans:
                video_cache.append(trans["image"][0])
            replay_buffer.add_transition(trans.detach())
            returns += trans["reward"][:, 0]

            if step // (envs.env_num * trainer._action_repeat) > trainer.batch_length + 1:
                update_num = trainer.pretrain if trainer._should_pretrain() else trainer._updates_needed(step)
                completed_updates = 0
                for _ in range(update_num):
                    try:
                        train_metrics = agent.update(replay_buffer)
                    except RuntimeError as error:
                        message = str(error).lower()
                        if "sample" not in message and "slice" not in message and "trajectory" not in message:
                            raise
                        self.skipped_updates_no_sequence += 1
                        break
                    completed_updates += 1
                update_count += completed_updates
                if train_metrics and trainer._should_log(step):
                    for name, value in train_metrics.items():
                        value = trainer.tools.to_np(value) if isinstance(value, torch.Tensor) else value
                        logger.scalar(f"train/{name}", value)
                    logger.scalar("train/opt/updates", update_count)
                    logger.write(step, fps=True)

        self.final_step = step
        if train_metrics:
            for name, value in train_metrics.items():
                value = trainer.tools.to_np(value) if isinstance(value, torch.Tensor) else value
                logger.scalar(f"train/{name}", value)
            logger.scalar("train/opt/updates", update_count)
            logger.write(step, fps=True)


def write_json(path: Path, value: dict[str, Any]) -> None:
    """Write indented UTF-8 JSON, creating parents."""
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, allow_nan=False) + "\n", encoding="utf-8")
