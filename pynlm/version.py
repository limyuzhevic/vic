"""
NLM (Neural Learning Machine) - Python Bindings
================================================

A Python interface to the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld,
SensoryInput, and Action, plus enhanced functionality for
power users and scientific computing.

Key Features:
- Complete Python API for all NLM functionality
- Pythonic interfaces with object-oriented patterns
- Enhanced error handling and validation
- Convenience methods for common operations
- Type hints for better IDE support
- Comprehensive documentation and examples
"""

__version__ = "0.1.0"

# Core classes
from .core import (
    Brain, Config, Action, SensoryInput, Vision, Audio, InternalSignals,
    NeuronId, SynapseId, RegionId, PopulationId
)

# Agent and world classes  
from .agent import AgentBrain, AgentBody, SensoryPercept
from .world import SimpleWorld, WorldObject

# Sensory processing
from .sensory import Vision, Audio, InternalSignals

# Motor system
from .motor import Action, MotorCommand

# Memory systems
from .memory import WorkingMemory, EpisodicMemory, AssociativeMemory

# Advanced systems
from .prediction import PredictionSystem, NeuralPrediction
from .cognition import NeuralPlanner, ConceptFormation, AttentionalSelection
from .neuromodulation import Dopamine, Curiosity, Novelty, PredictionError
from .development import DevelopmentSystem, DevelopmentalStage
from .plasticity import STDP, Hebbian, StructuralPlasticity
from .dynamics import SpikeSystem, NeuralDynamics

# Experiments and utilities
from .experiments import Experiment, ExperimentRunner
from .visualization import VisualizationInterface

# Factory functions for convenience
from .factory import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    createSensoryPercept, createAction, createWorldObject
)

# Utility functions
from .utils import (
    run_simulation, capture_brain_state, restore_brain_state,
    set_config_batch, validate_config, plot_neural_activity,
    benchmark_simulation, analyze_spiking_patterns
)

# Custom exceptions
from .exceptions import NLMError, NLMConfigurationError, NLMBrainError, 
                      NLMSimulationError, NLMFileError

# Performance utilities
from .performance import Profiler, MemoryTracker, ActivityRecorder

__all__ = [
    # Core classes
    'Brain', 'Config', 'Action', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Agent and world classes
    'AgentBrain', 'AgentBody', 'SensoryPercept', 'SimpleWorld', 'WorldObject',
    
    # Sensory processing
    'Vision', 'Audio', 'InternalSignals',
    
    # Motor system
    'Action', 'MotorCommand',
    
    # Memory systems
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    
    # Advanced systems
    'PredictionSystem', 'NeuralPrediction',
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'DevelopmentSystem', 'DevelopmentalStage',
    'STDP', 'Hebbian', 'StructuralPlasticity',
    'SpikeSystem', 'NeuralDynamics',
    
    # Experiments and utilities
    'Experiment', 'ExperimentRunner', 'VisualizationInterface',
    
    # Factory functions
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    'createSensoryPercept', 'createAction', 'createWorldObject',
    
    # Utility functions
    'run_simulation', 'capture_brain_state', 'restore_brain_state',
    'set_config_batch', 'validate_config', 'plot_neural_activity',
    'benchmark_simulation', 'analyze_spiking_patterns',
    
    # Custom exceptions
    'NLMError', 'NLMConfigurationError', 'NLMBrainError',
    'NLMSimulationError', 'NLMFileError',
    
    # Performance utilities
    'Profiler', 'MemoryTracker', 'ActivityRecorder',
    
    # Version
    '__version__'
]
