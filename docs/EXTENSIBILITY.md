# NLM Extensibility Guide

This document describes the extensibility architecture of NLM (熙然), focusing on the new abstractions and interfaces that make the system easier to extend and customize.

## Overview

NLM's extensibility is organized around five main patterns:

1. **Abstract Base Classes for Dynamics** - Neuron dynamics, plasticity rules, and sensory processing
2. **Plugin Architecture** - Factory interfaces for component creation and dependency injection
3. **Observer Pattern** - System state change and event handling
4. **Strategy Pattern** - Different learning algorithms and performance strategies
5. **Extensibility Points** - Custom neural models and neuromodulation

## 1. Abstract Base Classes for Dynamics

### NeuronDynamics Interface

**Purpose:** Abstract interface for neuron dynamics models

**Key Methods:**
- `virtual void initialize(const NeuronState& state) = 0`
- `virtual bool update(MembranePotential& membranePotential, FiringRate& firingRate, Timestamp currentTime, TimestepDuration dt) = 0`
- `virtual void onSpike(Timestamp currentTime) = 0`
- `virtual void reset() = 0`
- `virtual const std::string& getName() const = 0`

**Usage Example:**
```cpp
class MyCustomDynamics : public NeuronDynamics {
public:
    void initialize(const NeuronState& state) override {
        // Custom initialization
    }
    
    bool update(MembranePotential& membranePotential, 
                FiringRate& firingRate,
                Timestamp currentTime,
                TimestepDuration dt) override {
        // Custom dynamics implementation
        return false;
    }
    
    void onSpike(Timestamp currentTime) override {
        // Handle spike events
    }
    
    void reset() override {
        // Reset dynamics
    }
    
    const std::string& getName() const override {
        static const std::string name = "MyCustomDynamics";
        return name;
    }
};
```

### PlasticityRule Interface

**Purpose:** Abstract interface for plasticity rules

**Key Methods:**
- `virtual void initialize(const PlasticityFlags& flags) = 0`
- `virtual void updateWeights(float& weight, float preSpikeTime, float postSpikeTime, float eligibilityTrace) = 0`
- `virtual void setLearningRate(float rate) = 0`
- `virtual float getLearningRate() const = 0`
- `virtual const std::string& getName() const = 0`
- `virtual bool isEnabled() const = 0`

### SensoryProcessor Interface

**Purpose:** Abstract interface for sensory processing

**Key Methods:**
- `virtual MembranePotential processInput(const SensoryInput& input, Timestamp timestamp) = 0`
- `virtual void setGain(float gain) = 0`
- `virtual float getGain() const = 0`
- `virtual const std::string& getName() const = 0`
- `virtual bool isActive() const = 0`

## 2. Plugin Architecture

### ComponentFactory Interface

**Purpose:** Factory interface for creating components

**Key Methods:**
- `virtual std::unique_ptr<void> create(const std::vector<float>& parameters) = 0`
- `virtual const std::string& getComponentType() const = 0`
- `virtual bool canCreate(const std::string& type) const = 0`
- `virtual std::vector<float> getDefaultParameters() const = 0`

### DependencyInjector Interface

**Purpose:** Dependency injection container

**Key Methods:**
- `virtual void registerFactory(std::unique_ptr<ComponentFactory> factory) = 0`
- `virtual ComponentFactory* getFactory(const std::string& type) = 0`
- `virtual std::unique_ptr<void> createComponent(const std::string& type, const std::vector<float>& parameters = {}) = 0`
- `virtual std::vector<std::string> getRegisteredTypes() const = 0`
- `virtual void clear() = 0`

### PluginManager Interface

**Purpose:** Manage experiment plugins

**Key Methods:**
- `virtual void registerPluginFactory(std::unique_ptr<PluginFactory> factory) = 0`
- `virtual std::unique_ptr<ExperimentPlugin> createPlugin(const std::string& type, const std::vector<float>& parameters = {}) = 0`
- `virtual std::vector<std::string> getRegisteredPluginTypes() const = 0`
- `virtual std::vector<ExperimentPlugin*> getPlugins() const = 0`
- `virtual bool setPluginActive(const std::string& pluginName, bool active) = 0`
- `virtual bool removePlugin(const std::string& pluginName) = 0`
- `virtual void clear() = 0`

## 3. Observer Pattern

### Observer Interface

**Purpose:** Handle system state changes

**Key Methods:**
- `virtual void onStateChange(const std::string& eventType, const std::string& key, const ConfigValue& value) = 0`
- `virtual const std::string& getName() const = 0`

**Usage Example:**
```cpp
class MyObserver : public Observer {
public:
    void onStateChange(const std::string& eventType,
                      const std::string& key,
                      const ConfigValue& value) override {
        std::cout << "Event: " << eventType 
                  << ", Key: " << key 
                  << ", Value: " << value << std::endl;
    }
    
    const std::string& getName() const override {
        static const std::string name = "MyObserver";
        return name;
    }
};
```

### EventHandler Interface

**Purpose:** Handle events

**Key Methods:**
- `virtual void handleEvent(const std::string& eventType, const std::map<std::string, ConfigValue>& data) = 0`
- `virtual bool canHandle(const std::string& eventType) const = 0`
- `virtual const std::string& getName() const = 0`

## 4. Strategy Pattern

### Strategy Interface

**Purpose:** Implement different strategies (e.g., learning algorithms)

**Key Methods:**
- `virtual ConfigValue execute(const std::vector<ConfigValue>& inputs) = 0`
- `virtual const std::string& getName() const = 0`
- `virtual void setParameters(const std::map<std::string, ConfigValue>& params) = 0`
- `virtual std::map<std::string, ConfigValue> getParameters() const = 0`

### PerformanceStrategy Interfaces

**Purpose:** Strategy interfaces for performance optimization

#### MemoryPoolStrategy
- `virtual void* allocate(size_t size) = 0`
- `virtual void deallocate(void* ptr, size_t size) = 0`
- `virtual size_t getMemoryUsage() const = 0`
- `virtual size_t getAllocatedCount() const = 0`
- `virtual void reset() = 0`
- `virtual const std::string& getName() const = 0`
- `virtual bool isEnabled() const = 0`

#### EventQueueStrategy
- `virtual void enqueueSpike(NeuronId neuronId, Timestamp time) = 0`
- `virtual bool dequeueSpike(NeuronId& neuronId, Timestamp& time) = 0`
- `virtual bool hasEvents() const = 0`
- `virtual size_t getEventCount() const = 0`
- `virtual void clear() = 0`
- `virtual const std::string& getName() const = 0`

#### ConnectivityStrategy
- `virtual SynapseId addConnection(NeuronId source, NeuronId target, float weight = 1.0f, Delay delay = 1) = 0`
- `virtual bool removeConnection(SynapseId id) = 0`
- `virtual std::vector<SynapseId> getOutgoingConnections(NeuronId source) const = 0`
- `virtual std::vector<SynapseId> getIncomingConnections(NeuronId target) const = 0`
- `virtual float getConnectionWeight(SynapseId id) const = 0`
- `virtual void setConnectionWeight(SynapseId id, float weight) = 0`
- `virtual size_t getTotalConnectionCount() const = 0`
- `virtual const std::string& getName() const = 0`
- `virtual bool isEnabled() const = 0`

## 5. Extensibility Points

### NeuralModel Interface

**Purpose:** Custom neural models

**Key Methods:**
- `virtual std::vector<float> process(const std::vector<float>& input) = 0`
- `virtual const std::string& getName() const = 0`
- `virtual const std::string& getType() const = 0`
- `virtual void setParameters(const std::map<std::string, float>& params) = 0`
- `virtual std::map<std::string, float> getParameters() const = 0`

### Neuromodulator Interface

**Purpose:** Custom neuromodulation implementations

**Key Methods:**
- `virtual void apply(NeuronId neuronId, float signal) = 0`
- `virtual const std::string& getName() const = 0`
- `virtual const std::string& getType() const = 0`
- `virtual void setStrength(float strength) = 0`
- `virtual float getStrength() const = 0`
- `virtual bool isActive() const = 0`
- `virtual void activate() = 0`
- `virtual void deactivate() = 0`

## Implementation Examples

### Creating a Custom Dynamics Implementation

```cpp
// In your custom_dynamics.hpp
#pragma once
#include "../brain/Neuron.hpp"

class CustomLIFDynamics : public NeuronDynamics {
public:
    CustomLIFDynamics() = default;
    ~CustomLIFDynamics() override = default;
    
    void initialize(const NeuronState& state) override {
        // Custom initialization logic
    }
    
    bool update(MembranePotential& membranePotential, 
                FiringRate& firingRate,
                Timestamp currentTime,
                TimestepDuration dt) override {
        // Custom dynamics update
        // Return true if neuron fired
        return false;
    }
    
    void onSpike(Timestamp currentTime) override {
        // Handle spike events
    }
    
    void reset() override {
        // Reset dynamics
    }
    
    const std::string& getName() const override {
        static const std::string name = "CustomLIFDynamics";
        return name;
    }
};

// In your custom_dynamics.cpp
#include "custom_dynamics.hpp"

// Implementation...

// Register with Neuron
void registerCustomDynamics(DynamicsFactory& factory) {
    // Custom registration logic
}
```

### Creating a Custom Plugin

```cpp
// In your plugin.hpp
#pragma once
#include "../experiments/Experiment.hpp"

class DataVisualizationPlugin : public Experiment::ExperimentPlugin {
public:
    DataVisualizationPlugin() = default;
    ~DataVisualizationPlugin() override = default;
    
    void initialize(const Experiment& experiment) override {
        // Initialize with experiment reference
    }
    
    void process() override {
        // Process experiment data and create visualizations
    }
    
    const std::string& getName() const override {
        static const std::string name = "DataVisualization";
        return name;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    void setActive(bool active) override {
        active_ = active;
    }
    
private:
    bool active_ = true;
};

// In your plugin.cpp
#include "plugin.hpp"

// Implementation...

// Register with PluginManager
void registerVisualizationPlugin(Experiment::PluginFactory& factory) {
    // Plugin registration logic
}
```

### Creating a Custom Performance Strategy

```cpp
// In your performance_strategy.hpp
#pragma once
#include "../performance/Performance.hpp"

class OptimizedMemoryPoolStrategy : public MemoryPoolStrategy {
public:
    OptimizedMemoryPoolStrategy() = default;
    ~OptimizedMemoryPoolStrategy() override = default;
    
    void* allocate(size_t size) override {
        // Optimized allocation
        return std::malloc(size);
    }
    
    void deallocate(void* ptr, size_t size) override {
        // Optimized deallocation
        std::free(ptr);
    }
    
    size_t getMemoryUsage() const override {
        return memoryUsage_;
    }
    
    size_t getAllocatedCount() const override {
        return allocatedCount_;
    }
    
    void reset() override {
        // Reset pool
    }
    
    const std::string& getName() const override {
        static const std::string name = "OptimizedMemoryPool";
        return name;
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
private:
    size_t memoryUsage_ = 0;
    size_t allocatedCount_ = 0;
    bool enabled_ = true;
};

// In your performance_strategy.cpp
#include "performance_strategy.hpp"

// Implementation...

// Register with PerformanceStrategyFactory
void registerOptimizedStrategy(PerformanceStrategyFactory& factory) {
    // Strategy registration logic
}
```

## Integration with Existing Code

### Using Custom Dynamics

```cpp
// In your main application or configuration setup
void setupCustomDynamics(Brain& brain) {
    // Create custom dynamics
    auto customDynamics = std::make_unique<CustomLIFDynamics>();
    
    // Apply to all neurons or specific neurons
    for (auto& neuron : brain.getNeurons()) {
        neuron.setDynamics(std::move(customDynamics));
    }
}
```

### Using Custom Plugins

```cpp
// In your experiment setup
void setupExperimentPlugins(Experiment& experiment) {
    // Create plugin manager
    auto pluginManager = std::make_unique<SimplePluginManager>();
    experiment.setPluginManager(std::move(pluginManager));
    
    // Register plugin factory
    auto pluginFactory = std::make_unique<DefaultPluginFactory>();
    experiment.registerPluginFactory(std::move(pluginFactory));
    
    // Create and activate plugins
    auto dataPlugin = experiment.createPlugin("data_collection");
    experiment.setPluginActive("data_collection", true);
    
    auto analysisPlugin = experiment.createPlugin("analysis");
    experiment.setPluginActive("analysis", true);
    
    // Process plugins during experiment
    experiment.processPlugins();
}
```

### Using Custom Strategies

```cpp
// In your config setup
void setupCustomStrategies(Config& config) {
    // Create custom strategy
    auto customStrategy = std::make_unique<SimpleStrategy>(
        "CustomStrategy",
        [](const std::vector<ConfigValue>& inputs) {
            // Custom strategy logic
            return ConfigValue(42.0);
        }
    );
    
    // Apply to config
    config.setStrategy(std::move(customStrategy));
    
    // Execute strategy
    std::vector<ConfigValue> inputs = {1.0, 2.0, 3.0};
    ConfigValue result = config.executeStrategy(inputs);
}
```

## Best Practices

### 1. Interface Design

- Keep interfaces simple and focused
- Use pure virtual functions for abstract base classes
- Provide clear documentation for all methods
- Use const references where possible for efficiency

### 2. Memory Management

- Use `std::unique_ptr` for ownership transfer
- Avoid raw pointers in interfaces
- Consider move semantics for large objects
- Implement proper cleanup in destructors

### 3. Extension Points

- Define clear extension points in base classes
- Use factory patterns for component creation
- Provide default implementations where appropriate
- Allow runtime configuration of strategies

### 4. Testing

- Write unit tests for all new abstractions
- Test concrete implementations against interfaces
- Verify that existing code still works with new extensions
- Test edge cases and error conditions

### 5. Documentation

- Document all new interfaces
- Provide usage examples
- Include performance considerations
- Note any limitations or restrictions

## Benefits of the Extensibility Architecture

### 1. Easier Customization

- Custom dynamics can be implemented without modifying core code
- New plasticity rules can be added as plugins
- Sensory processors can be swapped at runtime

### 2. Better Testability

- Interfaces can be mocked for unit testing
- Different strategies can be tested independently
- Components can be tested in isolation

### 3. Improved Performance

- Performance strategies can be optimized for specific hardware
- Memory pools can be customized for different workloads
- Event queues can be tuned for specific use cases

### 4. Enhanced Maintainability

- Clear separation of concerns
- Easier to modify individual components
- Reduced risk of breaking existing functionality

### 5. Future-Proof Design

- New features can be added without breaking changes
- Multiple implementations can coexist
- Third-party extensions can be easily integrated

## Migration Guide

### From Custom Code to Extensible Architecture

1. **Identify Components**: Find all components that could benefit from abstraction
2. **Define Interfaces**: Create abstract base classes for each component
3. **Implement Concrete Classes**: Create concrete implementations
4. **Update Factory Registration**: Register new components with existing factories
5. **Test Integration**: Verify that new components work with existing code
6. **Documentation**: Document usage and configuration options

### Tips for Migration

- Start with one component and migrate gradually
- Use existing factories where possible
- Maintain backward compatibility
- Write comprehensive tests for new code
- Document the migration process

## Conclusion

The NLM extensibility architecture provides a robust framework for extending the neural simulation system while maintaining clean interfaces and good design principles. By using abstract base classes, factory patterns, observer pattern, strategy pattern, and well-defined extensibility points, developers can easily add new functionality to the system without breaking existing code.

This architecture makes NLM more flexible, testable, and maintainable, enabling both simple customizations and complex extensions. The combination of abstract interfaces and concrete implementations allows users to start with basic functionality and gradually add advanced features as needed.
