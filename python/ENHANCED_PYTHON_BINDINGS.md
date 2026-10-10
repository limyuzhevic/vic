# Enhanced NLM Python Bindings - Documentation

Comprehensive documentation for the enhanced Python bindings of NLM (Neural Learning Machine).

## Overview

This documentation covers the enhanced Python bindings for NLM, which provides a comprehensive interface for working with neural simulation, agent brains, and world interactions.

## Quick Start

### Installation

The enhanced Python bindings are automatically installed with the NLM framework. You can import them directly:

```python
import pynlm
print("NLM Python version:", pynlm.__version__)
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Spike count: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

## Core Components

### 1. Configuration System

The configuration system allows you to customize neural simulation parameters.

#### Creating and Modifying Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set basic parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Get configuration values
neuron_count = config.get("neuron_count")
connection_prob = config.get("connection_probability")

# Load configuration from file
config.loadFromFile("config.json")

# Save configuration to file
config.saveToFile("my_config.json")
```

#### Configuration Keys

| Key | Type | Description | Default |
|-----|------|-------------|---------|
| neuron_count | int | Number of neurons | 1000 |
| region_count | int | Number of brain regions | 2 |
| connection_probability | float | Probability of synaptic connections | 0.1 |
| simulation_timestep | float | Simulation timestep in seconds | 0.001 |
| random_seed | int | Random seed for reproducibility | 42 |

### 2. Brain Class

The `Brain` class represents the central neural simulation engine.

#### Basic Operations

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.step(0)
brain.step(1)

# Get brain statistics
stats = brain.getStatistics()
print(f"Total neurons: {stats['neuron_count']}")
print(f"Total synapses: {stats['synapse_count']}")
print(f"Active neurons: {stats['active_neurons']}")
```

#### Brain Methods

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain with configuration |
| `step(currentStep)` | Perform a simulation step |
| `step(currentStep, currentTime)` | Perform step with timestamp |
| `receiveSensoryInput(input)` | Inject sensory input |
| `produceAction()` | Generate motor action |
| `reset()` | Reset brain state |
| `save(filepath)` | Save brain state to file |
| `load(filepath)` | Load brain state from file |

### 3. World System

The `SimpleWorld` class represents the simulation environment.

#### World Configuration

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
world.setAgentStart(50, 50)  # Agent starting position

# Get world information
width = world.getWidth()
height = world.getHeight()
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
```

#### World Operations

```python
# Update world
world.update(0.1)  # timestep

# Get sensory percept
percept = world.getSensoryPercept()

# Apply motor command
action = pynlm.Action(pynlm.ActionType.MoveForward)
world.applyMotorCommand(action.getType(), 0.0)

# Add world objects
world_obj = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 5.0)
world.addObject(world_obj)
```

### 4. Agent Brain Interface

The `AgentBrain` class bridges the NLM brain with the world environment.

#### Agent Brain Operations

```python
import pynlm

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)

# Initialize with world
agent_brain.initialize(world)

# Process sensory input and produce action
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Decode motor command
action = agent_brain.decodeMotorCommand()

# Apply reward modulation
agent_brain.applyRewardModulation(1.0, 0.5)  # reward, predicted_reward

# Get neuromodulation levels
curiosity = agent_brain.getCuriosityLevel()
novelty = agent_brain.getNoveltyLevel()
```

### 5. Sensory Inputs

#### Vision

```python
import pynlm
import numpy as np

# Create vision input
vision = pynlm.Vision(10, 10, 3)  # width, height, channels

# Set vision data (flattened)
data = np.random.rand(300).astype(np.float32)  # 10*10*3
vision.setData(data)

# Get 3D array representation
array_data = vision.getDataAsArray()  # Returns list of lists of lists
```

#### Audio

```python
import pynlm

# Create audio input
audio = pynlm.Audio(44100, 1000)  # sample_rate, num_samples
data = [0.0] * 1000
audio.setData(data)
```

### 6. Actions

#### Action Creation and Modification

