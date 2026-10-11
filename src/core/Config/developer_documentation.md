# NLM Developer Documentation

This document provides comprehensive guidance for developers working with the NLM (熙然) project, including build instructions, testing procedures, troubleshooting tips, and performance optimization recommendations.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Building the Project](#building-the-project)
3. [Running Experiments](#running-experiments)
4. [Testing Procedures](#testing-procedures)
5. [Configuration Management](#configuration-management)
6. [Troubleshooting](#troubleshooting)
7. [Performance Optimization](#performance-optimization)
8. [Code Contributions](#code-contributions)
9. [Advanced Usage](#advanced-usage)
10. [Maintenance Guidelines](#maintenance-guidelines)

## Getting Started

### Prerequisites

The NLM project requires the following software:

- **Compiler**: C++17 or later compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake**: Version 3.10 or higher
- **Make/Ninja**: Build system
- **Git**: Version control

### Platform Support

- **Linux**: Fully supported
- **macOS**: Supported (with some dependencies)
- **Windows**: Supported via WSL or native compilation

### Quick Start

1. **Clone the repository**
```bash
git clone <repository-url>
cd nlm
```

2. **Build the project**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Run the demo**
```bash
./nlm_phase6_demo
```

4. **Run tests**
```bash
./test_runner
```

## Building the Project

### Build Types

The NLM project supports multiple build types:

- **Debug**: Maximum debugging information, no optimizations
- **Release**: Optimized build with performance in mind
- **RelWithDebInfo**: Release with debug symbols
- **MinSizeRel**: Optimized for minimal binary size

### Configuration Options

CMake provides several configuration options:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
  -DBUILD_TESTING=ON \
  -DBUILD_EXAMPLES=ON \
  -DENABLE_VISUALIZATION=OFF
```

**Configuration Options:**

- `CMAKE_BUILD_TYPE`: Build type (Debug, Release, etc.)
- `CMAKE_CXX_STANDARD`: C++ standard version
- `BUILD_TESTING`: Enable/disable tests
- `BUILD_EXAMPLES`: Enable/disable example programs
- `ENABLE_VISUALIZATION`: Enable/disable visualization
- `RANDOM_SEED`: Default random seed
- `SIMULATION_STEPS`: Default maximum steps

### Build Commands

#### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Debug Build
```bash
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Build with Tests
```bash
mkdir build_tests
cd build_tests
cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Installation
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Performance Tips

1. **Parallel Building**: Use `-j` flag for multiple cores
2. **Caching**: Use ccache for faster incremental builds
3. **Incremental Builds**: Clean only changed files
4. **Dependencies**: Pre-download external dependencies

## Running Experiments

### Default Demo

The NLM project includes a comprehensive demo that exercises all major systems:

```bash
./nlm_phase6_demo
```

**What the demo does:**
- Initializes a complete neural brain
- Runs through all developmental stages
- Simulates sensory processing and motor control
- Demonstrates learning through plasticity
- Shows neuromodulation effects
- Displays final capabilities

### Custom Experiment Scripts

NLM includes several example scripts:

1. **Basic Brain Test**
```bash
./test_basic_brain
```

2. **Learning Experiment**
```bash
./test_learning
```

3. **Development Experiment**
```bash
./test_development
```

4. **Neuromodulation Test**
```bash
./test_neuromodulation
```

### Experiment Command Line Options

Most NLM programs accept the following command-line arguments:

```bash
./nlm_phase6_demo --help
```

**Common Options:**

- `--random_seed=N`: Set random seed
- `--steps=N`: Maximum simulation steps
- `--time_limit=N`: Maximum simulation time (seconds)
- `--config=FILE`: Load configuration from file
- `--verbose`: Enable verbose output
- `--save_state=FILE`: Save brain state to file
- `--load_state=FILE`: Load brain state from file

### Example Usage

```bash
# Run a specific experiment with custom configuration
./nlm_phase6_demo \
  --random_seed=12345 \
  --steps=5000 \
  --time_limit=60.0 \
  --config=custom_config.cfg \
  --save_state=experiment_1.brain

# Run with detailed logging
./test_learning \
  --verbose \
  --log_to_file=true \
  --log_filename=learning_test.log
```

## Testing Procedures

### Overview

The NLM project includes comprehensive tests covering:

1. **Unit Tests**: Individual components and algorithms
2. **Integration Tests**: System interactions and interfaces
3. **Performance Tests**: Computational efficiency and scalability
4. **Regression Tests**: Consistency across versions

### Running Tests

#### Test Runner Script
```bash
./test_runner [--all] [--unit] [--integration] [--performance]
```

**Options:**

- `--all`: Run all tests
- `--unit`: Run unit tests only
- `--integration`: Run integration tests only
- `--performance`: Run performance tests only
- `--verbose`: Enable verbose output
- `--timeout=N`: Maximum test duration (seconds)

#### Individual Test Programs

**Unit Tests:**
- `./test_neuron`: Neuron dynamics and LIF model
- `./test_synapse`: Synaptic transmission and plasticity
- `./test_stdp`: Spike-timing-dependent plasticity
- `./test_hebbian`: Hebbian learning
- `./test_config`: Configuration management
- `./test_random`: Random number generation

**Integration Tests:**
- `./test_plasticity`: Plasticity rule integration
- `./test_neuromodulation`: Neuromodulator systems
- `./test_memory`: Memory system integration
- `./test_prediction`: Prediction system integration

**Performance Tests:**
- `./benchmark`: Performance benchmarking
- `./stress_test`: Stress testing
- `./scalability_test`: Scalability analysis

### Test Output Format

Tests produce output in a structured format:

```
=== Test Results ===
Test Suite: <name>
Status: <PASSED|FAILED|ERROR>
Duration: <seconds>
Tests Run: <number>
Passed: <number>
Failed: <number>
Skipped: <number>

=== Details ===
Test: <test_name>
Result: <PASS|FAIL|ERROR>
Message: <optional message>
Time: <seconds>

=== Summary ===
Overall Status: <PASS|FAIL>
Total Tests: <number>
Total Time: <seconds>
```

### Writing Tests

#### Unit Test Structure

Example unit test structure:

```cpp
#include <gtest/gtest.h>
#include "src/neuron/LIFNeuron.hpp"

TEST(LIFNeuronTest, Initialization) {
    LIFNeuron neuron(0.0f, -0.07f, 0.02f);
    
    EXPECT_FLOAT_EQ(neuron.getThreshold(), 0.0f);
    EXPECT_FLOAT_EQ(neuron.getResetVoltage(), -0.07f);
    EXPECT_FLOAT_EQ(neuron.getMembraneTimeConstant(), 0.02f);
    EXPECT_FALSE(neuron.didFire());
}

TEST(LIFNeuronTest, SpikeGeneration) {
    LIFNeuron neuron(0.0f, -0.07f, 0.02f);
    
    // Simulate input to trigger spike
    neuron.addInput(0.1f);
    neuron.update(0.001f);
    
    EXPECT_TRUE(neuron.didFire());
}

TEST(LIFNeuronTest, RefractoryPeriod) {
    LIFNeuron neuron(0.0f, -0.07f, 0.02f);
    
    // Trigger spike
    neuron.addInput(0.1f);
    neuron.update(0.001f);
    
    // Should not fire again immediately
    neuron.addInput(0.1f);
    neuron.update(0.001f);
    
    EXPECT_FALSE(neuron.didFire());
}
```

#### Test Best Practices

1. **Deterministic Tests**: Use fixed random seeds for reproducibility
2. **Isolated Tests**: Each test should be independent
3. **Clear Assertions**: Use descriptive test names and messages
4. **Performance Tests**: Include timing measurements
5. **Error Handling**: Test edge cases and error conditions
6. **Resource Management**: Test memory allocation/deallocation

### Test Coverage

The NLM project aims for comprehensive test coverage:

- **Code Coverage**: Minimum 80% for production code
- **Branch Coverage**: Ensure all code paths are tested
- **Condition Coverage**: Test both true and false conditions
- **Exception Coverage**: Test error handling and edge cases

### Continuous Integration

The NLM project includes CI configuration for GitHub Actions:

```yaml
# .github/workflows/test.yml
name: NLM Tests

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential
    
    - name: Configure
nlm
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
        make -j4
    
    - name: Run Tests
      run: |
        cd build
        ./test_runner --all --verbose
```

## Configuration Management

### Configuration File Format

NLM uses a simple key-value configuration format:

```ini
# Configuration file example
# Comments start with #

# Core parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Learning parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation parameters
dopamine_baseline = 0.1

# Reward parameters
reward_discount_factor = 0.99

# Environment parameters
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging parameters
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Performance parameters
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
```

### Configuration Command Line Interface

The NLM configuration system supports command-line overrides:

```bash
# Override random seed
./nlm_phase6_demo --random_seed=12345

# Override simulation steps
./nlm_phase6_demo --steps=5000

# Load custom configuration
./nlm_phase6_demo --config=my_config.cfg

# Combine command line and config file
./nlm_phase6_demo --config=base_config.cfg --steps=2000
```

### Configuration Validation

The NLM project includes configuration validation:

```bash
# Validate configuration file
cmake -P scripts/validate_config.cmake

# Validate against schema
cmake -P scripts/validate_config_schema.cmake
```

### Configuration Backup and Recovery

```bash
# Backup current configuration
cp build/config/default.cfg backup_config_$(date +%Y%m%d_%H%M%S).cfg

# Restore previous configuration
cp backup_config_*.cfg build/config/default.cfg
```

## Troubleshooting

### Common Issues

#### 1. Build Failures

**Symptom**: CMake configuration errors or compilation failures

**Solutions**:
- **CMake Version**: Ensure CMake 3.10 or higher
- **Compiler Support**: Check compiler C++ standard support
- **Dependencies**: Install required system libraries
- **Clean Build**: Remove build directory and rebuild

```bash
# Clean and rebuild
rm -rf build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### 2. Runtime Errors

**Symptom**: Program crashes or segmentation faults

**Solutions**:
- **Enable Debugging**: Recompile with Debug build type
- **Check Configuration**: Validate configuration parameters
- **Memory Issues**: Check for memory leaks or overflow
- **Input Validation**: Ensure valid input data

```bash
# Build with debug symbols
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run with valgrind for memory debugging
valgrind --leak-check=full --track-origins=yes ./nlm_phase6_demo
```

#### 3. Performance Issues

**Symptom**: Slow simulation or excessive memory usage

**Solutions**:
- **Reduce Network Size**: Lower neuron_count or region_count
- **Optimize Connectivity**: Reduce connection_probability
- **Disable Visualization**: Turn off visualization if not needed
- **Use Efficient Data Structures**: Enable compiler optimizations

#### 4. Configuration Errors

**Symptom**: Invalid parameter values or missing configuration

**Solutions**:
- **Validate Configuration**: Check parameter ranges and types
- **Use Examples**: Start with provided configuration templates
- **Gradual Tuning**: Modify one parameter at a time
- **Log Configuration**: Enable logging to debug issues

### Debugging Tools

#### GDB Debugging

```bash
# Run program with gdb
gdb ./nlm_phase6_demo

# Set breakpoints
gdb> break main

# Run program
gdb> run
```

#### Valgrind Memory Debugging

```bash
# Run with memory debugging
valgrind --leak-check=full --track-origins=yes ./nlm_phase6_demo

# Common valgrind options
--tool=memcheck
--leak-check=full
--track-origins=yes
--show-leak-kinds=all
```

#### AddressSanitizer

```bash
# Compile with AddressSanitizer
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
make -j$(nproc)

# Run with AddressSanitizer
./nlm_phase6_demo
```

### Logging and Monitoring

#### Enable Debug Logging

```cpp
// In your code
#include <nlm/core/Logger/Logger.hpp>

// Enable debug logging
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);

// Log debug information
NLM_LOG_DEBUG("Starting neuron simulation");
NLM_LOG_INFO("Neuron state: V = " << neuron.getVoltage());
```

#### Performance Monitoring

```cpp
// Add performance timing
#include <chrono>

auto start_time = std::chrono::high_resolution_clock::now();

// Run simulation code
// ...

auto end_time = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

std::cout << "Simulation completed in " << duration.count() << " ms" << std::endl;
```

## Performance Optimization

### Profiling Tools

#### GNU gprof

```bash
# Compile with profiling
cd build
cmake .. -DCMAKE_CXX_FLAGS="-pg"
make -j$(nproc)

# Run program
./nlm_phase6_demo

# Generate profile report
gprof ./nlm_phase6_demo gmon.out > profile_report.txt
```

#### Intel VTune

```bash
# Use VTune for performance analysis
vtune-ampli --collect hotspots ./nlm_phase6_demo
```

#### perf

```bash
# Use perf for Linux performance analysis
sudo perf record ./nlm_phase6_demo
sudo perf report
sudo perf annotate ./nlm_phase6_demo
```

### Optimization Strategies

#### 1. Memory Optimization

```cpp
// Use smart pointers instead of raw pointers
std::unique_ptr<Neuron> neuron = std::make_unique<Neuron>(...);

// Use memory pools for frequent allocations
MemoryPool<Neuron> neuron_pool;

// Use static arrays for fixed-size collections
Neuron neurons[1000];  // Instead of vector<Neuron>
```

#### 2. Cache Optimization

```cpp
// Use cache-friendly data layouts
struct AlignedNeuron {
    float voltage;
    float current;
    // ... other fields
} __attribute__((aligned(64)));

AlignedNeuron neurons[1000];  // Cache-aligned
```

#### 3. Parallel Processing

```cpp
// Use OpenMP for parallel loops
#pragma omp parallel for
for (int i = 0; i < neuron_count; i++) {
    neurons[i].update(...);
}

// Use std::thread for concurrent tasks
std::thread t1(&AgentBrain::processSensoryInput, &brain, percept);
std::thread t2(&AgentBrain::decodeMotorCommand, &brain);
```

#### 4. Algorithm Optimization

```cpp
// Replace expensive operations with cheaper alternatives
// Use approximate algorithms where appropriate
// Cache frequently computed values
// Use vectorization for SIMD operations
```

### Performance Testing

#### Benchmark Suite

The NLM project includes a comprehensive benchmark suite:

```bash
# Run benchmarks
./benchmark --help

# Common benchmark options
--neuron_counts=100,1000,10000
--timesteps=1000,10000,100000
--iterations=10
--output_format=json|csv|text
```

#### Performance Metrics

Benchmarks measure:

- **Time Complexity**: O(n), O(n²), O(n³) behavior
- **Memory Usage**: MB per neuron, GB for large networks
- **Scalability**: Performance scaling with network size
- **Bottleneck Analysis**: Identify performance limitations

## Code Contributions

### Coding Standards

The NLM project follows these coding standards:

1. **C++17**: Use modern C++17 features
2. **Consistent Style**: Follow Google C++ Style Guide
3. **Documentation**: Use Doxygen-style comments
4. **Error Handling**: Use exceptions for recoverable errors
5. **Memory Management**: Use smart pointers where possible
6. **Testing**: Include unit tests for all public interfaces

### Pull Request Guidelines

1. **Branch Strategy**: Use feature branches for new development
2. **Commit Messages**: Follow conventional commit format
3. **Code Review**: Ensure all changes are reviewed and approved
4. **Testing**: Add tests for new functionality
5. **Documentation**: Update documentation for new features
6. **Backwards Compatibility**: Maintain API compatibility

### Example Pull Request Format

```markdown
## Summary

Implement feature X with improved performance and better memory usage.

### Changes
- Add new class `X` with comprehensive documentation
- Modify existing class `Y` to use new interface
- Add performance optimizations for critical paths
- Include unit tests for new functionality

### Testing
- All existing tests pass
- New unit tests added for X class
- Performance benchmarks show 20% improvement

### Performance
- Memory usage reduced by 15%
- Processing time improved by 30%
- Cache misses reduced by 25%

### Documentation
- Add Doxygen comments for all public methods
- Update API documentation
- Add usage examples
```

## Advanced Usage

### Custom Development Stages

The NLM development system allows custom developmental stages:

```cpp
// Define custom developmental stages
enum class CustomDevelopmentalStage {
    Embryonic,
    Fetal,
    Infant,
    Juvenile,
    Adult,
    Elderly
};

// Implement custom development logic
void DevelopmentSystem::customUpdate() {
    switch (currentStage_) {
        case Embryonic:
            // High plasticity, rapid growth
            break;
        case Fetal:
            // Fine-tuning of connections
            break;
        // ... other stages
    }
}
```

### Custom Plasticity Rules

Extend the plasticity system with custom rules:

```cpp
class CustomPlasticityRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom plasticity implementation
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Custom weight change application
    }
};
```

### Environment Extensions

Create custom environments:

```cpp
class CustomEnvironment : public Environment {
public:
    void reset() override {
        // Custom environment reset
    }
    
    bool isEpisodeDone() const override {
        // Custom termination condition
        return false;
    }
    
    float getReward() const override {
        // Custom reward calculation
        return 0.0f;
    }
    
    SensoryPercept getSensoryInput() const override {
        // Custom sensory input generation
        return SensoryPercept();
    }
};
```

## Maintenance Guidelines

### Code Quality

1. **Code Reviews**: Conduct regular code reviews
2. **Testing**: Maintain comprehensive test coverage
3. **Documentation**: Keep documentation up to date
4. **Performance**: Monitor and optimize performance
5. **Security**: Address security vulnerabilities promptly

### Version Control

1. **Branching Strategy**: Use feature branches and pull requests
2. **Tagging**: Create tags for releases and major milestones
3. **Rollback**: Have procedures for rolling back changes
4. **Documentation**: Update documentation for every release

### Backup and Recovery

1. **Regular Backups**: Back up code and data regularly
2. **Version Control**: Use git for code versioning
3. **Configuration Management**: Track configuration changes
4. **Recovery Procedures**: Document recovery procedures

### Monitoring and Alerting

1. **Performance Monitoring**: Monitor system performance
2. **Resource Usage**: Track memory and CPU usage
3. **Error Tracking**: Monitor for errors and warnings
4. **Health Checks**: Implement health monitoring

### Rollback Procedures

1. **Version Control**: Use git to revert changes
2. **Configuration**: Restore previous configuration
3. **Database**: Restore database backups
4. **Documentation**: Update documentation

## Conclusion

The NLM project provides a comprehensive framework for developing and testing artificial neural systems. This documentation serves as a guide for developers working with the project, covering everything from basic setup to advanced usage and maintenance.

Key takeaways:

1. **Start Simple**: Begin with basic configurations and gradually increase complexity
2. **Test Thoroughly**: Use comprehensive tests to ensure correctness
3. **Optimize Performance**: Profile and optimize code for performance
4. **Document Everything**: Maintain up-to-date documentation for all components
5. **Follow Best Practices**: Use established coding standards and patterns

The NLM project is designed to be flexible and extensible, allowing researchers to explore different approaches to artificial neural computation while maintaining a solid foundation for scientific investigation and development.

EOF