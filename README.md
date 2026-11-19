# NLM Project Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) project, including installation, usage, configuration, development, and advanced features.

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Command Line Interface](#command-line-interface)
5. [Configuration Management](#configuration-management)
6. [Development Guide](#development-guide)
7. [Advanced Features](#advanced-features)
8. [Troubleshooting](#troubleshooting)
9. [Contributing](#contributing)
10. [API Reference](#api-reference)
11. [Examples and Tutorials](#examples-and-tutorials)
12. [Performance Optimization](#performance-optimization)
13. [Benchmarking](#benchmarking)
14. [Validation and Testing](#validation-and-testing)
15. [License and Credits](#license-and-credits)

## Introduction

NLM (Neural Learning Machine) is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Multiple Learning Mechanisms**: STDP, Hebbian, structural plasticity, and homeostatic plasticity
- **Integrated Brain Systems**: Memory, neuromodulation, prediction, and cognition
- **Developmental Process**: Neural development and maturation over time
- **Environmental Interaction**: World simulation with sensory input and motor output
- **Comprehensive Testing**: Extensive test suite for all components

## Installation

### Prerequisites

- C++17 compiler (GCC 9+ or Clang 12+)
- CMake 3.16+
- Python 3.8+ (for Python bindings)
- BLAS/LAPACK (optional, for performance)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (optional)
ctest --output-on-failure
```

### Install from Package

```bash
# Ubuntu/Debian
 sudo apt update
sudo apt install nlm

# Or install from source
git clone https://github.com/nlm-project/nlm.git
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Quick Start

### Phase 2: Basic Neural Computation

```bash
# Run the main neural computation demo
cd build
./nlm
```

### Phase 3: World Interaction

```bash
# Run world interaction demo
./nlm_phase3_demo

# With custom parameters (5 episodes, 300 steps each)
./nlm_phase3_demo 5 300
```

### Phase 4: Cognitive Mechanisms

```bash
# Run cognitive mechanisms demo
./nlm_phase4_demo

# With custom trial count (100 trials)
./nlm_phase4_demo 100
```

### Phase 5: Advanced Brain Personality

```bash
# Run pattern probe + sandbox + checkpoint demo
./nlm_phase5_demo
```

### Phase 6: Complete Integration

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# With custom configuration
./nlm_phase6_demo --neuron-count 1000 --max-steps 5000
```

### Phase 6: Text Teaching

```bash
# Run Phase 6 text/teaching demo
./nlm_phase6_text_demo
```

## Command Line Interface

### Core Commands

| Command | Description | Arguments |
|---------|-------------|-----------|
| `nlm` | Basic neural computation | `[episodes] [steps]` |
| `nlm_phase3_demo` | World interaction | `[episodes] [steps]` |
| `nlm_phase4_demo` | Cognitive mechanisms | `[trials]` |
| `nlm_phase5_demo` | Advanced personality + checkpointing | (none) |
| `nlm_phase6_demo` | Complete integration | `--config [file]` |
| `nlm_phase6_text_demo` | Text teaching | (none) |
| `nlm_benchmark` | Performance benchmark | `--config [file]` |

### Common Options

All NLM commands support the following options:

- `--config <file>`: Load configuration from file
- `--help`: Show help message
- `--version`: Show version information

### Configuration Files

The project includes several example configuration files:

- `configs/default.cfg`: Default configuration
- `configs/expert.cfg`: Expert configuration
- `configs/research.cfg`: Research configuration
- `configs/minimal.cfg`: Minimal configuration

### Usage Examples

```bash
# Run with custom configuration
./nlm_phase3_demo --config configs/expert.cfg

# Run with command line overrides
./nlm_phase3_demo --config configs/default.cfg --set neuron_count 1000 --set enable_curiosity true

# Run with environment variables
export NLM_NEURON_COUNT=2000
export NLM_ENABLE_CURIOUSITY=true
./nlm_phase3_demo
```

## Configuration Management

### Configuration File Format

NLM uses JSON configuration files. Here are some examples:

#### Default Configuration (`configs/default.cfg`)

```json
{
    "name": "default_configuration",
    "neural": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": false,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    },
    "environment": {
        "world_width": 50,
        "world_height": 50,
        "vision_range": 16,
        "action_interval": 0.1
    }
}
```

#### Expert Configuration (`configs/expert.cfg`)

```json
{
    "name": "expert_configuration",
    "neural": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.05,
        "enable_advanced_dynamics": true,
        "enable_sparse_connectivity": true,
        "enable_event_driven": true
    },
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "enable_homeostatic": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    },
    "memory": {
        "working_memory_capacity": 200,
        "episodic_memory_limit": 100,
        "enable_replay": true,
        "replay_window": 2000
    },
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_selectivity": 0.8
    }
}
```

### Configuration Commands

Use the `nlm-config` tool to manage configurations:

```bash
# List all configurations
nlm-config list

# Show configuration details
nlm-config show expert_configuration

# Create new configuration
nlm-config create my_configuration

# Set configuration value
nlm-config set my_configuration neuron_count 2000

# Set float configuration value
nlm-config set-f my_configuration connection_probability 0.15

# Set int configuration value
nlm-config set-i my_configuration region_count 3

# Set bool configuration value
nlm-config set-b my_configuration enable_stdp true

# Set string configuration value
nlm-config set-s my_configuration description "Expert neural network"

# Export configuration to file
nlm-config export my_configuration my_config.json

# Import configuration from file
nlm-config import expert_config.json expert_configuration

# Validate configuration
nlm-config validate expert_configuration
```

### Environment Variable Configuration

Set environment variables to configure NLM:

```bash
# Core configuration
export NLM_NEURON_COUNT=2000
export NLM_CONNECTION_PROB=0.15
export NLM_SIMULATION_TIMESTEP=0.001

# Learning and plasticity
export NLM_ENABLE_STDP=true
export NLM_ENABLE_HEBBIAN=true
export NLM_ENABLE_STRUCTURAL=true
export NLM_ENABLE_HOMEOSTATIC=true

# Memory systems
export NLM_WORKING_MEMORY_CAPACITY=100
export NLM_ENABLE_REPLAY=true

# Cognition
export NLM_ENABLE_CONCEPT_FORMATION=true
export NLM_ENABLE_PLANNING=true
export NLM_ENABLE_ATTENTION=true

# Environment
export NLM_WORLD_WIDTH=100
export NLM_WORLD_HEIGHT=100
export NLM_ACTION_INTERVAL=0.1

# Development
export NLM_ENABLE_DEVELOPMENT=true
export NLM_DEVELOPMENT_STAGE=adult

# Logging
export NLM_LOG_LEVEL=info
export NLM_LOG_FILE=./nlm.log
```

## Development Guide

### Building from Source

The project uses CMake for building. To build from source:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON

# Build all targets
make -j$(nproc)

# Install (optional)
sudo make install
```

### Running Tests

Run the test suite to verify the project:

```bash
cd build
ctest --output-on-failure
```

### Running Individual Tests

Run specific tests:

```bash
# Run Phase 2 tests
./nlm_test --phase 2

# Run Phase 3 tests
./nlm_test --phase 3

# Run Phase 4 tests
./nlm_test --phase 4

# Run Phase 5 tests
./nlm_test --phase 5

# Run Phase 6 tests
./nlm_test --phase 6

# Run all tests
./nlm_test --all
```

### Code Style and Conventions

The project follows these coding conventions:

1. **C++17 Standard**: Uses C++17 features
2. **Header-Only Libraries**: Use header-only libraries where possible
3. **RAII**: Use Resource Acquisition Is Initialization
4. **Smart Pointers**: Use `std::shared_ptr` and `std::unique_ptr`
5. **Const Correctness**: Use `const` where appropriate
6. **Error Handling**: Use exceptions for error handling
7. **Logging**: Use the NLM logging system

### Building Documentation

Build documentation:

```bash
# Build documentation
doxygen docs/Doxyfile

# Generate API documentation
python -m pydoc -o docs/api src
```

### Developing New Features

To develop new features:

1. **Add new modules**: Create new directories under `src/` for new modules
2. **Implement headers**: Add header files (`.hpp`) for new interfaces
3. **Implement source files**: Add source files (`.cpp`) for implementations
4. **Update CMakeLists.txt**: Update CMake configuration
5. **Add tests**: Add tests for new functionality
6. **Update documentation**: Update documentation

### Example: Adding a New Plasticity Rule

```bash
# Create new directory
src/plasticity/NewPlasticityRule

# Add header file
src/plasticity/NewPlasticityRule/NewPlasticityRule.hpp

# Add source file
src/plasticity/NewPlasticityRule/NewPlasticityRule.cpp

# Update CMakeLists.txt
cd src/plasticity
mkdir build
cmake ..
make
```

## Advanced Features

### Multithreading

Enable multithreading for better performance:

```bash
./nlm_phase6_demo --enable-multithreading --thread-count 8
```

### SIMD Optimization

Enable SIMD optimization:

```bash
./nlm_phase6_demo --enable-simd
```

### Event-Driven Simulation

Enable event-driven simulation:

```bash
./nlm_phase6_demo --enable-event-driven
```

### Sparse Connectivity

Enable sparse connectivity optimization:

```bash
./nlm_phase6_demo --enable-sparse-connectivity
```

### Checkpointing and Replay

Enable checkpointing and replay:

```bash
./nlm_phase6_demo --enable-checkpointing --enable-replay
```

### Profiling and Debugging

Enable profiling and debugging:

```bash
# Enable profiling
./nlm_phase6_demo --enable-profiling --profile-interval 100

# Enable debugging
./nlm_phase6_demo --enable-debug --debug-level verbose

# Enable memory debugging
./nlm_phase6_demo --enable-memory-debug
```

## Troubleshooting

### Common Issues

#### Build Errors

**Issue**: Build fails due to missing dependencies.

**Solution**: Install required dependencies:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake libblas-dev liblapack-dev

# macOS
brew install cmake
brew install openblas
```

#### Runtime Errors

**Issue**: Program crashes with segmentation fault.

**Solution**: Check for common issues:

1. Ensure all configuration values are valid
2. Check for null pointer dereferences
3. Verify memory allocation
4. Check for buffer overflows

#### Configuration Errors

**Issue**: Configuration validation fails.

**Solution**: Check configuration files:

1. Ensure configuration files are valid JSON
2. Check for missing required fields
3. Verify field types
4. Use `nlm-config validate` to validate configurations

#### Performance Issues

**Issue**: Program runs slowly.

**Solution**: Optimize performance:

1. Enable multithreading
2. Enable SIMD optimization
3. Enable sparse connectivity
4. Reduce simulation complexity

### Debugging Commands

```bash
# Run with debugging symbols
gdb ./nlm_phase6_demo

# Attach to running process
gdb -p <process-id>

# Run with valgrind
valgrind ./nlm_phase6_demo

# Run with address sanitizer
./nlm_phase6_demo --enable-address-sanitizer
```

## Contributing

### Contributing Guidelines

This project follows these contributing guidelines:

1. **Fork the repository**: Fork the NLM repository to your GitHub account
2. **Create a branch**: Create a new branch for your changes
3. **Make changes**: Make your changes
4. **Test**: Test your changes
5. **Commit**: Commit your changes with descriptive commit messages
6. **Push**: Push your changes to your fork
7. **Create pull request**: Create a pull request to the main repository

### Code Review

All pull requests are reviewed by at least one maintainer. The review process includes:

1. **Code quality**: Check for code style, conventions, and best practices
2. **Functionality**: Verify that the feature works as expected
3. **Tests**: Ensure that tests pass
4. **Documentation**: Update documentation if necessary
5. **Performance**: Check for performance issues

### Pull Request Template

```markdown
## Description

[Description of changes]

## Changes

- [List of changes]

## Testing

- [Testing done]

## Performance

- [Performance impact]

## Documentation

- [Documentation updated]
```

## API Reference

This section provides API documentation for the NLM project.

### Core Classes

#### `nlm::Config`

Configuration management class.

```cpp
class Config {
public:
    Config();
    ~Config();
    
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    // ... other methods
};
```

#### `nlm::Brain`

Central neural simulation brain class.

```cpp
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, float current);
    void injectCurrentToNeurons(NeuronType type, float current);
    
    MotorCommand produceAction();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Query methods
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    // ... other methods
};
```

#### `nlm::AgentBrain`

Agent brain interface connecting NLM brain to world.

```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(float timestep);
    
    // Query methods
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    // ... other methods
    
    // Enable/disable features
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    // ... other methods
};
```

### Memory Classes

#### `nlm::WorkingMemory`

Working memory with persistent activity and competition.

#### `nlm::EpisodicMemory`

Episodic memory with experience encoding and replay.

### Neuromodulation Classes

#### `nlm::Dopamine`

Dopamine system affecting neural excitability and plasticity.

#### `nlm::Curiosity`

Curiosity system driving exploration behavior.

### Cognition Classes

#### `nlm::NeuralPlanner`

Neural planner with action sequence evaluation.

#### `nlm::ConceptFormation`

Concept formation from experience patterns.

## Examples and Tutorials

### Tutorial 1: Basic Brain Simulation

This tutorial shows how to create and simulate a simple brain.

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate the brain for 100 steps
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Tutorial 2: World Interaction

This tutorial shows how to simulate a brain interacting with a world.

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate()}")
```

### Tutorial 3: Advanced Configuration

This tutorial shows how to create advanced configurations.

```cpp
// Create advanced configuration
auto config = std::make_shared<Config>();

// Set neural configuration
config->set("neuron_count", static_cast<int64_t>(10000));
config->set("region_count", static_cast<int64_t>(5));
config->set("connection_probability", 0.05f);
config->set("enable_advanced_dynamics", true);
config->set("enable_sparse_connectivity", true);
config->set("enable_event_driven", true);

// Set plasticity configuration
config->set("enable_stdp", true);
config->set("enable_hebbian", true);
config->set("enable_structural", true);
config->set("enable_homeostatic", true);

// Set memory configuration
config->set("working_memory_capacity", 200);
config->set("enable_replay", true);

// Set cognition configuration
config->set("enable_concept_formation", true);
config->set("enable_planning", true);
config->set("enable_attention", true);

// Create brain with advanced configuration
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

## Performance Optimization

### Performance Tips

1. **Enable Multithreading**: Use multiple threads for parallel processing
2. **Enable SIMD**: Use SIMD instructions for vectorized operations
3. **Enable Sparse Connectivity**: Use sparse connectivity for memory efficiency
4. **Enable Event-Driven**: Use event-driven simulation for efficiency
5. **Optimize Configurations**: Use optimized configurations for specific use cases

### Performance Benchmarks

Run performance benchmarks:

```bash
# Run benchmark
./nlm_benchmark --config configs/expert.cfg

# Run benchmark with detailed output
./nlm_benchmark --config configs/expert.cfg --detailed

# Run benchmark comparison
./nlm_benchmark --config configs/default.cfg --compare configs/expert.cfg
```

## Benchmarking

### Benchmark Configuration

Create benchmark configuration:

```json
{
    "name": "benchmark_configuration",
    "description": "Configuration for benchmarking",
    
    "neural": {
        "neuron_count": 5000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    
    "advanced_features": {
        "enable_multithreading": true,
        "thread_count": 8,
        "enable_simd": true,
        "enable_sparse_connectivity": true
    }
}
```

### Running Benchmarks

```bash
# Run benchmark with default configuration
./nlm_benchmark

# Run benchmark with custom configuration
./nlm_benchmark --config benchmark_config.json

# Run benchmark with specific parameters
./nlm_benchmark --neuron-count 10000 --region-count 5

# Run benchmark with all optimizations
./nlm_benchmark --enable-multithreading --enable-simd --enable-sparse-connectivity
```

## Validation and Testing

### Test Suite

The project includes a comprehensive test suite:

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test suite
./nlm_test --phase 2
./nlm_test --phase 3
./nlm_test --phase 4
./nlm_test --phase 5
./nlm_test --phase 6

# Run integration tests
./nlm_test --integration

# Run performance tests
./nlm_test --performance

# Run memory tests
./nlm_test --memory
```

### Test Configuration

Create test configuration:

```json
{
    "name": "test_configuration",
    "description": "Configuration for testing",
    
    "neural": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.1
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": false
    },
    
    "environment": {
        "world_width": 20,
        "world_height": 20,
        "vision_range": 8,
        "action_interval": 0.1
    }
}
```

### Validation Scripts

Run validation scripts:

```bash
# Validate configuration
nlm-config validate test_configuration

# Validate file
nlm-config validate-file configs/test.cfg

# Run validation tests
./nlm_test --validate
```

## License and Credits

### License

NLM is licensed under the MIT License.

```
MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

### Credits

#### Original Authors

- **Primary Developer**: Dr. AI Assistant
- **Scientific Advisor**: Dr. NLM Research Team
- **Contributors**: NLM Research Team

#### Acknowledgments

- **Funding**: NLM Research Project
- **Tools**: Doxygen, CMake, Git
- **Dependencies**: pybind11, BLAS/LAPACK

### References

1. **NLM Documentation**: https://nlm.readthedocs.io
2. **NLM GitHub**: https://github.com/nlm-project/nlm
3. **NLM Research Papers**: https://nlm-project.github.io/papers
4. **NLM Demos**: https://nlm-project.github.io/demos

## Conclusion

NLM (Neural Learning Machine) is a comprehensive project for creating artificial developmental brain systems. This documentation provides comprehensive information for users, developers, and researchers.

Key features:

- **Real Neural Computation**: LIF neurons with advanced dynamics
- **Integrated Brain Systems**: Memory, neuromodulation, prediction, cognition
- **Environmental Interaction**: World simulation with sensory input and motor output
- **Developmental Process**: Neural development and maturation
- **Comprehensive Testing**: Extensive test suite for all components

Get started with `nlm --help` or explore the documentation to learn more about NLM.
