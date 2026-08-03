from __future__ import annotations

import importlib.util
from pathlib import Path

import torch

MODULE_PATH = Path(__file__).parents[1] / "scripts" / "p3" / "p3_runtime.py"
SPEC = importlib.util.spec_from_file_location("p3_runtime", MODULE_PATH)
assert SPEC and SPEC.loader
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class TinyVisionAgent(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.encoder = torch.nn.Linear(3, 2)
        self.rssm = torch.nn.Linear(2, 2)


def test_vision_diagnostics_tracks_rgb_and_reset_terminal_semantics():
    agent = TinyVisionAgent()
    diagnostics = MODULE.VisionDiagnostics(agent)
    image = torch.arange(2 * 8 * 8 * 3, dtype=torch.uint8).reshape(2, 8, 8, 3)
    previous_done = torch.tensor([True, False])
    step_done = torch.tensor([False, True])
    transition = {
        "image": image,
        "is_first": previous_done.unsqueeze(-1),
        "is_last": step_done.unsqueeze(-1),
        "is_terminal": torch.tensor([[False], [True]]),
        "reward": torch.tensor([[0.0], [1.0]]),
    }

    diagnostics.observe_transition(transition, previous_done, step_done)
    result = diagnostics.result(agent)

    assert result["images"]["observations"] == 1
    assert result["images"]["shape"] == [2, 8, 8, 3]
    assert result["images"]["dtype"] == "torch.uint8"
    assert result["images"]["rgb_batch"]
    assert result["images"]["range_valid"]
    assert result["episode_semantics"]["is_first_matches_previous_done"]
    assert result["episode_semantics"]["is_last_matches_step_done"]
    assert result["episode_semantics"]["terminal_implies_last"]
    assert result["episode_semantics"]["reset_reward_zero"]
    assert result["episode_semantics"]["reset_frames"] == 1
    assert result["episode_semantics"]["terminal_frames"] == 1


def test_vision_diagnostics_tracks_r2_loss_module_gradients_actions_and_performance():
    agent = TinyVisionAgent()
    diagnostics = MODULE.VisionDiagnostics(agent)
    for parameter in agent.parameters():
        parameter.grad = torch.ones_like(parameter)

    diagnostics.observe_module_gradients(agent)
    diagnostics.observe_update({"loss/barlow": torch.tensor(2.5), "opt/loss": torch.tensor(4.0)})
    diagnostics.observe_action(torch.tensor([[-0.5], [0.75]]))
    diagnostics.observe_env_step(0.25, 2)
    diagnostics.observe_train_update(0.5)
    diagnostics.set_runtime(wall_seconds=1.0, peak_cuda_bytes=1024)
    with torch.no_grad():
        agent.encoder.weight.add_(1.0)
        agent.rssm.weight.add_(1.0)

    result = diagnostics.result(agent)

    assert result["r2_loss"]["observations"] == 1
    assert result["r2_loss"]["finite"]
    assert result["r2_loss"]["min"] == 2.5
    assert result["gradients"]["encoder"]["observations"] == 1
    assert result["gradients"]["encoder"]["finite"]
    assert result["gradients"]["rssm"]["finite"]
    assert result["updates"]["modules"]["encoder"]["changed"]
    assert result["updates"]["modules"]["rssm"]["changed"]
    assert result["actions"]["std"] > 0
    assert result["performance"]["environment_fps"] == 8.0
    assert result["performance"]["training_updates_per_second"] == 2.0
    assert result["performance"]["peak_cuda_bytes"] == 1024


def test_invalid_image_contract_is_reported_without_crashing():
    agent = TinyVisionAgent()
    diagnostics = MODULE.VisionDiagnostics(agent)
    transition = {
        "image": torch.ones(2, 3, 8, 8, dtype=torch.float32),
        "is_first": torch.zeros(2, 1, dtype=torch.bool),
        "is_last": torch.zeros(2, 1, dtype=torch.bool),
        "is_terminal": torch.zeros(2, 1, dtype=torch.bool),
        "reward": torch.ones(2, 1),
    }

    diagnostics.observe_transition(transition, torch.zeros(2, dtype=torch.bool), torch.zeros(2, dtype=torch.bool))
    result = diagnostics.result(agent)

    assert not result["images"]["rgb_batch"]
    assert not result["images"]["range_valid"]
