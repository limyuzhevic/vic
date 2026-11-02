# Enhanced NLM Python Bindings

This document provides a comprehensive guide to the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Introduction

The enhanced NLM Python bindings provide a Pythonic interface to the C++ neural simulation framework with:

- **Pythonic APIs**: Intuitive method names and parameter handling
- **Context Managers**: Automatic resource management and simulation helpers
- **Enhanced Configuration**: Presets and validation utilities
- **Advanced Simulation Tools**: Batch running, episode management, and statistical analysis
- **Better Documentation**: Comprehensive examples and API reference

## Quick Start

### Basic Usage

```python
import pynlm

# Create a brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Agent Simulation with Context Manager

```python
import pynlm

# Create a complete agent simulation setup
with pynlm.agent_simulation(preset="research") as sim:
    # Run 100 simulation steps
    sim.runSteps(100)
    
    # Get detailed statistics
    stats = sim.getStatistics()
    print(f"Neurons: {stats['neurons']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    
    # Format statistics as a table
    print(sim.formatStatistics("table"))
```

## Enhanced Configuration System

### Presets

The enhanced configuration system includes several presets:

```python
import pynlm

# Basic preset - minimal configuration
config = pynlm.ConfigHelper.createPreset("basic")

# Research preset - comprehensive configuration for scientific experiments
config = pynlm.ConfigHelper.createPreset("research")

# Real-time preset - optimized for real-time applications
config = pynlm.ConfigHelper.createPreset("real_time")
```

### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

# Validate configuration (raises exception if invalid)
try:
    pynlm.validateConfig(config)
    print("Configuration is valid!")
except Exception as e:
    print(f"Configuration error: {e}")

# Get validation errors without raising exception
errors = pynlm.getConfigValidationErrors(config)
if errors:
    print("Validation errors:")
    for error in errors:
        print(f"  - {error}")
```

### Configuration Schema

```python
import pynlm

# Get configuration schema
schema = pynlm.ConfigHelper.getConfigSchema()
for key, description in schema.items():
    print(f"{key}: {description}")

# List available presets
presets = pynlm.ConfigHelper.listPresets()
print(f"Available presets: {presets}")
```

## Advanced Simulation Features

### Agent Simulation Context Manager

The `BrainSimulationContext` provides a high-level interface for running agent simulations:

```python
import pynlm

# Method 1: Manual setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

sim = pynlm.agentSimulation("advanced", brain, world, agent)
sim.runSteps(100).runSteps(100).runSteps(100)
stats = sim.getStatistics()

# Method 2: Using presets
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)
    print(sim.formatStatistics())
```

### World Simulation Context Manager

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Run world simulation
with pynlm.worldSimulation(world) as world_sim:
    world_sim.runSteps(100)
    print(f"Final world time: {world_sim.getTime()}")
```

### Batch Running for Statistical Analysis

```python
import pynlm

# Run 50 episodes with 200 steps each
results = pynlm.batchRunEpisodes(
    num_episodes=50,
    steps_per_episode=200,
    preset="research"
)

# Analyze results
total_rewards = [r["total_reward"] for r in results]
avg_rewards = [r["avg_reward_per_step"] for r in results]

print(f"Average total reward: {sum(total_rewards) / len(total_rewards):.2f}")
print(f"Average reward per step: {sum(avg_rewards) / len(avg_rewards):.4f}")
print(f"Standard deviation: {sum((x - sum(avg_rewards)/len(avg_rewards))**2 for x in avg_rewards) / len(avg_rewards):.4f}")
```

### Complete Episode Runner

```python
import pynlm

# Run a single complete episode
results = pynlm.runAgentEpisode(max_steps=1000, preset="research")

print("Episode Results:")
for key, value in results.items():
    print(f"  {key}: {value}")
```

## Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced utilities."""
    
    # 1. Create configuration with preset
    config = pynlm.ConfigHelper.createPreset("research")
    
    # 2. Create and initialize brain
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
    
    # 7. Use enhanced context manager for simulation
    with pynlm.agent_simulation("advanced", brain, world, agent) as sim:
        # Run simulation
        sim.runSteps(num_steps)
        
        # Print progress
        for step in range(0, num_steps, 100):
            print(f"Step {step}:")
            print(f"  Firing neurons: {sim.getStatistics()['firing_neurons']}")
            print(f"  Curiosity: {sim.getStatistics()['curiosity']:.3f}")
            print(f"  Novelty: {sim.getStatistics()['novelty']:.3f}")
            print(f"  Dev Stage: {sim.getStatistics()['development_stage']}")
            
            # Save checkpoint every 100 steps
            if step % 100 == 0:
                brain.save(f"checkpoint_step_{step}.bin")
    
    print("Agent simulation complete!")
    
    # 8. Save final statistics
    with open("simulation_stats.txt", "w") as f:
        f.write(sim.formatStatistics("json"))

# Run the simulation
run_agent_simulation(1000)
```

## Pythonic API Examples

### Context Manager for Resource Management

```python
import pynlm

# Example: Using context managers for resource management
with pynlm.agent_simulation(preset="research") as sim:
    # Resources are automatically managed
    sim.runSteps(100)
    
# Outside the context, resources are cleaned up automatically
```

### Method Chaining

```python
import pynlm

# Chain method calls for fluent interface
pynlm.agent_simulation(preset="research")
    .runSteps(50)
    .runSteps(50)
    .runSteps(50)
```

### Statistical Analysis

```python
import pynlm

# Run simulation and get detailed statistics
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)
    
    # Get statistics in different formats
    table_stats = sim.formatStatistics("table")
    json_stats = sim.formatStatistics("json")
    
    # Parse statistics
    stats = sim.getStatistics()
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Average firing rate: {stats['avg_firing_rate']}")
```

## Configuration Management

### Configuration Presets

The enhanced configuration system provides three presets:

1. **Basic**: Minimal configuration for quick testing
2. **Research**: Comprehensive configuration for scientific experiments
3. **Real-time**: Optimized for real-time applications

```python
import pynlm

# Available presets
presets = pynlm.ConfigHelper.listPresets()
print(f"Available presets: {presets}")

# Create different presets
basic_config = pynlm.ConfigHelper.createPreset("basic")
research_config = pynlm.ConfigHelper.createPreset("research")
real_time_config = pynlm.ConfigHelper.createPreset("real_time")
```

### Configuration Validation

```python
import pynlm

# Create a configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)

# Validate configuration
errors = pynlm.getConfigValidationErrors(config)
if not errors:
    print("✓ Configuration is valid")
else:
    print("✗ Configuration errors:")
    for error in errors:
        print(f"  - {error}")
```

## Error Handling and Debugging

### Enhanced Error Messages

```python
import pynlm

# Error handling is now more descriptive
try:
    # Create invalid configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", -1)  # Invalid: negative neurons
    
    # This will raise a descriptive error
    pynlm.validateConfig(config)
except Exception as e:
    print(f"Configuration error: {e}")
```

### Simulation Debugging

```python
import pynlm

# Run simulation with detailed logging
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(10)
    print("Initial statistics:")
    print(sim.formatStatistics("table"))
    
    sim.runSteps(90)
    print("\nFinal statistics:")
    print(sim.formatStatistics("table"))
```

## Performance and Efficiency

### Batch Processing

```python
import pynlm

# Run multiple episodes for statistical analysis
print("Running batch simulation...")
results = pynlm.batchRunEpisodes(
    num_episodes=20,
    steps_per_episode=500,
    preset="research"
)

print(f"Completed {len(results)} episodes")
avg_reward = sum(r["total_reward"] for r in results) / len(results)
print(f"Average reward per episode: {avg_reward:.2f}")
```

### Memory Management

```python
import pynlm

# Context managers ensure proper cleanup
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)
    # Resources are automatically cleaned up here

# No need to manually delete objects - context manager handles it
```

## Migration Guide

### From Basic to Enhanced API

If you're using the basic API and want to upgrade, here's what changes:

#### Old API
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### New API
```python
import pynlm

with pynlm.agent_simulation(preset="basic") as sim:
    sim.runSteps(100)
```

### Error Handling Improvements

The enhanced bindings provide better error handling:

```python
# Old: Silent failures
brain.step(step)  # Might fail silently

# New: Descriptive error messages
with pynlm.agent_simulation(preset="basic") as sim:
    sim.runSteps(100)  # Clear error messages if something goes wrong
```

## Best Practices

### 1. Use Context Managers

Always use context managers for simulation management:

```python
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)
```

### 2. Validate Configurations

Validate configurations before running simulations:

```python
config = pynlm.createDefaultConfig()
errors = pynlm.getConfigValidationErrors(config)
if errors:
    raise ValueError(f"Invalid configuration: {errors}")
```

### 3. Use Presets for Consistency

Use presets for consistent experimental setups:

```python
config = pynlm.ConfigHelper.createPreset("research")
```

### 4. Batch Run for Statistical Analysis

For scientific experiments, use batch running:

```python
results = pynlm.batchRunEpisodes(50, 200)
```

### 5. Save Checkpoints

Save checkpoints during long simulations:

```python
with pynlm.agent_simulation(preset="research") as sim:
    for i in range(0, 1000, 100):
        sim.runSteps(100)
        brain.save(f"checkpoint_{i}.bin")
```

## Reference

### Key Classes

- **BrainSimulationContext**: Context manager for agent simulations
- **WorldSimulationContext**: Context manager for world simulations
- **ConfigHelper**: Configuration presets and validation utilities

### Key Methods

- **agentSimulation()**: Create agent simulation context
- **worldSimulation()**: Create world simulation context
- **runAgentEpisode()**: Run a complete episode
- **batchRunEpisodes()**: Run multiple episodes
- **validateConfig()**: Validate configuration
- **getConfigSchema()**: Get configuration schema

### Key Attributes

- **__version__**: Library version
- **__description__**: Library description
- **INVALID_NEURON_ID**: Invalid neuron ID constant
- **INVALID_SYNAPSE_ID**: Invalid synapse ID constant
- **INVALID_REGION_ID**: Invalid region ID constant
- **INVALID_POPULATION_ID**: Invalid population ID constant

## Changelog

### Version 2.0.0 - Enhanced

- Added Pythonic context managers
- Added enhanced configuration system with presets
- Added advanced simulation helpers and utilities
- Improved error handling and validation
- Enhanced documentation and examples
- Added batch running capabilities
- Added statistical analysis tools

### Version 1.0.0 - Basic

- Original Python bindings
- Basic API for NLM access
- Limited configuration options
- Basic error handling

## Credits

This enhanced version builds on the original NLM Python bindings and adds significant improvements for better usability, error handling, and scientific experimentation capabilities.

The enhancements were developed to address common pain points in neural simulation workflows and provide a more intuitive Python experience for researchers and developers working with the NLM framework.