# NLM Coding Guidelines

## Overview

This document provides coding standards and best practices for developing the NLM (Neural Learning Machine) system. It covers code style, architecture patterns, testing, debugging, and project structure.

## Code Style

### C++ Standards

#### File Structure
Each source file should follow this structure:
```cpp
// Includes
#pragma once
#include "header.hpp"
#include <vector>
#include <memory>

// Using declarations (if needed)
using namespace nlm;

// Namespace (if file has its own)
namespace component {

// Forward declarations (if needed)

// Implementation

} // namespace component
```

#### Naming Conventions

**Classes:**
- **Class names:** CamelCase with descriptive names
- **Method names:** camelCase with descriptive names
- **Variable names:** camelCase with descriptive names
- **Constant names:** UPPER_SNAKE_CASE

**Examples:**
```cpp
// Good
class NeuralWorkingMemory {
public:
    void initialize(Brain* brain);
    void storeToNeuron(NeuronId neuron_id, float activation);
    float getNeuronActivation(NeuronId neuron_id) const;
    
private:
    static constexpr float DEFAULT_DECAY_RATE = 0.99f;
    std::vector<MemoryItem> memory_buffer_;
};

// Bad
class nwm { void init(Brain* b) {} void addNeuron(nid id, float act) {} };
```

#### Comments

**Documentation comments:**
- Use Doxygen-style comments for public methods
- Include parameter descriptions and return value explanations
- Document exceptions that can be thrown
- Provide usage examples where helpful

**Inline comments:**
- Explain complex algorithms or non-obvious code
- Document temporary workarounds or TODOs
- Explain performance optimizations

**Forbidden:**
- Comments that describe what code does rather than why
- Excessive inline comments for simple operations
- Comments that should be self-documenting code

#### Code Organization

**Header files (.hpp):**
```cpp
#pragma once

// Core types and forward declarations
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"

// Standard library includes
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Class definitions with public/private sections
class SomeComponent {
public:
    // Constructor(s)
    explicit SomeComponent(Config& config);
    
    // Public methods
    void initialize();
    void process();
    
    // Getters and setters
    float getValue() const;
    void setValue(float value);
    
    // Status and statistics
    bool isInitialized() const;
    std::string getStatus() const;
    
protected:
    // Protected members (for inheritance)
    
private:
    // Private implementation details
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
```

**Source files (.cpp):**
```cpp
#include "Component.hpp"

namespace nlm {

// Component implementation

} // namespace nlm
```

#### Error Handling

**Exception handling patterns:**
```cpp
// Exception hierarchy in nlm namespace
enum class ErrorCode {
    Success = 0,
    ConfigError = 1,
    SimulationError = 2,
    MemoryError = 3,
    FileError = 4,
    ValidationError = 5
};

class NLMException : public std::exception {
public:
    NLMException(ErrorCode code, const std::string& message)
        : code_(code), message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    ErrorCode getCode() const noexcept {
        return code_;
    }
    
private:
    ErrorCode code_;
    std::string message_;
};

// Usage examples
void riskyOperation() {
    try {
        // Code that can throw exceptions
        performRiskyAction();
    } catch (const ConfigError& e) {
        // Handle configuration errors
        std::cerr << "Configuration error: " << e.what() << std::endl;
        throw; // Re-throw after logging
    } catch (const SimulationError& e) {
        // Handle simulation errors
        std::cerr << "Simulation error: " << e.what() << std::endl;
        // Recover if possible
        recoverFromSimulationError();
    } catch (const MemoryError& e) {
        // Handle memory errors
        std::cerr << "Memory error: " << e.what() << std::endl;
        // Try to free resources
        cleanupMemory();
        throw; // Re-throw after cleanup
    } catch (const std::exception& e) {
        // Handle other standard exceptions
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        throw SimulationError("Unexpected error in risky operation");
    }
}
```

### Python Standards (for research examples)

#### File Structure
```python
"""Module docstring explaining purpose and usage."""

import numpy as np
import matplotlib.pyplot as plt
from typing import List, Dict, Any, Optional

class ResearchComponent:
    """Class docstring with description."""
    
    def __init__(self, parameter: float, setting: str = "default"):
        """Initialize component with parameters.
        
        Args:
            parameter: Some parameter value
            setting: Configuration setting
        """
        self.parameter = parameter
        self.setting = setting
        self._internal_state = None
    
    def process(self, data: np.ndarray) -> np.ndarray:
        """Process input data.
        
        Args:
            data: Input data array
            
        Returns:
            Processed data array
        """
        # Implementation
        return processed_data
    
    @property
    def state(self) -> Dict[str, Any]:
        """Get current state."""
        return {
            "parameter": self.parameter,
            "setting": self.setting,
            "internal": self._internal_state
        }
```

#### Code Style Rules

**Naming:**
- Function names: snake_case
- Variable names: snake_case
- Class names: CamelCase
- Constants: UPPER_SNAKE_CASE

**Imports:**
- Standard library first
- Third-party libraries
- Local imports last
- Group related imports
- Use absolute imports

**Documentation:**
- Docstrings with Args, Returns, Raises sections
- Type hints for all parameters and return values
- Module-level documentation

## Architecture Patterns

### Component Design

**Interface-based design:**
```cpp
// Abstract base class
class NeuralComponent {
public:
    virtual ~NeuralComponent() = default;
    
    virtual void initialize() = 0;
    virtual void update(float timestep) = 0;
    virtual bool isReady() const = 0;
};

// Concrete implementation
class WorkingMemory : public NeuralComponent {
public:
    void initialize() override {
        // Initialize working memory
    }
    
    void update(float timestep) override {
        // Update memory contents
    }
    
    bool isReady() const override {
        return initialized_ && capacity_ > 0;
    }
    
private:
    bool initialized_ = false;
    size_t capacity_ = 0;
};
```

### Dependency Injection

**Dependency injection pattern:**
```cpp
// Service interface
class BrainService {
public:
    virtual ~BrainService() = default;
    virtual void process(const std::vector<float>& input) = 0;
};

// Concrete service
class NeuralBrainService : public BrainService {
public:
    explicit NeuralBrainService(std::shared_ptr<NeuralNetwork> network)
        : network_(std::move(network)) {}
    
    void process(const std::vector<float>& input) override {
        // Process using neural network
    }
    
private:
    std::shared_ptr<NeuralNetwork> network_;
};

// Client using dependency injection
class Client {
public:
    explicit Client(std::unique_ptr<BrainService> service)
        : service_(std::move(service)) {}
    
    void execute(const std::vector<float>& input) {
        service_->process(input);
    }
    
private:
    std::unique_ptr<BrainService> service_;
};
```

### Factory Pattern

**Component factory:**
```cpp
// Abstract factory
class BrainComponentFactory {
public:
    virtual ~BrainComponentFactory() = default;
    virtual std::unique_ptr<NeuralComponent> createComponent() = 0;
};

// Concrete factory
class WorkingMemoryFactory : public BrainComponentFactory {
public:
    explicit WorkingMemoryFactory(size_t capacity) : capacity_(capacity) {}
    
    std::unique_ptr<NeuralComponent> createComponent() override {
        return std::make_unique<WorkingMemory>(capacity_);
    }
    
private:
    size_t capacity_;
};

// Factory manager
class ComponentFactory {
public:
    template<typename T, typename... Args>
    void registerFactory(Args&&... args) {
        factories_.push_back(
            std::make_unique<T>(std::forward<Args>(args)...)
        );
    }
    
    std::unique_ptr<NeuralComponent> createComponent(const std::string& type) {
        for (auto& factory : factories_) {
            // Check if factory can create this type
            if (factory->canCreate(type)) {
                return factory->createComponent();
            }
        }
        throw std::runtime_error("No factory registered for type: " + type);
    }
    
private:
    std::vector<std::unique_ptr<BrainComponentFactory>> factories_;
};
```

## Testing

### Testing Framework

**Google Test integration:**
```cpp
// Example test file
test_BrainBasics.cpp

#include <gtest/gtest.h>
#include "Brain.hpp"
#include "Config.hpp"

class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<nlm::Config>();
        config_->set("neuron_count", 100);
        config_->set("region_count", 1);
        
        brain_ = std::make_unique<nlm::Brain>(config_);
        ASSERT_TRUE(brain_->initialize());
    }
    
    void TearDown() override {
        brain_.reset();
        config_.reset();
    }
    
    std::shared_ptr<nlm::Config> config_;
    std::unique_ptr<nlm::Brain> brain_;
};

TEST_F(BrainTest, Initialization) {
    EXPECT_TRUE(brain_->isInitialized());
}

TEST_F(BrainTest, NeuronCount) {
    EXPECT_EQ(brain_->getTotalNeuronCount(), 100);
}

TEST_F(BrainTest, Step) {
    brain_->step(0, 0.0f);
    // Verify step completed
}
```

