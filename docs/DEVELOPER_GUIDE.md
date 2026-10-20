# NLM Developer Guide

## Overview

This guide provides comprehensive information for developers working on the NLM (熙然) brain simulation project, covering setup, development workflows, testing, and contribution guidelines.

## Getting Started

### Prerequisites

#### Software Requirements
- **Compiler**: C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **Build System**: CMake 3.15 or higher
- **Package Manager**: Git for version control
- **IDE**: Visual Studio Code, CLion, or any C++ IDE with CMake support

#### System Dependencies
- Standard C++ libraries
- POSIX-compliant system (Linux, macOS, BSD)
- Optional: Python for testing integration

### Project Structure

```
/workspace/app/
├── build/                    # Build directory
├── configs/                  # Default configuration files
├── docs/                     # Documentation files
│   ├── API_REFERENCE.md      # Complete API documentation
│   ├── ARCHITECTURE.md       # System architecture documentation
│   ├── CODE_STYLE_GUIDELINES.md  # Coding standards
│   ├── DEVELOPER_GUIDE.md    # This guide
│   ├── RESEARCH_METHODS.md   # Research background
│   ├── ROADMAP.md            # Project roadmap
│   ├── SCALING.md             # Performance scaling
│   └── SCIENCE.md            # Scientific foundations
├── easy_usage.md             # Basic usage examples
├── ENHANCED_API_SUMMARY.md    # API summary
├── ENHANCED_CONFIG_SUMMARY.md # Configuration summary
├── FINAL_SUMMARY.md          # Project final summary
├── HOW_TO_USE.md              # Usage instructions
├── IMPLEMENTATION_COMPLETE.md # Implementation status
├── python/                   # Python bindings
├── README.md                 # Project overview
├── src/                      # Source code
│   ├── agent/                # Agent interface
│   ├── brain/                # Core neural computation
│   ├── cognition/           # Cognitive functions
│   ├── core/                 # Core utilities
│   ├── development/          # Development processes
│   ├── dynamics/             # Neural dynamics
│   ├── environment/          # Environment interaction
│   ├── experiments/          # Experimental framework
│   ├── memory/               # Memory systems
│   ├── motor/                # Motor control
│   ├── neuromodulation/       # Neuromodulation
│   ├── plasticity/           # Plasticity mechanisms
│   ├── sensory/              # Sensory processing
│   └── tests/               # Test framework
├── tests/                   # Test suite
├── CMakeLists.txt            # Build configuration
└── pyproject.toml           # Python package configuration
```

## Development Environment Setup

### 1. Initial Setup

#### Clone the Repository
```bash
git clone <repository-url>
cd nlm
```

#### Create Build Directory
```bash
mkdir build
cd build
cmake ..
```

### 2. Build Options

#### Standard Release Build
```bash
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

#### Debug Build
```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

#### With Python Bindings
```bash
mkdir build-python
cd build-python
cmake -DBUILD_PYTHON_BINDINGS=ON ..
cmake --build . --config Release
```

### 3. Running Tests

#### Unit Tests
```bash
# Build tests
cmake --build . --target test

# Run tests
./nlm_tests
# or
ctest --output-on-failure
```

#### Integration Tests
```bash
# Run specific test suites
./nlm_experiment_tests
./nlm_integration_tests
```

#### Python Tests
```bash
# Install Python dependencies
pip install -r python/requirements.txt

# Run Python tests
cd python
pytest test_*.py
```

## Core Development Workflow

### 1. Working with Headers (.hpp)

#### Best Practices
- **Always include the corresponding .cpp file** when modifying headers
- **Provide comprehensive Doxygen documentation** for all public APIs
- **Maintain backward compatibility** when changing interfaces
- **Use forward declarations** instead of including unnecessary headers

#### Example Header Structure
```cpp
#pragma once

#include "../core/Types/Types.hpp"
// Forward declarations
class SomeClass;

namespace nlm {

/**
 * @class ExampleClass
 * @brief Brief description of the class
 * 
 * @details Extended explanation of class purpose and behavior
 */
class ExampleClass {
public:
    /**
     * @brief Constructor with parameter validation
     * 
     * @param param Description of parameter
     * @throws std::invalid_argument if parameter is invalid
     */
    explicit ExampleClass(int param);
    
    // Other methods...
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

### 2. Working with Implementation (.cpp)

#### Best Practices
- **Include only the header file** corresponding to the implementation
- **Add inline documentation** for complex algorithms
- **Use logging macros** for debugging and monitoring
- **Implement proper error handling** and validation

#### Example Implementation Structure
```cpp
#include "ExampleClass.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

ExampleClass::ExampleClass(int param) {
    NLM_LOG_INFO("Creating ExampleClass with parameter: " + std::to_string(param));
    
    // Validate parameter
    if (param <= 0) {
        NLM_LOG_ERROR("Parameter must be positive, got: " + std::to_string(param));
        throw std::invalid_argument("Parameter must be positive");
    }
    
    // Initialize implementation
    pImpl = std::make_unique<Impl>(param);
}

