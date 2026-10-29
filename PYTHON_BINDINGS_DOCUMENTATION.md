NLM Python Bindings Documentation
=================================

## Overview

The NLM Python bindings provide a complete interface to the NLM (Neural Learning Machine) neural simulation framework using Python. The bindings are implemented using pybind11 and provide a natural, high-level API that maintains full access to all NLM capabilities while being intuitive and easy to use.

The Python bindings include:

- **Brain simulation**: Create and control neural networks
- **World simulation**: Simulate environments and agents  
- **Configuration management**: Load and save configurations
- **Experiment control**: Run and monitor experiments
- **Data analysis**: Analyze and visualize simulation results
- **Performance monitoring**: Monitor system performance and resources
- **Advanced features**: Neuromodulation, development, plasticity

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for step in range(1000):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Example

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
```

## API Reference

### Core Classes

#### pynlm.Config
**Configuration management**

The `Config` class manages configuration for the NLM system. It supports loading from files, command line arguments, and programmatic setup.

**Key Methods:**
- `config.has(key)`: Check if a key exists
- `config.get(key)`: Get configuration value
- `config.set(key, value)`: Set configuration value
- `config.loadFromFile(filepath)`: Load from JSON file
- `config.saveToFile(filepath)`: Save to JSON file
- `config.summary()`: Get configuration summary

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("config.json")

# Check configuration
if config.has("brain.neuron_count"):
    print(f"Neurons: {config.get('brain.neuron_count')}")
```

#### pynlm.Brain
**Main neural simulation component**

The `Brain` class implements the core neural simulation functionality, including neurons, synapses, plasticity, neuromodulation, memory, and development.

**Key Properties:**
- `totalNeuronCount`: Total number of neurons
- `totalSynapseCount`: Total number of synapses
- `firingNeuronCount`: Number of currently firing neurons
- `averageFiringRate`: Average firing rate across neurons
- `excitatioInhibitionRatio`: E/I balance ratio
- `totalSpikeCount`: Total spike count

**Key Methods:**
- `initialize()`: Initialize brain with configuration
- `step(step)`: Run a simulation step
- `step(step, time)`: Run a simulation step with timestamp
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Produce motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.3f} Hz")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

#### pynlm.SimpleWorld
**Simple 2D world simulation**

The `SimpleWorld` class provides a basic 2D environment for NLM simulation, including vision sensing, action execution, and object management.

**Key Properties:**
- `width`: World width
- `height`: World height
- `visionWidth`: Vision width
- `visionHeight`: Vision height
- `simulationTime`: Current simulation time

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `reset()`: Reset world to initial state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body state
- `addObject(x, y, objType, value, radius)`: Add object to world
- `removeObject(x, y)`: Remove object from world
- `isValidPosition(x, y)`: Check if position is valid

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)

# Run simulation
for i in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    print(f"Vision: {percept.getVision()}")
    print(f"Touch: {percept.getTouch()}")
    print(f"Internal: {percept.getInternal()}")
```

#### pynlm.AgentBrain
**Bridge between brain and world**

The `AgentBrain` class connects the NLM brain to the world, handling sensory transduction and motor decoding.

**Key Properties:**
- `curiosityLevel`: Current curiosity level
- `noveltyLevel`: Current novelty level
- `predictionError`: Current prediction error
- `neuromodulationLevel`: Current neuromodulation level

**Key Methods:**
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development system
- `getDevelopmentalStage()`: Get developmental stage
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error

**Example:**
```python
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Create and initialize agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
```

### Enumerations

#### NeuronType
```python
import pynlm

print(pynlm.NeuronType.EXCITATORY)
print(pynlm.NeuronType.INHIBITORY)
print(pynlm.NeuronType.SENSORY)
print(pynlm.NeuronType.MOTOR)
print(pynlm.NeuronType.INTERNAL)
```

#### SynapseType
```python
import pynlm

print(pynlm.SynapseType.EXCITATORY)
print(pynlm.SynapseType.INHIBITORY)
print(pynlm.SynapseType.MODULATORY)
print(pynlm.SynapseType.ELECTRICAL)
print(pynlm.SynapseType.GAP_JUNCTION)
```

#### DevelopmentalStage
```python
import pynlm

print(pynlm.DevelopmentalStage.INITIAL)
print(pynlm.DevelopmentalStage.CRITICAL_PERIOD)
print(pynlm.DevelopmentalStage.MATURATION)
print(pynlm.DevelopmentalStage.ADULT)
print(pynlm.DevelopmentalStage.AGING)
```

#### ActionType
```python
import pynlm

print(pynlm.ActionType.MOVE_FORWARD)
print(pynlm.ActionType.TURN_LEFT)
print(pynlm.ActionType.TURN_RIGHT)
print(pynlm.ActionType.INTERACT)
print(pynlm.ActionType.EAT)
print(pynlm.ActionType.WAIT)
```

#### WorldObjectType
```python
import pynlm

print(pynlm.WorldObjectType.EMPTY)
print(pynlm.WorldObjectType.RESOURCE)
print(pynlm.WorldObjectType.HAZARD)
print(pynlm.WorldObjectType.WALL)
print(pynlm.WorldObjectType.MARKER)
```

### Data Structures

#### SensoryInput
Base class for sensory input data:

```python
import pynlm

percept = pynlm.SensoryPercept()

# Set data
percept.setVision([0.1, 0.2, 0.3])
percept.setTouch([0.0, 1.0])
percept.setInternal([0.5])
percept.setTimestamp(0.0)

# Get data
print(percept.getVision())
print(percept.getTouch())
print(percept.getInternal())
print(percept.getTimestamp())

# Combine all signals
all_signals = percept.getAllSignals()
```

#### Action
Motor action representation:

```python
import pynlm

# Create action from type
action1 = pynlm.Action(pynlm.ActionType.MOVE_FORWARD)

# Create action from type and parameters
action2 = pynlm.Action(pynlm.ActionType.CUSTOM, [0.1, 0.2, 0.3])

# Get action properties
print(action1.getType())
print(action2.getType())
print(action2.getParameters())

# Clone action
action3 = action1.clone()
```

#### WorldObject
World object representation:

```python
import pynlm

# Create world object
obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.RESOURCE, value=1.0)

# Get object properties
print(f"Position: ({obj.x}, {obj.y})")
print(f"Radius: {obj.radius}")
print(f"Type: {obj.type}")
print(f"Value: {obj.value}")
print(f"Active: {obj.active}")
```

#### AgentBody
Agent body state:

```python
import pynlm

body = pynlm.AgentBody()
body.x = 5.0
body.y = 5.0
body.orientation = 0.0
body.velocityX = 0.1
body.velocityY = 0.1
body.angularVelocity = 0.0
body.energy = 100.0
body.health = 100.0
body.age = 0
body.isMoving = False
body.isTurning = False
body.lastActionTime = 0.0

# Reset body
body.reset()
```

### Utility Functions

#### pynlm.createDefaultConfig()
```python
import pynlm

config = pynlm.createDefaultConfig()
```

#### pynlm.createBrain(config)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

#### pynlm.createSimpleWorld()
```python
import pynlm

world = pynlm.createSimpleWorld()
```

#### pynlm.createAgentBrain(brain)
```python
import pynlm

agent = pynlm.createAgentBrain(brain)
```

### Advanced Usage

#### Custom Configuration
```python
import pynlm
import json

# Custom configuration
custom_config = {
    "brain": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.15,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.005,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "learning_rate": 0.01
        }
    }
}

# Create configuration from dictionary
config = pynlm.createConfigFromDict(custom_config)
```

#### Performance Monitoring
```python
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

