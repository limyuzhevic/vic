#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for synaptic plasticity rules
// Plasticity rules govern how synaptic weights change in response to neural activity
// They are the core learning mechanism in the brain, enabling adaptation and memory formation
// 
// This abstract base class defines the interface that all plasticity rules must implement
// Real implementations will be developed in Phase 2 with biologically-inspired dynamics
// 
// Plasticity rules are called during the learning phase of the simulation
// They receive spike timing information and produce weight changes
// Multiple plasticity rules can be active simultaneously (e.g., STDP + Hebbian)
// Rules can be enabled/disabled dynamically based on developmental stage or context

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // This is the core learning operation - computes weight changes from neural activity
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times (for timing-based rules)
    //   postSpikes - Vector of post-synaptic spike times (for timing-based rules)
    //   dt - Simulation timestep (for numerical integration and rate calculations)
    // Note: Rule implementations should check synapse validity and handle empty spike vectors
    // TODO PHASE 2: Implement real plasticity rule dynamics
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply a weight change (e.g., from reward-modulated plasticity)
    // This method allows plasticity rules to receive weight changes from other sources
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply (positive for potentiation, negative for depression)
    // Note: Used for integrating different plasticity mechanisms (e.g., reward + STDP)
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get plasticity rule name
    // Returns: String identifier for the plasticity rule (e.g., "STDP", "Hebbian")
    virtual const char* getName() const = 0;
    
    // Check if plasticity rule is enabled
    // Returns: true if rule is active and should be applied, false otherwise
    // Useful for dynamically controlling plasticity based on developmental stage,
    // learning state, or environmental conditions
    bool isEnabled() const;
    
    // Enable or disable plasticity rule
    // Parameters:
    //   enabled - New enabled state
    // Note: Disabled rules will not contribute to synaptic weight changes
    // but will still respond to external weight changes via applyWeightChange()
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;  // Current enable state of the plasticity rule
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Implements the classic Hebb's learning rule based on co-activation
// If pre- and post-synaptic neurons fire in close proximity in time:
// - Strengthen the synaptic connection (potentiation)
// - Form associations between co-active neurons
// Biological basis: NMDA receptor-dependent synaptic strengthening
// Mathematical formulation: Δw = η × f(pre) × g(post)
// Where η is learning rate, f and g are activation functions
// 
// Hebbian learning is credit assignment - neurons that fire together get stronger connections
// This implements pattern completion and memory association
// 
// HebbianRule implementation placeholder - Phase 2 will implement real dynamics
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    // Update synaptic weights based on co-activation
    // Implements Hebbian learning: strengthen synapses when pre- and post-synaptic
    // neurons fire together (co-activation within learning window)
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times (used for activity calculation)
    //   postSpikes - Vector of post-synaptic spike times (used for activity calculation)
    //   dt - Simulation timestep (used for rate calculations)
    // Note: Modern implementations often use sliding windows or rate-based models
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change from other plasticity mechanisms
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply (e.g., from reward-modulated plasticity)
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get plasticity rule name
    const char* getName() const override;
    
    // Hebbian learning parameters
    
    // Set learning rate (η)
    // Parameters: rate - Learning rate (typically 0.001-0.01)
    // Controls strength of synaptic modification
    // Higher rate = faster learning but less stability
    void setLearningRate(float rate);
    
    // Get current learning rate
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Opposite of Hebbian learning - strengthens inhibition between co-active neurons
// Used for implementing competition, rivalry, and normalization
// Useful for maintaining network balance and preventing runaway excitation
// Biological basis: Competition for limited resources
// Mathematical formulation: Δw = -η × (pre × post)
// Where η is learning rate, pre is pre-synaptic activity, post is post-synaptic activity
// 
// Anti-HebbianRule implementation placeholder - Phase 2 will implement real dynamics
class AntiHebbianRule : public PlasticityRule {
public:
    // Update synaptic weights based on co-activation (anti-Hebbian)
    // Implements opposite of Hebb's rule: weaken synapses when pre- and post-synaptic
    // neurons fire together
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times
    //   postSpikes - Vector of post-synaptic spike times
    //   dt - Simulation timestep
    // Note: Placeholder implementation - Phase 2 will implement real Anti-Hebbian rule
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    // Apply weight change from other plasticity mechanisms
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply
    // Note: Placeholder implementation - Phase 2 will implement real Anti-Hebbian rule
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    // Get plasticity rule name
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// Stability-plasticity trade-off rule that combines Hebbian and anti-Hebbian learning
// Uses sliding threshold (θm) to determine when plasticity switches from LTP to LTD
// Implements homeostatic regulation of synaptic weights
// Prevents runaway excitation while maintaining learning capability
// Biological basis: Synaptic scaling and homeostatic plasticity
// Mathematical formulation: 
// If post > θm: Δw = η × (post - θm) × pre (LTP)
// If post < θm: Δw = η × (post - θm) × pre (LTD)
// 
// BCMRule implementation placeholder - Phase 2 will implement real dynamics
class BCMRule : public PlasticityRule {
public:
    // Update synaptic weights with sliding threshold
    // Implements BCM rule: LTP when post > θm, LTD when post < θm
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times
    //   postSpikes - Vector of post-synaptic spike times
    //   dt - Simulation timestep
    // Note: Placeholder implementation - Phase 2 will implement real BCM rule
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    // Apply weight change from other plasticity mechanisms
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply
    // Note: Placeholder implementation - Phase 2 will implement real BCM rule
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    // Get plasticity rule name
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
