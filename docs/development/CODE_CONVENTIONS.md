# Development Documentation

## CODE_CONVENTIONS.md

This document provides comprehensive coding conventions and standards for the NLM development team. It ensures code quality, consistency, and maintainability across the project.

### 1. Naming Conventions

#### File Names
- **Source files**: Use lowercase with underscores (e.g., `neuron.cpp`, `synapse.hpp`)
- **Header files**: Same as source files with `.hpp` extension
- **Class names**: Use PascalCase (e.g., `Neuron`, `Synapse`, `Brain`)
- **Namespace**: Use `nlm` for the main namespace

#### Variable and Function Names
- **Variables**: Use lowercase with underscores (e.g., `membrane_potential`, `learning_rate`)
- **Functions**: Use lowercase with underscores (e.g., `update_neuron()`, `calculate_synaptic_weight()`)
- **Methods**: Use lowercase with underscores (e.g., `get_membrane_potential()`, `set_learning_rate()`)
- **Constants**: Use UPPER_SNAKE_CASE (e.g., `MAX_NEURONS`, `DEFAULT_SYNAPSE_WEIGHT`)

#### Class Names
- **Class names**: Use PascalCase (e.g., `Neuron`, `Synapse`, `Brain`, `PlasticityRule`)
- **Struct names**: Use PascalCase (e.g., `NeuronState`, `ConfigEntry`, `Experience`)
- **Enum names**: Use PascalCase (e.g., `NeuronType`, `PlasticityType`, `ActionType`)

### 2. Code Style

#### Indentation
- Use 4 spaces per indentation level
- No tabs (convert all tabs to spaces)
- Maximum indentation level: 3 levels (12 spaces)

#### Line Length
- Maximum line length: 100 characters
- Break long lines at logical points
- Use continuation lines for function calls

#### Commenting Style
- **File headers**: Include file description, author, date, copyright, version
- **Class documentation**: Doxygen-style comments with @class, @brief, @details
- **Method documentation**: Doxygen-style comments with @param, @return, @since
- **Inline comments**: Use for complex logic, not obvious points

#### Doxygen Style Example
```cpp
/**
 * @class Neuron
 * @brief Represents a single neuron with LIF dynamics.
 * 
 * This class implements the Leaky Integrate-and-Fire (LIF) model of neuronal
 * computation, including refractory periods, spike detection, and state
 * management for individual neurons.
 * 
 * @details The neuron model includes membrane potential dynamics, threshold
 *          crossing, spike reset, and refractory period management. It supports
 *          both rate-based and spike-based computation modes.
 * 
 * @note This is a simplified biological model that captures essential
 *       neuronal dynamics while remaining computationally efficient.
 * 
 * @see Synapse, Brain, PlasticityRule
 * 
 * @author NLM Development Team
 * @date 2024
 * @version 1.0
 */
class Neuron {
```

### 3. Code Organization

#### Directory Structure
```
src/
├── brain/              # Brain components
│   ├── Neuron.hpp      # Individual neuron representation
│   ├── Synapse.hpp     # Synaptic connections
│   ├── NeuralPopulation.hpp  # Neuron groups
│   ├── NeuralRegion.hpp # Brain regions
│   └── Brain.hpp       # Central brain coordinator
├── core/              # Core infrastructure
│   ├── Types/          # Type definitions
│   ├── Config/         # Configuration system
│   ├── Random/         # Random number generator
│   ├── Logger/         # Logging system
│   └── SimulationClock/ # Time management
├── plasticity/        # Learning rules
│   ├── PlasticityRule.hpp  # Base class
│   ├── Hebbian.hpp        # Hebbian learning
│   ├── STDP.hpp          # Spike-timing dependent plasticity
│   └── StructuralPlasticity.hpp # Synaptogenesis and pruning
├── development/       # Developmental processes
│   ├── DevelopmentSystem.hpp # Development orchestration
│   ├── Synaptogenesis.hpp  # New synapse formation
│   ├── Pruning.hpp         # Synapse elimination
│   └── Maturation.hpp      # Adult property development
├── neuromodulation/    # Neuromodulatory systems
│   ├── Neuromodulator.hpp # Base neuromodulator
│   ├── Dopamine.hpp       # Reward signaling
│   ├── Novelty.hpp        # Novelty detection
│   ├── Curiosity.hpp      # Exploration motivation
│   └── PredictionError.hpp # Error signals
├── memory/           # Memory systems
│   ├── Memory.hpp              # High-level memory interface
│   ├── NeuralWorkingMemory.hpp # Working memory
│   └── NeuralEpisodicMemory.hpp # Episodic memory
├── prediction/        # Predictive processing
│   ├── PredictionSystem.hpp # Prediction interface
│   └── NeuralPrediction.hpp  # Sequence prediction
├── cognition/         # Cognitive processes
│   ├── NeuralPlanner.hpp   # Action planning
│   └── ConceptFormation.hpp # Pattern discovery
├── sensory/           # Sensory processing
│   ├── SensoryInput.hpp   # Base sensory interface
│   ├── Vision.hpp         # Visual processing
│   ├── Audio.hpp          # Audio processing
│   └── InternalSignals.hpp # Internal state signals
├── motor/            # Motor control
│   ├── MotorSystem.hpp    # Motor control interface
│   └── Action.hpp         # Action representation
├── environment/       # Environment interface
│   ├── Environment.hpp     # Base environment interface
│   └── Observation.hpp     # Observation processing
├── experiments/       # Experiment framework
│   ├── Experiment.hpp      # Experiment interface
│   ├── ExperimentRunner.hpp # Experiment execution
│   └── Metrics.hpp         # Data analysis
└── utils/             # Utility functions
    ├── BrainUtils.hpp     # Brain utility functions
    └── MathUtils.hpp      # Mathematical utilities
```

#### Subdirectory Organization
- **brain**: Core neural computation components
- **core**: Foundational infrastructure and utilities
- **plasticity**: Learning and adaptation mechanisms
- **development**: Developmental processes and maturation
- **neuromodulation**: Chemical signaling and modulation
- **memory**: Storage and retrieval systems
- **prediction**: Forward models and prediction
- **cognition**: Higher-level cognitive processes
- **sensory**: Input processing and perception
- **motor**: Output generation and action
- **environment**: Interaction with external world
- **experiments**: Testing and validation framework
- **utils**: Supporting utilities and helper functions

### 4. Memory Management

#### Smart Pointers
```cpp
// Prefer std::unique_ptr for exclusive ownership
std::unique_ptr<Neuron> neuron = std::make_unique<Neuron>(NeuronId(0));

// Use std::shared_ptr for shared ownership
std::shared_ptr<PlasticityRule> rule = std::make_shared<Hebbian>(0.001f);
```

#### Pimpl Idiom
```cpp
// For large classes with implementation details
class Synapse {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    // ... public interface
};
```

#### Move Semantics
```cpp
// Implement move constructor and assignment operator
Neuron(Neuron&& other) noexcept {
    // Transfer ownership
}

Neuron& operator=(Neuron&& other) noexcept {
    // Transfer ownership
    return *this;
}
```

### 5. Error Handling

#### Exception Hierarchy
```cpp
// Base exception class
class NLMError : public std::exception {
public:
    explicit NLMError(const std::string& message, 
                      const std::shared_ptr<NLMError>& cause = nullptr)
        : message_(message), cause_(cause) {}
    
    const char* what() const noexcept override { return message_.c_str(); }
    
    const std::shared_ptr<NLMError>& getCause() const { return cause_; }
    
private:
    std::string message_;
    std::shared_ptr<NLMError> cause_;
};

// Specific exception types
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message, 
                        const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause) {}
};

class PlasticityError : public NLMError {
public:
    explicit PlasticityError(const std::string& message, 
                           const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause) {}
};
```

#### Error Handling Best Practices
- Use exceptions for recoverable errors
- Use error codes for system-level errors
- Validate inputs before processing
- Use assertions for programming errors
- Implement graceful error recovery where possible

### 6. Testing

#### Testing Framework
```cpp
// Test structure
TEST(TestClassName, TestMethodName) {
    // Arrange
    TestClassName obj;
    // Setup test conditions
    
    // Act
    obj.methodToTest();
    
    // Assert
    EXPECT_TRUE(condition);
    EXPECT_EQ(expectedValue, actualValue);
    EXPECT_THROW(exceptionCode, ExceptionType);
}

// Parameterized test
TEMPLATE_TEST_CASE("TestClass", "", TypesToTest) {
    TestClassName obj;
    // Test implementation
}
```

#### Test Categories
1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete system functionality
4. **Performance Tests**: Test performance characteristics
5. **Stress Tests**: Test system under extreme conditions

#### Test Coverage Guidelines
- Aim for >90% code coverage
- Test boundary conditions
- Test error conditions
- Test performance requirements
- Test concurrent scenarios

### 7. Documentation

#### Doxygen Configuration
```doxygen
PROJECT_NAME           = "NLM"
PROJECT_NUMBER         = "1.0.0"
OUTPUT_DIRECTORY       = "docs"
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
GENERATE_XML           = YES
GENERATE_MAN           = NO
GENERATE_RTF           = NO
CREATE_SUBDIRS          = YES
```

#### Code Documentation Standards
- **File headers**: Include description, author, date, copyright, version
- **Class documentation**: Document purpose, methods, attributes, usage
- **Method documentation**: Document parameters, return values, exceptions
- **Inline comments**: Explain complex logic, not obvious points

### 8. Build System

#### CMake Configuration
```cmake
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find dependencies
find_package(nlohmann_json REQUIRED)

# Add source files
add_library(nlm_core STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    # ... other core files
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
```

### 9. Version Control

#### Git Guidelines
- Use meaningful commit messages
- Include Co-authored-by: openhands <openhands@all-hands.dev>
- Rebase feature branches on main
- Use semantic versioning
- Keep commit messages concise but descriptive

### 10. Performance Considerations

#### Optimization Guidelines
- Profile before optimizing
- Focus on hotspots
- Maintain code readability
- Use efficient data structures
- Minimize memory allocations

#### Profiling Tools
- gprof: Time profiling
- Valgrind: Memory profiling
- Cachegrind: Cache analysis
- perf: Performance analysis

## Performance Tuning Guide

This guide provides comprehensive instructions for optimizing NLM performance, from basic profiling to advanced tuning strategies.

### 1. Profiling

#### Time Profiling
```bash
# Compile with profiling support
gcc -pg -O2 -o nlm main.cpp

# Run the application
./nlm > /dev/null 2>&1

# Generate profile data
gprof nlm gmon.out > profile.txt

# Analyze profile
less profile.txt
```

#### Memory Profiling
```bash
# Install Valgrind
sudo apt-get install valgrind

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./nlm

# Generate memory report
valgrind --leak-check=full ./nlm 2> memory_report.txt
```

#### Cache Profiling
```bash
# Install cachegrind tool
sudo apt-get install valgrind

# Run with cache profiling
cachegrind_launcher ./nlm
cg_annotate cache.dat > cache_report.html
```

### 2. Optimization Strategies

#### Algorithm Optimization
- **Vectorization**: Use SIMD instructions for parallel processing
- **Cache locality**: Optimize memory access patterns
- **Parallelization**: Use multi-threading for concurrent operations
- **Reduce complexity**: Simplify algorithms and data structures

#### Memory Optimization
- **Object pooling**: Reuse memory allocations
- **Move semantics**: Use move constructor/assignment
- **Smart pointers**: Avoid reference counting overhead
- **Inline functions**: Reduce function call overhead

#### Code Optimization
- **Constant folding**: Pre-compute constant expressions
- **Loop unrolling**: Unroll small loops
- **Function inlining**: Inline small functions
- **Dead code elimination**: Remove unused code

### 3. Tuning for Different Targets

#### Embedded Systems
```cmake
# Optimize for embedded systems
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_GUI=OFF 
     -DNLM_ENABLE_PYTHON=OFF -DNLM_ENABLE_TESTS=OFF ..

# Use specific compiler optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
```

#### High-Performance Computing
```cmake
# Optimize for HPC systems
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNLM_ENABLE_MPI=ON 
     -DNLM_ENABLE_OPENMP=ON -DNLM_ENABLE_CUDA=ON ..

# Use parallel processing
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native -ffast-math")
```

#### Mobile Devices
```cmake
# Optimize for mobile devices
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_GUI=ON 
     -DNLM_ENABLE_PYTHON=OFF -DNLM_ENABLE_TESTS=OFF ..

# Optimize for power efficiency
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -flto -fomit-frame-pointer")
```

### 4. Performance Testing

#### Performance Metrics
```cpp
// Performance measurement utility
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& name) 
        : name_(name), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceProfiler() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        std::cout << name_ << ": " << duration.count() << " ms" << std::endl;
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Usage
void criticalFunction() {
    PERFORMANCE_PROFILER("criticalFunction")
    // Function implementation
}
```

#### Benchmarking
```bash
#!/bin/bash
# Benchmark script for NLM

cd build

# Run benchmark
./nlm --benchmark > benchmark_results.txt

# Parse results
python3 parse_benchmark.py benchmark_results.txt > benchmark_analysis.txt

# Generate report
python3 generate_report.py benchmark_analysis.txt > performance_report.html
```

### 5. Debugging Techniques

#### Debugging Tools
```bash
# GDB debugging
gdb ./nlm
# Set breakpoints
break brain.cpp:123
# Step through code
next
# Inspect variables
print neuron.getMembranePotential()
# Continue execution
continue
```

#### Core Dumps
```bash
# Enable core dumps
set ulimit -c unlimited
# Run with core dump generation
./nlm --core

# Analyze core dumps
gdb ./nlm corefile.gcore
```

#### AddressSanitizer
```bash
# Compile with AddressSanitizer
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" ..
cmake --build .

# Run with sanitizers
./nlm --sanitize
```

#### Valgrind
```bash
# Install Valgrind
sudo apt-get install valgrind

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./nlm
```

### 6. Best Practices

#### Profiling First
- Measure before optimizing
- Identify bottlenecks
- Focus on high-impact optimizations

#### Maintain Readability
- Don't sacrifice code clarity for performance
- Use profiling data to guide optimizations
- Document performance decisions

#### Test Optimizations
- Verify correctness after optimization
- Compare performance before and after
- Ensure optimizations don't break compatibility

### 7. Common Performance Issues

#### 1. Inefficient Loops
```cpp
// Bad: O(n²) algorithm
for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
        // O(1) operation
    }
}
```

```cpp
// Good: O(n) algorithm
for (int i = 0; i < n; ++i) {
    // O(1) operation
}
```

#### 2. Memory Allocations
```cpp
// Bad: Frequent memory allocations
for (int i = 0; i < n; ++i) {
    std::vector<float> temp;
    // ... use temp
}
```

```cpp
// Good: Reuse memory allocations
std::vector<float> temp;
temp.reserve(n);
for (int i = 0; i < n; ++i) {
    // ... use temp
}
```

#### 3. Cache Inefficiency
```cpp
// Bad: Poor cache locality
for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
        matrix[i][j] = 0.0f;
    }
}
```

```cpp
// Good: Optimize cache locality
for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
        matrix[j][i] = 0.0f;  // Better cache locality
    }
}
```

## Conclusion

This development documentation provides comprehensive guidance for NLM developers. It covers:

1. **Code Conventions**: Best practices for coding and documentation
2. **Build System**: Instructions for building and installing the system
3. **Testing**: Strategies and best practices for testing
4. **Debugging**: Techniques and tools for debugging
5. **Performance Tuning**: Optimization strategies and guidelines

The NLM framework is designed to be flexible and extensible, allowing for continuous improvement and evolution. This documentation provides the foundation for maintaining code quality, performance, and maintainability as the system grows and evolves.