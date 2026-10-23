# NLM (Neural Learning Machine) - Code Style Guide

This document provides coding standards and best practices for the NLM project. Following these guidelines ensures code quality, maintainability, and consistency across the codebase.

## Overview

The NLM project uses modern C++20 with a focus on:

- **Performance**: Efficient algorithms and memory management
- **Readability**: Clear, self-documenting code
- **Maintainability**: Consistent patterns and organization
- **Safety**: Robust error handling and memory safety

## C++ Coding Standards

### General Principles

1. **Use modern C++**: Leverage C++20 features where appropriate
2. **RAII**: Use Resource Acquisition Is Initialization for resource management
3. **Zero-cost abstractions**: Design abstractions that don't compromise performance
4. **Don't Repeat Yourself (DRY)**: Eliminate code duplication
5. **Separation of concerns**: Keep classes and functions focused on single responsibilities

### Syntax and Formatting

#### Indentation

- **Spaces only**: No tabs
- **4 spaces per indent level**
- **Consistent indentation**: Never mix tabs and spaces

Example:

```cpp
class NeuralWorkingMemory {
private:
    struct Impl {
        Brain* brain;
        // ...
    };
    
    std::unique_ptr<Impl> pImpl;
    // ...
};
```

#### Line Length

- **Maximum 120 characters** for source code lines
- **Maximum 100 characters** for documentation and comments
- **Break long lines** at logical points, not in the middle of expressions

#### Brackets

```cpp
// If
if (condition) {
    // body
}

// For
for (auto it = container.begin(); it != container.end(); ++it) {
    // body
}

// While
while (condition) {
    // body
}

// Try-catch
if (condition) {
    try {
        // risky code
    } catch (const std::exception& e) {
        // error handling
    }
} else {
    // fallback
}
```

#### Spacing

```cpp
// With spaces
int x = 10 + 5 * 2;  // good

int x=10+5*2;        // bad

// Function calls
std::cout << "Hello" << std::endl;

// Operators
auto sum = a + b * c - d / e;

// Templates
std::vector<int> vec{1, 2, 3};
std::map<std::string, int> map;
```

### Naming Conventions

#### Types and Classes

| Type | Convention | Example |
|------|------------|---------|
| **Classes** | PascalCase | `NeuralWorkingMemory` |
| **Structs** | PascalCase | `SimulationStep` |
| **Enums** | PascalCase | `NeuronType` |
| **Typedefs** | CamelCase | `NeuronId`, `SynapseId` |
| **Type aliases** | CamelCase | `Matrix = std::vector<std::vector<double>>` |

#### Variables and Functions

| Entity | Convention | Example |
|--------|------------|---------|
| **Variables** | snake_case | `simulation_step`, `neuron_id` |
| **Functions** | snake_case | `store_neural_memory()` |
| **Constants** | UPPER_SNAKE_CASE | `MAX_NEURONS = 10000` |
| **Macros** | UPPER_SNAKE_CASE | `#define PI 3.14159` |
| **Namespaces** | snake_case | `namespace nlm { ... }` |

#### Member Variables

- **Private**: `private:` section first
- **Protected**: `protected:` section after private
- **Public**: `public:` section last
- **Immutable**: `const` for truly immutable members
- **References**: Use `const` references for parameters

Example:

```cpp
class Brain {
public:
    Brain(const Config& config);
    ~Brain() = default;
    
    // Public interface
    void initialize();
    void step(SimulationStep step);
    int getTotalNeuronCount() const;
    
private:
    // Configuration
    Config config_;
    
    // Neural components
    std::vector<std::unique_ptr<Neuron>> neurons_;
    std::vector<std::unique_ptr<Synapse>> synapses_;
    
    // State
    int total_spikes_ = 0;
    bool is_initialized_ = false;
    
    // Internal implementation
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};
```

### Documentation

#### Doxygen Comments

Use Doxygen format for C++ documentation:

```cpp
/**
 * @class NeuralWorkingMemory
 * @brief Neural-enhanced working memory system
 * @details This class provides neural integration with working memory,
 *          implementing both maintenance connections and competition dynamics.
 *
 * @note Phase 6: Enhanced neural working memory with attentional selection
 * @see WorkingMemory, NeuralEpisodicMemory
 */
class NeuralWorkingMemory {
    // ...
};
```

#### Function Documentation

```cpp
/**
 * @brief Store a neural pattern in working memory
 * @param pattern The neural pattern to store (vector of activation values)
 * @param strength The initial strength of the memory trace
 * @param neuron_id Optional specific neuron to store in
 * @return bool True if stored successfully, false otherwise
 *
 * @details This method implements pattern storage with automatic
 *          competition and maintenance connections. The pattern is
 *          distributed across available neurons using Hebbian learning.
 *
 * @note Memory capacity is limited and traces decay over time.
 * @see retrieve(), update()
 *
 * @param[in] pattern The input pattern vector
 * @param[in] strength Initial strength (0.0 to 1.0)
 * @param[in] neuron_id Optional target neuron
 * @return Storage success status
 */
bool store(const std::vector<float>& pattern, float strength,
           NeuronId neuron_id = INVALID_NEURON_ID);
```

#### Header File Structure

1. **License information** (if not in main license)
2. **Include guards** (`#pragma once` or `#ifndef _FILENAME_HPP_`)
3. **Module documentation** (brief description)
4. **Includes**: Standard library headers first, then project headers
5. **Namespaces**: One `namespace nlm` block
6. **Type definitions**: Forward declarations and typedefs
7. **Class declarations**: Public first, then protected, then private
8. **Inline functions**: At the end (implementation in headers)

### Error Handling

#### Exception Safety

1. **Basic exception safety**: Objects remain in valid state
2. **Strong exception safety**: Either complete operation or none
3. **No-throw guarantee**: Operations that don't allocate memory

#### Exception Usage

```cpp
class Brain {
public:
    void initialize() {
        try {
            // Resource allocation
            neurons_.reserve(config_.getNeuronCount());
            
            // Initialize neurons
            for (int i = 0; i < config_.getNeuronCount(); ++i) {
                neurons_.push_back(std::make_unique<Neuron>(i));
            }
            
            // Initialize synapses
            initializeSynapses();
            
            is_initialized_ = true;
            
        } catch (const std::exception& e) {
            // Clean up partially initialized state
            neurons_.clear();
            synapses_.clear();
            is_initialized_ = false;
            
            // Log error
            Logger::getInstance().error(std::string("Brain initialization failed: ") + e.what());
            
            // Re-throw
            throw;
        }
    }
    
private:
    void initializeSynapses() {
        // May throw
    }
};
```

#### Custom Exceptions

```cpp
// Custom exception classes for NLM
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message)
        : NLMException("Config error: " + message) {}
};

class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException("Memory error: " + message) {}
};
```

### Memory Management

#### Smart Pointers

```cpp
// Use std::unique_ptr for exclusive ownership
class Brain {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Use std::shared_ptr for shared ownership
class MemoryManager {
private:
    std::shared_ptr<Brain> brain_;
    std::vector<std::weak_ptr<Neuron>> neurons_;
};

// Use std::reference_wrapper for references
class NeuralNetwork {
private:
    std::vector<std::reference_wrapper<Neuron>> neurons_;
};
```

#### Resource Management

```cpp
class ResourceHolder {
public:
    ResourceHolder() {
        resource_ = allocateResource();
    }
    
    ~ResourceHolder() {
        deallocateResource(resource_);
    }
    
    // No copy constructor or assignment operator
    ResourceHolder(const ResourceHolder&) = delete;
    ResourceHolder& operator=(const ResourceHolder&) = delete;
    
    // Move constructor and assignment
    ResourceHolder(ResourceHolder&& other) noexcept {
        resource_ = other.resource_;
        other.resource_ = nullptr;
    }
    
    ResourceHolder& operator=(ResourceHolder&& other) noexcept {
        if (this != &other) {
            deallocateResource(resource_);
            resource_ = other.resource_;
            other.resource_ = nullptr;
        }
        return *this;
    }
    
    void* getResource() const { return resource_; }
    
private:
    void* resource_ = nullptr;
    
    static void* allocateResource();
    static void deallocateResource(void* ptr);
};
```

### Concurrency and Threading

#### Thread Safety

