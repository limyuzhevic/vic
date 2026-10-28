# Enumerations for NLM
from enum import Enum
from typing import Optional

class NeuronType(Enum):
    """Types of neurons in NLM."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    SENSORY = 3
    MOTOR = 4
    INTERNAL = 5

class SynapseType(Enum):
    """Types of synapses in NLM."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    ELECTRICAL = 3
    GAP_JUNCTION = 4

class DevelopmentalStage(Enum):
    """Developmental stages of NLM brain."""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

class FiringState(Enum):
    """Firing states of neurons."""
    RESTING = 0
    ACTIVE = 1
    REFRACTORY = 2
    INHIBITED = 3

class ActionType(Enum):
    """Types of motor actions."""
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
    """Motor commands for action selection."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7

class WorldObjectType(Enum):
    """Types of objects in the world."""
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

# Unique identifiers
class NeuronId:
    """Unique identifier for a neuron."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this neuron."""
        return self.value
    
    def __eq__(self, other) -> bool:
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    def __ne__(self, other) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<NeuronId: {self.value}>"

class SynapseId:
    """Unique identifier for a synapse."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this synapse."""
        return self.value
    
    def __eq__(self, other) -> bool:
        if isinstance(other, SynapseId):
            return self.value == other.value
        return False
    
    def __ne__(self, other) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<SynapseId: {self.value}>"

class RegionId:
    """Unique identifier for a brain region."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this region."""
        return self.value
    
    def __eq__(self, other) -> bool:
        if isinstance(other, RegionId):
            return self.value == other.value
        return False
    
    def __ne__(self, other) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<RegionId: {self.value}>"

class PopulationId:
    """Unique identifier for a neuron population."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def __eq__(self, other) -> bool:
        if isinstance(other, PopulationId):
            return self.value == other.value
        return False
    
    def __ne__(self, other) -> bool:
        return not self.__eq__(other)
    
    def __repr__(self) -> str:
        return f"<PopulationId: {self.value}>"

# Invalid constants
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)
