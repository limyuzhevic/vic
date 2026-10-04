# NLM (熙然) - Development Guide

## Overview

This document provides comprehensive guidance for developers working with the NLM (Neural Learning Machine) project. It covers:

1. **Quick Start**: Getting started with development
2. **Adding New Components**: How to extend the system
3. **Integration Patterns**: Best practices for component integration
4. **Testing Guidelines**: How to test new functionality
5. **Performance Considerations**: Optimizing your additions
6. **Debugging Techniques**: Troubleshooting development issues

## Quick Start

### 1. Build Environment Setup

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (choose your build type)
cmake .. -DCMAKE_BUILD_TYPE=Release  # Optimized build
cmake .. -DCMAKE_BUILD_TYPE=Debug    # Development build with debugging

# Build all targets
make -j$(nproc)

# Run existing tests
ctest --output-on-failure
```

### 2. Running Examples

```bash
# Run different demonstration phases
./nlm              # Phase 2: Real Neural Computation (main demo)
./nlm_phase3_demo   # Phase 3: World Interaction Demo
./nlm_phase4_demo   # Phase 4: Cognition Demo
./nlm_phase6_demo   # Phase 6: Integration Test (comprehensive)
```

### 3. Creating a New Component

Follow these steps to add a new system component to NLM:

#### Step 1: Create the Component Directory

```bash
# Create component directory
mkdir -p src/mycomponent

# Create header and implementation files
mkdir -p src/mycomponent

# Add to CMakeLists.txt
# (see Section 3 for specific instructions)
```

#### Step 2: Design the Component Interface

Create the public interface in the header file:

```cpp
// src/mycomponent/MyComponent.hpp
#pragma once

#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

class MyComponent {
public:
    virtual ~MyComponent() = default;
    
    // Core lifecycle methods
    virtual bool initialize() = 0;
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    
    // Configuration methods
    virtual void configure(const Config& config) = 0;
    virtual bool isReady() const = 0;
    
    // Integration hooks
    virtual void onSensoryInput(const SensoryInput& input) = 0;
    virtual void onActionOutput(Action& action) = 0;
    
    // Statistics and monitoring
    virtual float getPerformanceMetric() const = 0;
    virtual std::string getStatus() const = 0;
};
```

#### Step 3: Implement the Component

Create the implementation file:

```cpp
// src/mycomponent/MyComponent.cpp
#include "MyComponent.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class MyComponent::Impl {
    // Implementation details
};

bool MyComponent::initialize() {
    // Component initialization logic
    NLM_LOG_INFO("MyComponent initialized");
    return true;
}

void MyComponent::update(double timestep) {
    // Update logic
}

// Implement other methods...

} // namespace nlm
```

#### Step 4: Integrate into Brain Class

Add the component to the Brain class integration:

```cpp
// src/brain/Brain.hpp - Add component access methods

// Getter methods
MyComponent* getMyComponent();
const MyComponent* getMyComponent() const;

// Integration hooks
void integrateMyComponent(std::unique_ptr<MyComponent> component);
```

```cpp
// src/brain/Brain.cpp - Implementation

MyComponent* Brain::getMyComponent() {
    return pImpl->myComponent.get();
}

void Brain::integrateMyComponent(std::unique_ptr<MyComponent> component) {
    pImpl->myComponent = std::move(component);
    // Register with integration systems
}
```

#### Step 5: Add to CMake Configuration

Update the main CMakeLists.txt to include your component:

```cmake
# Add component to core library
add_library(nlm_core STATIC
    # ... existing files ...
    src/mycomponent/MyComponent.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/mycomponent>
)

# Link with Brain if needed
target_link_libraries(nlm_core PUBLIC nlm_brain)

# Add demo executable for testing
add_executable(test_mycomponent
    examples/test_mycomponent.cpp
)
target_link_libraries(test_mycomponent PUBLIC nlm_core)
```

## Integration Patterns

### Pattern 1: Memory System Integration

```cpp
// For memory components, follow this integration pattern

class MyMemoryComponent : public NeuralMemorySystem {
public:
    bool initialize() override {
        // Initialize memory capacity
        setMemoryCapacity(10000);
        setRecallThreshold(0.7f);
        return true;
    }
    
    void update(double timestep) override {
        // Update memory consolidation
        consolidateMemories(timestep);
    }
    
    void onSensoryInput(const SensoryInput& input) override {
        // Store sensory patterns
        storePattern(input.getSensoryData());
    }
    
    void onActionOutput(Action& action) override {
        // Use memory to guide actions
        auto memoryContext = retrieveRelevantMemories(action.getContext());
        action.applyMemoryInfluence(memoryContext);
    }
};
```

### Pattern 2: Neuromodulation Integration

```cpp
// For neuromodulation components, follow this pattern

class MyNeuromodulator : public Neuromodulator {
public:
    bool initialize() override {
        // Set neuromodulation parameters
        setModulationStrength(1.0f);
        setDecayRate(0.1f);
        return true;
    }
    
    void update(double timestep) override {
        // Update modulation signals
        updateModulationSignals(timestep);
    }
    
    void onSensoryInput(const SensoryInput& input) override {
        // Process novelty or prediction error
        float predictionError = computePredictionError(input);
        modulateLearning(predictionError, timestep);
    }
};
```

## Testing Guidelines

### Unit Testing

Create comprehensive unit tests for new components:

```cpp
// test_mycomponent.cpp - Example unit test

#include <gtest/gtest.h>
#include "src/mycomponent/MyComponent.hpp"

class MyComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        component = std::make_unique<MyComponent>();
        component->initialize();
    }
    
    std::unique_ptr<MyComponent> component;
};

TEST_F(MyComponentTest, InitializationTest) {
    EXPECT_TRUE(component->isReady());
}

TEST_F(MyComponentTest, UpdateTest) {
    component->update(0.1f);
    EXPECT_EQ(getPerformanceMetric(), expectedValue);
}

TEST_F(MyComponentTest, IntegrationTest) {
    SensoryInput input;
    component->onSensoryInput(input);
    // Verify component responded correctly
}
```

### Integration Testing

Test component integration with Brain:

```cpp
// integration_test.cpp

#include "src/brain/Brain.hpp"
#include "src/mycomponent/MyComponent.hpp"

bool testComponentIntegration() {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        return false;
    }
    
    // Create and integrate component
    auto myComponent = std::make_unique<MyComponent>();
    brain->integrateMyComponent(std::move(myComponent));
    
    // Test integration
    SensoryInput input;
    Action action;
    
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
        brain->receiveSensoryInput(input);
        brain->produceAction(action);
        
        // Verify component affected behavior
        if (!verifyComponentEffectiveness()) {
            return false;
        }
    }
    
    return true;
}
```

## Performance Considerations

### Optimization Tips

1. **Memory Layout**: Keep related data structures close together
2. **Cache Efficiency**: Use contiguous containers where possible
3. **Vectorization**: Consider SIMD optimizations for numerical operations
4. **Parallel Processing**: Use multithreading for independent computations

### Profiling and Benchmarking

```cpp
// Example profiling code

#include <chrono>
#include <iostream>

class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& name) : name_(name) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceProfiler() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        std::cout << name_ << ": " << duration.count() << " microseconds" << std::endl;
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Usage in component update method
void MyComponent::update(double timestep) {
    PerformanceProfiler profiler("MyComponent::update");
    
    // Component update logic
    // ...
}
```

## Debugging Techniques

### Debug Build Configuration

Use debug builds for development:

```bash
# Configure with debugging
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_DEBUG=ON -DNLM_PROFILE=ON

