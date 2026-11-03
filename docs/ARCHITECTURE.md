# NLM Developer Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain project. This document provides comprehensive guidance for developers working with the codebase.

## Project Structure

```
NLM/
├── src/
│   ├── core/                    # Core utilities
│   │   ├── Config/             # Configuration system
│   │   ├── Random/             # Random number generation
│   │   ├── Logger/             # Logging system
│   │   └── SimulationClock/    # Simulation clock
│   ├── brain/                  # Neural components
│   │   ├── Brain.hpp           # Central brain class
│   │   ├── Neuron.hpp          # LIF neuron implementation
│   │   ├── Synapse.hpp         # Synapse implementation
│   │   └── NeuralRegion.hpp    # Neural region management
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── agent/                  # Agent-brain interface
│   ├── experiments/            # Experiment framework
│   └── visualization/          # Visualization system
│
├── python/                      # Python bindings
│   └── bindings.cpp            # Pybind11 bindings
│
├── configs/                     # Configuration files
│   └── default.cfg             # Default configuration
│
├── tests/                      # Unit tests
│   ├── test_config.cpp         # Configuration tests
│   ├── test_brain.cpp          # Brain tests
│   └── ...                    # Other tests
│
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md         # Architecture overview
│   ├── SCIENCE.md             # Scientific background
│   └── ...                    # Other documentation
│
├── CMakeLists.txt              # Build configuration
├── pyproject.toml             # Python package configuration
├── README.md                   # User guide
└── easy_usage.md              # Quick start guide
```

## Configuration System Improvements

### Key Improvements

1. **Fixed Duplicate Configuration**
   - Removed duplicate `dopamine_baseline` line from `default.cfg`
   - Added missing Phase 2+ configuration parameters

2. **Enhanced Configuration File Format**
   - Removed Phase 1 references
   - Added comprehensive comments
   - Improved parameter organization
   - Added Phase 2+ parameters:
     - STDP parameters (`stdp_ltp_weight`, `stdp_ltd_weight`, `stdp_tau`)
     - Structural plasticity (`synaptogenesis_rate`, `pruning_rate`)
     - Neuromodulation (`dopamine_baseline`, `curiosity_baseline`)
     - Prediction system (`prediction_horizon`, `confidence_threshold`)
     - Memory systems (`working_memory_size`, `episodic_memory_capacity`)
     - Attention system (`attention_focus_strength`)
     - Planning system (`planning_horizon`)

3. **Improved Config.cpp with Enhanced Error Handling**
   - **File loading validation**: Validates key=value format, key names, and value types
   - **Command-line argument parsing**: Better validation of command-line arguments
   - **Error reporting**: Detailed error messages with line numbers and context
   - **Input validation**: Validates key names (alphanumeric, underscore, hyphen only)
   - **Exception handling**: Proper exception handling with descriptive error messages
   - **File saving**: Improved file saving with proper formatting and descriptions

### Configuration File Format

```ini
# NLM Configuration File
# This is the default configuration for Phase 2: Real Neural Computation

# Random seed for reproducible experiments
random_seed = 42

# Simulation settings
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP and Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1

# Curiosity and novelty parameters
curiosity_baseline = 0.05
novelty_sensitivity = 0.5

# ... additional parameters
```

### Configuration Usage

#### Loading Configuration

```cpp
// Load from file
auto config = std::make_shared<Config>();
if (config->loadFromFile("configs/my_config.cfg")) {
    std::cout << "Configuration loaded successfully" << std::endl;
} else {
    std::cout << "Failed to load configuration" << std::endl;
}

// Load from command line arguments
int argc = 3;
const char* argv[] = {"program", "--random_seed", "42", "--simulation_timestep", "0.001"};
config->loadFromArgs(argc, argv);
```

#### Getting Configuration Values

```cpp
// Get configuration values with type safety
auto seed = config->getOr<int64_t>("random_seed", 42);
auto timestep = config->getOr<double>("simulation_timestep", 0.001);
auto neurons = config->getOr<int64_t>("neuron_count", 500);

// Check if a key exists
if (config->has("custom_parameter")) {
    auto value = config->get<std::string>("custom_parameter");
}
```

#### Setting Configuration Values

```cpp
// Set configuration values
config->set("random_seed", 123, ConfigSource::Runtime);
config->set("simulation_timestep", 0.01, ConfigSource::Runtime);
config->set("neuron_count", 1000, ConfigSource::Runtime);

// Set different types
config->set("bool_value", true, ConfigSource::Runtime);
config->set("double_value", 3.14, ConfigSource::Runtime);
config->set("string_value", "test", ConfigSource::Runtime);
```

## Command-Line Interface Improvements

### New CLI Features

The main entry point (`src/main.cpp`) now includes:

1. **Comprehensive Argument Parsing**
   - Support for `--basic`, `--plasticity`, `--stdp` test modes
   - Support for `--all` to run all tests
   - Support for `--single-test <name>` to run specific tests
   - Support for `--config <file>` to load custom configuration
   - Support for `--help` and `--version` flags

2. **Usage Examples**

```bash
# Run basic connectivity test with custom config
./nlm --basic --config my_config.cfg

# Run plasticity experiment
./nlm --plasticity

# Run STDP verification test
./nlm --stdp

# Run all tests
./nlm --all

# Show help
./nlm --help

# Show version
./nlm --version
```

3. **Improved Test Organization**
   - Each test runs in isolation with brain reset
   - Clear test progress reporting
   - Detailed test results

## Python Bindings Improvements

### Key Enhancements

1. **Improved Naming Conventions**
   - `loadFromFile()` → `load_from_file()` (PEP8 compliant)
   - `getKeys()` → `get_keys()` (PEP8 compliant)
   - `has()` → unchanged (common Python naming)

2. **Better Type Safety**
   - Enhanced type conversion methods:
     - `get_int(key, default)`
     - `get_float(key, default)`
     - `get_string(key, default)`

3. **Convenience Methods**
   - Added helper methods for common operations
   - Improved error messages
   - Better documentation strings

### Python Usage Examples

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set_int("random_seed", 42)
config.set_float("simulation_timestep", 0.001)
config.set_string("neuron_count", "500")

# Load from file
config.load_from_file("configs/my_config.cfg")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.step(0)
brain.step(1)

# Get brain statistics
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

## Testing Improvements

### Enhanced Configuration Tests

Added comprehensive tests for the configuration system:

1. **File Loading Tests**
   - Test loading from valid configuration file
   - Test loading from non-existent file
   - Test loading from malformed configuration

2. **Command-Line Loading Tests**
   - Test parsing various command-line argument formats
   - Test error handling for malformed arguments

3. **Type Conversion Tests**
   - Test various type conversions (int, double, string, bool)
   - Test getOr() method with default values

4. **Source Tracking Tests**
   - Test that configuration sources are properly tracked
   - Test default, file, command-line, and runtime sources

5. **Edge Case Tests**
   - Test empty configuration
   - Test configuration clearing
   - Test configuration removal

### Test Framework

The test framework includes:

1. **Comprehensive Error Handling**
   - Catch exceptions and report detailed error messages
   - Continue testing even if individual tests fail

2. **Detailed Reporting**
   - Clear pass/fail reporting for each test category
   - Summary of all test results

3. **Test Categories**
   - Configuration tests
   - Random number generation tests
   - Clock tests
   - Neuron tests
   - Synapse tests
   - STDP tests
   - Brain tests

## Code Quality Improvements

### Configuration System Quality

1. **Input Validation**
   - Validate key names (alphanumeric, underscore, hyphen only)
   - Validate file formats and syntax
   - Validate command-line arguments

2. **Error Handling**
   - Detailed error messages with context
   - Proper exception handling
   - Graceful failure handling

3. **Memory Management**
   - Proper smart pointer usage
   - Exception-safe operations

### Testing Quality

1. **Test Coverage**
   - Comprehensive test coverage for configuration system
   - Edge case testing
   - Error condition testing

2. **Test Organization**
   - Separate test files for different components
   - Clear test naming conventions
   - Consistent testing patterns

## Development Guidelines

### Coding Standards

1. **Configuration Keys**
   - Use alphanumeric characters, underscores, and hyphens only
   - Avoid reserved keywords
   - Use descriptive names

2. **File Organization**
   - Keep configuration files in `configs/` directory
   - Use `.cfg` extension for configuration files
   - Include comments in configuration files

3. **Testing**
   - Write comprehensive tests for new features
   - Test edge cases and error conditions
   - Use the existing test framework

### Building and Running Tests

#### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Running Tests

```bash
# Run all tests
./test_runner

# Run specific test category
./test_runner --config
./test_runner --brain
./test_runner --neuron
```

#### Python Package

```bash
# Build Python bindings
pip install -e .

# Test Python bindings
python -c "import pynlm; print(pynlm.__version__)"
```

### Configuration Management

#### Best Practices

1. **Configuration File Structure**
   - Use comments to document parameters
   - Group related parameters together
   - Include source information in comments

2. **Configuration Validation**
   - Validate configuration before use
   - Check for required parameters
   - Validate parameter ranges and types

