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
// Implements reward-modulated plasticity using dopamine signals
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
    void setLearningRate(float rate);    // Base learning rate
    float getLearningRate() const;
    void setTraceDecay(float decay);     // Eligibility trace decay rate
    float getTraceDecay() const;
    void setRewardModulation(float modulation); // Reward modulation strength
    float getRewardModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Captures spike history for more complex learning dynamics
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
    
    // Triplet STDP parameters
    void setAPlus(float weight);          // Potentiation amplitude
    float getAPlus() const;
    void setAMinus(float weight);         // Depression amplitude
    float getAMinus() const;
    void setTauPlus(float tau);           // Potentiation time constant
    float getTauPlus() const;
    void setTauMinus(float tau);          // Depression time constant
    float getTauMinus() const;
    void setAPlusPrime(float weight);     // Triplet A+'
    float getAPlusPrime() const;
    void setBPlus(float weight);          // Triplet B+
    float getBPlus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
