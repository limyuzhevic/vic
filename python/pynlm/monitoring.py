"""Performance monitoring for expert users."""

from .core import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SimulationStep, Timestamp, MembranePotential
from .plasticity import PlasticityRule
from .neuromodulation import Neuromodulator

__all__ = [
    'PerformanceMonitor',
    'SystemProfiler',
]

# Note: Classes are imported from C++ bindings and re-exported
def __getattr__(name):
    if name in __all__:
        # Import from C++ bindings
        from ..bindings import (
            PerformanceMonitor, SystemProfiler
        )
        return locals()[name]
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")