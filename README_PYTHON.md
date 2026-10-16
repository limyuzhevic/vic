# NLM Python API Documentation

## Overview

The NLM Python API provides a high-level interface to the Neural Learning Machine (NLM) computational brain framework. This documentation covers the Python bindings, examples, and best practices for working with NLM in Python.

## Installation

### From Source (Recommended)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install the package
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build

# Install from wheel
pip install dist/*.whl
```

### Quick Start

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Core API Reference

### Brain Class

```python
class Brain:
    def __init__(self, config: Config)
        """Create a new brain with configuration."""
    
    def initialize(self) -> bool
        """Initialize the brain with configuration. Returns True on success."""
    
    def step(self, step_number: int) -> None
        """Perform a simulation step."""
    
    def step(self, step_number: int, time: float) -> None
        """Perform a simulation step with timestamp."""
    
    def receiveSensoryInput(self, input: SensoryInput) -> None
        """Inject sensory input into the brain."""
    
    def injectCurrent(self, neuron: NeuronId, current: float) -> None
        """Inject current into a specific neuron."""
    
    def injectCurrentToNeurons(self, neuron_type: NeuronType, current: float) -> None
        """Inject current into all neurons of a specific type."""
    
    def produceAction(self) -> Action
        """Produce motor action based on neural activity."""
    
    def reset(self) -> None
        """Reset brain state."""
    
    def save(self, filepath: str) -> bool
        """Save brain state to file."""
    
    def load(self, filepath: str) -> bool
        """Load brain state from file."""
    
    def addRegion(self, name: str = "") -> RegionId
        """Add a new neural region."""
    
    # Statistics and properties
    def getTotalNeuronCount(self) -> int
    def getTotalSynapseCount(self) -> int
    def getActiveNeuronCount(self) -> int
    def getFiringNeuronCount(self) -> int
    def getAverageFiringRate(self) -> float
    def getTotalSpikeCount(self) -> int
    def getPendingSpikeEventCount(self) -> int
    def getExcitationInhibitionRatio(self) -> float
    def getDevelopmentalStage(self) -> DevelopmentalStage
    def setDevelopmentalStage(self, stage: DevelopmentalStage) -> None
    def getConfig(self) -> Config
    def logStatus(self) -> None
```

### Config Class

```python
class Config:
    def __init__(self)
        """Create a default configuration."""
    
    def loadFromFile(self, filepath: str) -> bool
        """Load configuration from a JSON file."""
    
    def loadFromArgs(self, argc: int, argv: list) -> bool
        """Load configuration from command line arguments."""
    
    def saveToFile(self, filepath: str) -> bool
        """Save configuration to a JSON file."""
    
    def has(self, key: str) -> bool
        """Check if a configuration key exists."""
    
    def get(self, key: str) -> Optional[Any]
        """Get configuration value. Returns None if not found."""
    
    def getOr(self, key: str, default: Any) -> Any
        """Get configuration value with default."""
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.Runtime) -> None
        """Set configuration value."""
    
    def remove(self, key: str) -> None
        """Remove configuration key."""
    
    def getKeys(self) -> list
        """Get all configuration keys."""
    
    def clear(self) -> None
        """Clear all configuration entries."""
    
    def summary(self) -> str
        """Get configuration summary."""
```

### AgentBrain Class

```python
class AgentBrain:
    def __init__(self, brain: Brain)
        """Create an agent brain interface."""
    
    def initialize(self, world: SimpleWorld) -> None
        """Initialize with world."""
    
    def getSensoryInputSize(self) -> int
        """Get expected sensory input size."""
    
    def getMotorOutputSize(self) -> int
        """Get expected motor output size."""
    
    def processSensoryInput(self, percept: SensoryPercept) -> None
        """Process sensory percept and inject into brain."""
    
    def decodeMotorCommand(self) -> MotorCommand
        """Decode brain motor activity into motor command."""
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None
        """Apply reward-based neuromodulation."""
    
    def updateDevelopment(self, timestep: float) -> None
        """Update development system."""
    
    def getDevelopmentalStage(self) -> DevelopmentalStage
        """Get current developmental stage."""
    
    def getNeuromodulationLevel(self) -> float
        """Get current neuromodulation level."""
    
    def getCuriosityLevel(self) -> float
        """Get curiosity level."""
    
    def getNoveltyLevel(self) -> float
        """Get novelty level."""
    
    def getPredictionError(self) -> float
        """Get prediction error."""
    
    def reset(self) -> None
        """Reset agent for new episode."">
    
    def getBrain(self) -> Brain
        """Get the underlying brain."""
    
    # Subsystem control
    def enableRewardModulation(self, enable: bool) -> None
    def enableStructuralPlasticity(self, enable: bool) -> None
    def enableDevelopment(self, enable: bool) -> None
    def enableCuriosity(self, enable: bool) -> None
    
    def isRewardModulationEnabled(self) -> bool
    def isStructuralPlasticityEnabled(self) -> bool
    def isDevelopmentEnabled(self) -> bool
    def isCuriosityEnabled(self) -> bool
```

### SimpleWorld Class

```python
class SimpleWorld:
    def __init__(self)
        """Create a new simple world."""
    
    def configure(self, width: int, height: int, 
                  vision_width: int, vision_height: int) -> None
        """Configure world dimensions."""
    
    def reset(self) -> None
        """Reset world to initial state."""
    
    def setAgentStart(self, x: float, y: float) -> None
        """Set agent starting position."""
    
    def update(self, timestep: float) -> None
        """Update world by timestep seconds."""
    
    def applyMotorCommand(self, cmd: MotorCommand, current_time: float) -> ActionResult
        """Apply motor command to world."""
    
    def getSensoryPercept(self) -> SensoryPercept
        """Get sensory percept from world."""
    
    def getAgentBody(self) -> AgentBody
        """Get agent body state."""
    
    def addObject(self, obj: WorldObject) -> None
        """Add world object."""
    
    def removeObject(self, x: float, y: float) -> None
        """Remove object at position."""
    
    def isValidPosition(self, x: float, y: float) -> bool
        """Check if position is valid."""
    
    # Properties
    def getWidth(self) -> int
    def getHeight(self) -> int
    def getMaxEnergy(self) -> float
    def setMaxEnergy(self, e: float) -> None
    def getEnergyDecayRate(self) -> float
    def setEnergyDecayRate(self, r: float) -> None
    def getSimulationTime(self) -> float
    def setRandomSeed(self, seed: int) -> None
    def getRandomSeed(self) -> int
```