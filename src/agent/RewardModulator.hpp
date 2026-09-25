#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include <cmath>

namespace nlm {

class RewardModulator {
public:
    RewardModulator();
    ~RewardModulator();
    
    // Apply reward modulation based on prediction error
    void applyRewardModulation(
        float reward,
        float predictedReward,
        float plasticityModifier = 1.0f,
        bool rewardModulationEnabled = true
    );
    
    // Get neuromodulation level (dopamine-like signal)
    float getDopamineLevel() const { return dopamineLevel_; }
    
    // Get prediction error
    float getPredictionError() const { return predictionError_; }
    
    // Get expected reward (for next time)
    float getExpectedReward() const { return expectedReward_; }
    
    // Reset modulation state
    void reset() {
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        dopamineLevel_ = 0.0f;
    }
    
    // Configuration
    void setRewardModulationEnabled(bool enabled) { rewardModulationEnabled_ = enabled; }
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Plasticity factor calculation
    float calculatePlasticityFactor(float dopamineLevel) const;

private:
    // Neuromodulation state
    float dopamineLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    float plasticityModifier_;
};

} // namespace nlm