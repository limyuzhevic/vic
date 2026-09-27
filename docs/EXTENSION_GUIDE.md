# NLM Extension Development Guide

## Overview

The NLM (Neural Learning Machine) Extension Development Guide provides comprehensive instructions for creating custom extensions that can be loaded and integrated with the NLM brain simulator. This guide covers the complete extension development workflow from basic concepts to advanced plugin architecture.

## Extension Architecture

### Core Components

NLM extensions consist of several key components:

1. **Extension Points**: Hooks for custom integration and behavior modification
2. **Event System**: Event-driven architecture for brain behavior extension
3. **Component Factory**: Template-based component creation for custom brain components
4. **Plugin Architecture**: Dynamic loading and management of extensions
5. **Command System**: Advanced commands for extension control

### Extension Lifecycle

1. **Discovery**: Extensions are discovered automatically or manually
2. **Loading**: Extensions are loaded from specified paths
3. **Initialization**: Extensions receive global configuration and brain reference
4. **Registration**: Extensions register components, handlers, and listeners
5. **Execution**: Extensions participate in simulation via extension points and events
6. **Unloading**: Extensions are cleaned up and unloaded

## Getting Started with Extensions

### 1. Extension Structure

A basic extension follows this structure:

```
my_extension/
├── include/
│   └── my_extension/
│       ├── Extension.hpp          # Main extension interface
│       ├── MyNeuromodulator.hpp    # Custom neuromodulator
│       └── ExtensionPointHandler.hpp # Custom handler
├── src/
│   └── my_extension/
│       ├── Extension.cpp          # Extension implementation
│       ├── MyNeuromodulator.cpp    # Neuromodulator implementation
│       └── ExtensionPointHandler.cpp # Handler implementation
└── my_extension.nlmext              # Extension manifest file
```

### 2. Extension Manifest

The extension manifest file contains metadata about the extension:

```xml
<!-- my_extension.nlmext -->
<extension>
    <name>MyExtension</name>
    <version>1.0.0</version>
    <description>A custom extension for NLM</description>
    <author>Your Name</author>
    <dependencies>
        <extension>NLMCore</extension>
    </dependencies>
    <provides>
        <component>neuromodulator</component>
        <component>memory_system</component>
    </provides>
    <extension_points>
        <point name="brain_step" priority="50">
            <description>Custom brain step processing</description>
        </point>
        <point name="neuron_spike" priority="30">
            <description>Custom neuron spike handling</description>
        </point>
    </extension_points>
    <events>
        <event name="custom_event">
            <description>Custom extension event</description>
        </event>
    </events>
</extension>
```

### 3. Basic Extension Class

```cpp
// include/my_extension/Extension.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <nlm/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

namespace nlm {
namespace my_extension {

class Extension {
public:
    Extension() = default;
    virtual ~Extension() = default;
    
    // Initialize extension with global configuration and brain reference
    virtual bool initialize(std::shared_ptr<Config> globalConfig, std::shared_ptr<Brain> brain) {
        globalConfig_ = globalConfig;
        brain_ = brain;
        
        // Register components
        registerComponents();
        
        // Register extension point handlers
        registerExtensionPointHandlers();
        
        // Register event listeners
        registerEventListeners();
        
        return true;
    }
    
    // Shutdown extension
    virtual void shutdown() {
        // Cleanup resources
        globalConfig_.reset();
        brain_.reset();
    }
    
    // Get extension name
    virtual std::string getName() const { return "MyExtension"; }
    
    // Get extension version
    virtual std::string getVersion() const { return "1.0.0"; }
    
    // Get extension description
    virtual std::string getDescription() const { return "A custom extension for NLM"; }
    
protected:
    // Pure virtual methods for extension customization
    virtual void registerComponents() = 0;
    virtual void registerExtensionPointHandlers() = 0;
    virtual void registerEventListeners() = 0;
    
    // Helper methods
    std::shared_ptr<Config> getGlobalConfig() const { return globalConfig_; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Config> globalConfig_;
    std::shared_ptr<Brain> brain_;
};

}} // namespace nlm::my_extension
```

## Component Factory Usage

### Custom Neuromodulator Example

```cpp
// include/my_extension/MyNeuromodulator.hpp
#pragma once

#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <string>

namespace nlm {
namespace my_extension {

class MyNeuromodulator : public Neuromodulator {
public:
    MyNeuromodulator(const std::string& name, float level, float decayRate);
    
    // Neuromodulator interface implementation
    void update(float timestep) override;
    float getLevel() const override;
    float getPlasticityFactor() const override;
    
    // Custom neuromodulator functionality
    void setDecayRate(float decayRate) { decayRate_ = decayRate; }
    float getDecayRate() const { return decayRate_; }
    
private:
    std::string name_;
    float level_;
    float decayRate_;
    float currentLevel_;
};

}} // namespace nlm::my_extension
```

```cpp
// src/my_extension/MyNeuromodulator.cpp
#include "MyNeuromodulator.hpp"
#include <cmath>

namespace nlm {
namespace my_extension {

MyNeuromodulator::MyNeuromodulator(const std::string& name, float level, float decayRate)
    : name_(name), level_(level), decayRate_(decayRate), currentLevel_(level) {}

void MyNeuromodulator::update(float timestep) {
    // Custom decay model
    currentLevel_ = std::max(0.0f, currentLevel_ * std::exp(-decayRate_ * timestep));
}

float MyNeuromodulator::getLevel() const {
    return currentLevel_;
}

float MyNeuromodulator::getPlasticityFactor() const {
    // Custom plasticity factor calculation
    return 1.0f + currentLevel_ * 0.5f;
}

}} // namespace nlm::my_extension
```

### Registering Custom Components

```cpp
// src/my_extension/Extension.cpp (partial)
#include "Extension.hpp"
#include "MyNeuromodulator.hpp"
#include <nlm/brain/AdvancedBrainInterface.hpp>

namespace nlm {
namespace my_extension {

void Extension::registerComponents() {
    // Get advanced brain interface
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    // Register custom neuromodulator factory
    advancedInterface.registerComponentType<Neuromodulator>(
        "my_neuromodulator",
        [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
            // Create neuromodulator from configuration
            std::string name = config->getOr<std::string>("name", "default");
            float level = config->getOr<float>("level", 0.5f);
            float decayRate = config->getOr<float>("decay_rate", 0.01f);
            
            return std::make_shared<MyNeuromodulator>(name, level, decayRate);
        }
    );
}

}} // namespace nlm::my_extension
```

## Extension Point Handlers

### Creating Custom Extension Point Handlers

```cpp
// include/my_extension/ExtensionPointHandler.hpp
#pragma once

#include <string>
#include <any>
#include <unordered_map>

namespace nlm {
namespace my_extension {

// Extension point handler function type
using ExtensionPointHandler = std::function<bool(
    const std::string& pointName,
    const std::any& data,
    const std::unordered_map<std::string, std::any>& params
)>;

class CustomExtensionPointHandler {
public:
    bool handleBrainStep(const std::string& pointName, const std::any& data,
                        const std::unordered_map<std::string, std::any>& params);
    
    bool handleNeuronSpike(const std::string& pointName, const std::any& data,
                          const std::unordered_map<std::string, std::any>& params);
    
    bool handleSynapseFormed(const std::string& pointName, const std::any& data,
                            const std::unordered_map<std::string, std::any>& params);
};

}} // namespace nlm::my_extension
```

```cpp
// src/my_extension/ExtensionPointHandler.cpp
#include "ExtensionPointHandler.hpp"
#include <nlm/brain/AdvancedBrainInterface.hpp>
#include <nlohmann/json.hpp>

namespace nlm {
namespace my_extension {

bool CustomExtensionPointHandler::handleBrainStep(const std::string& pointName,
                                                  const std::any& data,
                                                  const std::unordered_map<std::string, std::any>& params) {
    // Custom brain step processing
    // data contains step information (step, time)
    // params may contain additional configuration
    
    try {
        // Parse data
        auto step = std::any_cast<size_t>(data);
        auto time = std::any_cast<float>(params.at("time"));
        
        // Custom processing logic
        // For example, modify brain behavior based on custom conditions
        if (shouldApplyCustomBehavior(step, time)) {
            applyCustomBrainBehavior();
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool CustomExtensionPointHandler::handleNeuronSpike(const std::string& pointName,
                                                     const std::any& data,
                                                     const std::unordered_map<std::string, std::any>& params) {
    // Custom neuron spike processing
    // data contains neuron_id, time
    
    try {
        auto neuronId = std::any_cast<NeuronId>(data);
        auto time = std::any_cast<float>(params.at("time"));
        
        // Custom spike handling logic
        if (isInterestingSpike(neuronId, time)) {
            handleInterestingSpike(neuronId, time);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

}} // namespace nlm::my_extension
```

### Registering Extension Point Handlers

```cpp
// In Extension.cpp
void Extension::registerExtensionPointHandlers() {
    // Get advanced brain interface
    auto brain = getBrain();
    if (!brain) return;
    
    auto& advancedInterface = *brain->getAdvancedInterface();
    
    // Create handler instance
    auto handler = std::make_unique<CustomExtensionPointHandler>();
    
    // Register extension point handlers with different priorities
    advancedInterface.registerExtensionPointHandler(
        "brain_step",
        [handler](const std::string& pointName, const std::any& data,
                 const std::unordered_map<std::string, std::any>& params) {
            return handler->handleBrainStep(pointName, data, params);
        },
        "brain_step_handler",
        50  // Higher priority
    );
    
    advancedInterface.registerExtensionPointHandler(
        "neuron_spike",
        [handler](const std::string& pointName, const std::any& data,
                 const std::unordered_map<std::string, std::any>& params) {
            return handler->handleNeuronSpike(pointName, data, params);
        },
        "neuron_spike_handler",
        30  // Medium priority
    );
}
```

## Event System

### Event Listeners

```cpp
// include/my_extension/EventListener.hpp
#pragma once

#include <nlm/brain/AdvancedBrainInterface.hpp>
#include <memory>

namespace nlm {
namespace my_extension {

class CustomEventListener : public AdvancedBrainInterface::ExtensionListener {
public:
    void onExtensionLoaded(const std::string& extensionName) override;
    void onExtensionUnloaded(const std::string& extensionName) override;
    void onExtensionError(const std::string& extensionName, const std::string& error) override;
    void onComponentCreated(const std::string& componentName, const std::string& type) override;
    void onComponentDestroyed(const std::string& componentName) override;
};

}} // namespace nlm::my_extension
```

```cpp
// src/my_extension/EventListener.cpp
#include "EventListener.hpp"
#include <nlm/brain/AdvancedBrainInterface.hpp>

namespace nlm {
namespace my_extension {

void CustomEventListener::onExtensionLoaded(const std::string& extensionName) {
    // Called when an extension is loaded
    // Can be used for cross-extension communication
    if (extensionName == "NLMCore") {
        // Core extension loaded, ready to use
        ready_ = true;
    }
}

void CustomEventListener::onExtensionUnloaded(const std::string& extensionName) {
    // Called when an extension is unloaded
    // Can be used for cleanup
    if (extensionName == "MyExtension") {
        shutdown_ = true;
    }
}

void CustomEventListener::onExtensionError(const std::string& extensionName, const std::string& error) {
    // Called when an extension encounters an error
    // Can be used for error recovery
    if (extensionName == "NLMCore") {
        // Core extension error - this is critical
        // Might want to shutdown or attempt recovery
    }
}

void CustomEventListener::onComponentCreated(const std::string& componentName, const std::string& type) {
    // Called when a component is created
    // Can be used for monitoring or cross-component communication
    if (type == "neuromodulator") {
        // A new neuromodulator was created
        onNeuromodulatorCreated(componentName);
    }
}

void CustomEventListener::onComponentDestroyed(const std::string& componentName) {
    // Called when a component is destroyed
    // Can be used for cleanup
    if (isOurComponent(componentName)) {
        cleanupComponent(componentName);
    }
}

}} // namespace nlm::my_extension
```

### Registering Event Listeners

```cpp
// In Extension.cpp
void Extension::registerEventListeners() {
    // Get advanced brain interface
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    // Create and register event listener
    auto eventListener = std::make_unique<CustomEventListener>();
    
    // The listener will be automatically managed by the advanced interface
    // It will receive events for all extensions and components
    if (!advancedInterface.addExtensionListener(std::move(eventListener))) {
        // Failed to add event listener
    }
}
```

## Advanced Event System

### Emitting Custom Events

```cpp
// Function to emit custom events from within the extension
bool emitCustomEvent(const std::string& eventName, const std::any& data) {
    // Get advanced brain interface
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    return advancedInterface.emitEvent(
        eventName,
        data,
        "MyExtension"  // This extension as source
    );
}
```

### Event Data Structures

```cpp
// Example event data structure
struct CustomEventData {
    std::string eventType;
    float timestamp;
    std::unordered_map<std::string, std::any> parameters;
    std::string sourceExtension;
};
```

### Event History and Debugging

```cpp
// Access event history (if enabled)
void logRecentEvents(size_t maxEvents = 100) {
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    // Event history access would be provided through the extension system
    // This is useful for debugging and analysis
}
```

## Component Factory Advanced Usage

### Template-Based Component Creation

