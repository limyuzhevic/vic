#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian Plasticity Rule: "Neurons that Fire Together, Wire Together"
//
// Hebbian learning is one of the most fundamental learning principles in
// neuroscience, famously summarized by Donald Hebb's dictum: "When one 
// nerve cell strengthens another, the synapse between them is enhanced.
// This process underlies the formation of neural assemblies and working memory.
// 
// Key characteristics:
// - Correlated pre/post synaptic activity strengthens connections
// - Implements associative learning and pattern completion
// - Foundation for memory formation and concept learning
// - Activity-dependent modulation for stability-plasticity balance
//
// Biological basis:
// - AMPA receptor trafficking at synapses
// - Synaptic strengthening during co-activation
// - Critical for memory consolidation
// - Works synergistically with STDP for complex learning
//
// Computational advantages:
// - Simple, biologically plausible implementation
// - Fast convergence for strongly correlated patterns
// - Robust to noise and variability
// - Complementary to timing-based learning rules

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synaptic weights based on correlated pre/post synaptic activity
    //
    // Parameters:
    //   synapse: Target synapse to update
    //   preSpikes: Timestamps of pre-synaptic spikes (sorted)
    //   postSpikes: Timestamps of post-synaptic spikes (sorted)
    //   dt: Simulation timestep
    //
    // Algorithm:
    //   Uses optimized correlation computation with O(N log M) complexity
    //   Implements covariance rule with adaptive learning rates
    //   Prevents runaway potentiation through activity-dependent scaling
    //   Maintains biologically plausible weight bounds
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply a specific weight change with comprehensive safety checks
    //
    // Parameters:
    //   synapse: Target synapse for weight modification
    //   delta: Weight change amount (positive for potentiation, negative for depression)
    //
    // Features:
    //   - Clamps weights to biologically realistic bounds [-1.0, 1.0]
    //   - Logs significant changes for debugging
    //   - Updates synaptic statistics and history
    //   - Handles both excitatory and inhibitory synapses
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the human-readable name of this plasticity rule
    const char* getName() const override;
    
    // Hebbian learning parameters
    //
    // Learning rate controls the strength of synaptic modifications
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Synaptic bounds for stability
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Advanced Hebbian variants (implementation status):
    // 
    // Competitive Hebbian: Ensures only the strongest synapses are strengthened
    // Oja's rule: Normalized Hebbian learning for principal component extraction
    // Covariance Hebbian: Learns statistical correlations between neural activities
    // Anti-Hebbian: Implements "neurons that fire apart, wire apart" for inhibition

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