```cpp
class ThreadSafeBrain {
public:
    void step(SimulationStep step) {
        std::lock_guard<std::mutex> lock(mutex_);
        // Thread-safe operations
    }
    
    int getTotalNeuronCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return neuron_count_;
    }
    
private:
    mutable std::mutex mutex_;
    int neuron_count_ = 0;
    std::vector<Neuron> neurons_;
};
```

#### Thread-Local Storage

```cpp
class ThreadLocalSimulation {
public:
    static ThreadLocalSimulation& getInstance() {
        thread_local ThreadLocalSimulation instance;
        return instance;
    }
    
    void setStep(SimulationStep step) {
        thread_local_step_ = step;
    }
    
    SimulationStep getStep() const {
        return thread_local_step_;
    }
    
private:
    ThreadLocalSimulation() = default;
    ~ThreadLocalSimulation() = default;
    
    thread_local SimulationStep thread_local_step_;
};
```

### Performance Optimization

#### Performance Considerations

1. **Minimize allocations**: Reuse objects where possible
2. **Cache locality**: Organize data for good cache performance
3. **Use move semantics**: Avoid unnecessary copies
4. **Pre-allocate**: Reserve capacity when possible
5. **Avoid virtual functions**: For performance-critical code

#### Optimization Patterns

```cpp
// Use pre-allocated buffers for frequent allocations
class OptimizedMemoryPool {
public:
    OptimizedMemoryPool(size_t capacity) {
        buffer_ = std::make_unique<float[]>(capacity);
        capacity_ = capacity;
        current_ = 0;
    }
    
    float* allocate(size_t count) {
        if (current_ + count > capacity_) {
            throw std::bad_alloc();
        }
        float* result = &buffer_[current_];
        current_ += count;
        return result;
    }
    
    void reset() {
        current_ = 0;
    }
    
private:
    std::unique_ptr<float[]> buffer_;
    size_t capacity_ = 0;
    size_t current_ = 0;
};
```

#### SIMD and Vectorization

```cpp
// Use SIMD for mathematical operations
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2

class VectorMath {
public:
    static void add(const float* a, const float* b, float* result, size_t n) {
        for (size_t i = 0; i < n; i += 4) {
            __m128 va = _mm_loadu_ps(&a[i]);
            __m128 vb = _mm_loadu_ps(&b[i]);
            __m128 vs = _mm_add_ps(va, vb);
            _mm_storeu_ps(&result[i], vs);
        }
    }
};
```

### Build System and Dependencies

#### CMake Integration

```cpp
// CMakeLists.txt - Example target
add_executable(nlm_main src/main.cpp)

target_include_directories(nlm_main PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_main PUBLIC cxx_std_20)

// Dependencies
find_package(pybind11 REQUIRED)
find_package(Threads REQUIRED)

// Link libraries
target_link_libraries(nlm_main
    PRIVATE
        nlm_core
        Threads::Threads
        ${PYTHON_LIBRARIES}
)
```

#### Compiler Warnings

```cpp
// Enable maximum warnings
// In CMakeLists.txt:
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

// Platform-specific warnings
#ifdef _MSC_VER
    #pragma warning(disable: 4267) // size_t to int conversion
    #pragma warning(disable: 4305) // truncation from 'double' to 'float'
#else
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
```

#### Configuration

```cpp
// Compile-time configuration
#define NLM_MAX_NEURONS 10000
#define NLM_ENABLE_PROFILING 1

// Runtime configuration via Config class
class Config {
public:
    template<typename T>
    T getValue(const std::string& key) const {
        auto it = values_.find(key);
        if (it == values_.end()) {
            throw ConfigException("Missing required config key: " + key);
        }
        return std::any_cast<T>(it->second);
    }
    
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        validateValue(key, value);
        values_[key] = value;
    }
    
private:
    std::unordered_map<std::string, std::any> values_;
    
    template<typename T>
    void validateValue(const std::string& key, const T& value) {
        // Type and range validation
        if (key == "brain.neuron_count") {
            int count = std::any_cast<int>(value);
            if (count <= 0 || count > NLM_MAX_NEURONS) {
                throw ConfigException("Invalid neuron count: " + std::to_string(count));
            }
        }
        // Add more validation as needed
    }
};
```

### Testing Standards

#### Unit Testing

```cpp
// Google Test example
#include <gtest/gtest.h>
#include "Brain.hpp"

TEST(BrainTest, InitializationSetsCorrectNeuronCount) {
    Config config;
    config.setValue("brain.neuron_count", 100);
    
    Brain brain(config);
    brain.initialize();
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_EQ(brain.getFiringNeuronCount(), 0);
}

TEST(BrainTest, StepUpdatesSpikes) {
    Config config;
    config.setValue("brain.neuron_count", 10);
    
    Brain brain(config);
    brain.initialize();
    
    int initial_spikes = brain.getTotalSpikeCount();
    brain.step(0);
    
    EXPECT_GT(brain.getTotalSpikeCount(), initial_spikes);
}
```

#### Integration Testing

```cpp
// Integration test example
#include <gtest/gtest.h>
#include "Brain.hpp"
#include "SimpleWorld.hpp"

class AgentIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = Config::createDefault();
        config.setValue("brain.neuron_count", 50);
        
        brain = std::make_unique<Brain>(config);
        brain->initialize();
        
        agent = std::make_unique<AgentBrain>(brain.get());
        world = std::make_unique<SimpleWorld>();
        
        world->configure(20, 20, 8, 8);
        world->reset();
        
        agent->initialize(*world);
    }
    
    Config config;
    std::unique_ptr<Brain> brain;
    std::unique_ptr<AgentBrain> agent;
    std::unique_ptr<SimpleWorld> world;
};

TEST_F(AgentIntegrationTest, CompleteAgentLoop) {
    // Test complete agent-world interaction
    for (int step = 0; step < 100; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    // Verify final state
    EXPECT_GT(brain->getFiringNeuronCount(), 0);
}
```

## Python Coding Standards

### PEP 8 Compliance

Follow PEP 8 (Python Style Guide) with NLM-specific adjustments:

```python
# PEP 8 compliant with NLM adjustments
def run_simulation(brain, world, agent, num_steps):
    """
    Run a complete simulation loop.
    
    Args:
        brain: The neural brain object
        world: The world object
        agent: The agent brain interface
        num_steps: Number of simulation steps to run
        
    Returns:
        dict: Simulation statistics
    """
    stats = {
        'total_steps': num_steps,
        'final_firing_rate': 0.0,
        'max_firing_rate': 0.0,
        'curiosity_levels': [],
    }
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.get_simulation_time())
        
        # Collect statistics
        firing_rate = brain.get_average_firing_rate()
        stats['curiosity_levels'].append(agent.get_curiosity_level())
        
        if firing_rate > stats['max_firing_rate']:
            stats['max_firing_rate'] = firing_rate
    
    stats['final_firing_rate'] = brain.get_average_firing_rate()
    return stats
```

#### Python Naming Conventions

- **Variables**: snake_case (e.g., `simulation_step`, `neuron_id`)
- **Functions**: snake_case (e.g., `get_neuron_count()`)
- **Classes**: PascalCase (e.g., `SimpleWorld`, `AgentBrain`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_SIMULATION_STEPS`)
- **Modules**: snake_case (e.g., `simple_world.py`, `agent_brain.py`)

#### Python Documentation

```python
class SimpleWorld:
    """
    A simple 2D world for agent-environment interaction.
    
    The world simulates a 2D environment where agents can move, sense,
    and interact with objects. It provides a complete simulation
    environment for testing and demonstrating NLM capabilities.
    
    Attributes:
        width: World width in simulation units
        height: World height in simulation units
        vision_width: Agent's vision width
        vision_height: Agent's vision height
        
    Examples:
        >>> world = SimpleWorld()
        >>> world.configure(width=20, height=20, vision_width=8, vision_height=8)
        >>> world.reset()
        >>> world.set_agent_start(10.0, 10.0)
    """
    
    def __init__(self):
        """Initialize a new world instance."""
        self.width = 0.0
        self.height = 0.0
        self.vision_width = 0
        self.vision_height = 0
        self.simulation_time = 0.0
        
    def configure(self, width, height, vision_width, vision_height):
        """
        Configure world dimensions and agent vision.
        
        Args:
            width: World width in simulation units
            height: World height in simulation units
            vision_width: Agent's vision width in cells
            vision_height: Agent's vision height in cells
            
        Raises:
            ValueError: If any dimension is invalid
        """
        if width <= 0 or height <= 0:
            raise ValueError("World dimensions must be positive")
        if vision_width <= 0 or vision_height <= 0:
            raise ValueError("Vision dimensions must be positive")
            
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
```

### Python Error Handling

```python
import pynlm
from typing import Optional, Dict, Any

class SimulationError(Exception):
    """Base exception for simulation errors."""
    pass

class ConfigurationError(SimulationError):
    """Exception raised for configuration errors."""
    pass

class BrainInitializationError(SimulationError):
    """Exception raised for brain initialization errors."""
    pass

def run_simulation(config_file: Optional[str] = None,
                  num_steps: int = 1000) -> Dict[str, Any]:
    """
    Run a complete simulation with comprehensive error handling.
    
    Args:
        config_file: Optional configuration file path
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary containing simulation statistics
        
    Raises:
        ConfigurationError: If configuration is invalid
        BrainInitializationError: If brain fails to initialize
        SimulationError: For other simulation errors
    """
    try:
        # Create configuration
        if config_file:
            config = pynlm.create_default_config()
            config.load_from_file(config_file)
        else:
            config = pynlm.create_default_config()
        
        # Validate configuration
        validate_config(config)
        
        # Create and initialize brain
        brain = pynlm.create_brain(config)
        
        try:
            brain.initialize()
        except Exception as e:
            raise BrainInitializationError(f"Failed to initialize brain: {e}")
        
        # Run simulation
        for step in range(num_steps):
            try:
                brain.step(step)
            except Exception as e:
                # Log and continue (optional: break on error)
                pynlm.logger.warning(f"Step {step} failed: {e}")
                continue
        
        return {
            'status': 'success',
            'steps_completed': num_steps,
            'final_firing_rate': brain.get_average_firing_rate(),
            'total_spikes': brain.get_total_spike_count(),
        }
        
    except ConfigurationError:
        raise
    except BrainInitializationError:
        raise
    except Exception as e:
        raise SimulationError(f"Unexpected error in simulation: {e}")
```

### Code Analysis and Tools

#### Static Analysis

Use these tools for code analysis:

```bash
# Clang-Tidy for C++
clang-tidy src/brain/Brain.cpp -- -Iinclude

# CPPCheck for C++
cppcheck --enable=all --std=c++20 src/brain/Brain.cpp

# Python linting
flake8 python/
mypy python/
```

#### Coverage Analysis

```bash
# C++ coverage
cd build
gcov -b -c -o nlm_core.o src/brain/Brain.cpp

# Python coverage
coverage run -m pytest tests/
coverage report
coverage html
```

## Implementation Checklist

Use this checklist when implementing new features:

### Requirements Analysis
- [ ] Understand user needs
- [ ] Define success criteria
- [ ] Identify edge cases
- [ ] Consider performance implications

### Design
- [ ] Design API
- [ ] Create documentation
- [ ] Plan error handling
- [ ] Consider threading/safety

### Implementation
- [ ] Follow code style guide
- [ ] Write unit tests
- [ ] Document code
- [ ] Add error handling
- [ ] Update documentation

### Testing
- [ ] All tests pass
- [ ] Edge cases covered
- [ ] Performance tested
- [ ] Documentation tested

### Review
- [ ] Code reviewed
- [ ] Documentation reviewed
- [ ] Tests reviewed
- [ ] Performance reviewed

## Continuous Improvement

### Code Review Process

1. **Peer Review**: Get feedback from other developers
2. **Style Check**: Run code analysis tools
3. **Test Review**: Ensure tests are comprehensive
4. **Documentation Review**: Verify documentation is accurate

### Performance Optimization

1. **Profile**: Identify bottlenecks
2. **Optimize**: Focus on critical paths
3. **Test**: Verify optimizations don't break functionality
4. **Document**: Document performance characteristics

### Quality Assurance

1. **Code Quality**: Use automated linting and analysis
2. **Testing**: Maintain high test coverage
3. **Documentation**: Keep documentation up to date
4. **Compatibility**: Test with different platforms and compilers

## Acknowledgments

Following the work of:

- **Google C++ Style Guide**
- **PEP 8 (Python Style Guide)**
- **The C++ Core Guidelines**
- **Effective Modern C++**
- **Python Documentation Examples**

Special thanks to the open-source community for contributing to these standards.