```python
import pynlm

# Create actions with different types
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Look)
action3 = pynlm.Action(pynlm.ActionType.Eat, [0.5, 0.5])

# Modify action properties
action2.setType(pynlm.ActionType.TurnLeft)
action2.setParameters([1.0])  # turn angle

# Get action information
print(f"Action type: {action2.getType()}")
print(f"Action name: {action2.getName()}")
print(f"Action parameters: {action2.getParameters()}")

# Clone action
cloned_action = action1.clone()
```

### 7. Advanced Features

#### Brain Simulator

The `BrainSimulator` class provides a complete simulation environment:

```python
import pynlm

# Create simulator
simulator = pynlm.BrainSimulator()
simulator.initialize(neuronCount=500, worldWidth=100, worldHeight=100)

# Run simulation
for i in range(100):
    energy = simulator.step()
    if i % 10 == 0:
        stats = simulator.getStatistics()
        print(f"Step {i}: Energy={stats['agent_energy']:.2f}, "
              f"Curiosity={stats['curiosity_level']:.2f}")

# Reset simulator
simulator.reset()
```

#### Configuration Manager

The `ConfigManager` provides advanced configuration management:

```python
import pynlm

# Create config manager
config_manager = pynlm.ConfigManager()
config_manager.set("neuron_count", 1000)
config_manager.set("random_seed", 123)

# Get values with type conversion
neuron_count = config_manager.get("neuron_count")
random_seed = config_manager.get("random_seed")

# Load and save configuration
config_manager.loadFromFile("config.json")
config_manager.save("saved_config.json")
```

### 8. Error Handling

The enhanced bindings provide specific exception classes:

```python
import pynlm

# Catch specific exceptions
try:
    config = pynlm.createDefaultConfig()
    config.set("key", "value")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.WorldError as e:
    print(f"World error: {e}")
```

### 9. Type Conversion Helpers

Helper functions for working with enumerations:

```python
import pynlm

# Get string names for types
neuron_type_name = pynlm.get_neuron_type_name(pynlm.NeuronType.Excitatory)
action_type_name = pynlm.get_action_type_name(pynlm.ActionType.MoveForward)

print(f"Neuron type: {neuron_type_name}")
print(f"Action type: {action_type_name}")
```

## Example Applications

### Example 1: Simple Learning Agent

```python
import pynlm
import random

# Create environment
world = pynlm.create_simple_world()
world.configure(200, 200, 20, 20)
world.setAgentStart(100, 100)

# Create brain
brain = pynlm.create_brain_with_size(500)
agent_brain = pynlm.create_agent_brain(brain)
agent_brain.initialize(world)

# Enable neuromodulation for learning
agent_brain.enable_reward_modulation(True)
agent_brain.enable_curiosity(True)

# Run learning episode
for step in range(500):
    # Get current state
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    # Produce action
    action = agent_brain.decodeMotor_command()
    world.apply_motor_command(action.get_type(), float(step) * 0.1)
    
    # Update world
    world.update(0.1)
    
    # Simulate reward based on position
    agent_body = world.get_agent_body()
    if agent_body.x > 150 and agent_body.y > 150:
        agent_brain.apply_reward_modulation(1.0, 0.5)
    elif agent_body.x < 50 or agent_body.y < 50:
        agent_brain.apply_reward_modulation(-0.5, -0.2)

# Get final statistics
stats = agent_brain.get_episode_statistics(world)
print(f"Final energy: {stats['agent_energy']:.2f}")
print(f"Final curiosity: {stats['curiosity_level']:.2f}")
print(f"Total steps: {step + 1}")
```

### Example 2: Configuration and Model Persistence

