# Python Bindings Improvements

## Overview

This document describes the improvements made to the Python bindings in `python/bindings.cpp` for the NLM (Neural Learning Machine) framework. The improvements focus on creating a more Pythonic, user-friendly API while maintaining C++ performance.

## Changes Summary

### 1. Core Types Enhancement

#### NeuronId, SynapseId, RegionId, PopulationId
- **Simplified constructors**: Direct exposure without unnecessary wrapper functions
- **Pythonic naming**: Added snake_case aliases
- **Comprehensive docstrings**: Detailed documentation with examples
- **Better operator support**: Added full comparison operators (`<`, `<=`, `>`, `>=`)
- **Improved string representation**: Better `__repr__` and `__str__` methods

### 2. Config Class Improvements

#### Pythonic API
- **snake_case aliases**: `load_from_file()`, `save_to_file()`, `key_exists()`, `keys()`, `items()`
- **Convenience methods**: `update()`, `merge()`, `validate()`
- **Type-safe getters**: Automatic type detection with `get()` method
- **Parameter validation**: File path validation with clear error messages
- **Rich error handling**: Python exceptions with descriptive messages

#### New Methods
- `get(key)` - Get value with automatic type conversion
- `get_or(key, default_value)` - Get with fallback to default
- `set(key, value)` - Set with Python type support
- `set_with_source(key, value, source)` - Set with explicit source
- `items()` - Get all configuration as (key, value) tuples
- `update(updates)` - Update from dictionary
- `merge(other_config)` - Merge another configuration
- `validate(schema)` - Validate against schema

### 3. Sensory Input Classes

#### Vision
- **Improved initialization**: Clear documentation of parameters
- **Pythonic method names**: `set_data()`, `get_width()`, `get_height()`, `get_channels()`

#### Audio
- **Enhanced interface**: Better documentation and parameter descriptions

#### InternalSignals
- **Simplified API**: Focus on essential functionality

### 4. Motor Output Classes

#### Action
- **Cleaner constructor overloads**: Multiple constructors for different use cases
- **Better type support**: Direct ActionType enum access
- **Pythonic method names**: `get_name()`, `clone()`

### 5. World Class

#### Enhanced Interface
- **Simplified return policies**: More consistent behavior
- **Better error messages**: Clear feedback for invalid operations
- **Pythonic naming**: `configure()`, `set_agent_start()`, etc.

### 6. Brain Class

#### Standardized Return Policies
- **Consistent memory management**: Explicit return value policies
- **Pythonic convenience methods**: Added snake_case aliases
- **Parameter validation**: Neuron ID and current value validation

### 7. AgentBrain Class

#### Pythonic Enhancements
- **Snake_case methods**: `enable_reward_modulation()`, `get_curiosity_level()`, etc.
- **Simplified interface**: Focus on essential functionality
- **Better documentation**: Clear parameter descriptions

### 8. Global Constants

#### Available Constants
- `INVALID_NEURON_ID`
- `INVALID_SYNAPSE_ID`
- `INVALID_REGION_ID`
- `INVALID_POPULATION_ID`

### 9. Factory Functions

#### Pythonic Aliases
- `create_config()` - Alias for `create_default_config`
- `create_neural_brain()` - Alias for `create_brain`
- `create_simulation_world()` - Alias for `create_simple_world`

## Key Improvements

### 1. Parameter Validation

Added comprehensive validation throughout the bindings:
- File path validation (existence, readability, regular file)
- Neuron ID validation (sentinel value check)
- Membrane potential range validation
- Type conversion safety

### 2. Error Handling

- **Clear error messages**: Descriptive Python exceptions
- **Type safety**: Automatic type conversion with validation
- **Input validation**: Comprehensive parameter checking

### 3. Documentation

- **Comprehensive docstrings**: Detailed examples for all methods
- **Pythonic documentation**: PEP 257 compliant docstrings
- **Usage examples**: Practical code examples in documentation

### 4. API Consistency

- **Snake_case convention**: Python naming standard
- **Consistent return types**: Predictable behavior
- **Standardized parameters**: Clear, documented parameters

### 5. Performance