elapsed = end_time - start_time
steps_per_second = 1000 / elapsed
print(f"Performance: {steps_per_second:.2f} steps/sec")
```

#### Data Collection
```python
import pynlm
import json

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect data
step_data = []
for step in range(1000):
    brain.step(step)
    if step % 10 == 0:
        step_data.append({
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount()
        })

# Save data
with open("simulation_data.json", "w") as f:
    json.dump(step_data, f, indent=2)
```

#### Error Handling
```python
import pynlm

# Configuration error handling
config = pynlm.createDefaultConfig()
if not config.has("brain.neuron_count"):
    print("Warning: brain.neuron_count not set")

# Brain initialization error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Error initializing brain: {e}")

# Simulation error handling
try:
    for step in range(1000):
        brain.step(step)
except Exception as e:
    print(f"Error during simulation: {e}")
    try:
        brain.reset()
        brain.initialize()
        print("Brain recovered successfully")
    except Exception as e2:
        print(f"Error recovering brain: {e2}")
```

### Development Guide

#### Plugin Development
```python
import pynlm
from abc import ABC, abstractmethod

class CustomPlugin(pynlm.NLMPlugin):
    def __init__(self):
        self.name = "Custom Plugin"
        self.version = "1.0.0"
    
    def initialize(self, config):
        self.config = config
        print(f"Initialized {self.name}")
    
    def process(self, data):
        # Custom processing logic
        return data
    
    def cleanup(self):
        print(f"Cleaned up {self.name}")

# Use plugin
plugin = CustomPlugin()
config = pynlm.createDefaultConfig()
plugin.initialize(config)
```

#### Extension Development
```python
import pynlm
from abc import ABC, abstractmethod

class CustomExtension(pynlm.NLMExtension):
    def get_name(self):
        return "Custom Extension"
    
    def get_version(self):
        return "1.0.0"
    
    def initialize(self, config):
        self.config = config
        print(f"Initialized {self.name}")
    
    def shutdown(self):
        print(f"Shut down {self.name}")
    
    def get_capabilities(self):
        return {
            'feature1': 'value1',
            'feature2': 'value2'
        }

# Use extension
extension = CustomExtension()
config = pynlm.createDefaultConfig()
extension.initialize(config)
```

### Migration Guide

#### From Version 0.8 to 0.9
```python
# Old API (version 0.8)
from nlm_old import *

brain = create_brain()
brain.initialize()
brain.step(10)

# New API (version 0.9)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Migration of configuration
old_config = nlm_old.get_default_config()
new_config = pynlm.createConfigFromDict(old_config.to_dict())

