# NLM Python API Reference

## Overview

The NLM Python bindings provide comprehensive access to the Neural Learning Machine neural simulation framework, including all Phase 6 integrated systems for brain simulation, memory, learning, and environmental interaction.

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

## Core API Reference

### Configuration

#### `createDefaultConfig()`
Creates a default configuration for brain simulation.

```python
config = pynlm.createDefaultConfig()
```

#### `Config` class methods

| Method | Description |
|--------|-------------|
| `loadFromFile(filepath)` | Load configuration from JSON file |
| `loadFromArgs(argc, argv)` | Load from command line arguments |
| `saveToFile(filepath)` | Save configuration to JSON file |
| `has(key)` | Check if configuration key exists |
| `getKeys()` | Get all configuration keys |
| `clear()` | Clear all configuration entries |
| `summary()` | Get configuration summary string |

#### Dictionary-like interface

```python
config["brain.neuron_count"] = 1000
value = config["brain.neuron_count"]  # Returns Any, can be cast to specific type
key in config  # Check if key exists
config.getOr("missing", 100)  # Get with default
```

### Brain Simulation

#### `createBrain(config)`
Creates a brain with the given configuration.

```python
brain = pynlm.createBrain(config)
```

#### Factory functions

```python
# By complexity
brain = pynlm.createBrain("simple")
brain = pynlm.createBrain("medium") 
brain = pynlm.createBrain("complex")

# By parameters
brain = pynlm.createBrain(1000, 5)  # neuron_count, region_count
```

#### `Brain` class methods

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain |
| `step(step)` | Perform simulation step |
| `step(step, time)` | Perform step with timestamp |
| `reset()` | Reset brain state |
| `receiveSensoryInput(input)` | Inject sensory input |
| `produceAction()` | Produce motor action |
| `save(filepath)` | Save brain state |
| `load(filepath)` | Load brain state |
| `addRegion(name)` | Add neural region |

#### Properties and statistics

| Method | Description |
|--------|-------------|
| `getTotalNeuronCount()` | Total neurons across all regions |
| `getTotalSynapseCount()` | Total synapses |
| `getFiringNeuronCount()` | Currently firing neurons |
| `getActiveNeuronCount()` | Active neurons |
| `getAverageFiringRate()` | Average firing rate (Hz) |
| `getExcitationInhibitionRatio()` | E/I balance ratio |
| `getTotalSpikeCount()` | Total spike count |
| `getDevelopmentalStage()` | Current developmental stage |

#### Context Manager

```python
with brain.create_context() as ctx:
    ctx.step(step)
    ctx.processSensoryInput(percept)
    action = ctx.produceAction()
    snapshot = ctx.getSnapshot()
```

### Agent Brain Interface

#### `createAgentBrain(brain)`
Creates agent brain interface connecting brain to world.

```python
agent = pynlm.createAgentBrain(brain)
```

#### `createLearningAgent(brain, **kwargs)`
Creates agent with learning subsystems enabled.

```python
agent = pynlm.createLearningAgent(
    brain,
    enableRewardModulation=True,
    enableStructuralPlasticity=True,
    enableDevelopment=True,
    enableCuriosity=True
)
```

#### `AgentBrain` methods

| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize with world |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Decode motor command from brain activity |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward-based neuromodulation |
| `updateDevelopment(timestep)` | Update development system |

#### Learning status methods

| Method | Description |
|--------|-------------|
| `getLearningStatus()` | Get comprehensive learning status dictionary |
| `getLearningSummary()` | Get human-readable learning status summary |

### World Simulation

#### `createSimpleWorld()`
Creates a simple 2D world for simulation.

```python
world = pynlm.createSimpleWorld()
```

#### `SimpleWorld` methods

| Method | Description |
|--------|-------------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `reset()` | Reset world to initial state |
| `setAgentStart(x, y)` | Set agent starting position |
| `update(timestep)` | Update world simulation |
| `applyMotorCommand(cmd, currentTime)` | Apply motor command |
| `getSensoryPercept()` | Get sensory percept from world |
| `getAgentBody()` | Get agent body state |

### Sensory Input

#### Vision

```python
# Create vision with default parameters
vision = pynlm.Vision()

# Create with custom parameters  
vision = pynlm.Vision(8, 8, 3)

# Set data
vision.setData([0.1, 0.2, 0.3, ...])

# Get properties
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

#### Audio

```python
audio = pynlm.Audio(44100, 1024)  # sample_rate, num_samples
audio.setData([0.5, 0.3, 0.7, ...])
sample_rate = audio.getSampleRate()
```

#### InternalSignals

```python
internal = pynlm.InternalSignals()
internal.addSignal(0.8)
internal.addSignal(-0.3)
internal.clearSignals()
```

### Action System

#### Action creation

```python
# By action type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# With parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.2, 0.8])

# Get properties
type = action.getType()
params = action.getParameters()
name = action.getName()
```

#### WorldObject

```python
# Create different object types
resource = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 5.0)
hazard = pynlm.WorldObject(3.0, 4.0, pynlm.WorldObjectType.Hazard)
wall = pynlm.WorldObject(5.0, 6.0, pynlm.WorldObjectType.Wall, radius=1.0)

# Properties
print(f"Position: ({resource.x}, {resource.y})")
print(f"Type: {resource.type}")
print(f"Value: {resource.value}")
```

### Helper Functions

#### `configureBrain(config, neuronCount, regionCount, **kwargs)`
Configure brain with common parameters.

```python
config = pynlm.createDefaultConfig()
pynlm.configureBrain(config, 1000, 10, connectionProbability=0.15)
```

#### Factory helpers

```python
# Vision/audio creation helpers
vision = pynlm.createVision(8, 8, 3)
audio = pynlm.createAudio(44100, 1024)
```

### Constants

| Constant | Type | Description |
|----------|------|-------------|
| `INVALID_NEURON_ID` | NeuronId | Invalid neuron ID |
| `INVALID_SYNAPSE_ID` | SynapseId | Invalid synapse ID |
| `INVALID_REGION_ID` | RegionId | Invalid region ID |
| `INVALID_POPULATION_ID` | PopulationId | Invalid population ID |

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons  
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### SynapseType
- `Excitatory` - Excitatory synapses
- `Inhibitory` - Inhibitory synapses
- `Modulatory` - Modulatory synapses
- `Electrical` - Electrical synapses
- `GapJunction` - Gap junction synapses

#### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### ActionType
See `HOW_TO_USE.md` for complete list of action types.

### Context Manager

```python
# Create brain context
with brain.create_context() as ctx:
    # Safe, controlled operations
    ctx.step(0)
    ctx.processSensoryInput(percept)
    action = ctx.produceAction()
    
    # Get snapshot
    snapshot = ctx.getSnapshot()
    print(f"Step {snapshot.step}, firing: {snapshot.firing_neuron_count}")
```

### Error Handling

NLM provides Pythonic error handling:

```python
try:
    brain.initialize()
    brain.step(0)
except ValueError as e:
    print(f"Configuration error: {e}")
except RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Advanced Patterns

#### Agent lifecycle with learning

```python
# Create complete agent with learning
brain = pynlm.createBrain("complex")
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 10, 10)
world.reset()

agent = pynlm.createLearningAgent(
    brain,
    enableRewardModulation=True,
    enableCuriosity=True
)
agent.initialize(world)

# Run simulation with learning
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    agent.applyRewardModulation(reward, predicted_reward)
```

#### Data processing and serialization

```python
# Process sensory data
percept1 = world.getSensoryPercept()
percept2 = world.getSensoryPercept()
percept1.merge(percept2)

# Serialize to dictionary
data = percept1.to_dict()

# Create brain snapshot
with brain.create_context() as ctx:
    snapshot = ctx.getSnapshot()
    print(snapshot.to_string())
```

#### World interaction with object management

```python
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

# Add world objects
world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 10.0))
world.addObject(pynlm.WorldObject(25, 25, pynlm.WorldObjectType.Hazard, radius=2.0))
world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Wall, radius=1.5))
```

### Performance Monitoring

See `performance.py` for comprehensive performance monitoring and optimization tools.

### Examples

See `examples/advanced_usage.py` for comprehensive examples of advanced usage patterns.

## Migration Guide

### From C++ to Python

The NLM Python API provides a high-level interface that abstracts away the C++ implementation details:

**C++ approach:**
```cpp
nlm::Config config;
config.set("neuron_count", 1000);
nlm::Brain brain(config);
brain.initialize();
```

**Python approach:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
```

### From Old Python API to New Pythonic API

**Old patterns:**
```python
# Direct object access (still supported)
neuron_id = neuron.getId().value
config_value = config.get("key")
```

**New Pythonic patterns:**
```python
# Property-like access
neuron_id = neuron.id  # If property is implemented
config_value = config["key"]  # Dictionary-like access
```

## Type Safety

The NLM Python bindings include type hints and `py.typed` marker for better IDE support and type checking.

## Performance Considerations

1. **Batch operations**: Process multiple steps in loops
2. **Memory management**: Use context managers for controlled resource usage
3. **Simulation speed**: Use appropriate brain complexity for your use case
4. **Profiling**: Use `performance.py` for optimization

## Troubleshooting

### Common Issues

**"ModuleNotFoundError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"ImportError: libpynlm.so: cannot open shared object file""**
```bash
# On Linux/macOS
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# On Windows
# Check installation instructions in README.md
```

**"Brain initialization fails"**
```python
# Check configuration
config = pynlm.createDefaultConfig()
print(config.summary())

# Try with explicit parameters
config.set("brain.neuron_count", 100)
config.set("brain.region_count", 2)
```

## License

MIT License. See `LICENSE` file for details.

## Documentation Updates

This API reference is continuously updated. Check the `docs/` directory for architectural documentation and scientific background.
