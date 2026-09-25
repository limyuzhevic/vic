#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// IMPLEMENTED - Phase 2 real STDP with biological accuracy
// Based on spike-timing correlation with exponential window
// Includes: LTP when pre before post, LTD when post before pre
// Features: Eligibility traces for reward-modulated learning
//           Synaptic efficacy scaling for use-dependent modulation

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
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
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
// EXTENDED - Phase 2 reward-modulated learning
// Combines STDP with neuromodulatory signals (dopamine, etc.)
// Uses eligibility traces to bridge spikes and delayed rewards

class RewardModulatedSTDP : public PlasticityRule {
public:
    // Placeholder for future implementation with proper eligibility traces
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// FUTURE - Triplet STDP for more complex spike pairing rules
// Captures interactions between multiple spike pairs (e.g., pre-pre-post)
// More biologically accurate but computationally intensive

class TripletSTDP : public PlasticityRule {
public:
    // Placeholder for future implementation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
