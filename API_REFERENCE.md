# NLM Python Bindings - API Reference

This document provides complete API documentation for the NLM Python bindings.

## Module Overview

```python
import pynlm as nlm
```

The `pynlm` module provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework, enabling users to create and simulate artificial brains with neural dynamics, learning, and interaction capabilities.

## Core Classes

### Brain

```python
class Brain:
    def __init__(self, config: Config):
        """Create a new brain with configuration."""
        
    def initialize(self) -> bool:
        """Initialize the brain with configuration. Returns success status."""
        
    def step(self, currentStep: int, currentTime: float) -> None:
        """Perform a simulation step. Can also be called with just step number."""
        
    def receiveSensoryInput(self, input: SensoryInput) -> None:
        """Inject sensory input into the brain."""
        
    def injectCurrent(self, neuron: NeuronId, current: float) -> None:
        """Inject current into a specific neuron."""
        
    def injectCurrentToNeurons(self, type: NeuronType, current: float) -> None:
        """Inject current into all neurons of a specific type."""
        
    def produceAction(self) -> Action:
        """Produce motor action based on neural activity."""
        
    def reset(self) -> None:
        """Reset brain state to initial conditions."""
        
    def save(self, filepath: str) -> bool:
        """Save brain state to file. Returns success status."""
        
    def load(self, filepath: str) -> bool:
        """Load brain state from file. Returns success status."""
        
    def addRegion(self, name: str = "") -> RegionId:
        """Add a new neural region. Returns region ID."""
        
    def getRegion(self, id: RegionId) -> NeuralRegion:
        """Get a region by ID. Returns region reference."""
        
    def getRegionCount(self) -> int:
        """Get the number of regions."""
        
    def getRegionIds(self) -> List[RegionId]:
        """Get all region IDs."""
        
    def getRegions(self) -> List[NeuralRegion]:
        """Get all regions."""
        
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions."""
        
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions."""
        
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons."""
        
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons."""
        
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons."""
        
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        
    def getTotalSpikeCount(self) -> int:
        """Get total spike count."""
        
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        
    def setDevelopmentalStage(self, stage: DevelopmentalStage) -> None:
        """Set developmental stage."""
        
    def getConfig(self) -> Config:
        """Get the configuration."""
        
    def logStatus(self) -> None:
        """Log brain status and statistics."""
        
    def getWorkingMemory(self) -> Optional[WorkingMemory]:
        """Get working memory system."""
        
    def getEpisodicMemory(self) -> Optional[EpisodicMemory]:
        """Get episodic memory system."""
        
    def getDopamine(self) -> Optional[Neuromodulator]:
        """Get dopamine neuromodulator."""
        
    def getCuriosity(self) -> Optional[Neuromodulator]:
        """Get curiosity neuromodulator."""
        
    def getPredictionSystem(self) -> Optional[PredictionSystem]:
        """Get prediction system."""
        
    def getDevelopmentSystem(self) -> Optional[DevelopmentSystem]:
        """Get development system."""
        
    def getPlanner(self) -> Optional[NeuralPlanner]:
        """Get neural planner."""
        
    def getConceptFormation(self) -> Optional[ConceptFormation]:
        """Get concept formation system."""
        
    def getAttention(self) -> Optional[AttentionSystem]:
        """Get attention system."""
```

### Config

```python
class Config:
    def __init__(self):
        """Create a default configuration."""
        
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from a JSON file. Returns success status."""
        
    def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
        """Load configuration from command line arguments. Returns success status."""
        
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to a JSON file. Returns success status."""
        
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
        
    def clear(self) -> None:
        """Clear all configuration entries."""
        
    def summary(self) -> str:
        """Get a summary string of the configuration."""
```

### AgentBrain

```python
class AgentBrain:
    def __init__(self, brain: Brain):
        """Create an agent brain interface with underlying brain."""
        
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize with world."""
        
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size."""
        
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size."""
        
    def processSensoryInput(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain."""
        
    def decodeMotorCommand(self) -> MotorCommand:
        """Decode brain motor activity into motor command."""
        
    def applyRewardModulation(self, reward: float, predictedReward: float) -> None:
        """Apply reward-based neuromodulation."""
        
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system."""
        
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        
    def getPredictionError(self) -> float:
        """Get prediction error."""
        
    def reset(self) -> None:
        """Reset agent for new episode."""
        
    def getBrain(self) -> Brain:
        """Get the underlying brain."""
        
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable/disable reward modulation."""
        
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity."""
        
    def enableDevelopment(self, enable: bool) -> None:
        """Enable/disable development."""
        
    def enableCuriosity(self, enable: bool) -> None:
        """Enable/disable curiosity."""
```

### SimpleWorld

```python
class SimpleWorld:
    def __init__(self):
        """Create a simple 2D world for NLM simulation."""
        
    def configure(self, width: int, height: int, 
                  visionWidth: int = 64, visionHeight: int = 64) -> None:
        """Configure world dimensions and agent vision."""
        
    def reset(self) -> None:
        """Reset world to initial state."""
        
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position."""
        
    def update(self, timestep: float) -> None:
        """Update world state with current timestep."""
        
    def applyMotorCommand(self, cmd: MotorCommand, currentTime: float) -> None:
        """Apply motor command to world."""
        
    def getSensoryPercept(self) -> SensoryPercept:
        """Get current sensory percept from world."""
        
    def getAgentBody(self) -> AgentBody:
        """Get current agent body state."""
        
    def addObject(self, obj: WorldObject) -> None:
        """Add an object to the world."""
        
    def removeObject(self, x: float, y: float) -> None:
        """Remove object at position."""
        
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        
    def getWidth(self) -> int:
        """Get world width."""
        
    def getHeight(self) -> int:
        """Get world height."""
        
    def getMaxEnergy(self) -> float:
        """Get maximum agent energy."""
        
    def setMaxEnergy(self, e: float) -> None:
        """Set maximum agent energy."""
        
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate."""
        
    def setEnergyDecayRate(self, r: float) -> None:
        """Set energy decay rate."""
        
    def getSimulationTime(self) -> float:
        """Get current simulation time."""
        
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed."""
        
    def getRandomSeed(self) -> int:
        """Get random seed."""
```

### SensoryInput Classes

```python
class SensoryInput:
    def getType(self) -> NeuronType:
        """Get the type of sensory input."""
        
    def getData(self) -> List[float]:
        """Get the raw data as a vector."""
        
    def getDimensions(self) -> int:
        """Get the dimensionality."""
        
    def getTimestamp(self) -> float:
        """Get the timestamp."""
        
    def setTimestamp(self, timestamp: float) -> None:
        """Set the timestamp."""

class Vision(SensoryInput):
    def __init__(self, width: int = 64, height: int = 64, channels: int = 3):
        """Create vision sensory input."""
        
    def setData(self, data: List[float]) -> None:
        """Set vision data."""
        
    def getWidth(self) -> int:
        """Get vision width."""
        
    def getHeight(self) -> int:
        """Get vision height."""
        
    def getChannels(self) -> int:
        """Get number of channels."""

class Audio(SensoryInput):
    def __init__(self, sampleRate: int = 44100, numSamples: int = 1024):
        """Create audio sensory input."""
        
    def setData(self, data: List[float]) -> None:
        """Set audio data."""
        
    def setSampleRate(self, sampleRate: int) -> None:
        """Set sample rate."""
        
    def getSampleRate(self) -> int:
        """Get sample rate."""
        
    def getNumSamples(self) -> int:
        """Get number of samples."""

class InternalSignals(SensoryInput):
    def addSignal(self, value: float) -> None:
        """Add a signal value."""
        
    def clearSignals(self) -> None:
        """Clear all signals."""
```

### Action Classes

```python
class Action:
    def __init__(self, type: ActionType = ActionType.Wait, 
                 parameters: List[float] = None):
        """Create action with type and optional parameters."""
        
    def getType(self) -> ActionType:
        """Get action type."""
        
    def setType(self, type: ActionType) -> None:
        """Set action type."""
        
    def getParameters(self) -> List[float]:
        """Get action parameters."""
        
    def setParameters(self, params: List[float]) -> None:
        """Set action parameters."""
        
    def getName(self) -> str:
        """Get action name."""
        
    def clone(self) -> Action:
        """Clone the action."""

class ActionResult:
    def __init__(self, reward: float = 0.0, success: bool = False,
                 message: str = ""):
        """Create action result with reward, success status, and message."""
```

### Other Important Classes

```python
# Enumerations
class NeuronType:
    Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal

class SynapseType:
    Excitatory, Inhibitory, Modulatory, Electrical, GapJunction

class DevelopmentalStage:
    Initial, CriticalPeriod, Maturation, Adult, Aging

class FiringState:
    Resting, Active, Refractory, Inhibited

class ActionType:
    MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight,
    Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom

class MotorCommand:
    MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight,
    Interact, Wait

class WorldObjectType:
    Empty, Resource, Hazard, Wall, Marker

# Data Types
class NeuronId, SynapseId, RegionId, PopulationId:
    Unique identifiers with value, index, and comparison methods

# Component Classes
class AgentBody, SensoryPercept, WorldObject
```

## Functions

### Factory Functions

