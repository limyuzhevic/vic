# NLM Python API Reference Guide

## Overview

This guide provides comprehensive documentation for the NLM Python API (`pynlm`). The API provides Python bindings for the NLM (Neural Learning Machine) neural simulation framework, enabling users to create, configure, and simulate artificial brains through a high-level Python interface.

**Python Package**: `pynlm`
**Version**: 0.1.0
**Author**: NLM Project
**License**: MIT

---

## API Structure

The NLM Python API is organized into several main classes and functions:

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Config` | Configuration management | `set()`, `get()`, `loadFromFile()`, `saveToFile()` |
| `Brain` | Neural simulation engine | `initialize()`, `step()`, `injectCurrent()` |
| `AgentBrain` | Brain-world interface | `initialize()`, `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | Simulation environment | `configure()`, `update()`, `applyMotorCommand()` |
| `Vision` | Visual sensory input | `setData()`, `getWidth()`, `getHeight()` |
| `Audio` | Auditory sensory input | `setData()`, `getSampleRate()`, `getNumSamples()` |
| `Action` | Motor output | `getType()`, `setType()`, `getParameters()` |

### Type Enums

| Enum | Values | Description |
|------|--------|-------------|
| `NeuronType` | `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal` | Neuron classification |
| `ActionType` | `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `Look`, etc. | Action types |
| `DevelopmentalStage` | `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging` | Development stages |
| `WorldObjectType` | `Empty`, `Resource`, `Hazard`, `Wall`, `Marker` | World object types |

### Main Functions

| Function | Returns | Description |
|----------|---------|-------------|
| `createDefaultConfig()` | `Config` | Create default configuration |
| `createBrain(config)` | `Brain` | Create brain with configuration |
| `createAgentBrain(brain)` | `AgentBrain` | Create agent brain interface |
| `createSimpleWorld()` | `SimpleWorld` | Create simple world environment |

---

## Configuration Class

### Constructor

```python
def createDefaultConfig() -> Config
```

Creates a default configuration object with reasonable default values for neural simulation.

**Returns**: `Config` object

### Key Methods

#### Setting Configuration Values

```python
def set(key: str, value: Any, source: ConfigSource = ConfigSource.Runtime) -> None
```

Set a configuration value by key with optional source.

**Parameters**:
- `key`: Configuration key (e.g., `"brain.neuron_count"`)
- `value`: Configuration value (int, float, str, bool, etc.)
- `source`: Configuration source (optional, defaults to Runtime)

```python
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.enable", True)
```

#### Getting Configuration Values

```python
def get(key: str) -> Optional[Any]
```

Get a configuration value by key.

**Parameters**:
- `key`: Configuration key to retrieve

**Returns**: Configuration value or None if not found

```python
neuron_count = config.get("brain.neuron_count")  # Returns: 2000 or None
learning_rate = config.get("plasticity.stdp.learning_rate")  # Returns: 0.01
```

#### Typed Getters

```python
def setInt(key: str, value: int) -> None
def setFloat(key: str, value: float) -> None
def setString(key: str, value: str) -> None
def setBool(key: str, value: bool) -> None

def getInt(key: str) -> int
def getFloat(key: str) -> float
def getString(key: str) -> str
def getBool(key: str) -> bool
```

**Typed getter example**:

```python
config.setInt("brain.neuron_count", 2000)
config.setFloat("plasticity.stdp.learning_rate", 0.01)
config.setBool("neuromod.curiosity.enable", True)

print(f"Neurons: {config.getInt('brain.neuron_count')}")
print(f"Learning rate: {config.getFloat('plasticity.stdp.learning_rate')}")
print(f"Curiosity enabled: {config.getBool('neuromod.curiosity.enable')}")
```

#### File I/O Operations

```python
def loadFromFile(filepath: str) -> bool
def saveToFile(filepath: str) -> bool
def loadFromArgs(argc: int, argv: List[str]) -> bool
def saveToFile(filepath: str) -> bool
```

**File I/O examples**:

```python
# Load from JSON file
config.loadFromFile("my_config.json")  # Returns: True/False

# Save to JSON file
config.saveToFile("my_config.json")  # Returns: True/False

# Load from command line arguments
config.loadFromArgs(len(sys.argv), sys.argv)
```

#### Configuration Queries

```python
def has(key: str) -> bool
def getKeys() -> List[str]
def clear() -> None
def summary() -> str
```

**Query examples**:

```python
# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Get all configuration keys
keys = config.getKeys()
print(f"All keys: {keys}")

# Get configuration summary
print(config.summary())
```

### Exception Handling

```python
# Specific exception types
raise ValidationError("Invalid configuration value")
raise IndexError("Configuration key not found")
raise RuntimeError("Runtime error in configuration")

# Catching exceptions
try:
    config.setInt("brain.neuron_count", 2000)
except ValidationError as e:
    print(f"Invalid configuration: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

---

## Brain Class

### Constructor

```python
def createBrain(config: Config) -> Brain
```

Creates a new brain instance with the given configuration.

**Parameters**:
- `config`: Configuration object for the brain

**Returns**: `Brain` object

### Core Simulation Methods

```python
def initialize() -> bool
def step(currentStep: SimulationStep) -> None
def step(currentStep: SimulationStep, currentTime: Timestamp) -> None
```

**Simulation methods**:

```python
# Initialize brain
brain = createBrain(config)
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")

# Run simulation
for step in range(100):
    brain.step(step)

# Run simulation with time stamps
for step in range(100):
    brain.step(step, step * 0.001)
```

### Input/Output Methods

```python
def receiveSensoryInput(input: SensoryInput) -> None
def injectCurrent(neuron: NeuronId, current: float) -> None
def injectCurrentToNeurons(type: NeuronType, current: float) -> None
def produceAction() -> Action
def reset() -> None
```

**Input/output examples**:

```python
# Create sensory input
vision = Vision(8, 8, 3)
vision.setData([1.0, 0.5, 0.0] * 12)  # 8x8x3 RGB data

# Inject sensory input
brain.receiveSensoryInput(vision)

# Inject current into specific neuron
brain.injectCurrent(NeuronId(0), 5.0)

# Inject current into all neurons of a type
brain.injectCurrentToNeurons(NeuronType.Excitatory, 10.0)

# Get action from brain
action = brain.produceAction()
print(f"Action type: {action.getType()}")

# Reset brain
brain.reset()
```

### State Management

```python
def save(filepath: str) -> None
def load(filepath: str) -> None
```

**State management**:

```python
# Save brain state
brain.save("brain_state.bin")

# Load brain state
brain.load("brain_state.bin")
```

### Region Management

```python
def addRegion(name: str = "") -> RegionId
def getRegion(id: RegionId) -> NeuralRegion
def getRegionCount() -> int
def getRegionIds() -> List[RegionId]
```

**Region management**:

```python
# Add a new region
region_id = brain.addRegion("cortex")

# Get region
region = brain.getRegion(region_id)

# Get statistics
print(f"Region count: {brain.getRegionCount()}")
print(f"Region IDs: {brain.getRegionIds()}")
```

### Statistics and Monitoring

```python
def getTotalNeuronCount() -> int
def getTotalSynapseCount() -> int
def getActiveNeuronCount() -> int
def getFiringNeuronCount() -> int
def getAverageFiringRate() -> float
def getExcitationInhibitionRatio() -> float
def getTotalSpikeCount() -> int
def getDevelopmentalStage() -> DevelopmentalStage
```

**Statistics examples**:

```python
# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Enhanced Brain Methods

```python
def to_dict() -> str  # JSON string
def from_dict(json_str: str) -> None  # from JSON string
def validateConfiguration() -> str  # validation status
def debugState() -> str  # debug info
def getPerformanceStats() -> str  # performance statistics
```

**Enhanced methods**:

```python
# Export brain state as JSON
brain_state = brain.to_dict()
print(f"Brain state (JSON): {brain_state}")

# Load brain state from JSON
brain.from_dict(brain_state)

# Validate configuration
try:
    validation_result = brain.validateConfiguration()
    print(f"Validation: {validation_result}")
except ValidationError as e:
    print(f"Validation failed: {e}")

# Get debug information
debug_info = brain.debugState()
print(f"Debug info: {debug_info}")

# Get performance statistics
perf_stats = brain.getPerformanceStats()
print(f"Performance: {perf_stats}")
```

---

## AgentBrain Class

### Constructor

```python
def createAgentBrain(brain: Brain) -> AgentBrain
```

Creates an agent brain interface for brain-world interaction.

**Parameters**:
- `brain`: Brain instance to interface with

**Returns**: `AgentBrain` object

### Core Methods

```python
def initialize(world: SimpleWorld) -> None
def getSensoryInputSize() -> int
def getMotorOutputSize() -> int
def processSensoryInput(percept: SensoryPercept) -> None
def decodeMotorCommand() -> Action
def applyRewardModulation(reward: float, predictedReward: float) -> None
def updateDevelopment(timestep: float) -> None
def getDevelopmentalStage() -> DevelopmentalStage
```

**Agent methods**:

```python
# Initialize agent with world
agent = createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Update development
agent.updateDevelopment(0.1)
```

### Subsystem Control

```python
def enableRewardModulation(enable: bool) -> None
def enableStructuralPlasticity(enable: bool) -> None
def enableDevelopment(enable: bool) -> None
def enableCuriosity(enable: bool) -> None
```

**Subsystem control**:

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
```

### Advanced Methods

```python
def enableNoveltyDetection(enable: bool) -> None
def setLearningRate(learning_rate: float) -> None
def getLearningRate() -> float
def train(experience: List[float], reward: List[float], learning_rate: float) -> None
def simulateEpisode(world: SimpleWorld, num_steps: int, timestep: float) -> None
def getCuriosityLevel() -> float
def getNoveltyLevel() -> float
def getPredictionError() -> float
def isNoveltyDetectionEnabled() -> bool
```

**Advanced methods**:

```python
# Enhanced subsystem control
agent.enableNoveltyDetection(True)

# Learning rate control
agent.setLearningRate(0.01)
print(f"Current learning rate: {agent.getLearningRate()}")

# Training
agent.train(experience=[0.1, 0.2, 0.3], reward=[1.0, 0.5, 0.0], learning_rate=0.01)

# Episode simulation
agent.simulateEpisode(world, num_steps=100, timestep=0.1)

# Neuromodulation levels
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")

# Novelty detection status
print(f"Novelty detection enabled: {agent.isNoveltyDetectionEnabled()}")
```

---

## SimpleWorld Class

### Constructor

```python
def createSimpleWorld() -> SimpleWorld
```

Creates a new simple world instance.

**Returns**: `SimpleWorld` object

### Core World Methods

```python
def configure(width: int, height: int, visionWidth: int, visionHeight: int) -> None
def reset() -> None
def setAgentStart(x: float, y: float) -> None
def update(timestep: float) -> None
def applyMotorCommand(cmd: Action, currentTime: float) -> None
def getSensoryPercept() -> SensoryPercept
def getAgentBody() -> AgentBody
```

**World methods**:

```python
# Configure world
world = createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Update world
world.update(0.1)

# Apply motor command
action = Action(ActionType.MoveForward)
world.applyMotorCommand(action, world.getSimulationTime())

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
```

### Object Management

```python
def addObject(obj: WorldObject) -> None
def removeObject(x: float, y: float) -> None
def isValidPosition(x: float, y: float) -> bool
```

**Object management**:

```python
# Add world object
resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Remove object
world.removeObject(5.0, 5.0)

# Check valid position
if world.isValidPosition(10.0, 10.0):
    print("Position is valid")
```

### Enhanced World Methods

```python
def configureDetailed(width: int, height: int, visionWidth: int, visionHeight: int, 
                      maxEnergy: float, energyDecayRate: float, randomSeed: int) -> None
def spawnRandomResource(count: int, value: float = 1.0) -> None
def simulateEpisode(agent: AgentBrain, num_steps: int, timestep: float) -> None
def getStats() -> str
def clone() -> SimpleWorld
```

**Enhanced methods**:

```python
# Detailed configuration
world.configureDetailed(width=20, height=20, visionWidth=8, visionHeight=8,
                       maxEnergy=100.0, energyDecayRate=0.01, randomSeed=42)

# Spawn random resources
world.spawnRandomResource(count=10, value=1.0)

# Run complete episode
agent = createAgentBrain(brain)
agent.initialize(world)
world.simulateEpisode(agent, num_steps=100, timestep=0.1)

# Get world statistics
stats = world.getStats()
print(f"World stats: {stats}")

# Clone world
world_clone = world.clone()
```

### World Properties

```python
def getWidth() -> int
def getHeight() -> int
def getMaxEnergy() -> float
def setMaxEnergy(e: float) -> None
def getEnergyDecayRate() -> float
def setEnergyDecayRate(r: float) -> None
def getSimulationTime() -> float
def setRandomSeed(seed: int) -> None
def getRandomSeed() -> int
```

**World properties**:

```python
print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Energy decay rate: {world.getEnergyDecayRate()}")
print(f"Current simulation time: {world.getSimulationTime()}")
print(f"Random seed: {world.getRandomSeed()}")
```

---

## SensoryInput Classes

### Base SensoryInput Class

```python
def getType() -> SensoryInputType
def getData() -> List[float]
def getDimensions() -> int
def getTimestamp() -> float
def setTimestamp(timestamp: float) -> None
```

### Vision Class

```python
Vision(width: int, height: int, channels: int = 3)
Vision(width: int, height: int)  # 2D grayscale
Vision()
```

**Vision methods**:

```python
# Create vision sensor
vision = Vision(8, 8, 3)  # 8x8 RGB vision
vision.setData([1.0, 0.5, 0.0] * 12)  # Set pixel data

print(f"Vision dimensions: {vision.getDimensions()}")
print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")
```

### Audio Class

```python
audio = Audio(sampleRate: int, numSamples: int)
audio.setData(audio_data: List[float])
```

**Audio methods**:

```python
audio = Audio(sampleRate=44100, numSamples=1024)
# Set audio data
audio.setData(list_of_floats)

print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
```

### InternalSignals Class

```python
def addSignal(value: float) -> None
def clearSignals() -> None
```

**Internal signals**:

```python
internal = InternalSignals()
internal.addSignal(0.5)
internal.addSignal(1.0)
internal.addSignal(-0.3)

print(f"Internal signals: {internal.getData()}")
```

---

## Action and WorldObject Classes

### Action Class

```python
Action(type: ActionType)
Action(type: ActionType, parameters: List[float])
Action()
```

**Action methods**:

```python
# Create actions
action1 = Action(ActionType.MoveForward)
action2 = Action(ActionType.Custom, [1.0, 2.0, 3.0])

print(f"Action type: {action1.getType()}")
print(f"Action parameters: {action1.getParameters()}")

# Set action properties
action.setType(ActionType.TurnLeft)
action.setParameters([0.0, 45.0])
print(f"Action name: {action.getName()}")

# Clone action
cloned_action = action.clone()
```

### WorldObject Class

```python
WorldObject(x: float, y: float, type: WorldObjectType, value: float = 0.0, radius: float = 0.5)
WorldObject()
```

**WorldObject methods**:

```python
# Create world objects
resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0, radius=0.5)
hazard = WorldObject(10.0, 10.0, WorldObjectType.Hazard, value=5.0)
wall = WorldObject(15.0, 15.0, WorldObjectType.Wall, value=1.0)

# Access properties
print(f"Object position: ({resource.x}, {resource.y})")
print(f"Object type: {resource.type}")
print(f"Object value: {resource.value}")
print(f"Object radius: {resource.radius}")
print(f"Object active: {resource.active}")
```

---

## Type Classes

### NeuronId Class

```python
NeuronId()
NeuronId(value: int)
```

**NeuronId methods**:

```python
id1 = NeuronId()
id2 = NeuronId(42)

print(f"ID value: {id1.value}")
print(f"ID index: {id1.index}")
print(f"ID equality: {id1 == id2}")
print(f"ID hash: {hash(id1)}")
print(f"ID representation: {repr(id1)}")
```

### SynapseId Class

```python
SynapseId()
SynapseId(value: int)
```

**Similar to NeuronId**

### RegionId Class

```python
RegionId()
RegionId(value: int)
```

**Similar to NeuronId**

### PopulationId Class

```python
PopulationId()
PopulationId(value: int)
```

**Similar to NeuronId**

---

## Main Functions

### Core Creation Functions

```python
def createDefaultConfig() -> Config
def createBrain(config: Config) -> Brain
def createAgentBrain(brain: Brain) -> AgentBrain
def createSimpleWorld() -> SimpleWorld
```

**Main function examples**:

```python
# Create all components
config = createDefaultConfig()
brain = createBrain(config)
agent = createAgentBrain(brain)
world = createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)
```

### Type Enum Access

```python
# Access type enums
print(f"Neuron type: {NeuronType.Excitatory}")
print(f"Action type: {ActionType.MoveForward}")
print(f"Development stage: {DevelopmentalStage.Adult}")
print(f"World object type: {WorldObjectType.Resource}")
```

### Invalid Constants

```python
# Invalid constants
print(f"Invalid neuron ID: {INVALID_NEURON_ID}")
print(f"Invalid synapse ID: {INVALID_SYNAPSE_ID}")
print(f"Invalid region ID: {INVALID_REGION_ID}")
print(f"Invalid population ID: {INVALID_POPULATION_ID}")
```

---

## Error Handling

### Exception Types

| Exception Type | Description | When to Use |
|---------------|-------------|-------------|
| `ValidationError` | Invalid configuration or parameters | Configuration validation failures |
| `IndexError` | Out-of-bounds access | Invalid array indices or keys |
| `RuntimeError` | General runtime errors | Any runtime error |

### Exception Handling Examples

```python
try:
    config.setInt("brain.neuron_count", -100)
except ValidationError as e:
    print(f"Invalid configuration: {e}")
    # Retry with valid value
    config.setInt("brain.neuron_count", 1000)
except Exception as e:
    print(f"Unexpected error: {e}")

# Check if operation succeeded
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.getInt('brain.neuron_count')}")
```

---

## Type Hints and Autocompletion

The pynlm module supports Python type hints for better IDE autocompletion:

```python
import pynlm
from typing import Optional, List

# Type hints help with autocompletion
config: pynlm.Config = pynlm.createDefaultConfig()
brain: pynlm.Brain = pynlm.createBrain(config)
world: pynlm.SimpleWorld = pynlm.createSimpleWorld()

# Method type hints
vision_data: List[float] = [1.0, 0.5, 0.0] * 12
vision: pynlm.Vision = pynlm.Vision(8, 8, 3)
```

---

## Performance Tips

### Memory Efficiency

1. **Reuse objects**: Reuse brain and world instances instead of creating new ones
2. **Batch operations**: Use batch operations where available
3. **Memory monitoring**: Monitor memory usage in long simulations
4. **Checkpoints**: Use periodic checkpointing for long runs

### Simulation Performance

1. **Optimize timestep**: Use appropriate timesteps for your simulation
2. **Parallel processing**: Enable parallel processing where available
3. **SIMD optimizations**: Enable SIMD optimizations in configuration
4. **Configuration tuning**: Adjust configuration parameters for your use case

### Example Performance-Optimized Simulation

```python
import pynlm
import time

# Create brain with optimized configuration
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 1000)
config.setFloat("performance.optimization.enable_simd", True)
config.setBool("performance.optimization.enable_parallel", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run optimized simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

---

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python, keep these considerations in mind:

1. **Memory management**: Python handles memory automatically, but be aware of reference counting
2. **Error handling**: Python uses exceptions instead of error codes
3. **Type safety**: Python type hints provide better IDE support
4. **Performance**: Python API has some overhead but is suitable for most use cases

### Python Best Practices

1. **Use context managers**: Where available, use context managers for resource management
2. **Batch operations**: Use batch operations for efficiency
3. **Configuration validation**: Validate configuration before use
4. **Exception handling**: Always handle exceptions appropriately

---

## Advanced Usage

### Creating Custom Configurations

```python
def create_research_config():
    config = pynlm.createDefaultConfig()
    
    # Customize for research
    config.setInt("brain.neuron_count", 5000)
    config.setFloat("plasticity.stdp.learning_rate", 0.05)
    config.setFloat("neuromod.curiosity.scale", 2.0)
    config.setBool("development.enable", True)
    
    return config

research_config = create_research_config()
```

### Creating Simulation Macros

```python
def run_standard_experiment(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                            agent: pynlm.AgentBrain, steps: int = 1000):
    """Run a standard experiment with common settings"""
    world.reset()
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0  # Could be computed from percept
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel()
    }

