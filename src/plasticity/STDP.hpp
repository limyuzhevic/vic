#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Advanced STDP implementation with biological accuracy and efficiency optimizations

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Real STDP implementation with biological plausibility
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters with biological constraints
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (tau) (ms)
    float getTimeConstant() const;
    void setLTPTimeConstant(float tauPlus);  // Potentiation time constant
    float getLTPTimeConstant() const;
    void setLTDTimeConstant(float tauMinus); // Depression time constant
    float getLTDTimeConstant() const;
    
    // Advanced STDP variants
    void enableTripletInteractions(bool enable);  // Enable triplet STDP rule
    void enableRewardModulation(bool enable);     // Enable reward-modulated STDP
    bool isTripletEnabled() const;
    bool isRewardModulated() const;
    
    // Update parameters from config with validation
    void configure(float ltpWeight, float ltdWeight, float tau);
    void configureAdvanced(float ltpWeight, float ltdWeight, 
                           float tauPlus, float tauMinus,
                           bool tripletEnabled, bool rewardModulated);
    
    // Statistical tracking for analysis
    struct STDPStatistics {
        float totalPotentiation;    // Total weight change from LTP
        float totalDepression;      // Total weight change from LTD
        float averageTimeDifference; // Average Δt between pre/post spikes
        size_t spikePairCount;      // Number of spike pairs processed
        float maxWeightChange;      // Maximum single weight change
    };
    
    STDPStatistics getStatistics() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// PLACEHOLDER - Phase 2
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
