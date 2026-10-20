"""Core components and utilities for expert users."""

from .core import Config
from .cognition import ConceptFormation, NeuralPlanner, AttentionalSelection
from .cognition import ConceptFormation, NeuralPlanner, AttentionalSelection
from .checkpoint import CheckpointManager, SnapshotManager
from .plugin import PluginManager, PluginInterface
from .monitoring import PerformanceMonitor, SystemProfiler
from .learning import LearningManager, LearningProfiler

__all__ = [
    'Config',
    'ConceptFormation',
    'NeuralPlanner', 
    'AttentionalSelection',
    'CheckpointManager',
    'SnapshotManager',
    'PluginManager',
    'PluginInterface',
    'PerformanceMonitor',
    'SystemProfiler',
    'LearningManager',
    'LearningProfiler',
]

# Note: Classes are imported from C++ bindings and re-exported
def __getattr__(name):
    if name in __all__:
        # Import from C++ bindings
        from ..bindings import (
            ConceptFormation, NeuralPlanner, AttentionalSelection,
            CheckpointManager, SnapshotManager,
            PluginManager, PluginInterface,
            PerformanceMonitor, SystemProfiler,
            LearningManager, LearningProfiler
        )
        return locals()[name]
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")