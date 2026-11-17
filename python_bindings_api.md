# NLM Python API Reference

Complete documentation for the `pynlm` Python bindings for the NLM (Neural Learning Machine) neural simulation framework.

---

## Module Overview

The `pynlm` module provides Python bindings for the NLM C++ neural simulation framework. It enables you to:

- Create and simulate artificial brains with biologically-inspired neural dynamics
- Connect brains to environments through agents
- Enable learning, memory, and development in artificial systems
- Run comprehensive neural simulations with multiple integrated systems

The framework implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms, and full integration of cognitive, memory, and neuromodulation systems.

---

## Core Classes

### `pynlm.createBrain(config)`

Creates a new neural brain instance with the specified configuration.

**Parameters:**
- `config`: `pynlm.Config` instance containing brain configuration parameters

**Returns:**
- `Brain`: A configured brain instance ready for initialization

**Example:**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
```

---

### `pynlm.createSimpleWorld()`

Creates a simple 2D world environment for NLM simulations.

**Parameters:**
None

**Returns:**
- `SimpleWorld`: A configured world instance

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

---

### `pynlm.createAgentBrain(brain)`

Creates an agent brain that connects a neural brain to a world environment.

**Parameters:**
- `brain`: `Brain` instance to control

**Returns:**
- `AgentBrain`: Agent interface for sensory-motor processing

**Example:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
```

---

### `pynlm.createDefaultConfig()`

Creates a default configuration with sensible parameters for NLM simulation.

**Parameters:**
None

**Returns:**
- `Config`: Default configuration instance

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
```

---

## Configuration System

### `Config` Class

Manages configuration parameters for NLM systems using flexible key-value storage.

#### Constructor
```python
config = Config()  # Empty configuration
config = createDefaultConfig()  # With default values
```

#### Key Methods

- `set(key, value, source=ConfigSource.Runtime)`
  Set a configuration parameter with optional source specification

- `get(key)` → `Optional[T]`
  Get configuration value by key (returns None if not found)

- `getOr(key, default_value)` → `T`
  Get value with fallback to default if not present

- `has(key)` → `bool`
  Check if configuration key exists

- `loadFromFile(filepath)` → `bool`
  Load configuration from file (simple key=value format)

- `saveToFile(filepath)` → `bool`
  Save configuration to file

- `loadFromArgs(argv)` → `bool`
  Load from command-line arguments (--key=value format)

- `getKeys()` → `List[str]`
  Get all configuration keys

- `clear()`
  Remove all configuration entries

- `summary()` → `str`
  Get formatted configuration summary

#### Common Configuration Keys

| Key | Type | Default | Description |
|----- stranded | stranded | stranded | stranded | Type | Default | Description |

| **Core Neural Parameters** |
| neuron_count | int64 | 500 | Number of neurons in the brain |
| region_count | int64 | Primed | stranded | | Primed |
| connection_probability | float | 0.10 | Probability of synaptic connections |
| simulation_timestep | double | 0.001 | Time resolution in seconds |
| random_seed | int64 | 42 | Seed for random number generation |

| **Learning Parameters** |
| stdp_ltp_weight | float | 0.01 | STDP long-term potentiation |
| stdp_ltd_weight | float | 0.012 | STDP long-term depression |
| stdp_tau | float | 20.0 | STDP time constant (ms) |
| plasticity_learning_rate | float | 0.01 | Hebbian learning rate |

| **Structural Plasticity** |
| synaptogenesis_rate | float | 0.0001 | Rate of new connection formation |
| pruning_rate | float | 0.00001 | Rate of connection removal |

| **Memory Parameters** |
| working_memory_capacity | size_t | 100 | Working memory capacity |
| episodic_memory_capacity | size_t | 50 | Episodic memory capacity |

| **Development Parameters** |
| development_synaptogenesis_rate | float | 0.001 | Developmental growth |
| development_pruning_rate | float | 0.0001 | Developmental pruning |

| **Neuromodulation** |
| dopamine_baseline | float | 0.1 | Baseline dopamine level |
| curiosity_strength | float | 0.8 | Curiosity drive |
| novelty_threshold | float | 0.3 | Novelty detection sensitivity |

| **Environment Parameters** |
| environment_width | float | 10.0 | World width |
| environment_height | float | 10.0 | World height |
| vision_width | size_t | 8 | Visual field width |
| vision_height | size_t | 8 | Visual field height |

| **Logging** |
| log_level | str | "INFO" | Logging level (DEBUG, INFO, WARNING, ERROR) |
| log_to_file | bool | False | Enable file logging |
| log_filename | str | "nlm.log" | Log filename |

---

## Identifier Types

The NLM framework uses strongly-typed identifiers for safety and debugging:

### `NeuronId`
Represents a unique neuron identifier.

**Attributes:**
- `value` (int64): Underlying integer value

**Methods:**
- `index()` → `int64`: Get numeric index

**Example:**
```python
import pynlm

