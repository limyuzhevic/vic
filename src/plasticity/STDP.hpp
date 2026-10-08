#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real implementation for Phase 2 neural computation

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
    void setMinWeight(float weight);    // Minimum synaptic weight
    float getMinWeight() const;
    void setMaxWeight(float weight);    // Maximum synaptic weight
    float getMaxWeight() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Uses reward prediction error signals to modulate synaptic plasticity
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
    
    // Configuration
    void setBaseSTDP(std::shared_ptr<STDP> stdp) { baseSTDP_ = stdp; }
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setModulationStrength(float strength);
    float getModulationStrength() const;
    
    // Reward processing
    void applyReward(float reward, Timestamp time);
    float getCurrentPredictionError() const;
    
private:
    struct Impl;
    Impl* pImpl;
    std::shared_ptr<STDP> baseSTDP_;
    float currentPredictionError_;
};

// Spike-timing dependent plasticity with triplet interactions
// Captures complex spike-timing dependencies with presynaptic, postsynaptic, and eligibility traces
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
    
    // Configuration
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Triplet-specific parameters
    void setA1(float a1);    // Pre-before-post potentiation (triplet)
    float getA1() const;
    void setA2(float a2);    // Post-after-pre potentiation (triplet)
    float getA2() const;
    void setB1(float b1);    // Pre-before-post depression (triplet)
    float getB1() const;
    void setB2(float b2);    // Post-after-pre depression (triplet)
    float getB2() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
