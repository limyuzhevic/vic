#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-Modulated STDP (R-STDP)
// Real STDP implementation that incorporates reward prediction error
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
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Reward modulation
    void setRewardModulator(float modulator);
    float getRewardModulator() const;
    
    // Configure all parameters
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm