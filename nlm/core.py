# Core NLM classes implementation

"""
Core NLM classes providing the C++ bindings interface for the enhanced Python API.

This module provides both stub implementations for testing and delegates to C++
extensions when available.
"""

import warnings
from typing import Optional, List, Tuple, Dict, Any, Union

# Try to import C++ extensions first
try:
    import pynlm
    C_EXTENSIONS_AVAILABLE = True
except ImportError:
    C_EXTENSIONS_AVAILABLE = False
    warnings.warn("NLM C++ extensions not available - using stub implementations")

if C_EXTENSIONS_AVAILABLE:
    # Import from C++ bindings
    from pynlm import (
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage,
        FiringState, ActionType, MotorCommand,
        WorldObjectType,
        Config, Brain, AgentBrain, SimpleWorld,
        SensoryInput, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult,
        SensoryPercept
    )
    
    # Constants for invalid IDs (delegated to C++ bindings)
    INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
    INVALID_SYNAPSE_ID = pynlm.INVALID_SYNAPSE_ID
    INVALID_REGION_ID = pynlm.INVALID_REGION_ID
    INVALID_POPULATION_ID = pynlm.INVALID_POPULATION_ID
else:
    # Create simple stub implementations for testing when C++ extensions aren't available
    from enum import Enum
    
    # Basic type definitions
    class NeuronId:
        """Unique identifier for a neuron."""
        def __init__(self, value: int = 0):
            self.value = value
        
        def index(self) -> int:
            """Get the index value of this neuron ID."""
            return self.value
        
        def __eq__(self, other):
            return isinstance(other, NeuronId) and self.value == other.value
        
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __repr__(self):
            return f"NeuronId({self.value})"
    
    class SynapseId:
        """Unique identifier for a synapse."""
        def __init__(self, value: int = 0):
            self.value = value
    
    class RegionId:
        """Unique identifier for a brain region."""
        def __init__(self, value: int = 0):
            self.value = value
    
    class PopulationId:
        """Unique identifier for a neuron population."""
        def __init__(self, value: int = 0):
            self.value = value
    
    # Enumeration types
    class NeuronType(Enum):
        """Types of neurons in the NLM system."""
        EXCITATORY = 1
        INHIBITORY = 2
        MODULATORY = 3
        SENSORY = 4
        MOTOR = 5
        INTERNAL = 6
    
    class SynapseType(Enum):
        """Types of synapses in the NLM system."""
        EXCITATORY = 1
        INHIBITORY = 2
        MODULATORY = 3
        ELECTRICAL = 4
        GAP_JUNCTION = 5
    
    class DevelopmentalStage(Enum):
        """Developmental stages for neural development."""
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
        """Types of actions neurons can produce."""
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
        """Motor commands for action execution."""
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
    
    # Core NLM classes
    class Config:
        """Configuration class for NLM system parameters."""
        
        def __init__(self):
            """Initialize empty configuration."""
            self._data = {}
        
        def loadFromFile(self, filepath: str) -> bool:
            """
            Load configuration from a file.
            
            Args:
                filepath: Path to configuration file
                
            Returns:
                True if successful, False otherwise
            """
            # Simple key=value file parser
            try:
                with open(filepath, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            self._data[key.strip()] = value.strip()
                return True
            except Exception:
                return False
        
        def has(self, key: str) -> bool:
            """Check if a configuration key exists."""
            return key in self._data
        
        def getKeys(self) -> List[str]:
            """Get all configuration keys."""
            return list(self._data.keys())
        
        def clear(self):
            """Clear all configuration entries."""
            self._data.clear()
        
        def summary(self) -> str:
            """Get a summary string of the configuration."""
            return f"Config with {len(self._data)} entries"
        
        def set(self, key: str, value: Any):
            """Set a configuration value."""
            self._data[key] = value
        
        def get(self, key: str, default: Any = None) -> Any:
            """Get a configuration value."""
            return self._data.get(key, default)
    
    class Brain:
        """Central neural simulation brain class."""
        
        def __init__(self, config=None):
            """Initialize brain with optional configuration."""
            self.config = config or Config()
            self._initialized = False
            self._step_count = 0
            self._total_spikes = 0
            self._total_neurons = 1000
            self._total_synapses = 10000
        
        def initialize(self) -> bool:
            """Initialize the brain with configuration."""
            self._initialized = True
            return True
        
        def step(self, step_number, current_time=None):
            """
            Perform a simulation step.
            
            Args:
                step_number: Current simulation step
                current_time: Current simulation time (optional)
            """
            if not self._initialized:
                raise RuntimeError("Brain not initialized. Call initialize() first.")
            self._step_count += 1
            self._total_spikes += 1  # Simulated for stub
        
        def reset(self):
            """Reset brain state to initial conditions."""
            self._initialized = False
            self._step_count = 0
            self._total_spikes = 0
        
        def getTotalNeuronCount(self) -> int:
            """Get total neuron count across all regions."""
            return self._total_neurons
        
        def getTotalSynapseCount(self) -> int:
            """Get total synapse count across all regions."""
            return self._total_synapses
        
        def getFiringNeuronCount(self) -> int:
            """Get count of currently firing neurons."""
            return 10  # Simulated for stub
        
        def getAverageFiringRate(self) -> float:
            """Get average firing rate across all neurons."""
            return 1.0  # Simulated for stub
        
        def getTotalSpikeCount(self) -> int:
            """Get total spike count."""
            return self._total_spikes
        
        def getDevelopmentalStage(self):
            """Get current developmental stage."""
            class Stage(Enum):
                INITIAL = 0
            return Stage.INITIAL
    
    class AgentBrain:
        """Agent brain interface connecting NLM brain to world."""
        
        def __init__(self, brain):
            """Initialize agent brain with reference to neural brain."""
            self.brain = brain
            self._reward_mod_enabled = False
            self._plasticity_enabled = False
            self._development_enabled = False
            self._curiosity_enabled = False
            self._curiosity_level = 0.0
            self._novelty_level = 0.0
        
        def initialize(self, world):
            """Initialize with world environment."""
            pass
        
        def getSensoryInputSize(self) -> int:
            """Get expected sensory input size."""
            return 256 + 8 + 4 + 6  # Vision + touch + internal + proprioception
        
        def getMotorOutputSize(self) -> int:
            """Get expected motor output size."""
            return 6  # One motor neuron per action
        
        def processSensoryInput(self, percept):
            """Process sensory percept and inject into brain."""
            pass
        
        def decodeMotorCommand(self):
            """Decode brain motor activity into motor command."""
            return MotorCommand.WAIT
        
        def applyRewardModulation(self, reward, predictedReward):
            """Apply reward-based neuromodulation."""
            pass
        
        def updateDevelopment(self, timestep):
            """Update development system."""
            pass
        
        def reset(self):
            """Reset agent for new episode."""
            self._curiosity_level = 0.0
            self._novelty_level = 0.0
        
        def enableRewardModulation(self, enable):
            """Enable or disable reward modulation."""
            self._reward_mod_enabled = enable
        
        def enableStructuralPlasticity(self, enable):
            """Enable or disable structural plasticity."""
            self._plasticity_enabled = enable
        
        def enableDevelopment(self, enable):
            """Enable or disable development."""
            self._development_enabled = enable
        
        def enableCuriosity(self, enable):
            """Enable or disable curiosity-driven exploration."""
            self._curiosity_enabled = enable
        
        def getDevelopmentalStage(self):
            """Get current developmental stage."""
            if not self.brain:
                from enum import Enum
                class Stage(Enum):
                    INITIAL = 0
                return Stage.INITIAL
            return self.brain.getDevelopmentalStage()
        
        def getNeuromodulationLevel(self) -> float:
            """Get current neuromodulation level."""
            return 0.5 if self._reward_mod_enabled else 0.0
        
        def getCuriosityLevel(self) -> float:
            """Get curiosity level."""
            return self._curiosity_level
        
        def getNoveltyLevel(self) -> float:
            """Get novelty level."""
            return self._novelty_level
        
        def getPredictionError(self) -> float:
            """Get prediction error."""
            return 0.0
    
    class SimpleWorld:
        """Simple 2D world for NLM simulation."""
        
        def __init__(self):
            """Initialize world with default parameters."""
            self._width = 20.0
            self._height = 20.0
            self._vision_width = 16
            self._vision_height = 16
            self._sim_time = 0.0
        
        def configure(self, width: float, height: float, 
                     visionWidth: int, visionHeight: int):
            """Configure world dimensions and agent sensing."""
            self._width = width
            self._height = height
            self._vision_width = visionWidth
            self._vision_height = visionHeight
        
        def reset(self):
            """Reset world to initial state."""
            self._sim_time = 0.0
        
        def update(self, timestep: float):
            """Update world by given timestep."""
            self._sim_time += timestep
        
        def getSensoryPercept(self) -> 'SensoryPercept':
            """Get sensory percept from current world state."""
            return SensoryPercept()
        
        def getAgentBody(self) -> 'AgentBody':
            """Get agent body state."""
            return AgentBody()
        
        def setAgentStart(self, x: float, y: float):
            """Set agent starting position."""
            pass
        
        def applyMotorCommand(self, cmd, currentTime: float):
            """Apply motor command to world."""
            class ActionResult:
                def __init__(self):
                    self.reward = 0.0
                    self.success = True
                    self.message = ""
            return ActionResult()
        
        def getWidth(self) -> float:
            """Get world width."""
            return self._width
        
        def getHeight(self) -> float:
            """Get world height."""
            return self._height
        
        def getSimulationTime(self) -> float:
            """Get current simulation time."""
            return self._sim_time
    
    class SensoryPercept:
        """Sensory percept data from world."""
        
        def __init__(self):
            """Initialize with default values."""
            self.vision_data = [0.0] * 256
            self.touch_data = [0.0] * 8
            self.internal_data = [0.0] * 4
            self.proprioception_data = [0.0] * 6
            self.audio_data = []
            self.timestamp = 0.0
        
        def getVision(self) -> List[float]:
            """Get vision sensory data."""
            return self.vision_data
        
        def getTouch(self) -> List[float]:
            """Get touch sensory data."""
            return self.touch_data
        
        def getInternal(self) -> List[float]:
            """Get internal signals sensory data."""
            return self.internal_data
        
        def getProprioception(self) -> List[float]:
            """Get proprioception sensory data."""
            return self.proprioception_data
        
        def getAudio(self) -> List[float]:
            """Get audio sensory data."""
            return self.audio_data
    
    class AgentBody:
        """Agent body state representation."""
        
        def __init__(self):
            """Initialize agent body with default state."""
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
            """Reset agent body to initial state."""
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

# Factory function for creating default configuration
def create_default_config() -> 'Config':
    """
    Create a default NLM configuration with balanced parameters.
    
    Returns:
        Default configuration object
    """
    config = Config()
    
    # Set default values for a balanced simulation
    config.set('neuron_count', 1000)
    config.set('region_count', 1)
    config.set('connection_probability', 0.1)
    config.set('simulation_timestep', 0.001)
    config.set('random_seed', 42)
    config.set('plasticity.stdp.enable', True)
    config.set('plasticity.hebbian.enable', True)
    config.set('plasticity.structural.enable', True)
    config.set('neuromod.curiosity.enable', True)
    config.set('neuromod.novelty.enable', True)
    config.set('neuromod.dopamine.enable', True)
    config.set('replay_interval', 100)
    config.set('development_interval', 1000)
    
    return config

# Factory function for creating brain with optional configuration
def create_brain(config: Optional['Config'] = None) -> 'Brain':
    """
    Create a new NLM brain with optional configuration.
    
    Args:
        config: NLM configuration. If None, uses default configuration.
    
    Returns:
        Initialized Brain object.
    
    Raises:
        InitializationError: If brain fails to initialize
    """
    if config is None:
        config = create_default_config()
    
    try:
        brain = Brain(config)
        if not brain.initialize():
            raise InitializationError("Brain failed to initialize")
        return brain
    except Exception as e:
        if isinstance(e, InitializationError):
            raise
        raise InitializationError(f"Failed to create brain: {e}")

# Export public API
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Factory functions
    'create_default_config', 'create_brain',
    
    # Extensions availability
    'C_EXTENSIONS_AVAILABLE'
]
