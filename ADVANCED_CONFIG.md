# NLM Configuration Enhancement

This document describes the advanced configuration features implemented in the Neural Learning Machine (NLM) framework.

## Overview

The NLM framework has been enhanced with advanced configuration capabilities that provide:

1. **Schema Validation**: Type checking and value validation for configuration parameters
2. **Runtime Configuration Adjustment**: Dynamic modification of configuration during runtime
3. **Auto-Save Functionality**: Automatic configuration persistence
4. **Configuration Profiles**: Save and load configuration presets
5. **Advanced Configuration Options**: More comprehensive configuration options for users
6. **Improved Documentation**: Better examples and usage guides

## Key Improvements

### 1. Advanced Configuration Class

The `AdvancedConfig` class extends the basic `Config` class with enterprise-level features:

#### Schema Validation
- Type checking for configuration values (int, double, bool, string, arrays)
- Range validation for numeric parameters
- Required parameter validation
- Custom validation rules

#### Runtime Configuration Adjustment
- Support for mathematical operations (+, -, *, /)
- Runtime parameter modification without restarting the simulation
- Safe configuration adjustment with validation

#### Auto-Save Functionality
- Automatic configuration persistence at specified intervals
- Configurable save file locations
- Automatic backup of important configurations

#### Configuration Profiles
- Save configuration states as profiles
- Load predefined configuration profiles
- Profile management and discovery

### 2. Configuration Validator

The `BasicConfigValidator` class provides validation capabilities:

- Type validation against expected types
- Range validation for numeric parameters
- Custom validation rules for specific parameters
- Comprehensive error reporting

### 3. Enhanced Python API

The Python bindings have been enhanced with:

- New configuration classes with advanced features
- Better error handling and validation
- More intuitive interface for users
- Support for batch operations

### 4. Improved Documentation

- Comprehensive configuration reference
- Usage examples for basic and advanced features
- Configuration parameter descriptions
- Best practices for configuration management

## Usage Examples

### Basic Configuration

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.learning_rate", 0.02)

# Get configuration values
neuron_count = config.getOr("brain.neuron_count", 1000)
learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.01)

print(f"Configuration: {neuron_count} neurons, {learning_rate} learning rate")
```

### Advanced Configuration with Validation

```python
from nlm import AdvancedConfig

# Create advanced configuration
config = AdvancedConfig()

# Register schema entries
config.registerSchema("brain.neuron_count", 1000,
                    "Total number of neurons", True)
config.registerSchema("brain.connection_probability", 0.1,
                    "Connection probability between neurons")
config.registerSchema("plasticity.stdp.learning_rate", 0.01,
                    "STDP learning rate")

# Set values
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.learning_rate", 0.02)

# Validate configuration
if config.validateAll():
    print("✓ All configuration validations passed")
else:
    errors = config.getValidationErrors()
    print(f"✗ Validation errors: {errors}")
```

### Runtime Configuration Adjustment

```python
config = pynlm.createDefaultConfig()

# Set initial value
config.set("brain.neuron_count", 1000)
print(f"Initial neuron count: {config.getOr('brain.neuron_count', 0)}")

# Adjust values at runtime
success = config.adjust("brain.neuron_count", "+", "500")
if success:
    new_value = config.getOr("brain.neuron_count", 0)
    print(f"After +500: {new_value}")

success = config.adjust("brain.neuron_count", "*", "0.5")
if success:
    new_value = config.getOr("brain.neuron_count", 0)
    print(f"After *0.5: {new_value}")
```

### Configuration Profiles

```python
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.2)

# Save a profile
config.saveProfile("research_profile")
print("✓ Saved profile 'research_profile'")

# Load the profile
config.loadProfile("research_profile")
print(f"✓ Loaded profile, neuron count: {config.getOr('brain.neuron_count', 0)}")
```

### Auto-Save Configuration

```python
config = pynlm.createDefaultConfig()

# Enable auto-save
config.enableAutoSave("auto_save.cfg", 60.0)  # Save every 60 seconds

# Set some configuration values
config.set("brain.neuron_count", 3000)
config.set("brain.connection_probability", 0.25)

# ... continue running simulation ...

# Auto-save will happen automatically every 60 seconds
# ...

# Disable auto-save
config.disableAutoSave()
```

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | double | 0.1 | Probability of connection between neurons |
| `brain.initial_weight_mean` | double | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | double | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | double | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | double | -70.0 | Resting potential (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | double | 0.01 | STDP learning rate |
| `plasticity.stdp.tau_plus` | double | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | double | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | double | 0.01 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | double | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Building and Running Tests

### Prerequisites

- CMake 3.16 or higher
- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- Python 3.8 or higher
- pybind11 2.11.0 or higher

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Running Python Tests

```bash
# Install the package
pip install -e .

# Run Python tests
python test_config_basic.py
python test_advanced_config.py
```

## Configuration File Format

NLM supports simple configuration files in key=value format:

```
# Brain configuration
brain.neuron_count = 1000
brain.connection_probability = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0

# Plasticity configuration
plasticity.stdp.learning_rate = 0.01
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0

# Neuromodulation configuration
neuromod.dopamine.scale = 1.0
```

## Troubleshooting

### Common Issues

**Issue**: Configuration validation fails

**Solution**: Check the validation errors returned by the `validateAll()` method. Common issues include:
- Invalid types for configuration parameters
- Values out of valid ranges
- Missing required parameters

**Issue**: Auto-save not working

**Solution**: Ensure the configuration file path is writable and check for any permission issues.

**Issue**: Configuration profiles not found

**Solution**: Ensure configuration profiles are saved to the correct directory and verify the profile name is correct.

### Error Messages

- `Validation failed for key '...': ...` - Configuration validation failed
- `Failed to save configuration to ...` - Auto-save failed
- `Profile '...' not found` - Configuration profile not found

## Future Enhancements

The configuration system will continue to evolve with:

1. **JSON/YAML Support**: Proper JSON and YAML configuration file formats
2. **Configuration Templates**: Predefined configuration templates for common use cases
3. **Configuration Comparison**: Advanced configuration comparison and diff tools
4. **Configuration History**: Track changes to configuration over time
5. **Configuration Plugins**: Plugin system for custom configuration validators
6. **Web Configuration Interface**: Web-based configuration management interface

## Conclusion

The enhanced configuration system in NLM provides enterprise-level configuration management capabilities while maintaining the simplicity and ease of use that the framework is known for. The new features enable users to:

- Easily configure and manage NLM simulations
- Validate configuration parameters to prevent runtime errors
- Dynamically adjust configuration during runtime
- Save and load configuration profiles for different scenarios
- Automate configuration management with auto-save functionality

These improvements make NLM more accessible to users of all skill levels while providing powerful configuration management for advanced users.
