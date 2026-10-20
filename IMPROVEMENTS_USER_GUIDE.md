# NLM Improvements - User Guide

This document describes the improvements made to the NLM (Neural Learning Machine) codebase to enhance usability, extensibility, and developer experience.

## Overview

The NLM codebase has been significantly improved with the following enhancements:

1. **Configuration Builder**: Fluent API for easy configuration
2. **Error Handling System**: Comprehensive error handling with detailed error messages
3. **Preset Configurations**: Ready-to-use configurations for common use cases
4. **Enhanced Documentation**: Better API documentation and examples
5. **Improved Testing**: Comprehensive test coverage

## 1. Configuration Builder

The Configuration Builder provides a fluent interface for creating NLM configurations. It replaces the complex manual configuration with an intuitive, builder-pattern approach.

### Basic Usage

```cpp
#include "ConfigBuilder.hpp"

// Simple preset
auto config = nlm::ConfigBuilder::simple();

// Custom configuration with fluent interface
auto config = nlm::ConfigBuilder()
    .neuronCount(500)
    .regionCount(2)
    .connectionProbability(0.15f)
    .simulationTimestep(0.001)
    .randomSeed(42)
    .configureLIF(-70.0, -55.0, -70.0, 10.0)
    .configureSTDP(0.02f, 0.015f, 20.0f);
```

### Available Presets

| Preset | Neurons | Regions | Description |
|--------|---------|---------|-------------|
| `simple()` | 100 | 1 | Fast testing, basic functionality |
| `standard()` | 500 | 2 | Balanced configuration for research |
| `highPerformance()` | 200 | 1 | Optimized for simulation speed |
| `research()` | 1000 | 3 | Comprehensive configuration for detailed research |
| `demo()` | 300 | 2 | Small but feature-rich for demonstrations |

### Available Configuration Methods

#### Brain Core Configuration
- `neuronCount(size_t)` - Set number of neurons
- `regionCount(size_t)` - Set number of regions
- `connectionProbability(float)` - Set synaptic connection probability
- `simulationTimestep(double)` - Set simulation timestep
- `randomSeed(uint64_t)` - Set random seed for reproducibility

#### Neural Dynamics
- `configureLIF(double resting, double threshold, double reset, double leak)` - Configure Leaky Integrate-and-Fire neuron parameters

#### Synaptic Plasticity
- `configureSTDP(float ltp, float ltd, float tau)` - Configure Spike-Timing-Dependent Plasticity parameters

#### Memory Systems
- `configureWorkingMemory(size_t capacity, float decayRate)` - Configure working memory
- `configureEpisodicMemory(size_t maxEpisodes, size_t replayInterval)` - Configure episodic memory

#### Neuromodulation
- `configureNeuromodulation(float dopamineEffect, float curiosityThreshold)` - Configure neuromodulation parameters

#### Development
- `configureDevelopment(float plasticity, float maturation)` - Configure developmental parameters

### Configuration Validation

The ConfigBuilder includes built-in validation to ensure configurations are valid:

```cpp
auto config = nlm::ConfigBuilder::standard();

if (!config->validate()) {
    std::cout << "Configuration is invalid!";
    std::cout << config->summary();
    return 1;
}
```

Validation errors provide detailed information:

```cpp
std::string errors = config->getValidationErrors();
// Output: "Validation errors: Missing neuron_count; neuron_count must be positive; "
```

## 2. Error Handling System

The NLM now includes a comprehensive error handling system that provides detailed error information and recovery mechanisms.

### Error Types

NLM errors are categorized into:

- **Core Errors**: Configuration, brain, memory, neural, plasticity, simulation errors
- **Configuration Errors**: File not found, invalid format, key not found, validation failures
- **Brain Errors**: Initialization, stepping, region/neuron not found
- **Memory Errors**: Capacity exceeded, corruption
- **Neural Errors**: Invalid neuron/synapse state
- **Plasticity Errors**: Not enabled, invalid parameters
- **System Errors**: Out of memory, invalid parameters, not implemented

### Error Handling in Code

```cpp
#include "ErrorHandling/ErrorHandling.hpp"

// Basic error checking
bool result = someFunction();
if (!result) {
    NLM_THROW_ERROR(ConfigError, "Failed to load configuration");
}

// Parameter validation
nlm_validate_param(value > 0.0f, InvalidParameter, "Value must be positive", "ParameterValidation");

// Memory validation
void* ptr = allocateMemory();
nlm_validate_memory(ptr, size);

// Exception handling
try {
    auto config = nlm::ConfigBuilder::standard();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
} catch (const nlm::NLMException& e) {
    std::cerr << "NLM Error: " << e.what() << std::endl;
    return 1;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
    return 1;
}
```

### Error Categories

Each error has a specific category and detailed message:

```cpp
try {
    // Some operation that might fail
} catch (const nlm::NLMException& e) {
    switch (e.getErrorCode()) {
        case nlm::NLMErrorCategory::ConfigFileNotFound:
            std::cout << "Configuration file not found: " << e.getContext() << std::endl;
            break;
        case nlm::NLMErrorCategory::BrainInitFailed:
            std::cout << "Brain initialization failed: " << e.getMessage() << std::endl;
            break;
        // Handle other error types...
    }
}
```

## 3. Example Programs

### Basic Example

See `examples/ConfigBuilderExample.cpp` for a complete example demonstrating the new configuration system and error handling.

```bash
// Build and run the example
cd build
make nlm_config_builder_example
./nlm_config_builder_example
```

### Unit Tests

Run the unit tests to verify all improvements:

```bash
cd build
make nlm_test
./nlm_test
```

## 4. Building and Installation

### Building with CMake

```bash
mkdir build
cd build
cmake ..
make -j4
```

### Building with Default Options

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Phase 6 Demo

The Phase 6 integration demo demonstrates all improvements:

```bash
mkdir build
cd build
cmake ..
make
./nlm_phase6_demo
```

## 5. Migration Guide

### From Old Configuration to New Configuration Builder

**Old way:**

```cpp
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
```

**New way:**

```cpp
auto config = nlm::ConfigBuilder::simple();
```

Or for custom configuration:

```cpp
auto config = nlm::ConfigBuilder()
    .neuronCount(100)
    .regionCount(1)
    .connectionProbability(0.1f);
```

### Error Handling Migration

**Old way:**

```cpp
if (!someOperation()) {
    std::cerr << "Error: Something went wrong!" << std::endl;
    return 1;
}
```

**New way:**

```cpp
try {
    someOperation();
} catch (const nlm::NLMException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

## 6. Testing

### Running Unit Tests

All unit tests are built into the `nlm_test` executable:

```bash
cd build
./nlm_test
```

### Test Coverage

The test suite includes:

- **Core functionality tests**: Config, Random, Clock, Types
- **Brain component tests**: Neuron, Synapse, Brain
- **Plasticity tests**: STDP, Hebbian
- **ConfigBuilder tests**: New fluent interface
- **Integration tests**: Cross-component interactions

### Adding New Tests

To add new tests, create a new test file in the `tests/` directory following the existing patterns. Include it in `tests/CMakeLists.txt`.

## 7. Performance Considerations

### Memory Management

The new error handling system uses smart pointers for automatic memory management, reducing the risk of memory leaks.

### Configuration Loading

The ConfigBuilder uses JSON format for configuration files, which provides better structure and validation compared to the simple key=value format.

### Thread Safety

The Logger class is now thread-safe, making it suitable for multi-threaded applications.

## 8. Future Improvements

### Planned Enhancements

1. **Plugin System**: Architecture for adding new neural components
2. **Advanced Visualization**: Real-time brain monitoring tools
3. **Distributed Computing**: Support for distributed simulations
4. **Machine Learning Integration**: Integration with modern ML frameworks
5. **Advanced Configuration**: Schema validation and configuration templates

### Contributing

Contributions are welcome! Please see the repository's contribution guidelines for more information.

## 9. Support

For questions, issues, or support, please refer to the NLM documentation and support channels.

---

**Last Updated**: October 2026
**Version**: 0.1.0 improvements

This document describes the improvements made to the NLM codebase. For the original documentation, please refer to the project's README.md file.