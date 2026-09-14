# NLM Brain Python Module

"""
Brain module for NLM Python interface
"""

from . import __init__ as nlm_init
from typing import List, Optional, Union, Any
from pathlib import Path

# Re-export from main module
Config = nlm_init.Config
Brain = nlm_init.Brain
createBrain = nlm_init.createBrain
createDefaultConfig = nlm_init.createDefaultConfig
getVersion = nlm_init.getVersion
getInfo = nlm_init.getInfo

# Brain constants
INVALID_NEURON_ID = nlm_init.pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = nlm_init.pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = nlm_init.pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = nlm_init.pynlm.INVALID_POPULATION_ID

# Neuron types
class NeuronType:
    Excitatory = nlm_init.pynlm.NeuronType.Excitatory
    Inhibitory = nlm_init.pynlm.NeuronType.Inhibitory
    Modulatory = nlm_init.pynlm.NeuronType.Modulatory
    Sensory = nlm_init.pynlm.NeuronType.Sensory
    Motor = nlm_init.pynlm.NeuronType.Motor
    Internal = nlm_init.pynlm.NeuronType.Internal

# Synapse types
class SynapseType:
    Excitatory = nlm_init.pynlm.SynapseType.Excitatory
    Inhibitory = nlm_init.pynlm.SynapseType.Inhibitory
    Modulatory = nlm_init.pynlm.SynapseType.Modulatory
    Electrical = nlm_init.pynlm.SynapseType.Electrical
    GapJunction = nlm_init.pynlm.SynapseType.GapJunction

# Developmental stages
class DevelopmentalStage:
    Initial = nlm_init.pynlm.DevelopmentalStage.Initial
    CriticalPeriod = nlm_init.pynlm.DevelopmentalStage.CriticalPeriod
    Maturation = nlm_init.pynlm.DevelopmentalStage.Maturation
    Adult = nlm_init.pynlm.DevelopmentalStage.Adult
    Aging = nlm_init.pynlm.DevelopmentalStage.Aging

# Action types
class ActionType:
    MoveForward = nlm_init.pynlm.ActionType.MoveForward
    MoveBackward = nlm_init.pynlm.ActionType.MoveBackward
    MoveLeft = nlm_init.pynlm.ActionType.MoveLeft
    MoveRight = nlm_init.pynlm.ActionType.MoveRight
    TurnLeft = nlm_init.pynlm.ActionType.TurnLeft
    TurnRight = nlm_init.pynlm.ActionType.TurnRight
    Look = nlm_init.pynlm.ActionType.Look
    LookUp = nlm_init.pynlm.ActionType.LookUp
    LookDown = nlm_init.pynlm.ActionType.LookDown
    Interact = nlm_init.pynlm.ActionType.Interact
    Eat = nlm_init.pynlm.ActionType.Eat
    Drink = nlm_init.pynlm.ActionType.Drink
    Rest = nlm_init.pynlm.ActionType.Rest
    Wait = nlm_init.pynlm.ActionType.Wait
    Custom = nlm_init.pynlm.ActionType.Custom

# Motor commands
class MotorCommand:
    MoveForward = nlm_init.pynlm.MotorCommand.MoveForward
    MoveBackward = nlm_init.pynlm.MotorCommand.MoveBackward
    TurnLeft = nlm_init.pynlm.MotorCommand.TurnLeft
    TurnRight = nlm_init.pynlm.MotorCommand.TurnRight
    LookLeft = nlm_init.pynlm.MotorCommand.LookLeft
    LookRight = nlm_init.pynlm.MotorCommand.LookRight
    Interact = nlm_init.pynlm.MotorCommand.Interact
    Wait = nlm_init.pynlm.MotorCommand.Wait

# World object types
class WorldObjectType:
    Empty = nlm_init.pynlm.WorldObjectType.Empty
    Resource = nlm_init.pynlm.WorldObjectType.Resource
    Hazard = nlm_init.pynlm.WorldObjectType.Hazard
    Wall = nlm_init.pynlm.WorldObjectType.Wall
    Marker = nlm_init.pynlm.WorldObjectType.Marker

# Firing states
class FiringState:
    Resting = nlm_init.pynlm.FiringState.Resting
    Active = nlm_init.pynlm.FiringState.Active
    Refractory = nlm_init.pynlm.FiringState.Refractory
    Inhibited = nlm_init.pynlm.FiringState.Inhibited

# Neuron ID class
class NeuronId:
    def __init__(self, value: int = 0):
        self._id = nlm_init.pynlm.NeuronId(value)
    
    @property
    def value(self) -> int:
        return self._id.value
    
    def index(self) -> int:
        return self._id.index()
    
    def __eq__(self, other):
        if isinstance(other, NeuronId):
            return self._id == other._id
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self._id)
    
    def __repr__(self):
        return f"NeuronId({self.value})"

