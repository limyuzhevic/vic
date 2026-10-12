import sys
import numpy as np
from enum import Enum

# Version information
__version__ = "0.1.0"

# Core enumerations
class NeuronType(Enum):
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType(Enum):
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage(Enum):
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class FiringState(Enum):
    Resting = 0
    Depolarizing = 1
    Firing = 2
    Repolarizing = 3

class ActionType(Enum):
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookUp = 4
    LookDown = 5
    Attack = 6
    Defend = 7
    Eat = 8
    Drink = 9

class WorldObjectType(Enum):
    Empty = 0
    Resource = 1
    Obstacle = 2
    Predator = 3
    Prey = 4
    Shelter = 5
    Water = 6
    Food = 7

# Core ID types
class NeuronId:
    def __init__(self, value):
        self.value = int(value)
    
    def __repr__(self):
        return f"NeuronId({self.value})"
    
    def __eq__(self, other):
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

class SynapseId:
    def __init__(self, value):
        self.value = int(value)
    
    def __repr__(self):
        return f"SynapseId({self.value})"
    
    def __eq__(self, other):
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

class RegionId:
    def __init__(self, value):
        self.value = int(value)
    
    def __repr__(self):
        return f"RegionId({self.value})"
    
    def __eq__(self, other):
        return isinstance(other, RegionId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

class PopulationId:
    def __init__(self, value):
        self.value = int(value)
    
    def __repr__(self):
        return f"PopulationId({self.value})"
    
    def __eq__(self, other):
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

class ActionId:
    def __init__(self, value):
        self.value = int(value)
    
    def __repr__(self):
        return f"ActionId({self.value})"
    
    def __eq__(self, other):
        return isinstance(other, ActionId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

# Forward declarations for type hints
class Config:
    pass

class Brain:
    pass

class AgentBrain:
    pass

class SimpleWorld:
    pass

class Synapse:
    pass

class NeuralPopulation:
    pass

class NeuralRegion:
    pass

class SensoryInput:
    pass

class Vision:
    pass

class Audio:
    pass

class InternalSignals:
    pass

class Action:
    pass

class WorldObject:
    pass

class AgentBody:
    pass

class ActionResult:
    pass

class SensoryPercept:
    pass

class ActionResult:
    pass

# Factory functions
def createDefaultConfig():
    """Create a default configuration for NLM."""
    config = Config()
    # Initialize with default values
    config._values = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "brain.connection_probability": 0.1,
        "plasticity.enabled": True,
        "development.critical_period": 0.5,
        "development.maturation_rate": 0.8,
        "neuromodulation.reward_scale": 1.0,
        "neuromodulation.prediction_error_scale": 1.0,
        "memory.working_memory_size": 100,
        "memory.episodic_memory_size": 1000,
    }
    return config

def createBrain(config):
    """Create a brain from a configuration."""
    brain = Brain()
    brain._config = config
    brain._neurons = []
    brain._synapses = []
    brain._regions = []
    brain._populations = []
    brain._initialized = False
    return brain

def createSimpleWorld():
    """Create a simple world."""
    world = SimpleWorld()
    world._width = 20
    world._height = 20
    world._objects = {}
    world._vision = Vision(8, 8, 3)
    world._audio = Audio(44100, 4410)
    world._internal_signals = InternalSignals()
    world._reset()
    return world

def createAgentBrain(brain):
    """Create an agent brain from a brain."""
    agent = AgentBrain()
    agent._brain = brain
    agent._world = None
    agent._sensory_percept = None
    agent._initialized = False
    return agent

# Exception classes
class ConfigError(Exception):
    """Raised when there's an error in configuration."""
    pass

class BrainError(Exception):
    """Raised when there's an error in brain operations."""
    pass

class WorldError(Exception):
    """Raised when there's an error in world operations."""
    pass

# Utility functions
def print_version():
    """Print version information."""
    print(f"NLM Python Bindings v{__version__}")
    print("Built with pybind11 and C++20")

# For backward compatibility with pybind11
import pybind11
if hasattr(pybind11, 'exceptions'):
    ConfigError = pybind11.exceptions.ValueError
    BrainError = pybind11.exceptions.RuntimeError
    WorldError = pybind11.exceptions.ValueError