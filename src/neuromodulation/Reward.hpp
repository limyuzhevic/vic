#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2 will implement real reward computation

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
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Enhanced reward computation with environmental analysis
    float computeEnvironmentalReward(const class Observation& observation) const;
    
    // Integrated reward combining environmental and neuromodulatory signals
    float computeIntegratedReward(const class Observation& observation, 
                                  float neuromodulationLevel) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
