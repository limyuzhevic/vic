#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Enhanced implementation with reward integration and eligibility traces

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
    
    // Reward modulation parameters
    void setRewardGain(float gain);
    float getRewardGain() const;
    void setRewardSignal(float signal);
    float getRewardSignal() const;
    void setTemporalDifference(float td);
    float getTemporalDifference() const;
    
    // Reward modulation features
    void enableAccumulation(bool enable);
    void enableEligibilityTrace(bool enable);
    void setAccumulationDecay(float decay);
    void setEligibilityDecay(float decay);
    
    bool isAccumulationEnabled() const;
    bool isEligibilityTraceEnabled() const;
    float getAccumulationDecay() const;
    float getEligibilityDecay() const;
    
    // Integration with base STDP
    void setBaseSTDP(class STDP* stdp);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm