# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides an intuitive interface to the complete neural simulation framework. This documentation covers all available classes, methods, and examples for working with the Python bindings.

## Installation

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

## Quick Start

### Basic Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent Example

```python
import pynlm

# Create brain, world, and agent
brain = pynlm.createBrain()
brain.initialize()

world = pynlm.createWorld(width=20, height=20)
world.configure(visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableAll()

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Core Classes

### Config

The `Config` class manages simulation parameters and settings.

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)

# Get configuration values
neuron_count = config.get("neuron_count", 1000)
print(f"Neuron count: {neuron_count}")

# Save/load configuration
config.saveToFile("my_config.json")
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Get all keys
keys = config.getKeys()
print(f"All keys: {keys}")

# Get summary
print(config.summary())
```

**Key Methods:**
- `set(key, value)` - Set configuration value
- `get(key, default=None)` - Get configuration value with type conversion
- `has(key)` - Check if key exists
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all settings
- `summary()` - Get configuration summary

### Brain

The `Brain` class represents the neural simulation engine.

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Step through simulation
for step in range(100):
    brain.step(step)

# Inject current into neurons
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Save/load brain state
brain.save("checkpoint.bin")
brain2 = pynlm.createBrain(config)
brain2.load("checkpoint.bin")

# Get brain snapshot
snapshot = brain.getBrainSnapshot()
print(f"Snapshot: {snapshot}")
```

**Key Methods:**
- `initialize()` - Initialize the brain
- `step(step, time=0.0)` - Perform simulation step
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to all neurons of type
- `produceAction()` - Get motor action
- `receiveSensoryInput(input)` - Inject sensory input
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()` - Get total neurons
- `getTotalSynapseCount()` - Get total synapses
- `getFiringNeuronCount()` - Get firing neurons
- `getTotalSpikeCount()` - Get total spikes
- `getDevelopmentalStage()` - Get developmental stage
- `getConfig()` - Get configuration
- `logStatus()` - Log brain status

### SimpleWorld

The `SimpleWorld` class represents the simulated environment.

```python
import pynlm

# Create world with default settings
world = pynlm.createWorld(width=30, height=30)
world.configure(visionWidth=8, visionHeight=8)
world.reset()

# Set agent starting position
world.setAgentStart(15.0, 15.0)

# Run world update
timeStep = 0.1
world.update(timeStep)

# Get sensory percept
percept = world.getSensoryPercept()
print(f"Percept type: {percept.getType()}")
print(f"Percept dimensions: {percept.getDimensions()}")

# Get agent body
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation}")

# Add objects to world
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=0.5))

# Apply motor command
action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.5, 0.5])
world.applyMotorCommand(action, world.getSimulationTime())

# Get world statistics
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Environment complexity: {world.getEnvironmentComplexity():.2f}")
print(f"Available resources: {world.getAvailableResources():.2f}")
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world by timestep
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()`, `getHeight()` - Get world dimensions
- `getSimulationTime()` - Get current simulation time

### AgentBrain

The `AgentBrain` class connects the neural brain to the environment.

```python
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createWorld(width=20, height=20)
world.configure(visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableAll()

# Run complete simulation loop
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get learning status
learning_status = agent.getLearningStatus()
print(f"Learning status: {learning_status}")

# Get neuromodulation levels
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain motor activity
- `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system
- `trainEpisode(steps)` - Train for one episode
- `trainAgent(episodes, stepsPerEpisode)` - Train over multiple episodes
- `enableAll()` - Enable all learning subsystems
- `disableAll()` - Disable all learning subsystems

### SensoryInput Classes

#### Vision

```python
import pynlm

# Create vision input
vision = pynlm.Vision(width=64, height=64, channels=3)

# Set vision data
data = [0.1] * (64 * 64 * 3)  # Example data
vision.setData(data)

# Get vision data
print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")

# Get normalized data
normalized = vision.getNormalizedData()

# Validate data
validation = vision.validateData()
print(f"Data valid: {validation['valid']}")

# Apply filters
gray = vision.getGrayscale()

# Draw shapes
vision.drawCrosshair(32, 32, 10)
vision.drawCircle(50, 50, 15, filled=True)
```

#### Audio

```python
import pynlm

# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio data
samples = [0.0] * 1024  # Example audio samples
audio.setData(samples)
audio.setSampleRate(44100)

print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
```

#### InternalSignals

```python
import pynlm

# Create internal signals
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.5)
internal.addSignal(0.8)
internal.addSignal(0.3)

# Get all signals
signals = internal.getData()
print(f"Internal signals: {signals}")

internal.clearSignals()
```

### Action

```python
import pynlm

# Create action with type
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action type: {action1.getType()}")
print(f"Action name: {action1.getName()}")

# Create action with parameters
action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.5, 0.5])
print(f"Action parameters: {action2.getParameters()}")

# Clone action
action3 = action1.clone()
```

### Enumerations

All enumerations are available as class attributes:

```python
import pynlm

# Neuron types
print(f"Neuron types: {pynlm.NeuronType.Excitatory}, {pynlm.NeuronType.Inhibitory}")

# Action types
print(f"Action types: {pynlm.ActionType.MoveForward}, {pynlm.ActionType.TurnLeft}")

# World object types
print(f"Object types: {pynlm.WorldObjectType.Resource}, {pynlm.WorldObjectType.Hazard}")

# Developmental stages
print(f"Stages: {pynlm.DevelopmentalStage.Initial}, {pynlm.DevelopmentalStage.Adult}")
```

## Advanced Usage

### Neural Network Building

```python
import pynlm

# Create custom network configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("region_count", 3)
config.set("connection_probability", 0.05)
config.set("stdp.enable", True)
config.set("hebbian.enable", True)
config.set("structural.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create multiple regions
region1_id = brain.addRegion("cortex")
region2_id = brain.addRegion("hippocampus")
region3_id = brain.addRegion("amygdala")
```

### Experiment Control

```python
import pynlm

# Run comprehensive experiment
brain = pynlm.createBrain()
brain.initialize()

world = pynlm.createWorld(30, 30)
world.configure(visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run experiment with statistics
results = agent.trainAgent(episodes=10, stepsPerEpisode=100)
print(f"Experiment results: {results}")
```

### Performance Benchmarking

```python
import pynlm

# Benchmark brain performance
brain = pynlm.createBrain()
brain.initialize()

benchmark = pynlm.benchmarkBrain(brain, steps=1000)
print(f"Benchmark: {benchmark}")

# Benchmark agent performance
world = pynlm.createWorld(20, 20)
world.configure(visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for episode in range(5):
    agent.trainEpisode(world, 200)
```

### Error Handling

```python
import pynlm

# Safe operations with error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if brain.validateBrainState():
        print("Brain is valid for simulation")
        brain.initialize()
    else:
        print("Brain validation failed")
        
except Exception as e:
    print(f"Error: {pynlm.getErrorMessage(e)}")
```

## API Reference

### Functions

#### Factory Functions

| Function | Description |
|----------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create brain with configuration |
| `createBrain()` | Create brain with default config |
| `createSimpleWorld()` | Create simple world |
| `createWorld(width, height)` | Create world with dimensions |
| `createAgentBrain(brain)` | Create agent brain interface |

#### Utility Functions

| Function | Description |
|----------|-------------|
| `runIntegrationTest(brain, world)` | Run basic integration test |
| `benchmarkSimulation(brain, steps, verbose)` | Benchmark simulation performance |
| `validateBrain(brain)` | Validate brain state |
| `runExperiment(brain, world, episodes, stepsPerEpisode)` | Run complete experiment |

### Class Methods Summary

#### Config
- `loadFromFile(filepath)`, `loadFromArgs(argc, argv)`
- `saveToFile(filepath)`, `set(key, value)`, `get(key, default)`
- `has(key)`, `getKeys()`, `clear()`, `summary()`

#### Brain
- `initialize()`, `step(step, time)`, `reset()`
- `injectCurrent(neuron, current)`, `injectCurrentToNeurons(type, current)`
- `produceAction()`, `receiveSensoryInput(input)`
- `save(filepath)`, `load(filepath)`, `addRegion(name)`
- `getRegion(id)`, `getTotalNeuronCount()`, `getTotalSynapseCount()`
- `getFiringNeuronCount()`, `getTotalSpikeCount()`, `getDevelopmentalStage()`

#### SimpleWorld
- `configure(width, height, visionWidth, visionHeight)`
- `reset()`, `setAgentStart(x, y)`, `update(timestep)`
- `applyMotorCommand(cmd, time)`, `getSensoryPercept()`
- `getAgentBody()`, `addObject(obj)`, `isValidPosition(x, y)`

#### AgentBrain
- `initialize(world)`, `processSensoryInput(percept)`
- `decodeMotorCommand()`, `applyRewardModulation(reward, predicted)`
- `updateDevelopment(timestep)`, `trainEpisode(steps)`
- `enableAll()`, `disableAll()`, `getLearningStatus()`

#### Vision
- `__init__(width, height, channels)`, `setData(data)`
- `getData()`, `getNormalizedData()`, `validateData()`
- `getWidth()`, `getHeight()`, `getChannels()`
- `getGrayscale()`, `drawCrosshair(x, y, radius)`

#### Audio
- `__init__(sampleRate, numSamples)`, `setData(data)`
- `setSampleRate()`, `getSampleRate()`, `getNumSamples()`

