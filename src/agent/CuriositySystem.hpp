#pragma once

#include "../core/Types/Types.hpp"
#include "AgentBrainConstants.h"
#include <algorithm>

namespace nlm {

class CuriositySystem {
public:
    CuriositySystem() = default;
    ~CuriositySystem() = default;
    
    // Initialize curiosity state
    void initialize();
    
    // Update curiosity based on novelty and prediction error
    void updateCuriosity(
        float noveltyLevel,
        float predictionError,
        bool curiosityEnabled,
        float& curiosityLevel);
    
    // Calculate exploration chance based on curiosity level
    static float calculateExplorationChance(float curiosityLevel);
    
    // Generate random motor command for exploration
    static MotorCommand generateRandomMotorCommand(class RandomGenerator* rng);
    
    // Validate curiosity parameters
    static bool validateCuriosityParams(float noveltyLevel, float predictionError);
    
    // Get curiosity statistics
    float getCuriosityLevel() const { return curiosityLevel_; }
    
private:
    float curiosityLevel_ = 0.0f;
};

} // namespace nlm