```cpp
// Example of creating different component types using the factory
void createAllComponents() {
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    // Create neuromodulator
    auto neuromodulator = advancedInterface.createComponent<Neuromodulator>(
        "my_neuromodulator",
        {{ "name", "custom_dopamine" }, { "level", "0.8" }, { "decay_rate", "0.02" }}
    );
    
    // Create memory system
    auto memorySystem = advancedInterface.createComponent<MemorySystem>(
        "custom_working_memory",
        {{ "name", "my_working_memory" }, { "capacity", "1000" }, { "max_episodes", "100" }}
    );
    
    // Create cognitive function
    auto cognitiveFunction = advancedInterface.createComponent<CognitiveFunction>(
        "custom_planner",
        {{ "name", "my_planner" }, { "planning_depth", "5" }, { "inhibition_strength", "0.5" }}
    );
}
```

### Component Configuration

```cpp
// Component configuration with type-safe values
struct NeuromodulatorConfig {
    std::string name;
    float level;
    float decayRate;
    bool enabled;
    
    // Conversion to parameter map
    std::unordered_map<std::string, std::string> toParams() const {
        return {
            { "name", name },
            { "level", std::to_string(level) },
            { "decay_rate", std::to_string(decayRate) },
            { "enabled", enabled ? "true" : "false" }
        };
    }
};
```

## Command System Integration

### Advanced Command Registration

```cpp
// Register custom commands for the extension
void registerCustomCommands() {
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    // Example of registering a custom command
    // This would typically be done through the extension system's command registration
    // For simplicity, we'll show the concept
}
```

### Command Execution

```cpp
// Execute commands from within the extension
bool executeCommand(const std::string& commandName,
                   const std::vector<std::string>& args,
                   const std::unordered_map<std::string, std::any>& parameters) {
    auto brain = getBrain();
    if (!brain) return false;
    
    return brain->executeAdvancedCommand(commandName, args, parameters);
}
```

## Plugin Architecture

### Dynamic Extension Loading

```cpp
// Function to load extensions dynamically
bool loadAdditionalExtensions(const std::vector<std::string>& paths) {
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    bool success = true;
    for (const auto& path : paths) {
        if (!advancedInterface.loadExtensionsFromPath(path)) {
            // Log error
            success = false;
        }
    }
    
    return success;
}
```

### Extension State Management

```cpp
// Manage extension state
enum class ExtensionState {
    LOADING,
    INITIALIZING,
    READY,
    RUNNING,
    UNLOADING,
    UNLOADED,
    ERROR
};

ExtensionState getExtensionState(const std::string& extensionName) {
    auto& advancedInterface = AdvancedBrainInterface::getInstance();
    
    // Extension state queries would be available through the extension system
    // This is useful for managing extension lifecycle
}
```

## Advanced Extension Techniques

### Extension Point Composition

```cpp
// Combine multiple extension point handlers
class CompositeExtensionPointHandler {
public:
    void addHandler(ExtensionPointHandler handler, int priority);
    bool handle(const std::string& pointName, const std::any& data,
               const std::unordered_map<std::string, std::any>& params);
    
private:
    std::vector<std::pair<ExtensionPointHandler, int>> handlers_;
};
```

### Event Filtering

```cpp
// Filter events based on conditions
class EventFilter {
public:
    bool shouldProcessEvent(const std::string& eventName, const std::any& data) const;
    
    // Common filter conditions
    static bool filterByExtension(const std::string& eventName, const std::any& data,
                                  const std::string& extensionName);
    static bool filterByTime(const std::string& eventName, const std::any& data,
                            float minTime, float maxTime);
};
```

### Performance Monitoring for Extensions

```cpp
// Monitor extension performance
class ExtensionProfiler {
public:
    void startMeasurement(const std::string& operation);
    void stopMeasurement(const std::string& operation);
    double getAverageTime(const std::string& operation) const;
    size_t getOperationCount(const std::string& operation) const;
    
private:
    std::unordered_map<std::string, std::vector<double>> timings_;
    std::unordered_map<std::string, size_t> counts_;
};
```

## Extension Development Best Practices

### 1. Error Handling

```cpp
// Robust error handling
bool riskyOperation() {
    try {
        // Perform risky operation
        // ...
        return true;
    } catch (const std::exception& e) {
        // Log error
        // Attempt recovery
        // Return false to indicate failure
        return false;
    }
}
```

### 2. Resource Management

```cpp
// Proper resource cleanup
class Extension {
public:
    ~Extension() {
        // Ensure all resources are cleaned up
        shutdown();
    }
    
    void shutdown() {
        // Cleanup resources in reverse order of initialization
        cleanupComponents();
        cleanupHandlers();
        cleanupListeners();
        
        // Reset references
        globalConfig_.reset();
        brain_.reset();
    }
    
private:
    void cleanupComponents() {
        // Cleanup component factory
        // Clear component registry
    }
    
    void cleanupHandlers() {
        // Remove extension point handlers
        // Clear command registry
    }
    
    void cleanupListeners() {
        // Remove event listeners
        // Cleanup other listeners
    }
};
```

### 3. Thread Safety

```cpp
// Thread-safe extension operations
class ThreadSafeExtension {
public:
    bool initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
        std::lock_guard<std::mutex> lock(mutex_);
        return initializeImpl(config, brain);
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdownImpl();
    }
    
private:
    std::mutex mutex_;
    // ... other thread-safe members
};
```

### 4. Testing Extensions

```cpp
// Unit tests for extension functionality
class ExtensionTest {
public:
    void testComponentCreation() {
        // Test component factory
        // ...
    }
    
    void testExtensionPointHandlers() {
        // Test extension point registration
        // ...
    }
    
    void testEventSystem() {
        // Test event emission and handling
        // ...
    }
};
```

## Example Extension Templates

### 1. Simple Neuromodulator Extension

This extension adds a custom neuromodulator to NLM.

### 2. Custom Memory System Extension

This extension adds a custom working memory system.

### 3. Event-Driven Processing Extension

This extension processes events and modifies brain behavior.

### 4. Performance Monitoring Extension

This extension monitors and reports performance metrics.

## Extension Development Workflow

### Phase 1: Design

1. **Define Extension Purpose**: What does the extension do?
2. **Identify Components**: What components does the extension provide?
3. **Determine Extension Points**: Where does the extension integrate with the brain?
4. **Design Event System**: What events does the extension emit/receive?
5. **Plan Configuration**: What configuration options are needed?

### Phase 2: Implementation

1. **Create Extension Class**: Implement the main extension interface
2. **Register Components**: Use the component factory to create custom components
3. **Implement Handlers**: Create extension point handlers for custom integration
4. **Setup Event System**: Implement event listeners and emission
5. **Add Configuration**: Support extension-specific configuration

### Phase 3: Testing

1. **Unit Tests**: Test individual components and handlers
2. **Integration Tests**: Test extension integration with NLM
3. **Performance Tests**: Measure performance impact
4. **Stress Tests**: Test extension behavior under load

### Phase 4: Deployment

1. **Build Extension**: Compile and package the extension
2. **Create Manifest**: Generate extension manifest file
3. **Test Loading**: Verify extension can be loaded
4. **Deploy**: Distribute extension to users

## Extension Development Tools

### Development Environment Setup

```bash
# Install NLM development tools
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Build extension example
mkdir build_ext
cd build_ext
cmake .. -DNLM_EXTENSIONS_DIR=../../extensions
make -j$(nproc)
```

### Debugging Extensions

```bash
# Run with debug logging
export NLM_LOG_LEVEL=debug
./nlm --extension-debug

# Profile extension performance
./nlm --extension-profile --extension=my_extension
```

### Extension Testing Framework

```cpp
// Extension testing framework
class ExtensionTestFramework {
public:
    static void runAllTests();
    static void registerTest(const std::string& name, std::function<void()> test);
    static void assert(bool condition, const std::string& message);
};
```

## Future Extension Capabilities

### 1. Extension Marketplace

**Feature**: Centralized extension distribution and discovery.

**Benefits**:
- Easy extension discovery
- Version management
- Dependency resolution
- User ratings and reviews

### 2. Extension Versioning

**Feature**: Semantic versioning for extensions.

**Benefits**:
- Backward compatibility
- Upgrade management
- Feature tracking

### 3. Extension Dependencies

**Feature**: Automatic dependency management.

**Benefits**:
- Extension composition
- Conflict resolution
- Conditional loading

### 4. Extension Security

**Feature**: Extension sandboxing and permissions.

**Benefits**:
- Resource isolation
- Permission control
- Security auditing

## Conclusion

The NLM extension system provides a powerful framework for creating custom extensions that can modify, enhance, or extend the brain's functionality. By following the patterns and best practices outlined in this guide, developers can create sophisticated extensions that integrate seamlessly with the NLM architecture.

Key takeaways:

1. **Understand the Architecture**: Extension points, events, and component factory are the core building blocks.
2. **Follow Best Practices**: Error handling, resource management, and thread safety are essential.
3. **Test Thoroughly**: Unit tests, integration tests, and performance tests ensure quality.
4. **Document Well**: Clear documentation and examples help other developers.
5. **Follow the Workflow**: Design, implement, test, and deploy in a structured manner.

The extension system transforms NLM from a fixed neural simulator into a flexible, extensible artificial brain platform capable of supporting diverse research and application scenarios through custom extensions.
