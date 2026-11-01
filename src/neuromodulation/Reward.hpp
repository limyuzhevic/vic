#pragma once

#include "Neuromodulator.hpp"
#include "../environment/Observation.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Phase 4/5: Implemented real reward computation from observations

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
    float computeReward(const Observation& observation) const;
    
    // Compute novelty from observation (for curiosity bonus)
    float getNoveltyFromObservation(const Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
