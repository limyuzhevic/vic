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
    void setBaseLTPWeight(float weight);    // Base LTP weight
    float getBaseLTPWeight() const;
    void setBaseLTDWeight(float weight);    // Base LTD weight
    float getBaseLTDWeight() const;
    void setTimeConstant(float tau);        // STDP time constant (ms)
    float getTimeConstant() const;
    void setRewardModulation(float modulation);  // Reward modulation factor
    float getRewardModulation() const;
    
    // Reward handling
    void updateRewardHistory(float reward);
    float getRewardHistory() const;
    
    // Configuration
    void configure(float ltpWeight, float ltdWeight, float tau, float rewardMod);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Enhanced STDP that captures biological triplet interaction patterns
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
    void setAplus1(float weight);      // First spike potentiation (pre-post-pre)
    float getAplus1() const;
    void setAplus2(float weight);      // Second spike potentiation (pre-post-post)
    float getAplus2() const;
    void setAminus1(float weight);     // First spike depression (post-pre-post)
    float getAminus1() const;
    void setAminus2(float weight);     // Second spike depression (post-pre-pre)
    float getAminus2() const;
    void setTimeConstants(float tauF, float tauD, float tau);  // Time constants
    void setMinMaxWeight(float minWeight, float maxWeight);
    
    // Trace management
    float getFacilitationTrace() const;
    float getDepressionTrace() const;
    void resetTraces();
    
    // Configuration
    void configure(float aplus1, float aplus2, float aminus1, float aminus2,
                   float tauF, float tauD, float tau, float minWeight, float maxWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
