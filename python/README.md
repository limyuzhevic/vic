#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine Python Interface
=====================================================

A comprehensive Python interface to the NLM (Neural Learning Machine) neural
simulation framework. This interface provides Pythonic APIs for creating,
configuring, and running neural brain simulations.

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project.
The Python interface provides access to the full C++ neural simulation framework
through a user-friendly Python API with automatic error handling, convenience
functions, and comprehensive documentation.

## Features

### 1. Pythonic API Design
- Clean, intuitive interfaces for all major operations
- Automatic type conversions and error handling
- Comprehensive docstrings and examples

### 2. Convenience Functions
- Preset configurations for different use cases
- Factory methods for quick brain creation
- Simulation helpers and result objects
- World creation utilities

### 3. Error Handling
- Custom exception hierarchy for different error types
- Graceful failure with informative error messages
- Result objects for operations with success/failure status

### 4. Documentation and Examples
- Comprehensive docstrings with examples
- Practical examples demonstrating real usage
- Configuration presets for different scenarios
- Agent integration examples

### 5. Backward Compatibility
- Maintains compatibility with existing C++ bindings
- Gradual migration path for existing users
- Optional advanced features for power users

## Installation

### From Source (Development)

```bash
# Clone the repository
# Navigate to the workspace/app directory
cd /workspace/app

# Build the C++ bindings (if not already built)
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python package
pip install python/

# Or install in development mode
pip install -e python/
```

### Using pip (Recommended for Development)

```bash
pip install scikit-build-core pybind11
pip install -e python/
```

## Basic Usage

### 1. Import the Module

```python
import nlm
```

### 2. Create a Brain

```python
# Create with default configuration
config = nlm.create_config()
brain = nlm.create_brain()
brain.initialize()

# Create with preset configuration
config = nlm.create_config(preset='learning')
brain = nlm.create_brain(preset='learning')
```

### 3. Run a Simple Simulation

```python
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 4. Create a Complete Agent Simulation

```python
import nlm

# Create configuration
config = nlm.create_config(preset='learning')

# Create brain and initialize
brain = nlm.create_brain(config)
brain.initialize()

# Create world
world = nlm.create_world(width=20, height=20, agent_start=(10, 10))

# Create agent interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
stats = nlm.run_simulation(world, brain, agent, steps=1000)

print(f"Simulation completed with {stats.step_count} steps")
print(f"Final curiosity: {stats.get_curiosity_level():.3f}")
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM parameters.

```python
# Create configuration
config = nlm.create_config(preset='learning')

# Get and set values
config.set("brain.neuron_count", 2000)
value = config.get("brain.neuron_count")

# Load and save
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### Brain
Main neural computation engine.

```python
# Create and initialize
brain = nlm.create_brain()
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, timestamp)

# Input/Output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Statistics
brain.getTotalNeuronCount()
brain.getFiringRate()
brain.getExcitationInhibitionRatio()
```

#### AgentBrain
Interface between brain and world environment.

```python
# Create agent interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor commands
motor_cmd = agent.decodeMotorCommand()

# Apply neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
```

#### SimpleWorld
2D simulation environment.

```python
# Create world
world = nlm.create_world(width=20, height=20)
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent position
world.setAgentStart(10.0, 10.0)

# Run simulation
world.update(timestep=0.1)

# Get sensor data
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

### Factory Classes

#### BrainFactory
Factory for creating brains with convenient presets.

```python
factory = nlm.BrainFactory()

# Create brains by size
brain_small = factory.create_brain(size='small')
brain_medium = factory.create_brain(size='medium')
brain_large = factory.create_brain(size='large')

# Create with features
brain_with_dev = factory.create_brain_with_dev(development_enabled=True)
brain_with_neuromod = factory.create_brain_with_neuromod(neuromodulation_enabled=True)

# Create with custom configuration
custom_brain = factory.create_brain(custom_config={'brain.neuron_count': 3000})
```

#### WorldHelper
Helper for world creation and management.

```python
helper = nlm.WorldHelper()

# Create world with specific settings
world = helper.create_world(width=30, height=30, agent_start=(15, 15))
```

#### SimulationRunner
High-level simulation runner with statistics collection.

```python
runner = nlm.SimulationRunner()

# Run simulation with automatic statistics
stats = runner.run_simulation(world, brain, agent, steps=1000)

# Access statistics
print(f"Active neurons: {stats.get_active_neurons()}")
print(f"Firing rate: {stats.get_firing_rate():.3f}")
```

### Result Objects

#### Result
Generic result object for operation success/failure.

```python
# Perform operation with error handling
result = nlm.save_config(config, "config.json")

if result.success:
    print("Operation completed successfully")
else:
    print(f"Error: {result.error}")
```

#### SimulationStats
Comprehensive statistics from simulation runs.

