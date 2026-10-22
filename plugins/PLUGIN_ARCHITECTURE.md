# NLM Plugin Architecture

## Overview

This document describes the NLM plugin architecture, which provides a modular system for extending NLM with custom components. The plugin system allows for:

- Custom neuron models
- External plasticity rules
- User-defined experiments
- Runtime plugin loading and management
- Plugin communication and integration

## Plugin System Design

### Core Concepts

1. **Plugin Interface**: All plugins implement a common interface
2. **Plugin Registry**: Central management of available plugins
3. **Plugin Loader**: Dynamic loading and lifecycle management
4. **Plugin Communication**: Standardized message passing system
5. **Plugin Configuration**: Runtime configuration support

### Plugin Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Plugin Registry                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │  NeuronMgr  │  │ PlasticityMgr │  │ ExperimentMgr │          │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────┘
         │                         │                        │
         ▼                         ▼                        ▼
┌─────────────────────────────────────────────────────────────┐
│                Plugin Factory                                │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │  NeuronFactory │  │ PlasticityFactory │  │ ExperimentFactory │          │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────┘
         │                         │                        │
         ▼                         ▼                        ▼
┌─────────────────────────────────────────────────────────────┐
│                  Plugin Loader                               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │   DynamicLoader │  │    StaticLoader │  │    ConfigLoader │          │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────┘
```

## Plugin Interfaces

### 1. Neuron Model Interface

```cpp
class INeuronModel {
public:
    virtual ~INeuronModel() = default;
    
    // Core neuron dynamics
    virtual float update(float voltage, float current, float dt) = 0;
    virtual bool isSpiking(float voltage) = 0;
    virtual float getRestingPotential() const = 0;
    virtual float getThreshold() const = 0;
    
    // Neuron properties
    virtual float getMembraneTimeConstant() const = 0;
    virtual float getRefractoryPeriod() const = 0;
    virtual NeuronType getType() const = 0;
    
    // Plasticity integration
    virtual void applyPlasticity(float weightChange) = 0;
    
    // Plugin information
    virtual std::string getModelName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
};
```

### 2. Plasticity Rule Interface

```cpp
class IPlasticityRule {
public:
    virtual ~IPlasticityRule() = default;
    
    // Plasticity update
    virtual float updatePlasticity(float preSpike, float postSpike, 
                                  float weight, float dt) = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    virtual Config getDefaultConfig() const = 0;
    
    // Rule properties
    virtual std::string getRuleName() const = 0;
    virtual std::string getRuleType() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    
    // Statistics
    virtual void resetStats() = 0;
    virtual float getLearningRate() const = 0;
    virtual float getTotalWeightChange() const = 0;
};
```

### 3. Experiment Interface

```cpp
class IExperiment {
public:
    virtual ~IExperiment() = default;
    
    // Experiment lifecycle
    virtual bool initialize(const Config& config) = 0;
    virtual void run(int steps) = 0;
    virtual void finalize() = 0;
    
    // Data access
    virtual const Metrics& getMetrics() const = 0;
    virtual std::vector<float> getTimeSeriesData() const = 0;
    virtual std::vector<std::string> getEventLog() const = 0;
    
    // Experiment properties
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual ExperimentPhase getPhase() const = 0;
    
    // Plugin integration
    virtual bool requiresPlugin(const std::string& pluginType) const = 0;
};
```

## Plugin Registry

### Neuron Plugin Registry

```cpp
class NeuronPluginRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<INeuronModel>> neuronModels;
    std::vector<std::string> registeredModelNames;
    
public:
    // Registration
    bool registerNeuronModel(const std::string& name, 
                           std::unique_ptr<INeuronModel> model);
    bool unregisterNeuronModel(const std::string& name);
    
    // Retrieval
    INeuronModel* getNeuronModel(const std::string& name);
    const std::vector<std::string>& getRegisteredModelNames() const;
    
    // Default models
    void registerDefaultModels();
    
    // Configuration
    void clear() { neuronModels.clear(); registeredModelNames.clear(); }
};
```

### Plasticity Plugin Registry

```cpp
class PlasticityPluginRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<IPlasticityRule>> plasticityRules;
    std::vector<std::string> registeredRuleNames;
    std::unordered_map<std::string, std::string> ruleDependencies;
    
