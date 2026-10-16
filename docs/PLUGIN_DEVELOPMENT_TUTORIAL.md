"""
Plugin Development Tutorial
===========================

This tutorial guides you through developing custom plugins for the NLM framework.
It covers the complete process from plugin design to implementation and usage.

## Overview

This tutorial will show you how to create a custom neuron plugin, a custom
synaptic plasticity plugin, and integrate them into the NLM system.

## Prerequisites

- Basic understanding of NLM architecture
- C++ programming knowledge
- Understanding of neural dynamics and plasticity
- Familiarity with pybind11 for Python integration

## Step 1: Understanding the Plugin Interface

### Plugin Base Classes

The NLM plugin system provides base interfaces for different plugin types:

#### Neuron Plugin Interface
```cpp
class NeuronPlugin {
public:
    virtual ~NeuronPlugin() = default;
    virtual void initialize(const NeuronParams& params) = 0;
    virtual void update(float dt) = 0;
    virtual void onSpike() = 0;
    virtual float getPotential() const = 0;
    virtual bool shouldSpike() const = 0;
    virtual void setParameters(const std::unordered_map<std::string, float>& params) = 0;
};
```

#### Synapse Plugin Interface
```cpp
class SynapsePlugin {
public:
    virtual ~SynapsePlugin() = default;
    virtual void initialize(const SynapseParams& params) = 0;
    virtual void onPreSpike(float pre_weight) = 0;
    virtual void onPostSpike(float post_weight) = 0;
    virtual float getWeight() const = 0;
    virtual void setWeight(float weight) = 0;
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;
};
```

#### Plasticity Rule Interface
```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    virtual void initialize(const PlasticityParams& params) = 0;
    virtual float calculateChange(float pre_spike_time, float post_spike_time) = 0;
    virtual void applyModulation(float modulation_factor) = 0;
};
```

## Step 2: Creating a Custom Neuron Plugin

### 2.1 Design Your Neuron Plugin

Let's create a **BiophysicalNeuronPlugin** that implements a more realistic
neuron model with ion channels, rather than the simplified LIF model.

### 2.2 Implement the Plugin

Create the custom neuron plugin file:

```cpp
// CustomNeuronPlugin.hpp
#pragma once

#include "NeuronPlugin.hpp"
#include "Types.hpp"

class BiophysicalNeuronPlugin : public NeuronPlugin {
private:
    // Membrane properties
    float membrane_potential_;
    float threshold_;
    float resting_potential_;
    float membrane_capacitance_;
    
    // Ion channels
    struct IonChannel {
        float conductance;
        float reversal_potential;
        float activation;
        float inactivation;
        float activation_time_constant;
        float inactivation_time_constant;
    };
    
    std::vector<IonChannel> sodium_channels_;
    std::vector<IonChannel> potassium_channels_;
    std::vector<IonChannel> leak_channels_;
    
    // State variables
    float sodium_activation_;
    float sodium_inactivation_;
    float potassium_activation_;
    float membrane_current_;
    
    // Custom parameters
    float calcium_concentration_;
    float sodium_concentration_;
    float potassium_concentration_;
    
    // Input currents
    float injected_current_;
    float synaptic_current_;
    
public:
    BiophysicalNeuronPlugin(const std::string& name);
    ~BiophysicalNeuronPlugin() override = default;
    
    void initialize(const NeuronParams& params) override;
    void update(float dt) override;
    void onSpike() override;
    float getPotential() const override;
    bool shouldSpike() const override;
    void setParameters(const std::unordered_map<std::string, float>& params) override;
    
    // Custom methods
    void setInjectedCurrent(float current);
    void addSynapticCurrent(float current);
    float getCalciumConcentration() const;
    float getSodiumConcentration() const;
    float getPotassiumConcentration() const;
    
    // Getters for specific ion channels
    float getSodiumConductance() const;
    float getPotassiumConductance() const;
    float getLeakConductance() const;
};
```

### 2.3 Implementation Details

```cpp
// CustomNeuronPlugin.cpp
#include "BiophysicalNeuronPlugin.hpp"
#include <algorithm>
#include <numeric>

BiophysicalNeuronPlugin::BiophysicalNeuronPlugin(const std::string& name) {
    setName(name);
}

void BiophysicalNeuronPlugin::initialize(const NeuronParams& params) {
    // Initialize membrane properties
    threshold_ = params.threshold;
    resting_potential_ = params.resting_potential;
    membrane_capacitance_ = params.membrane_capacitance;
    membrane_potential_ = resting_potential_;
    
    // Initialize ion channels with default values
    int num_channels = params.neuron_parameters.count("num_channels") ? 
        static_cast<int>(params.neuron_parameters.at("num_channels")) : 100;
    
    // Sodium channels (fast spiking)
    sodium_channels_.resize(num_channels);
    for (auto& channel : sodium_channels_) {
        channel.conductance = params.neuron_parameters.count("sodium_conductance") ? 
            params.neuron_parameters.at("sodium_conductance") : 120.0f;
        channel.reversal_potential = params.neuron_parameters.count("sodium_reversal_potential") ? 
            params.neuron_parameters.at("sodium_reversal_potential") : 50.0f;
        channel.activation_time_constant = params.neuron_parameters.count("sodium_activation_tau") ? 
            params.neuron_parameters.at("sodium_activation_tau") : 0.1f;
        channel.inactivation_time_constant = params.neuron_parameters.count("sodium_inactivation_tau") ? 
            params.neuron_parameters.at("sodium_inactivation_tau") : 5.0f;
        channel.activation = 0.0f;
        channel.inactivation = 1.0f;
    }
    
    // Potassium channels (delayed rectifier)
    potassium_channels_.resize(num_channels);
    for (auto& channel : potassium_channels_) {
        channel.conductance = params.neuron_parameters.count("potassium_conductance") ? 
            params.neuron_parameters.at("potassium_conductance") : 36.0f;
        channel.reversal_potential = params.neuron_parameters.count("potassium_reversal_potential") ? 
            params.neuron_parameters.at("potassium_reversal_potential") : -77.0f;
        channel.activation_time_constant = params.neuron_parameters.count("potassium_activation_tau") ? 
            params.neuron_parameters.at("potassium_activation_tau") : 1.0f;
        channel.inactivation_time_constant = 0.0f;  // No inactivation for delayed rectifier
        channel.activation = 0.0f;
        channel.inactivation = 1.0f;
    }
    
    // Leak channels
    leak_channels_.resize(num_channels);
    for (auto& channel : leak_channels_) {
        channel.conductance = params.neuron_parameters.count("leak_conductance") ? 
            params.neuron_parameters.at("leak_conductance") : 0.3f;
        channel.reversal_potential = params.neuron_parameters.count("leak_reversal_potential") ? 
            params.neuron_parameters.at("leak_reversal_potential") : -54.387f;
        channel.activation_time_constant = 1.0f;  // Instant activation
        channel.inactivation_time_constant = 0.0f;
        channel.activation = 1.0f;
        channel.inactivation = 1.0f;
    }
    
    // Initialize concentrations
    calcium_concentration_ = params.neuron_parameters.count("initial_calcium") ? 
        params.neuron_parameters.at("initial_calcium") : 0.0001f;
    sodium_concentration_ = params.neuron_parameters.count("initial_sodium") ? 
        params.neuron_parameters.at("initial_sodium") : 0.001f;
    potassium_concentration_ = params.neuron_parameters.count("initial_potassium") ? 
        params.neuron_parameters.at("initial_potassium") : 0.15f;
    
    // Reset state variables
    sodium_activation_ = 0.0f;
    sodium_inactivation_ = 1.0f;
    potassium_activation_ = 0.0f;
    membrane_current_ = 0.0f;
    injected_current_ = 0.0f;
    synaptic_current_ = 0.0f;
}

void BiophysicalNeuronPlugin::update(float dt) {
    // Update ion channel activation variables
    update_ion_channels(dt);
    
    // Calculate total membrane current
    calculate_membrane_current();
    
    // Update membrane potential
    float dV = (membrane_current_ - membrane_potential_ + resting_potential_) / 
               membrane_capacitance_ * dt;
    membrane_potential_ += dV;
    
    // Update concentrations
    update_concentrations(dt);
}

void BiophysicalNeuronPlugin::onSpike() {
    // Custom spike handling - modulate ion channels
    for (auto& channel : sodium_channels_) {
        channel.inactivation = std::max(0.0f, channel.inactivation - 0.5f);
    }
    
    // Increase intracellular calcium during spike
    calcium_concentration_ += 0.01f;
    
    // Reset membrane potential to reset value
    membrane_potential_ = -65.0f;  // Custom reset potential
}

void BiophysicalNeuronPlugin::setParameters(const std::unordered_map<std::string, float>& params) {
    for (const auto& param : params) {
        if (param.first == "threshold") {
            threshold_ = param.second;
        } else if (param.first == "membrane_capacitance") {
            membrane_capacitance_ = param.second;
        } else if (param.first == "sodium_conductance") {
            for (auto& channel : sodium_channels_) {
                channel.conductance = param.second;
            }
        } else if (param.first == "potassium_conductance") {
            for (auto& channel : potassium_channels_) {
                channel.conductance = param.second;
            }
        }
    }
}

void BiophysicalNeuronPlugin::setInjectedCurrent(float current) {
    injected_current_ = current;
}

void BiophysicalNeuronPlugin::addSynapticCurrent(float current) {
    synaptic_current_ += current;
}

float BiophysicalNeuronPlugin::getPotential() const {
    return membrane_potential_;
}

bool BiophysicalNeuronPlugin::shouldSpike() const {
    return membrane_potential_ >= threshold_;
}

// Private helper methods implementation
void BiophysicalNeuronPlugin::update_ion_channels(float dt) {
    // Update sodium channel activation (Hodgkin-Huxley style)
    float alpha_m = 0.1f * (membrane_potential_ + 40.0f) / 
                   (1.0f - std::exp(-(membrane_potential_ + 40.0f) / 10.0f));
    float beta_m = 4.0f * std::exp(-(membrane_potential_ + 65.0f) / 18.0f);
    
    sodium_activation_ += dt * (alpha_m * (1.0f - sodium_activation_) - 
                               beta_m * sodium_activation_);
    sodium_activation_ = std::max(0.0f, std::min(1.0f, sodium_activation_));
    
    // Update sodium channel inactivation
    float alpha_h = 0.07f * std::exp(-(membrane_potential_ + 65.0f) / 20.0f);
    float beta_h = 1.0f / (1.0f + std::exp(-(membrane_potential_ + 35.0f) / 10.0f));
    
    float dh = dt * (alpha_h * (1.0f - sodium_inactivation_) - 
                    beta_h * sodium_inactivation_);
    sodium_inactivation_ += dh;
    sodium_inactivation_ = std::max(0.0f, std::min(1.0f, sodium_inactivation_));
    
    // Update potassium channel activation
    float alpha_n = 0.01f * (membrane_potential_ + 55.0f) / 
                   (1.0f - std::exp(-(membrane_potential_ + 55.0f) / 10.0f));
    float beta_n = 0.125f * std::exp(-(membrane_potential_ + 65.0f) / 80.0f);
    
    float dn = dt * (alpha_n * (1.0f - potassium_activation_) - 
                    beta_n * potassium_activation_);
    potassium_activation_ += dn;
    potassium_activation_ = std::max(0.0f, std::min(1.0f, potassium_activation_));
}

void BiophysicalNeuronPlugin::calculate_membrane_current() {
    // Calculate total ionic current
    float sodium_current = 0.0f;
    float potassium_current = 0.0f;
    float leak_current = 0.0f;
    
    for (const auto& channel : sodium_channels_) {
        sodium_current += channel.conductance * 
                        sodium_activation_ * sodium_inactivation_ * 
                        (membrane_potential_ - channel.reversal_potential);
    }
    
    for (const auto& channel : potassium_channels_) {
        potassium_current += channel.conductance * 
                          std::pow(potassium_activation_, 4) * 
                          (membrane_potential_ - channel.reversal_potential);
    }
    
    for (const auto& channel : leak_channels_) {
        leak_current += channel.conductance * 
                      (membrane_potential_ - channel.reversal_potential);
    }
    
    // Total current
    membrane_current_ = injected_current_ + synaptic_current_ + 
                       sodium_current + potassium_current + leak_current;
}

void BiophysicalNeuronPlugin::update_concentrations(float dt) {
    // Simple concentration dynamics
    float calcium_decay = 0.001f;
    float sodium_decay = 0.0001f;
    float potassium_decay = 0.00001f;
    
    // Update calcium concentration (modulated by spikes)
    calcium_concentration_ -= calcium_decay * calcium_concentration_;
    
    // Update sodium and potassium concentrations
    sodium_concentration_ -= sodium_decay * sodium_concentration_;
    potassium_concentration_ -= potassium_decay * potassium_concentration_;
}
```

## Step 3: Creating a Custom Plasticity Plugin

### 3.1 Design Your Plasticity Plugin

Let's create a **RewardModulatedSTDP** plugin that implements spike-timing-dependent
plasticity modulated by reward signals.

### 3.2 Implement the Plugin

```cpp
// RewardModulatedSTDP.hpp
#pragma once

#include "PlasticityRule.hpp"
#include "Types.hpp"
#include <vector>
#include <deque>

class RewardModulatedSTDP : public PlasticityRule {
private:
    float learning_rate_;
    float time_constant_;
    float reward_sensitivity_;
    float baseline_reward_;
    
    // Reward history for modulation
    std::deque<float> reward_history_;
    size_t max_history_size_;
    
    // Spike time history
    std::deque<float> pre_spike_times_;
    std::deque<float> post_spike_times_;
    
    // Modulation factors
    float current_modulation_factor_;
    
public:
    RewardModulatedSTDP(const std::string& name);
    ~RewardModulatedSTDP() override = default;
    
    void initialize(const PlasticityParams& params) override;
    float calculateChange(float pre_spike_time, float post_spike_time) override;
    void applyModulation(float modulation_factor) override;
    
    // Custom methods
    void addRewardSignal(float reward);
    float getAverageReward() const;
    float getCurrentModulationFactor() const;
    
    // Configuration
    void setLearningRate(float rate);
    void setTimeConstant(float tau);
    void setRewardSensitivity(float sensitivity);
    void setBaselineReward(float baseline);
};
```

### 3.3 Implementation Details

```cpp
// RewardModulatedSTDP.cpp
#include "RewardModulatedSTDP.hpp"
#include <algorithm>
#include <numeric>

RewardModulatedSTDP::RewardModulatedSTDP(const std::string& name) {
    setName(name);
    max_history_size_ = 100;
    current_modulation_factor_ = 1.0f;
}

void RewardModulatedSTDP::initialize(const PlasticityParams& params) {
    // Initialize plasticity parameters
    learning_rate_ = params.plasticity_parameters.count("learning_rate") ? 
        params.plasticity_parameters.at("learning_rate") : 0.01f;
    time_constant_ = params.plasticity_parameters.count("time_constant") ? 
        params.plasticity_parameters.at("time_constant") : 20.0f;
    reward_sensitivity_ = params.plasticity_parameters.count("reward_sensitivity") ? 
        params.plasticity_parameters.at("reward_sensitivity") : 0.5f;
    baseline_reward_ = params.plasticity_parameters.count("baseline_reward") ? 
        params.plasticity_parameters.at("baseline_reward") : 0.0f;
    
    // Reset history
    reward_history_.clear();
    pre_spike_times_.clear();
    post_spike_times_.clear();
    current_modulation_factor_ = 1.0f;
}

float RewardModulatedSTDP::calculateChange(float pre_spike_time, float post_spike_time) {
    // Standard STDP calculation
    float time_difference = post_spike_time - pre_spike_time;
    
    float weight_change = 0.0f;
    
    if (time_difference > 0) {
        // Post-before-pre: depression
        weight_change = -learning_rate_ * std::exp(-time_difference / time_constant_);
    } else {
        // Pre-before-post: potentiation
        weight_change = learning_rate_ * std::exp(time_difference / time_constant_);
    }
    
    // Apply current reward modulation
    weight_change *= current_modulation_factor_;
    
    // Store spike times for potential future calculations
    pre_spike_times_.push_back(pre_spike_time);
    post_spike_times_.push_back(post_spike_time);
    
    // Limit history size
    if (pre_spike_times_.size() > max_history_size_) {
        pre_spike_times_.pop_front();
        post_spike_times_.pop_front();
    }
    
    return weight_change;
}

void RewardModulatedSTDP::applyModulation(float modulation_factor) {
    current_modulation_factor_ = modulation_factor;
}

void RewardModulatedSTDP::addRewardSignal(float reward) {
    reward_history_.push_back(reward);
    
    if (reward_history_.size() > max_history_size_) {
        reward_history_.pop_front();
    }
    
    // Update modulation factor based on reward
    float average_reward = getAverageReward();
    current_modulation_factor_ = 1.0f + reward_sensitivity_ * (average_reward - baseline_reward_);
    
    // Clamp modulation factor to reasonable range
    current_modulation_factor_ = std::max(0.1f, std::min(10.0f, current_modulation_factor_));
}

float RewardModulatedSTDP::getAverageReward() const {
    if (reward_history_.empty()) {
        return baseline_reward_;
    }
    
    float sum = std::accumulate(reward_history_.begin(), reward_history_.end(), 0.0f);
    return sum / reward_history_.size();
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    learning_rate_ = std::max(0.0f, rate);
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    time_constant_ = std::max(1.0f, tau);
}

void RewardModulatedSTDP::setRewardSensitivity(float sensitivity) {
    reward_sensitivity_ = std::max(0.0f, sensitivity);
}

void RewardModulatedSTDP::setBaselineReward(float baseline) {
    baseline_reward_ = baseline;
}
```

## Step 4: Plugin Registration and Integration

### 4.1 Plugin Registration System

Create a plugin registration system:

```cpp
// PluginRegistration.hpp
#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

class PluginBase {
public:
    virtual ~PluginBase() = default;
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual bool initialize(const nlm::Config& config) = 0;
    virtual void shutdown() = 0;
};

class PluginRegistry {
private:
    static PluginRegistry* instance_;
    std::map<std::string, std::unique_ptr<PluginBase>> plugins_;
    std::map<std::string, std::vector<std::string>> plugin_dependencies_;
    
public:
    static PluginRegistry* getInstance() {
        if (!instance_) {
            instance_ = new PluginRegistry();
        }
        return instance_;
    }
    
    template<typename T, typename... Args>
    bool registerPlugin(const std::string& name, Args&&... args) {
        auto plugin = std::make_unique<T>(std::forward<Args>(args)...);
        
        // Check dependencies
        if (hasDependencies(plugin->getName())) {
            return false;
        }
        
        plugins_[name] = std::move(plugin);
        return true;
    }
    
    template<typename T>
    T* getPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        if (it != plugins_.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    bool loadPluginFromFile(const std::string& plugin_path);
    bool unloadPlugin(const std::string& name);
    
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getLoadedPlugins() const;
    
    void initializeAllPlugins(const nlm::Config& config);
    void shutdownAllPlugins();
    
private:
    PluginRegistry() = default;
    
    bool hasDependencies(const std::string& plugin_name) const;
};
```

### 4.2 Registration Function

```cpp
// PluginRegistration.cpp
#include "PluginRegistration.hpp"
#include "BiophysicalNeuronPlugin.hpp"
#include "RewardModulatedSTDP.hpp"

PluginRegistry* PluginRegistry::instance_ = nullptr;

void registerCustomPlugins() {
    // Register neuron plugins
    PluginRegistry::getInstance()->registerPlugin<BiophysicalNeuronPlugin>(
        "BiophysicalNeuron",
        "BiophysicalNeuron"
    );
    
    // Register plasticity plugins
    PluginRegistry::getInstance()->registerPlugin<RewardModulatedSTDP>(
        "RewardModulatedSTDP",
        "RewardModulatedSTDP"
    );
    
    // Set up dependencies
    PluginRegistry::getInstance()->plugin_dependencies_["RewardModulatedSTDP"] = {
        "BiophysicalNeuron"
    };
}

bool PluginRegistry::loadPluginFromFile(const std::string& plugin_path) {
    // Implement dynamic library loading
    // This would use dlopen, LoadLibrary, etc.
    return false;
}

// Call registration function
void initializePluginSystem() {
    registerCustomPlugins();
}
```

## Step 5: Plugin Configuration and Usage

### 5.1 Configuration Example

```cpp
// In your main application
int main(int argc, char* argv[]) {
    // Initialize plugin system
    initializePluginSystem();
    
    // Create configuration that uses plugins
    auto config = std::make_shared<nlm::Config>();
    
    // Configure neuron plugin
    config->set("neuron.plugin", "BiophysicalNeuron");
    config->set("neuron.threshold", -45.0f);
    config->set("neuron.sodium_conductance", 150.0f);
    config->set("neuron.potassium_conductance", 40.0f);
    
    // Configure plasticity plugin
    config->set("plasticity.rule", "RewardModulatedSTDP");
    config->set("plasticity.learning_rate", 0.02f);
    config->set("plasticity.reward_sensitivity", 1.0f);
    
    // Configure neuromodulation
    config->set("neuromod.reward.enable", true);
    config->set("neuromod.reward.sensitivity", 2.0f);
    
    // Create brain with plugin configuration
    auto brain = std::make_unique<nlm::Brain>(config);
    brain->initialize();
    
    // Rest of your application...
    return 0;
}
```

### 5.2 Runtime Plugin Switching

```cpp
class PluginManager {
public:
    void switchNeuronPlugin(const std::string& plugin_name) {
        if (current_neuron_plugin_) {
            current_neuron_plugin_->shutdown();
        }
        
        auto new_plugin = PluginRegistry::getInstance()->getPlugin<BiophysicalNeuronPlugin>(plugin_name);
        if (new_plugin) {
            new_plugin->initialize(current_neuron_params_);
            current_neuron_plugin_ = new_plugin;
        }
    }
    
    void switchPlasticityPlugin(const std::string& plugin_name) {
        if (current_plasticity_plugin_) {
            current_plasticity_plugin_->shutdown();
        }
        
        auto new_plugin = PluginRegistry::getInstance()->getPlugin<RewardModulatedSTDP>(plugin_name);
        if (new_plugin) {
            new_plugin->initialize(current_plasticity_params_);
            current_plasticity_plugin_ = new_plugin;
        }
    }
    
    // Other methods for plugin management...
};
```

## Step 6: Plugin Testing

### 6.1 Unit Testing Plugin

```cpp
// TestBiophysicalNeuronPlugin.cpp
#include "BiophysicalNeuronPlugin.hpp"
#include <cassert>
#include <iostream>

void test_biophysical_neuron_plugin() {
    std::cout << "Testing BiophysicalNeuronPlugin..." << std::endl;
    
    // Create plugin
    BiophysicalNeuronPlugin plugin("TestBiophysicalNeuron");
    
    // Create test parameters
    nlm::NeuronParams params;
    params.threshold = -50.0f;
    params.resting_potential = -65.0f;
    params.membrane_capacitance = 1.0f;
    params.neuron_parameters["num_channels"] = 10;
    params.neuron_parameters["sodium_conductance"] = 100.0f;
    params.neuron_parameters["potassium_conductance"] = 30.0f;
    
    // Initialize plugin
    plugin.initialize(params);
    
    // Test initial state
    assert(plugin.getPotential() == -65.0f);  // Resting potential
    assert(!plugin.shouldSpike());
    
    // Test injected current
    plugin.setInjectedCurrent(10.0f);
    
    // Update plugin
    plugin.update(0.001f);
    
    // Test potential change
    assert(plugin.getPotential() > -65.0f);  // Should depolarize
    
    // Test spike
    plugin.setInjectedCurrent(0.0f);  // Reset
    plugin.onSpike();
    assert(plugin.getPotential() == -65.0f);  // Should reset
    
    std::cout << "BiophysicalNeuronPlugin tests passed!" << std::endl;
}

void test_reward_modulated_stdp() {
    std::cout << "Testing RewardModulatedSTDP..." << std::endl;
    
    // Create plasticity rule
    RewardModulatedSTDP plasticity("TestRewardModulatedSTDP");
    
    // Create test parameters
    nlm::PlasticityParams params;
    params.plasticity_parameters["learning_rate"] = 0.01f;
    params.plasticity_parameters["time_constant"] = 20.0f;
    params.plasticity_parameters["reward_sensitivity"] = 0.5f;
    params.plasticity_parameters["baseline_reward"] = 0.0f;
    
    // Initialize plasticity rule
    plasticity.initialize(params);
    
    // Test initial modulation factor
    assert(plasticity.getCurrentModulationFactor() == 1.0f);
    
    // Add reward signal
    plasticity.addRewardSignal(1.0f);
    assert(plasticity.getCurrentModulationFactor() > 1.0f);
    
    // Test STDP calculation
    float pre_time = 100.0f;
    float post_time = 105.0f;  // Post before pre: depression
    
    float change1 = plasticity.calculateChange(pre_time, post_time);
    assert(change1 < 0.0f);  // Should be depression
    
    // Add negative reward
    plasticity.addRewardSignal(-1.0f);
    
    // Test modulation with negative reward
    float change2 = plasticity.calculateChange(pre_time + 10.0f, post_time + 10.0f);
    assert(change2 > change1);  // Should be less depression with negative reward
    
    std::cout << "RewardModulatedSTDP tests passed!" << std::endl;
}
```

## Step 7: Plugin Integration with NLM

### 7.1 Integration into Core Systems

The plugins need to be integrated into the core NLM systems:

```cpp
// Integration into Brain.hpp
class Brain {
public:
    // ... existing methods ...
    
    // Plugin management
    void loadPlugins(const std::string& plugin_directory);
    bool addNeuronPlugin(const std::string& plugin_name);
    bool addPlasticityPlugin(const std::string& plugin_name);
    
    // Plugin access
    NeuronPlugin* getNeuronPlugin() const;
    PlasticityRule* getPlasticityRule() const;
    
private:
    std::unique_ptr<NeuronPlugin> neuron_plugin_;
    std::unique_ptr<PlasticityRule> plasticity_rule_;
    std::unique_ptr<PluginRegistry> plugin_registry_;
};
```

### 7.2 Plugin Usage in Simulation

```cpp
// Integration into simulation loop
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Get current neuron and plasticity plugins
    NeuronPlugin* neuron_plugin = getNeuronPlugin();
    PlasticityRule* plasticity_rule = getPlasticityRule();
    
    if (neuron_plugin && plasticity_rule) {
        // Use plugin for neuron updates
        for (auto& neuron : neurons_) {
            neuron_plugin->update(1.0f / currentTime.getFrequency());
            
            if (neuron_plugin->shouldSpike()) {
                neuron_plugin->onSpike();
                plasticity_rule->applyPlasticity(neuron, currentTime);
            }
        }
    } else {
        // Use default implementation
        // ... original simulation code ...
    }
}
```

## Step 8: Python Plugin Integration

### 8.1 Python Plugin Interface

Create Python bindings for plugins:

```cpp
// Python bindings for plugins
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "BiophysicalNeuronPlugin.hpp"
#include "RewardModulatedSTDP.hpp"

namespace py = pybind11;

PYBIND11_MODULE(nlm_plugins, m) {
    m.doc() = "NLM Plugin Module";
    
    // Register BiophysicalNeuronPlugin
    py::class_<BiophysicalNeuronPlugin, NeuronPlugin, std::shared_ptr<BiophysicalNeuronPlugin>>(m, "BiophysicalNeuronPlugin")
        .def(py::init<const std::string&>())
        .def("set_injected_current", &BiophysicalNeuronPlugin::setInjectedCurrent)
        .def("add_synaptic_current", &BiophysicalNeuronPlugin::addSynapticCurrent)
        .def("get_calcium_concentration", &BiophysicalNeuronPlugin::getCalciumConcentration)
        .def("get_sodium_concentration", &BiophysicalNeuronPlugin::getSodiumConcentration)
        .def("get_potassium_concentration", &BiophysicalNeuronPlugin::getPotassiumConcentration)
        .def("get_sodium_conductance", &BiophysicalNeuronPlugin::getSodiumConductance)
        .def("get_potassium_conductance", &BiophysicalNeuronPlugin::getPotassiumConductance)
        .def("get_leak_conductance", &BiophysicalNeuronPlugin::getLeakConductance);
    
    // Register RewardModulatedSTDP
    py::class_<RewardModulatedSTDP, PlasticityRule, std::shared_ptr<RewardModulatedSTDP>>(m, "RewardModulatedSTDP")
        .def(py::init<const std::string&>())
        .def("add_reward_signal", &RewardModulatedSTDP::addRewardSignal)
        .def("get_average_reward", &RewardModulatedSTDP::getAverageReward)
        .def("get_current_modulation_factor", &RewardModulatedSTDP::getCurrentModulationFactor)
        .def("set_learning_rate", &RewardModulatedSTDP::setLearningRate)
        .def("set_time_constant", &RewardModulatedSTDP::setTimeConstant)
        .def("set_reward_sensitivity", &RewardModulatedSTDP::setRewardSensitivity)
        .def("set_baseline_reward", &RewardModulatedSTDP::setBaselineReward);
    
    // Plugin factory functions
    m.def("create_biophysical_neuron", []() {
        return std::make_shared<BiophysicalNeuronPlugin>("DefaultBiophysicalNeuron");
    });
    
    m.def("create_reward_modulated_stdp", []() {
        return std::make_shared<RewardModulatedSTDP>("DefaultRewardModulatedSTDP");
    });
}
```

### 8.2 Python Plugin Usage

```python
import pynlm
from pynlm import plugins

# Import plugin module
import nlm_plugins

# Create and use plugins
neuron_plugin = nlm_plugins.create_biophysical_neuron()
plasticity_plugin = nlm_plugins.create_reward_modulated_stdp()

# Configure plugins
neuron_plugin.set_parameters({
    'threshold': -45.0,
    'membrane_capacitance': 1.0,
    'sodium_conductance': 150.0,
    'potassium_conductance': 40.0
})

plasticity_plugin.set_learning_rate(0.02)
plasticity_plugin.set_time_constant(20.0)
plasticity_plugin.set_reward_sensitivity(1.0)

# Create brain with plugins
config = pynlm.createDefaultConfig()
config.set("neuron.plugin", "BiophysicalNeuron")
config.set("plasticity.rule", "RewardModulatedSTDP")

brain = pynlm.createBrain(config)
brain.initialize()
```

## Step 9: Plugin Development Best Practices

### 9.1 Code Quality

1. **Thread Safety**: Ensure plugins are thread-safe if used in multi-threaded environments
2. **Memory Management**: Properly manage memory and avoid leaks
3. **Error Handling**: Implement robust error handling
4. **Documentation**: Document plugin interfaces and behavior
5. **Testing**: Thoroughly test plugins in isolation

### 9.2 Performance Considerations

1. **Minimize Overhead**: Plugin interfaces should have minimal performance impact
2. **Efficient Updates**: Optimize update loops for real-time performance
3. **Resource Management**: Efficiently manage memory and computational resources
4. **Parallelization**: Consider parallelization for large-scale simulations

### 9.3 Plugin Distribution

1. **Version Control**: Use version control for plugin development
2. **Dependencies**: Clearly specify plugin dependencies
3. **Compatibility**: Maintain backward compatibility where possible
4. **Testing**: Test plugins with different NLM versions

## Step 10: Plugin Examples and Templates

### 10.1 Complete Plugin Example

Here's a complete example of a custom neuron plugin:

```cpp
// MyCustomNeuronPlugin.hpp
#pragma once

#include "NeuronPlugin.hpp"
#include "Types.hpp"

class MyCustomNeuronPlugin : public NeuronPlugin {
private:
    float potential_;
    float threshold_;
    float decay_rate_;
    float input_response_;
    float adaptation_current_;
    
public:
    MyCustomNeuronPlugin(const std::string& name);
    
    void initialize(const NeuronParams& params) override;
    void update(float dt) override;
    void onSpike() override;
    float getPotential() const override;
    bool shouldSpike() const override;
    void setParameters(const std::unordered_map<std::string, float>& params) override;
};

// MyCustomNeuronPlugin.cpp
#include "MyCustomNeuronPlugin.hpp"

MyCustomNeuronPlugin::MyCustomNeuronPlugin(const std::string& name) {
    setName(name);
    potential_ = 0.0f;
    threshold_ = -50.0f;
    decay_rate_ = 0.1f;
    input_response_ = 1.0f;
    adaptation_current_ = 0.0f;
}

void MyCustomNeuronPlugin::initialize(const NeuronParams& params) {
    threshold_ = params.threshold;
    decay_rate_ = params.neuron_parameters.count("decay_rate") ? 
        params.neuron_parameters.at("decay_rate") : 0.1f;
    input_response_ = params.neuron_parameters.count("input_response") ? 
        params.neuron_parameters.at("input_response") : 1.0f;
    
    potential_ = 0.0f;
    adaptation_current_ = 0.0f;
}

void MyCustomNeuronPlugin::update(float dt) {
    // Decay potential
    potential_ *= (1.0f - decay_rate_ * dt);
    
    // Add adaptation current
    potential_ -= adaptation_current_ * dt;
}

void MyCustomNeuronPlugin::onSpike() {
    potential_ = 0.0f;
    adaptation_current_ += 1.0f;  // Simple adaptation
}

float MyCustomNeuronPlugin::getPotential() const {
    return potential_;
}

bool MyCustomNeuronPlugin::shouldSpike() const {
    return potential_ >= threshold_;
}

void MyCustomNeuronPlugin::setParameters(const std::unordered_map<std::string, float>& params) {
    for (const auto& param : params) {
        if (param.first == "threshold") {
            threshold_ = param.second;
        } else if (param.first == "decay_rate") {
            decay_rate_ = param.second;
        } else if (param.first == "input_response") {
            input_response_ = param.second;
        }
    }
}
```

### 10.2 Plugin Template

```cpp
// PluginTemplate.hpp
#pragma once

#include "NeuronPlugin.hpp"
#include "Types.hpp"
#include <string>

class PluginTemplate : public NeuronPlugin {
public:
    PluginTemplate(const std::string& name);
    ~PluginTemplate() override = default;
    
    void initialize(const NeuronParams& params) override;
    void update(float dt) override;
    void onSpike() override;
    float getPotential() const override;
    bool shouldSpike() const override;
    void setParameters(const std::unordered_map<std::string, float>& params) override;
    
    // Override these methods to customize behavior
    virtual void customInitialize(const NeuronParams& params) {}
    virtual void customUpdate(float dt) {}
    virtual void customOnSpike() {}
    virtual void customSetParameters(const std::unordered_map<std::string, float>& params) {}
};

// PluginTemplate.cpp
#include "PluginTemplate.hpp"

PluginTemplate::PluginTemplate(const std::string& name) {
    setName(name);
    // Initialize your custom variables here
}

void PluginTemplate::initialize(const NeuronParams& params) {
    // Call base initialization if needed
    // Call your custom initialization
    customInitialize(params);
}

void PluginTemplate::update(float dt) {
    // Call your custom update logic
    customUpdate(dt);
}

void PluginTemplate::onSpike() {
    // Call your custom spike handling
    customOnSpike();
}

float PluginTemplate::getPotential() const {
    // Return your potential value
    return 0.0f;
}

bool PluginTemplate::shouldSpike() const {
    // Return whether neuron should spike
    return false;
}

void PluginTemplate::setParameters(const std::unordered_map<std::string, float>& params) {
    // Call your custom parameter setting
    customSetParameters(params);
}
```

## Conclusion

This tutorial has provided a comprehensive guide to developing custom plugins for the NLM framework. You now have the knowledge to:

1. **Design plugin interfaces** for different neural components
2. **Implement custom neuron and plasticity plugins** with realistic dynamics
3. **Register and integrate plugins** into the NLM system
4. **Test plugins** thoroughly before deployment
5. **Use plugins in both C++ and Python** environments
6. **Follow best practices** for plugin development

The plugin architecture transforms NLM from a fixed neural simulator into a flexible research platform where you can explore novel neural mechanisms, optimize performance, and customize the simulation for specific research needs.

By developing custom plugins, you can:

- **Extend NLM capabilities** beyond the built-in features
- **Explore new neural models** and learning algorithms
- **Optimize performance** for specific hardware or use cases
- **Share research** through plugin distribution
- **Reproduce research results** with consistent plugin configurations

Start with simple plugins and gradually build more complex ones. The plugin system is designed to be extensible, allowing you to grow your plugin ecosystem over time.

Happy plugin development!
