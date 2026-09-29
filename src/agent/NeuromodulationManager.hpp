#pragma once

#include "../core/Types/Types.hpp"
#include "AgentBrainConstants.h"
#include <vector>

namespace nlm {

class NeuromodulationManager {
public:
    NeuromodulationManager() = default;
    ~NeuromodulationManager() = default;
    
    // Initialize neuromodulation state
    void initialize();
    
    // Apply reward modulation based on reward and predicted reward
    void applyRewardModulation(
        float reward,
        float predictedReward,
        class Brain* brain,
        float plasticityModifier,
        float& dopamineLevel,
        float& predictionError,
        float& expectedReward);
    
    // Calculate plasticity factor from dopamine level
    static float calculatePlasticityFactor(float dopamineLevel);
    
    // Validate neuromodulation parameters
    static bool validateRewardModulationParams(float reward, float predictedReward);
    
    // Get neuromodulation statistics
    float getDopamineLevel() const { return dopamineLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    
private:
    float dopamineLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
};

} // namespace nlm