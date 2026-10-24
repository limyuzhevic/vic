"""
NLM data types and enumerations
"""

from enum import Enum
from typing import List, Optional, Dict, Any

# Type definitions from C++ headers
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int

# Numerical types (simplified for Python)
MembranePotential = float
FiringRate = float
Timestamp = float
SimulationStep = int
Delay = int

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

class PlasticityFlags:
    """Plasticity flags structure"""
    
    def __init__(self, hebbian: bool = False, stdp: bool = False, reward_modulated: bool = False):
        self.hebbian = hebbian
        self.stdp = stdp
        self.reward_modulated = reward_modulated
    
    def __str__(self):
        flags = []
        if self.hebbian:
            flags.append("Hebbian")
        if self.stdp:
            flags.append("STDP")
        if self.reward_modulated:
            flags.append("Reward")
        return "|".join(flags) if flags else "None"

# Constants for invalid IDs
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)

# Utility function to convert C++ style enum to Python-friendly names
def get_enum_name(enum_value: Enum) -> str:
    """Get readable name from enum"""
    return enum_value.name if hasattr(enum_value, 'name') else str(enum_value)

# Predefined lists for convenience
NEURON_TYPES = list(NeuronType)
ACTION_TYPES = list(ActionType)
MOTOR_COMMANDS = list(MotorCommand)
WORLD_OBJECT_TYPES = list(WorldObjectType)
DEVELOPER_STAGES = list(DevelopmentalStage)
FIRING_STATES = list(FiringState)
PLASTICITY_FLAGS = [
    PlasticityFlags(hebbian=True),
    PlasticityFlags(stdp=True),
    PlasticityFlags(reward_modulated=True),
    PlasticityFlags(hebbian=True, stdp=True),
    PlasticityFlags(hebbian=True, reward_modulated=True),
    PlasticityFlags(stdp=True, reward_modulated=True),
]
SYNAPSE_TYPES = list(SynapseType)