neuron_id = NeuronId(123)
print(f"Neuron index: {neuron_id.index()}")
```

### `SynapseId`
Represents a unique synapse identifier.

**Attributes:**
- `value` (int64): Underlying integer value

**Methods:**
- `index()` → `int64`: Get numeric index

### `RegionId`
Represents a brain region identifier.

**Attributes:**
- `value` (int64): Underlying integer value

**Methods:**
- `index()` → `int64`: Get numeric index

### `PopulationId`
Represents a neuron population identifier.

**Attributes:**
- `value` (int64): Underlying integer value

**Methods:**
- `index()` → `int64`: Get numeric index

---

## Enum Types

### `NeuronType`
Enumeration of neuron types.

**Values:**
- `Excitatory`: Standard excitatory neuron
- `Inhibitory`: Inhibitory neuron that suppresses activity
- `Modulatory`: Modulatory neuron that influences other neurons broadly
- `Sensory`: Sensory input neuron
- `Motor`: Motor output neuron
- `Internal`: Internal cognitive neuron

**Example:**
```python
import pynlm

if neuron_type == NeuronType.Sensory:
    print("This neuron processes sensory input")
elif neuron_type == NeuronType.Motor:
    print("This neuron controls action")
```

### `SynapseType`
Enumeration of synapse connection types.

**Values:**
- `Excitatory`: Promotes neural firing
- `Inhibitory`: Suppresses neural firing
- `Modulatory`: Broad neuromodulatory effect
- `Electrical`: Direct electrical coupling
- `GapJunction`: Electrical synapse

### `DevelopmentalStage`
Enumeration of brain developmental stages.

**Values:**
- `Initial`: Starting state, high plasticity
- `CriticalPeriod`: Sensitive learning window
- `Maturation`: Circuit refinement
- `Adult`: Mature, stable state
- `Aging`: Decline and adaptation

### `FiringState`
Enumeration of neuron firing states.

**Values:**
- `Resting`: Not firing, below threshold
- `Active`: Currently firing action potentials
- `Refractory`: Recovery period after firing
- `Inhibited`: Suppressed by inhibitory input

### `ActionType`
Enumeration of action types for motor output.

**Values:**
- `MoveForward`, `MoveBackward`: Movement actions
- `TurnLeft`, `TurnRight`: Orientation changes
- `Look`, `LookUp`, `LookDown`: Gaze control
- `Interact`, `Eat`, `Drink`: Goal-directed actions
- `Rest`, `Wait`: Passive states
- `Custom`: User-defined action

### `MotorCommand`
Enumeration of low-level motor commands.

**Values:**
- `MoveForward`, `MoveBackward`: Movement
- `TurnLeft`, `TurnRight`: Rotation
- `LookLeft`, `LookRight`: Gaze direction
- `Interact`: Interaction with environment
- `Wait`: No action

### `WorldObjectType`
Enumeration of world object types.

**Values:**
- `Empty`: No object
- `Resource`: Valuable object
- `Hazard`: Dangerous object
- `Wall`: Impassable barrier
- `Marker`: Landmark/reference point

### `ConfigSource`
Enumeration of configuration sources.

**Values:**
- `Default`: Built-in defaults
- `File`: Loaded from configuration file
- `CommandLine`: Set via command-line arguments
- `Runtime`: Set at runtime

---

## Brain Class

The central neural simulation brain implementing LIF dynamics with integrated systems.

#### Constructor
```python
brain = Brain(config)
```

#### Core Methods

- `initialize()`
  Initialize the brain with configuration and create neural components

- `step(currentStep)`
  Perform a simulation step with current step number

- `step(currentStep, currentTime)`
  Perform a simulation step with timestamp

- `injectCurrent(neuron, current)`
  Inject current into a specific neuron

- `injectCurrentToNeurons(neuron_type, current)`
  Inject current into all neurons of a specific type

- `receiveSensoryInput(input)`
  Receive sensory input and inject into sensory neurons

- `produceAction()` → `Action`
  Generate motor action based on current neural activity

- `reset()`
  Reset brain to initial state

- `save(filepath)` → `bool`
  Save brain state to file

- `load(filepath)` → `bool`
  Load brain state from file

#### Query Methods

- `getTotalNeuronCount()` → `int64`
  Get total number of neurons

- `getTotalSynapseCount()` → `int64`
  Get total number of synapses

- `getActiveNeuronCount()` → `int64`
  Get number of currently active neurons

- `getFiringNeuronCount()` → `int64`
  Get number of currently firing neurons

- `getAverageFiringRate()` → `float`
  Get average firing rate across all neurons

- `getExcitationInhibitionRatio()` → `float`
  Get E/I balance ratio

- `getTotalSpikeCount()` → `int64`
  Get total spike count since initialization

- `getDevelopmentalStage()` → `DevelopmentalStage`
  Get current developmental stage

- `setDevelopmentalStage(stage)`
  Set developmental stage

- `getWorkingMemorySize()` → `int64`
  Get working memory item count

- `getEpisodicMemorySize()` → `int64`
  Get episodic memory item count

- `getMeanSynapticWeightChange()` → `float`
  Get average synaptic weight change

- `getConfig()` → `Config`
  Get associated configuration

- `logStatus()`
  Log current brain status and statistics

- `enablePlasticity(flags)`
  Enable plasticity mechanisms

#### Advanced Query Methods

- `getRegion(region_id)` → `NeuralRegion`
  Get specific neural region

- `addRegion(name="") → `RegionId`
  Add new neural region