# Use the macro
result = run_standard_experiment(brain, world, agent, steps=500)
print(f"Experiment results: {result}")
```

### Configuration Sweeps

```python
def parameter_sweep(base_config: pynlm.Config, parameter_ranges: dict):
    """Run parameter sweeps for experiments"""
    results = []
    
    # For each parameter, create variations
    for param, values in parameter_ranges.items():
        for value in values:
            # Create new config for each parameter value
            config = base_config.clone() if hasattr(base_config, 'clone') else pynlm.createDefaultConfig()
            
            # Set parameter
            if param in ["brain.neuron_count", "plasticity.stdp.learning_rate"]:
                config.setFloat(param, float(value))
            elif param in ["neuromod.curiosity.enable", "development.enable"]:
                config.setBool(param, bool(value))
            
            # Run experiment with this config
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            result = {
                'parameters': {param: value},
                'stats': {
                    'neuron_count': brain.getTotalNeuronCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                    'total_spikes': brain.getTotalSpikeCount()
                }
            }
            results.append(result)
    
    return results
```

---

## Testing and Validation

### Unit Testing Examples

```python
import unittest
import pynlm

class TestNLMAPI(unittest.TestCase):
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_brain_creation(self):
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_simulation_step(self):
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        # Spike count should not decrease
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
    
    def test_agent_simulation(self):
        for step in range(10):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        self.assertIsNotNone(self.brain.produceAction())
    
    def test_configuration(self):
        self.config.setInt("brain.neuron_count", 100)
        self.assertEqual(self.config.getInt("brain.neuron_count"), 100)
    
    def test_error_handling(self):
        with self.assertRaises((pynlm.ValidationError, pynlm.IndexError)):
            # This should raise an exception
            self.config.setInt("invalid.key", -1)

