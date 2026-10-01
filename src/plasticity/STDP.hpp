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
// Real STDP implementation that incorporates reward prediction error
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
    
    // Reward modulation
    void setRewardModulator(float modulator);
    float getRewardModulator() const;
    
    // Configure all parameters
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Real implementation with synaptic history
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
    
    // STDP parameters
    void setLTPWeight1(float weight);
    float getLTPWeight1() const;
    void setLTDWeight1(float weight);
    float getLTDWeight1() const;
    void setLTPWeight2(float weight);
    float getLTPWeight2() const;
    void setLTDWeight2(float weight);
    float getLTDWeight2() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Configure all parameters
    void configure(float ltpWeight1, float ltdWeight1, float ltpWeight2, float ltdWeight2);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule
// Real implementation of sliding threshold learning
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // BCM parameters
    void setSigma(float sigma);          // Sliding threshold
    float getSigma() const;
    void setTauPlus(float tau);          // LTP time constant
    float getTauPlus() const;
    void setTauMinus(float tau);         // LTD time constant
    float getTauMinus() const;
    void setLearningRate(float rate);    // Learning rate
    float getLearningRate() const;
    
    // Configure all parameters
    void configure(float sigma, float tauPlus, float tauMinus, float learningRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
