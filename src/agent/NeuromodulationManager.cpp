#include "NeuromodulationManager.hpp"

namespace nlm {

NeuromodulationManager::NeuromodulationManager() 
    : dopamineLevel_(0.0f), predictionError_(0.0f), expectedReward_(0.0f), 
      plasticityModifier_(1.0f), eligibilityDecay_(0.1f), weightChangeRate_(1.0f),
      plasticitySensitivity_(0.5f), dopamineClampMin_(-1.0f), dopamineClampMax_(1.0f),
      lastWeightChangeDelta_(0.0f) { }

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::applyRewardModulation(float reward, float predictedReward) {
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Compute dopamine-like signal (based on prediction error)
    dopamineLevel_ = computeDopamineSignal(predictionError_);
    
    NLM_LOG_DEBUG("NeuromodulationManager: Applied reward modulation - "
                 "reward: " + std::to_string(reward) + 
                 ", predicted: " + std::to_string(predictedReward) + 
                 ", prediction error: " + std::to_string(predictionError_) + 
                 ", dopamine: " + std::to_string(dopamineLevel_));
    
    // Apply to all synapses with eligibility traces
    applyRewardModulatedWeights(dopamineLevel_, plasticityModifier_);
    
    // Modulate plasticity factors based on dopamine
    modulatePlasticityFactors(dopamineLevel_, plasticityModifier_);
}

float NeuromodulationManager::computeDopamineSignal(float predictionError) const {
    // Simple dopamine-like signal based on prediction error
    float dopamine = predictionError;
    
    // Clamp to reasonable range
    dopamine = std::clamp(dopamine, dopamineClampMin_, dopamineClampMax_);
    
    NLM_LOG_DEBUG("NeuromodulationManager: Computed dopamine signal " + std::to_string(dopamine) +
                 " from prediction error " + std::to_string(predictionError));
    
    return dopamine;
}

void NeuromodulationManager::updateEligibilityTraces(float decayRate) {
    if (!brain_) return;
    
    decayRate = std::clamp(decayRate, 0.0f, 1.0f);
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            float newEligibility = eligibility * (1.0f - decayRate);
            syn->setEligibilityTrace(newEligibility);
            
            if (std::abs(newEligibility) < 0.001f) {
                syn->setEligibilityTrace(0.0f);
            }
        }
    }
    
    NLM_LOG_DEBUG("NeuromodulationManager: Updated eligibility traces with decay " + 
                 std::to_string(decayRate));
}

void NeuromodulationManager::applyRewardModulatedWeights(float dopamineLevel, float plasticityFactor) {
    if (!brain_) return;
    
    lastWeightChangeDelta_ = 0.0f;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityFactor;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(eligibilityDecay_);
                
                lastWeightChangeDelta_ += std::abs(delta);
            }
        }
    }
    
    if (lastWeightChangeDelta_ > 0.0f) {
        NLM_LOG_DEBUG("NeuromodulationManager: Applied weight changes with total delta " + 
                     std::to_string(lastWeightChangeDelta_));
    }
}

void NeuromodulationManager::modulatePlasticityFactors(float dopamineLevel, float& plasticityModifier) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    plasticityModifier *= plasticityFactor * plasticitySensitivity_;
    plasticityModifier = std::clamp(plasticityModifier, 0.1f, 2.0f);
    
    NLM_LOG_DEBUG("NeuromodulationManager: Modulated plasticity modifier to " + 
                 std::to_string(plasticityModifier) + 
                 " (dopamine: " + std::to_string(dopamineLevel) + ")");
}

void NeuromodulationManager::reset() {
    dopamineLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("NeuromodulationManager: Reset");
}

} // namespace nlm
