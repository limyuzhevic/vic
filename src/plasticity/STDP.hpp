// STDP.hpp - Spike-Timing-Dependent Plasticity implementation
// Implements STDP learning rules with neuromodulation and reward-based plasticity
// Mathematical implementation based on spike-timing correlation with exponential windows
// Biological basis: Reflects NMDA receptor-mediated calcium signaling and backpropagating action potentials

#pragma once

#include "../brain/Synapse.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// STDP: Spike-Timing-Dependent Plasticity
// Represents activity-dependent synaptic plasticity where the timing of pre- and post-synaptic spikes determines weight changes
// Key parameters:
//   - A+ (ltpWeight): Potentiation amplitude for pre-before-post spikes
//   - A- (ltdWeight): Depression amplitude for post-before-pre spikes  
//   - τ (timeConstant): STDP time window (exponential decay parameter)
//   - Weight bounds: minWeight to maxWeight for synaptic stability
// Biological mechanism:
//   - Pre-before-post activates NMDA receptors when postsynaptic spikes occur
//   - Post-before-pre causes backpropagating action potentials
//   - Pairwise spike interaction reflects calcium signaling dynamics
// Learning rule implementation:
//   If Δt > 0 (pre before post): Potentiation Δw = A+ * exp(-Δt/τ)
//   If Δt < 0 (post before pre): Depression Δw = A- * exp(Δt/τ)
//   Where Δt = t_post - t_pre

class STDP {
public:
    STDP();
    ~STDP() = default;
    
    // Disable copying
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    
    // Enable moving
    STDP(STDP&&) noexcept = default;
    STDP& operator=(STDP&&) noexcept = default;
    
    // Update synaptic weights based on spike history
    // Pre-synaptic and post-synaptic spike timing determines potentiation or depression
    // Mathematical implementation follows spike-timing correlation rules with neuromodulation
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);
    
    // Apply direct weight change (for testing/validation)
    void applyWeightChange(Synapse* synapse, SynapticWeight delta);
    
    // Get algorithm name
    const char* getName() const;
    
    // Configuration methods
    void setLTPWeight(float weight);      // Set Long-Term Potentiation weight (A+)
    void setLTDWeight(float weight);     // Set Long-Term Depression weight (A-)
    void setTimeConstant(float tau);     // Set STDP time window (τ)
    
    // Configuration getters
    float getLTPWeight() const;
    float getLTDWeight() const;
    float getTimeConstant() const;
    
    // Configure all parameters at once
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Get current parameters
    float getLTPWeight() const { return getLTPWeight(); }
    float getLTDWeight() const { return getLTDWeight(); }
    float getTimeConstant() const { return getTimeConstant(); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
