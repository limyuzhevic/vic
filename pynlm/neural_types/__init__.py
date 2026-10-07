# Neural types module

from enum import Enum

class NeuronType(Enum):
    """Types of neurons in the NLM brain."""
    EXCITATORY = 0
    INHIBITORY = 1
    SENSORY = 2
    MOTOR = 3
    MODULATORY = 4
    INTERNAL = 5

class DevelopmentalStage(Enum):
    """Developmental stages of the brain."""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

class ActionType(Enum):
    """Types of motor actions."""
    MOVE_FORWARD = 0
    TURN_LEFT = 1
    TURN_RIGHT = 2
    INTERACT = 3
    EAT = 4
    SLEEP = 5
    EXPLORE = 6
    STAY = 7

class WorldObjectType(Enum):
    """Types of objects in the world."""
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

__all__ = [
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'WorldObjectType'
]