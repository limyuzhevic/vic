#pragma once

#include "Neuromodulator.hpp"
#include "../world/Observation.hpp"
#include <vector>

namespace nlm {

// Reward signal for reinforcement learning
// Real implementation with observation-based reward computation

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset() override;
    
    // Compute reward from observation
    float computeReward(const Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Integration with prediction system
    void setTargetValue(float target);
    void setLearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
