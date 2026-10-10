"""
Complete NLM Python API wrapper matching the official documentation.

This module provides the exact API documented in NLM's official
documentation, ensuring users can write code that matches
the examples and documentation.

This module includes all classes, methods, and functions from
the C++ NLM framework with a complete Python interface that
matches the documented API.
"""

import sys
import os
from typing import Any, List, Dict, Optional, Union

# Try to import from C++ bindings
try:
    from pynlm.bindings import (
        # Core classes
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        
        # Value types
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Enums
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        
        # Factory functions
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    HAVE_NATIVE_BINDINGS = True
    print("[INFO] Using native NLM C++ bindings")
    
except ImportError:
    HAVE_NATIVE_BINDINGS = False
    print("[WARNING] NLM C++ bindings not available, using stub implementation")
    
    # Create stub classes that match the documented API
    class Config:
        def __init__(self):
            self._data = {}
            
        def loadFromFile(self, filepath: str) -> bool:
            """Load configuration from a JSON file"""
            print(f"[STUB] Loading config from {filepath}")
            return True
            
        def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
            """Load configuration from command line arguments"""
            print(f"[STUB] Loading config from {argc} args")
            return True
            
        def saveToFile(self, filepath: str) -> bool:
            """Save configuration to a JSON file"""
            print(f"[STUB] Saving config to {filepath}")
            return True
            
        def has(self, key: str) -> bool:
            """Check if a configuration key exists"""
            return key in self._data
            
        def getKeys(self) -> List[str]:
            """Get all configuration keys"""
            return list(self._data.keys())
            
        def clear(self):
            """Clear all configuration entries"""
            self._data.clear()
            
        def summary(self) -> str:
            """Get a summary string of the configuration"""
            return f"Config ({len(self._data)} entries)"
            
        def set(self, key: str, value: Any, source: int = None):
            """Set a configuration value"""
            self._data[key] = value
            
        def getOr(self, key: str, default_value: Any) -> Any:
            """Get a configuration value or default if not found"""
            return self._data.get(key, default_value)
    
    # Create stub enums
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
    
    # Create stub IDs
    class NeuronId:
        def __init__(self, value: int = 0):
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
    
    # Create stub classes that match the documented API
    class Brain:
        def __init__(self, config: Config):
            self._config = config
            self._initialized = False
            
        def initialize(self):
            """Initialize the brain with configuration"""
            print("[STUB] Brain initialized")
            self._initialized = True
            
        def step(self, current_step: int):
            """Perform a simulation step"""
            if not self._initialized:
                raise RuntimeError("Brain not initialized. Call initialize() first.")
            print(f"[STUB] Brain step {current_step}")
            
        def step(self, current_step: int, current_time: float):
            """Perform a simulation step with timestamp"""
            if not self._initialized:
                raise RuntimeError("Brain not initialized. Call initialize() first.")
            print(f"[STUB] Brain step {current_step} at time {current_time}")
            
        def receiveSensoryInput(self, input_data: Any):
            """Inject sensory input into the brain"""
            print(f"[STUB] Received sensory input: {input_data}")
            
        def injectCurrent(self, neuron: Any, current: float):
            """Inject current into a specific neuron"""
            print(f"[STUB] Injecting {current} current into neuron {neuron}")
            
        def injectCurrentToNeurons(self, neuron_type: Any, current: float):
            """Inject current into all neurons of a specific type"""
            print(f"[STUB] Injecting {current} current to neurons of type {neuron_type}")
            
        def produceAction(self) -> Any:
            """Produce motor action based on neural activity"""
            print("[STUB] Producing action")
            from .core import Action
            return Action(ActionType.WAIT)
            
        def reset(self):
            """Reset brain state"""
            print("[STUB] Brain reset")
            self._initialized = False
            
        def save(self, filepath: str) -> bool:
            """Save brain state to file"""
            print(f"[STUB] Saving brain to {filepath}")
            return True
            
        def load(self, filepath: str) -> bool:
            """Load brain state from file"""
            print(f"[STUB] Loading brain from {filepath}")
            return True
            
        def addRegion(self, name: str = "") -> Any:
            """Add a new neural region"""
            print(f"[STUB] Adding region: {name}")
            return RegionId()
            
        def getRegion(self, region_id: Any) -> Any:
            """Get a region by ID"""
            print(f"[STUB] Getting region {region_id}")
            return None
            
        def getRegionCount(self) -> int:
            """Get the number of regions"""
            return 1
            
        def getRegionIds(self) -> List[Any]:
            """Get all region IDs"""
            return [RegionId()]
            
        def getRegions(self) -> List[Any]:
            """Get all regions"""
            return []
            
        def getTotalNeuronCount(self) -> int:
            """Get total neuron count across all regions"""
            return 100
            
        def getTotalSynapseCount(self) -> int:
            """Get total synapse count across all regions"""
            return 500
            
        def getActiveNeuronCount(self) -> int:
            """Get count of active neurons"""
            return 10
            
        def getFiringNeuronCount(self) -> int:
            """Get count of currently firing neurons"""
            return 5
            
        def getAverageFiringRate(self) -> float:
            """Get average firing rate across all neurons"""
            return 2.5
            
        def getExcitationInhibitionRatio(self) -> float:
            """Get excitation/inhibition balance ratio"""
            return 1.0
            
        def getTotalSpikeCount(self) -> int:
            """Get total spike count"""
            return 25
            
        def getDevelopmentalStage(self):
            """Get current developmental stage"""
            return DevelopmentalStage.Adult
            
        def setDevelopmentalStage(self, stage):
            """Set developmental stage"""
            print(f"[STUB] Setting developmental stage to {stage}")
            
        def getConfig(self) -> Config:
            """Get the configuration"""
            return self._config
            
        def logStatus(self):
            """Log brain status"""
            print("[STUB] Brain status logged")
    
    class AgentBrain:
        def __init__(self, brain: Brain):
            self._brain = brain
            
        def initialize(self, world: Any):
            """Initialize with world"""
            print(f"[STUB] AgentBrain initialized with world")
            
        def getSensoryInputSize(self) -> int:
            """Get expected sensory input size"""
            return 1024
            
        def getMotorOutputSize(self) -> int:
            """Get expected motor output size"""
            return 128
            
        def processSensoryInput(self, percept: Any):
            """Process sensory percept and inject into brain"""
            print(f"[STUB] Processing sensory input: {percept}")
            
        def decodeMotorCommand(self) -> Any:
            """Decode brain motor activity into motor command"""
            print("[STUB] Decoding motor command")
            from .core import Action
            return Action(ActionType.WAIT)
            
        def applyRewardModulation(self, reward: float, predicted_reward: float):
            """Apply reward-based neuromodulation"""
            print(f"[STUB] Applying reward modulation: {reward}")
            
        def updateDevelopment(self, timestep: float):
            """Update development system"""
            print(f"[STUB] Updating development: {timestep}")
            
        def getDevelopmentalStage(self):
            """Get current developmental stage"""
            return DevelopmentalStage.Adult
            
        def getNeuromodulationLevel(self) -> float:
            """Get current neuromodulation level"""
            return 0.5
            
        def getCuriosityLevel(self) -> float:
            """Get curiosity level"""
            return 0.3
            
        def getNoveltyLevel(self) -> float:
            """Get novelty level"""
            return 0.2
            
        def getPredictionError(self) -> float:
            """Get prediction error"""
            return 0.1
            
        def reset(self):
            """Reset agent for new episode"""
            print("[STUB] AgentBrain reset")
            
        def getBrain(self) -> Brain:
            """Get the underlying brain"""
            return self._brain
            
        def enableRewardModulation(self, enable: bool):
            """Enable reward modulation"""
            print(f"[STUB] Reward modulation {'enabled' if enable else 'disabled'}")
            
        def enableStructuralPlasticity(self, enable: bool):
            """Enable structural plasticity"""
            print(f"[STUB] Structural plasticity {'enabled' if enable else 'disabled'}")
            
        def enableDevelopment(self, enable: bool):
            """Enable development"""
            print(f"[STUB] Development {'enabled' if enable else 'disabled'}")
            
        def enableCuriosity(self, enable: bool):
            """Enable curiosity"""
            print(f"[STUB] Curiosity {'enabled' if enable else 'disabled'}")
            
        def isRewardModulationEnabled(self) -> bool:
            """Check if reward modulation is enabled"""
            return False
            
        def isStructuralPlasticityEnabled(self) -> bool:
            """Check if structural plasticity is enabled"""
            return False
            
        def isDevelopmentEnabled(self) -> bool:
            """Check if development is enabled"""
            return False
            
        def isCuriosityEnabled(self) -> bool:
            """Check if curiosity is enabled"""
            return False
    
    class SimpleWorld:
        def __init__(self):
            self._width = 20
            self._height = 20
            self._vision_width = 8
            self._vision_height = 8
            
        def configure(self, width: int, height: int, vision_width: int, vision_height: int):
            self._width = width
            self._height = height
            self._vision_width = vision_width
            self._vision_height = vision_height
            print(f"[STUB] World configured: {width}x{height}, vision: {vision_width}x{vision_height}")
            
        def reset(self):
            print("[STUB] World reset")
            
        def setAgentStart(self, x: float, y: float):
            print(f"[STUB] Agent start set to: ({x}, {y})")
            
        def update(self, timestep: float):
            print(f"[STUB] World update: {timestep}")
            
        def applyMotorCommand(self, cmd: Any, current_time: float) -> Any:
            print(f"[STUB] Applying motor command: {cmd} at time {current_time}")
            return ActionResult()
            
        def getSensoryPercept(self) -> Any:
            print("[STUB] Getting sensory percept")
            return SensoryPercept()
            
        def getAgentBody(self) -> Any:
            print("[STUB] Getting agent body")
            return AgentBody()
            
        def addObject(self, obj: Any):
            print(f"[STUB] Adding world object: {obj}")
            
        def removeObject(self, x: float, y: float):
            print(f"[STUB] Removing object at: ({x}, {y})")
            
        def isValidPosition(self, x: float, y: float) -> bool:
            return 0 <= x < self._width and 0 <= y < self._height
            
        def getWidth(self) -> int:
            return self._width
            
        def getHeight(self) -> int:
            return self._height
            
        def getMaxEnergy(self) -> float:
            return 100.0
            
        def setMaxEnergy(self, e: float):
            print(f"[STUB] Max energy set to: {e}")
            
        def getEnergyDecayRate(self) -> float:
            return 0.01
            
        def setEnergyDecayRate(self, r: float):
            print(f"[STUB] Energy decay rate set to: {r}")
            
        def getSimulationTime(self) -> float:
            return 0.0
            
        def setRandomSeed(self, seed: int):
            print(f"[STUB] Random seed set to: {seed}")
            
        def getRandomSeed(self) -> int:
            return 42
    
    # Helper classes
    class Action:
        def __init__(self, action_type: ActionType = ActionType.WAIT, parameters: List[float] = None):
            self._type = action_type
            self._parameters = parameters or []
            
        def getType(self) -> ActionType:
            return self._type
            
        def setType(self, action_type: ActionType):
            self._type = action_type
            
        def getParameters(self) -> List[float]:
            return self._parameters
            
        def setParameters(self, params: List[float]):
            self._parameters = params
            
        def getName(self) -> str:
            return self._type.name
            
        def clone(self) -> Any:
            return Action(self._type, self._parameters.copy())
    
    class WorldObject:
        def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty, 
                     value: float = 0.0, radius: float = 0.5):
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
        def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
            self.reward = reward
            self.success = success
            self.message = message
    
    class SensoryPercept:
        def __init__(self):
            pass
            
        def getVision(self) -> Any:
            return Vision(8, 8)
            
        def setVision(self, vision: Any):
            pass
            
        def getVisionWidth(self) -> int:
            return 8
            
        def getVisionHeight(self) -> int:
            return 8
            
        def getTouch(self) -> Any:
            return None
            
        def setTouch(self, touch: Any):
            pass
            
        def getInternal(self) -> List[float]:
            return []
            
        def setInternal(self, internal: List[float]):
            pass
            
        def getProprioception(self) -> List[float]:
            return []
            
        def setProprioception(self, proprioception: List[float]):
            pass
            
        def getAudio(self) -> Any:
            return Audio(44100, 16384)
            
        def setAudio(self, audio: Any):
            pass
            
        def getAllSignals(self) -> List[float]:
            return []
            
        def getTimestamp(self) -> float:
            return 0.0
            
        def setTimestamp(self, timestamp: float):
            pass
    
    class Vision:
        def __init__(self, width: int, height: int, channels: int = 3):
            self._width = width
            self._height = height
            self._channels = channels
            self._data = []
            
        def getWidth(self) -> int:
            return self._width
            
        def getHeight(self) -> int:
            return self._height
            
        def getChannels(self) -> int:
            return self._channels
            
        def setData(self, data: List[float]):
            self._data = data
            
        def getData(self) -> List[float]:
            return self._data
    
    class Audio:
        def __init__(self, sample_rate: int, num_samples: int):
            self._sample_rate = sample_rate
            self._num_samples = num_samples
            self._data = []
            
        def getSampleRate(self) -> int:
            return self._sample_rate
            
        def setSampleRate(self, sample_rate: int):
            self._sample_rate = sample_rate
            
        def getNumSamples(self) -> int:
            return self._num_samples
            
        def setData(self, data: List[float]):
            self._data = data
            
        def getData(self) -> List[float]:
            return self._data
    
    class InternalSignals:
        def __init__(self):
            self._signals = []
            
        def addSignal(self, value: float):
            self._signals.append(value)
            
        def clearSignals(self):
            self._signals.clear()
            
        def getSignals(self) -> List[float]:
            return self._signals
    
    class RegionId:
        def __init__(self, value: int = 0):
            self.value = value
            
        def index(self):
            return self.value % 1000
            
        def __eq__(self, other):
            return isinstance(other, RegionId) and self.value == other.value
            
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __repr__(self):
            return f"<RegionId: {self.value}>"
    
    # Factory functions
    def createDefaultConfig() -> Config:
        return Config()
        
    def createBrain(config: Config) -> Brain:
        return Brain(config)
        
    def createSimpleWorld() -> SimpleWorld:
        return SimpleWorld()
        
    def createAgentBrain(brain: Brain) -> AgentBrain:
        return AgentBrain(brain)
        
    # Constants
    INVALID_NEURON_ID = NeuronId()
    INVALID_SYNAPSE_ID = NeuronId()
    INVALID_REGION_ID = RegionId()
    INVALID_POPULATION_ID = PopulationId()