public:
    // Registration
    bool registerPlasticityRule(const std::string& name,
                               std::unique_ptr<IPlasticityRule> rule,
                               const std::string& dependencies = "");
    
    // Retrieval
    IPlasticityRule* getPlasticityRule(const std::string& name);
    std::vector<std::string> getRulesForNeuronType(NeuronType type) const;
    
    // Configuration
    void enableRule(const std::string& name);
    void disableRule(const std::string& name);
    void configureRule(const std::string& name, const Config& config);
    
    // Default rules
    void registerDefaultRules();
};
```

## Plugin Loader

### Dynamic Plugin Loader

```cpp
class DynamicPluginLoader {
private:
    std::string pluginDirectory;
    std::unordered_map<std::string, void*> loadedPlugins;
    std::vector<std::string> pluginSearchPaths;
    
public:
    DynamicPluginLoader(const std::string& directory = "./plugins");
    ~DynamicPluginLoader();
    
    // Loading
    bool loadPlugin(const std::string& pluginName);
    bool unloadPlugin(const std::string& pluginName);
    bool loadAllPlugins();
    
    // Plugin information
    bool isPluginLoaded(const std::string& pluginName) const;
    std::vector<std::string> getLoadedPlugins() const;
    
    // Configuration
    void addSearchPath(const std::string& path);
    void setPluginDirectory(const std::string& directory);
};
```

## Plugin Examples

### Example 1: Custom Neuron Model

```cpp
class CustomLIFNeuron : public INeuronModel {
public:
    std::string getModelName() const override { return "CustomLIF"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getAuthor() const override { return "User"; }
    
    float update(float voltage, float current, float dt) override {
        // Custom LIF dynamics with additional parameters
        voltage += (current - voltage / tau_m) * dt / tau_m;
        if (voltage >= threshold) {
            voltage = reset_voltage;
            return 1.0f; // Spike
        }
        return 0.0f; // No spike
    }
    
    bool isSpiking(float voltage) override {
        return voltage >= threshold;
    }
    
    // ... other required methods
};
```

### Example 2: Custom Plasticity Rule

```cpp
class CustomSTDP : public IPlasticityRule {
public:
    std::string getRuleName() const override { return "CustomSTDP"; }
    std::string getRuleType() const override { return "SymmetricSTDP"; }
    
    float updatePlasticity(float preSpike, float postSpike,
                          float weight, float dt) override {
        if (preSpike > 0 && postSpike > 0) {
            // LTP
            return weight * std::exp(-abs(postSpike - preSpike) / tau_stdp);
        } else if (preSpike > 0 && postSpike < 0) {
            // LTD
            return weight * std::exp(-abs(postSpike - preSpike) / tau_stdp);
        }
        return weight;
    }
    
    // ... other required methods
};
```

## Integration with NLM

### Brain Configuration with Plugins

```cpp
class BrainWithPlugins {
private:
    std::shared_ptr<NeuronPluginRegistry> neuronRegistry;
    std::shared_ptr<PlasticityPluginRegistry> plasticityRegistry;
    std::shared_ptr<DynamicPluginLoader> pluginLoader;
    
public:
    bool initializeWithPlugins(const Config& config) {
        // Load plugins
        pluginLoader->loadAllPlugins();
        
        // Configure neuron models
        std::string neuronModelName = config.getOr<std::string>(
            "brain.neuron_model", "DefaultLIF");
        auto neuronModel = neuronRegistry->getNeuronModel(neuronModelName);
        if (!neuronModel) {
            // Fallback to default
            neuronRegistry->registerDefaultModels();
            neuronModel = neuronRegistry->getNeuronModel("DefaultLIF");
        }
        
        // Configure plasticity rules
        auto plasticityRule = plasticityRegistry->getPlasticityRule("STDP");
        if (plasticityRule) {
            Config plasticityConfig = plasticityRule->getDefaultConfig();
            plasticityConfig.set("learning_rate", 
                               config.getOr<float>("plasticity.learning_rate", 0.01f));
            plasticityRegistry->configureRule("STDP", plasticityConfig);
        }
        
        // Initialize brain with configured plugins
        return initializeCoreBrain(neuronModel, plasticityRule);
    }
};
```

### Experiment Plugin System

```cpp
class ExperimentPluginManager {
private:
    std::unordered_map<std::string, std::unique_ptr<IExperiment>> experiments;
    std::vector<std::string> availableExperiments;
    
public:
    // Experiment management
    bool registerExperiment(const std::string& name,
                           std::unique_ptr<IExperiment> experiment);
    IExperiment* getExperiment(const std::string& name);
    std::vector<std::string> getAvailableExperiments() const;
    
    // Experiment execution
    bool runExperiment(const std::string& name, const Config& config);
    void listAvailableExperiments() const;
    
    // Default experiments
    void registerDefaultExperiments();
};
```

## Building with Plugins

### CMake Integration

```cmake
# Enable plugins
option(NLM_ENABLE_PLUGINS "Build plugin system" ON)
option(NLM_ENABLE_DYNAMIC_PLUGINS "Enable dynamic plugin loading" ON)

if(NLM_ENABLE_PLUGINS)
    # Add plugin headers
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/plugins)
    
    # Build plugin library
    add_library(nlm_plugins STATIC
        src/plugins/PluginRegistry.cpp
        src/plugins/PluginLoader.cpp
        src/plugins/NeuronPluginRegistry.cpp
        src/plugins/PlasticityPluginRegistry.cpp
    )
    
    # Install plugins
    install(DIRECTORY src/plugins/ DESTINATION include/nlm/plugins)
endif()
```

### Plugin Examples in Source Tree

```
plugins/
├── examples/
│   ├── custom_neuron.cpp
│   ├── custom_stdp.cpp
│   ├── custom_experiment.cpp
│   └── neural_network.cpp
├── core/
│   ├── INeuronModel.hpp
│   ├── IPlasticityRule.hpp
│   ├── IExperiment.hpp
│   └── PluginBase.hpp
├── registry/
│   ├── NeuronPluginRegistry.hpp
│   ├── PlasticityPluginRegistry.hpp
│   └── ExperimentPluginRegistry.hpp
├── loader/
│   ├── DynamicPluginLoader.hpp
│   └── StaticPluginLoader.hpp
└── utils/
    ├── PluginUtils.hpp
    ├── PluginConfig.hpp
    └── PluginMessage.hpp
```

## Future Enhancements

### 1. Machine Learning Plugins

- TensorFlow/PyTorch integration plugins
- Custom neural network architectures
- Model compression plugins

### 2. Hardware Acceleration Plugins

- CUDA/GPU acceleration plugins
- OpenCL plugins
- SIMD optimization plugins

### 3. Visualization Plugins

- OpenGL visualization plugins
- Web-based visualization plugins
- Real-time streaming plugins

### 4. Cloud Integration Plugins

- AWS integration plugins
- GPU cloud plugins
- Distributed computing plugins

## Migration Guide

### From Current NLM to Plugin Architecture

1. **Identify Custom Components**: List all parts you want to customize
2. **Create Plugin Interfaces**: Define abstract base classes
3. **Implement Plugins**: Write concrete implementations
4. **Register Plugins**: Add to appropriate registries
5. **Configure Plugins**: Set up configuration files
6. **Test Integration**: Verify all plugins work together

### Example Migration: Custom Neuron Model

**Before:**
```cpp
// Custom neuron implementation in main.cpp
class MyNeuron {
    // Neuron implementation
};
```

**After:**
```cpp
// In custom_neuron.cpp
class CustomNeuron : public INeuronModel {
    // Plugin implementation
};

// In plugin registry
neuronRegistry->registerNeuronModel("CustomNeuron", 
                                    std::make_unique<CustomNeuron>());
```

## Performance Considerations

### Plugin Loading Overhead

- Dynamic plugins have startup overhead
- Consider caching frequently used plugins
- Use static linking for performance-critical components

### Memory Usage

- Plugin system adds memory overhead
- Monitor memory usage with large numbers of plugins
- Implement plugin lifecycle management

### Thread Safety

- Plugin registries should be thread-safe
- Consider concurrent access patterns
- Implement proper locking mechanisms

## Testing Plugin System

### Unit Tests

```bash
# Test plugin interfaces
./test_plugin_interfaces

# Test plugin registry
./test_plugin_registry

# Test plugin loading
./test_plugin_loader
```

### Integration Tests

```bash
# Test complete plugin ecosystem
./test_plugin_integration

# Test plugin compatibility
./test_plugin_compatibility

# Test plugin performance
./test_plugin_performance
```

## Plugin Development Best Practices

### 1. Interface Design

- Use pure virtual methods for interfaces
- Provide default implementations for common functionality
- Keep interfaces focused and minimal

### 2. Error Handling

- Implement proper error handling
- Use exception-safe resource management
- Provide detailed error messages

### 3. Configuration

- Use Config class for plugin configuration
- Provide sensible defaults
- Support runtime configuration changes

### 4. Documentation

- Document plugin interfaces clearly
- Provide examples for common use cases
- Include build instructions

### 5. Testing

- Write unit tests for plugin implementations
- Test plugin integration thoroughly
- Test plugin lifecycle management

## Conclusion

The NLM plugin architecture provides a flexible, extensible system for customizing and extending NLM functionality. By implementing the plugin interfaces and using the plugin registries, developers can:

- Add custom neuron models
- Create specialized plasticity rules
- Develop unique experiments
- Integrate with external systems
- Maintain backward compatibility

The plugin system is designed to be both powerful and easy to use, with clear interfaces and comprehensive tooling for development, testing, and deployment.