# Type stubs for pybind11 module integration
# This file provides type hints and improved API for the pynlm module

from typing import Any, Dict, List, Optional, Union, Tuple, overload
from enum import Enum
import json

# Type stubs - these will be replaced by actual imports from the pybind11 module
class _Config:
    pass

class _Brain:
    pass

class _AgentBrain:
    pass

class _SimpleWorld:
    pass

class _SensoryInput:
    pass

class _Vision(_SensoryInput):
    pass

class _Audio(_SensoryInput):
    pass

class _InternalSignals(_SensoryInput):
    pass

class _Action:
    pass

class _WorldObject:
    pass

class _AgentBody:
    pass

class _ActionResult:
    pass

class _SensoryPercept:
    pass

class _NeuronId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...

class _SynapseId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...

class _RegionId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...

class _PopulationId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...

# Actual types that will be imported from pybind11
Config = _Config
Brain = _Brain
AgentBrain = _AgentBrain
SimpleWorld = _SimpleWorld
SensoryInput = _SensoryInput
Vision = _Vision
Audio = _Audio
InternalSignals = _InternalSignals
Action = _Action
WorldObject = _WorldObject
AgentBody = _AgentBody
ActionResult = _ActionResult
SensoryPercept = _SensoryPercept

# Enum type stubs
class NeuronType(str, Enum):
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    SENSORY = "Sensory"
    MOTOR = "Motor"
    INTERNAL = "Internal"

class SynapseType(str, Enum):
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    ELECTRICAL = "Electrical"
    GAPJUNCTION = "GapJunction"

class DevelopmentalStage(str, Enum):
    INITIAL = "Initial"
    CRITICALPERIOD = "CriticalPeriod"
    MATURATION = "Maturation"
    ADULT = "Adult"
    AGING = "Aging"

class FiringState(str, Enum):
    RESTING = "Resting"
    ACTIVE = "Active"
    REFRACTORY = "Refractory"
    INHIBITED = "Inhibited"

class ActionType(str, Enum):
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    MOVELEFT = "MoveLeft"
    MOVERIGHT = "MoveRight"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOK = "Look"
    LOOKUP = "LookUp"
    LOOKDOWN = "LookDown"
    INTERACT = "Interact"
    EAT = "Eat"
    DRINK = "Drink"
    REST = "Rest"
    WAIT = "Wait"
    CUSTOM = "Custom"

class MotorCommand(str, Enum):
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOKLEFT = "LookLeft"
    LOOKRIGHT = "LookRight"
    INTERACT = "Interact"
    WAIT = "Wait"

class WorldObjectType(str, Enum):
    EMPTY = "Empty"
    RESOURCE = "Resource"
    HAZARD = "Hazard"
    WALL = "Wall"
    MARKER = "Marker"

# ID type stubs
class NeuronId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...

class SynapseId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...

class RegionId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...

class PopulationId:
    value: int
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...

# Module-level functions with type hints
@overload
def createDefaultConfig() -> Config: ...

def createDefaultConfig() -> Config: ...

@overload
def createBrain(config: Config) -> Brain: ...

def createBrain(config: Config) -> Brain: ...

@overload
def createSimpleWorld() -> SimpleWorld: ...

def createSimpleWorld() -> SimpleWorld: ...

@overload
def createAgentBrain(brain: Brain) -> AgentBrain: ...

def createAgentBrain(brain: Brain) -> AgentBrain: ...

# Invalid ID constants
INVALID_NEURON_ID: NeuronId
INVALID_SYNAPSE_ID: SynapseId
INVALID_REGION_ID: RegionId
INVALID_POPULATION_ID: PopulationId

# Type aliases for common types
FloatArray = List[float]
IntArray = List[int]
Matrix = List[List[float]]
ConfigDict = Dict[str, Union[int, float, bool, str, List[str]]]
Vector3D = Tuple[float, float, float]
Color = Tuple[float, float, float, float]

__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # ID types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Invalid IDs
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Type aliases
    'FloatArray', 'IntArray', 'Matrix', 'ConfigDict', 'Vector3D', 'Color',
    
    # Module functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'get_version', 'configure_logging',
    'validate_config', 'list_available_configs',
    
    # Version info
    '__version__', '__author__',
]