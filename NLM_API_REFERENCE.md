# NLM - Enhanced NLM Python Bindings

## Overview

The enhanced NLM Python bindings provide better type safety, error handling, and Pythonic interfaces compared to the basic pynlm bindings. This document describes the enhanced bindings for the NLM (Neural Learning Machine) project, which build upon the existing pynlm bindings while adding significant improvements.

## Overview

The enhanced bindings build upon the existing pynlm bindings while adding significant improvements:

### Key Improvements

1. **Enhanced Type Safety**
   - Automatic type conversion between Python and C++
   - Union type (NLMType) for flexible value storage
   - Runtime type checking with helpful error messages
   - Immutable type aliases for better IDE support

2. **Better Error Handling**
   - Custom exception hierarchy with specific error types
   - Contextual error messages with helpful suggestions
   - Graceful handling of edge cases and invalid inputs
   - Detailed error reporting with stack traces

3. **Pythonic Interfaces**
   - Context managers for resource management
   - Iterator support for collections
   - Property access with getters/setters
   - Rich comparison operators
   - Pythonic method names

4. **Enhanced Functionality**
   - Convenience methods for common operations
   - Factory methods for common use cases
   - Batch operation methods
   - Statistical analysis methods
   - Export/import utilities

5. **Comprehensive Validation**
   - Parameter validation and range checking
   - Schema validation for configurations
   - Type safety with automatic conversions

## Module Structure

```
python/
├── enhanced_bindings.cpp              # Enhanced C++ bindings
├── CMakeLists.txt                      # CMake configuration
└── README.md                           # Documentation
```

## Installation

### Prerequisites

- C++ compiler with C++17 support
- pybind11 library
- CMake 3.10 or higher

### Building

```bash
mkdir build
cd build
cmake ..
make -j4
```

### Python Usage

```python
# Import enhanced bindings
from nlm_enhanced import *

# Create enhanced brain with configuration
config = createDefaultConfig()
brain = createBrain(config)

# Enhanced simulation with error handling
try:
    brain.initialize()
    
    for step in range(1000):
        time = step * 0.01
        
        # Pythonic step control with type safety
        simulation_controller.step(step, time)
        
        # Perform simulation step
        brain.step(step, time)
        
        # Get statistics with automatic type conversion
        stats = brain.getStatistics()
        
except BrainError as e:
    print(f"Brain error: {e}")
except ConfigurationError as e:
    print(f"Configuration error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

## Enhanced Classes

### EnhancedConfig

Enhanced configuration class with:

- **Automatic type conversion**: Get/set values with automatic type conversion
- **Schema validation**: Validate configurations against schemas
- **Context manager support**: Use with `with` statements
- **Iterator support**: Iterate over configuration key-value pairs
- **Batch operations**: Set multiple values at once
- **Convenient methods**: `get()`, `set()`, `validate()`, `save()`

```python
# Create enhanced config
config = createDefaultConfig()

# Pythonic access with automatic type conversion
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
config.set("use_gpu", True)

# Get values with automatic type conversion
learning_rate = config["learning_rate"]  # Returns float
batch_size = config["batch_size"]        # Returns int
use_gpu = config["use_gpu"]              # Returns bool

# Validation
if config.validate():
    print("Configuration is valid")

# Iteration
for key, value in config.items():
    print(f"{key}: {value}")

# Context manager support
with createDefaultConfig() as config:
    config.set("temp_param", "value")
```

### EnhancedBrain

Enhanced brain class with:

- **Type-safe methods**: All methods return appropriately typed values
- **Error handling**: Graceful error handling with detailed messages
- **Context manager support**: Safe resource management
- **Statistics**: Comprehensive brain statistics
- **Convenience methods**: Easy-to-use methods for common operations

```python
# Create enhanced brain
brain = createBrain(config)

# Enhanced simulation with error handling
try:
    brain.initialize()
    
    for step in range(1000):
        time = step * 0.01
        
        # Pythonic step control
        simulation_controller.step(step, time)
        
        # Perform simulation step
        brain.step(step, time)
        
        # Get statistics
        stats = brain.getStatistics()
        
except BrainError as e:
    print(f"Simulation error: {e}")
```

### EnhancedAgentBrain

Enhanced agent brain class with:

- **Pythonic interfaces**: Context managers and property access
- **Feature control**: Enable/disable features with error checking
- **Neuromodulation**: Reward-based neuromodulation with type safety
- **Development tracking**: Developmental stage tracking with automatic updates

```python
# Create enhanced agent brain
agent_brain = createAgentBrain(brain)

# Pythonic feature control
agent_brain.enableFeature("curiosity")
agent_brain.disableFeature("structural_plasticity")

# Neuromodulation
agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.8)

# Get feature status
features = agent_brain.getFeatureStatus()
```

### EnhancedSimpleWorld

Enhanced world class with:

- **Pythonic iteration**: Easy iteration over world objects
- **Context manager support**: Safe world management
- **Type-safe methods**: All methods return appropriately typed values
- **Error handling**: Graceful error handling for invalid operations

## Type System

### NLMType

The enhanced bindings use a union type (`NLMType`) for flexible value storage:

```python
from nlm_enhanced import NLMType

# Create NLMType values
null_value = NLMType()  # Null value
int_value = NLMType(42)  # Integer
float_value = NLMType(3.14)  # Float
string_value = NLMType("hello")  # String
bool_value = NLMType(True)  # Boolean

# Type checking
if null_value.is_null():
    print("Value is null")

if int_value.is_int():
    print(f"Value is int: {int_value.as_int()}")

# Type conversion
value = NLMType(42)
converted = value.as_float()  # Convert to float
```

### Type Conversion Utilities

The enhanced bindings provide comprehensive type conversion utilities:

```python
from nlm_enhanced import *

# Convert Python to NLMType
py_value = [1, 2, 3, 4, 5]
nlm_value = py_to_nlm(py_value)

# Convert NLMType to Python
back_to_py = nlm_to_py(nlm_value)

# Type validation
validate_type("hello", "parameter_name")
validate_type(42, "learning_rate")
```

## Error Handling

### Exception Hierarchy

The enhanced bindings provide a comprehensive exception hierarchy:

- **NLMException**: Base exception for all NLM errors
- **ConfigurationError**: Errors related to configuration
- **BrainError**: Errors related to brain simulation
- **AgentError**: Errors related to agent brains
- **WorldError**: Errors related to world simulation

```python
from nlm_enhanced import *

try:
    config = createDefaultConfig()
    config.set("invalid_param", "value")
    
    # This will raise ConfigurationError
    if not config.validate():
        raise ConfigurationError("Invalid configuration")
        
except NLMException as e:
    print(f"NLM Error: {e}")
    print(f"Error type: {e.type()}")
    print(f"Error details: {get_error_details(e)}")
```

### Error Details

Each exception provides detailed error information:

```python
try:
    brain.step(step, time)
except BrainError as e:
    print(f"Error type: {get_error_type(e)}")
    print(f"Error details: {get_error_details(e)}")
    log_error(e)
```

## Utility Functions

### Type Conversion Utilities

- `py_to_nlm(py_value)`: Convert Python value to NLMType
- `nlm_to_py(nlm_value)`: Convert NLMType to Python value
- `validate_type(value, param_name)`: Validate value type
- `in_range(value, min, max)`: Check if value is in range

### String Utilities

- `trim(str)`: Trim whitespace from string
- `starts_with(str, prefix)`: Check if string starts with prefix
- `to_lower(str)`: Convert string to lowercase
- `generate_timestamp()`: Generate timestamp string
- `generate_uuid()`: Generate UUID string
- `escape_json(str)`: Escape JSON string

### Statistical Utilities

- `mean(values)`: Calculate mean of values
- `stddev(values)`: Calculate standard deviation
- `median(values)`: Calculate median

### File Utilities

- `file_exists(path)`: Check if file exists
- `get_file_extension(path)`: Get file extension
- `get_current_directory()`: Get current directory

## Constants

### Enhanced Constants

```python
from nlm_enhanced import *

# NLM Type constants
NLM_TYPE_NULL = NLMType()
NLM_TYPE_INT = 1
NLM_TYPE_INT64 = 2
NLM_TYPE_FLOAT = 3
NLM_TYPE_BOOL = 4
NLM_TYPE_STRING = 5
NLM_TYPE_INT_VECTOR = 6
NLM_TYPE_FLOAT_VECTOR = 7
NLM_TYPE_STRING_VECTOR = 8

# Enhanced version and configuration constants
ENHANCED_VERSION = "2.0.0"
MAX_CONFIG_SIZE = 1000000
DEFAULT_INDENT = 2
DEFAULT_SIMULATION_STEPS = 1000
```

## Migration Guide

### From Basic pynlm to Enhanced Bindings

#### Configuration

**Basic pynlm:**
```python
from pynlm import Config, createDefaultConfig

config = createDefaultConfig()
config.loadFromFile("config.json")
config.set("learning_rate", 0.01)
value = config["learning_rate"]
```

**Enhanced bindings:**
```python
from nlm_enhanced import *

config = createDefaultConfig()
config.loadFromFile("config.json")
config.set("learning_rate", 0.01)  # Automatic type conversion
value = config["learning_rate"]      # Returns appropriate type
```

#### Brain

**Basic pynlm:**
```python
from pynlm import createBrain, createDefaultConfig

brain = createBrain(createDefaultConfig())
brain.initialize()
brain.step(0, 0.0)
active_neurons = brain.getActiveNeuronCount()
```

**Enhanced bindings:**
```python
from nlm_enhanced import *

brain = createBrain(createDefaultConfig())

# Enhanced simulation with error handling
try:
    brain.initialize()
    
    for step in range(1000):
        time = step * 0.01
        brain.step(step, time)
        
except BrainError as e:
    print(f"Error: {e}")
```

#### AgentBrain

**Basic pynlm:**
```python
from pynlm import createAgentBrain

agent_brain = createAgentBrain(brain)
agent_brain.initialize(world)
```

**Enhanced bindings:**
```python
from nlm_enhanced import *

agent_brain = createAgentBrain(brain)

# Enhanced feature control
agent_brain.enableFeature("curiosity")
agent_brain.disableFeature("structural_plasticity")

# Enhanced neuromodulation
agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

## Backward Compatibility

The enhanced bindings maintain backward compatibility with the basic pynlm bindings:

- **Same API**: Core classes and methods have the same names
- **Same behavior**: Core functionality behaves identically
- **Enhanced features**: New features are additive, not breaking
- **Graceful degradation**: If enhanced features fail, basic functionality still works

## Performance Considerations

### Memory Usage

- Enhanced type conversion may use slightly more memory
- Automatic type checking adds minimal overhead
- Memory usage is similar to basic bindings for typical use cases

### Execution Speed

- Enhanced error handling adds minimal overhead
- Type conversion is optimized for common cases
- Performance impact is negligible for most applications

## Testing and Validation

### Unit Tests

The enhanced bindings should be tested with:

1. **Type conversion tests**: Verify automatic type conversion
2. **Error handling tests**: Verify error handling with various inputs
3. **Integration tests**: Verify compatibility with existing pynlm
4. **Performance tests**: Verify performance characteristics
5. **Edge case tests**: Verify behavior with edge cases

### Test Example

```python
import pytest
from nlm_enhanced import *

def test_type_conversion():
    """Test automatic type conversion"""
    config = createDefaultConfig()
    config.set("int_value", 42)
    config.set("float_value", 3.14)
    config.set("string_value", "hello")
    config.set("bool_value", True)
    
    # Verify type conversion
    assert config["int_value"] == 42
    assert config["float_value"] == 3.14
    assert config["string_value"] == "hello"
    assert config["bool_value"] is True

def test_error_handling():
    """Test error handling"""
    config = createDefaultConfig()
    
    # Test validation error
    config.set("invalid_param", "value")
    assert not config.validate()
    
    # Test type conversion error
    with pytest.raises(ConfigurationError):
        # This should raise an error
        pass

def test_enhanced_features():
    """Test enhanced features"""
    brain = createBrain(createDefaultConfig())
    brain.initialize()
    
    # Test statistics
    stats = brain.getStatistics()
    assert "active_neurons" in stats
    assert "firing_rate" in stats
    
    # Test context manager
    with brain:
        brain.step(0, 0.0)
    
    print("All tests passed!")
```

## Future Enhancements

### Planned Improvements

1. **Additional Exception Types**: More specific exception types for different error scenarios
2. **Enhanced Validation**: More sophisticated validation rules and schemas
3. **Performance Monitoring**: Built-in performance monitoring and profiling
4. **Logging Support**: Comprehensive logging support
5. **Async Support**: Asynchronous method support for non-blocking operations
6. **Documentation Generator**: Automatic documentation generation
7. **Test Framework Integration**: Integration with testing frameworks
8. **Performance Optimization**: Further performance optimizations

### Research Areas

1. **Machine Learning Integration**: Integration with machine learning libraries
2. **Distributed Computing**: Support for distributed simulations
3. **GPU Acceleration**: GPU-accelerated operations
4. **Real-time Processing**: Real-time processing capabilities
5. **Cloud Integration**: Integration with cloud platforms

## Conclusion

The enhanced NLM Python bindings provide significant improvements over the basic pynlm bindings:

- **Better type safety**: Automatic type conversion and runtime type checking
- **Enhanced error handling**: Comprehensive error handling with detailed messages
- **Pythonic interfaces**: Context managers, iterators, and properties
- **Advanced functionality**: Convenience methods and utility functions
- **Comprehensive validation**: Parameter validation and schema checking
- **Backward compatibility**: Seamless integration with existing code

These enhancements make the NLM Python bindings more robust, user-friendly, and powerful while maintaining backward compatibility with existing code.