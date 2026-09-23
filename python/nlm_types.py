"""
NLM type definitions and helper classes for Python bindings.

This module contains the strong type classes (NeuronId, SynapseId, etc.) and
type enumeration classes used throughout the NLM framework.
"""

import ctypes
import struct
import sys
from typing import Optional, Union, Dict, Any
class NeuronId:
    """
    Unique identifier for a neuron.
    
    This class represents a neuron identifier with methods for comparison,
    hashing, and string representation. NeuronIds can be created from
    unsigned integers or the default invalid ID.
    
    Attributes:
        value (int): The integer value of this neuron ID
        
    Examples:
        >>> nid = NeuronId()
        >>> nid2 = NeuronId(12345)
        >>> print(nid2.value)
        12345
        >>> print(repr(nid2))
        <NeuronId: 12345>
    """
    
    def __init__(self, value: Union[int, None] = None):
        """
        Initialize a NeuronId.
        
        Args:
            value: Integer value for the ID. If None, creates invalid ID (0).
        """
        if value is None:
            self.value = 0
        else:
            self.value = int(value)
    
    def index(self) -> int:
        """Return the index component of this ID.
        
        Returns:
            int: The index value
        """
        return self.value
    
    def __eq__(self, other) -> bool:
        """Check if this ID equals another ID."""
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    def __ne__(self, other) -> bool:
        """Check if this ID is not equal to another ID."""
        return not self.__eq__(other)
    
    def __lt__(self, other) -> bool:
        """Check if this ID is less than another ID."""
        if isinstance(other, NeuronId):
            return self.value < other.value
        return NotImplemented
    
    def __gt__(self, other) -> bool:
        """Check if this ID is greater than another ID."""
        if isinstance(other, NeuronId):
            return self.value > other.value
        return NotImplemented
    
    def __le__(self, other) -> bool:
        """Check if this ID is less than or equal to another ID."""
        if isinstance(other, NeuronId):
            return self.value <= other.value
        return NotImplemented
    
    def __ge__(self, other) -> bool:
        """Check if this ID is greater than or equal to another ID."""
        if isinstance(other, NeuronId):
            return self.value >= other.value
        return NotImplemented
    
    def __hash__(self) -> int:
        """Return hash value for this ID."""
        return hash(self.value)
    
    def __str__(self) -> str:
        """Return string representation."""
        return f"NeuronId({self.value})"
    
    def __repr__(self) -> str:
        """Return detailed string representation."""
        return f"<NeuronId: {self.value}>"
    
    def is_valid(self) -> bool:
        """Check if this is a valid ID (non-zero)."""
        return self.value != 0
    
    def to_bytes(self) -> bytes:
        """Convert ID to bytes.
        
        Returns:
            bytes: 8-byte little-endian representation
        """
        return struct.pack('<Q', self.value)
    
    @classmethod
    def from_bytes(cls, data: bytes) -> 'NeuronId':
        """
        Create NeuronId from bytes.
        
        Args:
            data: 8-byte little-endian representation
            
        Returns:
            NeuronId: New instance
            
        Raises:
            ValueError: If data is not 8 bytes
        """
        if len(data) != 8:
            raise ValueError("NeuronId.from_bytes() requires exactly 8 bytes")
        value = struct.unpack('<Q', data)[0]
        return cls(value)
class SynapseId(NeuronId):
    """
    Unique identifier for a synapse.
    
    This class extends NeuronId to provide synapse-specific functionality
    while maintaining the same interface for consistency.
    
    Attributes:
        value (int): The integer value of this synapse ID
    """
    
    def __repr__(self) -> str:
        """Return detailed string representation."""
        return f"<SynapseId: {self.value}>"
class RegionId(NeuronId):
    """
    Unique identifier for a brain region.
    
    This class extends NeuronId to provide region-specific functionality
    while maintaining the same interface for consistency.
    
    Attributes:
        value (int): The integer value of this region ID
    """
    
    def __repr__(self) -> str:
        """Return detailed string representation."""
        return f"<RegionId: {self.value}>"
