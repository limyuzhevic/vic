#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// 
// STDP implements the fundamental learning rule where synaptic strength
// changes depend on the precise timing relationship between pre- and 
// post-synaptic spikes. This is one of the most biologically realistic
// learning mechanisms in computational neuroscience.
// 
// Biological inspiration:
// - NMDA receptor-dependent calcium signaling
// - Spike-timing correlations in cortical networks
// - Pair-wise spike timing rules in hippocampal synapses
// - Backpropagating action potentials from postsynapse to presynapse
// 
// Mathematical formulation (pair-wise STDP):
// If Δt = t_post - t_pre > 0 (pre before post): 
//   Δw = A+ × exp(-Δt / τ+)  (potentiation)
// If Δt = t_post - t_pre < 0 (post before pre):
//   Δw = -A- × exp(Δt / τ-)  (depression)
// 
// Where:
// - A+ = LTP amplitude (potentiation strength)
// - A- = LTD amplitude (depression strength)
// - τ+ = τ- = time constant (typically 10-50ms)
// 
// Key features:
// - Asymmetric timing dependence (pre->post strengthens, post->pre weakens)
// - Exponential decay of influence with time difference
// - Forms the basis of temporal credit assignment
// 
// TODO PHASE 2: Implement comprehensive STDP framework
// - Add triplet STDP for synaptic facilitation/depression
// - Implement synaptic eligibility traces
// - Add reward-modulated STDP variants
// - Implement anti-Hebbian variants

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on spike timing correlations
    // 
    // Parameters:
    // - synapse: Pointer to the synapse being updated
    // - preSpikes: Vector of pre-synaptic spike timestamps (ms)
    // - postSpikes: Vector of post-synaptic spike timestamps (ms)
    // - dt: Time step duration (ms)
    // 
    // Computes pair-wise contributions from all pre-post spike pairs
    // and applies total weight change.
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change with biological bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // STDP parameters with biological ranges
    // 
    // LTP (Long-Term Potentiation) weight
    // - Typical values: 0.01-0.1
    // - Controls strength of potentiation for pre-before-post spikes
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    
    // LTD (Long-Term Depression) weight  
    // - Typical values: 0.01-0.1
    // - Controls strength of depression for post-before-pre spikes
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    
    // STDP time constant
    // - Typical values: 10-50ms
    // - Controls width of spike timing window
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Convenience method to configure all parameters at once
    void configure(float ltpWeight, float ltdWeight, float tau);
    
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
