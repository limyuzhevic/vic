# Enums module - Enumeration types
# ==============================

from enum import Enum

class NeuronType(Enum):
    """Types of neurons."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    SENSORY = 3
    MOTOR = 4
    INTERNAL = 5

class SynapseType(Enum):
    """Types of synapses."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    ELECTRICAL = 3
    GAP_JUNCTION = 4

class DevelopmentalStage(Enum):
    """Developmental stages of the brain."""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

class FiringState(Enum):
    """States of neuron firing."""
    RESTING = 0
    ACTIVE = 1
    REFRACTORY = 2
    INHIBITED = 3

class ActionType(Enum):
    """Types of actions."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    MOVE_LEFT = 2
    MOVE_RIGHT = 3
    TURN_LEFT = 4
    TURN_RIGHT = 5
    LOOK = 6
    LOOK_UP = 7
    LOOK_DOWN = 8
    INTERACT = 9
    EAT = 10
    DRINK = 11
    REST = 12
    WAIT = 13
    CUSTOM = 14

class MotorCommand(Enum):
    """Low-level motor commands."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7

class WorldObjectType(Enum):
    """Types of world objects."""
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

__all__ = [
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
]