"""Cognition system classes for expert users."""

from .core import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SimulationStep, Timestamp, MembranePotential
from .plasticity import PlasticityRule
from .neuromodulation import Neuromodulator

__all__ = [
    'ConceptFormation',
    'NeuralPlanner',
    'AttentionalSelection',
]

# Note: Classes are imported from C++ bindings and re-exported
def __getattr__(name):
    if name in __all__:
        # Import from C++ bindings
        from ..bindings import (
            ConceptFormation, NeuralPlanner, AttentionalSelection
        )
        return locals()[name]
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")