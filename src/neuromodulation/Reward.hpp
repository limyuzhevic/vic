#pragma once

#include "Neuromodulator.hpp"
#include "../environment/Observation.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Enhanced implementation for Phase 6 integration

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
    // TODO PHASE 2: Implement real reward computation from observation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Update reward system over time
    void update(float delta_t);
    
    // Reward components for neuromodulation
    float getExtrinsicReward() const;
    void setExtrinsicReward(float reward);
    
    float getIntrinsicReward() const;
    void setIntrinsicReward(float reward);
    
    float getExplorationReward() const;
    void setExplorationReward(float reward);
    
    float getPredictionReward() const;
    void setPredictionReward(float reward);
    
    // Reward learning system
    float getBaselineReward() const;
    void setBaselineReward(float reward);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
