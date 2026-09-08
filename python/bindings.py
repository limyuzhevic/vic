"""Python bindings for NLM C++ neural simulation framework."""

import sys
import os

# Add current directory to path for relative imports
if os.path.dirname(__file__) not in sys.path:
    sys.path.insert(0, os.path.dirname(__file__))

# The actual C++ bindings are in bindings.cpp
# This file exposes them to Python through pybind11
# For now, we'll create a stub module that will be populated
# by the actual pybind11 bindings

__version__ = "0.1.0"

# Try to import from the compiled module
# This will be populated by the pybind11 compilation process
try:
    from .bindings import *
    
    # Re-export all symbols
    __all__ = [
        # Config
        "Config",
        "createDefaultConfig",
        
        # Brain
        "Brain", 
        "createBrain",
        
        # World
        "SimpleWorld",
        "createSimpleWorld",
        
        # Agent
        "AgentBrain",
        "createAgentBrain",
        
        # Data classes
        "Action",
        "ActionType",
        "WorldObject",
        "WorldObjectType",
        "SensoryInput",
        "Vision",
        "Audio",
        "InternalSignals",
        "SensoryPercept",
        "AgentBody",
        "ActionResult",
        
        # ID classes
        "NeuronId",
        "SynapseId",
        "RegionId",
        "PopulationId",
        
        # Enumerations
        "NeuronType",
        "SynapseType",
        "DevelopmentalStage",
        "FiringState",
        "MotorCommand",
    ]
    
except ImportError:
    # Fallback: create basic classes for development/testing
    # These will be replaced by actual bindings when compiled
    
    class Config:
        """Configuration class for NLM system."""
        def __init__(self):
            self._data = {}
        
        def set(self, key, value, source=None):
            self._data[key] = value
        
        def get(self, key):
            return self._data.get(key)
        
        def has(self, key):
            return key in self._data
        
        def getOr(self, key, default):
            return self._data.get(key, default)
    
    class Brain:
        """Central neural simulation brain class."""
        def __init__(self, config=None):
            self.config = config or Config()
            self._initialized = False
        
        def initialize(self):
            self._initialized = True
            return True
        
        def step(self, currentStep, currentTime=None):
            if not self._initialized:
                self.initialize()
            return self
        
        def reset(self):
            self._initialized = False
        
        def receiveSensoryInput(self, input_data):
            return self
        
        def getTotalNeuronCount(self):
            return 1000
        
        def getTotalSynapseCount(self):
            return 10000
        
        def getFiringNeuronCount(self):
            return 100
        
        def getAverageFiringRate(self):
            return 10.0
        
        def getTotalSpikeCount(self):
            return 5000
        
        def produceAction(self):
            return Action(ActionType.Wait)
        
        def save(self, filepath):
            return True
        
        def load(self, filepath):
            return True
        
        def getRegionCount(self):
            return 1
        
        def getWorkingMemory(self):
            return None
        
        def getEpisodicMemory(self):
            return None
        
        def getAssociativeMemory(self):
            return None
        
        def getPredictionSystem(self):
            return None
        
        def getPlanner(self):
            return None
        
        def getConceptFormation(self):
            return None
        
        def getAttention(self):
            return None
        
        def getDevelopmentSystem(self):
            return None
        
        def getDopamine(self):
            return None
        
        def getCuriosity(self):
            return None
        
        def getNovelty(self):
            return None
        
        def getPredictionErrorSignal(self):
            return None
        
        def logStatus(self):
            pass
    
    class SimpleWorld:
        """Simple 2D world for NLM simulation."""
        def __init__(self):
            self._agent_x = 10.0
            self._agent_y = 10.0
            self._sim_time = 0.0
        
        def configure(self, width, height, visionWidth, visionHeight):
            self.width = width
            self.height = height
            self.visionWidth = visionWidth
            self.visionHeight = visionHeight
        
        def reset(self):
            self._agent_x = 10.0
            self._agent_y = 10.0
            self._sim_time = 0.0
        
        def setAgentStart(self, x, y):
            self._agent_x = x
            self._agent_y = y
        
        def update(self, timestep):
            self._sim_time += timestep
            return self
        
        def applyMotorCommand(self, cmd, currentTime):
            return ActionResult(0.0, True, "Action applied")
        
        def getSensoryPercept(self):
            return SensoryPercept()
        
        def getAgentBody(self):
            return AgentBody(self._agent_x, self._agent_y)
        
        def getWidth(self):
            return self.width
        
        def getHeight(self):
            return self.height
        
        def getSimulationTime(self):
            return self._sim_time
    
    class AgentBrain:
        """Agent brain interface connecting NLM brain to world."""
        def __init__(self, brain):
            self.brain = brain
            self._initialized = False
        
        def initialize(self, world):
            self.world = world
            self._initialized = True
        
        def processSensoryInput(self, percept):
            return self
        
        def decodeMotorCommand(self):
            return MotorCommand.Wait
        
        def applyRewardModulation(self, reward, predictedReward):
            return self
        
        def updateDevelopment(self, timestep):
            return self
        
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Initial
        
        def getNeuromodulationLevel(self):
            return 0.0
        
        def getCuriosityLevel(self):
            return 0.0
        
        def getNoveltyLevel(self):
            return 0.0
        
        def getPredictionError(self):
            return 0.0
        
        def reset(self):
            self._initialized = False
        
        def getSensoryInputSize(self):
            return 300
        
        def getMotorOutputSize(self):
            return 6
        
        def enableRewardModulation(self, enable):
            pass
        
        def enableStructuralPlasticity(self, enable):
            pass
        
        def enableDevelopment(self, enable):
            pass
        
        def enableCuriosity(self, enable):
            pass
        
        def isRewardModulationEnabled(self):
            return False
        
        def isStructuralPlasticityEnabled(self):
            return False
        
        def isDevelopmentEnabled(self):
            return False
        
        def isCuriosityEnabled(self):
            return False
    
    # Supporting classes
    
    class Action:
        def __init__(self, action_type=ActionType.Wait, parameters=None):
            self.type = action_type
            self.parameters = parameters or []
        
        def getType(self):
            return self.type
        
        def setType(self, action_type):
            self.type = action_type
        
        def getParameters(self):
            return self.parameters
        
        def setParameters(self, params):
            self.parameters = params
        
        def getName(self):
            return str(self.type)
        
        def clone(self):
            return Action(self.type, self.parameters.copy())
    
    class ActionType:
        Wait = 0
        MoveForward = 1
        MoveBackward = 2
        TurnLeft = 3
        TurnRight = 4
        LookLeft = 5
        LookRight = 6
        Interact = 7
    
    class WorldObject:
        def __init__(self, x=0.0, y=0.0, type=WorldObjectType.Empty, value=0.0, radius=0.5):
            self.x = x
            self.y = y
            self.type = type
            self.value = value
            self.radius = radius
    
    class WorldObjectType:
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    class SensoryInput:
        def __init__(self):
            pass
        
        def getType(self):
            return 0
        
        def getData(self):
            return []
        
        def getDimensions(self):
            return 0
        
        def getTimestamp(self):
            return 0.0
        
        def setTimestamp(self, timestamp):
            pass
    
    class Vision(SensoryInput):
        def __init__(self, width=0, height=0, channels=3):
            super().__init__()
            self._width = width
            self._height = height
            self._channels = channels
            self._data = []
        
        def setData(self, data):
            self._data = data
        
        def getWidth(self):
            return self._width
        
        def getHeight(self):
            return self._height
        
        def getChannels(self):
            return self._channels
        
        def getData(self):
            return self._data
    
    class Audio(SensoryInput):
        def __init__(self, sampleRate=0, numSamples=0):
            super().__init__()
            self._sampleRate = sampleRate
            self._numSamples = numSamples
            self._data = []
        
        def setData(self, data):
            self._data = data
        
        def setSampleRate(self, sampleRate):
            self._sampleRate = sampleRate
        
        def getSampleRate(self):
            return self._sampleRate
        
        def getNumSamples(self):
            return self._numSamples
    
    class InternalSignals(SensoryInput):
        def __init__(self):
            super().__init__()
            self._signals = []
        
        def addSignal(self, value):
            self._signals.append(value)
        
        def clearSignals(self):
            self._signals.clear()
        
        def getSignal(self, index):
            return self._signals[index] if index < len(self._signals) else 0.0
        
        def getNumSignals(self):
            return len(self._signals)
    
    class SensoryPercept:
        def __init__(self):
            self._vision = []
            self._vision_width = 0
            self._vision_height = 0
            self._touch = []
            self._internal = []
            self._proprioception = []
            self._audio = []
            self._timestamp = 0.0
        
        def setVision(self, vision):
            self._vision = vision
            self._vision_width = int(len(vision) ** 0.5) if vision else 0
            self._vision_height = self._vision_width
        
        def getVision(self):
            return self._vision
        
        def getVisionWidth(self):
            return self._vision_width
        
        def getVisionHeight(self):
            return self._vision_height
        
        def setTouch(self, touch):
            self._touch = touch
        
        def getTouch(self):
            return self._touch
        
        def setInternal(self, internal):
            self._internal = internal
        
        def getInternal(self):
            return self._internal
        
        def setProprioception(self, proprioception):
            self._proprioception = proprioception
        
        def getProprioception(self):
            return self._proprioception
        
        def setAudio(self, audio):
            self._audio = audio
        
        def getAudio(self):
            return self._audio
        
        def getAllSignals(self):
            all_signals = []
            all_signals.extend(self._vision)
            all_signals.extend(self._touch)
            all_signals.extend(self._internal)
            all_signals.extend(self._proprioception)
            all_signals.extend(self._audio)
            return all_signals
        
        def getTimestamp(self):
            return self._timestamp
        
        def setTimestamp(self, timestamp):
            self._timestamp = timestamp
    
    class AgentBody:
        def __init__(self, x=0.0, y=0.0, orientation=0.0):
            self.x = x
            self.y = y
            self.orientation = orientation
            self.velocityX = 0.0
            self.velocityY = 0.0
            self.angularVelocity = 0.0
            self.energy = 100.0
            self.health = 100.0
            self.age = 0.0
            self.isMoving = False
            self.isTurning = False
            self.lastActionTime = 0.0
        
        def reset(self):
            self.energy = 100.0
            self.health = 100.0
            self.age = 0.0
            self.isMoving = False
            self.isTurning = False
    
    class ActionResult:
        def __init__(self, reward=0.0, success=True, message=""):
            self.reward = reward
            self.success = success
            self.message = message
    
    class AgentBody:
        def __init__(self, x=0.0, y=0.0, orientation=0.0):
            self.x = x
            self.y = y
            self.orientation = orientation
            self.velocityX = 0.0
            self.velocityY = 0.0
            self.angularVelocity = 0.0
            self.energy = 100.0
            self.health = 100.0
            self.age = 0.0
            self.isMoving = False
            self.isTurning = False
            self.lastActionTime = 0.0
        
        def reset(self):
            self.energy = 100.0
            self.health = 100.0
            self.age = 0.0
            self.isMoving = False
            self.isTurning = False
    
    class ActionResult:
        def __init__(self, reward=0.0, success=True, message=""):
            self.reward = reward
            self.success = success
            self.message = message
    
    # ID types
    class NeuronId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value
        
        def __eq__(self, other):
            return isinstance(other, NeuronId) and self.value == other.value
        
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __hash__(self):
            return hash(self.value)
    
    class SynapseId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value
        
        def __eq__(self, other):
            return isinstance(other, SynapseId) and self.value == other.value
        
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __hash__(self):
            return hash(self.value)
    
    class RegionId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value
        
        def __eq__(self, other):
            return isinstance(other, RegionId) and self.value == other.value
        
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __hash__(self):
            return hash(self.value)
    
    class PopulationId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value
    
    # Enumerations
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
    
    class FiringState:
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3
    
    class MotorCommand:
        Wait = 0
        MoveForward = 1
        MoveBackward = 2
        TurnLeft = 3
        TurnRight = 4
        LookLeft = 5
        LookRight = 6
        Interact = 7

# Factory functions
def createDefaultConfig():
    """Create a default NLM configuration."""
    return Config()

def createBrain(config=None):
    """Create a new NLM brain with configuration."""
    return Brain(config)

def createSimpleWorld():
    """Create a new simple world."""
    return SimpleWorld()

def createAgentBrain(brain):
    """Create a new agent brain interface."""
    return AgentBrain(brain)
