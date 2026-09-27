# Developer Guide

## Overview

The NLM (熙然) project follows modern C++ development practices with a focus on performance, determinism, and biological inspiration. This guide provides comprehensive instructions for developers working on the NLM codebase.

## Project Structure

### Directory Organization

```
NLM/
├── CMakeLists.txt              # Build system
├── src/                        # Source code
│   ├── core/                   # Core utilities
│   ├── brain/                  # Neural components
│   ├── dynamics/               # Neural dynamics
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Developmental system
│   ├── neuromodulation/         # Neuromodulators
│   ├── memory/                 # Memory systems
│   ├── prediction/             # Prediction systems
│   ├── cognition/              # Cognitive mechanisms
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── experiments/            # Experiment framework
│   └── visualization/          # Visualization tools
├── tests/                      # Unit tests
├── configs/                    # Configuration files
├── docs/                       # Documentation
├── build/                      # Build artifacts
└── external/                   # Third-party dependencies
```

### Source Code Organization

The NLM codebase is organized around biological brain components:

- **Core Layer**: Fundamental types, configuration, random number generation, logging, and time management
- **Brain Layer**: Neural computation (neurons, synapses, populations, regions)
- **Dynamics Layer**: Real-time neural dynamics and event-driven processing
- **Plasticity Layer**: Learning rules (STDP, Hebbian, structural)
- **Development Layer**: Developmental stages and maturation processes
- **Neuromodulation Layer**: Biological neuromodulators (dopamine, curiosity, etc.)
- **Memory Layer**: Working memory, episodic memory, and associative memory
- **Prediction Layer**: Temporal sequence learning and action prediction
- **Cognitive Layer**: Attention, planning, self-model, and social learning
- **Sensory Layer**: Vision, audio, and internal signal processing
- **Motor Layer**: Action representation and motor control
- **Environment Layer**: World simulation and interaction
- **Experiment Layer**: Experiment framework and metrics

## Development Setup

### System Requirements

#### Minimum Requirements

- **Operating System**: Linux (macOS and Windows support available)
- **Compiler**: GCC 7+, Clang 5+, or MSVC 2019+
- **Architecture**: x86_64, ARM64
- **Memory**: 4+ GB RAM (16+ GB recommended)
- **Storage**: 1+ GB free space

#### Recommended Development Environment

- **IDE**: CLion, Visual Studio Code with C++ extensions, or Vim/Eclipse
- **Version Control**: Git 2.0+
- **Build Tools**: CMake 3.10+
- **Performance Tools**: Valgrind, AddressSanitizer, ThreadSanitizer

### Building from Source

#### Standard Build Process

```bash
# Navigate to project root
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with optimization flags
make -j$(nproc)

# Run tests (optional)
ctest --output-on-failure
```

#### Development Build

```bash
# Debug build for development
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON

# Build with sanitizers for memory safety
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
```

#### Cross-Compilation Support

NLM supports cross-compilation for embedded systems:

```bash
# Example for ARM target
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/arm-linux-gnu.toolchain.cmake
```

### Running Tests

#### Unit Tests

```bash
# Run all tests
cd build
ctest

# Run specific test category
ctest -R "test_neuron"
ctest -R "test_plasticity"
```

#### Performance Benchmarks

```bash
# Run performance benchmarks
./benchmark

# Run specific benchmark suite
./benchmark --suite=neural_computation
```

#### Integration Tests

```bash
# Run Phase 6 integration test
./nlm_phase6_demo

# Run custom experiments
./nlm --config=experiment.json
```

## Coding Standards

### C++ Style Guide

#### File Structure

Each source file follows this pattern:

```cpp
// File: src/brain/Neuron.cpp

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Neuron::Impl {
    // Implementation details
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Constructor implementation
}

// ... other methods ...

} // namespace nlm
```

#### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Namespaces | `nlm` | `namespace nlm` |
| Classes | PascalCase | `class NeuralPopulation` |
| Methods | camelCase | `void setMembranePotential()` |
| Variables | snake_case | `MembranePotential membranePotential` |
| Constants | UPPER_SNAKE_CASE | `static constexpr float TIME_CONSTANT = 20.0f` |
| Types | CamelCase | `using NeuronId = uint64_t` |

#### Documentation Style

All public classes and methods use Doxygen-style documentation:

```cpp
/**
 * @class Neuron
 * @brief Represents a single neuron with LIF dynamics
 * 
 * The Neuron class implements the Leaky Integrate-and-Fire (LIF) model
 * of neural computation, including membrane potential dynamics, spike
 * generation, and synaptic interactions.
 * 
 * @note This is a biological-inspired model with no claims of
 *       physiological accuracy.
 * 
 * @warning NeuralState::state must be properly initialized before use.
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire_model
 * 
 * @since Phase 2: Real LIF neuron dynamics
 * 
 * @author NLM Development Team
 * 
 * @note This class is thread-safe for concurrent access from multiple
 *       simulation steps.
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with specified identifier
     * 
     * @param id Unique identifier for this neuron
     * @throws std::invalid_argument if id is zero
     * 
     * @pre Neuron identifiers must be unique within the brain
     * @post Neuron is initialized with resting state
     */
    explicit Neuron(NeuronId id);
    
    // ... other methods ...
};
```

### Memory Management

#### Smart Pointers

NLM uses modern C++ memory management:

```cpp
// Preferred: std::shared_ptr for ownership sharing
std::shared_ptr<Config> config;

// Preferred: std::unique_ptr for exclusive ownership
std::unique_ptr<RandomGenerator> rng;

// Exception-safe cleanup
std::vector<std::unique_ptr<NeuralRegion>> regions;
```

#### Memory Pooling

For performance-critical components, NLM uses custom memory pools:

```cpp
// Memory pool usage in performance-sensitive code
class NeuronAllocator {
    using value_type = Neuron;
    
public:
    template<typename U>
    NeuronAllocator(const NeuronAllocator<U>& other) noexcept {}
    
    template<typename U>
    NeuronAllocator& operator=(const NeuronAllocator<U>& other) noexcept {
        return *this;
    }
    
    Neuron* allocate(std::size_t n) {
        return memoryPool.allocate(n);
    }
    
    void deallocate(Neuron* p, std::size_t n) {
        memoryPool.deallocate(p, n);
    }
};
```

### Error Handling

#### Exception Safety

NLM provides strong exception safety guarantees:

```cpp
void Neuron::addToMembranePotential(MembranePotential delta) {
    try {
        // Potentially throwing operations
        validateDelta(delta);
        
        // Atomic operation
        pImpl->state.membranePotential += delta;
        
        // If any operation throws, object remains in consistent state
    } catch (const std::exception& e) {
        // Log error and rethrow
        NLM_LOG_ERROR("Failed to update membrane potential: " + std::string(e.what()));
        throw;
    }
}
```

#### Error Codes and Categories

```cpp
enum class NLMError {
    // Configuration errors
    INVALID_CONFIG,
    MISSING_PARAMETER,
    INVALID_PARAMETER_VALUE,
    
    // Neural computation errors
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_HANDLE,
    MEMBRANE_POTENTIAL_OVERFLOW,
    
    // Memory system errors
    MEMORY_ALLOCATION_FAILED,
    MEMORY_CORRUPTION_DETECTED,
    MEMORY_LEAK_DETECTED,
    
    // System errors
    SIMULATION_STEP_FAILED,
    NEURAL_DYNAMICS_ERROR,
    PLASTICITY_UPDATE_FAILED
};

// Exception class with error category support
class NLMException : public std::runtime_error {
public:
    explicit NLMException(NLMError error, const std::string& message)
        : std::runtime_error(message), error_(error) {}
    
    NLMError error() const { return error_; }
    
private:
    NLMError error_;
};
```

### Testing Guidelines

#### Unit Testing

NLM uses a comprehensive unit testing framework:

```cpp
// Test file: tests/test_neuron.cpp

#include "gtest/gtest.h"
#include "brain/Neuron.hpp"
#include "core/Types/Types.hpp"

namespace nlm {
namespace tests {

TEST(NeuronTest, Initialization) {
    NeuronId id = 1;
    Neuron neuron(id);
    
    EXPECT_EQ(neuron.getId(), id);
    EXPECT_EQ(neuron.getType(), NeuronType::Internal);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), 0.0f);
}

TEST(NeuronTest, MembranePotentialUpdate) {
    Neuron neuron(1);
    neuron.addToMembranePotential(0.5f);
    
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), 0.5f);
}

TEST(NeuronTest, ThresholdCrossing) {
    Neuron neuron(1);
    neuron.setThreshold(0.3f);
    neuron.addToMembranePotential(0.4f);
    
    EXPECT_TRUE(neuron.shouldFire());
}

} // namespace tests
} // namespace nlm
```

#### Test Coverage Requirements

- **Minimum coverage**: 80% for public interfaces
- **Required coverage**: 100% for critical functions (memory management, error handling)
- **Integration tests**: All system components must be tested together

#### Performance Testing

NLM includes performance benchmarks for critical paths:

```cpp
// Benchmark: src/benchmark.cpp

#include "benchmark/benchmark.h"
#include "brain/Neuron.hpp"
#include "plasticity/STDP.hpp"

static void BM_NeuronDynamics(benchmark::State& state) {
    NeuronId id = 1;
    Neuron neuron(id);
    
    for (auto _ : state) {
        neuron.addToMembranePotential(0.01f);
        neuron.decrementRefractory();
    }
}

BENCHMARK(BM_NeuronDynamics);

BENCHMARK_MAIN();
```

#### Continuous Integration

The project uses GitHub Actions for CI/CD:

```yaml
# .github/workflows/ci.yml

name: NLM CI/CD

on:
  push:
    branches: [main, development]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
        compiler: [g++, clang++]
    
    steps:
      - uses: actions/checkout@v2
      
      - name: Configure CMake
        run: |
          mkdir build
          cd build
          cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
      
      - name: Build
        run: |
          cd build
          make -j4
      
      - name: Test
        run: |
          cd build
          ctest --output-on-failure
```

## Development Tools

### IDE Configuration

#### CLion Configuration

```xml
<!-- .idea/cmake.xml -->
<project version="4">
  <component name="CMakeWorkspace">
    <option name="cmakeExecutablePath" value="$PROJECT_DIR$/cmake/bin/cmake" />
    <option name="generatorName" value="Unix Makefiles" />
  </component>
</project>
```

#### VS Code Configuration

```json
// .vscode/c_cpp_properties.json
{
    "configurations": [
        {
            "name": "NLM Debug",
            "compilerPath": "/usr/bin/g++",
            "includePath": [
                "${workspaceFolder}/src",
                "${workspaceFolder}/include"
            ],
            "defines": ["DEBUG", "NLM_VERSION=\"1.0.0\""],
            "intelliSenseMode": "clang-x64",
            "browse": {
                "path": ["${workspaceFolder}/src"]
            }
        }
    ],
    "version": 4
}
```

### Code Analysis Tools

#### Clang-Tidy

Configure clang-tidy for NLM code style:

```yaml
# .clang-tidy

Checks: '*,-llvm-namespace-comments,-readability-braces-around-statements'

CheckOptions:
  - key: readability-identifier-naming.NamespaceCase
    value: 'lower_case'
  - key: readability-identifier-naming.ClassCase
    value: 'upper_case'
  - key: readability-identifier-naming.StructCase
    value: 'upper_case'
  - key: readability-identifier-naming.UnionCase
    value: 'upper_case'
  - key: readability-identifier-naming.FunctionCase
    value: 'lower_case'
  - key: readability-identifier-naming.VariableCase
    value: 'snake_case'
  - key: readability-identifier-naming.EnumCase
    value: 'upper_case'
```

#### Cppcheck

Run static analysis:

```bash
# Install and run cppcheck
cppcheck --enable=all --std=c++11 src/

# Check for memory leaks
cppcheck --enable=leak-check src/
```

### Performance Optimization

#### Profiling Tools

```bash
# Linux performance profiling
gprof ./nlm_phase6_demo gmon.out
matplotlib.plot(gmon.out)  # Python visualization

# Valgrind for memory errors
valgrind --tool=memcheck ./nlm_phase6_demo

# AddressSanitizer
./nlm_phase6_demo --enable-asan
```

#### Optimization Strategies

1. **Loop Unrolling**: Manually unroll tight loops
2. **Data Locality**: Optimize memory access patterns
3. **Vectorization**: Use SIMD instructions
4. **Thread Affinity**: Pin threads to specific cores
5. **NUMA Awareness**: Optimize memory placement

### Version Control

#### Git Configuration

Configure git for NLM development:

```bash
# Global git configuration
git config --global user.name "NLM Development Team"
git config --global user.email "dev@nlm.ai"

# Commit message conventions
git commit -m "feat: Add new memory consolidation feature

- Implements consolidation during sleep/rest phases
- Improves episodic memory retention
- Fixes memory leak in replay system

Co-authored-by: OpenHands <openhands@all-hands.dev>"
```

#### Branching Strategy

```bash
# Feature branch workflow
git checkout main
git pull origin main
git checkout -b feature/novelty-detection
git add src/neuromodulation/Novelty.cpp
git commit -m "feat: Add novelty detection system"

# Merge feature branch
git checkout main
git pull origin main
git merge --no-ff feature/novelty-detection

# Release branch for major changes
git checkout -b release/v2.0.0
```

## Advanced Development Topics

### Performance Tuning

#### Kernel Parameters

Optimize system for NLM performance:

```bash
# Linux kernel parameters
# /etc/sysctl.conf
sysctl -w net.core.somaxconn=65535
sysctl -w net.ipv4.tcp_max_syn_backlog=65535
sysctl -w vm.max_map_count=262144
```

#### Memory Management

```bash
# Linux memory management
swapoff -a  # Disable swap for faster performance
swapon -a   # Re-enable when needed

# Memory overcommitment for containerized environments
# /etc/fstab
swap swap swap defaults 0 0
```

### Multi-Threading

NLM uses thread-safe patterns for concurrent simulation:

```cpp
// Thread-safe neuron access
class Neuron {
public:
    void updateMembranePotential(MembranePotential delta) {
        std::lock_guard<std::mutex> lock(mutex_);
        pImpl->state.membranePotential += delta;
    }
    
private:
    std::mutex mutex_;
    std::shared_ptr<Impl> pImpl;
};
```

### Debugging Techniques

#### Debug Build Configuration

```bash
# Debug build with enhanced logging
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG_LOG=ON -DENABLE_PERFORMANCE_PROFILING=ON

# Enable backtrace generation
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_BACKTRACE=ON
```

#### Debug Session Setup

```bash
# Launch with debugger
gdb ./nlm_phase6_demo

# GDB commands
set pagination off
set print pretty on
set breakpoint pending on

# Continue execution
continue

# Inspect variables
print neuron.getId()
print neuron.getMembranePotential()
```

## Contributing to NLM

### Code Contribution Process

1. **Fork Repository**: Create your own fork of NLM
2. **Feature Branch**: Create a branch for your changes
3. **Development**: Implement features with tests
4. **Code Review**: Submit pull request for review
5. **Integration**: Merge reviewed changes

### Code Review Guidelines

- **Review Focus**: Functionality, performance, memory safety, documentation
- **Merge Strategy**: Use squash merge for feature branches
- **Commit Standards**: Conventional commits with co-author attribution
- **Testing Requirements**: All changes must pass existing test suite

### Testing New Features

#### Adding New Tests

```cpp
// tests/test_custom_feature.cpp

#include "gtest/gtest.h"
#include "your/feature/Header.h"

namespace nlm {
namespace tests {

TEST(CustomFeatureTest, BasicFunctionality) {
    CustomFeature feature;
    
    EXPECT_TRUE(feature.initialize({"param1": 1.0, "param2": "test"}));
    EXPECT_TRUE(feature.process({"input": "data"}));
    EXPECT_EQ(feature.getResult(), "expected_output");
}

TEST(CustomFeatureTest, ErrorHandling) {
    CustomFeature feature;
    
    EXPECT_THROW(feature.initialize({}), std::invalid_argument);
    EXPECT_THROW(feature.process({}), std::runtime_error);
}

TEST(CustomFeatureTest, Performance) {
    CustomFeature feature;
    feature.initialize({"param1": 1.0});
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        feature.process({"input": std::to_string(i)});
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 1000000);  // Less than 1 second
}

} // namespace tests
} // namespace nlm
```

### Performance Optimization Checklist

#### Before Optimization

- [ ] Profile code to identify bottlenecks
- [ ] Verify correctness with tests
- [ ] Measure baseline performance
- [ ] Identify memory usage patterns

#### Optimization Process

- [ ] Apply safe optimizations
- [ ] Verify performance improvements
- [ ] Ensure correctness maintained
- [ ] Update benchmarks if needed

#### After Optimization

- [ ] Verify no regression
- [ ] Update documentation
- [ ] Commit changes with performance notes
- [ ] Run full test suite

### Development Environment Setup

#### Docker Development

```dockerfile
# Dockerfile
d FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libomp-dev \
    valgrind

WORKDIR /app
COPY . /app

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j4

CMD ["./nlm_phase6_demo"]
```

```bash
# Build and run with Docker
docker build -t nlm-dev .
docker run --rm nlm-dev
```

### Academic Contribution

NLM is an academic research project. When contributing to the codebase:

1. **Cite**: Reference NLM in publications
2. **Acknowledge**: Credit original authors and contributors
3. **License**: Ensure all contributions are under the MIT license
4. **Documentation**: Update documentation for new features
5. **Testing**: Provide comprehensive test coverage

## Development Resources

### Online Resources

- **Documentation**: https://nlm.ai/docs
- **GitHub**: https://github.com/nlm/nlm
- **Discussions**: https://github.com/nlm/nlm/discussions
- **Issues**: https://github.com/nlm/nlm/issues

### Community Guidelines

- **Be respectful**: Treat all contributors with respect
- **Help others**: Share knowledge and assist with questions
- **Code quality**: Write clean, efficient, well-documented code
- **Testing**: Ensure thorough test coverage
- **Performance**: Optimize for both correctness and speed

### Getting Help

#### For Questions

- Check existing documentation first
- Search GitHub issues and discussions
- Ask questions on the NLM forum
- Contact maintainers through GitHub

#### For Bug Reports

```bash
# When reporting bugs, include:
# 1. Environment information
# 2. Reproduction steps
# 3. Configuration files
# 4. Log output
# 5. Expected vs actual behavior
```

## Future Development Directions

### Planned Features

1. **Advanced Neuromodulation**: More biologically realistic models
2. **Hierarchical Processing**: Multiple levels of abstraction
3. **Social Brain**: Multi-agent interaction capabilities
4. **Developmental Genetics**: Gene-based development influences
5. **Neuromodulation Interfaces**: Integration with biological signals

### Research Opportunities

- **Memory Consolidation**: During sleep/rest phases
- **Attention Mechanisms**: Beyond competitive selection
- **Predictive Coding**: Hierarchical prediction models
- **Embodiment**: Physical interaction with environment
- **Evolutionary Dynamics**: Selection-based learning

## Conclusion

The NLM project represents a significant step forward in artificial brain research. By following these development guidelines, contributors can:

- Maintain code quality and consistency
- Ensure performance and reliability
- Facilitate collaboration and knowledge sharing
- Advance the state of the art in neural computation

The NLM development community welcomes all contributions that align with our scientific mission and engineering excellence.

Happy brain simulation!