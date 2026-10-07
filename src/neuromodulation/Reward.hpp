#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Implements reward as a neuromodulator for integration with brain systems

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Get current reward value (overrides Neuromodulator::getLevel)
    float getValue() const;
    void setValue(float value);
    
    // Neuromodulator interface overrides
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
