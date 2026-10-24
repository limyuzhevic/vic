// STDP.hpp - Spike-Timing-Dependent Plasticity header

#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// STDP is one of the most biologically realistic learning rules
// Potentiation occurs when pre-before-post, depression when post-before-pre
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
// R-STDP combines spike timing with reward prediction error signals
// Dopamine/serotonin signals modulate STDP based on reward prediction error
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
    void setBaseLearningRate(float rate);
    float getBaseLearningRate() const;
    void setRewardModulationFactor(float factor);
    float getRewardModulationFactor() const;
    void setPredictionErrorWeight(float weight);
    float getPredictionErrorWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Reward-modulated STDP update
    void updateWithReward(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt,
                         float rewardPredictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Triplet STDP: more biologically realistic, incorporates presynaptic and postsynaptic traces
// Accounts for three spike interaction patterns: pre-post, post-pre-post, pre-pre-post
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
    void setAplus(float aPlus);           // Post-before-pre potentiation
    float getAplus() const;
    void setAminus(float aMinus);         // Pre-before-post depression
    float getAminus() const;
    void setbPlus(float bPlus);           // Additional presynaptic trace
    float getbPlus() const;
    void setbMinus(float bMinus);         // Additional postsynaptic trace
    float getbMinus() const;
    void setTauPlus(float tauPlus);       // Potentiation time constant
    float getTauPlus() const;
    void setTauMinus(float tauMinus);     // Depression time constant
    float getTauMinus() const;
    void setTimeConstant(float tau);      // Synaptic trace time constant
    float getTimeConstant() const;
    
    // Triplet-specific update using traces
    void updateWithTraces(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt,
                         float preTrace,
                         float postTrace);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm