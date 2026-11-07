# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings. The Python API allows you to create and interact with neural brains, simulate environments, and control artificial agents using simple, intuitive Python code.

## Quick Start

### Basic Usage

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")
```

## Core Classes

### Brain Class

The `Brain` class represents a neural simulation brain with spiking neurons.

#### Creation and Initialization

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain (always call before simulation)
brain.initialize()
```

#### Main Methods

| Method | Description |
|--------|-------------|
| `step(step_number)` | Run one simulation step |
| `step(step_number, time)` | Run step with timestamp |
| `reset()` | Reset brain state |
| `save(filepath)` | Save brain state to file |
| `load(filepath)` | Load brain state from file |
| `receiveSensoryInput(input)` | Inject sensory data |
| `produceAction()` | Get motor action |
| `addRegion(name)` | Add a new neural region |
| `getRegion(id)` | Get region by ID |

#### Statistics

```python
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Config Class

The `Config` class manages configuration for the NLM system.

#### Creation and Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.setInt("brain.neuron_count", 1000)
config.setFloat("brain.connection_probability", 0.1)
config.setString("brain.name", "MyBrain")
config.setBool("plasticity.stdp.enable", True)

# Get configuration values
neuron_count = config.getInt("brain.neuron_count", 500)
connection_prob = config.getFloat("brain.connection_probability", 0.05)

# Check if key exists
if config.has("brain.name"):
    print(f"Brain name: {config.getString('brain.name')}")

# Get all keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Convert to Python dictionary
config_dict = config.toDict()

# Load from Python dictionary
config_from_dict = pynlm.createDefaultConfig()
config_from_dict.loadFromDict({
    "brain.neuron_count": 2000,
    "plasticity.stdp.learning_rate": 0.001
})
```

#### File I/O

```python
# Save configuration to file
config.saveToFile("config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("config.json")
```

### AgentBrain Class

The `AgentBrain` class connects a neural brain to a world environment.

#### Creation and Initialization

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)
```

#### Main Methods

| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize agent with world |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Decode motor command from brain |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward-based learning |
| `updateDevelopment(timestep)` | Update development system |

#### Enabling Features

```python
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

#### Statistics

```python
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

### SimpleWorld Class

The `SimpleWorld` class represents a simple 2D world for NLM simulation.

#### Creation and Configuration

```python
import pynlm

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)
```

#### Main Methods

| Method | Description |
|--------|-------------|
| `update(timestep)` | Update world by timestep |
| `applyMotorCommand(cmd, current_time)` | Apply motor command to world |
| `getSensoryPercept()` | Get sensory percept from world |
| `getAgentBody()` | Get agent body state |
| `addObject(obj)` | Add world object |
| `removeObject(x, y)` | Remove object at position |

#### Sensory Access

```python
# Get what the agent sees
percept = world.getSensoryPercept()

# Access different sensory modalities
vision = percept.getVision()
audio = percept.getAudio()
internal = percept.getInternal()
touch = percept.getTouch()
proprioception = percept.getProprioception()

# Get world object
world_obj = pynlm.WorldObject(
    x=5.0, y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=10.0,
    radius=0.5
)
```

## Complete Example: Agent Simulation

```python
import pynlm
import time

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
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(1000)
```

## Advanced Features

### Neural Network Visualization

```python
# Create a simple visualization (if visualization module is available)
# Note: This requires compilation with visualization support
from nlm.visualization import create_visualizer

visualizer = create_visualizer()
visualizer.initialize(brain)

for i in range(100):
    brain.step(i)
    visualizer.update(i)
    visualizer.render()
```

### Checkpoint Management

```python
import os

# Save brain checkpoint
brain.save("checkpoint_1000.bin")

# Later, load the checkpoint
brain.load("checkpoint_1000.bin")

# Create checkpoint directory
checkpoint_dir = "checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

# Save with timestamp
timestamp = int(time.time())
brain.save(f"{checkpoint_dir}/checkpoint_{timestamp}.bin")
```

### Configuration Templates

```python
# Common configuration templates

def create_small_brain():
    """Create a small brain for quick testing."""
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 100)
    config.setFloat("brain.connection_probability", 0.2)
    return config

def create_large_brain():
    """Create a large brain for complex simulations."""
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 5000)
    config.setFloat("brain.connection_probability", 0.05)
    config.setFloat("plasticity.stdp.learning_rate", 0.0005)
    return config

