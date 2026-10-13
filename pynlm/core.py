"""Core NLM modules and functionality."""

from .config import Config, ConfigError
from .brain import (
    Brain, Neuron, Synapse, NeuralRegion, NeuralPopulation,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, FiringState, DevelopmentalStage,
    ActionType, MotorCommand, WorldObjectType
)
from .neuromodulation import Dopamine, Curiosity, Novelty, PredictionError
from .memory import (
    WorkingMemory, EpisodicMemory, AssociativeMemory,
    NeuralWorkingMemory, NeuralEpisodicMemory
)
from .cognition import NeuralPlanner, ConceptFormation, AttentionalSelection
from .prediction import PredictionSystem, NeuralPrediction
from .development import DevelopmentSystem, Synaptogenesis, Pruning, Maturation
from .plasticity import STDP, Hebbian, StructuralPlasticity, PlasticityRule
from .sensory import SensoryInput, Vision, Audio, InternalSignals
from .motor import Action, MotorSystem
from .agent import AgentBrain, AgentBody, SensoryPercept
from .world import SimpleWorld, WorldObject
from .environment import Environment, Observation
from .dynamics import NeuralDynamics, SpikeSystem
from .core import SimulationClock, RandomGenerator, Logger

# Factory functions
def createDefaultConfig():
    """Create a default NLM configuration."""
    return Config()

def createBrain(config):
    """Create a new brain with configuration."""
    return Brain(config)

def createAgentBrain(brain):
    """Create a new agent brain interface."""
    return AgentBrain(brain)

def createSimpleWorld():
    """Create a new simple world."""
    return SimpleWorld()

def createAction(action_type=None, parameters=None):
    """Create a new action."""
    if parameters is None:
        parameters = []
    if action_type is None:
        action_type = ActionType.Wait
    return Action(action_type, parameters)

def createWorldObject(x, y, obj_type, value=0.0, radius=0.5):
    """Create a new world object."""
    return WorldObject(x, y, obj_type, value, radius)

# Constants
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

# Utility functions
__all__ = [
    # Classes
    'Config', 'ConfigError',
    'Brain', 'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'FiringState', 'DevelopmentalStage',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    'NeuralWorkingMemory', 'NeuralEpisodicMemory',
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    'PredictionSystem', 'NeuralPrediction',
    'DevelopmentSystem', 'Synaptogenesis', 'Pruning', 'Maturation',
    'STDP', 'Hebbian', 'StructuralPlasticity', 'PlasticityRule',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'MotorSystem',
    'AgentBrain', 'AgentBody', 'SensoryPercept',
    'SimpleWorld', 'WorldObject',
    'Environment', 'Observation',
    'NeuralDynamics', 'SpikeSystem',
    'SimulationClock', 'RandomGenerator', 'Logger',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createAgentBrain',
    'createSimpleWorld', 'createAction', 'createWorldObject',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Version and info
    'getVersionInfo', 'validateConfig',
]

def getVersionInfo():
    """Get detailed version information for NLM."""
    return {
        'version': '0.1.0',
        'author': 'NLM Authors',
        'description': 'Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework',
        'python_version': '3.8+',
        'cpp_standard': 'C++20',
        'platform': 'Linux'
    }

def validateConfig(config):
    """Validate a configuration object."""
    if config is None:
        raise ValueError("Config cannot be None")
    return True