# Migration of brain methods
brain.step(10)  # Old: brain.step(10)
brain.step(0.001, 0.001)  # New: brain.step(0.001, 0.001)
```

## Configuration Reference

### Default Configuration Values

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| brain.neuron_count | int | 1000 | Total number of neurons |
| brain.region_count | int | 2 | Number of brain regions |
| brain.connection_probability | float | 0.1 | Connection probability |
| brain.v_thresh | float | -50.0 | Neuron threshold potential |
| brain.v_rest | float | -70.0 | Resting potential |
| brain.v_reset | float | -75.0 | Reset potential after spike |
| brain.tau_mem | float | 20.0 | Membrane time constant |
| brain.tau_ref | float | 2.0 | Refractory period |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| plasticity.stdp.enable | bool | true | Enable STDP |
| plasticity.stdp.learning_rate | float | 0.001 | STDP learning rate |
| plasticity.stdp.tau_plus | float | 20.0 | STDP time constant (potentiation) |
| plasticity.stdp.tau_minus | float | 20.0 | STDP time constant (depression) |
| plasticity.hebbian.enable | bool | true | Enable Hebbian learning |
| plasticity.hebbian.learning_rate | float | 0.01 | Hebbian learning rate |
| plasticity.structural.enable | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuromod.dopamine.scale | float | 1.0 | Dopamine modulation scale |
| neuromod.curiosity.enable | bool | true | Enable curiosity-driven exploration |
| neuromod.novelty.enable | bool | true | Enable novelty detection |
| neuromod.prediction_error.enable | bool | true | Enable prediction error |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| development.enable | bool | true | Enable development |
| development.synaptogenesis_rate | float | 0.001 | Synaptogenesis rate |
| development.pruning_rate | float | 0.0001 | Pruning rate |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| simulation.timestep | float | 0.001 | Simulation timestep |
| simulation.max_steps | int | 10000 | Maximum simulation steps |
| simulation.log_level | string | INFO | Log level |
| simulation.log_to_file | bool | false | Log to file |

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| world.width | int | 20 | World width |
| world.height | int | 20 | World height |
| world.visionWidth | int | 8 | Vision width |
| world.visionHeight | int | 8 | Vision height |
| world.maxEnergy | float | 100.0 | Maximum energy |

### Agent Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| agent.reward_modulation | bool | true | Enable reward modulation |
| agent.curiosity | bool | true | Enable curiosity |
| agent.development | bool | true | Enable development |

## Performance Tips

### Optimizing Performance

1. **Configure neuron count**: Adjust `brain.neuron_count` based on your performance requirements
2. **Use appropriate timestep**: Use smaller timesteps for accuracy, larger timesteps for speed
3. **Enable performance monitoring**: Monitor performance during simulation
4. **Use efficient data structures**: Use efficient data structures for better performance
5. **Profile code**: Profile code to identify bottlenecks

### Memory Management

1. **Monitor memory usage**: Monitor memory usage during simulation
2. **Force garbage collection**: Force garbage collection periodically
3. **Clean up references**: Clean up references to large objects
4. **Use efficient algorithms**: Use efficient algorithms for better performance

### Simulation Optimization

1. **Batch processing**: Process multiple simulation steps in batches
2. **Parallel processing**: Use parallel processing for multiple agents
3. **Caching**: Cache frequently used data
4. **Compression**: Compress data to reduce memory usage

## Troubleshooting

### Common Issues

**Issue: Import Error**
```bash
Error: ImportError: No module named 'pynlm'

Solution:
pip install pynlm
# Or: pip install -e .
```

**Issue: Configuration Error**
```python
Error: KeyError: 'brain.neuron_count'

Solution:
if config.has("brain.neuron_count"):
    print(f"Neurons: {config.get('brain.neuron_count')}")
else:
    print("Brain neuron count not found")
```

**Issue: Brain Initialization Error**
```python
Error: RuntimeError: Brain initialization failed

Solution:
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Error initializing brain: {e}")
```

### Debugging Tools

**Debug Configuration**
```python
import pynlm

config = pynlm.createDefaultConfig()
print("Configuration:")
print(config.summary())
```

**Debug Brain**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print("Brain before initialization:")
print(f"  Type: {type(brain)}")
print(f"  Initialized: {brain.is_initialized() if hasattr(brain, 'is_initialized') else 'N/A'}")

brain.initialize()
print("Brain after initialization:")
print(f"  Neuron count: {brain.getTotalNeuronCount()}")
print(f"  Synapse count: {brain.getTotalSynapseCount()}")
```

**Debug Simulation**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Starting simulation...")
for step in range(10):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Conclusion

The NLM Python bindings provide a comprehensive interface to the NLM neural simulation framework, offering:

- **Complete API coverage**: Access to all NLM functionality
- **Intuitive interface**: Natural and Pythonic API design
- **Performance optimization**: Optimized for performance
- **Error handling**: Comprehensive error handling and recovery
- **Documentation**: Complete documentation and examples
- **Testing**: Extensive testing and validation
- **Development tools**: Development tools and extensions
- **Migration support**: Migration guide for older versions

The Python bindings enable users to leverage the full power of NLM while maintaining ease of use and accessibility for both beginners and advanced users.

This documentation serves as a comprehensive guide for using NLM Python bindings, providing everything needed to get started with NLM development.
