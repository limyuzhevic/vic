#pragma once

#include "../core/Types/Types.hpp"
#include "../prediction/PredictionSystem.hpp"

namespace nlm {

// Dopamine signal for reward prediction error and motivation
// Integrates with prediction system for reward-modulated learning
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Get current dopamine level (0-1)
    float getLevel() const;
    
    // Set dopamine level
    void setLevel(float level);
    
    // Get plasticity factor for prediction learning
    float getPlasticityFactor() const;
    
    // Get prediction error signal for neuromodulation
    float getPredictionError() const;
    
    // Update dopamine based on reward prediction error
    // Uses prediction system error signals
    void update(float predictionError, float rewardPredictionError, TimestepDuration dt);
    
    // Reset dopamine system
    void reset();
    
    // Enable/disable reward-modulated learning
    void enableRewardModulatedLearning(bool enable);
    
    // Get dopamine statistics
    struct DopamineStats {
        float currentLevel;
        float meanLevel;
        float maxLevel;
        float meanPredictionError;
        size_t updateCount;
    };
    DopamineStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
