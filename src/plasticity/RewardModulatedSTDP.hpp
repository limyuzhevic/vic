#pragma once

#include "STDP.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Implements reward-modulated learning with eligibility traces

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // R-STDP parameters
    void setLearningRate(float rate);    // Learning rate for reward modulation
    float getLearningRate() const;
    void setTraceDecay(float decay);     // Eligibility trace decay rate
    float getTraceDecay() const;
    void setRewardScale(float scale);    // Reward signal scaling factor
    float getRewardScale() const;
    
    // Update parameters from config
    void configure(float learningRate, float traceDecay, float rewardScale);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm