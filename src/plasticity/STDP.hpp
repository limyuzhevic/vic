#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real pair-based STDP implementation based on spike-timing correlation

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
// Implements reward-modulated learning where synaptic changes are gated by reward signals
// Biological basis: Dopamine signaling in reinforcement learning
// Mathematical formulation: Δw = R * E(Δt) * f(Δt)
// Where R is reward signal, E(Δt) is eligibility trace, f(Δt) is STDP window

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override = default;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward-modulated STDP specific parameters
    void setRewardFactor(float factor);
    float getRewardFactor() const;
    void setTraceDecayRate(float rate);
    float getTraceDecayRate() const;
    void setTraceBaseline(float baseline);
    float getTraceBaseline() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau,
                    float rewardFactor, float traceDecayRate);
    
    // Decay eligibility trace over time (should be called periodically)
    void decayTrace(Synapse* synapse, TimestepDuration dt);
    
    // Apply reward signal (converts eligibility trace to weight change)
    void applyReward(Synapse* synapse, float rewardSignal);

private:
    float ltpWeight_;      // LTP amplitude
    float ltdWeight_;      // LTD amplitude
    float timeConstant_;   // STDP time window (ms)
    float minWeight_;      // Minimum synaptic weight
    float maxWeight_;      // Maximum synaptic weight
    float rewardFactor_;   // Reward modulation strength
    float traceDecayRate_; // Eligibility trace decay rate
    float traceBaseline_;  // Baseline eligibility trace
};

// Spike-timing dependent plasticity with triplet interactions
// Captures more complex temporal interactions beyond pairwise spike timing
// Biological basis: Synaptic interactions involving three spikes (pre-post or post-post-pre)
// Mathematical formulation: Considers triplet interactions for both LTP and LTD

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override = default;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet STDP specific parameters
    void setPairLTPWeight(float weight);
    float getPairLTPWeight() const;
    void setPairLTDWeight(float weight);
    float getPairLTDWeight() const;
    void setTripletLTPWeight(float weight);
    float getTripletLTPWeight() const;
    void setTripletLTDWeight(float weight);
    float getTripletLTDWeight() const;
    void setTripletTimeConstant(float tau);
    float getTripletTimeConstant() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    
    // Update parameters from config
    void configure(float pairLTPWeight, float pairLTDWeight, float tripletLTPWeight, float tripletLTDWeight, float tau);

private:
    // Pair-based STDP parameters (baseline for triplet rule)
    float pairLTPWeight_;       // Pair-based LTP amplitude
    float pairLTDWeight_;      // Pair-based LTD amplitude
    
    // Triplet-specific parameters
    float tripletLTPWeight_;   // Triplet LTP amplitude (typically weaker)
    float tripletLTDWeight_;   // Triplet LTD amplitude
    float tripletTimeConstant_; // Longer time window for triplet interactions
    
    // Weight bounds
    float minWeight_;           // Minimum synaptic weight
    float maxWeight_;            // Maximum synaptic weight
};

} // namespace nlm
