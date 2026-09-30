# NLM Plugin Examples

## Overview

This document provides examples of how to create and use plugins for the NLM system. The examples cover all plugin types and demonstrate best practices for plugin development.

## Example 1: Adaptive Neuron Plugin

### Overview
This plugin implements a custom neuronal type with adaptive properties that can adjust its behavior based on environmental feedback.

### Code Implementation

```cpp
// ExampleNeuronalPlugin.cpp
#include "include/nlm/plugin/INeuralPlugin.h"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/plasticity/PlasticityRule.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include <iostream>
#include <memory>

namespace nlm {
namespace plugin {

class ExampleNeuronalPlugin : public INeuralPlugin {
public:
    ExampleNeuronalPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleNeuronalPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleNeuronalPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example neuronal plugin implementing custom neuron types and plasticity rules";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::NEURAL};
    }

    std::vector<std::string> getDependencies() const override {
        return {};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleNeuronalPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleNeuronalPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Update neuronal components
        return true;
    }

    PluginState getState() const override {
        return state_;
    }

    bool isEnabled() const override {
        return enabled_;
    }

    bool enable() override {
        std::cout << "Enabling ExampleNeuronalPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleNeuronalPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "NeuronalPlugin";
    }

    std::string getID() const override {
        return "example_neuronal_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleNeuronalPlugin received event: " << eventName << std::endl;
    }

    std::string getNeuronTypeName() const override {
        return "AdaptiveNeuron";
    }

    std::shared_ptr<Neuron> createNeuron(double x, double y, double z) override {
        return std::make_shared<Neuron>(x, y, z);
    }

    std::string getPlasticityRuleName() const override {
        return "HomeostaticPlasticity";
    }

    std::shared_ptr<PlasticityRule> createPlasticityRule() override {
        return std::make_shared<PlasticityRule>();
    }

    void configureNeuralPopulation(NeuralPopulation& population) const override {
        population.setSize(100);
        population.setConnectionDensity(0.1);
    }

    bool configureNeuralDynamics(NeuralDynamics& dynamics) const override {
        dynamics.setTimeStep(0.001);
        dynamics.setMaxVoltage(100.0);
        dynamics.setResetVoltage(-70.0);
        return true;
    }

private:
    PluginState state_;
    bool enabled_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleNeuronalPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}
```

### Plugin Manifest

```json
{
  "name": "ExampleNeuronalPlugin",
  "version": "1.0.0",
  "author": "NLM Plugin Developer",
  "description": "Example neuronal plugin implementing custom neuron types and plasticity rules",
  "license": "MIT License",
  "pluginId": "example_neuronal_plugin_001",
  "entryPoint": "createPlugin",
  "platform": "cross-platform",
  "architecture": "x86_64",
  "dependencies": [],
  "capabilities": ["NEURAL"],
  "features": ["adaptive_neuron", "homeostatic_plasticity"],
  "optionalFeatures": [],
  "configSchema": {
    "neuron_type": {
      "type": "string",
      "default": "AdaptiveNeuron"
    },
    "plasticity_rule": {
      "type": "string",
      "default": "HomeostaticPlasticity"
    }
  },
  "defaultConfig": {
    "neuron_type": "AdaptiveNeuron",
    "plasticity_rule": "HomeostaticPlasticity"
  },
  "securityLevel": "SAFE",
  "compatibility": "COMPATIBLE",
  "requiredNlmVersion": ["1.0.0"],
  "createdDate": "2024-01-01",
  "lastModified": "2024-01-01"
}
```

### Configuration

```cpp
// Configure the plugin
std::map<std::string, std::string> config;
config["neuron_type"] = "AdaptiveNeuron";
config["plasticity_rule"] = "HomeostaticPlasticity";
config["max_neurons"] = "1000";
config["connection_density"] = "0.1";

pluginSystem.loadPlugin("/path/to/ExampleNeuronalPlugin.so");
pluginSystem.configurePlugin("example_neuronal_plugin_001", config);
pluginSystem.enablePlugin("example_neuronal_plugin_001");
```

## Example 2: Multi-Modal Sensory Plugin

### Overview
This plugin implements a multi-modal sensory system that can process input from multiple sensory modalities simultaneously.

### Code Implementation

