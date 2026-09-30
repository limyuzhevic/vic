# NLM Usage Guide

## Quick Start

This guide provides step-by-step instructions for getting started with the enhanced NLM (熙然) artificial brain system. It covers installation, basic usage, and advanced features.

## Prerequisites

### System Requirements
- **Operating System**: Linux, macOS, or Windows (64-bit)
- **Compiler**: C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19+)
- **CMake**: Version 3.16 or higher
- **Memory**: At least 2 GB RAM for basic simulations
- **Storage**: At least 1 GB free space for build and temporary files

### Build Tools
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential cmake git

# Install dependencies (macOS)
brew install cmake

# Install dependencies (Windows)
# Download from https://cmake.org/download/
```

## Installation and Building

### Standard Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Build with Tests
```bash
# Build with tests enabled
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON
make -j4

# Run tests
ctest
```

### Build with All Features
```bash
# Build with all optional features
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PYTHON_BINDINGS=ON -DENABLE_VISUALIZATION=ON
make -j4
```

### Installation
```bash
# Install to system directories
sudo make install

# Or install to local directory
make install DESTDIR=/path/to/install
```

## Basic Usage

### Running the Main Simulation
```bash
# Run the basic simulation
./nlm

# Run with custom configuration
./nlm --config custom.cfg

# Run with command line parameters
./nlm --neuron_count 5000 --region_count 4 --simulation_timestep 0.0005
```

### Running Integration Tests
```bash
# Run all integration tests
./nlm_phase6_demo

# Run specific test scenarios
./nlm_phase6_demo --scenario basic
./nlm_phase6_demo --scenario plasticity
./nlm_phase6_demo --scenario memory
```

### Running Unit Tests
```bash
# Build and run unit tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run unit tests
./nlm_test

# Or run with ctest
ctest -V
```

## Configuration Management

### Using Default Configuration
```bash
# Run with default configuration
./nlm

# View default configuration
cat configs/default.cfg
```

### Creating Custom Configuration
```bash
# Create custom configuration
cp configs/default.cfg my_config.cfg

# Edit configuration
nano my_config.cfg

# Or create from template
./nlm --generate-config my_config.cfg
```

### Configuration Examples

#### Minimal Configuration
```bash
# Basic configuration
./nlm --neuron_count 100 --region_count 1
```

#### Advanced Configuration
```bash
# Advanced configuration with detailed parameters
./nlm \
    --config research_config.cfg \
    --simulation_timestep 0.0005 \
    --max_simulation_steps 100000 \
    --plasticity_learning_rate 0.02 \
    --stdp_ltp_weight 0.015 \
    --stdp_ltd_weight 0.018
```

### Configuration Validation
```bash
# Validate configuration file
python3 validate_config.py my_config.cfg

# View configuration help
./nlm --help-configuration
```

## Advanced Features

### Using Enhanced Neuron Integration
```cpp
// Include enhanced neuron headers
#include "brain/Neuron.hpp"
#include "brain/NeuronIntegration.hpp"

// Create and configure neuron
nlm::Neuron neuron(nlm::NeuronId(1));
nlm::NeuronIntegrator integrator(neuron);

// Configure timestep
integrator.setTimestep(0.001f);  // 1ms

// Run simulation
nlm::Timestamp timestamp = 0.0f;
for (int step = 0; step < 1000; ++step) {
    neuron.step(timestamp);
    timestamp += 0.001f;
}
```

### Using Enhanced Synapse Integration
```cpp
// Include enhanced synapse headers
#include "brain/Synapse.hpp"
#include "brain/SynapseIntegration.hpp"

// Create and configure synapse
nlm::Synapse synapse(nlm::SynapseId(1), src, dst);
nlm::SynapseIntegrator integrator(synapse);

// Configure synaptic parameters
integrator.setFacilitationTimeConstant(150.0f);
integrator.setDepressionTimeConstant(250.0f);
integrator.setEligibilityDecayRate(0.0005f);

// Run simulation
nlm::Timestamp timestamp = 0.0f;
for (int step = 0; step < 1000; ++step) {
    synapse.step(timestamp);
    timestamp += 0.001f;
}
```

### Using Configuration-Based Synaptic Dynamics
```cpp
// Include configuration headers
#include "core/Config/Config.hpp"
#include "brain/Synapse.hpp"

// Create configuration
auto config = std::make_shared<nlm::Config>();

// Configure synaptic parameters
config->set("synaptic_facilitation_tau", 150.0f);
config->set("synaptic_depression_tau", 250.0f);
config->set("eligibility_decay_rate", 0.0005f);
config->set("min_efficacy", 0.5f);
config->set("max_efficacy", 1.5f);

// Create synapse with configuration
nlm::Synapse synapse(nlm::SynapseId(1), src, dst);

// Use configuration in simulation
nlm::Timestamp timestamp = 0.0f;
for (int step = 0; step < 1000; ++step) {
    synapse.step(timestamp, config.get());
    timestamp += 0.001f;
}
```

### Using Brain Integration Tests
```cpp
// Include brain integration headers
#include "brain/Brain.hpp"
#include "tests/BrainIntegrationTester.hpp"

// Create brain with configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
config->set("region_count", 4);
config->set("connection_probability", 0.15);

// Create brain and initialize
nlm::Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Failed to initialize brain" << std::endl;
    return 1;
}

// Run integration tests
BrainIntegrationTester tester(brain);

// Run basic integration test
if (!tester.runBasicIntegration()) {
    std::cerr << "Basic integration test failed" << std::endl;
    return 1;
}

// Run plasticity integration test
if (!tester.runPlasticityIntegration()) {
    std::cerr << "Plasticity integration test failed" << std::endl;
    return 1;
}

// Run memory integration test
if (!tester.runMemoryIntegration()) {
    std::cerr << "Memory integration test failed" << std::endl;
    return 1;
}
```

## Development Guide

### Creating New Configurations
```bash
# Create configuration from template
./nlm --generate-config research_config.cfg

# Edit configuration
nano research_config.cfg

# Validate configuration
./nlm --validate-config research_config.cfg
```

### Creating Custom Neurons
```cpp
// Create custom neuron implementation
class CustomNeuron : public nlm::Neuron {
public:
    CustomNeuron(nlm::NeuronId id) : nlm::Neuron(id) {}
    
    void stepCustomIntegration(nlm::Timestamp timestamp, nlm::TimestepDuration dt) {
        // Implement custom integration logic
        integrate(timestamp, dt);
        applyCustomPlasticity(timestamp);
    }
    
private:
    void integrate(nlm::Timestamp timestamp, nlm::TimestepDuration dt);
    void applyCustomPlasticity(nlm::Timestamp timestamp);
};
```

### Creating Custom Synapses
```cpp
// Create custom synapse implementation
class CustomSynapse : public nlm::Synapse {
public:
    CustomSynapse(nlm::SynapseId id, nlm::NeuronId src, nlm::NeuronId dst) 
        : nlm::Synapse(id, src, dst) {}
    
    void stepCustomIntegration(nlm::Timestamp timestamp, const nlm::Config* config) {
        // Implement custom integration logic
        applyCustomDynamics(timestamp, config);
        updateCustomState(timestamp);
    }
    
private:
    void applyCustomDynamics(nlm::Timestamp timestamp, const nlm::Config* config);
    void updateCustomState(nlm::Timestamp timestamp);
};
```

### Running Development Tests
```bash
# Run development tests
./nlm_test --dev-mode

# Run with verbose output
./nlm_test --verbose

# Run specific test category
./nlm_test --category neuron-integration
./nlm_test --category synapse-integration
./nlm_test --category brain-integration
```

## Troubleshooting

### Common Issues and Solutions

1. **Build Errors**
```
# Error: CMakeLists.txt not found
# Solution: Run cmake .. in build directory

# Error: Compiler not found
# Solution: Install C++ compiler (g++, clang++, or MSVC)
```

2. **Runtime Errors**
```
# Error: Configuration file not found
# Solution: Specify correct configuration file or use default

# Error: Invalid parameter values
# Solution: Check configuration file and parameter ranges
```

3. **Performance Issues**
```
# Problem: Simulation too slow
# Solution: Reduce neuron_count or increase simulation_timestep

# Problem: High memory usage
# Solution: Reduce max_simulation_steps or neuron_count
```

### Debugging Commands
```bash
# Debug build with symbols
trybuild/build_dir
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with debug output
./nlm --debug --log-level DEBUG

