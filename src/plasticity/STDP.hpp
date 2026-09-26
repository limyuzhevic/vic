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
// Integrates STDP with reward signals using eligibility traces
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
    
    // Reward integration parameters
    void setRewardFactor(float factor);
    float getRewardFactor() const;
    void setRewardDecayRate(float rate);
    float getRewardDecayRate() const;
    
    // Update with reward signal
    void updateWithReward(Synapse* synapse, float reward, Timestamp currentTime);
    
    // Base STDP rule for this variant
    void setBaseSTDP(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Captures more complex spike interaction patterns beyond pairwise STDP
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
    
    // Triplet model parameters
    void setA1(float a1);     // First spike interaction term
    float getA1() const;
    void setA2(float a2);     // Second spike interaction term  
    float getA2() const;
    void setB1(float b1);     // Second spike facilitation
    float getB1() const;
    void setB2(float b2);     // First spike facilitation
    float getB2() const;
    void setTau1(float tau);  // Time constant for facilitation
    float getTau1() const;
    void setTau2(float tau);  // Time constant for depression
    float getTau2() const;
    
    // Base STDP parameters
    void setBaseSTDP(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