3. **Configuration Overrides**
   - Use command-line arguments to override configuration file values
   - Use environment variables for development environments
   - Use default values for production environments

## Migration Guide

### From Phase 1 to Phase 2

#### Configuration Changes

1. **File Format**
   - Previous: Simple key=value pairs with minimal comments
   - Current: Enhanced key=value pairs with comprehensive comments and validation

2. **New Parameters**
   - Added Phase 2+ configuration parameters
   - Improved parameter organization and naming

3. **Validation**
   - Configuration files are now validated
   - Invalid configurations result in error messages

#### Command-Line Interface

1. **Test Modes**
   - Previous: Hardcoded sequence of tests
   - Current: Flexible test selection with `--basic`, `--plasticity`, `--stdp`, `--all`

2. **Configuration Loading**
   - Previous: Limited configuration file loading
   - Current: Enhanced configuration loading with validation

#### Python Bindings

1. **API Changes**
   - Previous: Basic Python bindings
   - Current: Enhanced Python bindings with better type safety and convenience methods

2. **Naming Conventions**
   - Previous: Mixed naming conventions
   - Current: PEP8 compliant naming conventions

### Configuration Migration Examples

#### Migrating Configuration Files

```ini
# Before (Phase 1):
random_seed = 42
neuron_count = 1000
plasticity_learning_rate = 0.01

# After (Phase 2):
# Random seed for reproducible experiments
random_seed = 42

# Simulation settings
neuron_count = 500
connection_probability = 0.15

# STDP and Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
plasticity_learning_rate = 0.01
```

#### Migrating Command-Line Usage

```bash
# Before:
./nlm

# After:
./nlm --basic --config configs/default.cfg
```

#### Migrating Python Code

```python
# Before:
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# After:
import pynlm
config = pynlm.createDefaultConfig()
config.set_int("random_seed", 42)
config.set_int("neuron_count", 500)
brain = pynlm.createBrain(config)
```

## Troubleshooting

### Common Issues and Solutions

#### Configuration Loading Issues

**Issue**: Configuration file not loading
**Solution**:
1. Check if the file exists
2. Verify file format (key=value pairs)
3. Check for syntax errors
4. Ensure the file is readable

**Example**:

```bash
# Check file exists and is readable
ls -la configs/default.cfg
cat configs/default.cfg
```

#### Command-Line Argument Issues

**Issue**: Command-line arguments not parsing correctly
**Solution**:
1. Check argument format (--key=value or -key value)
2. Ensure no typos in argument names
3. Verify argument order

**Example**:

```bash
# Correct usage
./nlm --basic --config configs/default.cfg

# Incorrect usage (errors)
./nlm --basic=configs/default.cfg  # Missing space
./nlm --config=configs/default.cfg  # Wrong format
./nlm --unknown-option  # Unknown option
```

#### Python Binding Issues

**Issue**: Python bindings not importing correctly
**Solution**:
1. Check if the package is installed correctly
2. Verify Python environment
3. Check for missing dependencies

**Example**:

```bash
# Check installation
python -c "import pynlm; print(pynlm.__version__)"

# Check for errors
python -c "import pynlm; print(pynlm.createDefaultConfig())" 2>&1
```

## Future Improvements

### Configuration System Enhancements

1. **JSON/YAML Support**
   - Implement JSON configuration file support
   - Implement YAML configuration file support
   - Maintain backward compatibility with existing config files

2. **Configuration Validation**
   - Add parameter validation rules
   - Add dependency checking between parameters
   - Add schema validation

3. **Configuration Templates**
   - Create configuration templates for common use cases
   - Add parameter documentation
   - Add example configuration files

### Testing Enhancements

1. **Integration Testing**
   - Add integration tests for full system functionality
   - Add performance testing
   - Add stress testing

2. **Continuous Integration**
   - Add CI/CD pipeline
   - Add automated testing
   - Add code coverage analysis

### Documentation Improvements

1. **API Documentation**
   - Generate API documentation automatically
   - Add detailed parameter descriptions
   - Add usage examples

2. **Tutorial Documentation**
   - Add step-by-step tutorials
   - Add code examples
   - Add best practices guides

## Conclusion

The NLM project has been significantly improved with:

1. **Enhanced Configuration System**: Better error handling, validation, and user-friendly features
2. **Improved Command-Line Interface**: Flexible test selection and configuration loading
3. **Better Python Bindings**: PEP8 compliance and enhanced type safety
4. **Comprehensive Testing**: Extensive test coverage for the configuration system
5. **Code Quality Improvements**: Better error handling, validation, and maintainability

These improvements make the NLM project more robust, user-friendly, and maintainable for both developers and users.