# Capture debugging information
./nlm --capture-debug debug_info.txt
```

## Example Workflows

### Research Workflow
```bash
# 1. Set up research environment
mkdir research_build
cd research_build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# 2. Run research simulation
./nlm --config research_config.cfg --analysis-mode

# 3. Process results
python3 process_research_results.py

# 4. Compare with baseline
./compare_results.py baseline_results.csv
```

### Development Workflow
```bash
# 1. Create new feature
mkdir dev_branch
cd dev_branch

# 2. Implement and test
./nlm_test --category my-new-feature

# 3. Integrate
./nlm --enable-new-feature

# 4. Document
./generate-documentation.py
```

### Performance Testing Workflow
```bash
# 1. Benchmark
mkdir perf_build
cd perf_build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8

# 2. Run performance test
./nlm_benchmark --iterations 100 --neuron_count 1000

# 3. Analyze results
./analyze_benchmark.py benchmark_results.csv
```

## Command Reference

### nlm (Main Application)
```bash
./nlm [OPTIONS]

Options:
  --config FILE              Configuration file path
  --neuron_count N           Number of neurons
  --region_count N           Number of regions
  --simulation_timestep T    Simulation timestep
  --max_simulation_steps S   Maximum simulation steps
  --plasticity_learning_rate R
  --stdp_ltp_weight R
  --stdp_ltd_weight R
  --dopamine_baseline R
  --help                     Show help information
  --version                  Show version information
```

### nlm_test (Unit Tests)
```bash
./nlm_test [OPTIONS]

Options:
  --category CATEGORY        Test category
  --verbose                  Verbose output
  --dev-mode                 Development mode
  --help                     Show help information
```

### nlm_phase6_demo (Integration Demo)
```bash
./nlm_phase6_demo [OPTIONS]

Options:
  --scenario SCENARIO        Test scenario
  --duration D               Duration in seconds
  --neurons N                Number of neurons
  --regions R                Number of regions
  --help                     Show help information
```

## Best Practices

### Configuration Best Practices
1. **Use configuration files** for complex settings
2. **Document all parameters** used in research
3. **Use different configurations** for different research questions
4. **Validate configuration** before running simulations
5. **Use descriptive filenames** for configuration files

### Development Best Practices
1. **Write comprehensive tests** for all new features
2. **Use consistent naming conventions** for variables and functions
3. **Document API changes** in comments and documentation
4. **Use configuration-based** rather than hard-coded parameters
5. **Test on multiple platforms** to ensure portability

### Performance Best Practices
1. **Profile simulations** to identify bottlenecks
2. **Use appropriate timestep** for research requirements
3. **Optimize for your target hardware**
4. **Monitor memory usage** during long simulations
5. **Use parallel processing** for large simulations

## Migration Guide

### From Previous Versions

#### Upgrading Configuration
```bash
# OLD: Basic configuration
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# NEW: Enhanced configuration with improved defaults
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=ON -DENABLE_PYTHON_BINDINGS=ON
make
```

#### Upgrading Code
```cpp
// OLD: Basic neuron integration
neuron.step(timestamp);

// NEW: Enhanced neuron integration with configurable timestep
nlm::NeuronIntegrator integrator(neuron);
integrator.setTimestep(0.001f);  // Configure timestep
integrator.step(timestamp);
```

#### Upgrading Synapse Integration
```cpp
// OLD: Basic synapse integration
synapse.step(timestamp);

// NEW: Enhanced synapse integration with configuration
nlm::SynapseIntegrator integrator(synapse);
integrator.setFacilitationTimeConstant(150.0f);
integrator.setDepressionTimeConstant(250.0f);
integrator.step(timestamp, config.get());
```

## Conclusion

This guide provides comprehensive instructions for getting started with the enhanced NLM system. The improvements include:

1. **Enhanced neuron integration** with configurable timesteps
2. **Enhanced synapse integration** with configuration-based parameters
3. **Simplified build system** with automatic source discovery
4. **Comprehensive testing** with integration tests
5. **Better documentation** with clear examples and best practices

The NLM system is now more flexible, configurable, and production-ready than ever before. Use these improvements to conduct more sophisticated research and development with artificial brain systems.

For support and additional information, visit the NLM documentation website or contact the development team.