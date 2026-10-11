# Enhanced NLM Python Package

## Overview

This document describes the enhancements made to the NLM (Neural Learning Machine) Python bindings.
The package now provides a **Pythonic API** while maintaining full backward compatibility
with the original `pynlm` module.

## Key Improvements

### 1. Python Package Structure

**Before**: Single `bindings.cpp` file with pybind11 bindings

**After**: Complete Python package with:
- `python/__init__.py` - Main package entry point
- `python/core/` - Core functionality package
- `python/core/exceptions.py` - Custom exception classes
- `python/core/config.py` - Enhanced configuration management
- `python/core/brain.py` - Enhanced brain wrapper
- `python/core/world.py` - Enhanced world wrapper
- `python/core/sensor_helper.py` - Sensor data processing helpers
- `python/core/factories.py` - Factory methods for common use cases

### 2. Custom Exception Classes

**Before**: Basic C++ exceptions only

**After**: Comprehensive Python exception hierarchy:
- `NLMError` (base class)
- `ConfigError`, `BrainError`, `WorldError`
- `SensorError`, `AgentError`
- `ValidationError`, `MemoryError`, `SimulationError`, `DevelopmentalError`

**Benefits**:
- Specific error handling for different error types
- Better error messages and debugging
- Ability to catch specific exception types

### 3. Enhanced Configuration Management

**Before**: Basic configuration with file I/O only

**After**: Pythonic configuration wrapper with:
- Dictionary-style access: `config["key"] = value`
- Type-safe access: `config.get_int("key")`, `config.get_float("key")`
- Validation: `config.validate_required_keys(["key1", "key2"])`
- Convenience methods: `config.items()`, `config.keys()`, `config.values()`
- Factory methods: `create_config_from_dict()`, `create_config_with_validation()`

### 4. Enhanced Brain Wrapper

**Before**: Basic brain with minimal Python interface

**After**: Enhanced brain with:
- Context manager support for resource management
- Convenience methods: `get_stats()`, `step_context()`
- Type-safe access to brain properties
- Better error handling with specific exception types
- Statistics collection and monitoring

### 5. Enhanced World Wrapper

**Before**: Basic world simulation

**After**: Enhanced world with:
- Convenience methods: `distance_to_target()`, `move_agent()`, `is_agent_moving()`
- Statistics collection
- Validation and safety checks
- Context manager support
- Better error handling

### 6. Sensor Helper

**Before**: No sensor data processing helpers

**After**: Comprehensive sensor data processing:
- Data normalization for different sensory modalities
- Conversion to numpy arrays for analysis
- Feature extraction from sensory data
- Edge detection and filtering
- Statistics collection

### 7. Factory Methods

**Before**: Manual object creation

**After**: Factory methods for common use cases:
- `NLMFactory.create_brain_with_validation()`
- `NLMFactory.create_world_with_agents()`
- `NLMFactory.create_agent_simulation()`
- `NLMFactory.create_experiment_config()`
- `NLMFactory.create_training_config()`

### 8. Pythonic API Design

**Benefits**:
- **Context Managers**: Proper resource management with `with` statements
- **Type Safety**: Type hints and validation
- **Error Handling**: Specific exceptions for different error types
- **Convenience Methods**: Shorter, more intuitive method names
- **Dictionary Access**: Pythonic key-value access patterns
- **Documentation**: Comprehensive docstrings and examples

### 9. Backward Compatibility

**The enhanced package maintains full backward compatibility**:

**Original API still works**:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

**New API available**:
```python
from pynlm.core import (
    create_brain_with_config, create_config_from_dict,
    BrainWrapper, WorldWrapper, ConfigWrapper, SensorHelper
)

config = create_config_from_dict({"brain.neuron_count": 1000})
brain_wrapper = create_brain_with_config(config)
```

### 10. Comprehensive Documentation

**Before**: Basic docstrings in C++

**After**: Complete documentation with:
- Detailed docstrings for all classes and methods
- Usage examples for every feature
- API reference documentation
- Best practices and design patterns

## Installation

### From Source

```bash
# Build the C++ extensions
pip install .

# Or in development mode
pip install -e .
```

### Using the Enhanced API

```python
# Import the enhanced package
import pynlm

# Access core functionality
from pynlm.core import (
    ConfigWrapper, BrainWrapper, WorldWrapper,
    SensorHelper, NLMFactory, create_default_config
)

# Use enhanced API
config = create_default_config()
config["brain.neuron_count"] = 1000
brain_wrapper = BrainWrapper(pynlm.createBrain(config))
```

## Usage Examples

### Basic Configuration

```python
from pynlm.core import create_config_from_dict

# Create configuration from dictionary
config_dict = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "world.width": 100.0,
    "world.height": 100.0,
}

config = create_config_from_dict(config_dict)
print(f"Config has {len(config.keys())} settings")

# Dictionary-style access
print(config["brain.neuron_count"])  # 1000

# Type-safe access
neuron_count = config.get_int("brain.neuron_count")
```

### Enhanced Brain Usage