# Synapse ID class
class SynapseId:
    def __init__(self, value: int = 0):
        self._id = nlm_init.pynlm.SynapseId(value)
    
    @property
    def value(self) -> int:
        return self._id.value
    
    def index(self) -> int:
        return self._id.index()
    
    def __eq__(self, other):
        if isinstance(other, SynapseId):
            return self._id == other._id
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self._id)
    
    def __repr__(self):
        return f"SynapseId({self.value})"

# Region ID class
class RegionId:
    def __init__(self, value: int = 0):
        self._id = nlm_init.pynlm.RegionId(value)
    
    @property
    def value(self) -> int:
        return self._id.value
    
    def index(self) -> int:
        return self._id.index()
    
    def __eq__(self, other):
        if isinstance(other, RegionId):
            return self._id == other._id
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self._id)
    
    def __repr__(self):
        return f"RegionId({self.value})"

# Population ID class
class PopulationId:
    def __init__(self, value: int = 0):
        self._id = nlm_init.pynlm.PopulationId(value)
    
    @property
    def value(self) -> int:
        return self._id.value
    
    def index(self) -> int:
        return self._id.index()
    
    def __eq__(self, other):
        if isinstance(other, PopulationId):
            return self._id == other._id
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self._id)
    
    def __repr__(self):
        return f"PopulationId({self.value})"

# World object class
class WorldObject:
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty, 
                 value: float = 0.0, radius: float = 0.5):
        self._obj = nlm_init.pynlm.WorldObject(x, y, obj_type, value, radius)
    
    @property
    def x(self) -> float:
        return self._obj.x
    
    @x.setter
    def x(self, value: float):
        self._obj.x = value
    
    @property
    def y(self) -> float:
        return self._obj.y
    
    @y.setter
    def y(self, value: float):
        self._obj.y = value
    
    @property
    def type(self) -> WorldObjectType:
        return self._obj.type
    
    @type.setter
    def type(self, value: WorldObjectType):
        self._obj.type = value
    
    @property
    def value(self) -> float:
        return self._obj.value
    
    @value.setter
    def value(self, value: float):
        self._obj.value = value
    
    @property
    def radius(self) -> float:
        return self._obj.radius
    
    @radius.setter
    def radius(self, value: float):
        self._obj.radius = value
    
    @property
    def active(self) -> bool:
        return self._obj.active
    
    @active.setter
    def active(self, value: bool):
        self._obj.active = value
    
    def __repr__(self):
        return f"WorldObject({self.x}, {self.y}, {self.type.name}, {self.value})"

# Agent body class
class AgentBody:
    def __init__(self):
        self._body = nlm_init.pynlm.AgentBody()
    
    @property
    def x(self) -> float:
        return self._body.x
    
    @x.setter
    def x(self, value: float):
        self._body.x = value
    
    @property
    def y(self) -> float:
        return self._body.y
    
    @y.setter
    def y(self, value: float):
        self._body.y = value
    
    @property
    def orientation(self) -> float:
        return self._body.orientation
    
    @orientation.setter
    def orientation(self, value: float):
        self._body.orientation = value
    
    @property
    def velocityX(self) -> float:
        return self._body.velocityX
    
    @velocityX.setter
    def velocityX(self, value: float):
        self._body.velocityX = value
    
    @property
    def velocityY(self) -> float:
        return self._body.velocityY
    
    @velocityY.setter
    def velocityY(self, value: float):
        self._body.velocityY = value
    
    @property
    def angularVelocity(self) -> float:
        return self._body.angularVelocity
    
    @angularVelocity.setter
    def angularVelocity(self, value: float):
        self._body.angularVelocity = value
    
    @property
    def energy(self) -> float:
        return self._body.energy
    
    @energy.setter
    def energy(self, value: float):
        self._body.energy = value
    
    @property
    def health(self) -> float:
        return self._body.health
    
    @health.setter
    def health(self, value: float):
        self._body.health = value
    
    @property
    def age(self) -> float:
        return self._body.age
    
    @age.setter
    def age(self, value: float):
        self._body.age = value
    
    @property
    def isMoving(self) -> bool:
        return self._body.isMoving
    
    @isMoving.setter
    def isMoving(self, value: bool):
        self._body.isMoving = value
    
    @property
    def isTurning(self) -> bool:
        return self._body.isTurning
    
    @isTurning.setter
    def isTurning(self, value: bool):
        self._body.isTurning = value
    
    @property
    def lastActionTime(self) -> float:
        return self._body.lastActionTime
    
    @lastActionTime.setter
    def lastActionTime(self, value: float):
        self._body.lastActionTime = value
    
    def reset(self) -> None:
        self._body.reset()
    
    def __repr__(self):
        return f"AgentBody(pos=({self.x}, {self.y}), vel=({self.velocityX}, {self.velocityY}))"

