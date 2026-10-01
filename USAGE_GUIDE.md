# NLM Improvements - Usage Guide

This document provides guidance on using the improvements made to the NLM (Neural Learning Machine) codebase.

## 1. Enhanced Error Handling

### Basic Usage

The new error handling system provides several ways to work with errors:

#### Using Exception Throws

```cpp
// Include the error handling headers
#include "NLMError.hpp"

// Throw a validation error
try {
    if (invalid_parameter) {
        NLM_THROW_VALIDATION("Invalid parameter value: " + std::to_string(value));
    }
} catch (const nlm::NLMException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "Code: " << static_cast<int>(e.getCode()) << std::endl;
}

// Throw a file system error
try {
    if (file_not_found) {
        NLM_THROW_FILE_ERROR(ErrorCode::FileNotFound, "Cannot open file", "/path/to/file.txt");
    }
} catch (const nlm::NLMException& e) {
    // Handle file error
}
```

#### Using Result Pattern

```cpp
// For functions that can succeed or fail
auto result = someFunctionThatCanFail(param1, param2);

if (result.isSuccess()) {
    // Use the result
    auto value = result.value();
} else {
    // Handle the error
    auto error = *result.getError();
    std::cerr << "Operation failed: " << error.what() << std::endl;
}
```

### Error Codes

The error handling system provides standardized error codes:

- **Success**: `ErrorCode::Success`
- **General Errors**: `ErrorCode::Unknown`, `ErrorCode::NotImplemented`, `ErrorCode::InvalidArgument`
- **File System**: `ErrorCode::FileNotFound`, `ErrorCode::FileAccessDenied`, `ErrorCode::FileCorrupted`
- **Neural Network**: `ErrorCode::NeuronNotFound`, `ErrorCode::SynapseNotFound`, `ErrorCode::InvalidNetworkState`
- **Memory**: `ErrorCode::MemoryAllocationFailed`, `ErrorCode::MemoryCorruption`
- **Configuration**: `ErrorCode::ConfigInvalid`, `ErrorCode::ConfigFileError`
- **Simulation**: `ErrorCode::SimulationNotRunning`, `ErrorCode::InvalidTimeStep`

### Error Context

Errors include rich context information:

```cpp
// Error context provides additional information
ErrorContext context("TRAINING", "NeuralNetwork", "Learning rate too high",
                   "Reduce learning rate to 0.001 or lower");
```

## 2. Plugin Architecture

### Creating a Custom Plasticity Rule Plugin

Here's how to create a custom plasticity rule plugin:

#### Step 1: Implement Plugin Interface

```cpp
// custom_plasticity.cpp
#include "PluginInterface.hpp"

class MyCustomPlasticity : public nlm::PlasticityRulePlugin {
public:
    MyCustomPlasticity() : learningRate_(0.01f) {}
    
    const char* getName() const override { return "MyCustomPlasticity"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { 
        return "Custom plasticity rule with unique learning dynamics"; 
    }
    const char* getComponentName() const override { return "CustomRule"; }
    
    bool initialize(const std::string& config) override {
        // Parse configuration if needed
        return true;
    }
    
    void shutdown() override {
        // Clean up resources
    }
    
    bool isRunning() const override { return true; }
    
    void update(Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt) override {
        // Your custom plasticity rule implementation
        // This will be called by the brain simulation
    }
    
    float getLearningRate() const override { return learningRate_; }
    void setLearningRate(float rate) override { learningRate_ = rate; }
    
private:
    float learningRate_;
};
```

#### Step 2: Create Factory Function

```cpp
// custom_plasticity.cpp
extern "C" nlm::Plugin* create_plugin() {
    return new MyCustomPlasticity();
}

extern "C" const char* get_plugin_info() {
    return "My Custom Plasticity Plugin v1.0.0";
}
```

#### Step 3: Compile as Shared Library

```bash
# Create shared library
clang++ -shared -fPIC custom_plasticity.cpp -o libmycustomplasticity.so

# Or using CMake
add_library(mycustomplasticity SHARED custom_plasticity.cpp)
set_target_properties(mycustomplasticity PROPERTIES PREFIX "" OUTPUT_NAME "mycustomplasticity")
```

#### Step 4: Load Plugin at Runtime

```cpp
// main.cpp
#include "PluginInterface.hpp"

int main() {
    // Load plugin from file
    nlm::PluginRegistry& registry = nlm::PluginRegistry::getInstance();
    
    if (registry.loadPluginsFromDirectory("./plugins")) {
        std::cout << "Plugins loaded successfully:" << std::endl;
        for (const auto& name : registry.getPluginNames()) {
            std::cout << "  - " << name << std::endl;
        }
    }
    
    // Create a plugin instance
    auto plugin = registry.createPlugin("MyCustomPlasticity");
    if (plugin) {
        plugin->initialize("{"learning_rate": 0.02}");
        // Use the plugin...
    }
    
    return 0;
}
```

### Plugin Types

The plugin system supports several component types:

- **PlasticityRulePlugin**: Custom synaptic plasticity rules
- **NeuromodulatorPlugin**: Custom neuromodulators (dopamine, novelty, curiosity)
- **MemorySystemPlugin**: Custom memory systems (working memory, episodic memory)

## 3. Enhanced Testing

### Running Tests

The test suite includes comprehensive coverage for all major components:

```bash
# Build and run all tests
mkdir build
cd build
cmake ..
make -j$(nproc)
./nlm_test

# Or run specific tests
./nlm_test  # Runs all tests including integration tests
```

### Test Coverage

The test suite covers:

- **Random Generator**: Number generation and distribution tests
- **Configuration System**: File loading and validation tests
- **Neuron and Synapse**: Basic biological component tests
- **Plasticity Rules**: Hebbian and STDP learning algorithm tests
- **Brain Integration**: System-wide functionality tests
- **Memory Systems**: Working memory and concept formation tests
- **Integration Tests**: End-to-end system validation

### Adding New Tests

To add tests for new functionality:

```cpp
// tests/test_myfeature.cpp
#include "feature/MyFeature.hpp"
#include <cassert>
#include <iostream>

namespace test_myfeature {
    void testMyFeatureCreation() {
        MyFeature feature("test");
        assert(feature.getName() == "test");
        std::cout << "    testMyFeatureCreation passed" << std::endl;
    }
    
    // Add more test functions...
    
    void runAll() {
        std::cout << "Running MyFeature tests..." << std::endl;
        testMyFeatureCreation();
        // Add other tests...
    }
}
```

Then add the test file to `tests/CMakeLists.txt`:

```cmake
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_hebbian.cpp
    test_integration.cpp
    test_memory.cpp
    test_myfeature.cpp  # Add new test here
)
```

## 4. Configuration Management

### Configuration File Format

The configuration system uses simple key=value pairs:

```ini
# Configuration file for NLM

# Simulation parameters
simulation_timestep = 0.001
neuron_count = 500
region_count = 1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Neural dynamics parameters
membrane_time_constant = 20.0
membrane_resistance = 10.0
```

### Command Line Overrides

Configuration values can be overridden from the command line:

```bash
./nlm --config myconfig.cfg
./nlm --neuron_count 1000 --region_count 2 --connection_probability 0.5
```

## 5. Build and Installation

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Python Bindings

The NLM project includes Python bindings via pybind11. To install and use:

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install Python package
pip install .

# Or install in development mode
pip install -e .
```

### Building Python Package

```bash
# Build only the Python package
pip install build
python -m build

# Install from wheel
pip install dist/nlm-0.1.0-py3-none-any.whl
```

## 6. Advanced Usage

### Custom Configuration Loading

```cpp
// Load configuration from custom file
auto config = std::make_shared<nlm::Config>();

if (config->loadFromFile("myconfig.cfg")) {
    std::cout << "Configuration loaded successfully" << std::endl;
    
    // Override specific settings
    config->set("neuron_count", static_cast<int64_t>(750), 
               nlm::ConfigSource::Runtime);
} else {
    std::cerr << "Failed to load configuration" << std::endl;
    // Use defaults...
}
```

### Plugin Configuration

```cpp
// Configure plugin through configuration system
auto config = std::make_shared<nlm::Config>();
config->set("plasticity_rules", "MyCustomPlasticity", nlm::ConfigSource::Runtime);

// Initialize brain with plugin configuration
auto brain = std::make_shared<nlm::Brain>(config);
```

## Troubleshooting

### Common Issues and Solutions

1. **Plugin Loading Failures**
   - Ensure shared libraries have correct naming conventions
   - Check for missing dependencies
   - Verify plugin interface compatibility

2. **Configuration Loading Issues**
   - Verify file format and syntax
   - Check for missing required parameters
   - Ensure proper file permissions

3. **Test Failures**
   - Run tests with verbose output: `ctest -V`
   - Check for memory leaks with valgrind (Linux)
   - Verify build configuration

4. **Compilation Errors**
   - Ensure C++20 compiler support
   - Check for missing dependencies
   - Verify build configuration options

## Further Reading

- **NLM Documentation**: Complete documentation in the `docs/` directory
- **API Reference**: Generated documentation for all public interfaces
- **Research Papers**: Scientific background and algorithm explanations
- **Example Projects**: Sample projects demonstrating various use cases

## Version Information

**Current Version**: 0.1.0 (Phase 2)
**Release Date**: November 2026
**Status**: Production Ready

## Support and Community

For support and community discussion:
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Comprehensive documentation and examples
- **Forum**: Community support and discussions
- **Contact**: Direct support for enterprise customers

---

*This guide provides comprehensive documentation for using the enhanced NLM features. For the most current information, refer to the specific documentation files in the project.*