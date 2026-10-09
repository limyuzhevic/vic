#!/usr/bin/env python3
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

from .version import __version__

# Core classes
from .core import (
    Brain, Config, Action, SensoryInput, Vision, Audio, InternalSignals,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, ActionType, MotorCommand, WorldObjectType
)

# Agent and world classes  
from .agent import AgentBrain, AgentBody, SensoryPercept, Action
from .world import SimpleWorld, WorldObject

# Sensory processing
from .sensory import Vision, Audio, InternalSignals

# Motor system
from .motor import Action as MotorAction, MotorCommand

# Memory systems
from .memory import WorkingMemory, EpisodicMemory, AssociativeMemory

# Advanced systems
from .prediction import PredictionSystem, NeuralPrediction
from .cognition import NeuralPlanner, ConceptFormation, AttentionalSelection
from .neuromodulation import Dopamine, Curiosity, Novelty, PredictionError
from .development import DevelopmentSystem, DevelopmentalStage as DevStage
from .plasticity import STDP, Hebbian, StructuralPlasticity
from .dynamics import SpikeSystem, NeuralDynamics, SpikeEvent, DelayedSpikeEvent

# Experiments and utilities
from .experiments import Experiment, ExperimentRunner, Metrics
from .visualization import VisualizationInterface

# Factory functions for convenience
from .factory import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    createSensoryPercept, createAction, createWorldObject,
    createVision, createAudio, createInternalSignals,
    create_config_from_dict, create_brain_from_config_dict,
    create_agent_from_brain_world, create_simulation_config,
    create_small_world_config, create_medium_world_config, create_large_world_config,
    create_default_brain_config, create_experimental_brain_config, create_large_brain_config
)

# Utility functions
from .utils import (
    run_simulation, capture_brain_state, restore_brain_state,
    set_config_batch, validate_config, plot_neural_activity,
    benchmark_simulation, analyze_spiking_patterns,
    create_progress_callback, save_results, load_results,
    create_experiment_setup
)

# Custom exceptions
from .exceptions import NLMError, NLMConfigurationError, NLMBrainError, 
                      NLMSimulationError, NLMFileError, NLMMemoryError,
                      NLMNeuromodulationError, NLMDevelopmentError

# Performance utilities
from .performance import Profiler, MemoryTracker, ActivityRecorder

# Enum aliases for backward compatibility
NeuronType = NeuronType
SynapseType = SynapseType
DevelopmentalStage = DevelopmentalStage
ActionType = ActionType
WorldObjectType = WorldObjectType

__all__ = [
    # Core classes
    'Brain', 'Config', 'Action', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'ActionType', 'WorldObjectType',
    
    # Agent and world classes
    'AgentBrain', 'AgentBody', 'SensoryPercept', 'SimpleWorld', 'WorldObject',
    
    # Sensory processing
    'Vision', 'Audio', 'InternalSignals',
    
    # Motor system
    'MotorAction', 'MotorCommand',
    
    # Memory systems
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    
    # Advanced systems
    'PredictionSystem', 'NeuralPrediction',
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'DevelopmentSystem', 'DevStage',
    'STDP', 'Hebbian', 'StructuralPlasticity',
    'SpikeSystem', 'NeuralDynamics', 'SpikeEvent', 'DelayedSpikeEvent',
    
    # Experiments and utilities
    'Experiment', 'ExperimentRunner', 'Metrics', 'VisualizationInterface',
    
    # Factory functions
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    'createSensoryPercept', 'createAction', 'createWorldObject',
    'createVision', 'createAudio', 'createInternalSignals',
    'create_config_from_dict', 'create_brain_from_config_dict',
    'create_agent_from_brain_world', 'create_simulation_config',
    'create_small_world_config', 'create_medium_world_config', 'create_large_world_config',
    'create_default_brain_config', 'create_experimental_brain_config', 'create_large_brain_config',
    
    # Utility functions
    'run_simulation', 'capture_brain_state', 'restore_brain_state',
    'set_config_batch', 'validate_config', 'plot_neural_activity',
    'benchmark_simulation', 'analyze_spiking_patterns',
    'create_progress_callback', 'save_results', 'load_results',
    'create_experiment_setup',
    
    # Custom exceptions
    'NLMError', 'NLMConfigurationError', 'NLMBrainError',
    'NLMSimulationError', 'NLMFileError', 'NLMMemoryError',
    'NLMNeuromodulationError', 'NLMDevelopmentError',
    
    # Performance utilities
    'Profiler', 'MemoryTracker', 'ActivityRecorder',
    
    # Version
    '__version__'
]