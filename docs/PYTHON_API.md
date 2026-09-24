# Python API Documentation for NLM (Neural Learning Machine)

## Overview

This document provides comprehensive documentation for the NLM Python API (pynlm), which provides Python bindings for the C++ Neural Learning Machine neural simulation framework.

## Quick Start

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

## Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pip (Pre-built wheels may be available)

```bash
pip install pynlm
```

## Basic Usage

### 1. Creating a Brain

#### Method 1: Using default configuration

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

#### Method 2: Using custom configuration

```python
import pynlm

# Create and configure custom brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Running a Simulation

#### Basic simulation loop

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Using convenience methods

```python
# Run for 1000 steps in one line
brain.run_for_steps(1000)

# Reset and run for 500 steps
brain.reset_and_run(500)

# Get comprehensive brain state as dictionary
brain_state = brain.get_brain_state()
print(f"Total neurons: {brain_state['total_neurons']}")
print(f"Firing neurons: {brain_state['firing_neurons']}")
```

### 3. Creating a World and Agent

#### Basic agent simulation

```python
import pynlm

# Create brain and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world and configure
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()

# Create agent and connect to brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent simulation
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 4. Using Convenience Methods

#### Agent episode and training

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run a complete agent episode
agent.run_agent_episode(world, 200)

# Train agent for multiple episodes
agent.train_agent(world, 10)
```

## Complete Agent Example

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
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = (world.getSensoryPercept().getInternal()[0] 
                 if world.getSensoryPercept().getInternal() 
                 else 0.0)
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
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'steps_completed': num_steps
    }

# Run the simulation
simulation_results = run_agent_simulation(1000)
print("Agent simulation complete!")
```

## Configuration System

### 1. Creating and Modifying Configurations

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration values
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Get all configuration keys
keys = config.getKeys()

# Modify configuration
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")
```

### 2. Configuration Presets

```python
# Create config from preset using convenience method
config = pynlm.createDefaultConfig()

# Use built-in presets
from_preset = pynlm.Config.from_preset  # Method will be added

# High plasticity configuration
config = pynlm.Config.from_preset("high_plasticity")

# Agent training configuration  
config = pynlm.Config.from_preset("agent_training")

# Simple exploration configuration
config = pynlm.Config.from_preset("exploration")
```

## World System

### 1. World Creation and Configuration

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Reset world to initial state
world.reset()

# Get world properties
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
```

### 2. World Object Management

```python
# Add resources to world
world.addObject(pynlm.WorldObject(
    x=5.0, y=5.0, 
    type=pynlm.WorldObjectType.Resource, 
    value=1.0
))

# Add hazards
world.addObject(pynlm.WorldObject(
    x=15.0, y=15.0,
    type=pynlm.WorldObjectType.Hazard,
    value=0.5
))

# Check valid positions
if world.isValidPosition(10.0, 10.0):
    print("Position is valid")

# Remove objects
world.removeObject(5.0, 5.0)
```

### 3. Agent Convenience Methods

```python
# Using world convenience methods
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

# Place agent at center
world.place_agent(7.5, 7.5)

# Add resources conveniently
world.add_resource(pynlm.WorldObjectType.Resource, 1.0)

# Update world simulation
world.update(timestep=0.1)

# Get sensory percept
percept = world.getSensoryPercept()
print(f"Vision data: {percept.getVision()}")
```

## Advanced Usage Patterns

### 1. Using Context Managers

```python
import pynlm

# Context manager for brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
with brain:
    brain.initialize()
    brain.run_for_steps(100)  # Context will reset brain
    print("Brain simulation completed")

# Context manager for agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

with agent as agent_brain:
    agent_brain.initialize(world)
    agent_brain.run_agent_episode(world, 100)
    print("Agent episode completed")
```

### 2. Simulation Statistics Collection

```python
import pynlm
import time

def run_and_collect_stats(brain, world, agent, steps):
    """Run simulation and collect comprehensive statistics."""
    stats = {
        'start_time': time.time(),
        'initial_state': brain.get_brain_state(),
        'steps': []
    }
    
    for step in range(steps):
        # Simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect stats every 10 steps
        if step % 10 == 0:
            stats['steps'].append({
                'step': step,
                'time': world.getSimulationTime(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'developmental_stage': agent.getDevelopmentalStage()
            })
    
    stats['end_time'] = time.time()
    stats['duration'] = stats['end_time'] - stats['start_time']
    stats['final_state'] = brain.get_brain_state()
    
    return stats

# Run simulation with stats collection
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

simulation_stats = run_and_collect_stats(brain, world, agent, 500)
print(f"Simulation completed in {simulation_stats['duration']:.2f} seconds")
```

## Error Handling and Validation

### 1. Basic Error Handling

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Always initialize before use
try:
    brain.initialize()
except Exception as e:
    print(f"Failed to initialize brain: {e}")
    # Create brain again
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()

# Safe simulation steps
for step in range(1000):
    try:
        brain.step(step)
    except Exception as e:
        print(f"Error at step {step}: {e}")
        break
```

### 2. Input Validation

```python
import pynlm

# Create config and validate
config = pynlm.createDefaultConfig()

# Validate configuration values
if config.get("brain.neuron_count", 0) < 100:
    print("Warning: Small brain size may affect simulation quality")

# Validate world configuration
world = pynlm.createSimpleWorld()
world.configure(width=1, height=1, visionWidth=100, visionHeight=100)
# This will create a very wide field of view

# Validate agent configuration
agent = pynlm.createAgentBrain(brain)
try:
    world.update(0.1)  # This should work
except Exception as e:
    print(f"World update error: {e}")
```

## Performance Monitoring

### 1. Basic Performance Metrics

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Performance monitoring
start_time = time.time()
steps_completed = 0

for step in range(1000):
    step_start = time.time()
    brain.step(step)
    steps_completed += 1
    
    # Log performance every 100 steps
    if step % 100 == 0:
        elapsed = time.time() - start_time
        steps_per_second = steps_completed / elapsed
        print(f"Performance: {steps_per_second:.1f} steps/sec")
        print(f"Brain state: {brain.get_brain_state()}")
```

### 2. Memory Usage Monitoring

```python
import pynlm
import psutil
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Initial memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")

# Run simulation
brain.run_for_steps(1000)

# Check final memory
final_memory_info = process.memory_info()
print(f"Final memory usage: {final_memory_info.rss / 1024 / 1024:.2f} MB")
print(f"Memory increase: {(final_memory_info.rss - memory_info.rss) / 1024 / 1024:.2f} MB")
```

## Serialization and Persistence

### 1. Saving and Loading Brain State

```python
import pynlm

# Create and run a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.run_for_steps(500)

# Save brain state to file
brain.save("my_brain_checkpoint.bin")
print("Brain state saved to my_brain_checkpoint.bin")

# Later, load the saved state
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("my_brain_checkpoint.bin")
print("Brain state loaded from file")
```

### 2. Configuration Serialization

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Convert to dictionary (convenience method)
config_dict = config.to_dict()
print(f"Configuration as dictionary: {config_dict}")

# Set configuration from dictionary
new_config = pynlm.createDefaultConfig()
new_config.set_from_dict(config_dict)

