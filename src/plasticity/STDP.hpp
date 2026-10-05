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
// Implements spike-timing dependent plasticity modulated by reward signals
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
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setRewardModulation(float modulation);
    float getRewardModulation() const;
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau, float learningRate, float rewardModulation);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Implements triplet-based STDP for presynaptic and postsynaptic spike timing
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
    void setA1Plus(float a1Plus);      // Presynaptic trace contribution
    float getA1Plus() const;
    void setA2Plus(float a2Plus);      // Postsynaptic trace contribution
    float getA2Plus() const;
    void setA1Minus(float a1Minus);    // Presynaptic depression trace
    float getA1Minus() const;
    void setA2Minus(float a2Minus);    // Postsynaptic depression trace
    float getA2Minus() const;
    void setTauPlus(float tauPlus);    // Time constant for potentiation
    float getTauPlus() const;
    void setTauMinus(float tauMinus);  // Time constant for depression
    float getTauMinus() const;
    
    // Update parameters
    void configure(float a1Plus, float a2Plus, float a1Minus, float a2Minus, float tauPlus, float tauMinus);
    
protected:  // Allow subclasses to access cleanTraces
    void cleanTraces();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
