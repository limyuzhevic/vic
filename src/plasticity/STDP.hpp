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
// Combines standard STDP with reward prediction error
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "RewardModulatedSTDP"; }
    
    // Parameters
    void setBaseLTPWeight(float weight);
    float getBaseLTPWeight() const;
    void setBaseLTDWeight(float weight);
    float getBaseLTDWeight() const;
    void setRewardGain(float gain);
    float getRewardGain() const;
    void setBaselineDopamine(float baseline);
    float getBaselineDopamine() const;
    
    // Update parameters from reward prediction error
    void updateWithReward(float rewardPredictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// More biologically realistic model including pre- and post- spikes bursts
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Parameters for triplet interactions
    void setUPlus(float u);
    float getUPlus() const;
    void setUMinus(float u);
    float getUMinus() const;
    void setTaup(float tau);
    float getTaup() const;
    void setTauMinus(float tau);
    float getTauMinus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: "neurons that fire together, disconnect together"
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "AntiHebbianRule"; }
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule
// Sliding threshold theory of synaptic modification
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "BCMRule"; }
    
    // Parameters
    void setTheta(float theta);  // Sliding threshold
    float getTheta() const;
    void setTau(float tau);      // Threshold dynamics
    float getTau() const;
    void setBCMWeight(float w); // Synaptic modification rate
    float getBCMWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm