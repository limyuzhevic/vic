"""Prediction system classes for expert users."""

from .core import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SimulationStep, Timestamp, MembranePotential
from .plasticity import PlasticityRule
from .neuromodulation import Neuromodulator

__all__ = [
    'PredictionSystem',
    'NeuralPrediction',
    'ActionConsequencePredictor',
    'PredictionErrorSignal',
]

# Note: Classes are imported from C++ bindings and re-exported
def __getattr__(name):
    if name in __all__:
        # Import from C++ bindings
        from ..bindings import (
            PredictionSystem, NeuralPrediction, ActionConsequencePredictor, PredictionErrorSignal
        )
        return locals()[name]
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")