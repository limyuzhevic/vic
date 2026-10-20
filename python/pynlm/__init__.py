"""NLM (Neural Learning Machine) Python API

An experimental artificial developmental brain system for neural simulation.
This module provides Python bindings to the NLM C++ neural simulation framework,
exposing advanced neural network capabilities for expert users.

Key features:
- Memory systems (Episodic, Working, Semantic, Neural Working)
- Neuromodulation (Reward, Prediction Error, Novelty, Curiosity)
- Development processes (Synaptogenesis, Pruning, Maturation)
- Advanced processing (Neural Dynamics, Spike System, Plasticity Rules)
- Visualization interface
- Brain components (Prediction System, Concept Formation, Neural Planner)
- Advanced configuration and simulation control
- Plugin/extension system
- Performance monitoring
- Advanced learning capabilities
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "contact@nlm-project.org"
__license__ = "MIT"

# Core imports from C++ bindings
from .bindings import *

# Re-export commonly used classes
from .core import (
    Config,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

# Memory systems
from .memory import (
    EpisodicMemory,
    WorkingMemory,
    SemanticMemory,
    NeuralWorkingMemory,
)

# Neuromodulation
from .neuromodulation import (
    Neuromodulator,
    Reward,
    PredictionError,
    Novelty,
    Curiosity,
    Dopamine,
    Acetylcholine,
    Norepinephrine,
    Serotonin,
)

# Development
from .development import (
    Synaptogenesis,
    Pruning,
    Maturation,
    DevelopmentSystem,
)

# Visualization
from .visualization import VisualizationInterface

# Cognition
from .cognition import (
    ConceptFormation,
    NeuralPlanner,
    AttentionalSelection,
)

# Prediction
from .prediction import (
    PredictionSystem,
    NeuralPrediction,
    ActionConsequencePredictor,
    PredictionErrorSignal,
)

# Simulation control
from .simulation import SimulationManager, SimulationProfiler

# Checkpoint management
from .checkpoint import CheckpointManager, SnapshotManager

# Plugin system
from .plugin import PluginManager, PluginInterface

# Performance monitoring
from .monitoring import PerformanceMonitor, SystemProfiler

# Advanced learning
from .learning import LearningManager, LearningProfiler

# Configuration management
from .config import ConfigManager, AdvancedConfig, ConfigFormat

# CLI interface
from .cli import NLMCLI

# Expert-level shortcuts and utilities
from .expert import (
    AdvancedSimulationControl,
    PerformanceProfiler,
    CheckpointManager as ExpertCheckpointManager,
    LearningProfiler as ExpertLearningProfiler,
)

__all__ = [
    # Core
    'Config',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    # Memory systems
    'EpisodicMemory',
    'WorkingMemory',
    'SemanticMemory',
    'NeuralWorkingMemory',
    # Neuromodulation
    'Neuromodulator',
    'Reward',
    'PredictionError',
    'Novelty',
    'Curiosity',
    'Dopamine',
    'Acetylcholine',
    'Norepinephrine',
    'Serotonin',
    # Development
    'Synaptogenesis',
    'Pruning',
    'Maturation',
    'DevelopmentSystem',
    # Advanced processing
    'NeuralDynamics',
    'SpikeSystem',
    'PlasticityRule',
    'Hebbian',
    'STDP',
    'RewardModulatedSTDP',
    'TripletSTDP',
    'StructuralPlasticity',
    # Visualization
    'VisualizationInterface',
    # Cognition
    'ConceptFormation',
    'NeuralPlanner',
    'AttentionalSelection',
    # Prediction
    'PredictionSystem',
    'NeuralPrediction',
    'ActionConsequencePredictor',
    'PredictionErrorSignal',
    # Simulation control
    'SimulationManager',
    'SimulationProfiler',
    # Checkpoint management
    'CheckpointManager',
    'SnapshotManager',
    # Plugin system
    'PluginManager',
    'PluginInterface',
    # Performance monitoring
    'PerformanceMonitor',
    'SystemProfiler',
    # Advanced learning
    'LearningManager',
    'LearningProfiler',
    # Configuration management
    'ConfigManager',
    'AdvancedConfig',
    'ConfigFormat',
    # CLI interface
    'NLMCLI',
    # Expert utilities
    'AdvancedSimulationControl',
    'PerformanceProfiler',
    'ExpertCheckpointManager',
    'ExpertLearningProfiler',
]