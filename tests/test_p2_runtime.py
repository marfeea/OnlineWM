from __future__ import annotations

import importlib.util
from pathlib import Path

import torch

MODULE_PATH = Path(__file__).parents[1] / "scripts" / "p2" / "p2_runtime.py"
SPEC = importlib.util.spec_from_file_location("p2_runtime", MODULE_PATH)
assert SPEC and SPEC.loader
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class TinyAgent(torch.nn.Module):
    def __init__(self):
        super().__init__()
        for name in MODULE.ChainDiagnostics.MODULES:
            setattr(self, name, torch.nn.Linear(2, 2))


def test_tensor_state_digest_is_stable_and_sensitive():
    state = {"weight": torch.arange(4, dtype=torch.float32).reshape(2, 2)}
    same = {"weight": state["weight"].clone()}
    changed = {"weight": state["weight"].clone()}
    changed["weight"][0, 0] = 10

    assert MODULE.tensor_state_digest(state) == MODULE.tensor_state_digest(same)
    assert MODULE.tensor_state_digest(state) != MODULE.tensor_state_digest(changed)


def test_diagnostics_detects_strict_contiguous_replay_and_module_changes():
    agent = TinyAgent()
    diagnostics = MODULE.ChainDiagnostics(agent)
    data = {
        "episode": torch.tensor([[3, 3, 3], [8, 8, 8]]),
        "is_first": torch.zeros(2, 3, 1, dtype=torch.bool),
        "stoch": torch.zeros(2, 3, 2),
        "deter": torch.zeros(2, 3, 2),
    }
    index = [
        torch.tensor([[0, 0, 0], [1, 1, 1]]),
        torch.tensor([[4, 5, 6], [9, 10, 11]]),
    ]
    diagnostics.observe_replay(data, index, (torch.zeros(2, 2), torch.zeros(2, 2)))
    diagnostics.observe_action(torch.tensor([[-0.5, 0.5], [0.25, -0.25]]))
    for parameter in agent.parameters():
        parameter.grad = torch.ones_like(parameter)
    diagnostics.observe_gradients(agent.parameters())
    diagnostics.observe_update({"loss": torch.tensor(1.0)})
    with torch.no_grad():
        for name in MODULE.ChainDiagnostics.MODULES:
            getattr(agent, name).weight.add_(1.0)

    result = diagnostics.result(agent)

    assert result["replay"]["episode_constant_per_sequence"]
    assert result["replay"]["no_internal_is_first"]
    assert result["replay"]["time_index_contiguous"]
    assert result["latents"]["finite"]
    assert result["gradients"]["finite"]
    assert result["actions"]["std"] > 0
    assert all(module["changed"] for module in result["updates"]["modules"].values())
