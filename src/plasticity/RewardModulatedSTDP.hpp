#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Combines spike timing with reward signals for credit assignment
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
    
    // Reward modulation parameters
    void setRewardScalar(float scalar);
    float getRewardScalar() const;
    
    void setRecentReward(float reward);
    float getRecentReward() const;
    
    void setBaselineLearningRate(float rate);
    float getBaselineLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm