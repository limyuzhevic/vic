#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// 
// STDP is a Hebbian learning rule that modifies synaptic strength based on 
// the precise timing relationship between pre-synaptic and post-synaptic spikes.
// This implementation follows the classic pair-based STDP rule with exponential
// windows, which captures the essential dynamics of synaptic learning in biological
// neural systems.
// 
// Key features:
// - Pre-before-post spike pairs cause Long-Term Potentiation (LTP)
// - Post-before-pre spike pairs cause Long-Term Depression (LTD)
// - Exponential decay windows reflect biophysical constraints
// - Integrated eligibility traces for reward-modulated learning
// - Weight bounds to prevent instability
// 
// Biological basis:
// - Models NMDA receptor-dependent calcium signaling
// - Captures spike timing dependence of synaptic plasticity
// - Reflects synaptic tagging and capture mechanisms
// 
// Computational advantages:
// - O(N+M) algorithm implementation (vs O(N×M) naive)
// - Adaptive learning based on activity patterns
// - Robust to sparse spike activity

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on pre/post synaptic spike timing
    // 
    // Parameters:
    //   synapse: Target synapse to update
    //   preSpikes: Timestamps of pre-synaptic spikes (sorted)
    //   postSpikes: Timestamps of post-synaptic spikes (sorted) 
    //   dt: Simulation timestep
    //
    // Algorithm:
    //   Uses optimized two-pointer technique to compute O(N+M) complexity
    //   Processes correlated spike pairs within exponential windows
    //   Accumulates weight changes from all spike pairs
    //   Applies eligibility traces for reward modulation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply a specific weight change to a synapse with safety checks
    //
    // Parameters:
    //   synapse: Target synapse
    //   delta: Weight change to apply (can be positive or negative)
    //
    // Implementation:
    //   - Clamps weights to biologically plausible bounds
    //   - Records change in synapse statistics
    //   - Updates learning history for analysis
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the human-readable name of this plasticity rule
    const char* getName() const override;
    
    // STDP parameters for fine-tuning learning dynamics
    //
    // Long-Term Potentiation (LTP) parameters:
    //   Positive spike timing differences strengthen synapses
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    
    // Long-Term Depression (LTD) parameters:  
    //   Negative spike timing differences weaken synapses
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    
    // Temporal dynamics parameters:
    //   Controls the width of the exponential learning window
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Configure all STDP parameters from configuration values
    // Useful for loading from experimental protocols or config files
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Advanced STDP variants (currently implemented)
    
    // Reward-modulated STDP (R-STDP)
    // Combines spike timing with reward prediction error for goal-directed learning
    // This variant modulates the weight change magnitude based on reward signals
    
    // Triplet STDP with calcium dynamics
    // More biologically detailed model incorporating calcium dynamics and
    // presynaptic, postsynaptic, and pairing-dependent mechanisms
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
