#include "NeuronModelPlugin.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <algorithm>

namespace nlm {

class DefaultPlasticityRulePlugin : public IPlasticityRulePlugin {
public:
    DefaultPlasticityRulePlugin() = default;
    ~DefaultPlasticityRulePlugin() override = default;
    
    std::string getName() const override { return "DefaultPlasticityRule"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getPlasticityRuleName() const override { return "STDP+Hebbian"; }
    
    bool initialize(const Config& config) override {
        // Initialize with default plasticity parameters
        return true;
    }
    
    void update(double timestep) override {
        // Update plasticity parameters if needed
    }
    
    void shutdown() override {
        // Clean up resources
    }
    
    float computeWeightChange(const SynapseState& pre, const SynapseState& post, double dt) override {
        // Default STDP + Hebbian combined implementation
        float stdpChange = computeSTDPChange(pre, post, dt);
        float hebbianChange = computeHebbianChange(pre, post);
        return stdpChange + hebbianChange;
    }
    
    bool shouldApplyPlasticity(const SynapseState& pre, const SynapseState& post, const Config& config) override {
        // Default: apply plasticity when both pre and post have spiked recently
        return (pre.lastSpikeTime > 0 && post.lastSpikeTime > 0);
    }
    
    void onPreSynapseSpikes(Synapse& synapse, Timestamp timestamp) override {
        // Record spike for STDP
        synapse.preSpikeHistory.push_back(timestamp);
    }
    
    void onPostSynapseSpikes(Synapse& synapse, Timestamp timestamp) override {
        // Record spike for STDP
        synapse.postSpikeHistory.push_back(timestamp);
    }
    
private:
    float computeSTDPChange(const SynapseState& pre, const SynapseState& post, double dt) {
        // Simplified STDP implementation
        if (pre.lastSpikeTime <= 0 || post.lastSpikeTime <= 0) {
            return 0.0f;
        }
        
        double delta_t = post.lastSpikeTime - pre.lastSpikeTime;
        float learningRate = 0.01f;
        
        if (delta_t > 0) {
            // Post before pre: Long-Term Depression
            return -learningRate * std::exp(-delta_t / 20.0);
        } else {
            // Pre before post: Long-Term Potentiation
            return learningRate * std::exp(delta_t / 20.0);
        }
    }
    
    float computeHebbianChange(const SynapseState& pre, const SynapseState& post) {
        // Simple Hebbian rule: increase weight when both neurons spike
        if (pre.lastSpikeTime > 0 && post.lastSpikeTime > 0) {
            return 0.005f; // Hebbian learning rate
        }
        return 0.0f;
    }
};

} // namespace nlm