#### InternalSignals
- `__init__()`, `addSignal(value)`, `getData()`, `clearSignals()`

#### Action
- `__init__(type, parameters=None)`, `getType()`, `getParameters()`
- `getName()`, `setParameters()`, `clone()`

## Examples

### Example 1: Silent Brain

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Complete Agent with Learning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure motor neurons are active

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

**"ImportError: No module named 'pynlm'"**
- Install the package: `pip install .`
- Install in development mode: `pip install -e .`

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for null pointers in your code

### Debugging Tips

```python
import pynlm

# Create brain with logging enabled
brain = pynlm.createBrain()
brain.initialize()

# Log brain status at intervals
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        brain.logStatus()

# Validate configuration before use
config = pynlm.createDefaultConfig()
if config.has("neuron_count"):
    print(f"Neuron count: {config.get('neuron_count')}")
```

## Performance Tips

1. **Use batch operations**: Perform multiple steps at once when possible
2. **Enable SIMD optimizations**: Configure CMake with appropriate flags
3. **Monitor memory usage**: Use `getTotalNeuronCount()` and `getTotalSynapseCount()`
4. **Use efficient data structures**: Python bindings pass data efficiently
5. **Benchmark regularly**: Use `benchmarkSimulation()` to track performance

## Advanced Features for Researchers

### Custom Configurations

```python
import pynlm
import json

# Create custom configuration
custom_config = {
    "neuron_count": 10000,
    "region_count": 5,
    "connection_probability": 0.08,
    "plasticity": {
        "stdp": {"enable": True, "learning_rate": 0.002},
        "hebbian": {"enable": True},
        "structural": {"enable": True}
    }
}

# Convert to NLM config
config = pynlm.createDefaultConfig()
for key, value in custom_config.items():
    config.set(key, value)
```

### Neural Architecture Search

```python
import pynlm

# Automated network search
best_config = None
best_score = 0.0

for neurons in [500, 1000, 2000]:
    for regions in [1, 2, 3]:
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neurons)
        config.set("region_count", regions)
        
        # Test configuration
        brain = pynlm.createBrain(config)
        if brain.initialize():
            # Run simple test
            for step in range(50):
                brain.step(step)
            
            # Score based on activity
            score = brain.getTotalSpikeCount() / (neurons * 0.01)
            if score > best_score:
                best_score = score
                best_config = config
```

### Multi-Experiment Management

```python
import pynlm
import time

def run_experiment(experiment_id, config_overrides):
    """Run a single experiment with given configuration"""
    config = pynlm.createDefaultConfig()
    for key, value in config_overrides.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createWorld(20, 20)
    world.configure(visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAll()
    
    start_time = time.time()
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    return {
        "experiment_id": experiment_id,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "duration": end_time - start_time,
        "development_stage": brain.getDevelopmentalStage()
    }

# Run multiple experiments
experiments = []
for i in range(5):
    experiment = run_experiment(i, {"neuron_count": 1000 + i * 500})
    experiments.append(experiment)

# Compare results
for exp in experiments:
    print(f"Exp {exp['experiment_id']}: {exp['total_spikes']} spikes in {exp['duration']:.2f}s")
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python:

1. **Configuration**: Use `createDefaultConfig()` instead of manual config setup
2. **Brain initialization**: `brain.initialize()` is required in Python
3. **Step execution**: `brain.step(step, time)` in Python vs `brain.step(step)` in C++
4. **Memory management**: Python handles garbage collection automatically
5. **Error handling**: Use try-catch blocks for Python exceptions

### Upgrading from Older Versions

- **Version 0.1.0**: Added `trainEpisode()` and `trainAgent()` methods
- **Version 0.0.1**: Initial release

## License and Attribution

The NLM Python bindings are part of the NLM (Neural Learning Machine) project.
See `LICENSE` file for project license information.

## Contact and Support

For issues with the Python API:
- Check the `README.md` for troubleshooting
- Review the examples in this documentation
- Check the NLM project documentation in `docs/`

## Future Enhancements

Planned improvements for future releases:
1. **TensorFlow integration**: Bridge with TensorFlow for deep learning
2. **Distributed computing**: Multi-GPU and multi-machine support
3. **Advanced visualization**: Interactive plotting and 3D visualization
4. **Cloud deployment**: Docker and cloud integration
5. **Real-time simulation**: Support for real-time applications
6. **Extended sensory modalities**: More input types and modalities

## Acknowledgments

The NLM project builds on decades of research in computational neuroscience and artificial intelligence. We acknowledge the contributions of the entire NLM research team and the open-source community.

---

*This documentation is automatically generated from the pybind11 bindings. For the most up-to-date information, refer to the source code.*