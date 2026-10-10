# Plugin Development Guide

This document provides comprehensive guidance for developing plugins for the NLM Neural Simulation Project.

## Overview

The NLM plugin architecture provides a modular and extensible framework for extending the neural simulation capabilities. This guide covers the plugin development process, including the plugin interface, categories, development tools, and best practices.

## Plugin Categories

### Neuron Plugins
- Extend neural dynamics with custom neuron models
- Support for LIF, adaptive, resonant, and other neuron types
- Custom parameters and configuration

### Synapse Plugins
- Customize synaptic dynamics and plasticity
- Support for Hebbian, STDP, and custom plasticity rules
- Connection management and optimization

### Memory Plugins
- Implement custom memory storage and retrieval strategies
- Support for different memory architectures (hierarchical, caching, etc.)
- Persistence and backup capabilities

### Cognition Plugins
- Extend cognitive functions and decision-making
- Custom planning and learning algorithms
- Knowledge representation and reasoning

### Neuromodulation Plugins
- Implement neuromodulation systems
- Reward prediction and error signaling
- Motivational and attentional mechanisms

### Visualization Plugins
- Create custom visualization outputs
- Real-time rendering and analysis
- Multiple output formats (text, CSV, JSON, graphics)

## Plugin Development Process

### 1. Understanding the Plugin Interface

The core plugin interface is defined in `src/plugin/PluginBase.hpp`. Key components include:

- **IPlugin**: Abstract base class for all plugins
- **PluginConfig**: Configuration structure for plugin settings
- **PluginManifest**: Metadata describing plugin properties and requirements
- **PluginManager**: Lifecycle management system
- **PluginLoader**: Dynamic library loading system

### 2. Creating a Custom Plugin

Basic template for a plugin implementation:

```cpp
#include "src/plugin/PluginBase.hpp"

class CustomPlugin : public IPlugin {
private:
    bool initialized;
    PluginConfig config;
    
public:
    std::string getName() const override {
        return "CustomPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getAuthor() const override {
        return "Your Name";
    }
    
    std::string getDescription() const override {
        return "A custom plugin for NLM";
    }
    
    bool initialize(const PluginConfig& cfg) override {
        config = cfg;
        // Initialize plugin resources
        return true;
    }
    
    void shutdown() override {
        // Cleanup resources
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"nlm_core"};
    }
};
```

### 3. Plugin Factory Function

Every plugin must export a factory function:

```cpp
extern "C" IPlugin* createPlugin() {
    return new CustomPlugin();
}

extern "C" void destroyPlugin(IPlugin* plugin) {
    delete plugin;
}
```

### 4. Plugin Manifest

Create a plugin manifest file (JSON format) for plugin metadata:

```json
{
    "name": "CustomPlugin",
    "version": "1.0.0",
    "author": "Your Name",
    "description": "A custom plugin for NLM",
    "category": "COGNITION",
    "dependencies": ["nlm_core", "nlm_cognition"],
    "features": ["learning", "planning"],
    "minCoreVersion": "0.1.0",
    "maxCoreVersion": "1.0.0"
}
```

### 5. Plugin Configuration

Configure your plugin with the PluginConfig structure:

```cpp
PluginConfig config;
config.name = "CustomPlugin";
config.enabled = true;
config.settings["learningRate"] = "0.01";
config.enabledFeatures = {"adaptive", "online"};
```

## Building Plugins

### Build System

The plugin build system is integrated with the main CMake configuration. To build plugin examples:

```bash
cd /path/to/nlm/repository
mkdir build
cd build
cmake -DNLM_BUILD_EXAMPLES=ON ..
make plugin_examples
```

### Plugin Dependencies

Plugins can depend on NLM core modules and other plugins. Dependencies are automatically resolved during loading.

## Plugin Testing

### Unit Testing

Create unit tests for your plugin components:

```bash
# Run plugin-specific tests
cd /path/to/nlm/repository
./run_memory_tests.sh
```

### Integration Testing

Test plugin integration with the main simulation:

```cpp
// Example integration test
void testPluginIntegration() {
    PluginManager manager;
    
    // Load a plugin
    bool success = manager.loadPlugin("path/to/plugin.so");
    assert(success);
    
    // Initialize the plugin
    PluginConfig config;
    config.name = "CustomPlugin";
    success = manager.initializePlugin("CustomPlugin", config);
    assert(success);
    
    // Verify plugin functionality
    auto plugin = manager.getPlugin("CustomPlugin");
    assert(plugin != nullptr);
    
    // Cleanup
    manager.shutdownPlugin("CustomPlugin");
    manager.unloadPlugin("CustomPlugin");
}
```

## Plugin Communication

### Events

Plugins can communicate through event-driven architecture:

```cpp
// Event structure
struct PluginEvent {
    std::string eventId;
    std::string sourcePlugin;
    std::string targetPlugin;
    std::map<std::string, std::string> data;
    double timestamp;
};
```

### Callbacks

Register callbacks for plugin-specific events:

```cpp
void MyPlugin::subscribeEvent(const std::string& eventId, IEventSubscriber* subscriber) {
    // Register subscriber for specific event
}

void MyPlugin::publishEvent(const PluginEvent& event) {
    // Notify subscribers of event
}
```

## Plugin Development Best Practices

### 1. Error Handling

Implement robust error handling:

```cpp
bool MyPlugin::initialize(const PluginConfig& config) {
    try {
        // Initialize resources
    } catch (const std::exception& e) {
        lastError = std::string("Initialization failed: ") + e.what();
        return false;
    }
    return true;
}
```

### 2. Resource Management

Use RAII for resource management:

```cpp
class MyPlugin : public IPlugin {
public:
    ~MyPlugin() override {
        shutdown(); // Ensure cleanup in destructor
    }
    
    void shutdown() override {
        // Cleanup allocated resources
    }
};
```

### 3. Thread Safety

Ensure thread-safe operations:

```cpp
class MyPlugin : public IPlugin {
private:
    std::mutex mutex;
    
public:
    void someMethod() {
        std::lock_guard<std::mutex> lock(mutex);
        // Critical section
    }
};
```

### 4. Configuration Validation

Validate configuration parameters:

```cpp
bool validateConfig(const PluginConfig& config) {
    if (config.name.empty()) {
        throw std::invalid_argument("Plugin name cannot be empty");
    }
    
    if (config.settings.find("learningRate") == config.settings.end()) {
        throw std::invalid_argument("Learning rate parameter required");
    }
    
    double learningRate = std::stod(config.settings.at("learningRate"));
    if (learningRate <= 0.0 || learningRate > 1.0) {
        throw std::invalid_argument("Learning rate must be between 0 and 1");
    }
    
    return true;
}
```

## Advanced Plugin Features

### Hot-Swapping

Plugins can be dynamically loaded and unloaded during runtime:

```cpp
// Hot-swap a plugin
manager.reloadPlugin("OldPlugin");
manager.unloadPlugin("OldPlugin");
manager.loadPlugin("NewPlugin.so");
```

### Plugin Marketplaces

Create plugin repositories and marketplaces for distribution:

1. Package plugins with manifest and resources
2. Create repository server for plugin discovery
3. Implement versioning and dependency resolution
4. Provide plugin metadata and ratings

## Troubleshooting

### Common Issues

#### Loading Failures
- Ensure the plugin library exports the correct symbols
- Check for mismatched compiler versions
- Verify plugin dependencies are satisfied

#### Initialization Failures
- Validate configuration parameters
- Check for resource conflicts
- Verify plugin dependencies are loaded

#### Runtime Errors
- Use appropriate logging and debugging tools
- Implement error handling for all external interactions
- Test in isolated environments

### Debugging Tips

1. Enable debug logging in the plugin manager
2. Use external debugging tools (gdb, Visual Studio, etc.)
3. Test plugins with minimal dependencies
4. Validate plugin manifests before loading

## Future Enhancements

### Plugin Categories
- Environment plugins for custom simulation environments
- Action plugins for custom motor systems
- Analysis plugins for data processing and visualization
- Integration plugins for external systems and APIs

### Plugin Tools
- Plugin wrapper framework for compatibility
- Plugin sandboxing for security
- Plugin performance monitoring
- Automated plugin testing framework

## Conclusion

The NLM plugin architecture provides a powerful and flexible framework for extending neural simulation capabilities. By following the guidelines in this document, developers can create high-quality plugins that integrate seamlessly with the NLM system.

For more detailed information, refer to the API documentation and example plugins provided in the repository.