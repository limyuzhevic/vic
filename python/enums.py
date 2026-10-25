"""Enumerations for NLM Python interface"""

from enum import Enum
from dataclasses import dataclass
from typing import Dict, Any

# Neuron Types

class NeuronType(Enum):
    """Neuron types in the NLM system."""
    
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"

# Synapse Types

class SynapseType(Enum):
    """Synapse types in the NLM system."""
    
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"

# Developmental Stages

class DevelopmentalStage(Enum):
    """Developmental stages in the NLM system."""
    
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

# Firing States

class FiringState(Enum):
    """Neuron firing states."""
    
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"

# Action Types

class ActionType(Enum):
    """Action types for motor control."""
    
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

# Motor Commands

class MotorCommand(Enum):
    """Low-level motor commands."""
    
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

# World Object Types

class WorldObjectType(Enum):
    """Types of objects in the world."""
    
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
# Mapping of enum names to values for easy access

ENUM_MAPPING: Dict[str, Dict[str, Any]] = {
    'neuron_type': {
        'excitatory': NeuronType.EXCITATORY,
        'inhibitory': NeuronType.INHIBITORY,
        'modulatory': NeuronType.MODULATORY,
        'sensory': NeuronType.SENSORY,
        'motor': NeuronType.MOTOR,
        'internal': NeuronType.INTERNAL
    },
    'synapse_type': {
        'excitatory': SynapseType.EXCITATORY,
        'inhibitory': SynapseType.INHIBITORY,
        'modulatory': SynapseType.MODULATORY,
        'electrical': SynapseType.ELECTRICAL,
        'gap_junction': SynapseType.GAP_JUNCTION
    },
    'developmental_stage': {
        'initial': DevelopmentalStage.INITIAL,
        'critical_period': DevelopmentalStage.CRITICAL_PERIOD,
        'maturation': DevelopmentalStage.MATURATION,
        'adult': DevelopmentalStage.ADULT,
        'aging': DevelopmentalStage.AGING
    },
    'firing_state': {
        'resting': FiringState.RESTING,
        'active': FiringState.ACTIVE,
        'refractory': FiringState.REFRACTORY,
        'inhibited': FiringState.INHIBITED
    },
    'action_type': {
        'move_forward': ActionType.MOVE_FORWARD,
        'move_backward': ActionType.MOVE_BACKWARD,
        'move_left': ActionType.MOVE_LEFT,
        'move_right': ActionType.MOVE_RIGHT,
        'turn_left': ActionType.TURN_LEFT,
        'turn_right': ActionType.TURN_RIGHT,
        'look': ActionType.LOOK,
        'look_up': ActionType.LOOK_UP,
        'look_down': ActionType.LOOK_DOWN,
        'interact': ActionType.INTERACT,
        'eat': ActionType.EAT,
        'drink': ActionType.DRINK,
        'rest': ActionType.REST,
        'wait': ActionType.WAIT,
        'custom': ActionType.CUSTOM
    },
    'motor_command': {
        'move_forward': MotorCommand.MOVE_FORWARD,
        'move_backward': MotorCommand.MOVE_BACKWARD,
        'turn_left': MotorCommand.TURN_LEFT,
        'turn_right': MotorCommand.TURN_RIGHT,
        'look_left': MotorCommand.LOOK_LEFT,
        'look_right': MotorCommand.LOOK_RIGHT,
        'interact': MotorCommand.INTERACT,
        'wait': MotorCommand.WAIT
    },
    'world_object_type': {
        'empty': WorldObjectType.EMPTY,
        'resource': WorldObjectType.RESOURCE,
        'hazard': WorldObjectType.HAZARD,
        'wall': WorldObjectType.WALL,
        'marker': WorldObjectType.MARKER
    }
}

# Helper functions for enum access


def get_enum_value(enum_type: str, value: str) -> Any:
    """Get enum value from string."""
    if enum_type in ENUM_MAPPING:
        mapping = ENUM_MAPPING[enum_type]
        if value in mapping:
            return mapping[value]
    return None


def get_enum_values(enum_type: str) -> List[str]:
    """Get all string values for enum type."""
    if enum_type in ENUM_MAPPING:
        return list(ENUM_MAPPING[enum_type].keys())
    return []


def get_enum_members(enum_type: str) -> List[Any]:
    """Get all enum members for enum type."""
    if enum_type == 'neuron_type':
        return list(NeuronType)
    elif enum_type == 'synapse_type':
        return list(SynapseType)
    elif enum_type == 'developmental_stage':
        return list(DevelopmentalStage)
    elif enum_type == 'firing_state':
        return list(FiringState)
    elif enum_type == 'action_type':
        return list(ActionType)
    elif enum_type == 'motor_command':
        return list(MotorCommand)
    elif enum_type == 'world_object_type':
        return list(WorldObjectType)
    
    return []


# String representations

@classmethod
def _get_action_type_name(cls, action_type):
    """Get human-readable name for action type."""
    return action_type.value.replace('_', ' ').title()


@classmethod
def _get_motor_command_name(cls, motor_command):
    """Get human-readable name for motor command."""
    return motor_command.value.replace('_', ' ').title()


# Add methods to enum classes
ActionType.get_name = _get_action_type_name
MotorCommand.get_name = _get_motor_command_name


# Export all enums
__all__ = [
    'NeuronType',
    'SynapseType', 
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'ENUM_MAPPING',
    'get_enum_value',
    'get_enum_values',
    'get_enum_members'
]