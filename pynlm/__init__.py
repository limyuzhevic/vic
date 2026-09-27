"""Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"""

import sys
from pathlib import Path

# Try to import the actual pybind11-generated module
try:
    import pynlm_core
    HAS_BINDINGS = True
except ImportError:
    HAS_BINDINGS = False
    pynlm_core = None

# Check if we have a native module (built with scikit-build)
if not HAS_BINDINGS:
    try:
        # Try importing from the package location
        import importlib.util
        spec = importlib.util.find_spec("pynlm")
        if spec and spec.origin:
            # Found the module, but let's try to import it
            import pynlm
            HAS_BINDINGS = True
    except ImportError:
        pass

# Forward declarations for Python API
try:
    from typing import Optional, List, Dict, Any, Union, Callable
    from enum import Enum
except ImportError:
    # Fallback for older Python versions
    typing_extensions = True
    typing = {}

# Make sure the main module is accessible
__version__ = "0.1.0"
__all__ = [
    'Config',
    'createBrain',
    'createDefaultConfig',
    'createSimpleWorld',
    'createAgentBrain',
    'createDefaultConfig',
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'WorldObjectType',
]

if HAS_BINDINGS:
    # Import the actual Python bindings
    from pynlm_core import (
        Config,
        createBrain,
        createDefaultConfig,
        createSimpleWorld,
        createAgentBrain,
        NeuronType,
        DevelopmentalStage,
        ActionType,
        WorldObjectType,
    )
else:
    # Create stub classes when bindings are not available
    class Config:
        """Configuration object (stub)"""
        def __init__(self):
            pass
        
        def set(self, key: str, value) -> None:
            pass
        
        def get(self, key: str) -> Any:
            return None
        
        def has(self, key: str) -> bool:
            return False
        
        def saveToFile(self, filepath: str) -> bool:
            return False
        
        def loadFromFile(self, filepath: str) -> bool:
            return False
    
    class NeuronType(Enum):
        """Neuron types (stub)"""
        Excitatory = 0
        Inhibitory = 1
        Sensory = 2
        Motor = 3
        Modulatory = 4
        Internal = 5
    
    class DevelopmentalStage(Enum):
        """Developmental stages (stub)"""
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class ActionType(Enum):
        """Action types (stub)"""
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7
        Eat = 8
        Drink = 9
        Sleep = 10
    
    class WorldObjectType(Enum):
        """World object types (stub)"""
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    def createBrain(config: Config) -> 'Brain':
        """Create a neural network brain (stub)"""
        return Brain()
    
    def createDefaultConfig() -> Config:
        """Create a default configuration (stub)"""
        return Config()
    
    def createSimpleWorld() -> 'SimpleWorld':
        """Create a simple world simulation (stub)"""
        return SimpleWorld()
    
    def createAgentBrain(brain: 'Brain') -> 'AgentBrain':
        """Create an agent brain interface (stub)"""
        return AgentBrain(brain)

# Stub classes for when bindings aren't available
class Brain:
    """Stub Brain class"""
    def __init__(self):
        self._initialized = False
    
    def initialize(self) -> bool:
        """Initialize the brain (stub)"""
        self._initialized = True
        return True
    
    def step(self, step_number: int) -> None:
        """Run one simulation step (stub)"""
        pass
    
    def reset(self) -> None:
        """Reset brain state (stub)"""
        pass
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count (stub)"""
        return 1000
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count (stub)"""
        return 5000
    
    def getFiringNeuronCount(self) -> int:
        """Get firing neuron count (stub)"""
        return 50
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate (stub)"""
        return 0.5
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get E/I ratio (stub)"""
        return 1.5
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count (stub)"""
        return 100
    
    def getPendingSpikeEventCount(self) -> int:
        """Get pending spike events (stub)"""
        return 0
    
    def getActiveNeuronCount(self) -> int:
        """Get active neuron count (stub)"""
        return 75
    
    def getFiringNeuronCount(self) -> int:
        """Get firing neuron count (stub)"""
        return 50
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate (stub)"""
        return 0.5
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count (stub)"""
        return 100
    
    def getPendingSpikeEventCount(self) -> int:
        """Get pending spike events (stub)"""
        return 0
    
    def getActiveNeuronCount(self) -> int:
        """Get active neuron count (stub)"""
        return 75
    
    def getFiringNeuronCount(self) -> int:
        """Get firing neuron count (stub)"""
        return 50
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate (stub)"""
        return 0.5
    
    def getDevelopmentStage(self) -> 'DevelopmentalStage':
        """Get developmental stage (stub)"""
        return DevelopmentalStage.Initial
    
    def save(self, filepath: str) -> bool:
        """Save brain state (stub)"""
        return True
    
    def load(self, filepath: str) -> bool:
        """Load brain state (stub)"""
        return True
    
    def addRegion(self, name: str = "") -> int:
        """Add neural region (stub)"""
        return 0
    
    def getRegion(self, region_id: int) -> Any:
        """Get neural region (stub)"""
        return None

class SimpleWorld:
    """Stub SimpleWorld class"""
    def __init__(self):
        self._width = 10
        self._height = 10
        self._vision_width = 8
        self._vision_height = 8
        self._time = 0.0
    
    def configure(self, width: int = 10, height: int = 10, 
                  vision_width: int = 8, vision_height: int = 8) -> None:
        """Configure world dimensions (stub)"""
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
    
    def reset(self) -> None:
        """Reset world to initial state (stub)"""
        pass
    
    def update(self, timestep: float) -> None:
        """Update world simulation (stub)"""
        self._time += timestep
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position (stub)"""
        pass
    
    def applyMotorCommand(self, action: Any, current_time: float) -> None:
        """Apply motor command to world (stub)"""
        pass
    
    def getSensoryPercept(self) -> Any:
        """Get sensory percept from world (stub)"""
        return None
    
    def getAgentBody(self) -> Any:
        """Get agent body state (stub)"""
        return None
    
    def addObject(self, obj: Any) -> None:
        """Add world object (stub)"""
        pass
    
    def removeObject(self, x: float, y: float) -> None:
        """Remove world object (stub)"""
        pass
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid (stub)"""
        return True
    
    def getWidth(self) -> int:
        """Get world width (stub)"""
        return self._width
    
    def getHeight(self) -> int:
        """Get world height (stub)"""
        return self._height
    
    def getSimulationTime(self) -> float:
        """Get simulation time (stub)"""
        return self._time
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed (stub)"""
        pass

class AgentBrain:
    """Stub AgentBrain class"""
    def __init__(self, brain: Brain):
        self._brain = brain
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize agent with world (stub)"""
        pass
    
    def processSensoryInput(self, percept: Any) -> None:
        """Process sensory input (stub)"""
        pass
    
    def decodeMotorCommand(self) -> Any:
        """Decode brain activity to motor command (stub)"""
        return ActionType.Wait
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward modulation (stub)"""
        pass
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system (stub)"""
        pass
    
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable/disable reward modulation (stub)"""
        pass
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity (stub)"""
        pass
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable/disable development (stub)"""
        pass
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable/disable curiosity (stub)"""
        pass
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level (stub)"""
        return 0.5
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level (stub)"""
        return 0.3
    
    def getPredictionError(self) -> float:
        """Get prediction error (stub)"""
        return 0.1
    
    def getNeuromodulationLevel(self) -> float:
        """Get neuromodulation level (stub)"""
        return 0.2
    
    def getDevelopmentalStage(self) -> 'DevelopmentalStage':
        """Get developmental stage (stub)"""
        return DevelopmentalStage.Initial

# Create instances for easy import
_config = None

def _get_default_config():
    """Get or create default config instance"""
    global _config
    if _config is None:
        _config = Config()
    return _config