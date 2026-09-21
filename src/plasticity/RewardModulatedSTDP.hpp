#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"

namespace nlm {

// Reward-Modulated STDP
// Implements spike-timing dependent plasticity gated by reward signals
// Combines timing-based learning with temporal credit assignment

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
    
    // STDP parameters (inherited from STDP)
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Reward-modulated STDP parameters
    void setLearningRate(float rate);    // Learning rate modulated by reward
    float getLearningRate() const;
    void setTraceDecayRate(float decay); // Eligibility trace decay rate
    float getTraceDecayRate() const;
    void setRewardSensitivity(float sens); // How sensitive to reward signals
    float getRewardSensitivity() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau, float rate, float decay, float sens);
    
    // Reward modulation
    void onRewardReceived(float reward);
    float getRecentReward() const;
    
private:
    struct Impl;
    Impl* pImpl;
    float recentReward_;
};

} // namespace nlm