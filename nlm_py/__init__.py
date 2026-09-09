"""
NLM (Neural Learning Machine) Python Bindings

A Python binding for the NLM C++ neural simulation framework.
Provides classes for Brain, Neuron, Synapse, NeuralRegion, NeuralPopulation,
and all major brain systems including neuromodulation, memory, prediction,
and cognition systems.

This module exposes the full C++ neural simulation framework in an
idiomatic Python interface while maintaining access to the full C++ power.
"""

import sys
from importlib import resources

# Version
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Core modules
from .core import *
from .brain import *
from .neuromodulation import *
from .memory import *
from .prediction import *
from .cognition import *
from .sensory import *
from .motor import *
from .environment import *
from .agent import *
from .experiments import *
from .visualization import *

# Convenience functions
from .convenience import *

# Exceptions
from .exceptions import *

# Main entry point
def get_version():
    """Get the version of the NLM Python bindings."""
    return __version__

def create_default_config():
    """Create a default configuration object."""
    from .core import Config
    return Config()

def create_brain(config=None):
    """Create a new brain with the given configuration.
    
    Args:
        config: Optional configuration. If None, a default config is created.
    
    Returns:
        A Brain object.
    """
    from .brain import Brain
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world():
    """Create a new SimpleWorld simulation environment."""
    from .environment import SimpleWorld
    return SimpleWorld()

def create_agent_brain(brain):
    """Create an agent brain interface for a brain object.
    
    Args:
        brain: A Brain object.
    
    Returns:
        An AgentBrain object.
    """
    from .agent import AgentBrain
    return AgentBrain(brain)

# Build module
if hasattr(sys, 'real_prefix') or hasattr(sys, 'base_prefix'):
    # We're in a virtual environment, do nothing special
    pass
else:
    # Normal Python environment, ensure we have the C++ extension loaded
    try:
        import nlm_py._core
    except ImportError:
        pass

__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Numeric types
    'SimulationStep', 'Timestamp', 'TimestepDuration',
    'NeuronIndex', 'SynapseIndex', 'PopulationIndex', 'RegionIndex',
    'SynapticWeight', 'MembranePotential', 'FiringRate', 'Delay',
    
    # Core classes
    'Config', 'RandomGenerator', 'SimulationClock', 'Logger',
    
    # Brain components
    'Brain', 'NeuralRegion', 'NeuralPopulation', 'Neuron', 'Synapse',
    
    # Systems
    'SpikeSystem', 'STDP', 'Hebbian', 'StructuralPlasticity',
    'Neuromodulator', 'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    
    # Memory systems
    'NeuralWorkingMemory', 'NeuralEpisodicMemory', 'NeuralAssociativeMemory',
    
    # Prediction system
    'PredictionSystem', 'NeuralPrediction',
    
    # Cognition systems
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    
    # Development system
    'DevelopmentSystem', 'Synaptogenesis', 'Pruning', 'Maturation',
    
    # Sensory systems
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    
    # Motor system
    'MotorSystem', 'Action',
    
    # Agent systems
    'AgentBrain', 'AgentBody', 'SensoryPercept', 'ActionResult',
    
    # Environment
    'SimpleWorld', 'WorldObject', 'Observation', 'Environment',
    
    # Experiments
    'Experiment', 'ExperimentRunner', 'Phase6IntegratedExperiment',
    
    # Visualization
    'VisualizationInterface',
    
    # Exceptions
    'NLMError', 'ConfigError', 'BrainError', 'NeuronError',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Convenience functions
    'run_simulation', 'create_default_config', 'create_brain',
    'create_simple_world', 'create_agent_brain', 'get_version',
    
    # Convenience functions
    'format_spike_statistics', 'format_memory_stats', 'format_prediction_stats',
    'create_custom_config', 'create_neural_config', 'create_agent_config',
]