### Test Categories

**Unit tests:**
- Component-specific tests (neurons, synapses, memory)
- Configuration validation tests
- Error handling tests

**Integration tests:**
- Brain system integration
- Memory system integration
- Neuromodulation integration

**Performance tests:**
- Throughput tests
- Memory usage tests
- Scalability tests

### Testing Best Practices

**Test design:**
```cpp
// Good test example
TEST(NeuralPopulationTest, PopulationInitialization) {
    // Arrange
    NeuralPopulation population(PopulationId(1), 100);
    
    // Act & Assert
    EXPECT_EQ(population.getSize(), 100);
    EXPECT_FALSE(population.isEmpty());
    EXPECT_EQ(population.getNeuronType(), NeuronType::Internal);
}

// Bad test example (too coupled)
TEST(ComplexIntegrationTest, FullBrainFunctionality) {
    // Create entire brain with all components
    // This test is slow and hard to maintain
    // Instead, test individual components
}
```

**Mocking and isolation:**
```cpp
// Mock for testing
class MockBrain : public Brain {
public:
    MOCK_METHOD(bool, initialize, (), (override));
    MOCK_METHOD(void, step, (SimulationStep, Timestamp), (override));
    MOCK_METHOD(size_t, getTotalNeuronCount, (), (const, override));
    // ... other methods
};

// Test using mock
TEST(MockTest, MockBrainFunctionality) {
    MockBrain mock_brain;
    EXPECT_CALL(mock_brain, initialize()).WillOnce(Return(true));
    
    mock_brain.initialize();
    EXPECT_TRUE(mock_brain.isInitialized());
}
```

## Debugging

### Debugging Techniques

**Logging levels:**
```cpp
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Usage examples
NLM_LOG_TRACE("Entering function with parameters: " << param1 << ", " << param2);
NLM_LOG_DEBUG("Processing neuron " << neuron_id);
NLM_LOG_INFO("Step completed successfully");
NLM_LOG_WARNING("Large spike count detected: " << count);
NLM_LOG_ERROR("Failed to allocate memory for " << size << " bytes");
NLM_LOG_CRITICAL("Simulation terminated due to critical error");
```

**Memory debugging:**
```cpp
// Memory leak detection
void checkForMemoryLeaks() {
    MemoryDebugger::checkLeaks();
}

// Memory usage monitoring
void monitorMemoryUsage() {
    size_t total_memory = MemoryMonitor::getTotalMemoryUsage();
    size_t peak_memory = MemoryMonitor::getPeakMemoryUsage();
    
    std::cout << "Memory usage: " << total_memory / 1024 / 1024 
              << " MB (peak: " << peak_memory / 1024 / 1024 
              << " MB)" << std::endl;
}
```

**Performance debugging:**
```cpp
// Profile function execution time
class Profiler {
public:
    static void startProfile(const std::string& name) {
        profile_stack_.push_back({name, std::chrono::high_resolution_clock::now()});
    }
    
    static void endProfile(const std::string& name) {
        if (!profile_stack_.empty() && profile_stack_.back().name == name) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - profile_stack_.back().start_time);
            
            std::cout << name << ": " << duration.count() << " μs" << std::endl;
            
            profile_stack_.pop_back();
        }
    }
    
private:
    struct ProfileEntry {
        std::string name;
        std::chrono::high_resolution_clock::time_point start_time;
    };
    
    static std::vector<ProfileEntry> profile_stack_;
};

// Usage
void someFunction() {
    Profiler::startProfile("someFunction");
    
    // Function implementation
    
    Profiler::endProfile("someFunction");
}
```

## Project Structure

### Directory Structure

**Recommended layout:**
```
NLM/
├── CMakeLists.txt                    # Build system
├── README.md                         # Project documentation
├── docs/                            # Documentation
│   ├── API_REFERENCE.md              # API documentation
│   ├── CONFIG_REFERENCE.md           # Configuration documentation
│   ├── TUTORIALS/                   # Tutorial documentation
│   │   ├── README.md                # Tutorials index
│   │   ├── PHASE2/                  # Phase 2 tutorials
│   │   │   └── README.md
│   │   ├── PHASE3/                  # Phase 3 tutorials
│   │   │   └── README.md
│   │   ├── PHASE4/                  # Phase 4 tutorials
│   │   │   └── README.md
│   │   └── PHASE6/                  # Phase 6 tutorials
│   │       └── README.md
│   ├── EXAMPLES/                    # Code examples
│   │   ├── beginner/                # Beginner examples
│   │   ├── intermediate/            # Intermediate examples
│   │   ├── advanced/                # Advanced examples
│   │   └── research/                # Research examples
│   ├── CHEAT_SHEET.md               # Quick reference guide
│   ├── FAQ.md                       # Frequently asked questions
│   ├── IMPLEMENTATION_NOTES.md       # Technical implementation details
│   └── CODING_GUIDELINES.md         # Coding standards
├── src/                             # Source code
│   ├── core/                        # Core utilities
│   │   ├── Config/                   # Configuration management
│   │   ├── Random/                   # Random number generation
│   │   ├── Logger/                   # Logging system
│   │   └── SimulationClock/          # Time management
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Development system
│   ├── neuromodulation/              # Neuromodulators
│   ├── memory/                      # Memory systems
│   ├── prediction/                  # Prediction systems
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system
│   ├── environment/                 # Environment interface
│   └── visualization/              # Visualization
├── tests/                           # Test suite
│   ├── unit/                        # Unit tests
│   ├── integration/                 # Integration tests
│   └── performance/                  # Performance tests
├── examples/                        # Example code
│   ├── CMakeLists.txt              # Examples build system
│   └── src/                        # Example source
├── benchmarks/                      # Performance benchmarks
├── docs/                            # Generated documentation
└── build/                           # Build directory
```

### Build System

**CMake configuration:**
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(ENABLE_PROFILING "Enable profiling" OFF)

# Include directories
include_directories(src)
include_directories(include)

# Add source directories
add_subdirectory(src)

# Add tests if enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Add examples if enabled
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
```

### Documentation Generation

**Doxygen configuration:**
```ini
# Doxyfile configuration
PROJECT_NAME           = "NLM Documentation"
PROJECT_NUMBER         = "Phase 6"
INPUT                  = docs/API_REFERENCE.md docs/CONFIG_REFERENCE.md
OUTPUT_DIRECTORY       = docs/generated
GENERATE_LATEX         = NO
GENERATE_HTML          = YES
GENERATE_XML           = YES
XML_OUTPUT             = "xml"
```

## Version Control

### Git Conventions

**Branch structure:**
```
main                           # Main development branch
feature/phase6-integration      # Phase 6 development
feature/memory-optimization     # Memory optimization work
feature/plasticity-enhancement   # Plasticity improvements
bugfix/initialization-failure    # Bug fixes

release/v1.0.0                  # Release branches
```

**Commit messages:**
```commit
feat(component): Add neural plasticity support

- Implement STDP learning rules
- Add Hebbian learning mechanisms
- Integrate plasticity into brain loop

Co-authored-by: Kilo <kilo@example.com>

commit fix(brain): Resolve initialization race condition

- Fix thread safety issue in brain initialization
- Add proper synchronization for component setup

Co-authored-by: Kilo <kilo@example.com>
```

**Pull request conventions:**
- Use descriptive PR titles
- Include co-authored-by attribution
- Reference related issues
- Include tests for new functionality
- Update documentation as needed

## Collaboration

### Code Review

**Review checklist:**
```
□ Code follows coding guidelines
□ Tests are added and passing
□ Documentation is updated
□ Performance is acceptable
□ Memory usage is reasonable
□ Error handling is comprehensive
□ Dependencies are properly managed
□ Style conventions are followed
```

**Review process:**
1. Initial review by author
2. Peer review by team members
3. Technical review by domain experts
4. Final approval and merge

### Continuous Integration

**CI configuration:**
```yaml
# .github/workflows/ci.yml
name: NLM CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Run tests
      run: cd build && ctest --output-on-failure
  
  lint:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Run linter
      run: make lint
```

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Tutorial Documentation](TUTORIALS/README.md)
- [Examples](EXAMPLES.md)
- [Cheat Sheet](CHEAT_SHEET.md)
- [FAQ](FAQ.md)
- [Implementation Notes](IMPLEMENTATION_NOTES.md)

## Version Information

**Current version:** Phase 6.0 (Final Integration)
**Development status:** Active
**Documentation status:** Complete
**Testing status:** Comprehensive

**Next steps:**
1. Complete Phase 6 integration testing
2. Publish final documentation
3. Release stable version
4. Establish continuous integration
5. Create training materials