- `getRegionCount()` → `int64`
  Get number of regions

- `getRegionIds()` → `List[RegionId]`
  Get all region IDs

- `getRegions()` → `List[NeuralRegion]`
  Get all regions

---

## AgentBrain Class

Connects a neural brain to a world environment through sensory-motor interface.

#### Constructor
```python
agent = AgentBrain(brain)
```

#### Core Processing Methods

- `initialize(world)`
  Initialize agent with world environment

- `processSensoryInput(percept)`
  Process sensory percept and inject into brain

- `decodeMotorCommand()` → `Action`
  Decode brain motor activity into motor command

- `applyRewardModulation(reward, predicted_reward)`
  Apply reward-based neuromodulation

- `updateDevelopment(timestep)`
  Update development system

- `reset()`
  Reset agent for new episode

#### Query Methods

- `getSensoryInputSize()` → `int64`
  Get expected sensory input size

- `getMotorOutputSize()` → `int64`
  Get expected motor output size

- `getBrain()` → `Brain`
  Get underlying brain

- `getDevelopmentalStage()` → `DevelopmentalStage`
  Get current developmental stage

- `getNeuromodulationLevel()` → `float`
  Get current neuromodulation level

- `getCuriosityLevel()` → `float`
  Get curiosity level

- `getNoveltyLevel()` → `float`
  Get novelty level

- `getPredictionError()` → `float`
  Get prediction error

- `getPlasticityEnabled()` → `bool`
  Check if plasticity is enabled

---

## SimpleWorld Class

2D world environment for NLM agent simulations.

#### Constructor
```python
world = SimpleWorld()
```

#### Configuration

