# NLM Plugin System Examples

This document provides comprehensive examples for creating and using plugins in the NLM (Neural Learning Module) brain system.

## Overview

The NLM plugin system provides a modular architecture for extending brain functionality through:
- Dynamic plugin loading
- Extension points for all brain systems
- Runtime plugin management
- Plugin lifecycle management
- Plugin versioning and compatibility

## Quick Start

### 1. Creating a Simple Plugin

Here's a minimal plugin implementation:

```cpp
#include "src/plugin/Plugin.hpp"

class MyNeuronPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "MyNeuronPlugin",
            "Custom neuron implementation",
            "Developer Name",
            PluginVersion(1, 0, 0),
            PluginType::Neuron
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // Initialize plugin resources
        return true;
    }
    
    // Custom functionality
    void customMethod() {
        // Implement custom logic
    }
};
```

### 2. Plugin Manager Initialization

```cpp
#include "src/plugin/PluginManager.hpp"

int main() {
    // Configure plugin system
    PluginManager::PluginSettings settings;
    settings.pluginDirectory = "./plugins";
    settings.autoLoad = true;
    settings.enableAllPlugins = false;
    settings.sandboxPlugins = true;
    
    // Initialize plugin manager
    PluginSystem& pluginSystem = PluginSystem::getInstance();
    bool success = pluginSystem.initialize(
        PluginSystem::SystemSettings()
    );
    
    if (!success) {
        // Handle initialization failure
        return 1;
    }
    
    return 0;
}
```

### 3. Loading and Enabling a Plugin

```cpp
#include "src/plugin/PluginManager.hpp"

void loadPluginExample() {
    PluginSystem& pluginSystem = PluginSystem::getInstance();
    
    // Load plugin from file
    bool success = pluginSystem.registerPlugin("./plugins/my_plugin.so", PluginType::Neuron);
    
    if (success) {
        // Find the loaded plugin
        std::string pluginId = "my_plugin";
        
        // Enable the plugin
        pluginSystem.enablePlugin(pluginId);
        
        // Configure plugin
        auto config = std::make_shared<PluginConfig>();
        config->setInt("param1", 42);
        config->setFloat("param2", 3.14f);
        
        pluginSystem.configurePlugin(pluginId, config);
        
        std::cout << "Plugin loaded and enabled successfully!" << std::endl;
    } else {
        std::cout << "Failed to load plugin!" << std::endl;
    }
}
```

### 4. Using Extension Points

```cpp
#include "src/plugin/PluginManager.hpp"

void useExtensionPointsExample() {
    PluginSystem& pluginSystem = PluginSystem::getInstance();
    
    // Register extension point
    pluginSystem.registerExtensionPoint(
        "custom_neuron_factory",
        PluginType::Neuron,
        [](std::shared_ptr<Plugin> plugin) {
            // Validation function
            return plugin->getMetadata().type == PluginType::Neuron;
        }
    );
    
    // Request extension point (another plugin provides it)
    bool success = pluginSystem.requestExtensionPoint(
        "custom_neuron_factory",
        myNeuronPlugin
    );
    
    if (success) {
        std::cout << "Successfully registered as extension point provider!" << std::endl;
    }
}
```

### 5. Plugin Communication

```cpp
#include "src/plugin/PluginManager.hpp"

class MyPlugin : public Plugin {
public:
    bool onInitialize() override {
        // Register message handler
        if (getManager()) {
            getManager()->registerMessageHandler(
                "neuron_update",
                [this](const std::string& message, const std::string& data) {
                    onNeuronUpdate(message, data);
                }
            );
        }
        return true;
    }
    
    void onMessage(const std::string& message, const std::string& data) override {
        if (message == "neuron_update") {
            onNeuronUpdate(message, data);
        }
    }
    
    void onNeuronUpdate(const std::string& message, const std::string& data) {
        // Handle neuron update message
        std::cout << "Received neuron update: " << data << std::endl;
    }
};
```

## Advanced Examples

### Neuron Plugin Example

