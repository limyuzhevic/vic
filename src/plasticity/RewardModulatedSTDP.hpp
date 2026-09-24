#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-Modulated STDP (R-STDP) implementation
// Implements reward-modulated spike timing dependence
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
    
    // R-STDP parameters
    void setBaseLTPWeight(float weight);
    float getBaseLTPWeight() const;
    void setBaseLTDWeight(float weight);
    float getBaseLTDWeight() const;
    void setRewardModulationFactor(float factor);
    float getRewardModulationFactor() const;
    void setBaseTimeConstant(float tau);
    float getBaseTimeConstant() const;
    
    // Reward signal
    void setReward(float reward);
    float getReward() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm