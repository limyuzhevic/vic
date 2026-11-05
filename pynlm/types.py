"""
Type definitions for NLM Python bindings
"""

from dataclasses import dataclass
from enum import Enum
from typing import Optional, List

# Basic types
class SimulationStep(int):
    """Simulation step counter"""
    pass

class Timestamp(float):
    """Simulation time stamp"""
    pass

class MembranePotential(float):
    """Membrane potential value"""
    pass

class Delay(int):
    """Delay in simulation steps"""
    pass

class Reward(float):
    """Reward value"""
    pass

# ID types
@dataclass(frozen=True)
class NeuronId:
    """Unique identifier for a neuron"""
    value: int = 0
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<NeuronId: {self.value}>"

@dataclass(frozen=True)
class SynapseId:
    """Unique identifier for a synapse"""
    value: int = 0
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<SynapseId: {self.value}>"

@dataclass(frozen=True)
class RegionId:
    """Unique identifier for a brain region"""
    value: int = 0
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, RegionId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<RegionId: {self.value}>"

@dataclass(frozen=True)
class PopulationId:
    """Unique identifier for a neuron population"""
    value: int = 0
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

# Enum types
class NeuronType(Enum):
    """Neuron type enumeration"""
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType(Enum):
    """Synapse type enumeration"""
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage(Enum):
    """Developmental stage enumeration"""
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class FiringState(Enum):
    """Neuron firing state enumeration"""
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

class ActionType(Enum):
    """Action type enumeration"""
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

class MotorCommand(Enum):
    """Low-level motor command enumeration"""
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType(Enum):
    """World object type enumeration"""
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# Constants
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)

# Utility functions
def to_simulation_step(value: int) -> SimulationStep:
    """Convert value to SimulationStep"""
    return SimulationStep(value)

def to_timestamp(value: float) -> Timestamp:
    """Convert value to Timestamp"""
    return Timestamp(value)

def to_membrane_potential(value: float) -> MembranePotential:
    """Convert value to MembranePotential"""
    return MembranePotential(value)

def to_delay(value: int) -> Delay:
    """Convert value to Delay"""
    return Delay(value)

def to_reward(value: float) -> Reward:
    """Convert value to Reward"""
    return Reward(value)

# Constants for plasticity flags
class PlasticityFlags:
    """Plasticity flags for synapse connections"""
    STDP_ENABLED = 0x01
    HEBBIAN_ENABLED = 0x02
    STRUCTURAL_ENABLED = 0x04
    NOVELTY_ENABLED = 0x08
    REWARD_BASED = 0x10
    DEVOLUTION_ENABLED = 0x20

# Default values
DEFAULT_WORLD_WIDTH = 640
DEFAULT_WORLD_HEIGHT = 480
DEFAULT_VISION_WIDTH = 320
DEFAULT_VISION_HEIGHT = 240
DEFAULT_MAX_ENERGY = 100.0
DEFAULT_ENERGY_DECAY_RATE = 0.01
DEFAULT_RANDOM_SEED = 42