```cpp
// ExampleSensoryPlugin.cpp
#include "include/nlm/plugin/ISensoryPlugin.h"
#include "src/sensory/SensoryInput.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

namespace nlm {
namespace plugin {

class ExampleSensoryPlugin : public ISensoryPlugin {
public:
    ExampleSensoryPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleSensoryPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleSensoryPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example sensory plugin implementing multi-modal sensory integration";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::SENSORY};
    }

    std::vector<std::string> getDependencies() const override {
        return {"VisionPlugin"};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleSensoryPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleSensoryPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Process sensory inputs
        for (auto& input : processedInputs_) {
            input.update(deltaTime);
        }
        return true;
    }

    PluginState getState() const override {
        return state_;
    }

    bool isEnabled() const override {
        return enabled_;
    }

    bool enable() override {
        std::cout << "Enabling ExampleSensoryPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleSensoryPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "SensoryPlugin";
    }

    std::string getID() const override {
        return "example_sensory_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleSensoryPlugin received event: " << eventName << std::endl;
    }

    std::string getSensoryModality() const override {
        return "MultiModal";
    }

    std::shared_ptr<SensoryInput> createSensoryInput() override {
        return std::make_shared<SensoryInput>();
    }

    std::vector<SensoryInput> processInput(const std::vector<SensoryInput>& inputs) override {
        std::vector<SensoryInput> processedInputs = inputs;

        for (auto& input : processedInputs) {
            input.setGain(input.getGain() * getInputGain());
            input.setNoise(input.getNoiseLevel() * getNoiseLevel());
        }

        processedInputs_.assign(processedInputs.begin(), processedInputs.end());
        return processedInputs;
    }

    void configureSensorySystem() override {
        std::cout << "Configuring multi-modal sensory system" << std::endl;
    }

    double getInputGain() const override {
        return 1.5;
    }

    double getNoiseLevel() const override {
        return 0.1;
    }

    std::map<std::string, double> getProcessingParameters() const override {
        return {
            {"threshold", 0.5},
            {"sensitivity", 2.0},
            {"integration_window", 0.1}
        };
    }

    void setProcessingParameters(const std::map<std::string, double>& params) override {
        processingParameters_ = params;
    }

private:
    PluginState state_;
    bool enabled_;
    std::vector<SensoryInput> processedInputs_;
    std::map<std::string, double> processingParameters_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleSensoryPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}
```

### Plugin Manifest

```json
{
  "name": "ExampleSensoryPlugin",
  "version": "1.0.0",
  "author": "NLM Plugin Developer",
  "description": "Example sensory plugin implementing multi-modal sensory integration",
  "license": "MIT License",
  "pluginId": "example_sensory_plugin_001",
  "entryPoint": "createPlugin",
  "platform": "cross-platform",
  "architecture": "x86_64",
  "dependencies": ["VisionPlugin"],
  "capabilities": ["SENSORY"],
  "features": ["multi_modal", "signal_processing", "noise_model"],
  "optionalFeatures": ["advanced_filtering"],
  "configSchema": {
    "modalities": {
      "type": "array",
      "default": ["vision", "audition", "proprioception"]
    },
    "integration_window": {
      "type": "double",
      "default": 0.1
    }
  },
  "defaultConfig": {
    "modalities": "vision,audition,proprioception",
    "integration_window": 0.1
  },
  "securityLevel": "CAUTION",
  "compatibility": "COMPATIBLE",
  "requiredNlmVersion": ["1.0.0"],
  "createdDate": "2024-01-01",
  "lastModified": "2024-01-01"
}
```

### Configuration

```cpp
// Configure the plugin
std::map<std::string, std::string> config;
config["modalities"] = "vision,audition,proprioception";
config["integration_window"] = "0.1";
config["input_gain"] = "1.5";
config["noise_level"] = "0.1";

pluginSystem.loadPlugin("/path/to/ExampleSensoryPlugin.so");
pluginSystem.configurePlugin("example_sensory_plugin_001", config);
pluginSystem.enablePlugin("example_sensory_plugin_001");
```

## Plugin Development Best Practices

### 1. Plugin Structure

- Follow the NLM plugin interface conventions
- Implement all required abstract methods
- Use proper error handling and logging
- Ensure thread-safe operations

### 2. Plugin Configuration

- Use configuration validation
- Provide sensible defaults
- Support plugin-specific configuration
- Document configuration options

### 3. Plugin Lifecycle

- Implement proper lifecycle management
- Handle initialization and shutdown correctly
- Manage plugin state transitions
- Support plugin enable/disable

### 4. Plugin Events

- Subscribe to relevant events
- Handle events properly
- Implement event filtering
- Monitor event statistics

### 5. Plugin Security

- Implement security checks
- Validate plugin capabilities
- Use proper sandboxing
- Follow security policies

### 6. Plugin Testing

- Write unit tests for plugin functionality
- Test plugin integration
- Test plugin performance
- Test plugin security

## Plugin Development Checklist

### Before Development
- [ ] Understand NLM plugin architecture
- [ ] Review plugin interface specifications
- [ ] Set up development environment
- [ ] Install plugin development tools
- [ ] Create plugin manifest template

### During Development
- [ ] Implement plugin base class
- [ ] Add plugin factory functions
- [ ] Implement plugin lifecycle methods
- [ ] Add plugin configuration
- [ ] Implement plugin event handling
- [ ] Test plugin functionality

### After Development
- [ ] Test plugin integration
- [ ] Validate plugin performance
- [ ] Test plugin security
- [ ] Document plugin usage
- [ ] Create plugin examples
- [ ] Update plugin documentation

## Conclusion

The NLM plugin architecture provides a comprehensive framework for extending the NLM system with custom modules. The examples demonstrate the full spectrum of plugin types and best practices for plugin development. By following these examples and best practices, developers can create powerful custom plugins for the NLM system while maintaining backward compatibility and providing a robust, extensible foundation for future plugin development.

The plugin system enables developers to create custom neuronal types, sensory modalities, motor commands, memory systems, neuromodulators, environments, experiments, and visualization capabilities, providing a complete solution for extending the NLM system.
