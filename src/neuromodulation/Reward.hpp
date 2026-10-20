#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Phase 6: Implemented real reward computation that considers:
// - Agent state (energy, health, position)
// - Environment features (resources, hazards)
// - Agent position relative to boundaries and desirable states
// - Motivational state based on needs (energy, health)

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
    // Returns a value between -1.0 and 1.0
    float computeReward(const Observation& observation) const;
    
    // Compute reward from environment state and agent
    // Returns a value between -1.0 and 1.0 based on:
    // - Agent's energy and health (positive reward)
    // - Agent's position relative to boundaries (positive reward)
    // - Resource presence in observation (positive reward)
    // - Motivation to seek resources (negative reward when low)
    float computeReward(const Observation& observation, const AgentBody& agent) const;
    
    // Reward history for analysis
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Neuromodulator interface
    const char* getName() const override { return "Reward"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