# Action result class
class ActionResult:
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        self._result = nlm_init.pynlm.ActionResult(reward, success, message)
    
    @property
    def reward(self) -> float:
        return self._result.reward
    
    @reward.setter
    def reward(self, value: float):
        self._result.reward = value
    
    @property
    def success(self) -> bool:
        return self._result.success
    
    @success.setter
    def success(self, value: bool):
        self._result.success = value
    
    @property
    def message(self) -> str:
        return self._result.message
    
    @message.setter
    def message(self, value: str):
        self._result.message = value
    
    def __repr__(self):
        return f"ActionResult(reward={self.reward}, success={self.success}, message='{self.message}')"

# Sensory percept class
class SensoryPercept:
    def __init__(self):
        self._percept = nlm_init.pynlm.SensoryPercept()
    
    def getVision(self) -> Any:
        return self._percept.getVision()
    
    def setVision(self, vision: Any) -> None:
        self._percept.setVision(vision)
    
    def getVisionWidth(self) -> int:
        return self._percept.getVisionWidth()
    
    def getVisionHeight(self) -> int:
        return self._percept.getVisionHeight()
    
    def getTouch(self) -> Any:
        return self._percept.getTouch()
    
    def setTouch(self, touch: Any) -> None:
        self._percept.setTouch(touch)
    
    def getInternal(self) -> List[float]:
        return self._percept.getInternal()
    
    def setInternal(self, internal: List[float]) -> None:
        self._percept.setInternal(internal)
    
    def getProprioception(self) -> Any:
        return self._percept.getProprioception()
    
    def setProprioception(self, proprioception: Any) -> None:
        self._percept.setProprioception(proprioception)
    
    def getAudio(self) -> Any:
        return self._percept.getAudio()
    
    def setAudio(self, audio: Any) -> None:
        self._percept.setAudio(audio)
    
    def getAllSignals(self) -> List[float]:
        return self._percept.getAllSignals()
    
    def getTimestamp(self) -> float:
        return self._percept.getTimestamp()
    
    def setTimestamp(self, timestamp: float) -> None:
        self._percept.setTimestamp(timestamp)
    
    def __repr__(self):
        return f"SensoryPercept(vision={self.getVisionWidth()}x{self.getVisionHeight()}, internal={len(self.getInternal())} signals)"

# Vision class
class Vision:
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        self._vision = nlm_init.pynlm.Vision(width, height, channels)
    
    def setData(self, data: List[float]) -> None:
        self._vision.setData(data)
    
    @property
    def width(self) -> int:
        return self._vision.getWidth()
    
    @property
    def height(self) -> int:
        return self._vision.getHeight()
    
    @property
    def channels(self) -> int:
        return self._vision.getChannels()
    
    def __repr__(self):
        return f"Vision({self.width}x{self.height}, {self.channels} channels)"

# Audio class
class Audio:
    def __init__(self, sampleRate: int = 0, numSamples: int = 0):
        self._audio = nlm_init.pynlm.Audio(sampleRate, numSamples)
    
    def setData(self, data: List[float]) -> None:
        self._audio.setData(data)
    
    def setSampleRate(self, sampleRate: int) -> None:
        self._audio.setSampleRate(sampleRate)
    
    @property
    def sampleRate(self) -> int:
        return self._audio.getSampleRate()
    
    @property
    def numSamples(self) -> int:
        return self._audio.getNumSamples()
    
    def __repr__(self):
        return f"Audio({self.sampleRate} Hz, {self.numSamples} samples)"

# Internal signals class
class InternalSignals:
    def __init__(self):
        self._signals = nlm_init.pynlm.InternalSignals()
    
    def addSignal(self, value: float) -> None:
        self._signals.addSignal(value)
    
    def clearSignals(self) -> None:
        self._signals.clearSignals()
    
    def getSignals(self) -> List[float]:
        return self._signals.getData()
    
    def __repr__(self):
        signals = self.getSignals()
        return f"InternalSignals({len(signals)} signals: {signals})"

# Action class
class Action:
    def __init__(self, actionType: ActionType = ActionType.Wait, parameters: Optional[List[float]] = None):
        if parameters is None:
            parameters = []
        self._action = nlm_init.pynlm.Action(actionType, parameters)
    
    @property
    def type(self) -> ActionType:
        return self._action.getType()
    
    @type.setter
    def type(self, value: ActionType):
        self._action.setType(value)
    
    @property
    def parameters(self) -> List[float]:
        return self._action.getParameters()
    
    @parameters.setter
    def parameters(self, value: List[float]):
        self._action.setParameters(value)
    
    def getName(self) -> str:
        return self._action.getName()
    
    def clone(self) -> "Action":
        return Action(self.type, self.parameters)
    
    def __repr__(self):
        return f"Action({self.type.name}, params={self.parameters})"


__all__ = [
    # Types
    "NeuronType", "SynapseType", "DevelopmentalStage", 
    "ActionType", "MotorCommand", "WorldObjectType", "FiringState",
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    # Classes
    "Brain", "Config", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept", "Vision", "Audio", "InternalSignals", "Action",
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    # Functions
    "createBrain", "createDefaultConfig", "getVersion", "getInfo"
]
