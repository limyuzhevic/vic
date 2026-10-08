#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
//
// This plasticity rule implements the biological principle that the precise
// timing of pre-synaptic and post-synaptic spikes determines synaptic change.
// STDP is fundamental to learning and memory in biological systems.
//
// Biological basis:
// - Spike-timing-dependent synaptic plasticity (STDP) is observed in biological
//   synapses (Bi & Poo, 1998; Dan & Poo, 2006)
// - Pre-synaptic spike followed by post-synaptic spike (positive interval)
//   strengthens synapse (LTP - Long-Term Potentiation)
// - Post-synaptic spike followed by pre-synaptic spike (negative interval)
//   weakens synapse (LTD - Long-Term Depression)
// - The magnitude of change depends on the time difference between spikes
//
// Mathematical formulation:
// - For post-before-pre (Δt < 0): w(t+Δt) = w(t) - A_-(Δt)
// - For pre-before-post (Δt > 0): w(t+Δt) = w(t) + A_+(Δt)
//
// Where A_+(Δt) and A_-(Δt) are asymmetric learning windows
//
// Applications:
// - Temporal sequence learning
// - Pattern separation/association
// - Memory encoding and consolidation
// - Skill acquisition
//
// @note This is a Phase 2 placeholder implementation
//       Real STDP should incorporate:
//       - Biologically realistic learning windows
//       - Spike train integration (not just single spikes)
//       - Neuromodulatory gating
//       - Multiple plasticity branches (e.g., NMDA-dependent)
//
// @see https://www.nature.com/articles/276737a0
//      https://www.pnas.org/content/105/35/13127
class STDP : public PlasticityRule {
public:
// Create STDP plasticity system with default parameters
//
// @note Default parameters:
//       - LTP weight: 0.01 (moderate potentiation)
//       - LTD weight: 0.012 (moderate depression, slightly stronger)
//       - Time constant: 20ms (standard for cortical synapses)
//
// @note These defaults can be adjusted based on:
//       - Target learning characteristics
//       - Synaptic properties
//       - Behavioral requirements
//       - Network stability constraints
STDP();
// Destructor
//
// @note Automatic memory management via Pimpl idiom
~STDP() override;
    
// Update synaptic weights based on spike timing patterns
//
// @param synapse Target synapse to modify
// @param preSpikes Timestamps of presynaptic spikes
// @param postSpikes Timestamps of postsynaptic spikes
// @param dt Simulation timestep duration
//
// @note STDP computation:
//       1. Calculate time differences between pre- and post-spikes
//       2. Apply asymmetric learning rules
//       3. Generate weight changes based on spike order
//
// @note Implementation should:
//       1. Integrate spike trains (not just single spikes)
//       2. Use convolution of spike trains
//       3. Apply window functions A_+(Δt) and A_-(Δt)
//       4. Consider synaptic eligibility traces
//
// @warning Spike times must be in simulation time units (seconds)
//         Must handle overlapping spike trains correctly
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;

    // Apply weight change to synapse
//
// @param synapse Target synapse to modify
// @param delta Weight change amount (positive or negative)
//
// @note Weight changes are applied using addToWeight() which ensures:
//       - Weight bounds are maintained
//       - Stability is preserved
//       - Plasticity limits are respected
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;

    // Get plasticity rule name
//
// @return const char* String "STDP" identifying this plasticity rule
//
// @note Used for:
//       - Rule identification
//       - Debugging output
//       - Configuration settings
//       - Experimental reporting
    const char* getName() const override;
    
// Set long-term potentiation (LTP) weight parameter
//
// @param weight LTP weight parameter (default: 0.01)
//
// @note LTP magnitude determines how strongly synapses are strengthened
//       when presynaptic spikes precede postsynaptic spikes
//
// @note Weight ranges:
//       - 0.001 to 0.1 typical for biological synapses
//       - Higher values = stronger potentiation
//       - Lower values = more subtle effects
//
// @see https://www.nature.com/articles/413733a
//      https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2685836/
    void setLTPWeight(float weight);    // Long-term potentiation weight

    // Get current LTP weight parameter
//
// @return float Current LTP weight parameter
    float getLTPWeight() const;

    // Set long-term depression (LTD) weight parameter
//
// @param weight LTD weight parameter (default: 0.012)
//
// @note LTD magnitude determines how strongly synapses are weakened
//       when postsynaptic spikes precede presynaptic spikes
//
// @note Weight ranges:
//       - 0.001 to 0.1 typical for biological synapses
//       - Often slightly stronger than LTP in biological systems
//       - Affects symmetry of learning rule
//
// @note In some biological systems, LTD can be more pervasive than LTP
    void setLTDWeight(float weight);    // Long-term depression weight

    // Get current LTD weight parameter
//
// @return float Current LTD weight parameter
    float getLTDWeight() const;

    // Set STDP time constant (learning window width)
//
// @param tau STDP time constant in milliseconds (default: 20.0ms)
//
// @note Time constant determines the width of the learning window:
//       - Larger tau: broader window, effects spread over longer times
//       - Smaller tau: narrower window, more precise timing dependence
//       - Typical values: 10-100ms for cortical synapses
//       - Affects temporal integration range
    void setTimeConstant(float tau);    // STDP time constant (ms)

    // Get current STDP time constant
//
// @return float Current STDP time constant in milliseconds
    float getTimeConstant() const;

    // Configure STDP parameters from configuration
//
// @param ltpWeight LTP weight parameter
// @param ltdWeight LTD weight parameter  
// @param tau STDP time constant
//
// @note Convenience function to set all STDP parameters at once
//       Useful for loading from configuration files
//       Ensures parameter consistency
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
