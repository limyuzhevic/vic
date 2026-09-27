# NLM Python Enhanced API Documentation

This document provides comprehensive information about the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The NLM Python bindings provide access to the C++ neural simulation framework through an enhanced Python API. This improved version includes:

- **Pythonic API Design**: Intuitive method names and parameter handling
- **Enhanced Error Handling**: Custom exceptions and comprehensive validation
- **Simulation Helpers**: Convenience methods for common operations
- **Configuration Management**: Type-safe configuration with JSON support
- **Advanced Features**: Context managers, data export, and visualization helpers
- **Machine Learning Integration**: Tools for converting NLM data to ML formats

## Installation

The enhanced NLM Python bindings are built using scikit-build-core. The package includes:

```bash
pip install pynlm
```

For development or to build from source:

```bash
pip install -e .
# or
pip install scikit-build-core pybind11
python -m pip install -e .
```

## Quick Start

### Basic Usage

```python
import pynlm as nlm
import numpy as np

# Create configuration
config = nlm.Config.create_default()
config.set_with_validation("brain.neuronCount", 1000)
config.set_with_validation("simulation.dt", 0.1)

# Create brain and world
brain = nlm.Brain(config)
world = nlm.SimpleWorld()
world.configure(100, 100, 20, 20)

# Initialize
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)
    world.update()

# Get statistics
stats = brain.get_statistics()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
```

### Advanced Usage

```python
# Create sensory input from numpy array
vision_array = np.random.rand(64, 64, 3).astype(np.float32)
vision = nlm.Vision.from_array(vision_array)

# Create action
action = nlm.Action(nlm.ActionType.MoveForward, [1.0, 0.0])

# Process sensory input and get action
percept = nlm.SensoryPercept()
percept.set_vision(vision.to_array())

# Get brain statistics with enhanced API
brain = nlm.Brain(config)
stats = brain.get_statistics()
detailed_stats = {
    "neurons": stats["total_neurons"],
    "synapses": stats["total_synapses"],
    "firing_rate": stats["avg_firing_rate"],
    "prediction_error": stats["prediction_error"]
}
```

## API Reference

### Core Classes

#### Config
Configuration management with validation and JSON support.

**Methods:**
- `create_default()` - Create default configuration
- `load_from_dict(config_dict)` - Load from Python dictionary
- `load_from_json_string(json_str)` - Load from JSON string
- `save_to_json()` - Save to JSON string
- `get_value(key, default=None)` - Get value with type safety
- `set_with_validation(key, value)` - Set with validation
- `as_dict()` - Convert to Python dictionary
- `get_keys()` - Get all configuration keys
- `has(key)` - Check if key exists

**Example:**
```python
config = nlm.Config.create_default()
config.set_with_validation("brain.neuronCount", 1000)
neuron_count = config.get_value("brain.neuronCount", 500)
```

#### Vision
Vision sensory input with numpy-compatible interface.

**Methods:**
- `from_array(array)` - Create from numpy array
- `from_nested_list(nested_list)` - Create from nested Python list
- `get_width()`, `get_height()`, `get_channels()` - Get dimensions
- `to_array()` - Convert to numpy array

**Example:**
```python
vision = nlm.Vision.from_array(np.random.rand(64, 64, 3))
print(f"Vision: {vision.get_width()}x{vision.get_height()}x{vision.get_channels()}")
```

#### Audio
Audio sensory input with sample rate and duration information.

**Methods:**
- `from_array(array, sample_rate=44100)` - Create from numpy array
- `get_sample_rate()`, `get_num_samples()` - Get audio properties
- `to_array()` - Convert to numpy array
- `get_metadata()` - Get audio metadata

**Example:**
```python
audio = nlm.Audio.from_array(np.random.randn(44100), sample_rate=44100)
print(f"Audio duration: {audio.get_num_samples()/audio.get_sample_rate():.2f}s")
```

#### Action
Action representation with type and parameter support.

**Methods:**
- `Action(type)` or `Action(type, parameters)` - Create action
- `get_type()`, `get_name()` - Get action properties
- `get_parameters()`, `set_parameters(params)` - Parameter management
- `to_dict()`, `from_dict(dict)` - Dictionary conversion
- `clone()` - Create copy

**Example:**
```python
action = nlm.Action(nlm.ActionType.MoveForward, [1.0, 0.0])
action_dict = action.to_dict()
cloned_action = action.clone()
```

#### SensoryPercept
Container for all sensory modalities.

**Methods:**
- `get_vision()`, `set_vision(data)` - Vision data
- `get_audio()`, `set_audio(data)` - Audio data
- `get_internal()`, `set_internal(data)` - Internal signals
- `get_touch()`, `set_touch(data)` - Touch data
- `get_proprioception()`, `set_proprioception(data)` - Proprioception
- `get_all_data()` - Get all data as dictionary
- `from_dict(data)` - Create from dictionary

**Example:**
```python
percept = nlm.SensoryPercept()
percept.set_vision(vision.to_array())
percept.set_audio(audio.to_array())
all_data = percept.get_all_data()
```

#### SimpleWorld
2D simulation environment.

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world
- `update(timestep=0.1)` - Update simulation
- `get_agent_body()` - Get agent state
- `get_sensory_percept()` - Get sensory input
- `apply_action(action)` - Apply action
- `add_world_object(x, y, type, value, radius)` - Add object
- `is_valid_position(x, y)` - Check position validity
- `get_dimensions()` - Get world dimensions
- `reset()` - Reset world

**Example:**
```python
world = nlm.SimpleWorld()
world.configure(200, 200, 32, 32)
world.add_world_object(50, 50, nlm.WorldObjectType.Resource, value=10)
body = world.get_agent_body()
```

#### AgentBrain
Interface between NLM brain and simulation world.

**Methods:**
- `initialize(world)` - Initialize with world
- `process_sensory_input(percept)` - Process sensory input
- `decode_motor_command()` - Decode action from neural activity
- `get_sensory_input_size()`, `get_motor_output_size()` - Get sizes
- `apply_reward_modulation(reward, predicted_reward)` - Apply reward
- `update_development(timestep)` - Update development system
- `get_neuromodulation_levels()` - Get neuromodulation state
- `get_developmental_stage()` - Get current developmental stage
- Configuration toggles: `enable_reward_modulation()`, `enable_curiosity()`, etc.

**Example:**
```python
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)
agent_brain.enable_reward_modulation(True)
agent_brain.enable_curiosity(True)
```

#### Brain
Main neural simulation brain with integrated subsystems.

**Methods:**
- `initialize()` - Initialize brain
- `step(current_step)` or `step_with_time(current_step, current_time)` - Simulation steps
- `receive_sensory_input(input)` - Receive sensory input
- `produce_action()` - Produce motor action
- `reset()` - Reset brain state
- `get_statistics()` - Get comprehensive statistics
- `get_prediction_error()` - Get prediction error
- `develop_stage(stage)` - Set developmental stage
- Configuration access and region management

**Example:**
```python
brain = nlm.Brain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
    stats = brain.get_statistics()
    print(f"Step {step}: {stats['total_spikes']} spikes")
```

### Advanced Features

#### Simulation Context Manager

```python
# Create simulation context
context = brain.create_simulation_context(start_step=0, end_step=100)

# Run simulation
context.run()

# Run and collect statistics
stats = context.run_and_collect()
```

#### High-Level Experiment Helpers

```python
# Run complete episode
episode_data = nlm.run_episode(brain, world, agent_brain, max_steps=1000)

# Create standard configuration
config = nlm.create_standard_config()
```

#### Error Handling

The enhanced API provides custom exceptions:

- `NLMException` - Base exception for all NLM errors
- `ConfigurationError` - Configuration-related errors
- `SimulationError` - Simulation-related errors
- `BrainError` - Brain-specific errors

**Example:**
```python
try:
    config.set_with_validation("invalid.key", "not_a_number")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

### Machine Learning Integration

The API includes utilities for converting NLM data to machine learning formats:

```python
# Prepare ML dataset from experiment
ml_data = prepare_ml_dataset(logger)

