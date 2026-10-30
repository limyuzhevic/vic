"""
Python enumerations module for NLM

This module provides Python-friendly enumerations for NLM types,
including neuron types, action types, and other enumerations with
convenience methods and type checking.
"""

from enum import Enum
from typing import Any, Dict, List
import sys

# Import from C++ bindings if available
try:
    from .bindings import (
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
    )
except ImportError:
    # Create Python enums as fallback
    NeuronType = Enum('NeuronType', [
        'Excitatory', 'Inhibitory', 'Modulatory', 
        'Sensory', 'Motor', 'Internal'
    ])
    
    SynapseType = Enum('SynapseType', [
        'Excitatory', 'Inhibitory', 'Modulatory',
        'Electrical', 'GapJunction'
    ])
    
    DevelopmentalStage = Enum('DevelopmentalStage', [
        'Initial', 'CriticalPeriod', 'Maturation',
        'Adult', 'Aging'
    ])
    
    FiringState = Enum('FiringState', [
        'Resting', 'Active', 'Refractory', 'Inhibited'
    ])
    
    ActionType = Enum('ActionType', [
        'MoveForward', 'MoveBackward', 'MoveLeft', 'MoveRight',
        'TurnLeft', 'TurnRight', 'Look', 'LookUp', 'LookDown',
        'Interact', 'Eat', 'Drink', 'Rest', 'Wait', 'Custom'
    ])
    
    MotorCommand = Enum('MotorCommand', [
        'MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight',
        'LookLeft', 'LookRight', 'Interact', 'Wait'
    ])
    
    WorldObjectType = Enum('WorldObjectType', [
        'Empty', 'Resource', 'Hazard', 'Wall', 'Marker'
    )

# Extension functions and utilities for enumerations
class EnumerationMixin:
    """Mixin class to add Pythonic features to enumerations"""
    
    @classmethod
    def from_string(cls, name: str) -> Any:
        """
        Create enumeration value from string.
        
        Args:
            name: String representation of the value
            
        Returns:
            Corresponding enumeration value
            
        Raises:
            ValueError: If name doesn't match any value
        """
        try:
            return cls[name]
        except KeyError:
            raise ValueError(f"Unknown {cls.__name__}: {name}")
    
    @classmethod
    def get_value(cls, name: str) -> Any:
        """
        Get enumeration value by name (case-insensitive).
        
        Args:
            name: Enumeration value name
            
        Returns:
            Corresponding enumeration value
        """
        try:
            return cls[name.upper()]
        except KeyError:
            # Try case-insensitive match
            for member in cls:
                if member.name.lower() == name.lower():
                    return member
            raise ValueError(f"Unknown {cls.__name__}: {name}")
    
    @classmethod
    def get_names(cls) -> List[str]:
        """Get all enumeration value names"""
        return [member.name for member in cls]
    
    @classmethod
    def get_values(cls) -> List[Any]:
        """Get all enumeration values"""
        return [member.value for member in cls]
    
    @classmethod
    def is_valid(cls, value: Any) -> bool:
        """
        Check if value is a valid enumeration member.
        
        Args:
            value: Value to check
            
        Returns:
            True if value is valid, False otherwise
        """
        return isinstance(value, cls) or value in cls._value2member_map_

# Add Pythonic features to enumerations
NeuronType = EnumerationMixin()
SynapseType = EnumerationMixin()
DevelopmentalStage = EnumerationMixin()
FiringState = EnumerationMixin()
ActionType = EnumerationMixin()
MotorCommand = EnumerationMixin()
WorldObjectType = EnumerationMixin()

# Convenience functions for enumerations
def get_neuron_types() -> List[NeuronType]:
    """Get all neuron types"""
    return list(NeuronType)

def get_excitatory_neuron_types() -> List[NeuronType]:
    """Get excitatory neuron types"""
    return [NeuronType.Excitatory, NeuronType.Sensory]

def get_inhibitory_neuron_types() -> List[NeuronType]:
    """Get inhibitory neuron types"""
    return [NeuronType.Inhibitory, NeuronType.Modulatory]

def get_motor_neuron_types() -> List[NeuronType]:
    """Get motor neuron types"""
    return [NeuronType.Motor, NeuronType.Internal]

def get_action_types() -> List[ActionType]:
    """Get all action types"""
    return list(ActionType)

def get_movement_actions() -> List[ActionType]:
    """Get movement-related action types"""
    return [
        ActionType.MoveForward, ActionType.MoveBackward,
        ActionType.MoveLeft, ActionType.MoveRight,
        ActionType.TurnLeft, ActionType.TurnRight
    ]

def get_interaction_actions() -> List[ActionType]:
    """Get interaction-related action types"""
    return [
        ActionType.Look, ActionType.LookUp, ActionType.LookDown,
        ActionType.Interact, ActionType.Eat, ActionType.Drink,
        ActionType.Rest, ActionType.Wait, ActionType.Custom
    ]

def get_developmental_stages() -> List[DevelopmentalStage]:
    """Get all developmental stages"""
    return list(DevelopmentalStage)

def get_early_stages() -> List[DevelopmentalStage]:
    """Get early developmental stages"""
    return [DevelopmentalStage.Initial, DevelopmentalStage.CriticalPeriod]

def get_late_stages() -> List[DevelopmentalStage]:
    """Get late developmental stages"""
    return [DevelopmentalStage.Maturation, DevelopmentalStage.Adult]

def get_motor_commands() -> List[MotorCommand]:
    """Get all motor commands"""
    return list(MotorCommand)

def get_movement_commands() -> List[MotorCommand]:
    """Get movement-related motor commands"""
    return [
        MotorCommand.MoveForward, MotorCommand.MoveBackward,
        MotorCommand.TurnLeft, MotorCommand.TurnRight
    ]

def get_world_object_types() -> List[WorldObjectType]:
    """Get all world object types"""
    return list(WorldObjectType)

def get_passive_object_types() -> List[WorldObjectType]:
    """Get passive object types"""
    return [WorldObjectType.Empty, WorldObjectType.Marker]

def get_active_object_types() -> List[WorldObjectType]:
    """Get active object types"""
    return [WorldObjectType.Resource, WorldObjectType.Hazard, WorldObjectType.Wall]

# String conversion utilities
def get_neuron_type_string(neuron_type: NeuronType) -> str:
    """Get string representation of neuron type"""
    return neuron_type.name.lower().replace('_', ' ')

def get_action_type_string(action_type: ActionType) -> str:
    """Get string representation of action type"""
    return action_type.name.lower().replace('_', ' ')

def get_developmental_stage_string(stage: DevelopmentalStage) -> str:
    """Get string representation of developmental stage"""
    return stage.name.lower().replace('_', ' ')

def get_world_object_type_string(obj_type: WorldObjectType) -> str:
    """Get string representation of world object type"""
    return obj_type.name.lower().replace('_', ' ')

# Type checking utilities
def validate_neuron_type(neuron_type: Any) -> NeuronType:
    """
    Validate and convert neuron type.
    
    Args:
        neuron_type: Neuron type to validate
        
    Returns:
        Valid NeuronType
        
    Raises:
        TypeError: If neuron_type is invalid
    """
    if isinstance(neuron_type, NeuronType):
        return neuron_type
    elif isinstance(neuron_type, str):
        return NeuronType.from_string(neuron_type)
    else:
        raise TypeError(f"Invalid NeuronType: {neuron_type}")

def validate_action_type(action_type: Any) -> ActionType:
    """
    Validate and convert action type.
    
    Args:
        action_type: Action type to validate
        
    Returns:
        Valid ActionType
        
    Raises:
        TypeError: If action_type is invalid
    """
    if isinstance(action_type, ActionType):
        return action_type
    elif isinstance(action_type, str):
        return ActionType.from_string(action_type)
    else:
        raise TypeError(f"Invalid ActionType: {action_type}")

# Metadata and documentation
__all__ = [
    # Enum classes
    'NeuronType',
    'SynapseType', 
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Extension classes
    'EnumerationMixin',
    
    # Convenience functions
    'get_neuron_types',
    'get_excitatory_neuron_types',
    'get_inhibitory_neuron_types',
    'get_motor_neuron_types',
    'get_action_types',
    'get_movement_actions',
    'get_interaction_actions',
    'get_developmental_stages',
    'get_early_stages',
    'get_late_stages',
    'get_motor_commands',
    'get_movement_commands',
    'get_world_object_types',
    'get_passive_object_types',
    'get_active_object_types',
    
    # String conversion functions
    'get_neuron_type_string',
    'get_action_type_string',
    'get_developmental_stage_string',
    'get_world_object_type_string',
    
    # Validation functions
    'validate_neuron_type',
    'validate_action_type',
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python enumerations for NLM neural simulation"

print(f"NLM enumerations module loaded (version {__version__})")