class PopulationId(NeuronId):
    """
    Unique identifier for a neuron population.
    
    This class extends NeuronId to provide population-specific functionality
    while maintaining the same interface for consistency.
    
    Attributes:
        value (int): The integer value of this population ID
    """
    
    def __repr__(self) -> str:
        """Return detailed string representation."""
        return f"<PopulationId: {self.value}>"
class NeuronType:
    """
    Enumeration of neuron types.
    
    These types define the functional classification of neurons within
    the NLM framework.
    
    Constants:
        EXCITATORY: Excitatory neuron (stimulates target neurons)
        INHIBITORY: Inhibitory neuron (suppresses target neurons)  
        MODULATORY: Modulatory neuron (affects plasticity and dynamics)
        SENSORY: Sensory neuron (receives environmental input)
        MOTOR: Motor neuron (generates action output)
        INTERNAL: Internal neuron (internal processing)
    """
    
    # Type constants
    EXCITATORY = 1
    INHIBITORY = 2
    MODULATORY = 3
    SENSORY = 4
    MOTOR = 5
    INTERNAL = 6
    
    _value_to_name = {
        1: 'EXCITATORY',
        2: 'INHIBITORY', 
        3: 'MODULATORY',
        4: 'SENSORY',
        5: 'MOTOR',
        6: 'INTERNAL'
    }
    
    _name_to_value = {
        'EXCITATORY': 1,
        'INHIBITORY': 2,
        'MODULATORY': 3,
        'SENSORY': 4,
        'MOTOR': 5,
        'INTERNAL': 6
    }
    
    @classmethod
    def from_value(cls, value: int) -> 'NeuronType':
        """Get NeuronType from integer value."""
        return cls._value_to_name.get(value, 'UNKNOWN')
    
    @classmethod
    def to_value(cls, name: str) -> int:
        """Get integer value from name."""
        return cls._name_to_value.get(name, 0)
    
    @classmethod
    def get_names(cls) -> list:
        """Get all neuron type names."""
        return list(cls._name_to_value.keys())
    
    @classmethod
    def get_values(cls) -> list:
        """Get all neuron type values."""
        return list(cls._name_to_value.values())
class SynapseType:
    """
    Enumeration of synapse types.
    
    These types define the functional classification of synapses within
    the NLM framework.
    
    Constants:
        EXCITATORY: Standard excitatory synapse
        INHIBITORY: Standard inhibitory synapse
        MODULATORY: Modulatory synapse (affects plasticity)
        ELECTRICAL: Electrical synapse (direct current flow)
        GAP_JUNCTION: Gap junction synapse
    """
    
    # Synapse type constants
    EXCITATORY = 1
    INHIBITORY = 2
    MODULATORY = 3
    ELECTRICAL = 4
    GAP_JUNCTION = 5
    
    _value_to_name = {
        1: 'EXCITATORY',
        2: 'INHIBITORY',
        3: 'MODULATORY',
        4: 'ELECTRICAL',
        5: 'GAP_JUNCTION'
    }
    
    _name_to_value = {
        'EXCITATORY': 1,
        'INHIBITORY': 2,
        'MODULATORY': 3,
        'ELECTRICAL': 4,
        'GAP_JUNCTION': 5
    }
class DevelopmentalStage:
    """
    Enumeration of developmental stages.
    
    These stages represent different phases of neural development
    and maturation within the NLM framework.
    
    Constants:
        INITIAL: Initial developmental stage (primitive)
        CRITICAL_PERIOD: Critical period for learning
        MATURATION: Ongoing maturation phase
        ADULT: Adult/stable stage
        AGING: Aging phase
    """
    
    # Developmental stage constants
    INITIAL = 1
    CRITICAL_PERIOD = 2
    MATURATION = 3
    ADULT = 4
    AGING = 5
    
    _value_to_name = {
        1: 'INITIAL',
        2: 'CRITICAL_PERIOD',
        3: 'MATURATION',
        4: 'ADULT',
        5: 'AGING'
    }
    
    _name_to_value = {
        'INITIAL': 1,
        'CRITICAL_PERIOD': 2,
        'MATURATION': 3,
        'ADULT': 4,
        'AGING': 5
    }
