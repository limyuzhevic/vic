#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Implements real STDP based on biological spike timing correlations
// 
// Biological basis:
// - NMDA receptor-mediated calcium signaling timing
// - Pre-before-post spike pairs potentiate synapses (LTP)
// - Post-before-pre spike pairs depress synapses (LTD)
// - Reflects synaptic learning from temporal spike correlations
// 
// Mathematical formulation:
// For each pre-post spike pair with Δt = t_post - t_pre:
// - Δt > 0 (pre before post): Δw = A+ * exp(-Δt / τ+)
// - Δt < 0 (post before pre): Δw = A- * exp(Δt / τ-)
// 
// Where:
//   A+ = ltpWeight (potentiation amplitude)
//   A- = ltdWeight (depression amplitude)
//   τ+ = τ- = timeConstant (STDP time window)
// 
// Key features:
// - Exponential STDP window with configurable time constant
// - Separate potentiation and depression amplitudes
// - Weight clamping to maintain stability
// - Eligibility trace support for reward-modulated learning

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Disable copying, enable moving
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    STDP(STDP&&) noexcept;
    STDP& operator=(STDP&&) noexcept;
    
    // Update synaptic weights based on spike timing
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change directly
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // STDP parameters with biological validation
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (τ, in ms)
    float getTimeConstant() const;
    
    // Convenience configuration method
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward-modulated STDP (R-STDP)
// Combines STDP with reward signals for reinforcement learning
// 
// Extends standard STDP by multiplying weight changes by reward prediction
// Enables credit assignment for temporally distant events
// 
// Formula: Δw = STDP_update * reward_signal * eligibility_trace

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // STDP update with reward modulation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // Reward modulation parameters
    void setRewardGain(float gain);
    float getRewardGain() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// More biologically realistic version that accounts for spike triplets
// 
// Extends pairwise STDP by including contributions from triplets of spikes:
// - Pre-pre-post interactions (facilitation)
// - Post-post-pre interactions (depression)
// - Complex temporal patterns

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    // Triplet-based STDP update
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Triplet interaction parameters
    void setPairwiseLTP(float weight);
    float getPairwiseLTP() const;
    void setPairwiseLTD(float weight);
    float getPairwiseLTD() const;
    void setFacilitationWeight(float weight);
    float getFacilitationWeight() const;
    void setDepressionWeight(float weight);
    float getDepressionWeight() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
