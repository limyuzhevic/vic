#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real STDP implementation based on biological principles
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synapse weights based on spike timing
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply a direct weight change (used for reward-modulated learning)
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (τ) in ms
    float getTimeConstant() const;
    
    // Update all parameters from configuration
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Calculate eligibility trace for reward-modulated learning
    float calculateEligibilityTrace(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    float tau) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Integrates STDP with neuromodulatory reward signals
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // Update with spike timing and reward modulation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward modulation parameters
    void setLearningRate(float rate);    // Overall learning rate
    float getLearningRate() const;
    void setRewardThreshold(float threshold);  // Reward threshold for plasticity
    float getRewardThreshold() const;
    
    // Set the neuromodulatory signal (dopamine level)
    void setNeuromodulator(float value);
    float getNeuromodulator() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// More biologically accurate model including presynaptic and postsynaptic spikes
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    // Update synapse weights using triplet STDP rules
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet STDP parameters
    void setPreTraceTimeConstant(float tau);    // Presynaptic trace decay
    float getPreTraceTimeConstant() const;
    void setPostTraceTimeConstant(float tau);   // Postsynaptic trace decay
    float getPostTraceTimeConstant() const;
    void setSTPWeight(float weight);            // Short-term plasticity weight
    float getSTPWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
