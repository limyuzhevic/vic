// RewardModulatedSTDP header
#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Combines spike-timing dependence with reward prediction error
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
    void setRewardPredictionError(float rpe);    // Reward prediction error (-1 to +1)
    float getRewardPredictionError() const;
    void setLTPWeight(float weight);              // Base A+ for potentiation
    float getLTPWeight() const;
    void setLTDWeight(float weight);              // Base A- for depression
    float getLTDWeight() const;
    void setTimeConstant(float tau);              // STDP time constant (ms)
    float getTimeConstant() const;
    void setMinWeight(float minWeight);           // Minimum synaptic weight
    float getMinWeight() const;
    void setMaxWeight(float maxWeight);           // Maximum synaptic weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