// Other method implementations...

} // namespace nlm
```

### 3. Configuration Management

#### Configuration File Format
Configuration files use a simple key=value format:
```ini
# Example configuration file
# Comments start with # or /

# Neural parameters
neuron_count = 1000
region_count = 4
connection_probability = 0.1

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Simulation parameters
simulation_timestep = 0.001
replay_interval = 100
consolidation_interval = 1000

# Randomization
random_seed = 42

# Logging
log_level = INFO
trace_enabled = false
```

#### Accessing Configuration
```cpp
#include "../core/Config/Config.hpp"

// Get values with type safety
int neuronCount = config->getOr<int>("neuron_count", 1000);
double timestep = config->getOr<double>("simulation_timestep", 0.001);

// Check if key exists
if (config->has("custom_parameter")) {
    auto value = config->get<std::string>("custom_parameter");
}
```

### 4. Testing and Validation

#### Running Tests
```bash
# Build all tests
cmake --build . --target all

# Run all tests
ctest --output-on-failure

# Run specific test suites
./nlm_brain_tests
./nlm_plasticity_tests
./nlm_integration_tests
```

#### Test Structure
```cpp
// Example test file structure
test/test_example.cpp
#include "../brain/ExampleClass.hpp"
#include <gtest/gtest.h>

TEST(ExampleTest, Constructor) {
    EXPECT_NO_THROW(ExampleClass obj(42));
}

TEST(ExampleTest, Functionality) {
    ExampleClass obj(10);
    EXPECT_EQ(obj.getValue(), 10);
}
```

### 5. Debugging and Profiling

#### Logging
```cpp
#include "../core/Logger/Logger.hpp"

// Different log levels
NLM_LOG_FATAL("Fatal error message");
NLM_LOG_ERROR("Error message: %s", error.c_str());
NLM_LOG_WARN("Warning message");
NLM_LOG_INFO("Information message");
NLM_LOG_DEBUG("Debug message");
NLM_LOG_TRACE("Trace message");
```

#### Debug Flags
```cpp
// Enable/disable different logging levels
// Configure in CMake or runtime
// Examples:
// -DNLM_ENABLE_TRACE=1
// -DNLM_LOG_LEVEL=DEBUG
```

### 6. Performance Optimization

#### Common Optimization Techniques
```cpp
// Use references for small objects
const NeuronState& state = neuron->getState();

// Cache frequently accessed data
const float& threshold = neuron->getThreshold();  // Returns reference

// Use move semantics for temporary objects
std::vector<Neuron*> neurons;
neurons.push_back(new Neuron(id));  // Risky - use smart pointers

// Use range-based for loops
for (const auto& neuron : population->getNeurons()) {
    // Process neuron
}
```

## Project-Specific Features

### 1. Neural Dynamics Models

#### Available Models
- **Leaky Integrate-and-Fire (LIF)**: Simple and efficient
- **Hodgkin-Huxley**: Detailed biophysical modeling
- **FitzHugh-Nagumo**: Simplified excitable system

#### Switching Models
```cpp
// Configuration-based model selection
std::string modelType = config->getOr<std::string>("neuron_model", "LIF");

NeuralDynamics* createDynamics(const std::string& type) {
    if (type == "LIF") {
        return new IntegrateAndFireDynamics();
    } else if (type == "HH") {
        return new HodgkinHuxleyDynamics();
    } else if (type == "FHN") {
        return new FitzHughNagumoDynamics();
    }
    return nullptr;
}
```

### 2. Plasticity Rules

#### Supported Plasticity Mechanisms
- **Hebbian**: Fire together, wire together
- **STDP**: Spike-timing-dependent plasticity
- **Anti-Hebbian**: Fire together, wire apart
- **BCM**: Bienenstock-Cooper-Munro learning rule

#### Plasticity Configuration
```cpp
// Enable/disable plasticity rules
plasticitySystem->enableHebbian(true);
plasticitySystem->enableSTDP(true);
plasticitySystem->enableRewardModulated(true);

// Configure parameters
plasticitySystem->setLearningRate(0.01f);
plasticitySystem->setSTDPParameters(ltp, ltd, tau);
```

### 3. Memory Systems

#### Memory Integration
```cpp
// Access memory systems through brain interface
brain->getWorkingMemory()->store(value, timestamp);
brain->getEpisodicMemory()->recordExperience(event);
brain->getAssociativeMemory()->learnAssociation(pattern1, pattern2);
```

#### Memory Configuration
```cpp
// Configure memory capacities and properties
config->set("working_memory_capacity", 1000);
config->set("episodic_memory_duration", 3600.0);  // 1 hour in seconds
config->set("associative_memory_strength", 0.8f);
```

### 4. Neuromodulation

#### Neuromodulator Types
- **Dopamine**: Reward prediction error
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **Prediction Error**: Temporal difference signals

#### Neuromodulation Usage
```cpp
// Apply neuromodulatory signals
brain->applyNeuromodulation(DopamineSignal(reward, prediction_error));
brain->applyNeuromodulation(CuriositySignal(novelty, prediction_error));
```

## Advanced Development

### 1. Extending the System

#### Adding New Neural Dynamics
```cpp
// Step 1: Create new dynamics class
class CustomDynamics : public NeuralDynamics {
public:
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        // Custom implementation
    }
    // Implement other pure virtual methods...
};

