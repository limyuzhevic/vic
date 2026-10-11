# Core types and utilities for NLM
from dataclasses import dataclass
from enum import Enum
from typing import Optional, Union, Dict, Any, List

# Basic type aliases
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int
MembranePotential = float
FiringRate = float
Timestamp = float
SimulationStep = int
TimestepDuration = float

# Value type (would typically be in a separate types header)
@dataclass(frozen=True)
class ValueType:
    value: Union[int, float, str]
    type_name: str = "unknown"

# Enumerations
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
    Active = 1
    Refractory = 2
    Inhibited = 3

class ActionType(Enum):
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
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType(Enum):
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# Factory functions for backward compatibility
def create_neuron_id(value: int = 0) -> NeuronId:
    return NeuronId(value)

def create_synapse_id(value: int = 0) -> SynapseId:
    return SynapseId(value)

def create_region_id(value: int = 0) -> RegionId:
    return RegionId(value)

def create_population_id(value: int = 0) -> PopulationId:
    return PopulationId(value)

# Utility functions
def get_enum_values(enum_class: type) -> List[Any]:
    return [member.value for member in enum_class]

def get_enum_names(enum_class: type) -> List[str]:
    return [member.name for member in enum_class]
