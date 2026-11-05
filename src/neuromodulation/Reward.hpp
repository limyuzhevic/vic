#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Forward declarations to avoid circular dependencies
class Observation;
class AgentBrain;
class World;

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
    float computeReward(const Observation& observation) const;
    
    // Advanced reward computation with full context
    // Phase 6: Enhanced reward computation
    void computeAdvancedReward(const Observation& observation,
                              const AgentBrain& agent,
                              const World& world,
                              float timestep);
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