// Step 2: Register with factory
void registerCustomDynamics() {
    DynamicsFactory::registerType("Custom", &createCustomDynamics);
}
```

#### Adding New Plasticity Rules
```cpp
// Step 1: Create new plasticity rule class
class CustomPlasticityRule : public PlasticityRule {
public:
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Custom implementation
    }
    const char* getName() const override { return "Custom"; }
};

// Step 2: Register with factory
void registerCustomPlasticity() {
    PlasticityFactory::registerType("Custom", &createCustomPlasticity);
}
```

### 2. Performance Profiling

#### Profiling Tools
```bash
# Linux/macOS profiling
valgrind --tool=callgrind ./nlm_simulation

# Python profiling
python -m cProfile -o profile.stats simulation.py

# Clang static analysis
scan-build -analyze-headers -enable-experimental-new-analysis make
```

#### Performance Monitoring
```cpp
// Performance tracking
struct PerformanceMetrics {
    size_t neuronsProcessed;
    size_t synapsesUpdated;
    size_t spikesGenerated;
    double simulationTime;
    size_t memoryUsage;
};

PerformanceMetrics getPerformanceMetrics() {
    PerformanceMetrics metrics;
    // Collect metrics from various systems
    return metrics;
}
```

### 3. Configuration Extensions

#### Adding New Configuration Types
```cpp
// Extend configuration with custom types
struct CustomConfigValue {
    std::string name;
    std::vector<float> parameters;
};

// Add to ConfigValue variant
using ExtendedConfigValue = std::variant<
    int, int64_t, double, bool, std::string,
    std::vector<int>, std::vector<double>, std::vector<std::string>,
    CustomConfigValue  // Additional type
>;
```

## Contributing

### 1. Code Contributions

#### Pull Request Process
1. **Fork the repository** to your personal account
2. **Create a feature branch** from the main branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make your changes** and commit them:
   ```bash
   git add modified_files
   git commit -m "feat: Your commit message"
   ```
4. **Push to your branch**:
   ```bash
   git push origin feature/your-feature-name
   ```
5. **Create a pull request** through GitHub interface

#### Commit Message Guidelines
```
<type>[scope]: <description>

[optional body]

[optional footer]: Co-authored-by: openhands <openhands@all-hands.dev>

Types:
- feat: New feature
- fix: Bug fix
- docs: Documentation changes
- style: Code style changes
- refactor: Code refactoring
- perf: Performance improvements
- test: Test additions/modifications
- chore: Maintenance tasks
```

### 2. Documentation Contributions

#### Documentation Style
- Follow existing documentation patterns
- Use consistent formatting and terminology
- Include code examples where helpful
- Reference related documentation files

#### Updating Documentation
```bash
# Update API documentation
make update-docs

# Generate documentation
cmake --build . --target docs
```

### 3. Testing Contributions

#### Adding Tests
```cpp
// New test file
tests/test_new_feature.cpp
#include "../your/module/Header.hpp"
#include <gtest/gtest.h>

TEST(YourFeatureTest, BasicFunctionality) {
    EXPECT_TRUE(true);
}
```

#### Running Specific Tests
```bash
# Run tests matching pattern
./nlm_tests --filter "YourFeature"

# Run tests with verbose output
./nlm_tests --verbose
```

## Troubleshooting

### Common Issues

#### Build Errors
```bash
# Error: CMakeLists.txt not found
mkdir build
cd build
cmake ..
```

#### Runtime Errors
```bash
# Error: Configuration file not found
# Check if file exists and permissions
ls -la configs/
cat configs/default.ini
```

#### Performance Issues
```bash
# Memory usage too high
# Try reducing simulation size
# Optimize data structures
# Use profiling tools

# CPU usage high
# Check for infinite loops
# Profile with valgrind
```

### Getting Help

#### Community Support
- **GitHub Issues**: [https://github.com/Kilo-Org/kilocode/issues](https://github.com/Kilo-Org/kilocode/issues)
- **Documentation**: [https://kilo.ai/docs](https://kilo.ai/docs)
- **Discord**: [https://discord.gg/kilocode](https://discord.gg/kilocode)

#### Development Support
- **Project Maintainers**: Contact through GitHub
- **Code Reviews**: Request through pull requests
- **Feature Requests**: Submit through GitHub Issues

## License

This project is licensed under the terms specified in the LICENSE file.

## Acknowledgments

We thank all contributors to the NLM project and the open-source community for their valuable contributions and feedback.

---

**Last Updated**: 2026-10-20
**Version**: 1.0
**Authors**: NLM Development Team

This guide will be continuously updated as the project evolves and new development workflows are established.