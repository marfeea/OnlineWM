"""Probe the official Isaac Lab Cartpole environment for the P1 gates."""

from __future__ import annotations

import argparse
import json
import math
from datetime import datetime
from pathlib import Path

from isaaclab.app import AppLauncher

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--task", default="Isaac-Cartpole-Direct-v0")
parser.add_argument("--num_envs", type=int, default=16)
parser.add_argument("--steps", type=int, default=640)
parser.add_argument("--output", type=Path, required=True)
AppLauncher.add_app_launcher_args(parser)
args_cli = parser.parse_args()

app_launcher = AppLauncher(args_cli)
simulation_app = app_launcher.app

import gymnasium as gym
import torch

import isaaclab_tasks  # noqa: F401
from isaaclab_tasks.utils.parse_cfg import load_cfg_from_registry


def _policy_observation(observations: object) -> torch.Tensor:
    if isinstance(observations, dict):
        observations = observations.get("policy", next(iter(observations.values())))
    if not isinstance(observations, torch.Tensor):
        raise TypeError(f"Expected tensor observations, got {type(observations).__name__}")
    return observations


def main() -> int:
    """Run reset and random steps, then save machine-readable evidence."""
    env_cfg = load_cfg_from_registry(args_cli.task, "env_cfg_entry_point")
    env_cfg.scene.num_envs = args_cli.num_envs
    env_cfg.sim.device = args_cli.device or env_cfg.sim.device
    env = gym.make(args_cli.task, cfg=env_cfg)

    try:
        observations, reset_info = env.reset()
        policy_observations = _policy_observation(observations)
        previous_observations = policy_observations.clone()
        reward_means: list[float] = []
        reward_mins: list[float] = []
        reward_maxs: list[float] = []
        observation_deltas: list[float] = []
        terminated_counts: list[int] = []
        truncated_counts: list[int] = []

        generator = torch.Generator(device=env.unwrapped.device)
        generator.manual_seed(20260725)
        action_shape = (env.unwrapped.num_envs, *env.unwrapped.single_action_space.shape)

        for _ in range(args_cli.steps):
            actions = (
                2.0
                * torch.rand(
                    action_shape,
                    device=env.unwrapped.device,
                    generator=generator,
                )
                - 1.0
            )
            observations, rewards, terminated, truncated, _ = env.step(actions)
            policy_observations = _policy_observation(observations)
            reward_means.append(float(rewards.mean().item()))
            reward_mins.append(float(rewards.min().item()))
            reward_maxs.append(float(rewards.max().item()))
            observation_deltas.append(float((policy_observations - previous_observations).abs().max().item()))
            terminated_counts.append(int(terminated.sum().item()))
            truncated_counts.append(int(truncated.sum().item()))
            previous_observations = policy_observations.clone()

        all_finite = all(
            math.isfinite(value)
            for series in (reward_means, reward_mins, reward_maxs, observation_deltas)
            for value in series
        )
        result = {
            "schema_version": 1,
            "collected_at": datetime.now().astimezone().isoformat(),
            "task": args_cli.task,
            "num_envs": env.unwrapped.num_envs,
            "device": str(env.unwrapped.device),
            "steps": args_cli.steps,
            "reset_info_keys": sorted(reset_info),
            "observation_shape": list(policy_observations.shape),
            "action_shape": list(action_shape),
            "reward": {
                "step_mean": reward_means,
                "overall_min": min(reward_mins),
                "overall_max": max(reward_maxs),
                "mean_range": max(reward_means) - min(reward_means),
            },
            "terminated": {
                "per_step": terminated_counts,
                "total": sum(terminated_counts),
                "steps_with_events": sum(count > 0 for count in terminated_counts),
            },
            "truncated": {
                "per_step": truncated_counts,
                "total": sum(truncated_counts),
                "steps_with_events": sum(count > 0 for count in truncated_counts),
            },
            "observation": {
                "max_step_delta": max(observation_deltas),
                "steps_changed": sum(delta > 0.0 for delta in observation_deltas),
            },
            "checks": {
                "parallel_environment_created": env.unwrapped.num_envs == args_cli.num_envs,
                "all_values_finite": all_finite,
                "reward_updated": max(reward_means) > min(reward_means),
                "observation_updated": any(delta > 0.0 for delta in observation_deltas),
                "termination_or_truncation_observed": (sum(terminated_counts) + sum(truncated_counts)) > 0,
            },
        }
        result["pass"] = all(result["checks"].values())
        args_cli.output.parent.mkdir(parents=True, exist_ok=True)
        args_cli.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({**result["checks"], "output": str(args_cli.output)}, indent=2))
        return 0 if result["pass"] else 1
    finally:
        env.close()


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    finally:
        simulation_app.close()
