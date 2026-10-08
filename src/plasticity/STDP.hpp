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
// PLACEHOLDER - Phase 2
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
    void setRewardPredictionError(float rpe);    // Reward prediction error (-1 to +1)
    float getRewardPredictionError() const;
    void setLTPWeight(float weight);              // Base A+ for potentiation
    float getLTPWeight() const;
    void setLTDWeight(float weight);              // Base A- for depression
    float getLTDWeight() const;
    void setTimeConstant(float tau);              // STDP time constant (ms)
    float getTimeConstant() const;
    void setMinWeight(float minWeight);           // Minimum synaptic weight
    float getMinWeight() const;
    void setMaxWeight(float maxWeight);           // Maximum synaptic weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
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
    void setAplus(float weight);                  // Pre-post-pre triplet potentiation
    float getAplus() const;
    void setAminus(float weight);                 // Post-pre-post triplet depression
    float getAminus() const;
    void setAtimes(float weight);                 // Pre-post interaction term
    float getAtimes() const;
    void setBpre(float weight);                   // Pre-only depression
    float getBpre() const;
    void setBpost(float weight);                  // Post-only depression
    float getBpost() const;
    void setTau1(float tau);                      // Time constant for pre spikes
    float getTau1() const;
    void setTau2(float tau);                      // Time constant for post spikes
    float getTau2() const;
    void setMinWeight(float minWeight);           // Minimum synaptic weight
    float getMinWeight() const;
    void setMaxWeight(float maxWeight);           // Maximum synaptic weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
