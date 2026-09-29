#include "NeuromodulationManager.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

void NeuromodulationManager::initialize() {
    dopamineLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
}

void NeuromodulationManager::applyRewardModulation(
    float reward,
    float predictedReward,
    class Brain* brain,
    float plasticityModifier,
    float& dopamineLevel,
    float& predictionError,
    float& expectedReward) {
    
    if (!brain) {
        return;
    }
    
    // Validate inputs
    if (!validateRewardModulationParams(reward, predictedReward)) {
        return;
    }
    
    // Compute prediction error
    predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward = 0.95f * expectedReward + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel = predictionError;
    dopamineLevel = std::clamp(dopamineLevel, AgentBrainConstants::DOPAMINE_CLAMP_MIN, 
                               AgentBrainConstants::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::REWARD_ELIGIBILITY_DECAY);
            }
        }
    }
}

float NeuromodulationManager::calculatePlasticityFactor(float dopamineLevel) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

bool NeuromodulationManager::validateRewardModulationParams(float reward, float predictedReward) {
    // Check for NaN or infinite values
    if (std::isnan(reward) || std::isnan(predictedReward) ||
        std::isinf(reward) || std::isinf(predictedReward)) {
        return false;
    }
    return true;
}

} // namespace nlm