# Save configuration to file
config.saveToFile("brain_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("brain_config.json")
```

## Working with Different Neuron and Synapse Types

### 1. Exploring Neural Populations

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get different neuron types
# Note: This assumes the API provides access to neuron populations
# You may need to implement custom methods based on actual API

brain_state = brain.get_brain_state()
print(f"Total neurons: {brain_state['total_neurons']}")
print(f"Total synapses: {brain_state['total_synapses']}")

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Get plasticity status
stats = brain.get_brain_state()
print(f"Excitatory/Inhibitory ratio: {stats['excitatory_inhibitory_ratio']:.2f}")
```

## Debugging and Troubleshooting

### 1. Common Issues and Solutions

```python
import pynlm

# Issue 1: Brain not firing neurons
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check if neurons are getting input
brain_state = brain.get_brain_state()
if brain_state['firing_neurons'] == 0:
    print("No neurons firing - checking configuration")
    # Increase input or adjust parameters
    brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 50.0f)

# Issue 2: Agent not moving
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()
agent.initialize(world)

action = agent.decodeMotorCommand()
if action.getType() == pynlm.MotorCommand::Wait:
    print("Agent not moving - checking motor neuron activity")
    # Check brain state for motor neuron activity

# Issue 3: Memory usage growing
import psutil, os
process = psutil.Process(os.getpid())

# Monitor memory usage
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        memory_mb = process.memory_info().rss / 1024 / 1024
        print(f"Step {step}, Memory: {memory_mb:.2f} MB")
        
        if memory_mb > 500:  # If memory > 500 MB
            print("High memory usage detected - consider saving checkpoint")
            brain.save("checkpoint_memory.bin")
```

## Best Practices

### 1. Memory Management

```python
import pynlm

# Use context managers for automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    brain.run_for_steps(100)

# Explicit cleanup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
# ... use brain ...
brain.reset()  # Clear internal state
```

### 2. Configuration Management

```python
import pynlm

# Create configuration once and reuse
config_template = pynlm.createDefaultConfig()
config_template.set("brain.neuron_count", 1000)

# Create multiple brains with same template
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("random_seed", i)  # Different seeds for variation
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)
```

### 3. Error Recovery

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Robust simulation with error recovery
for episode in range(10):
    try:
        brain.initialize()
        for step in range(100):
            brain.step(step)
            
            # Check for critical errors
            if brain.getTotalSpikeCount() < 10 and step > 50:
                print(f"Warning: Low activity at episode {episode}, step {step}")
                
    except Exception as e:
        print(f"Episode {episode} failed: {e}")
        # Try to recover
        try:
            brain.reset()
            brain.initialize()
        except:
            print("Failed to recover, creating new brain")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
```

## Migration Guide

### From Old API to New API

#### Old Way:
```python
# Old verbose approach
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

#### New Way:
```python
# New concise approach
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.run_for_steps(100)

# Or even simpler
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.reset_and_run(100)
```

## API Reference

### Brain Class

| Method | Description | Pythonic Equivalent |
|--------|-------------|-------------------|
| `initialize()` | Initialize brain | `brain.initialize()` |
| `step(step)` | Run one simulation step | `brain.step(step)` |
| `reset()` | Reset brain state | `brain.reset()` |
| `run_for_steps(n)` | Run N simulation steps | `brain.run_for_steps(n)` |
| `reset_and_run(n)` | Reset and run N steps | `brain.reset_and_run(n)` |
| `get_brain_state()` | Get brain statistics | `brain.get_brain_state()` |

### AgentBrain Class

| Method | Description | Pythonic Equivalent |
|--------|-------------|-------------------|
| `initialize(world)` | Initialize with world | `agent.initialize(world)` |
| `processSensoryInput(percept)` | Process sensory input | `agent.processSensoryInput(percept)` |
| `decodeMotorCommand()` | Decode motor command | `agent.decodeMotorCommand()` |
| `run_agent_episode(world, steps)` | Run complete episode | `agent.run_agent_episode(world, steps)` |
| `train_agent(world, episodes)` | Train agent | `agent.train_agent(world, episodes)` |

### Config Class

| Method | Description | Pythonic Equivalent |
|--------|-------------|-------------------|
| `set(key, value)` | Set configuration value | `config.set(key, value)` |
| `has(key)` | Check if key exists | `config.has(key)` |
| `get(key)` | Get configuration value | `config.get(key)` |
| `set_from_dict(dict)` | Set from Python dict | `config.set_from_dict(dict)` |
| `to_dict()` | Convert to Python dict | `config.to_dict()` |
| `from_preset(name)` | Create from preset | `pynlm.Config.from_preset(name)` |

### SimpleWorld Class

| Method | Description | Pythonic Equivalent |
|--------|-------------|-------------------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world | `world.configure(width, height, visionWidth, visionHeight)` |
| `place_agent(x, y)` | Place agent at position | `world.place_agent(x, y)` |
| `add_resource(type, value)` | Add resource | `world.add_resource(type, value)` |

## Conclusion

The NLM Python API provides powerful tools for creating and simulating artificial neural systems. The new Pythonic convenience methods significantly simplify common operations while maintaining full backward compatibility with existing code.

Key improvements:
- **Reduced complexity**: One-line operations instead of verbose C++ boilerplate
- **Better readability**: Intuitive method names and Pythonic interfaces
- **Enhanced usability**: Context managers and comprehensive error handling
- **Rich state access**: Easy access to all statistics and metrics
- **Zero breaking changes**: All existing code continues to work

The API is now more accessible to Python developers while providing powerful functionality for advanced neural simulations.