- **Reduced overhead**: Removed unnecessary lambda wrappers
- **Efficient memory management**: Appropriate return value policies
- **Minimal overhead**: Direct C++ exposure where appropriate

## Usage Examples

### Basic Config Usage
```python
import pynlm

# Create configuration
config = pynlm.create_config()

# Set configuration values
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
config.set("enabled", True)

# Get values
learning_rate = config.get("learning_rate")
batch_size = config.get("batch_size")

# Use get_or with default
max_neurons = config.get_or("max_neurons", 100)

# Update from dictionary
config.update({"new_param": "value", "another_param": 42})

# Validate configuration
schema = {"learning_rate": "float", "batch_size": "int"}
if config.validate(schema):
    print("Configuration is valid")

# Save to file
config.save_to_file("config.json")

# Load from file
config2 = pynlm.create_config()
config2.load_from_file("config.json")
```

### Brain Usage
```python
import pynlm

# Create brain with configuration
config = pynlm.create_config()
config.set("brain_width", 100)
brain = pynlm.create_neural_brain(config)

# Initialize brain
brain.initialize()

# Create sensory input
vision = pynlm.Vision(84, 84, 3)

# Step simulation
brain.step(current_step)

# Get action
action = brain.produce_action()
```

### AgentBrain Usage
```python
import pynlm

# Create brain and agent brain
config = pynlm.create_config()
brain = pynlm.create_neural_brain(config)
agent_brain = pynlm.create_agent_brain(brain)

# Initialize with world
world = pynlm.create_simulation_world()
agent_brain.initialize(world)

# Process sensory input
percept = agent_brain.get_sensory_percept()
action = agent_brain.decode_motor_command()

# Apply reward modulation
agent_brain.apply_reward_modulation(reward, predicted_reward)
```

## Build Instructions

The Python bindings use scikit-build-core with pybind11. To build and install:

```bash
pip install -e .
```

Or to build in development mode:

```bash
cd python
python -m pip install -e .
```

## Testing

Run the test script to verify the improvements:

```bash
python test_bindings_improvements.py
```

The test script verifies all the new functionality and improvements.

## Migration Guide

For users of the previous version, the following changes are introduced:

### Renamed Methods (snake_case)

| Old Name | New Name |
|----------|----------|
| loadFromFile | load_from_file |
| saveToFile | save_to_file |
| has | key_exists |
| getKeys | keys |
| loadFromArgs | load_from_args |

### New Methods

- `items()` - Get all configuration as tuples
- `update()` - Update from dictionary
- `merge()` - Merge another configuration
- `validate()` - Validate configuration
- `get_or()` - Get with default fallback
- `set_with_source()` - Set with explicit source

### Enhanced Methods

- `get()` - Now returns Python types (int, float, str, bool) instead of C++ types
- `save_to_file()` and `load_from_file()` - Added parameter validation
- `validate()` - New validation method for configuration

All existing functionality is preserved, but the API is now more Pythonic and user-friendly.

## Testing

Run comprehensive tests to verify the improvements:

```bash
python test_bindings_improvements.py
```

This test script verifies:
- Module import and basic functionality
- Config class improvements (items, update, merge, validate)
- File operations with parameter validation
- Configuration validation
- NeuronId functionality
- World creation and configuration

## Files Modified

1. `python/bindings.cpp` - Complete rewrite with Pythonic API and comprehensive documentation
2. `test_bindings_improvements.py` - Test script for verifying improvements

## Future Improvements

Planned enhancements for future versions:
1. Additional convenience methods for common operations
2. Better integration with popular Python libraries (NumPy, Pandas)
3. More sophisticated validation schemas
4. Performance profiling and optimization
5. Additional example notebooks and tutorials

## Conclusion

The improved Python bindings provide a more Pythonic, user-friendly API while maintaining C++ performance. The changes include:

- Comprehensive parameter validation and error handling
- Pythonic naming conventions and snake_case methods
- Rich documentation with practical examples
- Enhanced Config class with dictionary-like interface
- Better separation of concerns and logical organization
- Maintained backward compatibility where possible

These improvements make the NLM framework more accessible to Python users while preserving the high performance and capabilities of the C++ core.