# NLM Python Bindings - Enhanced Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings.

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain
project that provides Python bindings for a C++ neural simulation framework. The enhanced
Python bindings (version 0.1.0) provide significant improvements over the original API:

### Key Improvements

- **Context Managers**: Automatic resource management for brain lifecycle
- **Pythonic API**: Dictionary-style configuration access and convenience methods
- **Enhanced Error Handling**: Type-safe operations and comprehensive error handling
- **Advanced Debugging**: Performance monitoring and visualization tools
- **Experimental Setup**: Advanced configuration for research experiments
- **Performance Monitoring**: Built-in statistics and timing utilities

## Installation

### Prerequisites

- Python 3.8+
- C++ compiler with C++20 support
- CMake 3.16+

### Installation from Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install the package
pip install .

# Or install in development mode
pip install -e .
```

### Installation Verification

```python
import pynlm

print(f"NLM Python Bindings Version: {pynlm.__version__}")
print(f"Description: {pynlm.__description__}")
print(f"Author: {pynlm.__author__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

## Basic Usage

### Creating a Brain

```python
import pynlm

# Simple brain creation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

### Running a Simulation

```python
import pynlm

# Using enhanced convenience methods
config = pynlm.createDefaultConfig()
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=1000, timestep=0.1, verbose=True)
```

### Complete Agent Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Enhanced Features

### 1. Context Managers

Context managers provide automatic resource management:

```python
# Automatic brain lifecycle management
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=500)
    # Brain is automatically reset when exiting context
```

### 2. Pythonic Convenience Methods

#### Brain Class

```python
# Get comprehensive statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']} Hz")

# Run enhanced simulations
brain.simulate(steps=1000, timestep=0.05, verbose=True)
brain.run_episode(world, agent, steps=100)
brain.visualize_brain("output.png")
```

#### AgentBrain Class

```python
# Get agent statistics
agent_stats = agent.get_agent_stats()
print(f"Curiosity level: {agent_stats['curiosity_level']}")
print(f"Novelty level: {agent_stats['novelty_level']}")

# Setup learning with convenience method
agent.setup_learning(reward=True, curiosity=True, development=True)

# Create and run episodes
agent.create_episode(world, steps=200)
```

### 3. Enhanced Configuration API

#### Dictionary-Style Access

```python
config = pynlm.createDefaultConfig()

# Get values with type-safe methods
neuron_count = config.get_int("brain.neuron_count", 1000)
synapse_density = config.get_float("brain.synapse_density", 0.1)
is_enabled = config.get_bool("plasticity.stdp.enable", True)

# Python dictionary-style access
config["brain.neuron_count"] = 500
value = config["some.key"]  # Raises KeyError if not found

# Update multiple values
updates = {
    "brain.synapse_density": 0.15,
    "neuromod.curiosity.enable": True
}
config.update(updates)

# Save configuration as dictionary
config_dict = config.save_dict()
for key, value in config_dict.items():
    print(f"{key}: {value}")
```

### 4. Error Handling

```python
config = pynlm.createDefaultConfig()

# Safe access with defaults
value = config.get_string("nonexistent.key", "default_value")

# Type checking
if isinstance(value, int):
    # Safe to use as integer
    pass
elif isinstance(value, float):
    # Safe to use as float
    pass

# Exception handling
try:
    invalid_value = config["invalid.key"]
except KeyError as e:
    print(f"Key not found: {e}")
```

### 5. Performance Monitoring

```python
import time

brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
brain.simulate(steps=1000, verbose=True)
end_time = time.time()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Get performance statistics
stats = brain.get_statistics()
print(f"Firing rate: {stats['average_firing_rate']:.3f} Hz")
print(f"Network efficiency: {stats['firing_neurons']/stats['total_neurons']*100:.1f}%")
```

## Advanced Features

### Experimental Setup

```python
def create_experimental_brain():
    """Create a brain with experimental configuration"""
    config = pynlm.createDefaultConfig()
    
    experimental_config = {
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.12,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.strength": 0.8,
        "development.enabled": True,
        "debug.enabled": True,
        "debug.log_level": "INFO"
    }
    
    config.update(experimental_config)
    return pynlm.createBrain(config)

