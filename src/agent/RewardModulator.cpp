#include "RewardModulator.hpp"
#include "../core/Constants.hpp"

namespace nlm {

RewardModulator::RewardModulator()
    : dopamineLevel_(0.0f), predictionError_(0.0f), expectedReward_(0.0f),
      rewardModulationEnabled_(true), plasticityModifier_(1.0f) {}

RewardModulator::~RewardModulator() {}

void RewardModulator::applyRewardModulation(
    float reward,
    float predictedReward,
    float plasticityModifier,
    bool rewardModulationEnabled) {
    
    if (!rewardModulationEnabled) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Store plasticity modifier for later use
    plasticityModifier_ = plasticityModifier;
}

float RewardModulator::calculatePlasticityFactor(float dopamineLevel) const {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    return std::clamp(plasticityFactor, PlasticityConstants::PLASTICITY_MODIFIER_MIN, 
                     PlasticityConstants::PLASTICITY_MODIFIER_MAX);
}

} // namespace nlm