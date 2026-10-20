"""Neuromodulation classes for expert users."""

from .core import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SimulationStep, Timestamp, MembranePotential
from .plasticity import PlasticityRule
from .neuromodulation import Neuromodulator

__all__ = [
    'Reward',
    'PredictionError',
    'Novelty',
    'Curiosity',
    'Dopamine',
    'Acetylcholine',
    'Norepinephrine',
    'Serotonin',
]

# Note: Classes are imported from C++ bindings and re-exported
def __getattr__(name):
    if name in __all__:
        # Import from C++ bindings
        from ..bindings import (
            Reward, PredictionError, Novelty, Curiosity, Dopamine, 
            Acetylcholine, Norepinephrine, Serotonin
        )
        return locals()[name]
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")