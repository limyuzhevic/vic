#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// Optimized implementation for real neural computation
// 
// Mathematical formulation:
// For each pre-post spike pair with timing difference Δt = t_post - t_pre:
// 
// If Δt > 0 (pre before post): POTENTIATION
//   Δw = A+ * exp(-Δt / τ+)
//   
// If Δt < 0 (post before pre): DEPRESSION
//   Δw = A- * exp(Δt / τ-)
// 
// Optimization:
//   - O(n log n) instead of O(n²) using sorting and two-pointer technique
//   - Limited time window (±5τ) for significant contributions
//   - Parallel pre->post and post->pre calculations

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weight based on spike timing
    // Uses optimized O(n log n) algorithm instead of O(n²)
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
    void setTimeConstant(float tau);    // STDP time constant (τ, in ms)
    float getTimeConstant() const;
    
    // Performance optimization parameters
    void setTimeWindowMultiplier(float multiplier);  // How many τ to consider (default: 5.0f)
    float getTimeWindowMultiplier() const;
    
    // Update parameters from configuration
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Performance optimization parameters
    void setTimeWindowMultiplier(float multiplier);  // How many τ to consider (default: 5.0f)
    float getTimeWindowMultiplier() const;
    
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
