# NLM Developer Guide

## Introduction

This guide provides comprehensive instructions for developing, extending, and contributing to the Neural Learning Machine (NLM) framework. It covers:

- Repository structure and build system
- Development setup and environment configuration
- Core development practices and conventions
- Extension point documentation
- Testing strategies
- Performance optimization
- Code quality and maintainability

## 1. Repository Structure

### Project Layout

```
/workspace/project/
├── src/
│   ├── brain/           # Core neural computation
│   │   ├── Brain.hpp/cpp # Main brain controller
│   │   ├── Neuron.hpp/cpp # Individual neuron models
│   │   └── Synapse.hpp/cpp # Synaptic connections
│   ├── memory/          # Memory systems
│   │   ├── NeuralWorkingMemory.hpp/cpp
│   │   ├── NeuralEpisodicMemory.hpp/cpp
│   │   └── NeuralAssociativeMemory.hpp/cpp
│   ├── plasticity/      # Learning mechanisms
│   │   ├── STDP.hpp/cpp
│   │   ├── Hebbian.hpp/cpp
│   │   └── StructuralPlasticity.hpp/cpp
│   ├── core/            # Core infrastructure
│   │   ├── Config.hpp/cpp # Configuration management
│   │   ├── Logger.hpp/cpp # Logging system
│   │   ├── Random.hpp/cpp # Random number generation
│   │   └── Types.hpp     # Type definitions
│   ├── dynamics/        # Neural dynamics
│   │   ├── NeuralDynamics.hpp # Base dynamics class
│   │   └── SpikeSystem.hpp/cpp # Event-driven spike processing
│   └── performance/     # Performance optimizations
│       ├── MemoryPool.hpp/cpp
│       ├── SIMDVectorization.hpp/cpp
│       └── ParallelProcessing.hpp/cpp
├── tests/              # Test suite
│   ├── test_*.cpp       # Unit tests
│   └── integration/    # Integration tests
├── examples/           # Usage examples
│   ├── basic/          # Simple usage
│   ├── advanced/       # Complex usage
│   └── migration/      # Migration guides
├── docs/               # Documentation
│   ├── ARCHITECTURE.md # System architecture
│   ├── PERFORMANCE.md  # Performance documentation
│   └── SCIENCE.md      # Scientific background
├── build/              # Build artifacts
├── configs/            # Configuration examples
└── python/             # Python bindings
```

### Key Directories

#### `src/` - Source Code
- **brain/**: Core neural computation and brain architecture
- **memory/**: Working memory, episodic memory, and associative memory
- **plasticity/**: Learning rules including STDP, Hebbian, and structural changes
- **core/**: Fundamental infrastructure (config, logging, random, types)
- **dynamics/**: Neural dynamics models and spike processing
- **performance/**: Optimizations for scalability and speed

#### `tests/` - Testing
- Unit tests for individual components
- Integration tests for system interactions
- Performance benchmarks
- Regression tests

#### `examples/` - Usage Examples
- Basic setup and configuration
- Advanced usage patterns
- Migration guides for version updates

## 2. Build System

### Prerequisites

#### CMake Configuration
```bash
# Minimum requirements
cmake --version >= 3.15
compiler with C++17 support
make or ninja build system
```

#### Build Options
```bash
# Configure with options
cmake -DOPTION=value ..

# Common options
-DNLM_ENABLE_PYTHON=ON/OFF     # Python bindings
-DNLM_PERFORMANCE_ENABLED=ON/OFF # Performance optimizations
-DNLM_DEVELOPMENT=ON/OFF         # Development features
-DNLM_TESTS=ON/OFF              # Build tests
-DNLM_EXAMPLES=ON/OFF           # Build examples
-DCMAKE_BUILD_TYPE=Debug/Release # Build configuration
```

### Build Commands

#### Standard Build
```bash
# Create and configure build directory
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with parallel compilation
make -j$(nproc)

# Install to system
make install
```

#### Development Build
```bash
# For development and debugging
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DNLM_DEVELOPMENT=ON ..
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

#### Performance Build
```bash
# Optimized for performance
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run benchmarks
./nlm --benchmark --neurons=10000 --steps=10000
```

### Build Artifacts

- **Static Library**: `libnlm.a` - Core library
- **Shared Library**: `libnlm.so` - Dynamic library (if enabled)
- **Python Module**: `nlm/` - Python bindings
- **Executable**: `nlm` - Main simulation executable
- **Test Programs**: Individual test executables

## 3. Development Environment

### IDE Configuration

#### Visual Studio Code
Create `.vscode/settings.json`:
```json
{
    "cmake.useCMakeFromPath": "CMakeTools",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureOnOpen": "always",
    "editor.formatOnSave": true,
    "editor.tabSize": 4,
    "cpplint.args": ["--filter=-build/include_subdir,-whitespace/line_length"]
}
```

#### CLion
- Import CMake project directly
- Use CMake integration for code navigation
- Configure compiler flags in project settings

### Editor Configuration

#### Clang-Format
Create `.clang-format`:
```yaml
BasedOnStyle: LLVM
IndentWidth: 4
UseTab: Never
BreakBeforeBraces: Allman
AllowShortIfStatementsOnASingleLine: false
IndentCaseLabels: false
ColumnLimit: 120
```

#### Clang-Tidy
Create `.clang-tidy`:
```yaml
Checks: '-*,cppcoreguidelines-*,modernize-*,performance-*,readability-*,bugprone-*'
WarningsAsErrors: true
```

### Development Tools

#### Code Analysis
```bash
# Linting
clang-tidy src/brain/Brain.cpp -- -I./include

# Static analysis
 cppcheck --enable=all src/

# Include guard checks
include-what-you-use
```

#### Debugging
```bash
# GDB for debugging
gdb --args ./nlm --test=unit

# LLDB (macOS)
lldb ./nlm -- test=unit

# Valgrind for memory issues
valgrind --leak-check=full ./nlm --benchmark
```

#### Profiling
```bash
# Performance profiling
perf record -g ./nlm --benchmark
perf report

# GNU profiler
gprof nlm main.o gmon.out

# Intel VTune
amplxe-cl --collect hotspots --result-dir=results ./nlm --benchmark
```

## 4. Code Quality and Conventions

### Naming Conventions

#### Variables and Functions
- **snake_case**: Private members, local variables
- **PascalCase**: Public class methods, types
- **CONSTANT_CASE**: Constants and macros

#### File Naming
- `.hpp`: Header files
- `.cpp`: Implementation files
- `_test.cpp`: Test files

### Documentation Standards

#### Doxygen-style Comments
```cpp
/**
 * Class Description.
 *
 * Key features and behavior description.
 *
 * @param parameter Description of parameter.
 * @return Return value description.
 * @note Important implementation details.
 * @warning Potential issues or limitations.
 * @see Related classes or functions.
 */
```

#### Header File Structure
```cpp
#pragma once

// Include guards and copyright notice

#include "dependencies"

namespace nlm {

/**
 * Class documentation.
 */
class ClassName {
public:
    // Public interface
    ClassName();                    // Constructor
    ~ClassName();                   // Destructor
    
    // Public methods
    ReturnType methodName(Parameter param) const;  // Method with const
    void setParameter(const std::string& param);   // Setter method
    
protected:
    // Protected members (access by derived classes)
    
private:
    // Private members (implementation details)
    Type memberName_;
};

} // namespace nlm
```

### Error Handling

#### Exception Hierarchy
```cpp
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message)
        : NLMException("Configuration: " + message) {}
};

class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message)
        : NLMException("Neural: " + message) {}
};
```

#### Error Handling Pattern
```cpp
void dangerousFunction() {
    try {
        // Code that may throw
        riskyOperation();
    } catch (const ConfigurationException& e) {
        // Handle configuration errors
        std::cerr << "Configuration error: " << e.what() << std::endl;
        throw;  // Re-throw with more context
    } catch (const NeuralException& e) {
        // Handle neural computation errors
        std::cerr << "Neural error: " << e.what() << std::endl;
        // Attempt recovery
    } catch (const std::exception& e) {
        // Handle other standard exceptions
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        throw NeuralException("Unexpected error in dangerousFunction");
    }
}
```

### Testing Strategy

#### Unit Testing
- Test individual classes and functions in isolation
- Mock dependencies where appropriate
- Aim for high code coverage (>90%)
- Use parameterized tests for variations

#### Integration Testing
- Test component interactions
- Test end-to-end scenarios
- Test with real configuration files
- Test performance characteristics

#### Test Framework
```cpp
// Example test structure
test/NeuralTest.cpp
#include <gtest/gtest.h>
#include "brain/Neuron.hpp"

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    }
    
    std::unique_ptr<nlm::Neuron> neuron;
};

