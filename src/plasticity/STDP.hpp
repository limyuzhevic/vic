#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Implements STDP with reward prediction error modulation
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

// Spike-timing dependent plasticity with triplet interactions
// Implements triplet STDP with pre-post pair interactions
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet parameters
    void setA1(float weight);        // First trace amplitude
    float getA1() const;
    void setA2(float weight);        // Second trace amplitude
    float getA2() const;
    void setA3(float weight);        // Third trace amplitude
    float getA3() const;
    void setTau1(float tau);         // First trace time constant
    float getTau1() const;
    void setTau2(float tau);         // Second trace time constant
    float getTau2() const;
    
    // Configuration helper
    void configure(float a1, float a2, float a3, float tau1, float tau2);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
