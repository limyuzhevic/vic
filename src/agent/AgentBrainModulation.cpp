#include "AgentBrainModulation.hpp"
#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {
namespace modulation {

void AgentBrainModulation::applyRewardModulation(AgentBrain& agentBrain, float reward, float predictedReward) {
    if (!agentBrain.brain_ || !agentBrain.rewardModulationEnabled_) return;
    
    // Compute prediction error
    agentBrain.predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    agentBrain.expectedReward_ = 0.95f * agentBrain.expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    agentBrain.dopamineLevel_ = agentBrain.predictionError_;
    
    // Clamp to reasonable range
    agentBrain.dopamineLevel_ = std::clamp(agentBrain.dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : agentBrain.brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * agentBrain.dopamineLevel_ * agentBrain.plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * agentBrain.dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = agentBrain.brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

} // namespace modulation
} // namespace nlm
