#pragma once

#include "Neuromodulator.hpp"
#include <memory>

namespace nlm {

// Reward signal for reinforcement learning
// Implements reward computation and accumulation
// Part of the neuromodulation system

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
