#pragma once

#include "CustomPlasticityPlugin.hpp"
#include "Core/PluginInterface.hpp"
#include "Core/Types/Types.hpp"
#include "Brain/Synapse.hpp"
#include "Brain/Neuron.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {
namespace plugins {

class CustomPlasticityPlugin : public Plugin {
public:
    CustomPlasticityPlugin() : learningRate_(0.01f), decayRate_(0.001f), 
                              enableSTDP_(true), enableHebbian_(true) {}
    
    const std::string& getName() const override {
        static const std::string name = "CustomPlasticityPlugin";
        return name;
    }
    
    const std::string& getVersion() const override {
        static const std::string version = "1.2.0";
        return version;
    }
    
    const std::string& getAuthor() const override {
        static const std::string author = "NLM Development Team";
        return author;
    }
    
    const std::string& getDescription() const override {
        static const std::string description = "Custom plasticity rule implementation with adaptive learning rates and novelty-based modulation";
        return description;
    }
    
    bool initialize(std::shared_ptr<PluginManager> manager, 
                   std::shared_ptr<Config> config) override {
        pluginManager_ = manager;
        config_ = config;
        
        // Apply configuration if available
        if (config_) {
            learningRate_ = config_->getOr<float>("plugin_custom_plasticity_learning_rate", learningRate_);
            decayRate_ = config_->getOr<float>("plugin_custom_plasticity_decay_rate", decayRate_);
            enableSTDP_ = config_->getOr<bool>("plugin_custom_plasticity_enable_stdp", enableSTDP_);
            enableHebbian_ = config_->getOr<bool>("plugin_custom_plasticity_enable_hebbian", enableHebbian_);
        }
        
        // Register for plugin system updates
        if (pluginManager_) {
            pluginManager_->executePluginFunction(getName(), "register", {"plasticity"});
        }
        
        return true;
    }
    
    void shutdown() override {
        if (pluginManager_) {
            pluginManager_->executePluginFunction(getName(), "unregister", {});
        }
        
        // Clean up any registered hooks
        registeredHooks_.clear();
    }
    
    void update(TimestepDuration dt) override {
        // Update plugin state
        currentTime_ += dt;
        
        // Adjust learning rate based on network activity
        adjustLearningRate();
        
        // Check for activity-dependent plasticity events
        processActivityEvents(dt);
    }
    
    bool onPluginMessage(const std::string& message, 
                        const std::vector<std::string>& data) override {
        if (message == "synapse_update") {
            return handleSynapseUpdate(data);
        } else if (message == "neuron_spike") {
            return handleNeuronSpike(data);
        } else if (message == "get_plasticity_rules") {
            return providePlasticityRules(data);
        }
        return false;
    }
    
    std::string getPluginSpecificInfo() const override {
        json info;
        info["learning_rate"] = learningRate_;
        info["decay_rate"] = decayRate_;
        info["enable_stdp"] = enableSTDP_;
        info["enable_hebbian"] = enableHebbian_;
        info["current_time"] = currentTime_;
        info["total_synapses_processed"] = totalSynapsesProcessed_;
        
        std::stringstream ss;
        ss << info.dump(2);
        return ss.str();
    }
    
    bool requires(const std::string& dependency) const override {
        static const std::vector<std::string> dependencies = {"brain", "synapse", "neuron"};
        return std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end();
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"brain", "synapse", "neuron"};
    }
    
    // Custom plasticity methods
    void setLearningRate(float rate) { learningRate_ = std::max(0.0f, std::min(rate, 1.0f)); }
    float getLearningRate() const { return learningRate_; }
    
    void setDecayRate(float rate) { decayRate_ = std::max(0.0f, rate); }
    float getDecayRate() const { return decayRate_; }
    
    void enableSTDP(bool enable) { enableSTDP_ = enable; }
    bool isSTDPEnabled() const { return enableSTDP_; }
    
    void enableHebbian(bool enable) { enableHebbian_ = enable; }
    bool isHebbianEnabled() const { return enableHebbian_; }
    
    // Custom plasticity rule implementation
    void computeCustomPlasticity(Synapse* synapse, 
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate spike timing differences
        float minPreDelta = std::numeric_limits<float>::max();
        float minPostDelta = std::numeric_limits<float>::max();
        
        for (const auto& spike : preSpikes) {
            float delta = std::abs(static_cast<float>(spike - currentTime_) * 0.001f); // Convert to seconds
            minPreDelta = std::min(minPreDelta, delta);
        }
        
        for (const auto& spike : postSpikes) {
            float delta = std::abs(static_cast<float>(spike - currentTime_) * 0.001f);
            minPostDelta = std::min(minPostDelta, delta);
        }
        
        // Custom plasticity rule based on spike timing
        float weightChange = 0.0f;
        
        if (enableSTDP_) {
            // Custom STDP variant with sigmoidal modulation
            float timeDiff = minPostDelta - minPreDelta;
            
            if (std::abs(timeDiff) < 0.1f) { // Within 100ms window
                float modulation = 1.0f / (1.0f + std::exp(-5.0f * (timeDiff - 0.02f)));
                weightChange = learningRate_ * modulation * synapse->getWeight();
            }
        }
        
        if (enableHebbian_ && !preSpikes.empty() && !postSpikes.empty()) {
            // Add Hebbian component based on co-activation
            float coActivation = static_cast<float>(preSpikes.size() * postSpikes.size()) / 100.0f;
            weightChange += learningRate_ * 0.1f * coActivation * synapse->getWeight();
        }
        
        // Apply decay
        weightChange *= (1.0f - decayRate_ * dt.count());
        
        if (std::abs(weightChange) > 0.0001f) {
            synapse->applyWeightChange(weightChange);
            totalSynapsesProcessed_++;
            
            // Notify other plugins
            if (pluginManager_) {
                pluginManager_->executePluginFunction(
                    "NeuromodulatorPlugin", 
                    "plasticity_event", 
                    {std::to_string(weightChange), synapse->getId()}
                );
            }
        }
    }
    
private:
    bool handleSynapseUpdate(const std::vector<std::string>& data) {
        if (data.size() < 2) return false;
        
        // Parse synapse update data
        // Format: "<pre_spike_count> <post_spike_count>"
        // This is a simplified implementation
        
        if (pluginManager_) {
            // Forward to other plugins
            pluginManager_->executePluginFunction(
                "SensoryPlugin",
                "plasticity_request",
                {"custom_rule", data[0]}
            );
        }
        
        return true;
    }
    
    bool handleNeuronSpike(const std::vector<std::string>& data) {
        if (data.size() < 1) return false;
        
        // Process neuron spike data
        // In real implementation, would update spike history and trigger plasticity
        
        return true;
    }
    
    bool providePlasticityRules(const std::vector<std::string>& data) {
        // Return available plasticity rules
        std::vector<std::string> rules = {"custom_stdp", "custom_hebbian", "adaptive_decay"};
        
        for (const auto& rule : rules) {
            pluginManager_->executePluginFunction(
                getName(),
                "rule_available",
                {rule}
            );
        }
        
        return true;
    }
    
    void adjustLearningRate() {
        // Adaptive learning rate based on network activity
        if (currentTime_ > lastActivityCheck_) {
            float activityLevel = static_cast<float>(totalSynapsesProcessed_) / 1000.0f;
            
            // Increase learning rate with higher activity (up to a limit)
            learningRate_ = std::min(0.05f, learningRate_ * (1.0f + activityLevel * 0.1f));
            
            lastActivityCheck_ = currentTime_ + 1.0f; // Check every second
        }
    }
    
    void processActivityEvents(TimestepDuration dt) {
        // Process any registered hooks
        for (const auto& hook : registeredHooks_) {
            if (currentTime_ >= hook.nextTrigger) {
                // Process hook logic
                hook.callback(dt);
                hook.nextTrigger = currentTime_ + hook.interval;
            }
        }
    }
    
    struct ActivityHook {
        std::function<void(TimestepDuration)> callback;
        float nextTrigger = 0.0f;
        float interval = 1.0f;
    };
    
    float learningRate_;
    float decayRate_;
    float currentTime_ = 0.0f;
    float lastActivityCheck_ = 0.0f;
    size_t totalSynapsesProcessed_ = 0;
    bool enableSTDP_;
    bool enableHebbian_;
    std::vector<ActivityHook> registeredHooks_;
};

} // namespace plugins
} // namespace nlm

// Plugin factory function
extern "C" {
    std::shared_ptr<nlm::cli::Plugin> createPlugin() {
        return std::make_shared<nlm::plugins::CustomPlasticityPlugin>();
    }
}
