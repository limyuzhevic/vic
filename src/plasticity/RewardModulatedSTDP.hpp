#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-Modulated STDP (R-STDP)
// Combines spike timing with reward signals for credit assignment
// Essential for reinforcement learning in spiking neural networks

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
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Reward-modulated learning parameters
    void setEligibilityDecay(float decay);  // Eligibility trace decay rate
    float getEligibilityDecay() const;
    void setRewardScale(float scale);       // Reward signal scaling factor
    float getRewardScale() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm