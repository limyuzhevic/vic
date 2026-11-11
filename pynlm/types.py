# Types module - Python wrapper for NLM type definitions
"""
Python wrapper for NLM type definitions, providing enhanced Python interfaces
to C++ type definitions.
"""

from enum import Enum
from typing import Any, Union
import numpy as np

# Core type definitions
class NeuronId:
    """Python wrapper for NeuronId type."""
    
    def __init__(self, value: Union[int, 'NeuronId'] = 0):
        """Initialize neuron ID.
        
        Args:
            value: ID value.
        """
        if isinstance(value, NeuronId):
            self.value = value.value
        else:
            self.value = value
    
    def index(self) -> int:
        """Get index representation.
        
        Returns:
            Index value.
        """
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality with other NeuronId."""
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality with other NeuronId."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Hash function."""
        return hash(self.value)
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<NeuronId: {self.value}>"
    
    def __str__(self) -> str:
        """String representation."""
        return f"NeuronId({self.value})"


class SynapseId:
    """Python wrapper for SynapseId type."""
    
    def __init__(self, value: Union[int, 'SynapseId'] = 0):
        """Initialize synapse ID.
        
        Args:
            value: ID value.
        """
        if isinstance(value, SynapseId):
            self.value = value.value
        else:
            self.value = value
    
    def index(self) -> int:
        """Get index representation.
        
        Returns:
            Index value.
        """
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality with other SynapseId."""
        if isinstance(other, SynapseId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality with other SynapseId."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Hash function."""
        return hash(self.value)
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<SynapseId: {self.value}>"
    
    def __str__(self) -> str:
        """String representation."""
        return f"SynapseId({self.value})"


class RegionId:
    """Python wrapper for RegionId type."""
    
    def __init__(self, value: Union[int, 'RegionId'] = 0):
        """Initialize region ID.
        
        Args:
            value: ID value.
        """
        if isinstance(value, RegionId):
            self.value = value.value
        else:
            self.value = value
    
    def index(self) -> int:
        """Get index representation.
        
        Returns:
            Index value.
        """
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality with other RegionId."""
        if isinstance(other, RegionId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality with other RegionId."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Hash function."""
        return hash(self.value)
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<RegionId: {self.value}>"
    
    def __str__(self) -> str:
        """String representation."""
        return f"RegionId({self.value})"


class PopulationId:
    """Python wrapper for PopulationId type."""
    
    def __init__(self, value: Union[int, 'PopulationId'] = 0):
        """Initialize population ID.
        
        Args:
            value: ID value.
        """
        if isinstance(value, PopulationId):
            self.value = value.value
        else:
            self.value = value
    
    def index(self) -> int:
        """Get index representation.
        
        Returns:
            Index value.
        """
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality with other PopulationId."""
        if isinstance(other, PopulationId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality with other PopulationId."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Hash function."""
        return hash(self.value)
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<PopulationId: {self.value}>"
    
    def __str__(self) -> str:
        """String representation."""
        return f"PopulationId({self.value})"


# Enumerations

class NeuronType(Enum):
    """Neuron type enumeration."""
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

    def __str__(self) -> str:
        """String representation."""
        return self.name


class SynapseType(Enum):
    """Synapse type enumeration."""
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

    def __str__(self) -> str:
        """String representation."""
        return self.name


class DevelopmentalStage(Enum):
    """Developmental stage enumeration."""
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

    def __str__(self) -> str:
        """String representation."""
        return self.name


class FiringState(Enum):
    """Neuron firing state enumeration."""
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

    def __str__(self) -> str:
        """String representation."""
        return self.name


class ActionType(Enum):
    """Action type enumeration."""
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

    def __str__(self) -> str:
        """String representation."""
        return self.name


class MotorCommand(Enum):
    """Motor command enumeration."""
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

    def __str__(self) -> str:
        """String representation."""
        return self.name


class WorldObjectType(Enum):
    """World object type enumeration."""
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

    def __str__(self) -> str:
        """String representation."""
        return self.name


# Type aliases for convenience
IdType = Union[NeuronId, SynapseId, RegionId, PopulationId]
ValueType = Union[int, float, str, bool, List, Dict]
SimulationStep = int
Timestamp = float

# Constants

INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)

# Convenience functions

def is_valid_id(obj: Any) -> bool:
    """Check if object is a valid ID type.
    
    Args:
        obj: Object to check.
        
    Returns:
        True if object is a valid ID type, False otherwise.
    """
    return isinstance(obj, (NeuronId, SynapseId, RegionId, PopulationId))


def get_id_value(obj: IdType) -> int:
    """Get value from ID object.
    
    Args:
        obj: ID object.
        
    Returns:
        Integer value.
    """
    return obj.value if hasattr(obj, 'value') else obj


def format_id(obj: IdType) -> str:
    """Format ID object for display.
    
    Args:
        obj: ID object.
        
    Returns:
        Formatted string.
    """
    return str(obj)


# Re-export for convenience
__all__ = [
    # Classes
    'NeuronId',
    'SynapseId', 
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Type aliases
    'IdType',
    'ValueType',
    'SimulationStep',
    'Timestamp',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Functions
    'is_valid_id',
    'get_id_value',
    'format_id',
]