- `configure(width, height, visionWidth, visionHeight)`
  Set world dimensions and agent vision

- `setAgentStart(x, y)`
  Set agent starting position

#### Core Methods

- `reset()`
  Reset world to initial state

- `update(timestep)`
  Update world simulation

- `applyMotorCommand(cmd, currentTime)` → `ActionResult`
  Apply motor command and return result

- `getSensoryPercept()` → `SensoryPercept`
  Get current sensory percept

- `getAgentBody()` → `AgentBody`
  Get agent body state

- `addObject(obj)`
  Add object to world

- `removeObject(x, y)`
  Remove object at position

#### Query Methods

- `isValidPosition(x, y)` → `bool`
  Check if position is valid

- `getWidth()` → `float`
  Get world width

- `getHeight()` → `float`
  Get world height

- `getVisionWidth()` → `int64`
  Get vision width

- `getVisionHeight()` → `int64`
  Get vision height

- `getMaxEnergy()` → `float`
  Get maximum agent energy

- `setMaxEnergy(e)`
  Set maximum energy

- `getEnergyDecayRate()` → `float`
  Get energy decay rate

- `setEnergyDecayRate(r)`
  Set energy decay rate

- `getSimulationTime()` → `double`
  Get current simulation time

- `getRandomSeed()` → `int64`
  Get random seed

- `setRandomSeed(seed)`
  Set random seed

---

## Sensory Classes

### `Vision`
Vision sensory input with spatial data.

- `Vision(width, height, channels=3)`
  Create vision input

- `setData(data)`
  Set vision data

- `getWidth()` → `int64`
  Get width

- `getHeight()` → `int64`
  Get height

- `getChannels()` → `int64`
  Get number of channels

### `Audio`
Audio sensory input with temporal data.

- `Audio(sampleRate, numSamples)`
  Create audio input

- `setData(data)`
  Set audio data

- `setSampleRate(rate)`
  Set sample rate

- `getSampleRate()` → `int64`
  Get sample rate

- `getNumSamples()` → `int64`
  Get number of samples

### `InternalSignals`
Internal neural state signals.

- `InternalSignals()`
  Create internal signals

- `addSignal(value)`
  Add signal value

- `clearSignals()`
  Clear all signals

### `SensoryInput`
Base class for sensory inputs.

- `getType()` → `NeuronType`
  Get input type

- `getData()` → `List[float]`
  Get raw data

- `getDimensions()` → `int64`
  Get dimensionality

- `getTimestamp()` → `double`
  Get timestamp

- `setTimestamp(timestamp)`
  Set timestamp

---

## Action Classes

### `Action`
Motor command representation.

- `Action()`
  Create empty action

- `Action(type)`
  Create action with type

- `Action(type, parameters)`
  Create action with type and parameters

**Attributes:**
- `type` (ActionType): Action type
- `parameters` (List[float]): Action parameters

**Methods:**
- `getType()` → `ActionType`
  Get action type

- `setType(type)`
  Set action type

- `getParameters()` → `List[float]`
  Get parameters

- `setParameters(params)`
  Set parameters

- `getName()` → `str`
  Get human-readable name

- `clone()` → `Action`
  Create copy

### `ActionResult`
Result of applying an action.

- `ActionResult()`
  Create empty result

- `ActionResult(reward, success, message="")`
  Create with reward and success

**Attributes:**
- `reward` (float): Reward value
- `success` (bool): Whether action succeeded
- `message` (str): Result message

---

## State Classes

### `AgentBody`
Agent physical state.

**Attributes:**
- `x`, `y`: Position
- `orientation`: Facing direction
- `velocityX`, `velocityY`: Linear velocity
- `angularVelocity`: Rotational velocity
- `energy`: Energy level
- `health`: Health status
- `age`: Age in simulation steps
- `isMoving`: Movement state
- `isTurning`: Turning state
- `lastActionTime`: Time of last action

### `WorldObject`
World object representation.

**Attributes:**
- `x`, `y`: Position
- `radius`: Size
- `type` (WorldObjectType): Object type
- `value`: Object value
- `active`: Activation state

### `SensoryPercept`
Complete sensory perception data.

**Attributes:**
- `vision`: Visual data
- `touch`: Tactile data
- `internal`: Internal state signals
- `proprioception`: Body position sense
- `audio`: Audio data

**Methods:**
- `getVision()`, `setVision(data)`
- `getTouch()`, `setTouch(data)`
- `getInternal()`, `setInternal(data)`
- `getProprioception()`, `setProprioception(data)`
- `getAudio()`, `setAudio(data)`
- `getAllSignals()` → `Dict[str, List[float]]`
- `getTimestamp()`, `setTimestamp(timestamp)`
---

## Memory System Classes

### `NeuralWorkingMemory`
Short-term memory for active information.

- `getCapacity()` → `int64`
- `getSize()` → `int64`
- `addItem(item)`
- `removeItem(item_id)`
- `getItem(item_id)`
- `clear()`

### `NeuralEpisodicMemory`
Long-term memory for experiences.

- `getCapacity()` → `int64`
- `getSize()` → `int64`
- `encode(experience)`
- `replay(count)` → `List[Experience]`
- `getConsolidationRatio()` → `float`

### `NeuralAssociativeMemory`
Pattern-based memory association.

- `addAssociation(pattern, item)`
- `retrieve(pattern)` → `List[Any]`
- `getSimilarity(item1, item2)` → `float`
---

## Neuromodulation Classes

### `Dopamine`
Dopamine-based reward modulation.

### `Curiosity`
Curiosity-driven exploration.

### `PredictionError`
Prediction error-based learning.

### `Novelty`
Novelty detection.

All neuromodulator classes typically have:
- `getLevel()` → `float`
- `setLevel(level)`
- `getStrength()` → `float`
- `setStrength(strength)`

---

## Development Classes

### `DevelopmentSystem`
Brain development and maturation.

### `Synaptogenesis`
Connection formation during development.

### `Pruning`
Connection elimination during development.

### `Maturation`
Neural maturation and refinement.

---

## Performance Classes

### `CheckpointManager`
Brain state management.

### `MemoryPool`
Efficient memory allocation.

### `EventQueue`
Spike event management.

---

## Constants and Configuration

### Default Values

```python
DEFAULT_CONFIG = {
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.Youth,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00005,
    "working_memory_capacity": 100,
    "episodic_memory_capacity":  więcej50,
    "environment_width": 10,
    "environment_height": 10,
    "vision_width": 8,
    "vision_height": 8,
    "log_level": "INFO",
    "log_to_file": False
}
```

---

## Utility Functions

### Error Handling

```python
try:
    result = brain.step(step)
except RuntimeError as e:
    print(f"Simulation error: {e}")
```

### Configuration Validation

```python
def validate_config(config):
    """Validate configuration parameters."""
    required_keys = ["neuron_count", "region_count", "connection_probability"]
    for key in required_keys:
        if not config.has(key):
            raise ValueError(f"Missing required config key: {key}")
```

### Simulation Utilities

```python
def run_brain_simulation(brain, steps, world=None, agent=None):
    """Run complete simulation with optional world and agent."""
    for step in range(steps):
        # Process world if provided
        if world and agent:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(step)
        
        # Generate action if agent provided
        if agent:
            action = agent.decodeMotorCommand()
            if world:
                world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain.getTotalSpikeCount()
```

---

## Error Handling

The framework raises various exceptions:

- `RuntimeError`: General simulation errors
- `ValueError`: Invalid configuration or parameters
- `IndexError`: Invalid neuron or synapse IDs
- `MemoryError`: Memory allocation failures

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

try:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
except RuntimeError as e:
    print(f"Simulation failed: {e}")
    # Handle error or cleanup