def create_development_brain():
    """Create a brain with development enabled."""
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1000)
    config.setBool("development.enable", True)
    config.setFloat("development.critical_period_start", 0.3)
    config.setFloat("development.critical_period_end", 0.7)
    return config
```

## Error Handling

### Common Errors and Solutions

```python
import pynlm

# Error 1: Brain not initialized
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.step(0)  # This will crash if not initialized
except Exception as e:
    print(f"Error: {e}")
    print("Solution: Always call brain.initialize() before simulation")

# Error 2: Invalid configuration key
try:
    value = config.getString("nonexistent.key", "default")
    print(f"Value: {value}")
except Exception as e:
    print(f"Error: {e}")

# Error 3: File not found
try:
    config.loadFromFile("nonexistent.json")
except Exception as e:
    print(f"Error: {e}")
```

### Custom Exceptions

```python
try:
    # Your NLM code here
    brain.step(step)
except pynlm.RuntimeError as e:
    print(f"NLM runtime error: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except pynlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
```

## Performance Tips

### Optimizing Simulation Speed

```python
import pynlm

# Configuration for faster simulation
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 500)  # Smaller brain
config.setFloat("brain.connection_probability", 0.05)  # Fewer connections
config.setFloat("simulation.timestep", 0.01)  # Larger timesteps

brain = pynlm.createBrain(config)
brain.initialize()

# Process multiple steps at once (if supported)
for i in range(0, 1000, 10):
    # You could process 10 steps per call if the API supports it
    brain.step(i)
```

### Memory Management

```python
# For long-running simulations, periodically save checkpoints
import time

def simulate_with_checkpoints(steps_per_checkpoint=500):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(0, 10000):
        brain.step(step)
        
        # Save checkpoint periodically
        if step > 0 and step % steps_per_checkpoint == 0:
            timestamp = int(time.time())
            brain.save(f"checkpoint_{step}_{timestamp}.bin")
```

## Complete Reference Guide

### Class Summary

| Class | Location | Description |
|-------|----------|-------------|
| `Brain` | Main | Central neural simulation brain |
| `Config` | Core | Configuration management |
| `AgentBrain` | Agent | Interface between brain and world |
| `SimpleWorld` | World | 2D simulation world |
| `Vision` | Sensory | Visual sensory input |
| `Audio` | Sensory | Audio sensory input |
| `InternalSignals` | Sensory | Internal signal processing |
| `Action` | Motor | Motor action representation |
| `WorldObject` | World | World object representation |
| `AgentBody` | Agent | Agent body state |

### Enumerations

#### Neuron Types

| Enum | Value |
|------|-------|
| `NeuronType.Excitatory` | 0 |
| `NeuronType.Inhibitory` | 1 |
| `NeuronType.Modulatory` | 2 |
| `NeuronType.Sensory` | 3 |
| `NeuronType.Motor` | 4 |
| `NeuronType.Internal` | 5 |

#### Action Types

| Enum | Value |
|------|-------|
| `ActionType.MoveForward` | 0 |
| `ActionType.MoveBackward` | 1 |
| `ActionType.MoveLeft` | 2 |
| `ActionType.MoveRight` | 3 |
| `ActionType.TurnLeft` | 4 |
| `ActionType.TurnRight` | 5 |
| `ActionType.Look` | 6 |
| `ActionType.Interact` | 7 |
| `ActionType.Eat` | 8 |

#### Developmental Stages

| Enum | Value |
|------|-------|
| `DevelopmentalStage.Initial` | 0 |
| `DevelopmentalStage.CriticalPeriod` | 1 |
| `DevelopmentalStage.Maturation` | 2 |
| `DevelopmentalStage.Adult` | 3 |
| `DevelopmentalStage.Aging` | 4 |

#### World Object Types

| Enum | Value |
|------|-------|
| `WorldObjectType.Empty` | 0 |
| `WorldObjectType.Resource` | 1 |
| `WorldObjectType.Hazard` | 2 |
| `WorldObjectType.Wall` | 3 |
| `WorldObjectType.Marker` | 4 |

### Module Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `__version__` | str | Python module version |
| `__author__` | str | Module author |
| `__description__` | str | Module description |

### Quick Reference Functions

| Function | Description |
|----------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create neural brain |
| `createSimpleWorld()` | Create simple world |
| `createAgentBrain(brain)` | Create agent brain interface |

## Further Reading

For more detailed information, refer to:

- `HOW_TO_USE.md` - Build and installation instructions
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration audit

## License

MIT License. See LICENSE file for details.