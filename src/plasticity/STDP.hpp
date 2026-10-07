#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Enhanced with reward-modulated learning support

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Enhanced STDP with reward modulation support
    // Implements Reward-Modulated STDP (R-STDP)
    // Δw = STDP_delta * reward_signal * eligibility_trace
    void updateWithReward(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt,
                          float rewardSignal);
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float minWeight = -1.0f, float maxWeight = 1.0f);
    
    // STDP statistics and debugging
    std::string getStatistics() const;
    
    // Complex STDP variants
    // Triplet STDP accounts for spike triplets for more realistic dynamics
    void updateWithTripletInteractions(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt);

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
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // R-STDP specific parameters
    void setRewardScale(float scale);
    float getRewardScale() const;
    void setEligibilityDecayRate(float rate);
    float getEligibilityDecayRate() const;
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
