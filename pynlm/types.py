"""
Type Definitions Module

This module provides comprehensive type definitions for the NLM Python bindings.
It includes type aliases, enumerations, and data structures that match the
underlying C++ types.
"""

from typing import (
    TypeVar, Generic, List, Dict, Optional, Union, Any, Tuple,
    Callable, Iterator, Sequence, Set, Mapping, MutableMapping,
    Type, overload
)

# Optional imports for NumPy support
try:
    import numpy as np
    NUMPY_AVAILABLE = True
    ndarray = np.ndarray
except ImportError:
    np = None
    NUMPY_AVAILABLE = False
    ndarray = Any  # Fallback to Any when numpy is not available

from dataclasses import dataclass, field
from enum import Enum
import uuid
import time

# Core type aliases matching C++ types

# Numeric types
MembranePotential = float
PopulationSize = int
ConnectionWeight = float
SimulationStep = int
Timestamp = float
NeuronIdValue = int
SynapseIdValue = int
RegionIdValue = int
PopulationIdValue = int

# Type variables
T = TypeVar('T')
K = TypeVar('K')
V = TypeVar('V')

# Enums matching C++ enumerations

class NeuronType(Enum):
    """Neuron type enumeration matching C++ NeuronType."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"

class SynapseType(Enum):
    """Synapse type enumeration matching C++ SynapseType."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"

class DevelopmentalStage(Enum):
    """Developmental stage enumeration matching C++ DevelopmentalStage."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class FiringState(Enum):
    """Neuron firing state enumeration matching C++ FiringState."""
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"

class ActionType(Enum):
    """Action type enumeration matching C++ ActionType."""
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
    " ""Motor command enumeration matching C++ MotorCommand."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class WorldObjectType(Enum):
    """World object type enumeration matching C++ WorldObjectType."""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

# ID types matching C++ ID classes

@dataclass(frozen=True)
class NeuronId:
    """Unique identifier for a neuron."""
    value: NeuronIdValue = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    def __post_init__(self):
        if not isinstance(self.value, int) or self.value < 0:
            raise ValueError("NeuronId value must be a non-negative integer")
    
    @property
    def index(self) -> int:
        """Get the index of the neuron ID."""
        return self.value
    
    def __str__(self) -> str:
        return f"<NeuronId: {self.value}>"
    
    def __repr__(self) -> str:
        return str(self)

@dataclass(frozen=True)
class SynapseId:
    """Unique identifier for a synapse."""
    value: SynapseIdValue = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    def __post_init__(self):
        if not isinstance(self.value, int) or self.value < 0:
            raise ValueError("SynapseId value must be a non-negative integer")
    
    @property
    def index(self) -> int:
        return self.value
    
    def __str__(self) -> str:
        return f"<SynapseId: {self.value}>"
    
    def __repr__(self) -> str:
        return str(self)

@dataclass(frozen=True)
class RegionId:
    """Unique identifier for a brain region."""
    value: RegionIdValue = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    def __post_init__(self):
        if not isinstance(self.value, int) or self.value < 0:
            raise ValueError("RegionId value must be a non-negative integer")
    
    @property
    def index(self) -> int:
        return self.value
    
    def __str__(self) -> str:
        return f"<RegionId: {self.value}>"
    
    def __repr__(self) -> str:
        return str(self)

@dataclass(frozen=True)
class PopulationId:
    """Unique identifier for a neuron population."""
    value: PopulationIdValue = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    def __post_init__(self):
        if not isinstance(self.value, int) or self.value < 0:
            raise ValueError("PopulationId value must be a non-negative integer")
    
    @property
    def index(self) -> int:
        return self.value

# Special constants for invalid IDs
INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)

# Data structure definitions

@dataclass
class Position2D:
    """2D position in the world."""
    x: float = 0.0
    y: float = 0.0
    
    def distance_to(self, other: 'Position2D') -> float:
        """Calculate Euclidean distance to another position."""
        return ((self.x - other.x) ** 2 + (self.y - other.y) ** 2) ** 0.5

@dataclass
class ActionParameters:
    """Parameters for motor actions."""
    values: List[float] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def __post_init__(self):
        if not isinstance(self.values, list):
            raise ValueError("ActionParameters.values must be a list")

@dataclass
class SensoryData:
    """Generic sensory data container."""
    data: Any  # Use Any instead of np.ndarray to avoid numpy dependency
    timestamp: Timestamp
    sensor_id: str = ""
    
    def __post_init__(self):
        if NUMPY_AVAILABLE and isinstance(self.data, np.ndarray):
            pass  # numpy array is valid
        elif isinstance(self.data, list):
            self.data = list(self.data)
        elif isinstance(self.data, (int, float)):
            self.data = [self.data]
        else:
            self.data = Any(self.data) if hasattr(Any, '__call__') else self.data

@dataclass
class MemoryTrace:
    """Memory trace with activation and age."""
    activation: float = 0.0
    age: SimulationStep = 0
    timestamp: Timestamp = field(default_factory=time.time)
    importance: float = 1.0

@dataclass
class NeuralConnection:
    """Neural connection between neurons."""
    source: NeuronId
    target: NeuronId
    weight: ConnectionWeight = 1.0
    delay: SimulationStep = 1
    plasticity_flags: Dict[str, bool] = field(default_factory=dict)
    
    def __post_init__(self):
        if not isinstance(self.source, NeuronId):
            self.source = NeuronId(self.source)
        if not isinstance(self.target, NeuronId):
            self.target = NeuronId(self.target)

# Utility type functions

def is_valid_neuron_id(neuron_id: Any) -> bool:
    """Check if neuron ID is valid."""
    return isinstance(neuron_id, NeuronId) or (
        isinstance(neuron_id, int) and neuron_id >= 0
    )

def is_valid_synapse_id(synapse_id: Any) -> bool:
    """Check if synapse ID is valid."""
    return isinstance(synapse_id, SynapseId) or (
        isinstance(synapse_id, int) and synapse_id >= 0
    )

def is_valid_region_id(region_id: Any) -> bool:
    """Check if region ID is valid."""
    return isinstance(region_id, RegionId) or (
        isinstance(region_id, int) and region_id >= 0
    )

# Type checking and validation

def validate_neuron_id(neuron_id: Any) -> NeuronId:
    """Validate and convert neuron ID."""
    if isinstance(neuron_id, NeuronId):
        return neuron_id
    elif isinstance(neuron_id, int):
        return NeuronId(neuron_id)
    else:
        raise ValueError(f"Invalid neuron ID: {neuron_id}")

def validate_synapse_id(synapse_id: Any) -> SynapseId:
    """Validate and convert synapse ID."""
    if isinstance(synapse_id, SynapseId):
        return synapse_id
    elif isinstance(synapse_id, int):
        return SynapseId(synapse_id)
    else:
        raise ValueError(f"Invalid synapse ID: {synapse_id}")

def validate_region_id(region_id: Any) -> RegionId:
    """Validate and convert region ID."""
    if isinstance(region_id, RegionId):
        return region_id
    elif isinstance(region_id, int):
        return RegionId(region_id)
    else:
        raise ValueError(f"Invalid region ID: {region_id}")

# Collection types

class NeuronIdSet(Set[NeuronId]):
    """Set of neuron IDs with optimized operations."""
    
    def __init__(self, *ids: Union[NeuronId, int]):
        self._set = set()
        for id_ in ids:
            self._set.add(validate_neuron_id(id_))
    
    def add(self, neuron_id: Union[NeuronId, int]):
        """Add neuron ID to set."""
        self._set.add(validate_neuron_id(neuron_id))
    
    def remove(self, neuron_id: Union[NeuronId, int]):
        """Remove neuron ID from set."""
        self._set.remove(validate_neuron_id(neuron_id))
    
    def contains(self, neuron_id: Union[NeuronId, int]) -> bool:
        """Check if neuron ID is in set."""
        return validate_neuron_id(neuron_id) in self._set
    
    def __contains__(self, neuron_id: Union[NeuronId, int]) -> bool:
        return self.contains(neuron_id)
    
    def __len__(self) -> int:
        return len(self._set)
    
    def __iter__(self) -> Iterator[NeuronId]:
        return iter(self._set)
    
    def to_list(self) -> List[NeuronId]:
        """Convert to list."""
        return list(self._set)
    
    def union(self, other: 'NeuronIdSet') -> 'NeuronIdSet':
        """Union with another set."""
        result = NeuronIdSet()
        result._set = self._set.union(other._set)
        return result
    
    def intersection(self, other: 'NeuronIdSet') -> 'NeuronIdSet':
        """Intersection with another set."""
        result = NeuronIdSet()
        result._set = self._set.intersection(other._set)
        return result

# Type guards

def is_neuron_id(obj: Any) -> bool:
    """Type guard for NeuronId."""
    return isinstance(obj, NeuronId)

def is_synapse_id(obj: Any) -> bool:
    """Type guard for SynapseId."""
    return isinstance(obj, SynapseId)

def is_region_id(obj: Any) -> bool:
    """Type guard for RegionId."""
    return isinstance(obj, RegionId)

# Function type definitions

Callback = Callable[[Any], None]
BinaryCallback = Callable[[Any, Any], None]
StepCallback = Callable[[SimulationStep, 'Brain'], None]
MemoryCallback = Callable[['MemoryType', Any], None]
ErrorCallback = Callable[[Exception], None]

# Export commonly used types

__all__ = [
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    # ID types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    # Special constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    # Data structures
    'Position2D', 'ActionParameters', 'SensoryData', 'MemoryTrace',
    'NeuralConnection',
    # Utility functions
    'is_valid_neuron_id', 'is_valid_synapse_id', 'is_valid_region_id',
    'validate_neuron_id', 'validate_synapse_id', 'validate_region_id',
    'NeuronIdSet', 'is_neuron_id', 'is_synapse_id', 'is_region_id',
    # Type variables
    'T', 'K', 'V',
    # Numeric types
    'MembranePotential', 'PopulationSize', 'ConnectionWeight',
    'SimulationStep', 'Timestamp',
    # Type aliases
    'NeuronIdValue', 'SynapseIdValue', 'RegionIdValue', 'PopulationIdValue',
    # NumPy support
    'NUMPY_AVAILABLE', 'ndarray',
]
