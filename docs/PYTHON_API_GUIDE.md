# NLM Python API User Guide
# Comprehensive Documentation for the Enhanced Python Bindings

## Overview

This guide provides comprehensive documentation for the NLM Python bindings, covering everything from basic usage to advanced features. The enhanced Python bindings provide access to the full capabilities of the NLM brain simulation framework while maintaining a user-friendly Python interface.

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

## Core Concepts

### Brain Objects

The NLM brain system provides main Python classes:

1. **Brain** - Core neural simulation engine
2. **Config** - Configuration management
3. **AgentBrain** - Interface between brain and world
4. **SimpleWorld** - Simulated environment

## Brain Class

### Creation and Initialization

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Configure parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()
```

### Core Methods

| Method | Description |
|--------|-------------|
| `step(step_number)` | Run simulation step |
| `step(step_number, time)` | Run step with timestamp |
| `reset()` | Reset brain state |
| `save(filepath)` | Save brain state |
| `load(filepath)` | Load brain state |

### Statistics and Metrics

```python
# Basic statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Excitination/Inhibition ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Config Class

### Configuration Management

```python
import pynlm

# Create empty configuration
config = pynlm.createDefaultConfig()

# Set basic parameters
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Check if key exists
if config.has("brain.neuron_count"):
    neurons = config.getOr("brain.neuron_count", 1000)

# Get all configuration keys
keys = config.getKeys()

# Save configuration
config.saveToFile("config.json")
```

## AgentBrain Class

### Agent Configuration

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Initialize agent with world
agent.initialize(world)

# Enable advanced learning
agent.enableAdvancedLearning()
```

### Sensory Processing

```python
# Get current sensory perception
percept = world.getSensoryPercept()

# Process sensory input
agent.processSensoryInput(percept)

# Access sensory data
vision = percept.getVision()
touch = percept.getTouch()
```

### Action Selection

```python
# Get motor command from brain
motor_command = agent.decodeMotorCommand()
print(f"Selected action: {motor_command}")

# Apply action to world
world.applyMotorCommand(motor_command, world.getSimulationTime())
```

## SimpleWorld Class

### Environment Setup

```python
import pynlm

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=30,
    height=30,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(15.0, 15.0)

# Reset world to initial state
world.reset()
```

## Complete Example

```python
import pynlm

def runCompleteSimulation():
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
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain

# Run the simulation
brain = runCompleteSimulation()
```

## Advanced Features

### Performance and Profiling

```python
import pynlm
import time

# Create simulation with profiling
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Profile performance
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Steps per second: {1000 / (end_time - start_time):.2f}")
```

### Checkpointing and Persistence

```python
import pynlm

# Create simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(500):
    brain.step(step)

# Save checkpoint
brain.save("checkpoint.bin")
print("Checkpoint saved")

# Create new brain and load checkpoint
new_brain = pynlm.createBrain(config)
new_brain.initialize()
if new_brain.load("checkpoint.bin"):
    print("Checkpoint loaded successfully")
```

### Configuration Management

```python
import pynlm

# Create expert configuration
def createExpertConfig():
    config = pynlm.createDefaultConfig()
    
    # High-performance settings
    config.set("neuron_count", 5000)
    config.set("simulation_timestep", 0.0001)
    config.set("connection_probability", 0.05)
    
    # Advanced plasticity
    config.set("plasticity_learning_rate", 0.005)
    config.set("stdp_ltp_weight", 0.015)
    config.set("stdp_ltd_weight", 0.01)
    
    return config

# Save configuration
expert_config = createExpertConfig()
expert_config.saveToFile("expert_config.json")
```

## API Reference

### Classes

- **Brain**: Core neural simulation engine
- **Config**: Configuration management system
- **AgentBrain**: Interface between brain and world
- **SimpleWorld**: Simulated environment

### Key Methods

- **createDefaultConfig()**: Create default configuration
- **createBrain(config)**: Create brain with configuration
- **createSimpleWorld()**: Create simple world
- **createAgentBrain(brain)**: Create agent brain interface

## Support and Community

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Check the latest documentation
- **Community**: Questions and discussions

## License

This documentation is part of the NLM project. For licensing information, see the main NLM documentation.

---

*This documentation is continuously updated. For the latest information, visit the NLM documentation website.*