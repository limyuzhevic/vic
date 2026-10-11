#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// Core synaptic plasticity mechanism based on the timing between pre- and post-synaptic spikes
// "Cells that fire together, wire together" - but only if pre fires before post
// If pre fires before post: Long-Term Potentiation (LTP) - synaptic strengthening
// If post fires before pre: Long-Term Depression (LTD) - synaptic weakening
// Time window for STDP: typically 10-100ms
// Biological basis: NMDA receptor dynamics and calcium signaling
// Mathematical basis: Exponential decay of synaptic modification probability with time difference
// 
// STDP implementation placeholder - Phase 2 will implement real dynamics with synaptic eligibility traces

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on spike timing
    // Implements the classic STDP learning rule
    // Parameters: 
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times
    //   postSpikes - Vector of post-synaptic spike times  
    //   dt - Simulation timestep (used for numerical stability)
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply a weight change (e.g., from reward-modulated plasticity)
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply (positive for potentiation, negative for depression)
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get plasticity rule name
    // Returns: String identifier ("STDP")
    const char* getName() const override;
    
    // STDP parameters - configure learning strength and time constants
    
    // Set long-term potentiation weight (A+)
    // Parameters: weight - LTP amplitude (typically 0.01-0.1)
    // Controls synaptic strengthening when pre fires before post
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    // Get current LTP weight
    float getLTPWeight() const;
    
    // Set long-term depression weight (A-)
    // Parameters: weight - LTD amplitude (typically 0.01-0.1, slightly > LTP)
    // Controls synaptic weakening when post fires before pre
    void setLTDWeight(float weight);    // Long-term depression weight
    
    // Get current LTD weight
    float getLTDWeight() const;
    
    // Set STDP time constant (tau)
    // Parameters: tau - Time constant for exponential decay (ms, typically 10-50ms)
    // Controls width of the STDP learning window
    // Larger tau = longer learning window, smaller changes
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    // Get current time constant
    float getTimeConstant() const;
    
    // Update all STDP parameters at once
    // Parameters:
    //   ltpWeight - Long-term potentiation weight
    //   ltdWeight - Long-term depression weight  
    //   tau - Time constant
    // Convenience method for batch configuration from config files
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Combines standard STDP with reward prediction error signals
// Dopaminergic signals modulate STDP strength based on outcome prediction errors
// Implements reward-modulated synaptic learning
// Biological basis: Dopamine modulation of STDP in vivo experiments
// 
// Reward-modulated STDP implementation placeholder - Phase 2 will implement real dynamics
class RewardModulatedSTDP : public PlasticityRule {
public:
    // Update synaptic weights with reward modulation
    // Integrates spike timing with reward prediction error signals
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times
    //   postSpikes - Vector of post-synaptic spike times
    //   dt - Simulation timestep
    // Note: Placeholder implementation - Phase 2 will implement real R-STDP
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    // Apply weight change with reward modulation
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply
    // Note: Placeholder implementation - Phase 2 will implement real R-STDP
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    // Get plasticity rule name
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// More complex STDP variant that includes triplet spike interactions
// Captures biological observations of spike-triplet STDP in experiments
// Accounts for multiple spikes within the learning window
// Provides more accurate modeling of in vivo STDP measurements
// Biological basis: Synaptic tagging and capture theories
// 
// Triplet STDP implementation placeholder - Phase 2 will implement real dynamics
class TripletSTDP : public PlasticityRule {
public:
    // Update synaptic weights with triplet interactions
    // Implements triplet-based STDP learning rule
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times
    //   postSpikes - Vector of post-synaptic spike times
    //   dt - Simulation timestep
    // Note: Placeholder implementation - Phase 2 will implement real Triplet STDP
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    // Apply weight change with triplet interactions
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply
    // Note: Placeholder implementation - Phase 2 will implement real Triplet STDP
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    // Get plasticity rule name
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
