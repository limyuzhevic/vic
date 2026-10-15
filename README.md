# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Improvements

This document describes the significant improvements made to the NLM codebase:

### 1. Enhanced Configuration System ✅

**New Configuration Features:**
- **AdvancedConfig Class**: Enterprise-level configuration with schema validation, runtime adjustment, auto-save, and profiles
- **ConfigValidator**: Comprehensive validation framework for configuration parameters
- **BasicConfigValidator**: Implementation of validation rules for common NLM parameters

**Key Features:**
- **Schema Validation**: Type checking, range validation, and custom validation rules
- **Runtime Configuration Adjustment**: Dynamic modification of parameters during runtime
- **Auto-Save Functionality**: Automatic configuration persistence at specified intervals
- **Configuration Profiles**: Save and load configuration presets for different scenarios
- **Comprehensive Error Handling**: Detailed error messages and validation feedback

### 2. Code Organization Improvements ✅

**Main.cpp Refactoring:**
- Created `nlm_phase2` namespace for better organization
- Extracted learning experiment logic into organized classes and functions
- Improved separation between configuration, execution, and testing logic
- Better code structure and readability

**File Structure:**
- Created `src/core/Config/` directory for configuration management
- Added `src/core/Config/validators/` for validation components
- Updated CMakeLists.txt to include new configuration libraries

### 3. Enhanced Python API ✅

**Python Bindings Improvements:**
- Enhanced `Config` class with advanced Python interface
- New configuration classes accessible from Python
- Improved error handling and validation in Python bindings
- Better integration with existing Python API

### 4. Comprehensive Documentation ✅

**Documentation Files:**
- `easy_usage.md`: Beginner-friendly usage guide
- `HOW_TO_USE.md`: Technical documentation with comprehensive examples
- `ADVANCED_CONFIG.md`: Advanced configuration features and examples
- `IMPROVEMENTS_SUMMARY.md`: Detailed summary of all improvements
- `test_config_basic.py`: Basic configuration tests
- `test_advanced_config.py`: Advanced configuration tests

### 5. Extensive Testing ✅

**Test Coverage:**
- `test_config_basic.py`: Basic configuration functionality
- `test_advanced_config.py`: Advanced configuration features
- `test_runtime_adjustment.py`: Runtime adjustment functionality
- `test_profiles.py`: Configuration profile management
- `test_auto_save.py`: Auto-save functionality
- C++ tests: Original NLM test suite maintained

### 6. Performance Optimization ✅

**Performance Improvements:**
- Memory usage tracking and optimization
- Efficient validation system with early termination
- Optimized configuration loading and saving
- Reduced memory footprint for configuration storage

## Usage Examples

### Basic Configuration
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)

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

# Set values
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)

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

# Set configuration values
config.set("brain.neuron_count", 3000)
config.set("brain.connection_probability", 0.25)

# ... continue running simulation ...

# Auto-save will happen automatically every 60 seconds
# ...

# Disable auto-save
config.disableAutoSave()
```

## Building and Running

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

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Running Python Tests
```bash
# Install the package
pip install -e .

# Run Python tests
python test_config_basic.py
python test_advanced_config.py
python test_runtime_adjustment.py
python test_profiles.py
python test_auto_save.py
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

## Testing

The NLM codebase includes comprehensive test coverage:

### C++ Tests
- **test_brain.cpp**: Brain initialization and functionality tests
- **test_neuron.cpp**: Neuron behavior and dynamics tests
- **test_stdp.cpp**: STDP plasticity rule tests
- **test_config.cpp**: Configuration system tests
- **test_types.cpp**: Type system tests
- **test_random.cpp**: Random number generation tests
- **test_clock.cpp**: Simulation clock tests
- **test_synapse.cpp**: Synapse functionality tests

### Python Tests
- **test_config_basic.py**: Basic configuration functionality
- **test_advanced_config.py**: Advanced configuration features
- **test_runtime_adjustment.py**: Runtime adjustment functionality
- **test_profiles.py**: Configuration profile management
- **test_auto_save.py**: Auto-save functionality

## Performance Optimization

### Memory Efficiency
- Optimized configuration storage
- Lazy loading of configuration values
- Memory-efficient validation
- Reduced memory footprint

### Speed Optimization
- Fast validation with early termination
- Efficient configuration loading/saving
- Optimized validation rules
- Parallel validation where possible

## Future Enhancements

The configuration system is designed for future enhancements:

1. **JSON/YAML Support**: Proper JSON and YAML configuration file formats
2. **Configuration Templates**: Predefined configuration templates for common use cases
3. **Configuration Comparison**: Advanced configuration comparison and diff tools
4. **Configuration History**: Track changes to configuration over time
5. **Configuration Plugins**: Plugin system for custom configuration validators
6. **Web Configuration Interface**: Web-based configuration management interface

## Conclusion

The NLM codebase has been significantly enhanced with the following improvements:

### Key Achievements
1. **Enhanced Configuration System**: Advanced configuration with validation, runtime adjustment, auto-save, and profiles
2. **Better Organization**: Cleaner code structure and organization
3. **Enhanced Python API**: More Pythonic interface with advanced features
4. **Comprehensive Documentation**: Extensive documentation and examples
5. **Extensive Testing**: Comprehensive test coverage for all new features
6. **Performance Optimization**: Efficient implementation with reduced memory footprint
7. **Backward Compatibility**: All existing API and functionality preserved

### User Benefits
- **Easier Configuration**: Simplified configuration interface with better validation
- **Dynamic Adjustment**: Runtime configuration modification without restarting simulations
- **Profile Management**: Save and load configuration presets for different scenarios
- **Auto-Save**: Automatic configuration persistence
- **Enhanced Documentation**: Comprehensive documentation and examples for all features

### Developer Benefits
- **Better Organization**: Cleaner code structure and maintainability
- **Enhanced Features**: Advanced configuration and validation capabilities
- **Improved API**: More Pythonic and intuitive interface
- **Comprehensive Testing**: Extensive test coverage
- **Performance Optimization**: Efficient implementation with reduced memory footprint

The NLM framework is now more powerful, easier to use, and better organized while maintaining full backward compatibility. Users can benefit from the new advanced features while existing code continues to work without changes.

## Phase Summary

NLM has successfully implemented all requested improvements:

### Phase 6: Final Integration ✅ COMPLETE
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

### Configuration Enhancement ✅ COMPLETE
- Schema validation and type checking
- Runtime configuration adjustment
- Auto-save functionality
- Configuration profiles
- Comprehensive validation
- Enhanced Python API
- Extensive documentation
- Comprehensive testing

## License

MIT

## Authors

Research project — See docs for scientific background.