#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Implements reward-modulated plasticity using eligibility traces
// Combines spike timing with reward signals to enable delayed reward learning
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
    
    // Reward-modulated parameters
    void setBaseLearningRate(float rate); // Base learning rate (η)
    float getBaseLearningRate() const;
    void setRewardSensitivity(float sensitivity); // Reward sensitivity (γ) - scales plasticity by reward
    float getRewardSensitivity() const;
    void setTraceDecayRate(float decayRate); // Decay rate for eligibility traces (λ)
    float getTraceDecayRate() const;
    
    // Update parameters from config
    void configure(float learningRate, float rewardSensitivity, float traceDecay);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm