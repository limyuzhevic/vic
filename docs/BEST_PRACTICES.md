# NLM Best Practices Guide

## Overview

This document provides comprehensive best practices for developing with the NLM (熙然) neural learning machine system. It covers coding standards, testing methodologies, configuration management, performance optimization, and other key aspects of working with this brain-inspired computational architecture.

## Coding Standards

### 1. C++ Standards and Style

#### C++ Version Requirements
```cpp
// Use C++17 features and beyond
cmake_minimum_required(VERSION 3.15)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

#### File Structure Convention
```cpp
// Recommended header file structure
#pragma once

// Standard library includes (alphabetical)
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>
#include <iostream>
#include <fstream>

// NLM includes (project-specific, grouped)
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

// Forward declarations
namespace nlm {
    // Class declarations
}
```

#### Naming Conventions
```cpp
// Classes: CamelCase
class NeuralWorkingMemory;
class SpikeSystem;
class DevelopmentSystem;

// Functions: camelCase
void storePattern(const std::vector<float>& pattern);
float getAverageFiringRate() const;

// Variables: camelCase (local), UPPER_SNAKE_CASE (constants)
const float DEFAULT_LEARNING_RATE = 0.01f;
std::vector<float> experienceVector;

// Types: CamelCase
using NeuronId = uint32_t;
using SimulationStep = uint64_t;
using MembranePotential = float;

// Namespaces: nlm for all NLM code
namespace nlm {
    // All declarations
}
```

### 2. Documentation Standards

#### Doxygen Documentation Template
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
 * @version 1.0
 */
class NeuralWorkingMemory {
    /**
     * @brief Store a pattern in working memory
     * 
     * @param pattern Neural pattern to store (size must match memory capacity)
     * @throws std::invalid_argument if pattern size exceeds capacity
     * @note This will replace the oldest pattern if memory is full
     * @example
     * @code
     * std::vector<float> pattern = {0.1f, 0.5f, 0.8f};
     * memory->store(pattern);
     * @endcode
     */
    void store(const std::vector<float>& pattern);
    
    /**
     * @brief Retrieve current pattern from working memory
     * 
     * @return Current pattern (empty if memory is empty)
     * @note Returns the most recently stored pattern
     * @return std::vector<float> The retrieved pattern
     */
    std::vector<float> retrieve() const;
    
    /**
     * @brief Check if working memory is full
     * 
     * @return true if memory contains the maximum number of patterns
     * @note Useful for determining when to clear old patterns
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

#### Header File Documentation Checklist
- [ ] Brief description: One-line summary of class/function purpose
- [ ] Detailed description: Comprehensive explanation with technical details
- [ ] Parameters: Document all input parameters with types and descriptions
- [ ] Returns: Document return values with types and descriptions
- [ ] Throws: Document exceptions that may be thrown
- [ ] See Also: Reference related classes/functions
- [ ] Note: Important implementation details or warnings
- [ ] Examples: Code examples showing usage
- [ ] Author: Attribution
- [ ] Date: Creation/modification date
- [ ] Version: API version information

### 3. Memory Management

#### Smart Pointer Usage
```cpp
// Use std::shared_ptr for shared ownership
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);
auto sharedMemory = brain->getWorkingMemory();

// Use std::unique_ptr for exclusive ownership
std::vector<std::unique_ptr<NeuralRegion>> regions;
regions.push_back(std::make_unique<NeuralRegion>(/*...*/));

// Avoid raw pointers when possible
// BAD: Neuron* neuron = new Neuron(...);
// GOOD: auto neuron = std::make_unique<Neuron>(...);
```

#### Memory Pool Usage
```cpp
// Include memory pool header
#include "../core/MemoryPool.hpp"

// Define specialized memory pools
using NeuronPool = MemoryPool<Neuron, 1000>;
using SynapsePool = MemoryPool<Synapse, 5000>;
using ConfigPool = MemoryPool<Config, 100>;

// Use pools for high-frequency allocations
void initializeNeurons(NeuronPool& neuronPool, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        NeuronId id = i;
        NeuronType type = NeuronType::Interneuron;
        auto neuron = neuronPool.allocate(id, type, /* initialWeight */);
        // Use neuron...
        
        // When done, deallocate
        neuronPool.deallocate(neuron);
    }
}
```

#### Resource Cleanup
```cpp
// Ensure proper cleanup in destructors
Brain::~Brain() {
    try {
        // Stop simulation gracefully
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
        
    } catch (const std::exception& e) {
        // Log cleanup failure but don't throw
        Logger::getInstance().logError(std::string("Cleanup failed: ") + e.what());
    }
}
```

### 4. Error Handling

#### Exception Handling Strategy
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
        // Attempt cleanup and recovery
        cleanupFailedInitialization();
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
    NEUROMODULATION_ERROR = 5, // Neuromodulation error
    MEMORY_SYSTEM_ERROR = 6,    // Memory system error
    DEVELOPMENT_ERROR = 7,      // Development system error
    ENVIRONMENT_ERROR = 8,      // Environment error
    AGENT_ERROR = 9,            // Agent error
    PERFORMANCE_ERROR = 10,     // Performance error
    UNKNOWN_ERROR = 99          // Unknown error
};

ErrorCode Brain::initialize() {
    try {
        // ... initialization logic ...
        
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed");
        return ErrorCode::MEMORY_ERROR;
    } catch (const std::invalid_argument& e) {
        NLM_LOG_ERROR(std::string("Invalid configuration: ") + e.what());
        return ErrorCode::CONFIG_ERROR;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Initialization failed: ") + e.what());
        return ErrorCode::INITIALIZATION_ERROR;
    }
}
```

#### Error Recovery Patterns
```cpp
// Retry mechanism for transient failures
bool loadBrainState(const std::string& filepath, int maxRetries = 3) {
    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        try {
            if (brain->load(filepath)) {
                return true;
            }
        } catch (const std::exception& e) {
            if (attempt == maxRetries - 1) {
                NLM_LOG_ERROR(std::string("Failed to load brain after ") + 
                           std::to_string(maxRetries) + std::string(" attempts: ") + e.what());
                throw;
            }
            // Exponential backoff
            std::this_thread::sleep_for(std::chrono::milliseconds(100 * (1 << attempt)));
        }
    }
    return false;
}
```

### 5. Testing

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

### 6. Build System

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

### 7. Git Usage

#### Commit Conventions
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

#### Branch Management
```bash
# Create feature branch
git checkout -b feature/memory-optimization

# Push to remote
git push origin feature/memory-optimization

# Merge feature branch
git checkout main
git pull origin main
git merge origin feature/memory-optimization

# Resolve conflicts if any
# ...

# Clean up feature branch
git branch -d feature/memory-optimization
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
```

## Performance Optimization

### 1. Memory Optimization
```cpp
// Use memory pools for allocation-heavy operations
#include "../core/MemoryPool.hpp"

struct NeuronMemoryPoolConfig {
    static constexpr size_t poolSize = 1000;
    static constexpr size_t maxAllocs = 10000;
};

using NeuronMemoryPool = MemoryPool<Neuron, NeuronMemoryPoolConfig::poolSize>;

// Replace dynamic allocation with pool
Neuron* createNeuron(NeuronMemoryPool& pool, NeuronId id, NeuronType type) {
    return pool.allocate(id, type);
}
```

### 2. Computational Optimization
```cpp
// Use SIMD for parallel neuron computations
#include <xmmintrin.h> // SSE
#include <smmintrin.h> // SSE4.1

void processNeuronsSIMD(const std::vector<Neuron>& neurons) {
    for (size_t i = 0; i < neurons.size(); i += 4) {
        // Process 4 neurons simultaneously using SIMD
        __m128 potential1 = _mm_loadu_ps(&neurons[i].membranePotential);
        __m128 potential2 = _mm_loadu_ps(&neurons[i + 1].membranePotential);
        // ... SIMD operations ...
        
        _mm_storeu_ps(&neurons[i].membranePotential, potential1);
        _mm_storeu_ps(&neurons[i + 1].membranePotential, potential2);
    }
}
```

### 3. Event-Driven Updates
```cpp
// Only process spikes when they occur, not every timestep
class SpikeSystem {
public:
    void step(SimulationStep step) {
        // Only process if there are pending spikes
        if (!pendingSpikes_.empty()) {
            processPendingSpikes();
            clearProcessedSpikes();
        }
        
        // Update neuron dynamics based on timer
        updateNeuronDynamics(step);
    }
    
private:
    void processPendingSpikes() {
        for (const auto& spike : pendingSpikes_) {
            propagateSpike(spike.neuronId, spike.synapseId);
        }
    }
};
```

## Development Workflow

### 1. Daily Workflow
```bash
# Start of day: sync changes
git status
git log --oneline -5

# Work on specific issue
# Make changes, write tests, update documentation

# End of day: commit changes
git add .
git commit -m "feat: implement feature X

- Add new functionality
- Update documentation
- Write tests

Co-authored-by: OpenHands <openhands@all-hands.dev>"
```

### 2. Code Review Process
```markdown
**Review Criteria**:

- **Code Quality**: Clean, efficient, maintainable code
- **Test Coverage**: Comprehensive test coverage
- **Documentation**: Clear and complete documentation
- **Performance**: Meets performance requirements
- **Memory Management**: No memory leaks or issues
- **Thread Safety**: Proper thread safety where needed
- **Error Handling**: Proper error handling and recovery
- **Style Consistency**: Follows project conventions

**Review Comments Types**:

1. **Technical Comment**: Technical feedback on implementation
2. **Style Comment**: Style or convention feedback
3. **Test Comment**: Test or testing feedback
4. **Documentation Comment**: Documentation feedback
5. **Performance Comment**: Performance feedback
6. **Security Comment**: Security or safety feedback
```

### 3. Release Process
```markdown
**Pre-Release Checklist**:

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

**Release Checklist**:

1. [ ] Create release branch
2. [ ] Update version number
3. [ ] Update CHANGELOG
4. [ ] Create release tags
5. [ ] Build distributions
6. [ ] Test distributions
7. [ ] Deploy to production
8. [ ] Update documentation
9. [ ] Announce release
```

## Configuration Management

### 1. Configuration Files
```bash
# Configuration directory
configs/
├── config.json          # Default JSON configuration
├── config.xml           # XML configuration template
├── config.yaml          # YAML configuration template
├── config_memory.json    # Memory-optimized configuration
├── config_performance.json # Performance-optimized configuration
└── config_development.json # Development configuration
```

### 2. Configuration Validation
```cpp
// Validate configuration for correctness
class ConfigValidator {
public:
    static std::vector<std::string> validate(const Config& config) {
        std::vector<std::string> errors;
        
        // Validate neuron count
        auto neuronCount = config.getOr<int>("brain.maxNeurons", 0);
        if (neuronCount <= 0) {
            errors.push_back("brain.maxNeurons must be positive");
        } else if (neuronCount > 1000000) {
            errors.push_back("brain.maxNeurons too large (max: 1000000)");
        }
        
        // Validate learning rate
        auto learningRate = config.getOr<double>("brain.learningRate", 0.0);
        if (learningRate < 0.0 || learningRate > 1.0) {
            errors.push_back("brain.learningRate must be between 0.0 and 1.0");
        }
        
        // Validate consistency between memory systems
        auto workingMemoryEnabled = config.getOr<bool>("brain.memory.workingMemory.enabled", false);
        auto episodicMemoryEnabled = config.getOr<bool>("brain.memory.episodicMemory.enabled", false);
        auto associativeMemoryEnabled = config.getOr<bool>("brain.memory.associativeMemory.enabled", false);
        
        if (workingMemoryEnabled && !episodicMemoryEnabled && !associativeMemoryEnabled) {
            errors.push_back("At least one other memory system must be enabled with working memory");
        }
        
        return errors;
    }
};
```

## Collaboration Guidelines

### 1. Documentation Standards
- Keep documentation concise but comprehensive
- Include code examples for complex concepts
- Use consistent formatting and structure
- Update documentation with code changes
- Use version control for documentation history

### 2. Code Review Etiquette
- Be constructive and specific in feedback
- Focus on code quality, not personal preferences
- Encourage diverse perspectives
- Respect time and expertise of reviewers
- Be prompt in addressing review comments

### 3. Community Engagement
- Help new users and contributors
- Share knowledge and expertise
- Participate in code reviews
- Contribute to documentation improvements
- Support community events and discussions

## Conclusion

Following these best practices ensures:

1. **Code Quality**: Robust, maintainable, and efficient code
2. **Testing Coverage**: Comprehensive testing for reliability
3. **Documentation**: Clear and accessible documentation
4. **Performance**: Optimized for both accuracy and efficiency
5. **Collaboration**: Effective team development workflow
6. **Sustainability**: Long-term maintainability and extensibility

**Key Principles**:

1. **Code Quality**: Write clean, efficient, and maintainable code
2. **Testing**: Write comprehensive tests for all changes
3. **Documentation**: Keep documentation up-to-date and comprehensive
4. **Performance**: Optimize for performance and efficiency
5. **Community**: Collaborate with other developers and share knowledge
6. **Quality**: Ensure code meets high standards before merging

The NLM project welcomes contributions from developers at all experience levels. By following these best practices, you contribute to creating a high-quality, sustainable codebase that advances neural learning machine research.