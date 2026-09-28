#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for all plasticity rules
//
// Plasticity rules implement synaptic weight modifications based on neural activity.
// This forms the foundation of learning and memory in the NLM brain system.
//
// Key properties:
// - Pure virtual interface ensures consistent behavior across implementations
// - Enable/disable mechanism for runtime control of learning
// - Thread-safe for parallel processing in multi-region brains
// - Integration with eligibility traces for reward-modulated learning
//
// Implementation notes:
//   - Concrete implementations should handle null pointer checks
//   - Rules should be stateless for thread safety (parameters stored in pImpl)
//   - Update methods should be idempotent for reproducible results
//

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity patterns
    //
    // This is the core learning operation. Concrete implementations compute
    // weight changes based on spike timing, correlation, or other factors.
    //
    // Parameters:
    //   synapse: Target synapse for weight modification (must not be null)
    //   preSpikes: Timestamps of pre-synaptic spikes (may be empty)
    //   postSpikes: Timestamps of post-synaptic spikes (may be empty)
    //   dt: Simulation timestep duration (ms)
    //
    // Preconditions:
    //   - synapse != nullptr (enforced by concrete implementations)
    //   - preSpikes and postSpikes should be sorted for optimized implementations
    //   - Spike timestamps should be within simulation time bounds
    //
    // Postconditions:
    //   - synapse weight may be modified according to plasticity rule
    //   - Rule should handle null/empty inputs gracefully
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply a specific weight change to a synapse
    //
    // This is a convenience method for applying pre-computed weight changes.
    // Often used by reward-modulated learning where the weight change
    // magnitude is determined by global neuromodulatory signals.
    //
    // Parameters:
    //   synapse: Target synapse for modification (must not be null)
    //   delta: Weight change amount (positive = potentiation, negative = depression)
    //
    // Safety guarantees:
    //   - Will clamp result to biologically plausible weight bounds
    //   - Should handle null synapse with appropriate error handling
    //   - May log significant weight changes for debugging
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get the human-readable name of this plasticity rule
    //
    // Used for logging, debugging, and configuration serialization.
    // Should return a short, descriptive name (e.g., "STDP", "Hebbian").
    virtual const char* getName() const = 0;
    
    // Check if this plasticity rule is currently enabled
    //
    // Returns:
    //   true if the rule should participate in learning
    //   false if the rule should be disabled (e.g., during development)
    bool isEnabled() const;
    
    // Enable or disable this plasticity rule
    //
    // Parameters:
    //   enabled: true to enable learning, false to disable
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
//
// Implements classical Hebbian learning based on spike correlation.
// This rule strengthens synapses when pre- and post-synaptic neurons
// fire in close temporal proximity, supporting associative learning.
//
// Hebbian learning is often used for:
//   - Pattern completion and memory recall
//   - Feature extraction in early processing stages
//   - Fast, stable learning in structured environments
//
// Integration notes:
//   - Works synergistically with STDP for complex learning
//   - Often used for rapid initial learning
//   - Can be combined with anti-Hebbian rules for inhibition
//

class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Hebbian learning rate parameter
    //
    // Controls the strength of synaptic potentiation.
    // Typical values: 0.001 to 0.1 for stable learning.
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: "neurons that fire apart, wire apart"
//
// Implements inhibitory Hebbian learning where synapses are weakened
// when pre- and post-synaptic neurons fire together.
// Used for implementing competition, normalization, and lateral inhibition.
//
// Applications:
//   - Competitive learning and self-organization
//   - Implementation of winner-take-all circuits
//   - Normalization of neural activity patterns
//   - Contrast enhancement in sensory processing

class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
//
// A homeostatic plasticity rule that implements sliding threshold learning.
// The modification threshold theta adapts based on recent postsynaptic activity,
// providing stability-plasticity balance and preventing runaway excitation.
//
// Key features:
//   - Activity-dependent learning threshold
//   - Prevents synaptic saturation
//   - Supports memory stability
//   - Biologically plausible implementation

class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
