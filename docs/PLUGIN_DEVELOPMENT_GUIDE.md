# NLM Plugin Development Guide

## Overview
NLM's plugin development guide provides comprehensive instructions for developing plugins for the NLM system. This guide covers the plugin development process, API documentation, examples, and best practices.

## Plugin Development Process

### 1. Plugin Types

NLM supports four main plugin types:

#### Neuron Model Plugins
- Implement custom neuron dynamics and properties
- Support for biologically-inspired models
- Integration with spiking dynamics

#### Plasticity Rule Plugins
- Implement custom learning rules
- Support for STDP, Hebbian, and novel rules
- Neuromodulation integration

#### Synaptic Dynamics Plugins
- Custom synaptic properties and behaviors
- Short-term and long-term plasticity
- Activity-dependent modulation

#### Memory Plugins
- Custom memory system implementations
- Working memory and episodic memory
- Associative memory models

### 2. Plugin Development Workflow

#### Step 1: Understand Plugin Architecture
- Review plugin interfaces and APIs
- Understand plugin loading and management
- Learn about plugin configuration

#### Step 2: Choose Plugin Type
- Select the appropriate plugin type for your needs
- Define plugin requirements and specifications
- Design plugin interface

#### Step 3: Implement Plugin Interface
- Implement the plugin interface for your chosen type
- Add necessary methods and properties
- Ensure thread safety and error handling

#### Step 4: Implement Plugin Logic
- Implement plugin-specific logic
- Add configuration handling
- Implement plugin lifecycle management

#### Step 5: Build Plugin
- Create CMakeLists.txt for plugin building
- Set up plugin packaging
- Configure plugin deployment

#### Step 6: Test Plugin
- Write unit tests for plugin functionality
- Write integration tests for plugin interaction
- Test plugin with NLM system

#### Step 7: Package Plugin
- Package plugin for distribution
- Create plugin documentation
- Set up plugin repository

### 3. Plugin Development Tools

#### Plugin Builder
```bash
# Build plugin
nlm-build-plugin plugin_name

# Test plugin
nlm-test-plugin plugin_name

# Package plugin
nlm-package-plugin plugin_name
```

#### Plugin Debugger
```bash
# Debug plugin
nlm-debug-plugin plugin_name

# Profile plugin
nlm-profile-plugin plugin_name

# Analyze plugin
nlm-analyze-plugin plugin_name
```

#### Plugin Documentation Generator
```bash
# Generate plugin documentation
nlm-generate-plugin-docs plugin_name

# Generate plugin API documentation
nlm-generate-plugin-api-docs plugin_name
```

## Plugin Development Examples

### 1. Basic Neuron Model Plugin
```cpp
// Custom neuron model implementation
#include "INeuronModelPlugin.hpp"
#include <cmath>

class CustomNeuronModel : public INeuronModelPlugin {
public:
    std::string getName() const override {
        return "CustomNeuronModel";
    }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        try {
            membrane_tau = std::stof(parameters.at("membrane_tau"));
            membrane_threshold = std::stof(parameters.at("membrane_threshold"));
            membrane_reset = std::stof(parameters.at("membrane_reset"));
            resting_potential = std::stof(parameters.at("resting_potential"));
            gain = std::stof(parameters.at("gain"));
            spike_triggered_current = std::stof(parameters.at("spike_triggered_current"));
            
            enabled = true;
            error_message.clear();
            
            return true;
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomNeuronModel: " + std::string(e.what());
            return false;
        }
    }
    
    void update(float dt) override {
        if (refractory_timer > 0) {
            refractory_timer -= dt;
        }
    }
    
    float updateDynamics(const NeuronState& state, float dt) override {
        float dv = (-state.membranePotential + restingPotential) / membrane_tau;
        dv += currentInjection * gain;
        
        if (state.membranePotential > threshold) {
            dv += spike_triggered_current;
        }
        
        return dv * dt;
    }
    
    bool hasSpike() const override {
        return spike;
    }
    
    void resetSpike() override {
        spike = false;
    }
    
    float getRestingPotential() const override {
        return resting_potential;
    }
    
    float getThreshold() const override {
        return membrane_threshold;
    }
    
    float getMembraneTimeConstant() const override {
        return membrane_tau;
    }
    
    void resetState() override {
        state.membranePotential = resting_potential;
        state.spikeCount = 0;
        state.refractoryTime = 0.0f;
        spike = false;
        refractory_timer = 0.0;
    }
    
    NeuronState getState() const override {
        return state;
    }
    
    void setState(const NeuronState& newState) override {
        state = newState;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void enable() override {
        enabled = true;
    }
    
    void disable() override {
        enabled = false;
    }
    
    std::string getLastError() const override {
        return error_message;
    }
    
    void clearError() override {
        error_message.clear();
    }
    
private:
    bool enabled = false;
    std::string error_message;
    
    float membrane_tau = 0.01f;
    float membrane_threshold = 0.5f;
    float membrane_reset = 0.0f;
    float resting_potential = 0.0f;
    float gain = 1.0f;
    float spike_triggered_current = 0.1f;
    
    NeuronState state;
    bool spike = false;
    float refractory_timer = 0.0f;
};
```

```cpp
// Plugin factory function
NLM_PLUGIN_EXPORT INeuronModelPlugin* createPlugin() {
    return new CustomNeuronModel();
}

NLM_PLUGIN_EXPORT void destroyPlugin(INeuronModelPlugin* plugin) {
    delete plugin;
}
```

### 2. Basic Plasticity Rule Plugin
```cpp
// Custom plasticity rule implementation
#include "IPlasticityRulePlugin.hpp"
#include <cmath>

class CustomPlasticityRule : public IPlasticityRulePlugin {
public:
    std::string getName() const override {
        return "CustomPlasticityRule";
    }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        try {
            learning_rate = std::stof(parameters.at("learning_rate"));
            time_constant = std::stof(parameters.at("time_constant"));
            neuromodulation_factor = std::stof(parameters.at("neuromodulation_factor"));
            max_weight_change = std::stof(parameters.at("max_weight_change"));
            
            enabled = true;
            error_message.clear();
            
            return true;
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomPlasticityRule: " + std::string(e.what());
            return false;
        }
    }
    
    void update(float dt) override {
        // Update rule state
    }
    
    float computeWeightChange(const SynapseState& pre,
                             const SynapseState& post,
                             float dt) override {
        float pre_spike = pre.spikeRecent ? 1.0f : 0.0f;
        float post_spike = post.spikeRecent ? 1.0f : 0.0f;
        
        float weight_change = learning_rate * pre_spike * post_spike;
        weight_change *= (1.0 + neuromodulation_factor * dopamine_level);
        
        return std::clamp(weight_change, -max_weight_change, max_weight_change);
    }
    
    float getLearningRate() const override {
        return learning_rate;
    }
    
    float getTimeConstant() const override {
        return time_constant;
    }
    
    std::string getRuleType() const override {
        return "CustomHebbian";
    }
    
    void resetState() override {
        // Reset rule state
    }
    
    void setRuleParameters(const std::map<std::string, float>& parameters) override {
        for (const auto& param : parameters) {
            if (param.first == "learning_rate") learning_rate = param.second;
            else if (param.first == "time_constant") time_constant = param.second;
            else if (param.first == "neuromodulation_factor") neuromodulation_factor = param.second;
            else if (param.first == "max_weight_change") max_weight_change = param.second;
        }
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void enable() override {
        enabled = true;
    }
    
    void disable() override {
        enabled = false;
    }
    
    std::string getLastError() const override {
        return error_message;
    }
    
    void clearError() override {
        error_message.clear();
    }
    
private:
    bool enabled = false;
    std::string error_message;
    
    float learning_rate = 0.01f;
    float time_constant = 1.0f;
    float neuromodulation_factor = 1.0f;
    float max_weight_change = 0.1f;
    float dopamine_level = 0.0f;
};
```

```cpp
// Plugin factory function
NLM_PLUGIN_EXPORT IPlasticityRulePlugin* createPlugin() {
    return new CustomPlasticityRule();
}

NLM_PLUGIN_EXPORT void destroyPlugin(IPlasticityRulePlugin* plugin) {
    delete plugin;
}
```

### 3. Basic Synaptic Dynamics Plugin
```cpp
// Custom synaptic dynamics implementation
#include "ISynapticDynamicsPlugin.hpp"
#include <cmath>

class CustomSynapticDynamics : public ISynapticDynamicsPlugin {
public:
    std::string getName() const override {
        return "CustomSynapticDynamics";
    }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        try {
            initial_weight = std::stof(parameters.at("initial_weight"));
            weight_decay = std::stof(parameters.at("weight_decay"));
            conductance = std::stof(parameters.at("conductance"));
            reversal_potential = std::stof(parameters.at("reversal_potential"));
            refractory_modulation = std::stof(parameters.at("refractory_modulation"));
            
            enabled = true;
            active = false;
            error_message.clear();
            
            return true;
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomSynapticDynamics: " + std::string(e.what());
            return false;
        }
    }
    
    void update(float dt) override {
        if (refractory_timer > 0) {
            refractory_timer -= dt;
        }
    }
    
    float updateDynamics(const SynapticState& state, float dt) override {
        float current = conductance * (state.weight * (state.membranePotential - reversalPotential));
        float dw = -weightDecay * state.weight + current * dt;
        
        if (refractory_timer > 0) {
            dw *= refractory_modulation;
        }
        
        return dw;
    }
    
    bool isActive() const override {
        return active;
    }
    
    float getInitialWeight() const override {
        return initial_weight;
    }
    
    float getWeightDecay() const override {
        return weight_decay;
    }
    
    float getConductance() const override {
        return conductance;
    }
    
    void resetState() override {
        state.weight = initial_weight;
        state.conductance = conductance;
        state.reversalPotential = reversal_potential;
        active = false;
    }
    
    SynapticState getState() const override {
        return state;
    }
    
    void setState(const SynapticState& newState) override {
        state = newState;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void enable() override {
        enabled = true;
        active = true;
    }
    
    void disable() override {
        enabled = false;
        active = false;
    }
    
    std::string getLastError() const override {
        return error_message;
    }
    
    void clearError() override {
        error_message.clear();
    }
    
private:
    bool enabled = false;
    bool active = false;
    std::string error_message;
    
    float initial_weight = 0.1f;
    float weight_decay = 0.01f;
    float conductance = 1.0f;
    float reversal_potential = 0.0f;
    float refractory_modulation = 0.1f;
    
    SynapticState state;
    float refractory_timer = 0.0f;
};
```

```cpp
// Plugin factory function
NLM_PLUGIN_EXPORT ISynapticDynamicsPlugin* createPlugin() {
    return new CustomSynapticDynamics();
}

NLM_PLUGIN_EXPORT void destroyPlugin(ISynapticDynamicsPlugin* plugin) {
    delete plugin;
}
```

## Plugin Development Best Practices

### 1. Plugin Design Principles

#### Modularity
- Keep plugins focused on single responsibilities
- Use well-defined interfaces
- Minimize dependencies between plugins
- Implement clear initialization and cleanup

#### Performance
- Minimize overhead in critical paths
- Use efficient data structures
- Avoid unnecessary computations
- Cache frequently used values

#### Thread Safety
- Ensure thread-safe plugin operations
- Use proper synchronization
- Avoid race conditions
- Implement atomic operations where needed

### 2. Error Handling

#### Error Reporting
```cpp
class PluginError : public std::runtime_error {
public:
    PluginError(const std::string& message);
    PluginError(const std::string& pluginName, const std::string& message);
    
    const std::string& getPluginName() const;
    const std::string& getErrorCode() const;
    
private:
    std::string plugin_name_;
    std::string error_code_;
};
```

#### Error Handling Example
```cpp
bool loadPlugin(const std::string& pluginName, const std::string& pluginPath) {
    try {
        if (!loadLibrary(pluginPath)) {
            throw PluginError(pluginName, "Failed to load library");
        }
        
        PluginInterface* plugin = createPlugin();
        if (!plugin) {
            throw PluginError(pluginName, "Failed to create plugin instance");
        }
        
        if (!plugin->initialize(getDefaultParameters())) {
            throw PluginError(pluginName, plugin->getLastError());
        }
        
        storePlugin(pluginName, plugin);
        return true;
        
    } catch (const PluginError& e) {
        std::cerr << "Plugin error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return false;
    }
}
```

### 3. Plugin Testing

#### Unit Testing
```cpp
// test_custom_neuron_model.cpp
#include <gtest/gtest.h>
#include "CustomNeuronModel.hpp"

TEST(CustomNeuronModel, Initialization) {
    CustomNeuronModel plugin;
    std::map<std::string, std::string> parameters;
    parameters["membrane_tau"] = "0.01";
    parameters["membrane_threshold"] = "0.5";
    parameters["membrane_reset"] = "0.0";
    parameters["resting_potential"] = "0.0";
    parameters["gain"] = "1.0";
    parameters["spike_triggered_current"] = "0.1";
    
    EXPECT_TRUE(plugin.initialize(parameters));
    EXPECT_TRUE(plugin.isEnabled());
}

TEST(CustomNeuronModel, DynamicsUpdate) {
    CustomNeuronModel plugin;
    std::map<std::string, std::string> parameters;
    parameters["membrane_tau"] = "0.01";
    parameters["membrane_threshold"] = "0.5";
    parameters["membrane_reset"] = "0.0";
    parameters["resting_potential"] = "0.0";
    parameters["gain"] = "1.0";
    parameters["spike_triggered_current"] = "0.1";
    
    plugin.initialize(parameters);
    
    NeuronState state;
    state.membranePotential = 0.0f;
    state.spikeCount = 0;
    
    float result = plugin.updateDynamics(state, 0.001f);
    EXPECT_GE(result, -1.0f);
    EXPECT_LE(result, 1.0f);
}

TEST(CustomNeuronModel, SpikeReset) {
    CustomNeuronModel plugin;
    std::map<std::string, std::string> parameters;
    parameters["membrane_tau"] = "0.01";
    parameters["membrane_threshold"] = "0.5";
    parameters["membrane_reset"] = "0.0";
    parameters["resting_potential"] = "0.0";
    parameters["gain"] = "1.0";
    parameters["spike_triggered_current"] = "0.1";
    
    plugin.initialize(parameters);
    
    NeuronState state;
    state.membranePotential = 0.5f;
    
    plugin.updateDynamics(state, 0.001f);
    EXPECT_TRUE(plugin.hasSpike());
    
    plugin.resetSpike();
    EXPECT_FALSE(plugin.hasSpike());
}
```

#### Integration Testing
```cpp
// test_plugin_integration.cpp
#include <gtest/gtest.h>
#include "PluginManager.hpp"

class PluginIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<PluginManager>();
    }
    
    std::unique_ptr<PluginManager> manager;
};

TEST_F(PluginIntegrationTest, LoadAndEnablePlugin) {
    EXPECT_TRUE(manager->loadPlugin("test_plugin", "libtest_plugin.so"));
    EXPECT_TRUE(manager->enablePlugin("test_plugin"));
    EXPECT_TRUE(manager->initializePlugin("test_plugin", {}));
    
    PluginInterface* plugin = manager->getPlugin("test_plugin");
    EXPECT_NE(plugin, nullptr);
    EXPECT_TRUE(plugin->isEnabled());
}

TEST_F(PluginIntegrationTest, UpdateAllPlugins) {
    manager->loadPlugin("neuron_model", "libneuron_model.so");
    manager->loadPlugin("plasticity_rule", "libplasticity_rule.so");
    
    manager->enablePlugin("neuron_model");
    manager->enablePlugin("plasticity_rule");
    
    manager->initializePlugin("neuron_model", {});
    manager->initializePlugin("plasticity_rule", {});
    
    manager->updatePlugins(0.001f);
    
    // Verification logic
}
```

## Plugin Documentation

### Plugin API Documentation
```cpp
/**
 * @brief Custom neuron model plugin
 * 
 * This plugin implements a custom neuron model with adaptive firing rates
 * and subthreshold oscillations.
 */
template<typename T, typename U>
class CustomNeuronModel : public INeuronModelPlugin {
public:
    /**
     * @brief Constructor
     * 
     * @param parameters Plugin configuration parameters
     */
    CustomNeuronModel(const std::map<std::string, std::string>& parameters);
    
    /**
     * @brief Get plugin name
     * 
     * @return Plugin name
     */
    std::string getName() const override;
    
    /**
     * @brief Initialize plugin
     * 
     * @param parameters Plugin configuration parameters
     * @return True if initialization succeeded
     */
    bool initialize(const std::map<std::string, std::string>& parameters) override;
    
    /**
     * @brief Update neuron dynamics
     * 
     * @param state Neuron state
     * @param dt Time step
     * @return Change in neuron state
     */
    float updateDynamics(const NeuronState& state, float dt) override;
    
    /**
     * @brief Check if neuron has spiked
     * 
     * @return True if neuron has spiked
     */
    bool hasSpike() const override;
    
    /**
     * @brief Reset spike state
     */
    void resetSpike() override;
    
    /**
     * @brief Get resting potential
     * 
     * @return Resting potential
     */
    float getRestingPotential() const override;
    
    /**
     * @brief Get firing threshold
     * 
     * @return Firing threshold
     */
    float getThreshold() const override;
    
    /**
     * @brief Get membrane time constant
     * 
     * @return Membrane time constant
     */
    float getMembraneTimeConstant() const override;
    
    /**
     * @brief Reset neuron state
     */
    void resetState() override;
    
    /**
     * @brief Get current neuron state
     * 
     * @return Current neuron state
     */
    NeuronState getState() const override;
    
    /**
     * @brief Set neuron state
     * 
     * @param state New neuron state
     */
    void setState(const NeuronState& state) override;
    
    /**
     * @brief Check if plugin is enabled
     * 
     * @return True if plugin is enabled
     */
    bool isEnabled() const override;
    
    /**
     * @brief Enable plugin
     */
    void enable() override;
    
    /**
     * @brief Disable plugin
     */
    void disable() override;
    
    /**
     * @brief Get last error message
     * 
     * @return Last error message
     */
    std::string getLastError() const override;
    
    /**
     * @brief Clear error message
     */
    void clearError() override;
    
private:
    // Implementation details
};
```

## Plugin Configuration

### Plugin Configuration Schema
```yaml
plugin:
  name: "my_plugin"
  version: "1.0.0"
  description: "Custom plugin implementation"
  type: "neuron_model"
  
  parameters:
    membrane_tau:
      type: "float"
      default: 0.01
      min: 0.001
      max: 1.0
      description: "Membrane time constant"
```

### Plugin Configuration Example
```bash
# Configure plugin
nlm --plugin-config my_plugin:membrane_tau=0.02

# Configure with file
nlm --plugin-config-file my_plugin.cfg
```

## Plugin Deployment

### Plugin Installation
```bash
# Install system-wide
sudo cp libmy_plugin.so /usr/local/lib/
sudo ldconfig

# Install to user directory
cp libmy_plugin.so ~/.local/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/lib
```

### Plugin Testing
```bash
# Test plugin
ctest --output-on-failure

# Run benchmarks
./benchmark_plugin --plugin my_plugin

# Run integration tests
./integration_test --plugin my_plugin
```

## Plugin Development Tools

### Plugin Builder
```bash
# Build plugin
nlm-build-plugin my_plugin

# Test plugin
nlm-test-plugin my_plugin

# Package plugin
nlm-package-plugin my_plugin
```

### Plugin Debugger
```bash
# Debug plugin
nlm-debug-plugin my_plugin

# Profile plugin
nlm-profile-plugin my_plugin

# Analyze plugin
nlm-analyze-plugin my_plugin
```

### Plugin Documentation Generator
```bash
# Generate documentation
nlm-generate-plugin-docs my_plugin

# Generate API documentation
nlm-generate-plugin-api-docs my_plugin
```

## Plugin Migration Guide

### Migrating from v1.0 to v2.0

#### Breaking Changes
1. **Plugin Interface**: Changed from function pointers to classes
2. **Configuration**: Moved to structured format
3. **Error Handling**: Added comprehensive error handling
4. **Discovery**: Changed to directory-based discovery

#### Migration Steps
1. Update plugin interface
2. Update configuration format
3. Update error handling
4. Update discovery mechanism
5. Update build system

### Migration Example
```cpp
// Old plugin interface
void updateNeuronDynamics(NeuronState& state, float dt) {
    // Old implementation
}

// New plugin interface
class CustomNeuronModel : public INeuronModelPlugin {
public:
    float updateDynamics(const NeuronState& state, float dt) override {
        // New implementation
    }
};
```

## Plugin Support

### Getting Help
- **Documentation**: https://nlm.readthedocs.io/plugins
- **Issue Tracker**: https://github.com/nlm/plugins/issues
- **Forum**: https://forum.nlm.ai/plugins
- **Slack**: https://slack.nlm.ai/plugins

### Plugin Examples
- **Neuron Models**: https://github.com/nlm/plugin-examples/neuron-models
- **Plasticity Rules**: https://github.com/nlm/plugin-examples/plasticity-rules
- **Synaptic Dynamics**: https://github.com/nlm/plugin-examples/synaptic-dynamics

### Plugin Development Services
- **Consulting**: Professional plugin development
- **Support**: Priority support for enterprise
- **Training**: Plugin development workshops
- **Review**: Code review and optimization

## Plugin Future Roadmap

### Short-term (1-6 months)
- GPU-accelerated plugins
- Plugin versioning
- Plugin repository
- Improved documentation

### Medium-term (6-18 months)
- Plugin hot-swapping
- Plugin sandboxing
- Plugin testing framework
- Performance monitoring

### Long-term (18-36 months)
- Plugin marketplace
- AI-assisted development
- Plugin virtualization
- Plugin ecosystem