# Build with additional debugging features
make -j$(nproc)
```

### Common Debugging Patterns

1. **Logging**: Use the NLM logging system extensively
2. **Assertions**: Add sanity checks throughout the code
3. **Visualization**: Use the visualization interface for debugging
4. **Checkpoints**: Use checkpoint system for state debugging

### Example Debugging Code

```cpp
void MyComponent::update(double timestep) {
    // Add validation
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid timestep: " + std::to_string(timestep));
        return;
    }
    
    // Log component state
    NLM_LOG_DEBUG("MyComponent state: " + getStatus());
    
    // Add assertions for critical conditions
    ASSERT(currentPopulation_ > 0, "Population cannot be zero");
    
    // Debug output for specific conditions
    if (shouldLogDebug()) {
        NLM_LOG_DEBUG("Debug info: " + debugInfoString());
    }
    
    // Component update logic
    // ...
}
```

## Advanced Extensions

### Adding Custom Learning Rules

Extend the plasticity system with custom learning rules:

```cpp
class CustomLearningRule : public PlasticityRule {
public:
    bool initialize() override {
        // Configure custom learning parameters
        setLearningRate(0.001f);
        setEligibilityTraceDecay(0.9f);
        return true;
    }
    
    void applyPlasticity(Neuron& pre, Neuron& post, 
                        double timestep, double reward) override {
        // Implement custom plasticity rule
        float weightChange = computeCustomWeightChange(pre, post, reward);
        post.adjustWeight(weightChange);
    }
};
```

### Extending World Simulation

Create custom world environments:

```cpp
class CustomWorld : public Environment {
public:
    bool initialize() override {
        // Configure world parameters
        setWidth(100.0f);
        setHeight(100.0f);
        setTimeStep(0.01f);
        return true;
    }
    
    void update(double timestep) override {
        // Update world dynamics
        updateEnvironmentalFactors(timestep);
    }
    
    SensoryPercept getSensoryPercept() const override {
        // Generate custom sensory perception
        return createCustomPercept();
    }
};
```

### Performance Monitoring

Add performance monitoring to components:

```cpp
class PerformanceMonitor {
public:
    void startMeasurement(const std::string& operation) {
        measurements_[operation].start_time = std::chrono::high_resolution_clock::now();
    }
    
    void endMeasurement(const std::string& operation) {
        auto& measurement = measurements_[operation];
        auto end_time = std::chrono::high_resolution_clock::now();
        measurement.duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - measurement.start_time);
        
        // Log performance data
        NLM_LOG_PERF(operation + ": " + std::to_string(measurement.duration.count()) + " microseconds");
    }
    
    void reportPerformance() const {
        // Generate performance report
        // ...
    }
    
private:
    struct Measurement {
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::microseconds duration;
    };
    
    std::unordered_map<std::string, Measurement> measurements_;
};
```

## Best Practices

### Code Quality

1. **Follow existing patterns**: Match the coding style and conventions of the project
2. **Use consistent naming**: Follow NLM naming conventions
3. **Document thoroughly**: Add Doxygen-style comments for all public methods
4. **Error handling**: Implement robust error handling with clear error messages

### Memory Management

1. **Use smart pointers**: Prefer `std::unique_ptr` and `std::shared_ptr`
2. **Avoid memory leaks**: Ensure proper cleanup in destructors
3. **Use move semantics**: When appropriate, use move constructors and assignment operators
4. **Resource management**: Use RAII for resource management

### Integration Quality

1. **Check integration points**: Ensure all integration hooks are properly implemented
2. **Test thoroughly**: Test integration at all levels (unit, integration, system)
3. **Performance critical**: Profile and optimize performance-critical paths
4. **Thread safety**: Ensure thread-safe operation if using multiple threads

## Next Steps

After mastering the basics, consider:

1. **Contributing**: Submit improvements to the main repository
2. **Research**: Explore new neural computation techniques
3. **Applications**: Apply NLM to specific domains (robotics, healthcare, etc.)
4. **Education**: Create teaching materials and tutorials
5. **Community**: Join the NLM community and share knowledge

## References

- [NLM Documentation](https://nlm.org/documentation)
- [Architecture Guide](docs/ARCHITECTURE.md)
- [Phase Documentation](docs/PHASE6_FINAL_RESULTS.md)
- [Research Papers](docs/SCIENCE.md)

This guide will help you successfully develop and integrate new components into the NLM system. Remember that the key to success is following established patterns, testing thoroughly, and maintaining code quality.