```python
import pynlm
import json
import tempfile
import os

# Create sophisticated configuration
config = pynlm.create_config_with_defaults()
config.set("neuron_count", 2000)
config.set("region_count", 4)
config.set("connection_probability", 0.05)
config.set("simulation_timestep", 0.0005)
config.set("random_seed", 12345)

# Save configuration
with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
    config.saveToFile(f.name)
    print(f"Configuration saved to {f.name}")

# Create and run brain with this configuration
brain = pynlm.create_brain(config)
brain.initialize()

# Run some simulation
for i in range(100):
    brain.step(i)

# Save brain state
with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
    brain.save(f.name)
    print(f"Brain saved to {f.name}")

# Load brain state later
brain2 = pynlm.create_brain(pynlm.create_default_config())
brain2.load(f.name)

# Clean up
os.unlink(f.name)
```

### Example 3: Advanced Simulation with Statistics

```python
import pynlm
import time

# Create advanced simulator
simulator = pynlm.BrainSimulator()
simulator.initialize(
    neuronCount=1000,
    worldWidth=300,
    worldHeight=300,
    visionWidth=30,
    visionHeight=30
)

# Add interesting world objects
world = simulator.get_world()

# Add walls
for x in range(0, 300, 20):
    wall = pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall)
    world.addObject(wall)
    wall2 = pynlm.WorldObject(x, 300, pynlm.WorldObjectType.Wall)
    world.addObject(wall2)

for y in range(0, 300, 20):
    wall = pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall)
    world.addObject(wall)
    wall2 = pynlm.WorldObject(300, y, pynlm.WorldObjectType.Wall)
    world.addObject(wall2)

# Add resources and hazards
for i in range(20):
    x = random.uniform(50, 250)
    y = random.uniform(50, 250)
    if random.random() < 0.3:
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, random.uniform(5, 15))
        world.addObject(resource)
    else:
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, random.uniform(-5, -1))
        world.addObject(hazard)

# Run advanced simulation
start_time = time.time()
for step in range(1000):
    # Record statistics periodically
    if step % 50 == 0:
        stats = simulator.get_statistics()
        print(f"Step {step}: Energy={stats['agent_energy']:.2f}, "
              f"Curiosity={stats['curiosity_level']:.2f}, "
              f"Total spikes={stats['total_spikes']}")
    
    energy = simulator.step()
    
    # Check for termination conditions
    if stats['agent_energy'] <= 0 or step >= 999:
        print("Simulation terminated")
        break

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

## API Reference

### Core Classes

#### pynlm.Config
- **Methods**: `has(key)`, `get(key)`, `getOr(key, default)`, `set(key, value, source)`, `remove(key)`, `getKeys()`, `summary()`, `loadFromFile()`, `saveToFile()`, `loadFromArgs()`
- **Properties**: No direct properties

#### pynlm.Brain
- **Methods**: `initialize()`, `step(currentStep)`, `step(currentStep, currentTime)`, `receiveSensoryInput()`, `produceAction()`, `reset()`, `save()`, `load()`, `addRegion()`, `getRegion()`, `getRegionCount()`, `getRegionIds()`, `getRegions()`, `getTotalNeuronCount()`, `getTotalSynapseCount()`, `getActiveNeuronCount()`, `getFiringNeuronCount()`, `getAverageFiringRate()`, `getExcitationInhibitionRatio()`, `getTotalSpikeCount()`, `getDevelopmentalStage()`, `setDevelopmentalStage()`, `getConfig()`, `logStatus()`, `runSimulation()`, `getStatistics()`, `getRegionNames()`, `getNeuralActivity()`
- **Properties**: No direct properties

#### pynlm.AgentBrain
- **Methods**: `initialize(world)`, `getSensoryInputSize()`, `getMotorOutputSize()`, `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()`, `updateDevelopment()`, `getDevelopmentalStage()`, `getNeuromodulationLevel()`, `getCuriosityLevel()`, `getNoveltyLevel()`, `getPredictionError()`, `reset()`, `getBrain()`, `enableRewardModulation()`, `enableStructuralPlasticity()`, `enableDevelopment()`, `enableCuriosity()`, `isRewardModulationEnabled()`, `isStructuralPlasticityEnabled()`, `isDevelopmentEnabled()`, `isCuriosityEnabled()`, `runEpisode()`, `getEpisodeStatistics()`
- **Properties**: No direct properties

#### pynlm.SimpleWorld
- **Methods**: `configure()`, `reset()`, `setAgentStart()`, `update()`, `applyMotorCommand()`, `getSensoryPercept()`, `getAgentBody()`, `addObject()`, `removeObject()`, `isValidPosition()`, `getWidth()`, `getHeight()`, `getMaxEnergy()`, `setMaxEnergy()`, `getEnergyDecayRate()`, `setEnergyDecayRate()`, `getSimulationTime()`, `setRandomSeed()`, `getRandomSeed()`, `getObjectsCount()`, `getNearbyObjects()`
- **Properties**: No direct properties

#### pynlm.Vision
- **Methods**: `setData()`, `getDataAsArray()`, `getWidth()`, `getHeight()`, `getChannels()`
- **Properties**: No direct properties

#### pynlm.Action
- **Methods**: `getType()`, `setType()`, `getParameters()`, `setParameters()`, `getName()`, `getParameter(index)`, `addParameter()`, `clone()`
- **Properties**: No direct properties

#### pynlm.BrainSimulator
- **Methods**: `initialize()`, `step()`, `getStepCount()`, `getStatistics()`, `reset()`
- **Properties**: `brain`, `world`, `agent_brain`

#### pynlm.ConfigManager
- **Methods**: `set()`, `get()`, `has()`, `save()`, `getSummary()`, `getKeys()`, `loadFromFile()`
- **Properties**: No direct properties

### Factory Functions

#### pynlm.createDefaultConfig()
Creates a default configuration object.

#### pynlm.createBrain(config)
Creates a brain with the specified configuration.

#### pynlm.createSimpleWorld()
Creates a simple world object.

#### pynlm.createAgentBrain(brain)
Creates an agent brain interface.

### Utility Functions

#### pynlm.get_neuron_type_name(type)
Returns string name for neuron type.

#### pynlm.get_synapse_type_name(type)
Returns string name for synapse type.

#### pynlm.get_action_type_name(type)
Returns string name for action type.

#### pynlm.run_quick_test()
Runs a quick test simulation.

## Installation and Setup

### System Requirements

- Python 3.8 or higher
- NLM framework with Python bindings

### Building from Source

```bash
# Build the NLM framework
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install python/
```

### Testing

Run the comprehensive test suite:

```bash
python3 python/test_pynlm_comprehensive.py
```

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure NLM Python bindings are installed
   - Check if PYTHONPATH includes the NLM build directory

2. **RuntimeError: Could not load Python bindings**
   - Ensure the pynlm module is built correctly
   - Check for missing dependencies

3. **TypeError: invalid argument types**
   - Check function signatures and argument types
   - Ensure correct parameter order

### Getting Help

- **Documentation**: See `docs/` directory for detailed documentation
- **Examples**: Look in `examples/` directory for complete applications
- **Issues**: Report issues on the NLM GitHub repository
- **Community**: Join the NLM discussion forum

## Performance Tips

1. **Use efficient data structures**: For large simulations, use numpy arrays for sensor data
2. **Batch operations**: Use batch operations when possible
3. **Memory management**: Monitor memory usage in long-running simulations
4. **Parallel processing**: Consider parallel processing for multiple simulations
5. **Configuration optimization**: Optimize configuration settings for your specific use case

## Future Enhancements

The enhanced Python bindings provide a foundation for future improvements:

1. **GPU acceleration**: Integration with GPU for faster simulation
2. **Distributed computing**: Support for multi-GPU and distributed simulations
3. **Advanced visualization**: Enhanced plotting and visualization capabilities
4. **Machine learning integration**: Integration with modern ML frameworks
5. **Cloud computing**: Support for cloud-based simulations
6. **Mobile deployment**: Support for mobile and embedded systems

## License

This documentation is provided under the MIT License.

For more information about NLM, visit: https://github.com/nlm-project/nlm