```python
from pynlm.core import create_brain_with_config, brain_context

# Create brain with configuration
brain_config = {
    "brain.neuron_count": 500,
    "development.enabled": True,
}

brain_wrapper = create_brain_with_config(brain_config)
stats = brain_wrapper.get_stats()
print(f"Brain has {stats['neuron_count']} neurons")

# Use context manager for resource management
with brain_context(brain_wrapper) as brain:
    for step in range(100):
        brain.step(step)
```

### Enhanced World Usage

```python
from pynlm.core import NLMFactory

# Create world using factory
world_wrapper = NLMFactory.create_world_with_agents(
    width=200,
    height=200,
    vision_width=30,
    vision_height=30,
    agent_start=(100.0, 100.0),
)

# Use convenience methods
print(f"World size: {world_wrapper.get_width()}x{world_wrapper.get_height()}")

# Calculate distance to target
distance = world_wrapper.distance_to_target(150.0, 150.0)
print(f"Distance to target: {distance:.2f}")
```

### Complete Simulation

```python
from pynlm.core import NLMFactory, run_full_episode

# Create complete simulation
brain_wrapper, world_wrapper = NLMFactory.create_agent_simulation(
    brain_config={
        "brain.neuron_count": 800,
        "development.enabled": True,
        "neuromodulation.reward.enable": True,
    },
    world_config={
        "width": 150,
        "height": 150,
        "vision_width": 25,
        "vision_height": 25,
    }
)

# Run simulation
results = run_full_episode(brain_wrapper, world_wrapper, steps=100)
print(f"Simulation completed: {results['steps_completed']} steps")
```

### Error Handling

```python
from pynlm.core import ConfigError, ValidationError, NLMError

try:
    config = create_config_from_dict({"invalid.key": "value"})
    config.validate_required_keys(["required.key"])
except ConfigError as e:
    print(f"Configuration error: {e}")
except ValidationError as e:
    print(f"Validation error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

### Sensor Data Processing

```python
from pynlm.core import SensorHelper

helper = SensorHelper()

# Normalize sensory data
normalized = helper.normalize_sensory_data(percept)

# Convert to numpy for analysis
numpy_data = helper.convert_to_numpy(vision)

# Extract features
features = helper.extract_features(vision, window_size=5)
```

## Error Handling

The enhanced package provides specific exception types for different error conditions:

```python
try:
    # Code that might fail
    config["brain.neuron_count"] = -1
except ValidationError as e:
    print(f"Validation failed: {e}")
except ConfigError as e:
    print(f"Configuration error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

## Performance Considerations

### Memory Management

- Context managers ensure proper cleanup
- Enhanced configuration avoids unnecessary object creation
- Sensor helpers provide efficient data processing

### Type Safety

- Type-safe accessors prevent runtime errors
- Validation catches issues early
- Better performance through predictable data structures

### Resource Management

- Context managers prevent resource leaks
- Factory methods provide consistent resource allocation
- Proper cleanup in all scenarios

## Migration Guide

### From Old API to New API

**Old way**:
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

**New way**:
```python
from pynlm.core import create_config_from_dict, create_brain_with_config, brain_context

config = create_config_from_dict({"brain.neuron_count": 1000})
brain_wrapper = create_brain_with_config(config)

with brain_context(brain_wrapper) as brain:
    for step in range(100):
        brain.step(step)
```

### When to Use Enhanced API

**Use enhanced API when**:
- You need better error handling
- You want Pythonic convenience methods
- You need context managers for resource management
- You want type-safe accessors
- You need validation and error checking
- You want to create multiple objects with similar configurations

**Use original API when**:
- You need direct access to C++ objects
- You're writing low-level code
- You need maximum performance
- You're maintaining existing code

## Testing

Run the comprehensive test suite:

```bash
python test_enhanced_package.py
```

Run the usage examples:

```bash
python examples/enhanced_api_examples.py
```

## Dependencies

### Required Dependencies

- Python 3.8+
- NumPy (optional, for sensor data processing)
- scikit-build-core >= 0.5.0
- pybind11 >= 2.11.0

### Optional Dependencies

- NumPy (for sensor data conversion)
- matplotlib (for visualization)
- pandas (for data analysis)

## Future Enhancements

### Planned Features

1. **Async Support**: Asynchronous simulation for better performance
2. **Caching**: Intelligent caching for expensive operations
3. **Serialization**: Enhanced serialization for saving/loading simulations
4. **Monitoring**: Integrated monitoring and metrics collection
5. **Distributed Computing**: Support for distributed simulations
6. **Visualization**: Built-in visualization tools

### Extension Points

1. **Plugin System**: Extensible plugin architecture
2. **Custom Sensors**: Support for custom sensor types
3. **Advanced Brain Models**: Support for advanced brain architectures
4. **Custom Worlds**: Support for custom world implementations

## Support

For issues, questions, or feature requests:

- File issues in the GitHub repository
- Join the NLM community forums
- Check the documentation for API references
- Report bugs with detailed error messages

## License

This package is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

- Original NLM project authors and contributors
- pybind11 team for excellent Python bindings
- scikit-build-core team for modern Python packaging
- All contributors who provided feedback and suggestions

---

**Version**: 0.1.0  
**Last Updated**: 2026-10-11  
**License**: MIT