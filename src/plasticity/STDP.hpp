#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Implements STDP based on precise spike timing differences
// Pre-before-post spikes cause potentiation (LTP), post-before-pre cause depression (LTD)

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on spike timing differences
    // Implements the classic STDP rule from Bi & Poo (1998)
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply accumulated weight change
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight()までは depression weight)
    void setTimeConstant(float tau);    // STDP time constant (ms) for exponential decay
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Combines STDP with reward signals using eligibility traces
// Weight changes only occur if reward prediction error is positive
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // Update synaptic weights based on spike timing AND reward prediction
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply reward-modulated weight change
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Reward parameters
    void setLearningRate(float rate);        // Overall learning rate
    float getLearningRate() const;
    void setDiscountFactor(float factor);    // Eligibility trace decay
    float getDiscountFactor() const;
    void setPredictionError(float error);    // Reward prediction error
    float getPredictionError() const;
    
    // Reset reward state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Extends classic STDP to include contributions from spike triplets
// Accounts for short-term synaptic plasticity and causal relationships
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    // Update synaptic weights using triplet STDP rules
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply triplet STDP weight change
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Triplet parameters
    void setAPlusPre(float weight);     // Pre-synaptic triplet potentiation
    float getAPlusPre() const;
    void setAMinusPost(float weight);   // Post-synaptic triplet depression
    float getAMinusPost() const;
    void setATau(float tau);            // Time constant for triplet effects
    float getATau() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