```python
# Access various statistics
stats = nlm.run_simulation(world, brain, agent, steps=1000)

print(f"Step count: {stats.step_count}")
print(f"Active neurons: {stats.get_active_neurons()}")
print(f"Firing rate: {stats.get_firing_rate():.3f}")
print(f"E/I ratio: {stats.get_ei_ratio():.3f}")
print(f"Curiosity: {stats.get_curiosity_level():.3f}")
print(f"Development stage: {stats.get_development_stage()}")
```

### Configuration Presets

```python
# Available presets
presets = ['quick_dev', 'high_perf', 'learning', 'production']

# Create configuration with preset
config = nlm.create_config(preset='learning')
```

Each preset provides optimized settings for different use cases:

- **quick_dev**: Fast iteration for development
- **high_perf**: Maximum performance and throughput
- **learning**: Rich learning and development features
- **production**: Stable, production-ready configuration

### Error Handling

The NLM Python interface provides a comprehensive exception hierarchy:

```python
import nlm

try:
    # May raise different exceptions
    config = nlm.create_config(preset='invalid')
    brain = nlm.create_brain()
    
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
except nlm.BrainError as e:
    print(f"Brain error: {e}")
except nlm.WorldError as e:
    print(f"World error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NLMLibError as e:
    print(f"Library error: {e}")
```

### Example Scripts

The `examples.py` file contains comprehensive examples demonstrating:
- Basic usage and simulation
- Complete agent integration
- Configuration management
- Error handling
- Advanced factory usage
- Statistics collection and analysis

Run the examples with:
```bash
python examples.py
```

## Advanced Usage

### Custom Callbacks

```python
def simulation_callback(stats):
    if stats['step'] % 100 == 0:
        print(f"Step {stats['step']}: "
              f"{stats['active_neurons']} neurons, "
              f"curiosity={stats['curiosity']:.3f}")

stats = nlm.run_simulation(world, brain, agent, steps=1000,
                          callback=simulation_callback)
```

### Complex Configuration

```python
# Create complex configuration
custom_config = {
    'brain.neuron_count': 5000,
    'brain.synapse_density': 0.3,
    'plasticity.stdp.learning_rate': 0.001,
    'plasticity.hebbian.enable': True,
    'plasticity.structural.enable': True,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True,
    'neuromod.dopamine.scale': 1.0,
    'development.initial_stage': 'CriticalPeriod',
    'brain.enable_prediction': True,
    'brain.enable_memory': True,
    'brain.enable_cognition': True
}

config = nlm.create_config(custom_config=custom_config)
```

### Batch Processing

```python
# Process multiple simulations
brains = []
for i in range(5):
    brain = nlm.create_brain(preset='quick_dev')
    brain.initialize()
    brains.append(brain)

# Run all simulations
for i, brain in enumerate(brains):
    for step in range(100):
        brain.step(step)
    print(f"Brain {i} completed with {brain.getTotalSpikeCount()} spikes")
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_stage` | string | 'Maturation' | Initial developmental stage |
| `development.plasticity_rate` | float | 1.0 | Rate of plasticity changes with development |

## Migration Guide

### From C++ Bindings

If you're migrating from the C++ bindings, you can:

1. **Simple migration**: Use the convenience functions
2. **Gradual migration**: Keep using C++ bindings for advanced features
3. **Full migration**: Replace all C++ code with Pythonic interfaces

The new interface maintains access to all the same C++ APIs through the underlying `pynlm` module.

## Troubleshooting

### Common Issues

#### Import Error
```bash
pip install -e python/
```

#### Missing Dependencies
```bash
pip install scikit-build-core pybind11
```

#### Build Errors
```bash
# Ensure CMake and C++20 compiler
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Runtime Errors
```bash
# Check that initialize() is called before step()
# Ensure reset() is called before re-running simulations
```

## Development

### Building from Source

```bash
# Clone the repository
cd /workspace/app

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build Python bindings
make pynlm

# Install Python package
pip install -e python/
```

### Testing

```bash
# Run Python examples
python python/examples.py

# Test with pytest (if available)
pytest python/
```

### Adding New Examples

Add examples to `examples.py` or create separate example files in the `examples/` directory.

## License

MIT License. See LICENSE file for details.

## Authors

NLM Development Team
https://github.com/Kilo-Org/nlm

---

*Generated automatically from the NLM source code.*
*Last updated: 2026-10-17*
"""

from typing import List

# Re-export key symbols for easier import
__all__ = [
    'NLMLibError', 'ConfigError', 'BrainError', 'WorldError', 'SimulationError',
    'Result', 'SimulationStats', 'ConfigPresets',
    'BrainFactory', 'WorldHelper', 'SimulationRunner',
    'create_config', 'create_brain', 'create_world', 'run_simulation',
    'save_config', 'load_config'
]

# Import main classes for backward compatibility
if __name__ == '__main__':
    print(__doc__)