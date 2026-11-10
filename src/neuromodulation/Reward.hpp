#pragma once

#include "Neuromodulator.hpp"

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
    // ACTION: Implement biologically plausible reward computation
    // MODEL: Reinforcement learning with shaping and generalization
    // FORMULA: reward = utility + exploration bonus - penalty
    // PARAMETER: Include reward prediction error components
    // NEED: Integration with environment and neuromodulation
    // TIMELINE: Implement within 3 weeks for learning systems
    // BENEFIT: Enables adaptive decision-making and learning
    // PRIORITY: HIGH - Core reinforcement learning component
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
