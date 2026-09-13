#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Implements dopamine-dependent modulation of STDP
// Combines spike-timing with reward prediction error

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
    
    // Neuromodulation parameters
    void setDopamineGain(float gain);
    float getDopamineGain() const;
    void setDAWeight(float weight);
    float getDAWeight() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Reward prediction error integration
    void setRewardPredictionError(float rpe);
    float getRewardPredictionError() const;
    
    // Eligibility trace management
    void setTraceDecay(float decay);
    float getTraceDecay() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm