# Plugin Development Guide
# ====================

## Overview
This project implements a comprehensive plugin architecture for the NLM (Neural Learning Machine) framework.
The plugin system allows for dynamic loading, unloading, and management of neural computation components.

## Plugin Architecture

### Base Classes

#### Plugin Interface
All plugins must implement the base `Plugin` interface:

```cpp
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Identification
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual const std::string& getName() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual PluginStatus getStatus() const = 0;
    
    // Lifecycle
    virtual bool load(const nlohmann::json& config) = 0;
    virtual bool initialize() = 0;
    virtual bool run() = 0;
    virtual bool shutdown() = 0;
    virtual bool unload() = 0;
    
    // State management
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
    virtual nlohmann::json getState() const = 0;
    virtual bool setState(const nlohmann::json& state) = 0;
};
```

#### Component Plugin Interfaces
The plugin system provides specialized interfaces for different types of components:

- **NeuronTypePlugin**: For neuron implementation plugins
- **PlasticityRulePlugin**: For synaptic plasticity rule plugins  
- **NeuromodulatorPlugin**: For neuromodulatory signal plugins
- **MemorySystemPlugin**: For memory system plugins
- **LearningAlgorithmPlugin**: For learning algorithm plugins

### Plugin Manager

The `PluginManager` class provides:

- **Dynamic Loading/Unloading**: Load plugins from shared libraries at runtime
- **Dependency Resolution**: Ensure plugins are loaded in correct order based on dependencies
- **Thread-Safe Operations**: All operations are thread-safe for concurrent access
- **Configuration Integration**: Plugins can be configured via the global configuration system
- **Event System**: Callbacks for plugin lifecycle events

### Plugin Metadata System

Each plugin includes metadata about:

- **Basic Info**: Name, version, author, license
- **Categorization**: Category and tags for organization
- **Dependencies**: Required plugins for proper operation
- **Configuration**: Default configuration parameters

## Plugin Types

### Neuron Type Plugins

Available implementations:

#### LIF Neuron Plugin
- **Description**: Leaky Integrate-and-Fire neuron model
- **Parameters**: Threshold, resting potential, membrane time constant
- **Dynamics**: Standard LIF integration with refractory period

#### Adaptive Exponential Neuron Plugin
- **Description**: Adaptive exponential integrate-and-fire neuron model
- **Parameters**: Adaptation variable for spike-frequency adaptation
- **Dynamics**: Expressive power with spike initiation dynamics

### Plasticity Rule Plugins

Available implementations:

#### STDP Plugin
- **Description**: Spike-Timing Dependent Plasticity rule
- **Parameters**: LTP/LTD weights, time constant
- **Mechanism**: Pair-based STDP with exponential windows

#### Hebbian Plugin
- **Description**: Correlation-based plasticity rule
- **Parameters**: Learning rate, weight bounds
- **Mechanism**: Covariance-based Hebbian learning

### Neuromodulator Plugins

Available implementations:

#### Dopamine Plugin
- **Description**: Reward and reinforcement learning signal
- **Parameters**: Reward sensitivity, prediction error handling
- **Effects**: Modulates plasticity and motivation

#### Serotonin Plugin
- **Description**: Mood and impulsivity regulation
- **Parameters**: Mood influence, impulsivity suppression
- **Effects**: Behavioral inhibition and emotional regulation

### Memory System Plugins

Available implementations:

#### Working Memory Plugin
- **Description**: Short-term memory for active information
- **Parameters**: Capacity, activation decay
- **Functions**: Item storage, activation updates

#### Episodic Memory Plugin
- **Description**: Long-term memory for events and experiences
- **Parameters**: Consolidation threshold, decay rate
- **Functions**: Episode storage, retrieval, consolidation

## Configuration-Driven Loading

### JSON Configuration

Plugins can be configured via `config.json`:

```json
{
    "plugins": {
        "enabled": ["lif-neuron", "stdp-rule", "dopamine-modulator"],
        "pluginDirectories": ["./plugins", "/usr/local/lib/nlm/plugins"],
        "autoLoad": true,
        "autoStart": true
    },
    "neuronTypes": {
        "lif": {
            "threshold": -55.0,
            "restingPotential": -70.0,
            "membraneTimeConstant": 20.0
        }
    },
    "plasticityRules": {
        "stdp": {
            "ltpWeight": 0.01,
            "ltdWeight": 0.012,
            "timeConstant": 20.0
        }
    }
}
```

### INI Configuration

For additional configuration, `plugins.ini` provides:

- Plugin metadata and descriptions
- Default configuration values
- Dependency relationships
- Category classifications

## Plugin Lifecycle

### Loading Phase

1. **Plugin Discovery**: Scan directories for plugin libraries
2. **Validation**: Check plugin metadata and dependencies
3. **Dependency Resolution**: Ensure all required plugins are available
4. **Factory Instantiation**: Create plugin instance using factory pattern

### Initialization Phase

1. **Configuration**: Apply plugin-specific configuration
2. **Resource Allocation**: Allocate necessary resources
3. **State Setup**: Initialize internal state
4. **Registration**: Register with plugin manager

### Runtime Phase

1. **Execution**: Plugin performs its core functionality
2. **State Updates**: Maintain and update internal state
3. **Event Handling**: Respond to system events and configuration changes
4. **Error Management**: Handle errors and report status

### Shutdown Phase

1. **Graceful Shutdown**: Clean shutdown of plugin operations
2. **Resource Cleanup**: Release allocated resources
3. **State Persistence**: Save current state if needed
4. **Unregistration**: Remove from plugin manager

### Unloading Phase

1. **Lock Release**: Ensure no threads are using the plugin
2. **Library Unload**: Unload shared library from memory
3. **Cleanup**: Release all plugin-specific resources

## Plugin Development Guide

### Creating a New Plugin

1. **Choose Base Class**: Inherit from appropriate plugin interface
2. **Implement Virtual Methods**: Provide implementations for all pure virtual methods
3. **Plugin Factory**: Export `createPlugin()` function for plugin manager
4. **Library Build**: Compile plugin as shared library (`.so`, `.dylib`, or `.dll`)
5. **Configuration**: Provide default configuration and validation

### Plugin Example Structure

```cpp
// Plugin header
#pragma once
#include "PluginBase.hpp"

namespace nlm {
namespace plugin {

class MyPlugin : public ThreadSafePlugin {
public:
    MyPlugin();
    ~MyPlugin() override;
    
    const std::string& getName() const override;
    const std::string& getVersion() const override;
    PluginStatus getStatus() const override;
    
    bool load(const nlohmann::json& config) override;
    bool initialize() override;
    bool run() override;
    bool shutdown() override;
    bool unload() override;
    
    // ... other required methods
};

// Plugin factory function
extern "C" Plugin* createPlugin() {
    return new MyPlugin();
}

}} // namespace nlm::plugin
```

### Building Plugins

Plugin libraries should be built with the same compiler and options as the main framework:

```bash
# Example build command for Linux/macOS
g++ -shared -fPIC -o libmyplugin.so myplugin.cpp -I../include

# Example build command for Windows
cl /LD myplugin.cpp /I..\include /Fe:myplugin.dll
```

### Testing Plugins

Each plugin type has comprehensive unit tests:

- **Neuron Plugins**: Test spike generation and dynamics
- **Plasticity Rules**: Test weight updates and bounds checking
- **Neuromodulators**: Test concentration updates and effects
- **Memory Systems**: Test storage/retrieval operations

### Performance Considerations

- **Thread Safety**: All plugin operations are thread-safe
- **Resource Management**: Proper cleanup of all resources
- **Memory Efficiency**: Use move semantics and unique_ptr where possible
- **Error Handling**: Robust error handling and reporting

## Integration with NLM Framework

### Component Replacement

The plugin system allows replacing hardcoded components:

- Replace default LIF neuron with custom implementation
- Swap STDP rule with novel plasticity mechanism
- Add new neuromodulators without framework modification

### Backward Compatibility

The plugin system maintains backward compatibility:

- All existing hardcoded components continue to work
- Plugin manager gracefully handles missing plugins
- Configuration can be migrated gradually

### Migration Guide

1. **Phase 1**: Create plugin for component you want to replace
2. **Phase 2**: Configure plugin manager to use plugin
3. **Phase 3**: Disable old hardcoded component
4. **Phase 4**: Test thoroughly
5. **Phase 5**: Remove old hardcoded component

## Troubleshooting

### Common Issues

#### Plugin Not Loading
- Check library dependencies
- Verify plugin exports factory function
- Ensure plugin metadata is valid

#### Plugin Loading Failures
- Check configuration parameters
- Verify system resources
- Review error logs

#### Thread Safety Issues
- Ensure proper locking in plugin implementation
- Check for race conditions in shared state
- Use atomic operations where appropriate

### Debugging Tools

- **Plugin Manager Logs**: Detailed logging of plugin operations
- **Error Reporting**: Comprehensive error messages and stack traces
- **Performance Monitoring**: Plugin performance statistics
- **Configuration Validation**: Validate plugin configuration parameters

## Future Enhancements

### Planned Features

1. **Plugin Hot-Reload**: Update plugins without restarting the system
2. **Plugin Sandboxing**: Isolate plugins for security
3. **Plugin Market**: Share plugins between users
4. **Plugin Documentation**: Automated documentation generation

### Research Directions

- **Machine Learning for Plugin Discovery**: AI-powered plugin selection
- **Plugin Composition**: Combine multiple plugins for complex behaviors
- **Plugin Versioning**: Advanced version management and compatibility
- **Plugin Optimization**: Runtime optimization of plugin configurations

## Conclusion

The NLM plugin architecture provides a flexible, extensible framework for neural computation components. By separating concerns and enabling dynamic loading, it allows researchers and developers to easily create, test, and deploy novel neural mechanisms without modifying the core framework.
