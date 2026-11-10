#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Computes meaningful rewards based on environment observations and agent behavior

class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // Integrates with brain state, action outcomes, and prediction errors
    float computeReward(const class Observation& observation,
                        const class Brain& brain,
                        const class Action& action,
                        float predictionError) const;
    
    // Reward history and statistics
    const std::vector<float>& getHistory() const;
    void clearHistory();
    float getAverageReward() const;
    float getMaxReward() const;
    float getMinReward() const;
    
    // Reward modulation
    void applyRewardToPlasticity(float reward, Neuromodulator& neuromodulator) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
