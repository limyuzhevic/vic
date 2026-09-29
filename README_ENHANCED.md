# NLM Enhanced Python Bindings

Comprehensive type conversions and Python-idiomatic interfaces for NLM Python bindings.

![Documentation](https://img.shields.io/badge/Documentation-Complete-green)
![Features](https://img.shields.io/badge/Features-Comprehensive-blue)
![Performance](https://img.shields.io/badge/Performance-Optimized-orange)
![Compatibility](https://img.shields.io/badge/Compatibility-Backward-yellow)

## Overview

The NLM Python bindings have been comprehensively enhanced to provide:

1. **Type Conversion Utilities**: Automatic conversions between C++ and Python types
2. **NumPy Array Support**: Support for vision/audio data with efficient array operations
3. **Enum Conversions**: Bidirectional conversion between C++ enums and Python enums/strings
4. **Dictionary-based Configuration**: Pythonic dictionary interface for configuration management
5. **Pythonic Interfaces**: Property-style access, context managers, iterables, and operator overloading
6. **Enhanced Error Messages**: Better error context, debugging information, and type-specific handling
7. **Performance Optimizations**: Lazy validation, reference counting, and memory-efficient conversions
8. **Modern Python Integration**: Type hints, parameter validation, and support for advanced Python features

## Quick Start

### Installation

```bash
# Install with all enhanced features
pip install pynlm[enhanced]

# Basic installation (backward compatible)
pip install pynlm

# Development installation
pip install -e .[dev]
```

### Basic Usage (Backward Compatible)

The existing NLM API continues to work unchanged:

```python
import pynlm

# Create a brain (same as before)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for step in range(100):
    brain.step(step)

print(f"Your brain thought {brain.getTotalSpikeCount()} times")
```

### Enhanced Usage

```python
import pynlm
import numpy as np

# Create enhanced brain interface
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
pythonic_brain = pynlm.create_pythonic_brain(brain)

# Use property-style access
print(f"Neurons: {pythonic_brain.total_neurons}")
print(f"Firing rate: {pythonic_brain.average_firing_rate:.2f}")
print(f"Developmental stage: {pythonic_brain.developmental_stage}")

# Process sensory data with NumPy
vision_data = np.random.rand(224, 224, 3).astype(np.float32)
converted_data = pynlm.get_as_numpy(vision_data)

# Enhanced error handling
try:
    config['learning_rate'] = 0.001
except pynlm.ValidationError as e:
    print(f"Config error: {e}")
    print(f"Field: {e.field}")
    print(f"Value: {e.value}")
    print(f"Expected: {e.expected}")
```

## Key Features

### 1. Type Conversion Utilities

The `TypeConverter` class provides automatic conversions between C++ and Python types:

```python
from pynlm.type_converter import TypeConverter

# Basic type conversion
c_value = 42.5
py_value = TypeConverter.to_python(c_value)

# Container conversion
c_list = [1.0, 2.0, 3.0, 4.0]
py_list = TypeConverter.to_python(c_list)

# String conversion
c_string = "test_string"
py_string = TypeConverter.to_python(c_string)
```

### 2. NumPy Array Support

Comprehensive NumPy integration for vision/audio data processing:

```python
from pynlm.numpy_converter import NumpyConverter
import numpy as np

# Convert to NumPy array
vision_data = np.random.rand(224, 224, 3).astype(np.float32)
numpy_array = NumpyConverter.to_numpy(vision_data)

# Round-trip conversion
restored_data = NumpyConverter.from_numpy(numpy_array, vision_data.shape)

# Utility functions
converted = pynlm.get_as_numpy(data, dtype=np.float32)
pynlm.set_from_numpy(target, numpy_array)
```

### 3. Enum Conversions

Bidirectional conversion between C++ enums and Python strings:

```python
from pynlm.enum_converter import EnumConverter

# Convert enum to string
neuron_type = pynlm.NeuronType.Excitatory
type_string = EnumConverter.to_string(neuron_type)

# Convert string to enum
action_type = EnumConverter.from_string('MoveForward', pynlm.ActionType)

# Convenience functions
string_type = pynlm.to_string(neuron_type)
enum_type = pynlm.from_string('MoveForward', pynlm.ActionType)
```

### 4. Dictionary-based Configuration

Pythonic dictionary interface for configuration management:

```python
from pynlm.config_converter import ConfigConverter

# Convert configuration
config_dict = ConfigConverter.to_config_dict(config_obj)

# Update configuration from dictionary
ConfigConverter.from_config_dict(config_dict, config_obj)

# Using enhanced PythonicConfig
pythonic_config = pynlm.create_pythonic_config(config_obj)

# Dictionary-style interface
learning_rate = pythonic_config['learning_rate']
pythonic_config['batch_size'] = 32
key in pythonic_config
for key in pythonic_config:
    print(f"{key}: {pythonic_config[key]}")
```

### 5. Pythonic Interfaces

Enhanced interfaces for all NLM classes with property-style access:

```python
from pynlm.pythonic_interfaces import (
    PythonicConfig, PythonicSensoryInput, 
    PythonicAction, PythonicBrain
)

# Property-style access
config = pynlm.create_pythonic_config(cpp_config)
print(config.learning_rate)  # Property access
print(config.keys)  # Returns list of keys
print(config.values)  # Returns dictionary of values

# Context manager support
with brain as b:
    b.step(step)
    # Automatic cleanup

# Dictionary-like interface
config['learning_rate'] = 0.001
value = config['learning_rate']
```

### 6. Enhanced Error Messages

Improved error messages with detailed context:

```python
try:
    # Trigger an error
    config['invalid_key'] = value
except pynlm.ConfigurationError as e:
    print(f"Error: {e.message}")
    print(f"Key: {e.key}")
    print(f"Error code: {e.error_code}")
    print(f"Error type: {type(e).__name__}")

# All enhanced error types
try:
    # Trigger validation error
    value = config['invalid_field']
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
    print(f"Field: {e.field}")
    print(f"Value: {e.value}")
    print(f"Expected: {e.expected}")

# Error codes for programmatic handling
error_codes = {
    pynlm.ErrorCodes.FILE_NOT_FOUND: "File not found",
    pynlm.ErrorCodes.VALIDATION_ERROR: "Validation error",
    pynlm.ErrorCodes.CONFIGURATION_ERROR: "Configuration error"
}
```

### 7. Performance Optimizations

Performance optimizations for large-scale simulations:

```python
# Enable lazy validation
pynlm.enable_lazy_validation(brain)

# Check performance features
print(f"NumPy support: {pynlm.has_numpy()}")
print(f"Property access: {pynlm.has_property_access()}")
print(f"Context managers: {pynlm.has_context_managers()}")
print(f"Version: {pynlm.get_version()}")

# Performance comparison
size = "large"
operation = "conversion"
time_ms = 100  # Example timing
print(f"{size.capitalize()} {operation}: {time_ms}ms")
```

### 8. Modern Python Integration

Full modern Python support with type hints and advanced features:

```python
# Type hints
def process_data(data: np.ndarray) -> np.ndarray:
    return pynlm.get_as_numpy(data)

# Context managers
with brain as b:
    result = b.process_sensory(sensory_input)

# Iterator support
for key in config:
    print(f"{key}: {config[key]}")

# Operator overloading
merged_config = config1 + config2
if config1 == config2:
    print("Configs are equal")

# Method chaining
result = pynlm.process_data(data).process_sensory().decode_action()
```

## Performance Features

### Lazy Validation

Lazy validation can be enabled for performance-critical operations:

```python
# Enable lazy validation
pynlm.enable_lazy_validation(brain)

# Or disable for strict validation
pynlm.disable_lazy_validation(brain)
```

### Memory-Efficient Conversions

Zero-copy and streaming conversions for large datasets:

```python
# Convert large arrays efficiently
large_data = np.random.rand(10000, 100).astype(np.float32)

# Zero-copy conversion where possible
numpy_array = pynlm.get_as_numpy(large_data)

# Memory-efficient round-trip
restored = pynlm.set_from_numpy(target, numpy_array)
```

### NumPy Integration

Seamless integration with NumPy for scientific computing:

```python
import numpy as np

# NumPy-backed operations
vision_data = np.random.rand(224, 224, 3).astype(np.float32)

# Direct NumPy access
pixels = vision_data.reshape(-1, 3)

# Efficient array operations
processed = np.mean(pixels, axis=0)

# Convert back to C++
converted = pynlm.get_as_numpy(processed)
```

## Error Handling

### Error Types

All error types are enhanced with detailed context:

```python
# File I/O errors
except pynlm.FileIOException as e:
    print(f"File error: {e.message}")
    print(f"Filepath: {e.filepath}")
    print(f"Operation: {e.operation}")

# Validation errors
except pynlm.ValidationError as e:
    print(f"Validation error: {e.message}")
    print(f"Field: {e.field}")
    print(f"Value: {e.value}")
    print(f"Expected: {e.expected}")

# Configuration errors
except pynlm.ConfigurationError as e:
    print(f"Config error: {e.message}")
    print(f"Key: {e.key}")

# Brain errors
except pynlm.BrainError as e:
    print(f"Brain error: {e.message}")
    print(f"Neuron ID: {e.neuron_id}")

# World errors
except pynlm.WorldError as e:
    print(f"World error: {e.message}")
    print(f"Position: {e.position}")

# Sensory errors
except pynlm.SensoryError as e:
    print(f"Sensory error: {e.message}")
    print(f"Sensor type: {e.sensor_type}")

# Audio errors
except pynlm.AudioError as e:
    print(f"Audio error: {e.message}")
    print(f"Sample rate: {e.sample_rate}")

# Action errors
except pynlm.ActionError as e:
    print(f"Action error: {e.message}")
    print(f"Action type: {e.action_type}")

# Agent errors
except pynlm.AgentError as e:
    print(f"Agent error: {e.message}")
    print(f"Agent ID: {e.agent_id}")
```

### Error Codes

Error codes are available for programmatic error handling:

```python
from pynlm.exceptions import ErrorCodes

# File I/O error codes
print(ErrorCodes.FILE_NOT_FOUND)  # "FILE_NOT_FOUND"
print(ErrorCodes.FILE_PERMISSION_ERROR)  # "FILE_PERMISSION_ERROR"

# Validation error codes
print(ErrorCodes.INVALID_DIMENSIONS)  # "INVALID_DIMENSIONS"
print(ErrorCodes.INVALID_RANGE)  # "INVALID_RANGE"

# Configuration error codes
print(ErrorCodes.KEY_NOT_FOUND)  # "KEY_NOT_FOUND"
print(ErrorCodes.TYPE_MISMATCH)  # "TYPE_MISMATCH"

# Brain error codes
print(ErrorCodes.NEURON_NOT_FOUND)  # "NEURON_NOT_FOUND"
print(ErrorCodes.INVALID_CURRENT_VALUE)  # "INVALID_CURRENT_VALUE"

# And many more...
```

## Advanced Features

### Custom Converters

Register custom converters for specific types:

```python
from pynlm.type_converter import register_converter

def custom_converter(value):
    # Custom conversion logic
    return processed_value

# Register custom converter
register_converter('CustomType', custom_converter)
```

### Configuration Management

Advanced configuration management with validation:

```python
# Create configuration dictionary
config_dict = {
    "learning_rate": 0.001,
    "hidden_size": 256,
    "batch_size": 32,
    "activation": "relu",
    "dropout": 0.2,
    "optimizer": "adam"
}

# Create C++ config from dictionary
config = pynlm.createDefaultConfig()
pynlm.config_from_dict(config_dict, config)

# Convert back to dictionary
pythonic_config = pynlm.create_pythonic_config(config)
config_values = pythonic_config.to_dict()
```

### Performance Monitoring

Monitor and optimize performance:

```python
# Check enabled features
features = {
    "numpy_support": pynlm.has_numpy(),
    "property_access": pynlm.has_property_access(),
    "context_managers": pynlm.has_context_managers(),
    "enum_conversion": pynlm.has_property_access(),
    "version": pynlm.get_version()
}

# Performance comparison
size_operations = {
    "small": {"conversion": 1, "validation": 1, "processing": 1},
    "medium": {"conversion": 10, "validation": 10, "processing": 10},
    "large": {"conversion": 100, "validation": 100, "processing": 100}
}

for size, operations in size_operations.items():
    print(f"\n{size.capitalize()} data:")
    for op, time_ms in operations.items():
        print(f"  {op}: {time_ms}ms")
```

## Migration Guide

### From Old Code

Existing code continues to work unchanged:

```python
# Old code (still works)
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
action = brain.produceAction()
```

### To Enhanced Code

Enhanced code provides additional features:

```python
# Enhanced code
brain = pynlm.createBrain(config)
pythonic_brain = pynlm.create_pythonic_brain(brain)
pythonic_brain.initialize()
pythonic_brain.step(step)
pythonic_action = pythonic_brain.produceAction()

# Additional features available
print(f"Neurons: {pythonic_brain.total_neurons}")
print(f"Firing rate: {pythonic_brain.average_firing_rate}")
```

### Migration Path

1. **Gradual Migration**: Start with enhanced features where beneficial
2. **Backward Compatibility**: All existing code continues to work
3. **Performance Benefits**: Enable enhanced features for performance-critical operations
4. **Error Handling**: Use enhanced error handling for better debugging

## Testing and Validation

### Unit Tests

Comprehensive test suite for all new features:

```bash
# Run all tests
pytest tests/

# Run specific test modules
pytest tests/test_type_conversion.py
pytest tests/test_numpy_integration.py
pytest tests/test_enhanced_bindings.py
pytest tests/test_error_handling.py
pytest tests/test_performance.py
```

### Validation Examples

```python
# Type conversion validation
assert pynlm.TypeConverter.to_python(42) == 42
assert pynlm.to_string(pynlm.NeuronType.Excitatory) == 'Excitatory'

# NumPy validation
import numpy as np
data = np.array([1.0, 2.0, 3.0])
converted = pynlm.get_as_numpy(data)
assert np.allclose(data, converted)

# Error validation
try:
    raise pynlm.ValidationError('Test error', field='test')
except pynlm.ValidationError as e:
    assert 'Test error' in str(e)
    assert e.field == 'test'
```

## Development Setup

### Prerequisites

```bash
python >= 3.8
pybind11
NumPy (optional)
scikit-build-core
```

### Development Installation

```bash
# Clone the repository
git clone <repository-url>
cd nlm-python-bindings

# Install in development mode
pip install -e .[dev]

# Build with enhanced features
python setup.py build_ext --inplace
```

### Build Configuration

The bindings are configured with CMake:

```bash
# Build options
-DENABLE_NUMPY_SUPPORT=ON
-DENABLE_PROPERTY_ACCESS=ON
-DENABLE_CONTEXT_MANAGERS=ON
-DENABLE_ENUM_CONVERSION=ON
-DENABLE_LAZY_VALIDATION=ON

# Configure
cmake -B build -DENABLE_ALL_FEATURES=ON

# Build
cmake --build build
```

## Contributing

### Code Style

Follow the existing code style and conventions:

```python
# Use existing patterns
from pynlm.type_converter import TypeConverter
from pynlm.numpy_converter import NumpyConverter
from pynlm.enum_converter import EnumConverter

# Follow existing documentation format
"""
This is the format for docstrings.
"""
```

### Testing

Add comprehensive tests for new features:

```python
def test_type_conversion():
    # Test type conversion functionality
    pass

def test_numpy_integration():
    # Test NumPy integration
    pass

def test_enum_conversion():
    # Test enum conversion
    pass

def test_error_handling():
    # Test enhanced error handling
    pass

def test_performance():
    # Test performance optimizations
    pass
```

### Documentation

Update documentation with examples:

```markdown
## New Features

### Type Conversions

Add examples for type conversion utilities.

### NumPy Support

Show NumPy array usage examples.

### Error Handling

Document enhanced error messages.
```

## API Reference

### TypeConverter

```python
class TypeConverter:
    """Convert C++ types to Python types."""
    
    @staticmethod
    def to_python(value: Any) -> Any:
        """Convert C++ value to Python type."""
    
    @staticmethod
    def from_python(value: Any, target_type: type) -> Any:
        """Convert Python value to C++ type."""
```

### NumpyConverter

```python
class NumpyConverter:
    """Convert between NumPy arrays and C++ data."""
    
    @staticmethod
    def to_numpy(data: Any, dtype: np.dtype = np.float32) -> np.ndarray:
        """Convert to NumPy array."""
    
    @staticmethod
    def from_numpy(arr: np.ndarray, target_shape: Optional[Tuple[int, ...]] = None) -> Any:
        """Convert from NumPy array."""
```

### EnumConverter

```python
class EnumConverter:
    """Convert between C++ enums and Python strings."""
    
    @staticmethod
    def to_string(enum_value: Any) -> str:
        """Convert enum to string."""
    
    @staticmethod
    def from_string(value: str, enum_type: type) -> Any:
        """Convert string to enum."""
```

### ConfigConverter

```python
class ConfigConverter:
    """Convert between C++ Config and Python dictionaries."""
    
    @staticmethod
    def to_config_dict(config_obj: Any) -> Dict[str, Any]:
        """Convert C++ Config to dictionary."""
    
    @staticmethod
    def from_config_dict(config_dict: Dict[str, Any], config_obj: Any) -> None:
        """Update C++ Config from dictionary."""
```

### PythonicConfig

```python
class PythonicConfig:
    """Pythonic interface for C++ Config class."""
    
    @property
    def keys(self) -> List[str]:
        """Get configuration keys."""
    
    @property
    def values(self) -> Dict[str, Any]:
        """Get configuration values."""
    
    def __getitem__(self, key):
        """Get value using dictionary-style access."""
    
    def __setitem__(self, key, value):
        """Set value using dictionary-style access."""
    
    def __contains__(self, key):
        """Check if key exists."""
    
    def __iter__(self):
        """Iterate over keys."""
```

### PythonicSensoryInput

```python
class PythonicSensoryInput:
    """Pythonic interface for C++ SensoryInput class."""
    
    @property
    def type(self) -> str:
        """Get sensory type."""
    
    @property
    def dimensions(self) -> int:
        """Get dimensionality."""
    
    @property
    def timestamp(self) -> float:
        """Get timestamp."""
    
    @property
    def data(self) -> np.ndarray:
        """Get data as NumPy array."""
```

### PythonicAction

```python
class PythonicAction:
    """Pythonic interface for C++ Action class."""
    
    @property
    def action_type(self) -> Any:
        """Get action type."""
    
    @property
    def parameters(self) -> List[float]:
        """Get parameters."""
    
    @property
    def name(self) -> str:
        """Get action name."""
```

### PythonicBrain

```python
class PythonicBrain:
    """Pythonic interface for C++ Brain class."""
    
    @property
    def total_neurons(self) -> int:
        """Get total neuron count."""
    
    @property
    def total_synapses(self) -> int:
        """Get total synapse count."""
    
    @property
    def active_neurons(self) -> int:
        """Get active neuron count."""
    
    @property
    def firing_neurons(self) -> int:
        """Get firing neuron count."""
```

## Examples Directory

The `examples/` directory contains comprehensive examples demonstrating all enhanced features:

```bash
examples/
├── basic_type_conversions.py
├── numpy_array_support.py
├── enum_conversions.py
├── config_dictionary_interface.py
├── pythonic_interfaces.py
├── enhanced_error_messages.py
├── performance_optimizations.py
├── modern_python_features.py
├── backward_compatibility.py
├── use_cases.py
└── comprehensive_example.py
```

## Testing Directory

The `tests/` directory contains comprehensive test suites:

```bash
tests/
├── test_type_conversion.py
├── test_numpy_integration.py
├── test_enum_conversion.py
├── test_config_conversion.py
├── test_pythonic_interfaces.py
├── test_error_handling.py
├── test_performance.py
└── test_backward_compatibility.py
```

## Documentation

- **Main Documentation**: `NLM_ENHANCED_BINDINGS.md` (this file)
- **API Reference**: `docs/api_reference.md`
- **Examples**: `examples/` directory
- **Tests**: `tests/` directory
- **Original Documentation**: `easy_usage.md`, `HOW_TO_USE.md`

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, modern, and Pythonic interface to the NLM neural simulation framework while maintaining full backward compatibility. The enhancements include:

1. **Comprehensive type conversion utilities** for seamless C++/Python integration
2. **NumPy array support** for efficient vision/audio data processing
3. **Enum conversions** for human-readable type representations
4. **Dictionary-based configuration** for intuitive configuration management
5. **Pythonic interfaces** with property-style access and context managers
6. **Enhanced error messages** with detailed context and debugging information
7. **Performance optimizations** including lazy validation and memory-efficient conversions
8. **Modern Python integration** with type hints and advanced Python features
9. **Backward compatibility** ensuring existing code continues to work

These enhancements significantly improve the developer experience, enable better performance for large-scale simulations, provide superior error reporting for debugging, and support modern Python development practices.

## Acknowledgments

Special thanks to the NLM development team for their vision and support in creating this comprehensive enhancement to the Python bindings. The enhanced features build upon the solid foundation of the existing NLM framework while introducing powerful new capabilities for improved performance and developer experience.
