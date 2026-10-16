# NLM Developers Guide

## Overview

This guide provides comprehensive information for developers working on the NLM (熙然) neural learning machine project. It covers development setup, coding standards, testing, debugging, and contributing to the codebase.

## Development Environment Setup

### Prerequisites

1. **System Requirements**
   - **Operating Systems**: Linux, macOS, Windows
   - **Compiler**: C++17 compatible (GCC 9+, Clang 8+, MSVC 2019+)
   - **Build System**: CMake 3.15+
   - **Python**: 3.7+ (for documentation generation)

2. **Development Tools**
   - **Code Editor**: VS Code, CLion, or any C++ IDE with CMake support
   - **Git**: Version control
   - **Doxygen**: Documentation generation
   - **Python Packages**: Sphinx, Breathe, various markdown tools

### Setting Up the Development Environment

#### Clone the Repository
```bash
# Clone the repository
git clone https://github.com/neural-learning-machine/nlm.git
cd nlm
```

#### Create Development Branch
```bash
# Create and checkout a feature branch
git checkout -b feature/your-feature-name
```

#### Configure Build Directory
```bash
# Create dedicated build directory for development
mkdir dev_build
cd dev_build
```

#### Configure for Development
```bash
# Basic configuration for development
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# Advanced configuration with all features
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DBUILD_TESTS=ON \
         -DBUILD_EXAMPLES=ON \
         -DNLM_ENABLE_TESTS=ON \
         -DNLM_ENABLE_COVERAGE=ON \
         -DNLM_ENABLE_FORMATTING_CHECK=ON
```

#### Build the Project
```bash
# Build with parallel jobs
make -j$(nproc)

# Or use Ninja if available
# cmake .. -G "Ninja"
# ninja
```

### Development Workflow

#### 1. Understanding the Project Structure

The NLM project follows a modular architecture:

```
src/
├── core/           # Core utilities and infrastructure
│   ├── Config/     # Configuration management
│   ├── Logger/     # Logging system
│   ├── Random/     # Random number generation
│   ├── SimulationClock/ # Time management
│   └── Types/      # Fundamental types
│
├── brain/          # Neural core (Phase 6 integrated)
│   ├── Neuron.cpp  # Basic neuron implementation
│   ├── Brain.cpp   # Central brain coordinator
│   ├── NeuralRegion.cpp # Functional brain regions
│   └── Synapse.cpp  # Synaptic communication
│
├── dynamics/       # Neural dynamics and spike systems
├── plasticity/     # Plasticity rules (STDP, Hebbian)
├── development/    # Developmental system
├── neuromodulation/ # Neuromodulators (dopamine, curiosity, etc.)
├── memory/         # Memory systems (working, episodic, associative)
├── prediction/     # Prediction systems
├── cognition/      # Cognitive mechanisms
├── sensory/        # Sensory processing
├── motor/          # Motor system
└── environment/    # Environment interface
```

#### 2. Building and Running Tests

##### Running Unit Tests
```bash
# Navigate to build directory
cd dev_build

# Run all unit tests
ctest --output-on-failure

# Run specific test category
ctest -R "brain.*tests"
ctest -R "memory.*tests"
ctest -R "neuromodulation.*tests"
```

##### Running Integration Tests
```bash
# Run integration tests
./integration_test_suite

# Or run specific integration test
./integration_test_suite --test memory_integration
```

##### Running Performance Tests
```bash
# Run performance benchmarks
./performance_benchmark

# Run specific performance test
./performance_benchmark --test spike_processing
```

#### 3. Building Examples

##### Building All Examples
```bash
cd dev_build
make examples
```

##### Running Specific Examples
```bash
# Example 1: Basic brain simulation
./examples/ex01_basic_brain

# Example 2: Memory systems
./examples/ex02_memory_systems

# Example 3: Neuromodulation
./examples/ex03_neuromodulation

# Example 4: Development
./examples/ex04_development

# Example 5: Prediction
./examples/ex05_prediction

# Example 6: Cognition
./examples/ex06_cognition

# Example 7: Integration
./examples/ex07_integration

# Example 8: Advanced usage
./examples/ex08_advanced_usage

# Example 9: Visualization
./examples/ex09_visualization

# Example 10: Performance
./examples/ex10_performance
```

## Coding Standards

### 3.1 C++ Standards

#### C++ Version
- **Minimum**: C++17
- **Recommended**: C++20 (where supported)

#### Compiler Flags
```bash
# Recommended compiler flags for development
cmake .. -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Werror -O2"

# Debug builds
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release builds
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### 3.2 Code Style Guidelines

#### File Structure
```cpp
// Recommended header file structure
#pragma once

// Standard library includes
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>
#include <iostream>
#include <fstream>

// NLM includes (project-specific)
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

// Forward declarations
namespace nlm {
    // Class declarations
    class ExampleClass {
    public:
        // Constructor/destructor
        ExampleClass(std::shared_ptr<Config> config);
        ~ExampleClass();
        
        // Movement semantics
        ExampleClass(const ExampleClass&) = delete;
        ExampleClass& operator=(const ExampleClass&) = delete;
        ExampleClass(ExampleClass&&) noexcept;
        ExampleClass& operator=(ExampleClass&&) noexcept;
        
        // Core functionality
        bool initialize();
        void update(double timestep);
        void reset();
        
    private:
        // Implementation details
        struct Impl;
        std::unique_ptr<Impl> pImpl_;
    };
}
```

#### Naming Conventions
- **Classes**: CamelCase (e.g., `NeuralWorkingMemory`, `SpikeSystem`)
- **Functions**: camelCase (e.g., `getWorkingMemory()`, `initialize()`)
- **Variables**: camelCase for local variables, UPPER_SNAKE_CASE for constants
- **Types**: CamelCase (e.g., `MembranePotential`, `PlasticityFlags`)
- **Namespaces**: `nlm` for all NLM code

### 3.3 Documentation Standards

#### Doxygen Comments
```cpp
/**
 * @class NeuralWorkingMemory
 * @brief Manages working memory for active neural patterns
 * @details Working memory provides transient storage for currently
 * active neural patterns and contextual information. It has limited
 * capacity and is rapidly updated through neural activity.
 * 
 * @note Working memory is crucial for maintaining task context
 * and enabling planning based on current goals.
 * 
 * @see NeuralEpisodicMemory, NeuralAssociativeMemory
 * 
 * @ingroup memory_systems
 * 
 * @author NLM Development Team
 * @date 2026
 */
class NeuralWorkingMemory {
    /**
     * @brief Store a pattern in working memory
     * 
     * @param pattern Neural pattern to store (size must match memory capacity)
     * @throws std::invalid_argument if pattern size exceeds capacity
     * @note This will replace the oldest pattern if memory is full
     */
    void store(const std::vector<float>& pattern);
    
    /**
     * @brief Retrieve current pattern from working memory
     * 
     * @return Current pattern (empty if memory is empty)
     * @note Returns the most recently stored pattern
     */
    std::vector<float> retrieve() const;
    
    /**
     * @brief Check if working memory is full
     * 
     * @return true if memory contains the maximum number of patterns
     */
    bool isFull() const;
    
    /**
     * @brief Get the number of active patterns
     * 
     * @return Count of currently stored patterns
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Clear all patterns from working memory
     * 
     * @note Frees memory and prepares for new patterns
     */
    void clear();
};
```

#### Header File Documentation
- **Brief description**: One-line summary of class/function purpose
- **Detailed description**: Comprehensive explanation with technical details
- **Parameters**: Document all input parameters with types and descriptions
- **Returns**: Document return values with types and descriptions
- **Throws**: Document exceptions that may be thrown
- **See Also**: Reference related classes/functions
- **Note**: Important implementation details or warnings
- **Examples**: Code examples showing usage
- **Author**: Attribution
- **Date**: Creation/modification date

### 3.4 Memory Management

#### Smart Pointers
```cpp
// Use std::shared_ptr for shared ownership
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

// Use std::unique_ptr for exclusive ownership
std::vector<std::unique_ptr<NeuralRegion>> regions;
regions.push_back(std::make_unique<NeuralRegion>(/*...*/));

// Avoid raw pointers when possible
// BAD: Neuron* neuron = new Neuron(...);
// GOOD: auto neuron = std::make_unique<Neuron>(...);
```

#### Memory Pools
```cpp
// Use memory pools for high-performance systems
#include "../core/MemoryPool.hpp"

// Define specific memory pools
using NeuronPool = MemoryPool<Neuron, 1000>;
using SynapsePool = MemoryPool<Synapse, 5000>;

// Use pools for high-frequency allocations
Neuron* neuron = neuronPool.allocate(id, type, initialWeight);
synapsePool.deallocate(synapse);
```

#### Resource Cleanup
```cpp
// Ensure proper cleanup in destructors
Brain::~Brain() {
    // Stop simulation
    stopSimulation();
    
    // Save final state (checkpointing)
    save("final_checkpoint.brain");
    
    // Clean up memory systems
    if (workingMemory_) {
        workingMemory_->clear();
        delete workingMemory_;
        workingMemory_ = nullptr;
    }
    
    // Clean up other resources
    // ...
}
```

### 3.5 Error Handling

#### Exception Handling
```cpp
bool Brain::initialize() {
    try {
        // Initialize configuration
        auto config = std::make_shared<Config>();
        config->loadFromFile("config.json");
        
        // Initialize random generator
        uint64_t seed = config->getOr<uint64_t>("random_seed", 42);
        rng_ = std::make_unique<RandomGenerator>(seed);
        
        // Initialize components
        initializeNeurons();
        initializePlasticity();
        initializeMemorySystems();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    }
}
```

#### Error Codes
```cpp
enum class ErrorCode {
    SUCCESS = 0,
    CONFIG_ERROR = 1,           // Configuration error
    MEMORY_ERROR = 2,           // Memory allocation error
    INITIALIZATION_ERROR = 3,   // Initialization failure
    SIMULATION_ERROR = 4,       // Simulation step error
    NEUROMODULATION_ERROR = 5,  // Neuromodulation error
    MEMORY_SYSTEM_ERROR = 6,    // Memory system error
    DEVELOPMENT_ERROR = 7,      // Development system error
    ENVIRONMENT_ERROR = 8,      // Environment error
    AGENT_ERROR = 9,            // Agent error
    PERFORMANCE_ERROR = 10,     // Performance error
    UNKNOWN_ERROR = 99          // Unknown error
};
```

### 3.6 Testing

#### Unit Testing with Catch2
```cpp
// test_brain_basic.cpp
test_case("brain_basic_functionality") {
    given("brain is created with valid configuration") {
        auto config = std::make_shared<Config>();
        config->set("brain.maxNeurons", 100);
        
        auto brain = std::make_shared<Brain>(config);
        
        when("brain is initialized") {
            bool initialized = brain->initialize();
            
            then("initialization should succeed") {
                assert(initialized);
            }
            
            then("brain should have correct neuron count") {
                assert(brain->getTotalNeuronCount() == 100);
            }
        }
    }
}
```

#### Integration Testing
```cpp
// test_memory_integration.cpp
test_case("memory_system_integration") {
    given("brain with memory systems is created") {
        auto config = std::make_shared<Config>();
        config->set("brain.maxNeurons", 500);
        config->set("brain.memory.workingMemory.enabled", true);
        config->set("brain.memory.episodicMemory.enabled", true);
        config->set("brain.memory.associativeMemory.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        when("memory systems are accessed") {
            auto workingMemory = brain->getWorkingMemory();
            auto episodicMemory = brain->getEpisodicMemory();
            auto associativeMemory = brain->getAssociativeMemory();
            
            then("all memory systems should be accessible") {
                assert(workingMemory != nullptr);
                assert(episodicMemory != nullptr);
                assert(associativeMemory != nullptr);
            }
        }
    }
}
```

#### Performance Testing
```cpp
// test_performance.cpp
test_case("performance_benchmarks") {
    given("brain with large neuron count is created") {
        auto config = std::make_shared<Config>();
        config->set("brain.maxNeurons", 10000);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        when("simulation is run for many steps") {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < 1000; ++i) {
                brain->step(i);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end - start).count();
            
            then("performance should meet requirements") {
                double stepsPerSecond = 1000.0 / elapsed;
                assert(stepsPerSecond > 1000.0); // Should process >1000 steps/sec
            }
        }
    }
}
```

### 3.7 Build System

#### CMake Configuration
```cmake
# Basic CMake configuration
cmake_minimum_required(VERSION 3.15)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage" OFF)
option(NLM_ENABLE_FORMATTING_CHECK "Enable formatting checks" ON)

# Source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")

# Include directories
include_directories(src)

# Create library
add_library(NLM ${SRC_FILES} ${HEADER_FILES})

