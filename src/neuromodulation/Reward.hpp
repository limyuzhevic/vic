#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Real reward computation from observations and action outcomes
// Reward prediction error computation
// Reward history tracking and temporal integration

class Reward {
public:
    Reward();
    ~Reward();
    
    // Initialize with brain reference for context
    void initialize(class Brain* brain);
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward with temporal integration
    void add(float delta, float timeStep = 1.0f);
    
    // Compute reward from environment state and action outcome
    float computeReward(const class Observation& observation,
                       const class Action& action,
                       class Brain* brain) const;
    
    // Reward prediction error computation
    void computeRewardPredictionError(float predictedReward);
    float getRewardPredictionError() const;
    
    // Reset accumulated reward
    void reset();
    
    // Reward history tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Temporal integration methods
    void setIntegrationFactor(float factor);
    void setTimeConstant(float tc);
    
    // Reward modulation of plasticity
    float getPlasticityModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
