#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
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
    
    // Update reward system
    void update(TimestepDuration dt);
    
    // Compute reward from environment state
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Get exploration drive for curiosity-based behavior
    float getExplorationDrive() const { return 0.0f; }
    
    // Get novelty detection level
    float getNoveltyLevel() const { return 0.0f; }
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
