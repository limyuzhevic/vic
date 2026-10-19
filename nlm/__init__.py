"""
NLM (Neural Learning Machine) - Python Bindings

This package provides comprehensive Python bindings for the NLM C++ neural simulation framework.
It exposes all major neural network components including Brain, Neuron, Synapse, NeuralPopulation,
NeuralRegion, Memory Systems, Plasticity Rules, Neuromodulation Systems, and more.

The bindings provide a clean, Pythonic API with snake_case naming, type hints, comprehensive
error handling, and full integration with existing C++ functionality.
"""

# Import all public API from submodules
from .core import *
from .brain import *
from .memory import *
from .cognition import *
from .sensory import *
from .motor import *
from .environment import *
from .agent import *
from .world import *
from .experiments import *
from .visualization import *
from .performance import *
from .factory import *

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Exception classes for Python error handling
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class InvalidConfigurationError(NLMError):
    """Raised when configuration is invalid."""
    pass

class SimulationError(NLMError):
    """Raised during simulation errors."""
    pass

class MemoryError(NLMError):
    """Raised during memory operations."""
    pass

# Re-export all public classes and functions
__all__ = [
    # Core utilities and types
    'Scalar', 'Vector', 'Matrix', 'MembranePotential', 'SynapticWeight',
    'FiringRate', 'Delay', 'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'Timestamp', 'SimulationStep',
    'NeuronType', 'SynapseType', 'FiringState', 'DevelopmentalStage',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Config', 'RandomGenerator', 'SimulationClock', 'Logger',
    
    # Neural components
    'Neuron', 'Synapse', 'NeuralPopulation', 'NeuralRegion', 'Brain',
    
    # Memory systems
    'MemorySystem', 'WorkingMemory', 'EpisodicMemory',
    'SemanticMemory', 'ProceduralMemory', 'AssociativeMemory',
    'NeuralWorkingMemory', 'NeuralEpisodicMemory',
    
    # Plasticity rules
    'PlasticityRule', 'STDP', 'Hebbian', 'StructuralPlasticity',
    
    # Neuromodulation
    'Neuromodulator', 'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'Reward',
    
    # Development system
    'DevelopmentSystem', 'Synaptogenesis', 'Pruning', 'Maturation',
    
    # Dynamics
    'NeuralDynamics', 'SpikeSystem',
    
    # Cognition
    'ConceptFormation', 'NeuralPlanner', 'AttentionalSelection',
    
    # Sensory
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    
    # Prediction
    'PredictionSystem', 'NeuralPrediction',
    
    # Motor
    'MotorSystem', 'Action',
    
    # Environment
    'Environment', 'Observation',
    
    # World
    'WorldObject', 'SimpleWorld',
    
    # Agent
    'AgentBody', 'SensoryPercept', 'AgentBrain',
    
    # Experiment
    'Experiment', 'ExperimentRunner', 'Metrics',
    
    # Visualization
    'VisualizationInterface',
    
    # Performance
    'MemoryPool', 'EventQueue', 'ParallelProcessing',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain',
    
    # Exceptions
    'NLMError', 'InvalidConfigurationError', 'SimulationError', 'MemoryError',
]
