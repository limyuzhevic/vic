"""NLM type definitions and basic types."""

# Basic type definitions (mirrors C++ Types.hpp)
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int

# Type aliases for convenience
SimulationStep = int
Timestamp = float
MembranePotential = float
Delay = int
SynapticWeight = float
FiringRate = float
NeuronHandle = int
SynapseHandle = int

# Constants
INVALID_NEURON_ID = 0xFFFFFFFFFFFFFFFF  # UINT64_MAX
INVALID_SYNAPSE_ID = 0xFFFFFFFFFFFFFFFF
INVALID_REGION_ID = 0xFFFFFFFFFFFFFFFF
INVALID_POPULATION_ID = 0xFFFFFFFFFFFFFFFF

# Enums
class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class ActionType:
    Wait = 0
    MoveForward = 1
    MoveBackward = 2
    TurnLeft = 3
    TurnRight = 4

class WorldObjectType:
    Agent = 0
    Obstacle = 1
    Food = 2
    Water = 3

class FiringState:
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

class MotorCommand:
    Stop = 0
    Start = 1
    Accelerate = 2
    Decelerate = 3

# Plasticity flags
class PlasticityFlags:
    def __init__(self):
        self.hebbian = False
        self.stdp = False
        self.reward_modulated = False
        self.structural = False
        self.eligible = False

# Type conversions for C++ compatibility
def makeNeuronId(value):
    return value

def makeSynapseId(value):
    return value

def makeRegionId(value):
    return value

def makePopulationId(value):
    return value

# Python classes to mirror C++ types
class _NeuronId:
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, _NeuronId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<NeuronId: {self.value}>"
    
    def index(self):
        return self.value

class _SynapseId:
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, _SynapseId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<SynapseId: {self.value}>"
    
    def index(self):
        return self.value

class _RegionId:
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, _RegionId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<RegionId: {self.value}>"
    
    def index(self):
        return self.value

class _PopulationId:
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, _PopulationId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<PopulationId: {self.value}>"
    
    def index(self):
        return self.value

# Export types
NeuronId = _NeuronId
SynapseId = _SynapseId
RegionId = _RegionId
PopulationId = _PopulationId

# Deprecated type aliases (keeping for backward compatibility)
# These should ideally be replaced with proper wrapper types

__all__ = [
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'MembranePotential', 'Delay',
    'SynapticWeight', 'FiringRate', 'NeuronHandle', 'SynapseHandle',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID', 'NeuronType', 'SynapseType',
    'DevelopmentalStage', 'ActionType', 'WorldObjectType',
    'FiringState', 'MotorCommand', 'PlasticityFlags',
    'makeNeuronId', 'makeSynapseId', 'makeRegionId', 'makePopulationId'
]
