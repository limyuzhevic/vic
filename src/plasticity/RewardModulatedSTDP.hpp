#pragma once

#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Implements reward-modulated spike-timing dependent plasticity using eligibility traces
// Extends standard STDP with reward prediction error signals
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
    
    // Parameters
    void setSTDPWeight(float weight);
    float getSTDPWeight() const;
    void setRewardWeight(float weight);
    float getRewardWeight() const;
    void setRewardModulationTau(float tau);
    float getRewardModulationTau() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm