#pragma once

#include "../sensory/SensoryInput.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Implements real reward computation based on environmental observations

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
    // Analyzes sensory input to compute meaningful rewards
    float computeReward(const SensoryInput& sensoryInput) const;
    
    // Compute reward from observation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get accumulated reward (lifetime total)
    float getAccumulatedReward() const;
    
    // Set reward parameters
    void setWeight(float weight);
    void setBaseline(float baseline);
    
    // Get reward parameters
    float getWeight() const;
    float getBaseline() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