```cpp
#include "src/plugins/neurons/HodgkinHuxleyPlugin.hpp"

class CustomHodgkinHuxleyPlugin : public HodgkinHuxleyNeuronPlugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "CustomHodgkinHuxley",
            "Custom Hodgkin-Huxley implementation with tuning",
            "Developer Name",
            PluginVersion(1, 2, 0),
            PluginType::Neuron
        );
        return metadata;
    }
    
    bool onInitialize() override {
        HodgkinHuxleyNeuronPlugin::onInitialize();
        
        // Custom initialization
        setMaximumSodiumConductance(150.0f);  // Higher sodium conductance
        setMaximumPotassiumConductance(40.0f); // Different potassium conductance
        
        return true;
    }
};
```

### Plasticity Plugin Example

```cpp
#include "src/plugins/plasticity/BCMPlugin.hpp"

class CustomBCMPlugin : public BCMPlasticityPlugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "CustomBCM",
            "Custom BCM plasticity with learning rate adaptation",
            "Developer Name",
            PluginVersion(2, 0, 1),
            PluginType::Plasticity
        );
        return metadata;
    }
    
    void update(float dt) override {
        // Adapt learning rate based on network activity
        float activityLevel = getAverageNetworkActivity();
        
        // Increase learning rate with high activity
        float newLearningRate = 0.01f + activityLevel * 0.05f;
        newLearningRate = std::min(0.1f, std::max(0.001f, newLearningRate));
        
        setLearningRate(newLearningRate);
        
        BCMPlasticityPlugin::update(dt);
    }
    
private:
    float getAverageNetworkActivity() const {
        // Get average network activity from plugin manager
        if (getManager()) {
            // In a real implementation, query the brain system
            return 0.5f; // Placeholder
        }
        return 0.5f;
    }
};
```

### Memory Plugin Example

```cpp
#include "src/plugins/memories/WorkingMemoryPlugin.hpp"

class EnhancedWorkingMemoryPlugin : public WorkingMemoryPlugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "EnhancedWorkingMemory",
            "Working memory with consolidation and replay capabilities",
            "Developer Name",
            PluginVersion(1, 1, 0),
            PluginType::Memory
        );
        return metadata;
    }
    
    bool onInitialize() override {
        WorkingMemoryPlugin::onInitialize();
        
        // Custom initialization
        setCapacity(200);           // Larger capacity
        setDecayRate(0.005f);       // Slower decay
        setCompetitionStrength(0.7f);  // Stronger competition
        
        return true;
    }
    
    void enable() override {
        WorkingMemoryPlugin::enable();
        
        // Register additional extension points
        if (getManager()) {
            getManager()->registerExtensionPoint(
                "memory_consolidation",
                PluginType::Memory
            );
        }
    }
    
    void replayMemory() {
        // Implement memory replay
        auto activeItems = getActiveMemoryItems();
        
        for (auto& item : activeItems) {
            if (item.lastRehearsed < currentTime_ - rehearsalInterval_) {
                rehearseMemoryItem(item.id);
            }
        }
    }
};
```

## Plugin Development Best Practices

### 1. Plugin Structure

```
plugins/
├── neurons/
│   ├── HodgkinHuxleyPlugin.hpp
│   ├── HodgkinHuxleyPlugin.cpp
│   └── IzhikevichPlugin.hpp
├── plasticity/
│   ├── BCMPlugin.hpp
│   ├── BCMPlugin.cpp
│   └── OjaPlugin.hpp
├── memories/
│   ├── WorkingMemoryPlugin.hpp
│   └── EpisodicMemoryPlugin.hpp
├── neuromodulation/
│   ├── DopaminePlugin.hpp
│   └── SerotoninPlugin.hpp
├── sensory/
│   └── SensoryPlugin.hpp
├── motor/
│   └── MotorControlPlugin.hpp
├── environment/
│   └── EnvironmentModelPlugin.hpp
├── cognition/
│   └── CognitiveArchitecturePlugin.hpp
├── examples/
│   └── PluginExamples.md
└── configuration/
    ├── PluginConfig.hpp
    └── PluginSecurity.hpp
```

### 2. Build Configuration

Create a plugin-specific CMakeLists.txt:

```cmake
# plugins/neurons/HodgkinHuxleyPlugin/CMakeLists.txt
add_library(HodgkinHuxleyPlugin SHARED HodgkinHuxleyPlugin.cpp)
target_include_directories(HodgkinHuxleyPlugin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(HodgkinHuxleyPlugin PROPERTIES
    OUTPUT_NAME "nlm_hh_plugin"
    SOVERSION "1"
)
```

### 3. Plugin Information File

Create plugin information files:

```json
// plugins/neurons/HodgkinHuxleyPlugin/plugin.json
{
    "name": "HodgkinHuxleyPlugin",
    "version": "1.0.0",
    "type": "Neuron",
    "description": "Hodgkin-Huxley neuron model implementation",
    "author": "NLM Team",
    "license": "MIT",
    "dependencies": [],
    "extensionPoints": ["neuron_creation", "neuron_simulation"],
    "capabilities": {
        "supportsDynamicLoading": true,
        "supportsRuntimeManagement": true,
        "requiresSandboxing": false,
        "threadSafe": true
    }
}
```

## Troubleshooting

### Common Issues

1. **Plugin Loading Fails**
   - Check if the plugin file exists and is readable
   - Verify plugin file format (correct library extension)
   - Ensure all dependencies are available
   - Check for compilation errors in the plugin

2. **Plugin Validation Fails**
   - Check plugin metadata for required fields
   - Verify plugin version compatibility
   - Ensure security policies are not blocking the plugin

3. **Plugin Execution Fails**
   - Check for exceptions in plugin initialization
   - Verify plugin configuration is valid
   - Check for resource limits being exceeded

4. **Plugin Performance Issues**
   - Monitor plugin execution time
   - Check for memory leaks in plugins
   - Optimize plugin communication

### Debugging Tips

- Enable plugin logging for detailed information
- Use the plugin debugger to step through plugin code
- Check system logs for plugin-related errors
- Validate plugin configuration files

## Integration with NLM Brain

### Plugin-Aware Brain Integration

The NLM brain system can be extended with plugins:

```cpp
// In Brain.hpp
#include "plugin/PluginSystem.hpp"

class Brain {
private:
    std::unique_ptr<PluginSystem> pluginSystem_;
    // ...
public:
    bool initializeWithPlugins(const PluginSystem::SystemSettings& settings) {
        pluginSystem_ = std::make_unique<PluginSystem>();
        return pluginSystem_->initialize(settings) && 
               Brain::initialize();
    }
    
    void step(float dt) override {
        // Update plugins
        if (pluginSystem_ && pluginSystem_->isInitialized()) {
            pluginSystem_->updateAllPlugins(dt);
        }
        
        // Original brain step logic
        // ...
    }
};
```

### Plugin-Specific Debugging and Visualization

```cpp
// In Brain.cpp
void Brain::enablePluginVisualization() {
    if (pluginSystem_) {
        auto debugger = std::make_shared<PluginDebugger>();
        debugger->enableVisualization(true);
        debugger->setOutputFile("plugin_debug.log");
        
        pluginSystem_->attachDebugger(debugger);
    }
}

void Brain::logPluginStatus() const {
    if (pluginSystem_) {
        std::string status = pluginSystem_->getSystemStatus();
        std::cout << "=== Plugin System Status ===" << std::endl;
        std::cout << status << std::endl;
        std::cout << "==========================" << std::endl;
    }
}
```

## Conclusion

The NLM plugin system provides a powerful framework for extending and customizing the brain system. By following these examples and best practices, developers can create sophisticated plugins that enhance the capabilities of the NLM brain system.

The plugin system enables:
- Modular development of neural components
- Custom plasticity rules and learning algorithms
- Specialized memory systems
- Custom neuromodulatory signals
- Novel sensory and motor systems
- Advanced environment models
- Unique cognitive architectures

Start with the provided examples and gradually build more complex plugins as needed.
