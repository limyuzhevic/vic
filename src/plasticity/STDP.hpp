#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real STDP implementation based on spike-timing correlation
// Updates synaptic weights based on relative timing of pre- and post-synaptic spikes

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
// Implements reward-modulated spike-timing dependent plasticity using eligibility traces
// Extends standard STDP with reward prediction error signals
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
    
    // Parameters
    void setSTDPWeight(float weight);
    float getSTDPWeight() const;
    void setRewardWeight(float weight);
    float getRewardWeight() const;
    void setRewardModulationTau(float tau);
    float getRewardModulationTau() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP: implements triplet spike interaction rules
// Extends STDP with first-order and second-order spike interactions
// Captures more complex spike timing correlations in neural activity
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
    
    // Parameters
    void setPreTraceTimeConstant(float tau1);
    float getPreTraceTimeConstant() const;
    void setPostTraceTimeConstant(float tau2);
    float getPostTraceTimeConstant() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setThirdOrderLearningRate(float rate);
    float getThirdOrderLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