TEST_F(NeuronTest, Initialization) {
    EXPECT_EQ(neuron->getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Internal);
}

TEST_F(NeuronTest, Firing) {
    neuron->setType(nlm::NeuronType::Motor);
    EXPECT_FALSE(neuron->isFiring());
}
```

## 5. Extension Development

### Adding New Neuron Types

#### Extending Neuron Class
```cpp
class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {
        setType(NeuronType::Modulatory);
    }
    
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Custom dynamics implementation
        // Call parent method or implement from scratch
        return Neuron::stepLIF(currentTime, dt);
    }
    
    // Override other methods as needed
};
```

### Adding New Plasticity Rules

#### Implementing PlasticityRule
```cpp
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom plasticity implementation
        // Calculate weight change based on spike timing
        float delta = calculateWeightChange(preSpikes, postSpikes, dt);
        synapse->addToWeight(delta);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Custom weight application with additional logic
        float newWeight = synapse->getWeight() + delta;
        // Apply bounds, regularization, etc.
        synapse->setWeight(clampWeight(newWeight));
    }
    
    const char* getName() const override {
        return "CustomPlasticity";
    }
    
private:
    float calculateWeightChange(const std::vector<Timestamp>& pre,
                               const std::vector<Timestamp>& post,
                               TimestepDuration dt) {
        // Custom weight change calculation
        // Could implement variations of STDP, Hebbian, etc.
    }
    
    SynapticWeight clampWeight(SynapticWeight weight) {
        // Apply weight bounds and constraints
        return std::clamp(weight, -1.0f, 1.0f);
    }
};
```

### Adding New Memory Systems

#### Implementing MemorySystem
```cpp
class CustomMemory : public MemorySystem {
public:
    void store(const std::vector<float>& pattern, MemoryId id) override {
        // Store pattern with custom encoding
        storedPatterns_[id] = encodePattern(pattern);
    }
    
    std::vector<float> retrieve(MemoryId id) const override {
        // Retrieve and decode pattern
        auto it = storedPatterns_.find(id);
        if (it != storedPatterns_.end()) {
            return decodePattern(it->second);
        }
        return {};
    }
    
    void clear() override {
        // Clear all stored patterns
        storedPatterns_.clear();
    }
    
    const char* getName() const override {
        return "CustomMemory";
    }
    
    // Additional custom methods
    void setRetentionPolicy(const RetentionPolicy& policy) {
        retentionPolicy_ = policy;
    }
    
    std::vector<MemoryId> getRecentMemories(Timestamp cutoff) const {
        // Return memories newer than cutoff
    }
    
private:
    std::map<MemoryId, EncodedPattern> storedPatterns_;
    RetentionPolicy retentionPolicy_;
    
    EncodedPattern encodePattern(const std::vector<float>& pattern) {
        // Custom encoding implementation
    }
    
    std::vector<float> decodePattern(const EncodedPattern& encoded) {
        // Custom decoding implementation
    }
};
```

## 6. Performance Optimization

### Performance Profiling

#### Identifying Bottlenecks
```bash
# Generate call graph
perf report --callgraph > callgraph.txt

# Analyze cache performance
perf stat -e cache-references,cache-misses ./nlm --benchmark

# Check branch prediction
perf stat -e branches,branch-misses ./nlm --benchmark
```

#### Hot Path Optimization
```cpp
// Before: Inefficient implementation
void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Multiple if statements in hot path
    if (refractoryRemaining > 0) {
        // ...
    }
    if (V >= threshold) {
        // ...
    }
    // More conditionals...
}

// After: Optimized implementation
void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Early exit for refractory (common case)
    if (refractoryRemaining > 0) {
        handleRefractory();
        return false;
    }
    
    // Fast path for non-spiking
    bool fired = integrateMembranePotential(dt);
    if (!fired) {
        return false;
    }
    
    // Spike handling
    handleSpike(currentTime);
    return true;
}
```

### Memory Optimization

#### Cache-Friendly Data Layout
```cpp
// Structure of Arrays (SoA) for cache efficiency
struct NeuronState {
    std::vector<MembranePotential> membranePotentials;
    std::vector<MembranePotential> restingPotentials;
    std::vector<MembranePotential> thresholds;
    // ... other arrays
};

// Vectorized operations for SIMD
void updateNeuronsSIMD(const NeuronState& state, std::vector<NeuronState>& newState) {
    for (size_t i = 0; i < state.membranePotentials.size(); i += 16) {
        // Process 16 neurons at once with AVX-512
        __m512 v_pot = _mm512_loadu_ps(&state.membranePotentials[i]);
        // ... SIMD operations
    }
}
```

### Parallel Processing

#### Thread Pool Implementation
```cpp
class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] { 
                            return stop_ || !tasks_.empty(); 
                        });
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }
    
    template<class F>
    void submit(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            tasks_.emplace(std::forward<F>(f));
        }
        condition_.notify_one();
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
    }
    
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_ = false;
};
```

## 7. Continuous Integration

### GitHub Actions Configuration

#### `.github/workflows/ci.yml`
```yaml
name: NLM CI/CD

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macOS-latest, windows-latest]
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up CMake
      uses: actions/setup-cmake@v3
      with:
        cmake-version: '3.15'
    
    - name: Configure
      run: |
        mkdir build
        cd build
        cmake -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} ..
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run Tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Run Benchmarks
      run: |
        cd build
        ./nlm --benchmark
```

### Pre-commit Hooks

#### `.pre-commit-config.yaml`
```yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-case-conflict
      - id: check-merge-conflict
      - id: check-symlinks
      - id: destroyed-symlinks
      - id: end-of-file-fixer
      - id: mixed-line-ending
      - id: trailing-whitespace
  
  - repo: https://github.com/cpplint/cpplint-action
    rev: 'v1'
    hooks:
      - id: cpplint
  
  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: 'v14.0.0'
    hooks:
      - id: clang-format
  
  - repo: https://github.com/jorevens/pyproject-flake8
    rev: '6.0.0'
    hooks:
      - id: flake8
```

## 8. Testing and Validation

### Test Coverage

#### Coverage Requirements
- **Unit Tests**: Minimum 90% line coverage
- **Integration Tests**: Minimum 80% feature coverage  
- **Performance Tests**: Automated regression detection
- **Documentation**: Examples must be tested and working

#### Coverage Tools
```bash
# Generate coverage report
gcov -b -d . nlm Brain.cpp

# Combine multiple files
covcombine test_*.gcda -o combined.gcda
covreport combined.gcda

# HTML report for documentation
cov-html combined.gcda
```

### Performance Validation

#### Benchmark Suite
```cpp
// tests/PerformanceTest.cpp
#include <benchmark/benchmark.h>

static void BM_NeuronStep(benchmark::State& state) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(randomGenerator);
    
    for (auto _ : state) {
        neuron.step(state.range(0));
    }
}

BENCHMARK(BM_NeuronStep)->Arg(1000)->Arg(10000);
```

#### Performance Regression Detection
```bash
# Run performance tests regularly
cmake -DNLM_PERFORMANCE_TESTS=ON ..
make performance_tests

# Compare with baseline
./performance_tests --baseline=previous_baseline.json
```

## 9. Documentation Generation

### Doxygen Configuration

#### `Doxyfile` Configuration
```cmake
# Enable documentation generation
set(DOXYGEN_ON TRUE)

# Doxygen options
set(DOXYGEN_PROJECT_NAME "NLM")
set(DOXYGEN_PROJECT_NUMBER "1.0.0")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
set(DOXYGEN_EXCLUDE_PATTERNS "*/tests/*" "*/examples/*")
```

#### Documentation Targets
```cmake
# Generate API documentation
add_custom_target(doc
    COMMAND doxygen Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation"
    VERBATIM
)
```

## 10. Contribution Guidelines

### Pull Request Process

1. **Branch Naming**: `feature/`, `bugfix/`, `docs/`, `refactor/`
2. **Commit Messages**: Use conventional commits
   - `feat: add new feature`
   - `fix: resolve bug`
   - `docs: update documentation`
   - `refactor: improve code structure`
3. **Review Process**: All PRs require at least one approval
4. **Testing**: All tests must pass before merge
5. **Documentation**: Update relevant documentation

### Code Review Checklist

#### For All Changes
- [ ] Code follows project conventions
- [ ] Tests written and passing
- [ ] Documentation updated
- [ ] Performance impact assessed
- [ ] Dependencies checked
- [ ] Compatibility verified

#### For New Features
- [ ] Extension points documented
- [ ] Usage examples provided
- [ ] Configuration options defined
- [ ] Error handling implemented
- [ ] Performance characteristics documented

### Version Management

#### Semantic Versioning
- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes (backward compatible)

#### Version Bumping
```bash
# Update version in CMakeLists.txt
# Update CHANGELOG.md
# Update version documentation
# Create git tag
cd /workspace/project
export NEW_VERSION=1.2.3
git checkout -b release/v${NEW_VERSION}
sed -i "s/version.*/version = ${NEW_VERSION}/" CMakeLists.txt
echo "## [${NEW_VERSION}] - $(date +%Y-%m-%d)" >> CHANGELOG.md
git add CMakeLists.txt CHANGELOG.md
git commit -m "Release version ${NEW_VERSION}"
git tag v${NEW_VERSION}
git push origin release/v${NEW_VERSION}
git push origin v${NEW_VERSION}
```

## 11. Migration Guides

### Version 5 to 6 Migration

#### Key Changes
1. **Configuration System**: Switched from JSON to YAML support
2. **Memory Management**: New sparse connectivity implementation
3. **Threading Model**: Lock-free queues for better scalability
4. **Performance**: SIMD optimizations added

#### Migration Steps
```bash
# 1. Update configuration files
cp configs/config_v5.json configs/config_v6.yaml

# 2. Update build system
cmake -DNLM_PERFORMANCE_ENABLED=ON -DNLM_YAML_SUPPORT=ON ..

# 3. Update code if using deprecated APIs
# Check for deprecated warnings in build output

# 4. Run migration tests
./nlm --migration-test
```

## 12. Resources

### Online Resources

- **Documentation**: https://docs.nlm.org
- **Community**: https://community.nlm.org
- **Issues**: https://github.com/nlm/issues
- **Pull Requests**: https://github.com/nlm/pulls
- **Discord**: https://discord.gg/nlm
- **Discord**: https://discord.gg/nlm

### Offline Resources

- **API Reference**: `docs/API-reference.md`
- **Performance Guide**: `docs/PERFORMANCE.md`
- **Architecture**: `docs/ARCHITECTURE.md`
- **Science**: `docs/SCIENCE.md`

### Training and Tutorials

- **Developer Workshop**: `examples/training/`
- **API Tutorial**: `examples/api/tutorial.md`
- **Performance Optimization**: `examples/performance/optimization.md`

## 13. Legal and Compliance

### Code License

NLM is distributed under the MIT License. All contributions must be licensed under the same terms.

### Attribution Requirements

When using NLM in publications or presentations:

```
This research used the Neural Learning Machine (NLM) framework,
a biologically-inspired neural network simulator.

Reference: NLM Documentation, https://docs.nlm.org
```

### Data Privacy

- No user data is collected by default
- Configuration files may contain sensitive information
- Users responsible for securing their configuration

## 14. Future Directions

### Upcoming Features

1. **GPU Acceleration**: CUDA support for large-scale simulations
2. **Distributed Computing**: Multi-machine scaling
3. **Advanced Plasticity**: More biologically accurate learning rules
4. **Cognitive Architecture**: Enhanced prediction and planning
5. **Heterogeneous Computing**: CPU-GPU hybrid processing

### Research Areas

- Neuromodulatory systems
- Developmental learning
- Social learning and imitation
- Meta-learning and adaptation
- Neural basis of consciousness

This document will be updated as the project evolves. For the most recent version, always check the development branch.