class FiringState:
    """
    Enumeration of neuron firing states.
    
    These states represent the current activity level of neurons
    within the NLM framework.
    
    Constants:
        RESTING: Neuron is not firing
        ACTIVE: Neuron is actively firing
        REFRACTORY: Neuron is in refractory period
        INHIBITED: Neuron is inhibited
    """
    
    # Firing state constants
    RESTING = 1
    ACTIVE = 2
    REFRACTORY = 3
    INHIBITED = 4
    
    _value_to_name = {
        1: 'RESTING',
        2: 'ACTIVE',
        3: 'REFRACTORY',
        4: 'INHIBITED'
    }
    
    _name_to_value = {
        'RESTING': 1,
        'ACTIVE': 2,
        'REFRACTORY': 3,
        'INHIBITED': 4
    }
class ActionType:
    """
    Enumeration of action types.
    
    These types define the possible actions that the NLM agent can
    take in the environment.
    """
    
    # Action type constants
    MOVE_FORWARD = 1
    MOVE_BACKWARD = 2
    MOVE_LEFT = 3
    MOVE_RIGHT = 4
    TURN_LEFT = 5
    TURN_RIGHT = 6
    LOOK = 7
    LOOK_UP = 8
    LOOK_DOWN = 9
    INTERACT = 10
    EAT = 11
    DRINK = 12
    REST = 13
    WAIT = 14
    CUSTOM = 15
    
    _value_to_name = {
        1: 'MOVE_FORWARD',
        2: 'MOVE_BACKWARD',
        3: 'MOVE_LEFT',
        4: 'MOVE_RIGHT',
        5: 'TURN_LEFT',
        6: 'TURN_RIGHT',
        7: 'LOOK',
        8: 'LOOK_UP',
        9: 'LOOK_DOWN',
        10: 'INTERACT',
        11: 'EAT',
        12: 'DRINK',
        13: 'REST',
        14: 'WAIT',
        15: 'CUSTOM'
    }
    
    _name_to_value = {
        'MOVE_FORWARD': 1,
        'MOVE_BACKWARD': 2,
        'MOVE_LEFT': 3,
        'MOVE_RIGHT': 4,
        'TURN_LEFT': 5,
        'TURN_RIGHT': 6,
        'LOOK': 7,
        'LOOK_UP': 8,
        'LOOK_DOWN': 9,
        'INTERACT': 10,
        'EAT': 11,
        'DRINK': 12,
        'REST': 13,
        'WAIT': 14,
        'CUSTOM': 15
    }
class MotorCommand:
    """
    Enumeration of motor commands.
    
    These are low-level motor commands that can be sent to the
    environment or simulated physical system.
    """
    
    # Motor command constants
    MOVE_FORWARD = 1
    MOVE_BACKWARD = 2
    TURN_LEFT = 3
    TURN_RIGHT = 4
    LOOK_LEFT = 5
    LOOK_RIGHT = 6
    INTERACT = 7
    WAIT = 8
    
    _value_to_name = {
        1: 'MOVE_FORWARD',
        2: 'MOVE_BACKWARD',
        3: 'TURN_LEFT',
        4: 'TURN_RIGHT',
        5: 'LOOK_LEFT',
        6: 'LOOK_RIGHT',
        7: 'INTERACT',
        8: 'WAIT'
    }
class WorldObjectType:
    """
    Enumeration of world object types.
    
    These types define the categories of objects that can exist
    in the simulated environment.
    """
    
    # World object type constants
    EMPTY = 1
    RESOURCE = 2
    HAZARD = 3
    WALL = 4
    MARKER = 5
    
    _value_to_name = {
        1: 'EMPTY',
        2: 'RESOURCE',
        3: 'HAZARD',
        4: 'WALL',
        5: 'MARKER'
    }
    
    _name_to_value = {
        'EMPTY': 1,
        'RESOURCE': 2,
        'HAZARD': 3,
        'WALL': 4,
        'MARKER': 5
    }
class ConfigSource:
    """
    Enumeration of configuration sources.
    
    These values indicate where configuration data originated from,
    which affects how it should be treated during runtime.
    
    Constants:
        DEFAULT: Default configuration value
        FILE: Configuration loaded from file
        COMMAND_LINE: Configuration from command line arguments
        RUNTIME: Configuration set at runtime
    """
    
    DEFAULT = 1
    FILE = 2
    COMMAND_LINE = 3
    RUNTIME = 4
