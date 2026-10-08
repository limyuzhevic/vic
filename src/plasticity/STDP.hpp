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
// Combines STDP with reward signals for model-based learning
// Based on: Wu et al. (2010), "Reward-modulated spike-timing-dependent plasticity"

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
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    void setRewardRate(float rate);     // Reward modulation rate
    float getRewardRate() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau, float rewardRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Implements "neurons that fire together, divide together"
// Based on: Desai et al. (1996), "Competitive synaptic plasticity in visual cortex"

class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMinMaxWeights(float minWeight, float maxWeight);
    void setMinWeight(float minWeight);
    void setMaxWeight(float maxWeight);
    float getMinWeight() const;
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP with presynaptic and postsynaptic spike history
// Advanced STDP incorporating triplet interactions (Gerstner et al., 1996)
// Captures presynaptic spike history interactions

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
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstants(float tauPre, float tauPost, float tau);
    void setTripletWeights(float aPlus, float aMinus, float bPlus, float bMinus);
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tauPre, float tauPost, float tau, float aPlus, float aMinus, float bPlus, float bMinus);
    
private:
    struct Impl;
    Impl* pImpl;
};

// BCM (Bienenstock-Cooper-Munro) rule
// Homeostatic synaptic scaling based on postsynaptic activity
// Implements synaptic competition while maintaining stability

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
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setBeta(float beta);          // Stability parameter
    float getBeta() const;
    void setTheta(float theta);        // Sliding threshold
    float getTheta() const;
    void setMinMaxWeights(float minWeight, float maxWeight);
    void setMinWeight(float minWeight);
    void setMaxWeight(float maxWeight);
    float getMinWeight() const;
    float getMaxWeight() const;
    
    // Update parameters from config
    void configure(float beta, float theta, float learningRate, float minWeight, float maxWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
