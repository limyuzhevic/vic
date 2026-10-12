#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Phase 3+: Implements real reward computation and tracking
//
// Reward signals are a fundamental component of reinforcement learning,
// providing positive and negative feedback to guide learning.
//
// Key functions:
// - Accumulate reward from environment interactions
// - Compute reward based on environmental states
// - Track reward history for analysis and credit assignment
// - Integrate with neuromodulatory systems (dopamine, etc.)
//
// The reward system supports:
// - Temporal difference learning
// - Reward prediction error computation
// - Exploration-exploitation tradeoffs
// - Motivation and value-based decision making
//
// Implementation notes:
// - Reward signals can be scalar (single value) or vector-valued
// - Reward computation depends on environmental state representation
// - Reward can be immediate (step-based) or delayed
// - Integration with prediction error systems enables temporal abstraction
//
// TODO PHASE 2: Implement real reward computation
// - Implement proper reward function based on environmental states
// - Add support for sparse vs dense reward signals
// - Implement reward normalization and scaling
// - Integrate with reward prediction error systems
// - Add reward discounting for delayed rewards
// - Implement reward credit assignment across time steps

class Reward {
public:
    // Default constructor
    Reward();
    
    // Destructor
    ~Reward();
    
    // Get current accumulated reward value
    // @return Current reward value
    float getValue() const;
    
    // Set current reward value directly
    // @param value New reward value (typically overrides accumulated value)
    void setValue(float value);
    
    // Add reward to accumulated total
    // @param delta Reward delta to add (can be positive or negative)
    void add(float delta);
    
    // Reset accumulated reward to zero
    // Useful for experiment restarts or episode boundaries
    void reset();
    
    // Compute reward based on environmental observation
    // @param observation Current environmental state
    // @return Computed reward value (typically 0.0 to 1.0 normalized)
    // TODO PHASE 2: Implement real reward computation
    // Implement proper reward function:
    // 1. Map environmental observations to reward signals
    // 2. Apply appropriate reward shaping
    // 3. Normalize rewards for stable learning
    // 4. Consider temporal credit assignment
    // 5. Integrate with prediction error signals
    float computeReward(const class Observation& observation) const;
    
    // Get reward history for analysis and debugging
    // @return Reference to internal reward history vector
    const std::vector<float>& getHistory() const;
    
    // Clear reward history
    // Useful for experiment restarts
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
