"""
Core NLM classes and types.

This module contains the fundamental classes and types used throughout NLM.
"""

import sys

# Try to import from the C++ bindings if available
try:
    from pynlm.bindings import (
        # Configuration
        Config,
        createDefaultConfig,
        
        # Identifiers
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Enumerations
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    # Check if main classes are available
    try:
        from pynlm.bindings import Brain, AgentBrain, SimpleWorld
        HAVE_MAIN_CLASSES = True
    except ImportError:
        HAVE_MAIN_CLASSES = False
        Brain = AgentBrain = SimpleWorld = None
        
except ImportError:
    # Fallback: create basic classes for development/testing
    print("Warning: NLM C++ bindings not available. Creating stub classes.")
    
    # Create stub classes for development
    class Config:
        def __init__(self):
            self._data = {}
        
        def loadFromFile(self, filepath):
            print(f"[STUB] Loading config from {filepath}")
            return True
            
        def loadFromArgs(self, argc, argv):
            print(f"[STUB] Loading config from {argc} args")
            return True
            
        def saveToFile(self, filepath):
            print(f"[STUB] Saving config to {filepath}")
            return True
            
        def has(self, key):
            return key in self._data
            
        def getKeys(self):
            return list(self._data.keys())
            
        def clear(self):
            self._data.clear()
            
        def summary(self):
            return f"Config ({len(self._data)} entries)"
            
        def set(self, key, value, source=None):
            self._data[key] = value
            
        def getOr(self, key, default_value):
            return self._data.get(key, default_value)
    
    class NeuronId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value % 1000
            
        def __eq__(self, other):
            return isinstance(other, NeuronId) and self.value == other.value
            
        def __ne__(self, other):
            return not self.__eq__(other)
            
        def __hash__(self):
            return hash(self.value)
            
        def __repr__(self):
            return f"<NeuronId: {self.value}>"
    
    # Create stub enumerations
    from enum import Enum
    
    class NeuronType(Enum):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Sensory = 3
        Motor = 4
        Internal = 5
    
    class SynapseType(Enum):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Electrical = 3
        GapJunction = 4
    
    class DevelopmentalStage(Enum):
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class FiringState(Enum):
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3
    
    class ActionType(Enum):
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
    
    class MotorCommand(Enum):
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7
    
    class WorldObjectType(Enum):
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    # Create stub identifiers
    INVALID_NEURON_ID = NeuronId()
    INVALID_SYNAPSE_ID = SynapseId() = NeuronId()
    INVALID_REGION_ID = RegionId() = NeuronId()
    INVALID_POPULATION_ID = PopulationId() = NeuronId()
    
    # Create stub classes for missing components
    class Brain:
        def __init__(self, config):
            self._config = config
            self._initialized = False
            
        def initialize(self):
            print("[STUB] Brain initialized")
            self._initialized = True
            
        def step(self, current_step, current_time=0.0):
            if not self._initialized:
                raise RuntimeError("Brain not initialized. Call initialize() first.")
            print(f"[STUB] Brain step {current_step}")
            
        def receiveSensoryInput(self, input_data):
            print(f"[STUB] Received sensory input: {input_data}")
            
        def injectCurrent(self, neuron, current):
            print(f"[STUB] Injecting {current} current into neuron {neuron}")
            
        def injectCurrentToNeurons(self, neuron_type, current):
            print(f"[STUB] Injecting {current} current to neurons of type {neuron_type}")
            
        def produceAction(self):
            print("[STUB] Producing action")
            return Action(ActionType.WAIT)
            
        def reset(self):
            print("[STUB] Brain reset")
            self._initialized = False
            
        def save(self, filepath):
            print(f"[STUB] Saving brain to {filepath}")
            return True
            
        def load(self, filepath):
            print(f"[STUB] Loading brain from {filepath}")
            return True
            
        def addRegion(self, name=""):
            print(f"[STUB] Adding region: {name}")
            return RegionId()
            
        def getRegion(self, region_id):
            print(f"[STUB] Getting region {region_id}")
            return None
            
        def getRegionCount(self):
            return 1
            
        def getRegionIds(self):
            return [RegionId()]
            
        def getRegions(self):
            return []
            
        def getTotalNeuronCount(self):
            return 100
            
        def getTotalSynapseCount(self):
            return 500
            
        def getActiveNeuronCount(self):
            return 10
            
        def getFiringNeuronCount(self):
            return 5
            
        def getAverageFiringRate(self):
            return 2.5
            
        def getExcitationInhibitionRatio(self):
            return 1.0
            
        def getTotalSpikeCount(self):
            return 25
            
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Adult
            
        def setDevelopmentalStage(self, stage):
            print(f"[STUB] Setting developmental stage to {stage}")
            
        def getConfig(self):
            return self._config
            
        def logStatus(self):
            print("[STUB] Brain status logged")
    
    class AgentBrain:
        def __init__(self, brain):
            self._brain = brain
            
        def initialize(self, world):
            print(f"[STUB] AgentBrain initialized with world")
            
        def getSensoryInputSize(self):
            return 1024
            
        def getMotorOutputSize(self):
            return 128
            
        def processSensoryInput(self, percept):
            print(f"[STUB] Processing sensory input: {percept}")
            
        def decodeMotorCommand(self):
            print("[STUB] Decoding motor command")
            return Action(ActionType.WAIT)
            
        def applyRewardModulation(self, reward, predicted_reward):
            print(f"[STUB] Applying reward modulation: {reward}")
            
        def updateDevelopment(self, timestep):
            print(f"[STUB] Updating development: {timestep}")
            
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Adult
            
        def getNeuromodulationLevel(self):
            return 0.5
            
        def getCuriosityLevel(self):
            return 0.3
            
        def getNoveltyLevel(self):
            return 0.2
            
        def getPredictionError(self):
            return 0.1
            
        def reset(self):
            print("[STUB] AgentBrain reset")
            
        def getBrain(self):
            return self._brain
            
        def enableRewardModulation(self, enable):
            print(f"[STUB] Reward modulation {'enabled' if enable else 'disabled'}")
            
        def enableStructuralPlasticity(self, enable):
            print(f"[STUB] Structural plasticity {'enabled' if enable else 'disabled'}")
            
        def enableDevelopment(self, enable):
            print(f"[STUB] Development {'enabled' if enable else 'disabled'}")
            
        def enableCuriosity(self, enable):
            print(f"[STUB] Curiosity {'enabled' if enable else 'disabled'}")
            
        def isRewardModulationEnabled(self):
            return False
            
        def isStructuralPlasticityEnabled(self):
            return False
            
        def isDevelopmentEnabled(self):
            return False
            
        def isCuriosityEnabled(self):
            return False
    
    class SimpleWorld:
        def __init__(self):
            self._width = 20
            self._height = 20
            self._vision_width = 8
            self._vision_height = 8
            
        def configure(self, width, height, vision_width, vision_height):
            self._width = width
            self._height = height
            self._vision_width = vision_width
            self._vision_height = vision_height
            print(f"[STUB] World configured: {width}x{height}, vision: {vision_width}x{vision_height}")
            
        def reset(self):
            print("[STUB] World reset")
            
        def setAgentStart(self, x, y):
            print(f"[STUB] Agent start set to: ({x}, {y})")
            
        def update(self, timestep):
            print(f"[STUB] World update: {timestep}")
            
        def applyMotorCommand(self, cmd, current_time):
            print(f"[STUB] Applying motor command: {cmd} at time {current_time}")
            
        def getSensoryPercept(self):
            print("[STUB] Getting sensory percept")
            return SensoryPercept()
            
        def getAgentBody(self):
            print("[STUB] Getting agent body")
            return AgentBody()
            
        def addObject(self, obj):
            print(f"[STUB] Adding world object: {obj}")
            
        def removeObject(self, x, y):
            print(f"[STUB] Removing object at: ({x}, {y})")
            
        def isValidPosition(self, x, y):
            return 0 <= x < self._width and 0 <= y < self._height
            
        def getWidth(self):
            return self._width
            
        def getHeight(self):
            return self._height
            
        def getMaxEnergy(self):
            return 100.0
            
        def setMaxEnergy(self, e):
            print(f"[STUB] Max energy set to: {e}")
            
        def getEnergyDecayRate(self):
            return 0.01
            
        def setEnergyDecayRate(self, r):
            print(f"[STUB] Energy decay rate set to: {r}")
            
        def getSimulationTime(self):
            return 0.0
            
        def setRandomSeed(self, seed):
            print(f"[STUB] Random seed set to: {seed}")
            
        def getRandomSeed(self):
            return 42
    
    class Action:
        def __init__(self, action_type=None, parameters=None):
            self._type = action_type or ActionType.WAIT
            self._parameters = parameters or []
            
        def getType(self):
            return self._type
            
        def setType(self, action_type):
            self._type = action_type
            
        def getParameters(self):
            return self._parameters
            
        def setParameters(self, params):
            self._parameters = params
            
        def getName(self):
            return self._type.name
            
        def clone(self):
            return Action(self._type, self._parameters.copy())
    
    class WorldObject:
        def __init__(self, x=0.0, y=0.0, obj_type=WorldObjectType.Empty, value=0.0, radius=0.5):
            self.x = x
            self.y = y
            self.type = obj_type
            self.value = value
            self.radius = radius
            self.active = True
    
    class AgentBody:
        def __init__(self):
            self.x = 0.0
            self.y = 0.0
            self.orientation = 0.0
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
            self.x = self.y = 0.0
            self.orientation = 0.0
            self.velocityX = self.velocityY = 0.0
            self.angularVelocity = 0.0
            self.energy = 100.0
            self.health = 100.0
            self.age = 0.0
            self.isMoving = self.isTurning = False
            self.lastActionTime = 0.0
    
    class ActionResult:
        def __init__(self, reward=0.0, success=False, message=""):
            self.reward = reward
            self.success = success
            self.message = message
    
    class SensoryPercept:
        def __init__(self):
            pass
            
        def getVision(self):
            return Vision(self._vision_width, self._vision_height) if hasattr(self, '_vision_width') else Vision(8, 8)
            
        def setVision(self, vision):
            pass
            
        def getVisionWidth(self):
            return 8
            
        def getVisionHeight(self):
            return 8
            
        def getTouch(self):
            return None
            
        def setTouch(self, touch):
            pass
            
        def getInternal(self):
            return []
            
        def setInternal(self, internal):
            pass
            
        def getProprioception(self):
            return []
            
        def setProprioception(self, proprioception):
            pass
            
        def getAudio(self):
            return Audio(48000, 44100) if hasattr(self, '_sample_rate') else Audio(44100, 16384)
            
        def setAudio(self, audio):
            pass
            
        def getAllSignals(self):
            return []
            
        def getTimestamp(self):
            return 0.0
            
        def setTimestamp(self, timestamp):
            pass
    
    class Vision:
        def __init__(self, width, height, channels=3):
            self._width = width
            self._height = height
            self._channels = channels
            self._data = []
            
        def getWidth(self):
            return self._width
            
        def getHeight(self):
            return self._height
            
        def getChannels(self):
            return self._channels
            
        def setData(self, data):
            self._data = data
            
        def getData(self):
            return self._data
    
    class Audio:
        def __init__(self, sample_rate, num_samples):
            self._sample_rate = sample_rate
            self._num_samples = num_samples
            self._data = []
            
        def getSampleRate(self):
            return self._sample_rate
            
        def setSampleRate(self, sample_rate):
            self._sample_rate = sample_rate
            
        def getNumSamples(self):
            return self._num_samples
            
        def setData(self, data):
            self._data = data
            
        def getData(self):
            return self._data
    
    class InternalSignals:
        def __init__(self):
            self._signals = []
            
        def addSignal(self, value):
            self._signals.append(value)
            
        def clearSignals(self):
            self._signals.clear()
            
        def getSignals(self):
            return self._signals
    
    class RegionId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value % 1000
            
        def __eq__(self, other):
            return isinstance(other, RegionId) and self.value == other.value
            
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __repr__(self):
            return f"<RegionId: {self.value}>"

    class PopulationId:
        def __init__(self, value=0):
            self.value = value
            
        def index(self):
            return self.value % 1000
            
        def __eq__(self, other):
            return isinstance(other, PopulationId) and self.value == other.value
            
        def __ne__(self, other):
            return not self.__eq__(other)

    def createDefaultConfig():
        return Config()
        
    def createBrain(config):
        return Brain(config)
        
    def createSimpleWorld():
        return SimpleWorld()
        
    def createAgentBrain(brain):
        return AgentBrain(brain)

# Mark that we have stub classes
HAVE_STUB_CLASSES = True