```python
# Core creation functions
def createDefaultConfig() -> Config
    """Create a default configuration."""
    
def createBrain(config: Config) -> Brain
    """Create a brain with configuration."""
    
def createSimpleWorld() -> SimpleWorld
    """Create a simple world."""
    
def createAgentBrain(brain: Brain) -> AgentBrain
    """Create an agent brain interface."""

# Advanced factory functions
def createBrainFromConfig(configFile: str) -> Brain
    """Create a brain from configuration file."""
    
def createBrainWithArgs(argc: int, argv: List[str]) -> Brain
    """Create a brain from command line arguments."""
    
def createAgentWithWorld(brain: Brain, world: SimpleWorld) -> AgentBrain
    """Create an agent brain with a world."""
    
def createCustomAction(type: ActionType, params: List[float]) -> Action
    """Create a custom action with parameters."""
    
def createStandardAction(type: ActionType) -> Action
    """Create a standard action."""
    
def createSimpleVision(width: int = 64, height: int = 64, 
                       channels: int = 3) -> Vision
    """Create simple vision sensory input."""
    
def createSimpleAudio(sampleRate: int = 44100, 
                      numSamples: int = 1024) -> Audio
    """Create simple audio sensory input."""
    
def createBrainTemplate(templateName: str) -> Brain
    """Create a brain with predefined configuration template."""
```

### Utility Functions

```python
def validateBrainConfig(config: Config) -> List[str]
    """Validate configuration and return list of errors."""
    
def simulateEpisode(brain: Brain, world: SimpleWorld, 
                    maxSteps: int = 100, timestep: float = 0.001) -> Tuple[float, float]
    """Run a complete simulation episode and return (total_reward, novelty)."""
    
def extractNeuralFeatures(brain: Brain) -> Dict[str, float]
    """Extract neural features from a brain for analysis."""
```

### Constants

```python
# Error constants
ERROR_INVALID_CONFIG = "INVALID_CONFIG"
ERROR_NEURAL_INVALID = "NEURAL_INVALID"
ERROR_WORLD_INVALID = "WORLD_INVALID"

# Version constants
NLM_VERSION_MAJOR = 0
NLM_VERSION_MINOR = 1
NLM_VERSION_PATCH = 0

PYNLM_VERSION = "1.0.0"

# Invalid IDs
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()
```

## Configuration Options

### Default Configuration Keys

- `random_seed`: Random seed for reproducible experiments (default: 42)
- `simulation_timestep`: Simulation timestep in seconds (default: 0.001)
- `neuron_count`: Number of neurons in the brain (default: 1000)
- `region_count`: Number of brain regions (default: 2)
- `connection_probability`: Connection probability for random connectivity (default: 0.1)
- `plasticity_learning_rate`: Plasticity learning rate (default: 0.01)
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.012)
- `stdp_time_constant`: STDP time constant (default: 20.0)
- `development_synaptogenesis_rate`: Synaptogenesis rate (default: 0.001)
- `development_pruning_rate`: Pruning rate (default: 0.0001)
- `dopamine_baseline`: Dopamine baseline level (default: 0.1)
- `dopamine_decay_rate`: Dopamine decay rate (default: 0.9)
- `dopamine_sensitivity`: Dopamine sensitivity (default: 1.0)

### Templates

The `createBrainTemplate()` function supports these templates:

- `"small"`: 100 neurons, 2 regions, connection probability 0.1
- `"medium"`: 1000 neurons, 4 regions, connection probability 0.15
- `"large"`: 5000 neurons, 8 regions, connection probability 0.2

## Error Handling

### Common Exceptions

- `RuntimeError`: For invalid operations or missing configurations
- `ValueError`: For invalid parameter values

### Error Checking Functions

```python
# Validate configuration
errors = nlm.validateBrainConfig(config)
if errors:
    print("Configuration issues:")
    for error in errors:
        print(f"  - {error}")
```

## Performance Tips

### For Large Brains

1. Use template brains for consistent performance:
   ```python
   brain = nlm.createBrainTemplate("medium")
   ```

2. Run simulations in episodes rather than continuous runs
3. Use appropriate timestep for your simulation needs
4. Monitor memory usage with performance tools

### For Complex Environments

1. Configure world dimensions appropriately for your simulation
2. Use smaller vision fields for better performance
3. Enable only necessary neuromodulation systems
4. Implement efficient sensory processing pipelines

## Migration Guide

### From Version 0.1.0 to 1.0.0

1. **New Factory Functions**: Use `createBrainFromConfig()` and `createBrainTemplate()` instead of manual config creation
2. **Enhanced Actions**: Use `createStandardAction()` and `createCustomAction()` for better action management
3. **Advanced Utilities**: Use `validateBrainConfig()` and `simulateEpisode()` for common operations
4. **Error Handling**: Check return values and use proper exception handling

### Upgrading from Old Code

```python
# Old way
config = Config()
config.set("neuron_count", 1000)
brain = Brain(config)

# New way (recommended)
brain = nlm.createBrainTemplate("medium")

# Or with custom config
brain = nlm.createBrainFromConfig("my_config.cfg")
```

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Integration Demo](https://github.com/nlm-project/nlm/tree/main/src/experiments)
- [Examples](EXAMPLES.md)

This API reference covers all public classes, functions, and constants available in the pynlm module. For more detailed information about specific classes or methods, refer to their individual documentation.