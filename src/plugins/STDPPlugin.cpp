// Izhikevich Plasticity Rule Plugin
// Example plugin demonstrating a custom plasticity rule

#include "plugins/PluginSystem.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class SimpleSTDPPlasticityRulePlugin : public PlasticityRulePlugin {
public:
    SimpleSTDPPlasticityRulePlugin() : PluginInterface() {
        metadata.name = "simple_stdp";
        metadata.version = "1.0.0";
        metadata.author = "NLM Plugin Examples";
        metadata.description = "Simple Spike-Timing-Dependent Plasticity (STDP) rule";
        metadata.license = "MIT";
        metadata.type = PluginType::PlasticityRule;
        metadata.tags = {"plasticity", "stdp", "learning", "rule"};
        
        capabilities.type = PluginType::PlasticityRule;
        capabilities.name = "simple_stdp";
        capabilities.description = "Simple STDP plasticity rule with LTP/LTD";
        capabilities.version = "1.0.0";
        capabilities.dependencies = {"neuron"};
        capabilities.isCorePlugin = false;
        
        parameters = {
            {"learning_rate_ltp", 0.01f},
            {"learning_rate_ltd", 0.012f},
            {"time_constant", 20.0f},
            {"max_weight", 1.0f},
            {"min_weight", -1.0f}
        };
    }
    
    const PluginMetadata& getMetadata() const override {
        return metadata;
    }
    
    bool initialize() override {
        initialized = true;
        return true;
    }
    
    void shutdown() override {
        initialized = false;
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    const PluginCapabilities& getCapabilities() const override {
        return capabilities;
    }
    
    void updateSynapticWeight(
        float& weight,
        const std::vector<Timestamp>& preSpikeHistory,
        const std::vector<Timestamp>& postSpikeHistory,
        float learningRate,
        float timeConstant
    ) override {
        // Apply simplified STDP rule
        if (postSpikeHistory.empty() || preSpikeHistory.empty()) {
            return; // No learning without spikes
        }
        
        // Get most recent pre and post spike times
        Timestamp lastPreSpike = postSpikeHistory.back();
        Timestamp lastPostSpike = preSpikeHistory.back();
        
        // Calculate time difference
        float dt = static_cast<float>(lastPostSpike - lastPreSpike);
        
        // Apply STDP rule
        float weightChange = 0.0f;
        
        if (dt > 0) { // Post fires after pre (LTP)
            float ltpFactor = std::exp(-dt / timeConstant);
            weightChange = learningRate * ltpFactor;
        } else { // Post fires before pre (LTD)
            float ltdFactor = std::exp(dt / timeConstant);
            weightChange = -learningRate * ltdFactor;
        }
        
        // Apply weight change with bounds
        weight += weightChange;
        weight = std::clamp(weight, parameters.at("min_weight"), parameters.at("max_weight"));
    }
    
    std::string getRuleName() const override {
        return "Simple STDP";
    }
    
    const std::unordered_map<std::string, float>& getParameters() const override {
        return parameters;
    }
    
private:
    PluginMetadata metadata;
    PluginCapabilities capabilities;
    std::unordered_map<std::string, float> parameters;
    bool initialized = false;
};

// Simple STDP Plasticity Rule Factory
class SimpleSTDPPlasticityRulePluginFactory : public PluginFactory {
public:
    std::unique_ptr<PluginInterface> createPlugin(const PluginMetadata& metadata) override {
        if (metadata.name == "simple_stdp") {
            return std::make_unique<SimpleSTDPPlasticityRulePlugin>();
        }
        return nullptr;
    }
    
    bool canCreate(const PluginMetadata& metadata) const override {
        return metadata.name == "simple_stdp" || metadata.name == "stdp" || metadata.name == "STDP";
    }
};

} // namespace nlm