# Link libraries
target_link_libraries(NLM PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    m
)

# Install library
install(TARGETS NLM
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(FILES ${HEADER_FILES}
    DESTINATION include/nlm
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
```

#### Build Commands
```bash
# Configure build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Build examples
make examples

# Install locally
make install DESTDIR=/path/to/install
```

## Debugging and Troubleshooting

### 4.1 Debug Configuration

#### Enable Debug Logging
```cpp
// In main.cpp or debug.cpp
#include <nlm/Logger.hpp>

int main() {
    // Enable debug logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::DEBUG);
    
    // Enable specific components
    nlm::Logger::getInstance().enableComponent(nlm::LogComponent::Brain);
    nlm::Logger::getInstance().enableComponent(nlm::LogComponent::Neuromodulation);
    
    // ... rest of program
}
```

#### Debug Macros
```cpp
// debug_macros.h
#pragma once

#include <nlm/Logger.hpp>

#define DEBUG_LOG(message) \
    nlm::Logger::getInstance().logDebug(__FILE__, __LINE__, message)

#define INFO_LOG(message) \
    nlm::Logger::getInstance().logInfo(message)

#define ERROR_LOG(message) \
    nlm::Logger::getInstance().logError(message)

// Usage
DEBUG_LOG("Entering brain initialization");
INFO_LOG("Brain initialized successfully");
ERROR_LOG("Failed to load configuration");
```

### 4.2 Common Issues and Solutions

#### Issue: Brain Initialization Fails

**Symptoms:**
- `initialize()` returns false
- Error messages about configuration

**Solutions:**
```bash
# 1. Check configuration file
cat config.json | grep -E "(maxNeurons|learningRate)"

# 2. Validate configuration
./validate_config config.json

# 3. Check system resources
free -h
ulimit -a

# 4. Use default configuration
./nlm_simulation --use-defaults
```

#### Issue: Memory Allocation Fails

**Symptoms:**
- Segmentation fault
- Memory allocation errors
- Slow performance

**Solutions:**
```cpp
// Reduce memory usage
config->set("brain.maxNeurons", 500);  // From 2000
config->set("brain.memory.workingMemory.enabled", false);

// Use memory pools
config->set("brain.performance.useMemoryPools", true);

// Reduce simulation duration
config->set("brain.simulation.maxSteps", 1000);
```

#### Issue: Simulation Step Timeout

**Symptoms:**
- Steps taking too long
- Real-time deadline violations

**Solutions:**
```cpp
// Reduce network size
config->set("brain.maxNeurons", 1000);

// Optimize spike processing
config->set("brain.spikeProcessing.useCompression", true);

// Reduce plasticity frequency
config->set("brain.plasticity.updateFrequency", 10);
```

### 4.3 Profiling and Performance Analysis

#### Performance Profiling
```cpp
// profile_brain.cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <chrono>
#include <iostream>

class PerformanceProfiler {
public:
    struct Metrics {
        double totalTime;
        double initializationTime;
        double stepTime;
        double plasticityTime;
        double memoryTime;
        double spikeTime;
        uint64_t totalSpikes;
        uint64_t spikeEvents;
        size_t memoryUsage;
        double averageFiringRate;
        std::vector<double> stepTimes;
    };
    
    void startMeasurement() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stopMeasurement() {
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime_ = std::chrono::duration<double>(endTime - startTime_).count();
    }
    
    double getElapsedTime() const {
        auto currentTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(currentTime - startTime_).count();\n    // ... rest of implementation
}';
```

#### Profiling Commands
```bash
# Use gprof for profiling
make clean
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

gprof ./nlm_simulation program.prof program.from

# Use Valgrind for memory profiling
valgrind --tool=massif --stacks=yes ./nlm_simulation

# Use Intel VTune for performance analysis
# Need VTune installation and configuration
```

## Git Usage

### 5.1 Git Configuration

#### Configure User Information
```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

#### Configure Editor
```bash
git config --global core.editor "code --wait"
```

### 5.2 Branch Management

#### Create Feature Branch
```bash
# Create and checkout feature branch
git checkout -b feature/memory-optimization

# Push to remote
git push origin feature/memory-optimization
```

#### Merge Feature Branch
```bash
# Fetch and merge feature branch
git checkout main
git pull origin main
git merge origin feature/memory-optimization

# Resolve conflicts if any
# ...

# Clean up feature branch
git branch -d feature/memory-optimization
```

### 5.3 Commit Conventions

#### Conventional Commits
```bash
# Add your changes
git add .

# Commit with conventional format
git commit -m "feat: add memory system optimization

- Implement memory pool for better performance
- Add configuration options for memory management
- Update documentation with new features

Co-authored-by: OpenHands <openhands@all-hands.dev>"
```

#### Commit Message Examples
```bash
# Feature addition
git commit -m "feat(memory): add working memory system

- Implement working memory for active patterns
- Add configuration options
- Include unit tests"

# Bug fix
git commit -m "fix(brain): prevent memory leak in brain initialization

- Add proper cleanup in destructor
- Fix resource release order
- Add tests for cleanup"

# Documentation update
git commit -m "docs(api): add examples to Brain class documentation

- Add usage examples for key methods
- Update parameter descriptions
- Add references to related features"

# Performance improvement
git commit -m "perf: optimize spike processing for large networks

- Implement spike compression
- Use SIMD optimizations
- Update performance benchmarks"
```

### 5.4 Pull Request Process

#### Create Pull Request
```bash
# Push your branch
git push origin feature/memory-optimization

# Create pull request on GitHub
# Include description and reference issue
```

#### Pull Request Checklist
```markdown
**Before creating a pull request**:

- [ ] Branch created with descriptive name
- [ ] Code follows coding standards
- [ ] All tests pass
- [ ] Code review completed
- [ ] Documentation updated
- [ ] CHANGELOG updated
- [ ] Pull request created with template
- [ ] Reviewers assigned
- [ ] CI/CD pipeline triggered

#### Pull Request Template
```markdown
# Pull Request Template

## Description
Brief description of changes.

## Changes Made
- Added feature X
- Fixed bug Y
- Improved performance Z

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Performance benchmarks met
- [ ] Documentation updated

## Backward Compatibility
- [ ] No breaking changes
- [ ] Updated deprecated APIs
- [ ] Added new configuration options

## Performance Impact
- [ ] Performance improved
- [ ] Memory usage optimized
- [ ] CPU usage reduced

## Documentation
- [ ] API documentation updated
- [ ] Examples added/updated
- [ ] User guides updated

Co-authored-by: OpenHands <openhands@all-hands.dev>
```

## Running Tests Locally

### 6.1 Running Unit Tests
```bash
# Navigate to build directory
cd dev_build

# Run all tests
ctest --output-on-failure

# Run specific test category
ctest -R "brain.*tests"
ctest -R "memory.*tests"
ctest -R "neuromodulation.*tests"
```

### 6.2 Running Integration Tests
```bash
# Run integration tests
./integration_test_suite

# Or run specific integration test
./integration_test_suite --test memory_integration
```

### 6.3 Running Performance Tests
```bash
# Run performance benchmarks
./performance_benchmark

# Run specific performance test
./performance_benchmark --test spike_processing
```

## Code Review Guidelines

### 7.1 Review Criteria

**What to Review**:

- **Code Quality**: Clean, efficient, maintainable code
- **Test Coverage**: Comprehensive test coverage
- **Documentation**: Clear and complete documentation
- **Performance**: Meets performance requirements
- **Memory Management**: No memory leaks or issues
- **Thread Safety**: Proper thread safety where needed
- **Error Handling**: Proper error handling and recovery
- **Style Consistency**: Follows project conventions

**What to Approve**:

- **Feature Completeness**: Implements all required functionality
- **Test Coverage**: Sufficient test coverage
- **Documentation**: Complete documentation
- **Performance**: Meets performance requirements
- **Code Quality**: High code quality standards

### 7.2 Review Comments

**Use These Comment Types**:

1. **Technical Comment**: Technical feedback on implementation
2. **Style Comment**: Style or convention feedback
3. **Test Comment**: Test or testing feedback
4. **Documentation Comment**: Documentation feedback
5. **Performance Comment**: Performance feedback
6. **Security Comment**: Security or safety feedback

**Review Process**:

```markdown
Review Comments:

[ ] Technical: The implementation looks good but... [suggestions]

[ ] Style: Consider following... [conventions]

[ ] Tests: Need additional tests for... [specific functionality]

[ ] Documentation: Missing documentation for... [features]

[ ] Performance: Could be optimized by... [suggestions]

[ ] Memory: Potential memory leak in... [location]

[ ] Thread Safety: Not thread-safe in... [context]

[ ] Error Handling: Missing error handling for... [cases]

Overall Assessment:

[ ] Ready to merge
[ ] Needs changes before merging
[ ] Ready for release
```

## Documentation Standards

### 8.1 Doxygen Documentation

#### Header File Documentation
```cpp
/**
 * @class NeuralWorkingMemory
 * @brief Manages working memory for active neural patterns
 * @details Working memory provides transient storage for currently
 * active neural patterns and contextual information. It has limited
 * capacity and is rapidly updated through neural activity.
 * 
 * @note Working memory is crucial for maintaining task context
 * and enabling planning based on current goals.
 * 
 * @see NeuralEpisodicMemory, NeuralAssociativeMemory
 * 
 * @ingroup memory_systems
 * 
 * @author NLM Development Team
 * @date 2026
 */
```

#### Function Documentation
```cpp
/**
 * @brief Store a pattern in working memory
 * 
 * @param pattern Neural pattern to store (size must match memory capacity)
 * @throws std::invalid_argument if pattern size exceeds capacity
 * @note This will replace the oldest pattern if memory is full
 */
void store(const std::vector<float>& pattern);
```

### 8.2 API Documentation

The NLM API documentation is generated from Doxygen comments and maintained in:

- **docs/API_REFERENCE.md**: Complete API reference
- **examples/**: Usage examples
- **docs/TUTORIALS.md**: Step-by-step tutorials

### 8.3 Developer Documentation

- **CONTRIBUTING.md**: Contribution guidelines
- **BEST_PRACTICES.md**: Development best practices
- **TROUBLESHOOTING.md**: Common issues and solutions
- **PERFORMANCE_TUNING.md**: Performance optimization guide

## Release Process

### 9.1 Pre-Release Checklist

**Before releasing a version**:

1. [ ] All tests pass
2. [ ] Code review complete
3. [ ] Documentation complete
4. [ ] Examples tested
5. [ ] CHANGELOG updated
6. [ ] Version number updated
7. [ ] Build scripts updated
8. [ ] Package metadata updated
9. [ ] Release notes written
10. [ ] Deploy to staging environment

### 9.2 Release Checklist

**During release**:

1. [ ] Create release branch
2. [ ] Update version number
3. [ ] Update CHANGELOG
4. [ ] Create release tags
5. [ ] Build distributions
6. [ ] Test distributions
7. [ ] Deploy to production
8. [ ] Update documentation
9. [ ] Announce release

## License and Copyright

### 10.1 License Information

NLM is licensed under the MIT License. All contributions must:

1. **Be licensed under MIT**: Submit with MIT license
2. **Include copyright notice**: Add copyright to your contributions
3. **Maintain license information**: Preserve license headers

### 10.2 Copyright Guidelines

**When contributing code**:

1. **Add copyright notice**: Include copyright notice in files
2. **Use proper attribution**: Attribute original authors
3. **Maintain license headers**: Preserve existing license headers

```cpp
/*
 * Copyright 2026 NLM Contributors
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.opensource.org/licenses/mit-license.php
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

## Conclusion

Developing for NLM offers an exciting opportunity to contribute to cutting-edge neural learning machine research. By following these development guidelines, you can create high-quality code that advances the project while maintaining the high standards that make NLM successful.

**Key Principles**:

1. **Code Quality**: Write clean, efficient, and maintainable code
2. **Testing**: Write comprehensive tests for all changes
3. **Documentation**: Keep documentation up-to-date and comprehensive
4. **Performance**: Optimize for performance and efficiency
5. **Community**: Collaborate with other developers and share knowledge
6. **Quality**: Ensure code meets high standards before merging

**Getting Started**:

1. **Fork the repository**: Create your own copy
2. **Create a feature branch**: Work on specific features
3. **Write tests**: Ensure your changes are thoroughly tested
4. **Document changes**: Update documentation and comments
5. **Create pull requests**: Share your changes with the community
6. **Review code**: Participate in code reviews

The NLM project welcomes contributions from developers at all experience levels. Whether you're fixing a bug, adding a new feature, or improving documentation, your contributions help advance neural learning machine research and build a better future for artificial brain systems.

Happy coding! 🚀