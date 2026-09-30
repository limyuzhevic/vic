# NLM Python Interface - Enhanced Documentation

This document provides comprehensive information about the enhanced NLM Python interface.

## Overview

The NLM (Neural Learning Machine) Python interface provides a user-friendly way to interact with the complex C++ brain simulation framework. This enhanced interface introduces a more Pythonic API with convenience functions, error handling, and high-level abstractions.

## Installation

### Prerequisites

- Python 3.8 or higher
- pip package manager

### Installation from source

```bash
pip install -e .
```

### Quick installation

For development and testing:

```bash
pip install -e .
```

## Key Improvements

### 1. Pythonic API

- **Convenience wrappers**: High-level functions for common operations
- **Type safety**: Better error messages and type checking
- **Intuitive naming**: Pythonic method names (`create_brain()` instead of `createBrain()`)
- **Documentation**: Comprehensive docstrings and examples

### 2. Enhanced Configuration Management

- **Configuration templates**: Pre-defined configurations for common use cases
- **Validation**: Configuration parameter validation
- **Persistence**: Save and load configuration files
- **Default settings**: Sensible defaults for new projects

### 3. Advanced Learning Features

- **Curiosity-driven learning**: Exploration based on novelty and curiosity
- **Reward modulation**: Advanced reinforcement learning
- **Developmental progression**: Maturation and specialization over time
- **Prediction error learning**: Error-based learning and adaptation

### 4. Comprehensive Examples

- **Brain examples**: Basic brain operations and testing
- **Agent examples**: World interaction and navigation
- **Learning examples**: Advanced learning mechanisms
- **Benchmarking**: Performance testing and optimization

### 5. Error Handling and Robustness

- **Exception handling**: Clear, actionable error messages
- **Input validation**: Parameter checking and validation
- **Graceful fallbacks**: Handling of missing functionality
- **Progress reporting**: Informative output during long operations

## Quick Start Guide

### Creating and running a simple brain

```python
import pynlm

# Create a simple brain
brain = pynlm.create_brain()
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

# Get statistics
stats = brain.get_stats()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")
```

### Creating an agent in a world

```python
import pynlm

# Create brain, world, and agent
brain = pynlm.create_brain({"neuron_count": 200})
world = pynlm.create_simple_world(width=20, height=20)
agent = pynlm.create_agent(brain, world)

# Enable learning features
agent.enable_feature('reward_modulation', True)
agent.enable_feature('curiosity', True)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory(percept)
    brain.step(step)
    action = agent.decode_action()
    world.apply_action(action, world.get_simulation_time())
    agent.apply_reward(world.get_reward())

print("Simulation complete!")
```

### Using configuration templates

```python
import pynlm

# Available templates
print("Available templates:", pynlm.get_available_templates())

# Load a specific template
config = pynlm.load_config_template('rich')
brain = pynlm.create_brain(config)

# Or create custom configuration
from pynlm import Config
custom_config = Config({
    "neuron_count": 300,
    "connection_probability": 0.25,
    "enable_development": True
}, name="My Custom Brain")
```

### Running examples

```python
import pynlm

# Run brain examples
print("=== Brain Examples ===")
pynlm.brain_examples.simple_brain_example()

# Run agent examples  
print("\n=== Agent Examples ===")
pynlm.agent_examples.basic_agent_example()

# Run learning examples
print("\n=== Learning Examples ===")
pynlm.learning_examples.basic_learning_example()
```

## Configuration Templates

The following configuration templates are available:

### simple
- **Name**: Simple Brain
- **Description**: Basic brain with minimal configuration
- **Neurons**: 100
- **Connections**: 0.1 probability

### rich
- **Name**: Rich Simulation
- **Description**: Brain with lots of neurons and strong plasticity
- **Neurons**: 1000
- **Connections**: 0.3 probability
- **Development**: Enabled

### fast
- **Name**: Fast Test
- **Description**: Small brain for quick testing
- **Neurons**: 50
- **Connections**: 0.05 probability

## Available Features

### Agent Features

- **Reward modulation**: Learn from rewards and punishments
- **Structural plasticity**: Grow and prune connections
- **Development**: Maturation and specialization
- **Curiosity**: Explore novel situations

### World Features

- **Sensory input**: Vision, touch, internal signals
- **Action output**: Movement, interaction, communication
- **Environment dynamics**: Physics and interaction rules
- **Observation processing**: Sensor data integration

### Brain Features

- **Neural computation**: Spiking neuron dynamics
- **Memory systems**: Working memory, episodic memory
- **Plasticity**: Synaptic modification and learning
- **Prediction**: Forward models and error signals

## Advanced Usage

### Benchmarking performance

```python
from pynlm import benchmark_performance

# Test different configurations
configs = {
    "Small": {"neuron_count": 50},
    "Medium": {"neuron_count": 200},
    "Large": {"neuron_count": 1000}
}

for name, config in configs.items():
    results = benchmark_performance(config, steps=1000)
    print(f"{name}: {results['benchmark']['steps_per_second']:.0f} steps/second")
```

### Creating test simulations

```python
from pynlm import create_test_agent_simulation

# Create a complete test simulation
results = create_test_agent_simulation(steps=1000, config_template='fast')

# Analyze results
print(f"Simulation completed with {len(results['actions'])} actions")
print(f"Average reward: {sum(results['rewards']) / len(results['rewards']):.2f}")
```

### Saving and loading configurations

```python
from pynlm import save_config_to_file, load_config_from_file

# Save configuration
config_dict = {"neuron_count": 200, "connection_probability": 0.2}
save_config_to_file(config_dict, "/tmp/my_config.json")

# Load configuration
loaded_config = load_config_from_file("/tmp/my_config.json")
```

## API Reference

### Core Classes

#### Config
Configuration management class with validation and templates.

Methods:
- `__init__(config_dict=None, name="")`: Create configuration
- `set(key, value)`: Set configuration value
- `get(key, default=None)`: Get configuration value
- `has(key)`: Check if key exists
- `to_dict()`: Convert to dictionary
- `update(other_dict)`: Update from dictionary

#### Brain (Wrapper)
High-level brain interface with convenience methods.

Methods:
- `initialize()`: Initialize brain
- `step(step_number, time=None)`: Run simulation step
- `get_stats()`: Get comprehensive statistics
- `get_total_spikes()`: Get spike count
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `reset()`: Reset brain

#### Agent (Wrapper)
High-level agent interface for world interaction.

Methods:
- `initialize(world)`: Initialize with world
- `process_sensory(percept)`: Process sensory input
- `decode_action()`: Get motor command
- `apply_reward(reward, predicted_reward=0.0)`: Apply reward
- `update_development(timestep)`: Update development
- `enable_feature(feature, enable)`: Enable/disable features

#### World (Wrapper)
High-level world interface for simulation environment.

Methods:
- `initialize(width, height, vision_width, vision_height)`: Initialize world
- `update(timestep)`: Update world state
- `get_sensory_percept()`: Get sensory input
- `apply_action(action, time)`: Apply motor command

### Convenience Functions

#### create_brain(config_dict=None, name="")
Create a new brain with configuration.

#### create_simple_world(width, height, vision_width, vision_height)
Create a simple 2D world.

#### create_agent(brain, world)
Create an agent that interfaces brain and world.

#### load_config_template(template_name)
Load a predefined configuration template.

#### benchmark_performance(config_dict, steps)
Run performance benchmark.

#### create_test_agent_simulation(steps, config_template)
Create complete test simulation.

### Modules

#### pynlm.brain_examples
Brain creation and testing examples.

#### pynlm.agent_examples  
Agent behavior and world interaction examples.

#### pynlm.learning_examples
Learning mechanisms and cognitive development examples.

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- **Solution**: Did you call `brain.initialize()`?
- **Hint**: Try increasing the number of steps

#### "The agent isn't moving"
- **Solution**: Did you call `world.apply_action()`?
- **Hint**: Check that `world.update()` is being called

#### "Everything is 0"
- **Solution**: Brains need time to "warm up" - try more steps
- **Hint**: Some neurons need input to fire - ensure sensory input is connected

#### "Module not found: pynlm"
- **Solution**: Make sure the package is installed
- **Hint**: `pip install -e .` in the project root

#### "C++ bindings not available"
- **Solution**: Install pynlm with C++ support
- **Hint**: The package requires C++ compilation

## Performance Considerations

### Configuration Choices

- **Small brains (50-100 neurons)**: Fast for testing, limited capacity
- **Medium brains (200-500 neurons)**: Good balance of speed and capability
- **Large brains (1000+ neurons)**: High capacity, slower execution

### Simulation Speed

- **Step complexity**: O(neuron_count²) for dense connectivity
- **Memory usage**: O(neuron_count + synapse_count)
- **Recommendation**: Start with small configurations for development

## Future Enhancements

### Planned Features

1. **Web interface**: Interactive brain simulation
2. **Visualization tools**: Brain activity visualization
3. **Distributed computing**: Multi-agent simulations
4. **Export capabilities**: Save results in various formats
5. **Plugin architecture**: Extend functionality

### Research Directions

1. **Neuromorphic computing**: Hardware acceleration
2. **Cognitive architectures**: Advanced learning algorithms
3. **Social learning**: Multi-agent coordination
4. **Transfer learning**: Knowledge sharing between brains

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Authors

NLM Project contributors. See README.md for more information.

## Acknowledgements

- NLM research team
- Pybind11 for Python bindings
- Open-source community
- Funding agencies

## Contact

For questions or support:
- GitHub issues: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
