#pragma once

#include "STDP.hpp"
#include "../../core/Random/Random.hpp"
#include <vector>
#include <cmath>

namespace nlm {

// Reward-Modulated STDP (R-STDP)
// Combines spike-timing dependence with reward prediction error signals
// Implements eligibility traces for temporal credit assignment
// Based on dopamine-modulated learning in biological systems
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // Update with spike timing and reward modulation
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override;
    
    // Reward modulation parameters
    void setRewardStrength(float strength);    // Learning rate modulation by reward
    float getRewardStrength() const;
    
    void setTDDiscount(float discount);        // Temporal discount for eligibility traces
    float getTDDiscount() const;
    
    // Eligibility trace management
    void resetEligibilityTrace(Synapse* synapse);
    float getEligibilityTrace(Synapse* synapse) const;
    
    // Reward-based learning parameters
    void configure(float ltpWeight, float ltdWeight, float timeConstant,
                   float rewardStrength, float tDiscount);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
