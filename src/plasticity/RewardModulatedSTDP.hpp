#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Combines standard STDP with reward signals using eligibility traces
// Implements dopamine-dependent plasticity for reinforcement learning
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward modulation parameters
    void setLearningRate(float rate);    // Overall learning rate (default: 0.01)
    float getLearningRate() const;
    void setRewardFactor(float factor);   // Reward modulation strength (default: 1.0)
    float getRewardFactor() const;
    void setTraceDecay(float decay);     // Eligibility trace decay time constant (ms, default: 100)
    float getTraceDecay() const;
    
    // Update parameters from config
    void configure(float learningRate, float rewardFactor, float traceDecay);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm