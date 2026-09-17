// Reward.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Reward: Reward prediction error signal from dopamine system
// Integrates prediction error with synaptic eligibility traces for learning

class Reward {
public:
    Reward();
    ~Reward();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Signal received reward
    void signalReward(float reward);
    
    // Signal reward prediction error
    void signalRewardPredictionError(float error);
    
    // Get current reward level
    float getLevel() const;
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get plasticity factor for STDP
    float getPlasticityFactor() const;
    
    // Update reward system state
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm