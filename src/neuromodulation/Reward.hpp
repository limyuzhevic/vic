#pragma once

#ifndef NLM_REWARD_HPP
#define NLM_REWARD_HPP

#include "Neuromodulator.hpp"

namespace nlm {

// Reward: Represents reward signals for reinforcement learning
// Computes and accumulates reward based on agent interactions with the environment
// PLACEHOLDER - Phase 2 will implement real reward computation from observations

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward over time
    void add(float delta);
    
    // Reset accumulated reward to zero
    void reset();
    
    // Compute reward signal from environment observation
    // TODO: Implement real reward computation based on observation content
    // Should evaluate if action was beneficial, harmful, or neutral
    float computeReward(const class Observation& observation) const;
    
    // Get reward history for analysis
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

#endif // NLM_REWARD_HPP