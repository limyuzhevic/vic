#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2 will implement real reward computation

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
    void reset();
    
    // Compute reward from environment state
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Neuromodulator interface implementation
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