if __name__ == '__main__':
    unittest.main()
```

---

## Performance Monitoring

### Monitoring Brain Performance

```python
def monitor_brain_performance(brain: pynlm.Brain, duration: int = 1000):
    """Monitor brain performance over time"""
    metrics = {
        'steps': [],
        'firing_rates': [],
        'spike_counts': [],
        'e_i_ratios': [],
        'times': []
    }
    
    start_time = time.time()
    for step in range(duration):
        step_start = time.time()
        brain.step(step)
        
        metrics['steps'].append(step)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['spike_counts'].append(brain.getTotalSpikeCount())
        metrics['e_i_ratios'].append(brain.getExcitationInhibitionRatio())
        metrics['times'].append(time.time() - start_time)
        
        # Log every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: Firing rate={brain.getAverageFiringRate():.3f}")
    
    return metrics

# Monitor performance
performance_data = monitor_brain_performance(brain, duration=500)
```

---

## References

1. **NLM Project Documentation**: https://nlm.readthedocs.io
2. **NLM GitHub Repository**: https://github.com/nlm-project/nlm
3. **C++ API Documentation**: See `src/core/` and `src/brain/` headers
4. **Example Code**: See `examples/` directory
5. **Configuration Reference**: `docs/CONFIGURATION.md`
6. **Architecture Documentation**: `docs/ARCHITECTURE.md`

---

## Release Notes

### Version 0.1.0 (Current)
- Initial Python API release
- Basic brain simulation functionality
- Agent-world interaction support
- Configuration management
- Error handling and validation

### Future Versions
- Enhanced performance optimizations
- Advanced learning algorithms
- More sophisticated world models
- Better visualization tools
- Distributed computing support

---

This comprehensive API reference guide provides everything you need to understand, use, and extend the NLM Python API. Whether you're a beginner learning neural simulation or an advanced researcher developing complex experiments, this guide will help you get the most out of the NLM framework.