# Re-export all symbols for easy access
__all__ = [
    # Core classes
    'Config',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Enums
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Main classes
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    
    # Sensory classes
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    
    # Motor classes
    'Action',
    'WorldObject',
    
    # Agent classes
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]

# Make sure core classes are available at module level
from .core import *
from .brain import *
from .agent import *
from .world import *
from .sensory import *
from .motor import *
from .neuromodulation import *
from .memory import *
from .cognition import *

# Provide help and documentation
__doc__ = """
NLM (Neural Learning Machine) - Complete Python API

A brain-inspired spiking neural network simulator with Python bindings.

This module provides the exact API documented in NLM's official
documentation, ensuring users can write code that matches
the examples and documentation.

Key Features:
- LIF (Leaky Integrate-and-Fire) neuron dynamics
- Multiple memory systems (working, episodic, semantic, procedural)
- Plasticity mechanisms (STDP, Hebbian, structural)
- Neuromodulation (dopamine, curiosity, novelty)
- Cognitive functions (concept formation, planning, attention)
- Developmental stages affecting plasticity
- Environment simulation with sensory-motor loops
- Checkpoint save/load functionality

Usage Examples:
    import pynlm

    # Create and initialize
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Run simulation
    for step in range(100):
        brain.step(step)

    # Get statistics
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Spikes: {brain.getTotalSpikeCount()}")
"""

def get_help():
    """Return documentation string for the NLM Python package."""
    return __doc__

def print_help():
    """Print help documentation."""
    print(get_help())