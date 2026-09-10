# Type annotations for Python static type checking
from typing import Optional, List, Dict, Any, Tuple, Generator, Union, Callable
from enum import Enum

# Core configuration types
ConfigKey = str
ConfigValue = Union[str, int, float, bool]
SimulationStep = int
Timestamp = float
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int

# Forward declarations for type hints
class Config:
    pass

class Brain:
    pass

class AgentBrain:
    pass

class SimpleWorld:
    pass

class SensoryInput:
    pass

class SensoryPercept:
    pass

class Action:
    pass

class Vision(SensoryInput):
    pass

class Audio(SensoryInput):
    pass

class InternalSignals(SensoryInput):
    pass

# Enumerations
class NeuronType(Enum):
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"

class SynapseType(Enum):
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"

class DevelopmentalStage(Enum):
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class FiringState(Enum):
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"

class ActionType(Enum):
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"

class MotorCommand(Enum):
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class WorldObjectType(Enum):
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

# Config value types
ConfigValueType = Union[str, int, float, bool]

# Statistical types
StatisticalDict = Dict[str, float]
NeuronStats = Dict[str, Any]
MemoryStats = Dict[str, Any]
