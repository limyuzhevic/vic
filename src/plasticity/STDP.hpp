#pragma once

#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Improved implementation with reward modulation and triplet interactions

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
    
    // Reward-modulated STDP parameters
    void setRewardFactor(float factor);      // Reward modulation factor
    float getRewardFactor() const;
    void setRewardTraceDecay(float decay);   // Reward trace decay rate
    float getRewardTraceDecay() const;
    
    // Get eligibility trace for reward-modulated learning
    float getEligibilityTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Properly implemented with biological accuracy
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
    
    // Reward modulation parameters
    void setLearningRate(float rate);        // Base learning rate
    float getLearningRate() const;
    void setDiscountFactor(float factor);     // Reward discount factor
    float getDiscountFactor() const;
    void setTraceDecay(float decay);          // Eligibility trace decay
    float getTraceDecay() const;
    
    // Reward-based plasticity
    void applyReward(float reward);           // Apply reward signal
    float getReward() const;
    void resetReward();                       // Reset reward for next episode
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Properly implemented with biological accuracy
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
    
    // Triplet interaction parameters
    void setAplus(float a);                   // Triple potentiation parameter
    float getAplus() const;
    void setAminus(float a);                  // Triple depression parameter
    float getAminus() const;
    void setTauPlus(float tau);               // Triple potentiation time constant
    float getTauPlus() const;
    void setTauMinus(float tau);              // Triple depression time constant
    float getTauMinus() const;
    
    // Pairwise STDP baseline (for comparison)
    void setBaselineLTPWeight(float w);       // Baseline LTP weight
    float getBaselineLTPWeight() const;
    void setBaselineLTDWeight(float w);       // Baseline LTD weight
    float getBaselineLTDWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
