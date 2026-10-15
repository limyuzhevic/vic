# NLM Plugin Development API

## Overview
NLM's plugin development API provides a comprehensive interface for creating custom neural models, plasticity rules, and other components. This guide covers the plugin development process, API documentation, and best practices.

## Plugin Development Process

### 1. Plugin Structure

A typical plugin consists of:

#### Core Plugin Files
```
plugin_name/
├── include/plugin_name/
│   ├── PluginInterface.hpp
│   ├── NeuronModelPlugin.hpp (optional)
│   ├── PlasticityRulePlugin.hpp (optional)
│   └── SynapticDynamicsPlugin.hpp (optional)
├── src/plugin_name/
│   ├── PluginManager.cpp
│   ├── NeuronModelPlugin.cpp (optional)
│   ├── PlasticityRulePlugin.cpp (optional)
│   └── SynapticDynamicsPlugin.cpp (optional)
└── CMakeLists.txt
```

#### Plugin Directory Structure
- **include/**: Header files with plugin interfaces
- **src/**: Implementation files
- **CMakeLists.txt**: Build configuration

### 2. Plugin Interface Implementation

#### Base Plugin Interface
```cpp
// include/nlm/plugin/PluginInterface.hpp
#pragma once

#include <string>
#include <map>

namespace nlm {

class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::string>& parameters) = 0;
    virtual void update(float dt) = 0;
    virtual bool isEnabled() const = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    
    // Error handling
    virtual std::string getLastError() const = 0;
    virtual void clearError() = 0;
};

} // namespace nlm
```

#### Neuron Model Plugin Interface
```cpp
// include/nlm/plugin/NeuronModelPlugin.hpp
#pragma once

#include "PluginInterface.hpp"
#include "NeuronState.hpp"

namespace nlm {

class INeuronModelPlugin : public PluginInterface {
public:
    virtual ~INeuronModelPlugin() = default;
    
    // Neuron dynamics
    virtual float updateDynamics(const NeuronState& state, float dt) = 0;
    virtual bool hasSpike() const = 0;
    virtual void resetSpike() = 0;
    
    // Neuron properties
    virtual float getRestingPotential() const = 0;
    virtual float getThreshold() const = 0;
    virtual float getMembraneTimeConstant() const = 0;
    
    // State management
    virtual void resetState() = 0;
    virtual NeuronState getState() const = 0;
    virtual void setState(const NeuronState& state) = 0;
};

} // namespace nlm
```

#### Plasticity Rule Plugin Interface
```cpp
// include/nlm/plugin/PlasticityRulePlugin.hpp
#pragma once

#include "PluginInterface.hpp"
#include "SynapseState.hpp"

namespace nlm {

class IPlasticityRulePlugin : public PluginInterface {
public:
    virtual ~IPlasticityRulePlugin() = default;
    
    // Plasticity computation
    virtual float computeWeightChange(const SynapseState& pre,
                                     const SynapseState& post,
                                     float dt) = 0;
    
    // Rule parameters
    virtual float getLearningRate() const = 0;
    virtual float getTimeConstant() const = 0;
    virtual std::string getRuleType() const = 0;
    
    // State management
    virtual void resetState() = 0;
    virtual void setRuleParameters(const std::map<std::string, float>& parameters) = 0;
};

} // namespace nlm
```

#### Synaptic Dynamics Plugin Interface
```cpp
// include/nlm/plugin/SynapticDynamicsPlugin.hpp
#pragma once

#include "PluginInterface.hpp"
#include "SynapticState.hpp"

namespace nlm {

class ISynapticDynamicsPlugin : public PluginInterface {
public:
    virtual ~ISynapticDynamicsPlugin() = default;
    
    // Synaptic dynamics
    virtual float updateDynamics(const SynapticState& state, float dt) = 0;
    virtual bool isActive() const = 0;
    
    // Synaptic properties
    virtual float getInitialWeight() const = 0;
    virtual float getWeightDecay() const = 0;
    virtual float getConductance() const = 0;
    
    // State management
    virtual void resetState() = 0;
    virtual SynapticState getState() const = 0;
    virtual void setState(const SynapticState& state) = 0;
};

} // namespace nlm
```

## Plugin Implementation Examples

### 1. Basic Neuron Model Plugin
```cpp
// include/my_neuron_plugin/CustomNeuronModel.hpp
#pragma once

#include "INeuronModelPlugin.hpp"

namespace nlm {

class CustomNeuronModel : public INeuronModelPlugin {
public:
    std::string getName() const override { return "CustomNeuronModel"; }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        // Initialize from parameters
        try {
            membrane_tau = std::stof(parameters.at("membrane_tau"));
            membrane_threshold = std::stof(parameters.at("membrane_threshold"));
            membrane_reset = std::stof(parameters.at("membrane_reset"));
            resting_potential = std::stof(parameters.at("resting_potential"));
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomNeuronModel: " + std::string(e.what());
            return false;
        }
        return true;
    }
    
    void update(float dt) override {
        // Update neuron state
        if (refractory_timer > 0) {
            refractory_timer -= dt;
        }
    }
    
    float updateDynamics(const NeuronState& state, float dt) override {
        // Custom LIF dynamics
        float dv = (-state.membranePotential + restingPotential) / membrane_tau;
        dv += currentInjection * gain;
        
        // Add custom nonlinearities
        if (state.membranePotential > threshold) {
            dv += spike_triggered_current;
        }
        
        return dv * dt;
    }
    
    bool hasSpike() const override { return spike; }
    void resetSpike() override { spike = false; }
    
    float getRestingPotential() const override { return resting_potential; }
    float getThreshold() const override { return membrane_threshold; }
    float getMembraneTimeConstant() const override { return membrane_tau; }
    
    void resetState() override {
        state.membranePotential = resting_potential;
        state.spikeCount = 0;
        state.refractoryTime = 0.0f;
        spike = false;
        refractory_timer = 0.0;
    }
    
    NeuronState getState() const override { return state; }
    void setState(const NeuronState& newState) override { state = newState; }
    
    bool isEnabled() const override { return enabled; }
    void enable() override { enabled = true; }
    void disable() override { enabled = false; }
    
    std::string getLastError() const override { return error_message; }
    void clearError() override { error_message.clear(); }
    
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

} // namespace nlm
```

```cpp
// src/my_neuron_plugin/CustomNeuronModel.cpp
#include "CustomNeuronModel.hpp"

namespace nlm {

// Register plugin
NLM_PLUGIN_EXPORT INeuronModelPlugin* createPlugin() {
    return new CustomNeuronModel();
}

NLM_PLUGIN_EXPORT void destroyPlugin(INeuronModelPlugin* plugin) {
    delete plugin;
}

} // namespace nlm
```

### 2. Plasticity Rule Plugin
```cpp
// include/my_plasticity_plugin/CustomPlasticityRule.hpp
#pragma once

#include "IPlasticityRulePlugin.hpp"

namespace nlm {

class CustomPlasticityRule : public IPlasticityRulePlugin {
public:
    std::string getName() const override { return "CustomPlasticity"; }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        // Initialize from parameters
        try {
            learning_rate = std::stof(parameters.at("learning_rate"));
            time_constant = std::stof(parameters.at("time_constant"));
            neuromodulation_factor = std::stof(parameters.at("neuromodulation_factor"));
            max_weight_change = std::stof(parameters.at("max_weight_change"));
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomPlasticityRule: " + std::string(e.what());
            return false;
        }
        return true;
    }
    
    void update(float dt) override {
        // Update rule state
    }
    
    float computeWeightChange(const SynapseState& pre,
                             const SynapseState& post,
                             float dt) override {
        // Implement custom learning rule
        float pre_spike = pre.spikeRecent ? 1.0f : 0.0f;
        float post_spike = post.spikeRecent ? 1.0f : 0.0f;
        
        // Custom Hebbian-like rule with additional factors
        float weight_change = learning_rate * pre_spike * post_spike;
        
        // Add modulation by neuromodulators
        weight_change *= (1.0 + neuromodulation_factor * dopamine_level);
        
        // Apply bounds
        return std::clamp(weight_change, -max_weight_change, max_weight_change);
    }
    
    float getLearningRate() const override { return learning_rate; }
    float getTimeConstant() const override { return time_constant; }
    std::string getRuleType() const override { return "CustomHebbian"; }
    
    void resetState() override {
        // Reset rule state
    }
    
    void setRuleParameters(const std::map<std::string, float>& parameters) override {
        // Update rule parameters
        for (const auto& param : parameters) {
            if (param.first == "learning_rate") learning_rate = param.second;
            else if (param.first == "time_constant") time_constant = param.second;
            else if (param.first == "neuromodulation_factor") neuromodulation_factor = param.second;
            else if (param.first == "max_weight_change") max_weight_change = param.second;
        }
    }
    
    bool isEnabled() const override { return enabled; }
    void enable() override { enabled = true; }
    void disable() override { enabled = false; }
    
    std::string getLastError() const override { return error_message; }
    void clearError() override { error_message.clear(); }
    
private:
    bool enabled = false;
    std::string error_message;
    float learning_rate = 0.01f;
    float time_constant = 1.0f;
    float neuromodulation_factor = 1.0f;
    float max_weight_change = 0.1f;
    float dopamine_level = 0.0f;
};

} // namespace nlm
```

```cpp
// src/my_plasticity_plugin/CustomPlasticityRule.cpp
#include "CustomPlasticityRule.hpp"

namespace nlm {

// Register plugin
NLM_PLUGIN_EXPORT IPlasticityRulePlugin* createPlugin() {
    return new CustomPlasticityRule();
}

NLM_PLUGIN_EXPORT void destroyPlugin(IPlasticityRulePlugin* plugin) {
    delete plugin;
}

} // namespace nlm
```

### 3. Synaptic Dynamics Plugin
```cpp
// include/my_synaptic_plugin/CustomSynapticDynamics.hpp
#pragma once

#include "ISynapticDynamicsPlugin.hpp"

namespace nlm {

class CustomSynapticDynamics : public ISynapticDynamicsPlugin {
public:
    std::string getName() const override { return "CustomSynapticDynamics"; }
    
    bool initialize(const std::map<std::string, std::string>& parameters) override {
        // Initialize from parameters
        try {
            initial_weight = std::stof(parameters.at("initial_weight"));
            weight_decay = std::stof(parameters.at("weight_decay"));
            conductance = std::stof(parameters.at("conductance"));
            reversal_potential = std::stof(parameters.at("reversal_potential"));
        } catch (const std::exception& e) {
            error_message = "Failed to initialize CustomSynapticDynamics: " + std::string(e.what());
            return false;
        }
        return true;
    }
    
    void update(float dt) override {
        // Update synaptic state
        if (refractory_timer > 0) {
            refractory_timer -= dt;
        }
    }
    
    float updateDynamics(const SynapticState& state, float dt) override {
        // Custom synaptic dynamics
        float current = conductance * (state.weight * (state.membranePotential - reversal_potential));
        float dw = -weight_decay * state.weight + current * dt;
        
        // Apply refractory period
        if (refractory_timer > 0) {
            dw *= refractory_modulation;
        }
        
        return dw;
    }
    
    bool isActive() const override { return active; }
    
    float getInitialWeight() const override { return initial_weight; }
    float getWeightDecay() const override { return weight_decay; }
    float getConductance() const override { return conductance; }
    
    void resetState() override {
        state.weight = initial_weight;
        state.conductance = conductance;
        state.reversalPotential = reversal_potential;
        active = false;
    }
    
    SynapticState getState() const override { return state; }
    void setState(const SynapticState& newState) override { state = newState; }
    
    bool isEnabled() const override { return enabled; }
    void enable() override { enabled = true; active = true; }
    void disable() override { enabled = false; active = false; }
    
    std::string getLastError() const override { return error_message; }
    void clearError() override { error_message.clear(); }
    
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

} // namespace nlm
```

```cpp
// src/my_synaptic_plugin/CustomSynapticDynamics.cpp
#include "CustomSynapticDynamics.hpp"

namespace nlm {

// Register plugin
NLM_PLUGIN_EXPORT ISynapticDynamicsPlugin* createPlugin() {
    return new CustomSynapticDynamics();
}

NLM_PLUGIN_EXPORT void destroyPlugin(ISynapticDynamicsPlugin* plugin) {
    delete plugin;
}

} // namespace nlm
```

## Plugin Manager Implementation

### Plugin Manager Core
```cpp
// include/nlm/plugin/PluginManager.hpp
#pragma once

#include <string>
#include <map>
#include <vector>
#include "PluginInterface.hpp"

namespace nlm {

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginName, const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin management
    PluginInterface* getPlugin(const std::string& pluginName) const;
    std::vector<std::string> getPluginNames() const;
    std::vector<std::string> getPluginNames(PluginType type) const;
    
    // Plugin lifecycle
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool initializePlugin(const std::string& pluginName, const std::map<std::string, std::string>& parameters);
    void updatePlugins(float dt);
    
    // Plugin information
    PluginInfo getPluginInfo(const std::string& pluginName) const;
    std::string getPluginVersion(const std::string& pluginName) const;
    std::string getPluginDescription(const std::string& pluginName) const;
    
    // Plugin configuration
    void setPluginConfig(const std::string& pluginName, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    
    // Plugin dependencies
    bool addPluginDependency(const std::string& pluginName, const std::string& dependencyName);
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    bool resolveDependencies();
    
private:
    struct PluginEntry {
        PluginInterface* plugin;
        std::string path;
        PluginType type;
        bool enabled;
        std::map<std::string, std::string> config;
        std::vector<std::string> dependencies;
        PluginInfo info;
    };
    
    std::map<std::string, PluginEntry> plugins_;
    std::map<std::string, std::vector<std::string>> dependencies_;
    bool initialized_;
};

} // namespace nlm
```

```cpp
// src/plugin/PluginManager.cpp
#include "PluginManager.hpp"
#include <dlfcn.h>
#include <iostream>

namespace nlm {

PluginManager::PluginManager() : initialized_(false) {}

PluginManager::~PluginManager() {
    for (auto& pair : plugins_) {
        delete pair.second.plugin;
    }
}

bool PluginManager::loadPlugin(const std::string& pluginName, const std::string& pluginPath) {
    // Check if plugin already loaded
    if (plugins_.find(pluginName) != plugins_.end()) {
        return false;
    }
    
    // Load library
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load plugin " << pluginName << ": " << dlerror() << std::endl;
        return false;
    }
    
    // Get plugin factory function
    typedef PluginInterface*(*CreatePluginFunc)();
    CreatePluginFunc createPlugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
    if (!createPlugin) {
        std::cerr << "Failed to find createPlugin function in plugin " << pluginName << std::endl;
        dlclose(handle);
        return false;
    }
    
    // Create plugin instance
    PluginInterface* plugin = createPlugin();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance " << pluginName << std::endl;
        dlclose(handle);
        return false;
    }
    
    // Get plugin info
    PluginInfo info;
    info.name = pluginName;
    info.version = plugin->getVersion();
    info.description = plugin->getDescription();
    
    // Store plugin
    PluginEntry entry;
    entry.plugin = plugin;
    entry.path = pluginPath;
    entry.type = detectPluginType(plugin);
    entry.enabled = false;
    plugins_[pluginName] = entry;
    
    return true;
}

// Additional implementation details...

} // namespace nlm
```

## Plugin Build System

### CMake Plugin Support
```cmake
# CMakeLists.txt for plugin
project(my_plugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PLUGIN_NAME "my_plugin")

# Plugin library
add_library(${PLUGIN_NAME} SHARED
    src/${PLUGIN_NAME}.cpp
)

target_include_directories(${PLUGIN_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Export plugin for NLM
target_compile_definitions(${PLUGIN_NAME} PRIVATE
    NLM_PLUGIN_EXPORT
    PLUGIN_NAME="${PLUGIN_NAME}"
)

# Install rules
install(TARGETS ${PLUGIN_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
```

### Plugin Discovery
```cpp
// include/nlm/plugin/PluginDiscovery.hpp
#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

class PluginDiscovery {
public:
    PluginDiscovery();
    ~PluginDiscovery();
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins(const std::string& directory) const;
    std::vector<PluginInfo> discoverPlugins(const std::vector<std::string>& directories) const;
    
    // Plugin filtering
    std::vector<PluginInfo> filterPlugins(const std::vector<PluginInfo>& plugins,
                                         const std::map<std::string, std::string>& filters) const;
    
    // Plugin metadata
    PluginInfo getPluginInfo(const std::string& pluginPath) const;
    bool validatePlugin(const PluginInfo& info) const;
    
private:
    std::vector<std::string> searchPaths_;
};

} // namespace nlm
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
        // Load plugin
        if (!loadLibrary(pluginPath)) {
            throw PluginError(pluginName, "Failed to load library");
        }
        
        // Create plugin instance
        PluginInterface* plugin = createPlugin();
        if (!plugin) {
            throw PluginError(pluginName, "Failed to create plugin instance");
        }
        
        // Initialize plugin
        if (!plugin->initialize(getDefaultParameters())) {
            throw PluginError(pluginName, plugin->getLastError());
        }
        
        // Store plugin
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
// test_plugin.cpp
#include <gtest/gtest.h>
#include "CustomNeuronModel.hpp"

TEST(CustomNeuronModel, Initialization) {
    CustomNeuronModel plugin;
    EXPECT_TRUE(plugin.initialize(getTestParameters()));
    EXPECT_TRUE(plugin.isEnabled());
}

TEST(CustomNeuronModel, DynamicsUpdate) {
    CustomNeuronModel plugin;
    plugin.initialize(getTestParameters());
    
    NeuronState state;
    state.membranePotential = 0.0f;
    state.spikeCount = 0;
    
    float result = plugin.updateDynamics(state, 0.001f);
    EXPECT_GE(result, -1.0f);
    EXPECT_LE(result, 1.0f);
}

TEST(CustomNeuronModel, SpikeReset) {
    CustomNeuronModel plugin;
    plugin.initialize(getTestParameters());
    
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
    // Load multiple plugins
    manager->loadPlugin("neuron_model", "libneuron_model.so");
    manager->loadPlugin("plasticity_rule", "libplasticity_rule.so");
    
    // Enable plugins
    manager->enablePlugin("neuron_model");
    manager->enablePlugin("plasticity_rule");
    
    // Initialize plugins
    manager->initializePlugin("neuron_model", {});
    manager->initializePlugin("plasticity_rule", {});
    
    // Update plugins
    manager->updatePlugins(0.001f);
    
    // Verify plugins are updated
    // (implementation-specific checks)
}
```

## Plugin Documentation

### Plugin API Documentation
```cpp
/**
 * @brief Custom neuron model plugin
 * 
 * This plugin implements a custom neuron model with adaptive firing rates
 * and subthreshold oscillations. The model combines aspects of the
 * Leaky Integrate-and-Fire (LIF) and Adaptive Exponential Integrate-and-Fire (AdEx)
 * models to provide a more biophysically realistic representation of neural dynamics.
 * 
 * @tparam T Neuron state type
 * @tparam U Neuron parameters type
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
     * @return True if initialization succeeded, false otherwise
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
     * @return True if neuron has spiked, false otherwise
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
     * @return True if plugin is enabled, false otherwise
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
  description: "Custom neuron model plugin"
  type: "neuron_model"
  
  parameters:
    membrane_tau:
      type: "float"
      default: 0.01
      min: 0.001
      max: 1.0
      description: "Membrane time constant"
      
    membrane_threshold:
      type: "float"
      default: 0.5
      min: 0.0
      max: 1.0
      description: "Firing threshold"
      
    membrane_reset:
      type: "float"
      default: 0.0
      min: -1.0
      max: 0.0
      description: "Reset potential"
      
    resting_potential:
      type: "float"
      default: 0.0
      min: -1.0
      max: 1.0
      description: "Resting potential"
      
    gain:
      type: "float"
      default: 1.0
      min: 0.0
      max: 10.0
      description: "Input gain factor"
      
    spike_triggered_current:
      type: "float"
      default: 0.1
      min: 0.0
      max: 1.0
      description: "Spike-triggered current"
```

### Plugin Configuration Example
```bash
# Configure plugin with default parameters
nlm --plugin-config my_plugin

# Configure plugin with custom parameters
nlm --plugin-config my_plugin:membrane_tau=0.02:membrane_threshold=0.6

# Configure plugin with file
nlm --plugin-config-file my_plugin.cfg
```

## Plugin Deployment

### Plugin Installation
```bash
# Install plugin system-wide
sudo cp libmy_plugin.so /usr/local/lib/
sudo ldconfig

# Install plugin to user directory
cp libmy_plugin.so ~/.local/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/lib
```

### Plugin Testing
```bash
# Test plugin
ctest --output-on-failure

# Run plugin benchmarks
./benchmark_plugin --plugin my_plugin

# Run plugin integration tests
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

## Plugin Migration Guide

### Migrating from v1.0 to v2.0

#### Breaking Changes
1. **Plugin Interface**: Changed from simple function pointers to classes
2. **Configuration**: Moved from key-value pairs to structured configuration
3. **Error Handling**: Added comprehensive error handling
4. **Plugin Discovery**: Changed from file-based to directory-based discovery

#### Migration Steps
1. Update plugin interface implementation
2. Update plugin configuration format
3. Update error handling code
4. Update plugin discovery mechanism
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
- **Neuron Model Examples**: https://github.com/nlm/plugin-examples/neuron-models
- **Plasticity Rule Examples**: https://github.com/nlm/plugin-examples/plasticity-rules
- **Synaptic Dynamics Examples**: https://github.com/nlm/plugin-examples/synaptic-dynamics

### Plugin Development Services
- **Consulting**: Professional plugin development services
- **Support**: Priority support for enterprise customers
- **Training**: Plugin development training and workshops
- **Review**: Plugin code review and optimization

## Plugin Future Roadmap

### Short-term (1-6 months)
- Add support for GPU-accelerated plugins
- Implement plugin versioning
- Add plugin repository support
- Improve plugin documentation

### Medium-term (6-18 months)
- Add plugin hot-swapping capability
- Implement plugin sandboxing
- Add plugin testing framework
- Improve plugin performance monitoring

### Long-term (18-36 months)
- Add plugin marketplace
- Implement plugin AI-assisted development
- Add plugin virtualization
- Create plugin ecosystem