# Use context manager for controlled experiments
with pynlm.BrainContext(brain) as experimental_brain:
    experimental_brain.initialize()
    experimental_brain.simulate(steps=1000)
    stats = experimental_brain.get_statistics()
    experimental_brain.visualize_brain("experiment.png")
```

### Visualization and Debugging

```python
# Create visualizations
brain.visualize_brain("brain_state.png")
world.visualize("world_state.png")

# Get debugging information
brain.logStatus()
stats = brain.get_statistics()

# Debug agent behavior
agent_stats = agent.get_agent_stats()
print(f"Agent state: {agent_stats}")
```

## Examples

### Example 1: Basic Enhanced Usage

```python
import pynlm

# Basic usage with enhanced API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=200, verbose=True)
    
    # Get comprehensive statistics
    stats = brain.get_statistics()
    print(f"Neurons: {stats['total_neurons']}")
    print(f"Firing rate: {stats['average_firing_rate']:.3f} Hz")
```

### Example 2: Complete Agent Simulation

```python
import pynlm
import time

# Complete agent simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure and initialize
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)
agent.setup_learning(reward=True, curiosity=True)

# Run simulation with timing
start_time = time.time()
brain.run_episode(world, agent, steps=100, verbose=True)
end_time = time.time()

print(f"Episode completed in {end_time - start_time:.2f} seconds")
```

### Example 3: Experimental Setup

```python
import pynlm

# Advanced experimental setup
config = pynlm.createDefaultConfig()

# Configure for specific experiment
experimental_config = {
    "brain.neuron_count": 1500,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True,
    "development.enabled": True,
    "debug.enabled": True
}

config.update(experimental_config)

# Create and run experiment
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=500, timestep=0.01, verbose=True)
    
    # Analyze results
    stats = brain.get_statistics()
    print(f"Experiment results:")
    print(f"  Neurons: {stats['total_neurons']}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Efficiency: {stats['firing_neurons']/stats['total_neurons']*100:.1f}%")
```

## Performance Considerations

### Memory Management

Context managers automatically handle memory cleanup:

```python
# Recommended: Use context managers
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    # Use brain safely
    brain.simulate(steps=100)

# Avoid: Manual management without cleanup
brain = pynlm.createBrain(config)
brain.initialize()
# ... use brain ...
brain.reset()  # Manual cleanup required
```

### Large Scale Simulations

For simulations with many neurons:

```python
# Configure for large scale
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.structural.enable", True)

# Use performance monitoring
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=1000, timestep=0.01, verbose=True)
    
    # Monitor memory usage
    stats = brain.get_statistics()
    print(f"Performance:")
    print(f"  Neurons: {stats['total_neurons']}")
    print(f"  Synapses: {stats['total_synapses']}")
    print(f"  Firing rate: {stats['average_firing_rate']:.3f} Hz")
```

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**

```bash
pip install --force-reinstall .
```

2. **Brain doesn't initialize**

```python
# Ensure configuration is valid
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
brain = pynlm.createBrain(config)
brain.initialize()  # Should return True
```

3. **Segmentation Fault**

```python
# Use context managers for safe resource management
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=100)  # Safe execution
```

4. **Memory Usage**

```python
# Monitor memory usage during large simulations
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

### Error Messages

| Error | Cause | Solution |
|-------|--------|----------|
| KeyError: 'key' | Configuration key not found | Use `config.get_string('key', 'default')` |
| TypeError | Invalid configuration type | Use type checking with `isinstance()` |
| RuntimeError | Brain not initialized | Call `brain.initialize()` before using |

## Best Practices

### 1. Use Context Managers

Always use context managers for automatic resource management:

```python
# Good
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=100)

# Avoid
brain = pynlm.createBrain(config)
brain.initialize()
# ... use brain ...
brain.reset()
```

### 2. Leverage Convenience Methods

Use enhanced convenience methods for common operations:

```python
# Instead of manual loops
for i in range(100):
    brain.step(i)

# Use enhanced method
brain.simulate(steps=100, verbose=True)

# Instead of manual statistics
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()

# Use convenience method
stats = brain.get_statistics()
```

### 3. Pythonic Configuration

Use Pythonic configuration APIs:

```python
# Instead of multiple set() calls
config.set("key1", value1)
config.set("key2", value2)

# Use dictionary-style updates
updates = {"key1": value1, "key2": value2}
config.update(updates)
```

### 4. Error Handling

Always handle errors appropriately:

```python
config = pynlm.createDefaultConfig()

try:
    value = config["nonexistent.key"]
except KeyError:
    value = "default_value"  # Graceful handling
```

### 5. Performance Monitoring

Monitor performance for optimization:

```python
import time

start_time = time.time()
brain.simulate(steps=1000)
end_time = time.time()

print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Performance: {1000 / (end_time - start_time):.0f} steps/second")
```

## Migration Guide

### From Old API to New API

#### Old Way

```python
# Old API
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

#### New Way

```python
# New API (backward compatible)
import pynlm

with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=100, verbose=True)
    stats = brain.get_statistics()
    print(f"Neurons: {stats['total_neurons']}")
```

## API Reference

### Classes

#### `pynlm.Brain`
- `initialize()`: Initialize brain
- `step(step, time=None)`: Run simulation step
- `simulate(steps, timestep=0.1, verbose=False)`: Convenience simulation
- `run_episode(world, agent, steps, timestep=0.1, verbose=False)`: Complete episode
- `get_statistics()`: Get comprehensive statistics
- `visualize_brain(output_file=None)`: Create visualization
- `getConfig()`: Get configuration
- `logStatus()`: Log brain status

#### `pynlm.AgentBrain`
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `get_agent_stats()`: Get agent statistics
- `setup_learning(reward=False, curiosity=False, development=False)`: Setup learning

#### `pynlm.Config`
- `has(key)`: Check if key exists
- `getInt(key, default)`: Get integer value
- `getFloat(key, default)`: Get float value
- `getString(key, default)`: Get string value
- `getBool(key, default)`: Get boolean value
- `update(updates)`: Update multiple values
- `save_dict()`: Save as Python dictionary

### Constants

- `pynlm.INVALID_NEURON_ID`: Invalid neuron ID
- `pynlm.INVALID_SYNAPSE_ID`: Invalid synapse ID
- `pynlm.INVALID_REGION_ID`: Invalid region ID
- `pynlm.INVALID_POPULATION_ID`: Invalid population ID

### Enumerations

- `pynlm.NeuronType`: Neuron types (Excitatory, Inhibitory, etc.)
- `pynlm.DevelopmentalStage`: Developmental stages
- `pynlm.ActionType`: Action types
- `pynlm.MotorCommand`: Motor commands

## Version Information

- **Version**: 0.1.0
- **API Status**: Enhanced with backward compatibility
- **License**: MIT
- **Author**: NLM Authors

## Getting Help

### Documentation

- This document: `PYTHON_API.md`
- Examples: `examples.py`, `beginner_examples.py`
- Complete documentation: `docs/`

### Support

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Examples: Run `examples.py` for comprehensive demonstrations

## Acknowledgments

This enhanced API was developed to provide a more Pythonic and user-friendly interface to the NLM neural simulation framework. The improvements include:

- Context managers for resource management
- Pythonic convenience methods
- Enhanced error handling
- Performance monitoring tools
- Advanced configuration options

All enhancements maintain backward compatibility with existing code.
"""
