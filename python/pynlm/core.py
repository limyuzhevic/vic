"""
Python bindings for NLM core types and configuration.

This module provides Python bindings for NLM's core types and configuration
classes, making them accessible to Python users.
"""

class NeuronId:
    """Unique identifier for a neuron."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this neuron ID."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, NeuronId):
            return self.value == other.value
        return self.value == other
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<NeuronId: {self.value}>"
class SynapseId:
    """Unique identifier for a synapse."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this synapse ID."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, SynapseId):
            return self.value == other.value
        return self.value == other
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<SynapseId: {self.value}>"
class RegionId:
    """Unique identifier for a brain region."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this region ID."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, RegionId):
            return self.value == other.value
        return self.value == other
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<RegionId: {self.value}>"
class PopulationId:
    """Unique identifier for a neuron population."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        """Get the index of this population ID."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, PopulationId):
            return self.value == other.value
        return self.value == other
    
    def __ne__(self, other):
        return not self.__eq__(other)
class NeuronType:
    """Neuron type enumeration."""
    
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5
class SynapseType:
    """Synapse type enumeration."""
    
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4
class DevelopmentalStage:
    """Developmental stage enumeration."""
    
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4
class FiringState:
    """Neuron firing state enumeration."""
    
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3
class ActionType:
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
class MotorCommand:
    """Low-level motor command enumeration."""
    
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7
class WorldObjectType:
    """World object type enumeration."""
    
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4
class Config:
    """Configuration class for NLM system."""
    
    def __init__(self):
        self.data = {}
    
    def loadFromFile(self, filepath: str):
        """Load configuration from a JSON file."""
        import json
        with open(filepath, 'r') as f:
            self.data = json.load(f)
    
    def loadFromArgs(self, argc, argv):
        """Load configuration from command line arguments."""
        # Parse command line arguments
        for i in range(argc):
            if argv[i].startswith('--'):
                key = argv[i][2:]
                if i + 1 < argc:
                    self.data[key] = argv[i + 1]
    
    def saveToFile(self, filepath: str):
        """Save configuration to a JSON file."""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.data, f, indent=2)
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        return key in self.data
    
    def getKeys(self) -> list:
        """Get all configuration keys."""
        return list(self.data.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        self.data.clear()
    
    def summary(self) -> str:
        """Get a summary string of the configuration."""
        return f"Config with {len(self.data)} entries"
    
    def __repr__(self):
        return f"<Config: {self.summary()}>"
class SensoryInput:
    """Base class for sensory input."""
    
    def getType(self):
        """Get the type of sensory input."""
        raise NotImplementedError("Subclasses must implement getType")
    
    def getData(self):
        """Get the raw data as a list."""
        raise NotImplementedError("Subclasses must implement getData")
    
    def getDimensions(self):
        """Get the dimensionality."""
        raise NotImplementedError("Subclasses must implement getDimensions")
    
    def getTimestamp(self):
        """Get the timestamp."""
        raise NotImplementedError("Subclasses must implement getTimestamp")
    
    def setTimestamp(self, timestamp):
        """Set the timestamp."""
        raise NotImplementedError("Subclasses must implement setTimestamp")
class Vision(SensoryInput):
    """Vision sensory input."""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__()
        self._width = width
        self._height = height
        self._channels = channels
        self._data = []
    
    def setData(self, data: list):
        """Set the visual data."""
        self._data = data
    
    def getType(self):
        return "Vision"
    
    def getData(self):
        return self._data
    
    def getDimensions(self):
        return [self._width, self._height, self._channels]
    
    def getWidth(self) -> int:
        return self._width
    
    def getHeight(self) -> int:
        return self._height
    
    def getChannels(self) -> int:
        return self._channels
class Audio(SensoryInput):
    """Audio sensory input."""
    
    def __init__(self, sampleRate: int = 0, numSamples: int = 0):
        super().__init__()
        self._sampleRate = sampleRate
        self._numSamples = numSamples
        self._data = []
    
    def setData(self, data: list):
        """Set the audio data."""
        self._data = data
    
    def setSampleRate(self, sampleRate: int):
        self._sampleRate = sampleRate
    
    def getType(self):
        return "Audio"
    
    def getData(self):
        return self._data
    
    def getDimensions(self):
        return [self._sampleRate, self._numSamples]
    
    def getSampleRate(self) -> int:
        return self._sampleRate
    
    def getNumSamples(self) -> int:
        return self._numSamples
class InternalSignals(SensoryInput):
    """Internal signals sensory input."""
    
    def __init__(self):
        super().__init__()
        self._signals = []
    
    def addSignal(self, value: float):
        """Add a signal value."""
        self._signals.append(value)
    
    def clearSignals(self):
        """Clear all signals."""
        self._signals.clear()
    
    def getType(self):
        return "InternalSignals"
    
    def getData(self):
        return self._signals
    
    def getDimensions(self):
        return [len(self._signals)]
    
    def getTimestamp(self):
        return 0.0
    
    def setTimestamp(self, timestamp):
        pass  # Internal signals don't need timestamp
class Action:
    """Action representation for motor output."""
    
    def __init__(self, actionType: ActionType = ActionType.Wait):
        self._type = actionType
        self._parameters = []
    
    def __init__(self, actionType: ActionType, parameters: list):
        self._type = actionType
        self._parameters = parameters
    
    def getType(self) -> ActionType:
        return self._type
    
    def setType(self, actionType: ActionType):
        self._type = actionType
    
    def getParameters(self) -> list:
        return self._parameters
    
    def setParameters(self, params: list):
        self._parameters = params
    
    def getName(self) -> str:
        return ActionType(actionType).name if hasattr(ActionType, 'name') else str(actionType)
    
    def clone(self):
        """Create a copy of this action."""
        return Action(self._type, self._parameters.copy())
class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
class AgentBody:
    """Agent body state."""
    
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def reset(self):
        """Reset agent body to initial state."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
class ActionResult:
    """Action result from world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message
class SensoryPercept:
    """Sensory percept data."""
    
    def __init__(self):
        self._vision = Vision()
        self._touch = InternalSignals()
        self._internal = InternalSignals()
        self._proprioception = InternalSignals()
        self._audio = Audio()
        self._timestamp = 0.0
    
    def getVision(self) -> Vision:
        return self._vision
    
    def setVision(self, vision: Vision):
        self._vision = vision
    
    def getVisionWidth(self) -> int:
        return self._vision.getWidth()
    
    def getVisionHeight(self) -> int:
        return self._vision.getHeight()
    
    def getTouch(self) -> InternalSignals:
        return self._touch
    
    def setTouch(self, touch: InternalSignals):
        self._touch = touch
    
    def getInternal(self) -> InternalSignals:
        return self._internal
    
    def setInternal(self, internal: InternalSignals):
        self._internal = internal
    
    def getProprioception(self) -> InternalSignals:
        return self._proprioception
    
    def setProprioception(self, proprioception: InternalSignals):
        self._proprioception = proprioception
    
    def getAudio(self) -> Audio:
        return self._audio
    
    def setAudio(self, audio: Audio):
        self._audio = audio
    
    def getAllSignals(self):
        """Get all sensory signals."""
        return {
            'vision': self._vision,
            'touch': self._touch,
            'internal': self._internal,
            'proprioception': self._proprioception,
            'audio': self._audio
        }
    
    def getTimestamp(self):
        return self._timestamp
    
    def setTimestamp(self, timestamp):
        self._timestamp = timestamp

# Global constants
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()