# Access features for ML training
observations = ml_data['observations']
targets = ml_data['targets']
```

### Visualization and Data Export

The API provides tools for visualization and data export:

```python
# Export brain statistics
exporter.export_brain_statistics(brain, "my_brain_stats")

# Export simulation data
sim_data = exporter.export_simulation_data(world, agent_brain)

# Create visualization script
viz_script = exporter.create_visualization_script(sim_data, "viz.py")
```

## Examples

### Example 1: Basic Simulation

See `examples/basic_usage.py` for a complete example of basic NLM usage.

### Example 2: Advanced Usage

See `examples/advanced_usage.py` for advanced features including batch processing and machine learning integration.

### Example 3: Complete Demonstration

See `examples/comprehensive_example.py` for a complete demonstration of the enhanced API.

## Performance Considerations

### Memory Management

The enhanced bindings maintain Python object ownership patterns for:
- Automatic memory management of C++ objects
- Efficient data transfer between Python and C++
- Prevention of memory leaks and dangling pointers

### Performance Optimizations

- **Batch Operations**: Methods for processing multiple items efficiently
- **Vectorized Operations**: Support for numpy array operations
- **Context Managers**: Automatic resource cleanup
- **Caching**: Intelligent caching of computed values

### Profiling and Monitoring

The API includes utilities for performance monitoring:

```python
from nlm.profiling import PerformanceMonitor

monitor = PerformanceMonitor("Brain Operations")
result = monitor.time_operation("Brain initialization", brain.initialize)
report = monitor.get_report()
```

## Compatibility

### Python Version Support

- Python 3.8+
- NumPy 1.20+

### C++ Compiler Requirements

- GCC 7+ or Clang 5+
- C++20 standard
- pybind11 2.11+

### Dependencies

The enhanced NLM Python bindings depend on:

- **Core**: NLM C++ framework (nlm_core, nlm_agent, nlm_world)
- **Python**: pybind11 for bindings
- **Build**: scikit-build-core for package management
- **Optional**: NumPy for data processing

## Migration Guide

### From Original C++ Bindings

The enhanced API provides several improvements over the original C++ bindings:

| Feature | Original | Enhanced |
|---------|----------|----------|
| Method Names | C++-style (`getNeuronCount`) | Pythonic (`get_neuron_count`) |
| Error Handling | Basic C++ exceptions | Python exceptions with custom types |
| Configuration | Manual parameter setting | Type-safe `set_with_validation()` |
| Data Access | Direct C++ access | Python-compatible interfaces |
| Documentation | Minimal | Comprehensive docstrings and examples |

### Breaking Changes

There are no breaking changes - the enhanced API maintains backward compatibility while adding new functionality.

## Troubleshooting

### Common Issues

#### Module Not Found

```bash
pip install pynlm
```

#### Build Errors

If encountering build errors:

```bash
# Clean and rebuild
rm -rf build/
cmake -B build -S .
make -C build
```

#### Import Errors

```python
# Check installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Getting Help

- **Documentation**: See the examples directory for detailed usage
- **GitHub**: Project repository and issue tracker
- **Community**: NLM research community forums

## Contributing

### Development

To contribute to the enhanced NLM Python bindings:

1. Fork the repository
2. Create a feature branch
3. Implement changes following the enhanced API patterns
4. Add comprehensive examples and documentation
5. Run existing tests
6. Submit pull request

### Testing

Run the test suite:

```bash
pytest tests/
```

The enhanced bindings include:
- Unit tests for new Pythonic APIs
- Integration tests for simulation workflows
- Performance tests for benchmarking
- Compatibility tests for different Python versions

## Acknowledgments

The enhanced NLM Python API builds upon the original C++ framework and includes contributions from:

- NLM Research Team
- Python bindings developers
- Community contributors
- Documentation writers

## License

MIT License - See LICENSE file for details.

---

*This documentation is automatically generated from the enhanced NLM Python bindings source code.*