```

---

## Performance Tips

1. **Memory Optimization**: Use `working_memory_capacity` and `episodic_memory_capacity` to control memory usage

2. **Performance Monitoring**: Track `brain.getTotalSpikeCount()` and `brain.getAverageFiringRate()`

3. **Learning Balance**: Adjust `stdp_ltp_weight` and `stdp_ltd_weight` for desired learning behavior

4. **Developmental Timing**: Use `brain.setDevelopmentalStage()` to control learning phases

5. **Parallel Processing**: Enable multi-threading for large-scale simulations

6. **Checkpointing**: Use `brain.save()` and `brain.load()` for long simulations

---

## Advanced Usage Patterns

### Pattern 1: Learning with Reward

```python
import pynlm

# Create learning environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Learning loop
for step in range(500):
    # Environment interaction
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Reward-based learning
    if result.reward != 0:
        agent.applyRewardModulation(result.reward, 0.0)
```

### Pattern 2: Developmental Learning

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Start with high plasticity (critical period)
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
brain.initialize()

# Initial learning phase
for step in range(200):
    brain.step(step)

# Transition to maturation
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)

# Refinement phase
for step in range(200, 400):
    brain.step(step)

# Adult phase - stable behavior
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
```

### Pattern 3: Memory-Based Control

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("working_memory_capacity", 200)
config.set("episodic_memory_capacity", 100)

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Store past experiences in episodic memory
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Store experience for later recall
    experience = {
        "step": step,
        "percept": percept.getVision(),
        "action": agent.decodeMotorCommand(),
        "outcome": world.getReward()
    }
    # Store in episodic memory

# Use memory to guide decisions
for step in range(100, 200):
    # Retrieve relevant past experiences
    relevant_experiences = brain.getEpisodicMemory().retrieve("similar_to_current")
    
    # Incorporate memory into decision making
    for experience in relevant_experiences:
        agent.applyRewardModulation(experience["outcome"], 0.0)
    
    # Continue simulation
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
```

---

## Migration Guide

### From Version 0.0 to 1.0

1. **Configuration System**: New `Config` class with validation and source tracking
2. **Memory System**: Enhanced with `NeuralWorkingMemory` and `NeuralEpisodicMemory`
3. **Neuromodulation**: Added `Curiosity`, `Novelty`, and `PredictionError` systems
4. **Development**: Full developmental system with stages and maturation
5. **Python API**: Consistent naming and improved error handling

### From C++ to Python

**C++:**
```cpp
auto brain = std::make_shared<Brain>(config);
brain->initialize();
brain->step(step);
brain->injectCurrent(neuron, current);
```

**Python:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
brain.injectCurrent(neuron, current)
```

### From Old API to New API

**Old:**
```python
# Limited functionality
brain = Brain(config)
brain.init()
brain.step(step)
```

**New:**
```python
# Full feature set
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
# Plus memory, neuromodulation, development, etc.
```

---

## Testing and Validation

### Unit Testing

```python
import pynlm
import unittest

class TestBrain(unittest.TestCase):
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
    
    def test_initialization(self):
        self.brain.initialize()
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_simulation(self):
        self.brain.initialize()
        spikes_before = self.brain.getTotalSpikeCount()
        for step in range(10):
            self.brain.step(step)
        spikes_after = self.brain.getTotalSpikeCount()
        self.assertGreater(spikes_after, spikes_before)
```

### Performance Testing

```python
import pynlm
import time

def benchmark_brain(neuron_counts, steps):
    results = {}
    for neuron_count in neuron_counts:
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count)
        
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        elapsed = time.time() - start_time
        results[neuron_count] = {
            "time": elapsed,
            "spikes": brain.getTotalSpikeCount(),
            "rate": brain.getAverageFiringRate()
        }
    
    return results

# Run benchmark
results = benchmark_brain([100, 500, 1000, 5000], 100)
```

---

## References

1. **NLM Documentation**: `docs/ARCHITECTURE.md`, `docs/SCIENCE.md`
2. **C++ API**: Source headers in `src/` directory
3. **Configuration**: `configs/default.cfg`
4. **Phase Demos**: `nlm_phase*_demo` executables
5. **Research Papers**: Available in `docs/` directory

---

This API reference provides comprehensive documentation for the NLM Python bindings. For the most current information, always check the `pynlm` module documentation and source code.
