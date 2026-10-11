#pragma once

#include "STDP.hpp"

namespace nlm {

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
    void setBaselineLTP(float weight);    // Baseline LTP amplitude
    float getBaselineLTP() const;
    void setBaselineLTD(float weight);    // Baseline LTD amplitude
    float getBaselineLTD() const;
    void setLander(float lander);         // LTP landing rate
    float getLander() const;
    void setLanderr(float landerr);       // LTD landing rate
    float getLanderr() const;
    void setRewardLearningRate(float rate);  // Reward modulation strength
    float getRewardLearningRate() const;
    
    // Reward modulation
    void setPredictionError(float error);   // Prediction error δ
    float getPredictionError() const;
    
    // Configuration helper
    void configure(float ltp, float ltd, float lander, float landerr, float rewardRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
