// Custom STDP with Reward Modulation Plugin
// Example plugin demonstrating how to extend NLM with custom plasticity rules

#include "PluginInterface.hpp"
#include "brain/Synapse.hpp"
#include <cmath>

namespace nlm {

class CustomSTDPPlugin : public PlasticityRulePlugin {
public:
    CustomSTDPPlugin() : learningRate_(0.01f), rewardModulation_(1.0f) {}
    
    const char* getName() const override { return "CustomSTDP"; }
    const char* getVersion() const override { return "1.0.0"; }
    const char* getDescription() const override { 
        return "Custom STDP with reward modulation for enhanced learning"; 
    }
    const char* getComponentName() const override { return "EnhancedSTDP"; }
    
    bool initialize(const std::string& config) override {
        // Parse configuration
        // For simplicity, just return true
        return true;
    }
    
    void shutdown() override {
        // Clean up resources
    }
    
    bool isRunning() const override {
        return true; // Plugin is always "running" when loaded
    }
    
    void update(Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        float totalDelta = 0.0f;
        float tau = timeConstant_;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (dt > 0) {
                    // Pre before post: POTENTIATION with reward modulation
                    float baseDelta = ltpWeight_ * std::exp(-dt / tau);
                    float modulatedDelta = baseDelta * rewardModulation_;
                    totalDelta += modulatedDelta;
                } else if (dt < 0) {
                    // Post before pre: DEPRESSION with reward modulation
                    float baseDelta = -ltdWeight_ * std::exp(dt / tau);
                    float modulatedDelta = baseDelta * rewardModulation_;
                    totalDelta += modulatedDelta;
                }
            }
        }
        
        // Apply weight change with bounds
        if (std::abs(totalDelta) > 1e-6f) {
            float newWeight = synapse->getWeight() + totalDelta;
            newWeight = std::clamp(newWeight, minWeight_, maxWeight_);
            synapse->setWeight(newWeight);
        }
    }
    
    float getLearningRate() const override {
        return learningRate_;
    }
    
    void setLearningRate(float rate) override {
        learningRate_ = std::clamp(rate, 0.0f, 1.0f);
    }
    
    // Custom parameters for this plugin
    void setRewardModulation(float modulation) {
        rewardModulation_ = std::clamp(modulation, 0.0f, 2.0f);
    }
    
    float getRewardModulation() const {
        return rewardModulation_;
    }
    
    void setTimeConstant(float tau) {
        timeConstant_ = std::clamp(tau, 1.0f, 100.0f);
    }
    
    float getTimeConstant() const {
        return timeConstant_;
    }
    
private:
    float learningRate_;
    float rewardModulation_;
    float timeConstant_ = 20.0f;
    float ltpWeight_ = 0.02f;
    float ltdWeight_ = 0.015f;
    float minWeight_ = -1.0f;
    float maxWeight_ = 1.0f;
};

// Factory function to create the plugin
extern "C" Plugin* create_plugin() {
    return new CustomSTDPPlugin();
}

// Plugin information for discovery
extern "C" const char* get_plugin_info() {
    return "Custom STDP with Reward Modulation Plugin v1.0.0";
}

} // namespace nlm