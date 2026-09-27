# NLM Python API Reference

This document provides comprehensive documentation for the NLM Python API, which wraps the C++ neural network simulation framework.

## Overview

The NLM Python API provides programmatic access to the Neural Learning Machine (NLM) framework for creating virtual brains, simulating neural networks, and running experiments. The API is designed to be intuitive for beginners while providing powerful features for advanced users.

## Module Structure

The main module is `pynlm`, which can be imported as:

```python
import pynlm
from pynlm import Config, Brain, SimpleWorld, AgentBrain
```

## Core Classes

### Config

Configuration objects for neural network parameters.

#### Creating Config Objects

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create empty configuration
config = pynlm.Config()
```

#### Configuration Methods

| Method | Description | Example |
|--------|-------------|---------|
| `set(key, value)` | Set configuration parameter | `config.set("brain.neuron_count", 1000)` |
| `get(key)` | Get configuration parameter | `config.get("brain.neuron_count")` |
| `has(key)` | Check if parameter exists | `config.has("brain.neuron_count")` |
| `saveToFile(filepath)` | Save to JSON file | `config.saveToFile("config.json")` |
| `loadFromFile(filepath)` | Load from JSON file | `config.loadFromFile("config.json")` |
| `getKeys()` | Get all parameter keys | `config.getKeys()` |

#### Available Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Brain

Main neural network simulation engine.

#### Creating and Initializing Brains

```python
# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Methods

| Method | Description | Example |
|--------|-------------|---------|
| `initialize()` | Initialize brain for simulation | `brain.initialize()` |
| `step(step_number)` | Run one simulation step | `brain.step(42)` |
| `step(step_number, time)` | Run step with timestamp | `brain.step(42, 0.1)` |
| `reset()` | Reset brain to initial state | `brain.reset()` |

#### Input/Output Methods

| Method | Description | Example |
|--------|-------------|---------|
| `receiveSensoryInput(input)` | Inject sensory data | `brain.receiveSensoryInput(sensory_input)` |
| `produceAction()` | Generate motor action | `action = brain.produceAction()` |

#### Statistics and State Methods

| Method | Description | Example |
|--------|-------------|---------|
| `getTotalNeuronCount()` | Total neuron count | `count = brain.getTotalNeuronCount()` |
| `getTotalSynapseCount()` | Total synapse count | `count = brain.getTotalSynapseCount()` |
| `getFiringNeuronCount()` | Currently firing neurons | `count = brain.getFiringNeuronCount()` |
| `getAverageFiringRate()` | Average firing rate | `rate = brain.getAverageFiringRate()` |
| `getExcitationInhibitionRatio()` | E/I balance | `ratio = brain.getExcitationInhibitionRatio()` |
| `getTotalSpikeCount()` | Total spikes generated | `count = brain.getTotalSpikeCount()` |
| `getPendingSpikeEventCount()` | Pending spike events | `count = brain.getPendingSpikeEventCount()` |
| `getActiveNeuronCount()` | Active neurons | `count = brain.getActiveNeuronCount()` |
| `getDevelopmentStage()` | Current developmental stage | `stage = brain.getDevelopmentStage()` |

#### State Persistence

```python
# Save brain state
brain.save("checkpoint_1.bin")

# Load brain state  
brain.load("checkpoint_2.bin")
```

#### Region Management

```python
# Add neural region
region_id = brain.addRegion("cortex")

# Get region
region = brain.getRegion(region_id)

# Get all region IDs
region_ids = brain.getRegionIds()
```

### SimpleWorld

Simple 2D environment for agent simulation.

#### Creating and Configuring Worlds

```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Reset world to initial state
world.reset()
```

#### World Control Methods

| Method | Description | Example |
|--------|-------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Set world dimensions | `world.configure(20, 20, 8, 8)` |
| `reset()` | Reset to initial state | `world.reset()` |
| `update(timestep)` | Advance simulation | `world.update(0.1)` |
| `setRandomSeed(seed)` | Set random seed | `world.setRandomSeed(42)` |

#### Agent Interaction Methods

| Method | Description | Example |
|--------|-------------|---------|
| `setAgentStart(x, y)` | Set agent start position | `world.setAgentStart(5.0, 5.0)` |
| `applyMotorCommand(action, time)` | Apply motor action | `world.applyMotorCommand(action, time)` |
| `getSensoryPercept()` | Get sensory perception | `percept = world.getSensoryPercept()` |
| `getAgentBody()` | Get agent body state | `body = world.getAgentBody()` |

#### Object Management

```python
# Add world object (helper function)
from pynlm import WorldObject, WorldObjectType

obj = WorldObject(x=5.0, y=5.0, obj_type=WorldObjectType.Resource, value=1.0)
world.addObject(obj)

# Remove object
world.removeObject(x, y)

# Check if position is valid
world.isValidPosition(x, y)
```

#### World Properties

```python
# Get world properties
width = world.getWidth()
height = world.getHeight()
time = world.getSimulationTime()
```

### AgentBrain

Bridge between neural brain and world environment.

#### Creating and Initializing AgentBrains

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

#### Sensory Processing

| Method | Description | Example |
|--------|-------------|---------|
| `processSensoryInput(percept)` | Process sensory data | `agent.processSensoryInput(percept)` |

#### Motor Decoding

```python
# Decode brain activity to motor command
action = agent.decodeMotorCommand()
```

#### Neuromodulation

| Method | Description | Example |
|--------|-------------|---------|
| `applyRewardModulation(reward, predicted_reward)` | Apply reward modulation | `agent.applyRewardModulation(reward, predicted)` |
| `getCuriosityLevel()` | Get curiosity level | `curiosity = agent.getCuriosityLevel()` |
| `getNoveltyLevel()` | Get novelty level | `novelty = agent.getNoveltyLevel()` |
| `getPredictionError()` | Get prediction error | `error = agent.getPredictionError()` |
| `getNeuromodulationLevel()` | Get neuromodulation level | `level = agent.getNeuromodulationLevel()` |

#### Development

| Method | Description | Example |
|--------|-------------|---------|
| `updateDevelopment(timestep)` | Update development system | `agent.updateDevelopment(0.1)` |
| `getDevelopmentalStage()` | Get developmental stage | `stage = agent.getDevelopmentalStage()` |

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

## Enumerations

### NeuronType

```python
from pynlm import NeuronType

# Neuron types
NeuronType.Excitatory  # Excitatory neurons
NeuronType.Inhibitory  # Inhibitory neurons
NeuronType.Sensory     # Sensory neurons
NeuronType.Motor       # Motor neurons
NeuronType.Modulatory  # Modulatory neurons
NeuronType.Internal    # Internal state neurons
```

### DevelopmentalStage

```python
from pynlm import DevelopmentalStage

# Developmental stages
DevelopmentalStage.Initial       # Initial development
DevelopmentalStage.CriticalPeriod  # Critical learning period
DevelopmentalStage.Maturation    # Maturation phase
DevelopmentalStage.Adult        # Adult stage
DevelopmentalStage.Aging        # Aging phase
```

### ActionType

```python
from pynlm import ActionType

# Motor actions
ActionType.MoveForward    # Move forward
ActionType.MoveBackward   # Move backward
ActionType.TurnLeft       # Turn left
ActionType.TurnRight      # Turn right
ActionType.LookLeft       # Pan sensor left
ActionType.LookRight      # Pan sensor right
ActionType.Interact      # Interact with object
ActionType.Wait          # No action
ActionType.Eat           # Eat/consume
ActionType.Drink         # Drink
ActionType.Sleep         # Rest
```

### WorldObjectType

```python
from pynlm import WorldObjectType

# World object types
WorldObjectType.Empty     # Empty space
WorldObjectType.Resource  # Resource object
WorldObjectType.Hazard    # Hazard object
WorldObjectType.Wall      # Wall/boundary
WorldObjectType.Marker    # Navigation aid
```

## Helper Functions

### run_simulation

```python
# Run complete agent simulation
def run_simulation(brain, world, agent, num_steps: int) -> None:
    """Run a complete agent simulation with brain and world."""
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Simulation complete!")
```

### log

```python
# Logging helper
pynlm.log("Simulation started", "INFO")
pynlm.log("Error occurred", "ERROR")
```

## Common Usage Patterns

### Basic Neural Network Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Environment

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Configuration Management

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -55.0)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

print(f"Neuron count: {config2.get('brain.neuron_count')}")
```

## Error Handling

### Common Exceptions

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Failed to create brain: {e}")

try:
    world.update(0.1)
except RuntimeError as e:
    print(f"World simulation error: {e}")
```

## Performance Tips

1. **Reuse Objects**: Reuse brain, world, and agent objects across simulations
2. **Batch Processing**: Use `reset()` instead of creating new objects
3. **Configuration Caching**: Load configurations from file rather than creating defaults
4. **Memory Management**: Monitor memory usage during long simulations

## Version Information

```python
import pynlm

print(f"NLM Python API version: {pynlm.__version__}")
```

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Science Documentation](docs/SCIENCE.md)
- [NLM Experiment Documentation](docs/EXPERIMENTS.md)
- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage instructions
- [easy_usage.md](easy_usage.md) - Quick start guide for beginners