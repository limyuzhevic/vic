#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for synaptic plasticity rules
// Defines the interface for all plasticity mechanisms in the NLM
// 
// Plasticity rules implement the biological process of synaptic weight changes
// in response to neural activity patterns. This forms the foundation of learning
// and memory in artificial neural systems.
// 
// Key concepts:
// - Spike-timing-dependent plasticity (STDP): Timing between pre- and post-synaptic spikes
// - Hebbian plasticity: Co-activation strengthens connections
// - Anti-Hebbian plasticity: Co-activation weakens connections
// - Homeostatic plasticity: Maintains network stability
// - Neuromodulated plasticity: Rewards, punishments, and attention signals
// 
// Implementation details:
// - Event-driven computation based on spike timestamps
// - Real-time weight updates during neural activity
// - Support for eligibility traces and synaptic tags
// - Integration with neuromodulatory systems
// - Plasticity rule switching and weighting
// 
// Common properties:
// - Enabled/disabled state for conditional plasticity
// - Learning rate parameters
// - Weight bounds (min/max synaptic strength)
// - History tracking for online learning
// - Integration with reward prediction errors

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Virtual function that must be implemented by derived classes
    // Called during neural computation when plasticity is active
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change directly
    // Utility function for applying computed weight changes
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    // Returns human-readable name of the plasticity rule
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    // Returns true if the plasticity rule is currently active
    bool isEnabled() const;
    
    // Enable/disable plasticity rule
    // Turn the plasticity rule on or off conditionally
    void setEnabled(bool enabled);
    
    // Get learning rate
    // Returns the current learning rate (0.0 = no learning, 1.0 = maximum learning)
    virtual float getLearningRate() const { return 1.0f; }
    
    // Set learning rate
    // Configures the learning rate for the plasticity rule
    virtual void setLearningRate(float rate) {}
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Real implementation based on covariance-based learning
// 
// Mathematical formulation (Covariance rule):
// Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
// 
// Simplified version for spike-based systems:
// Δw = η * (coactivity - baseline)
// 
// Where:
// - coactivity = number of correlated pre/post spike pairs
// - baseline = learningRate * mean activity
// - η = learning rate
// 
// Biological inspiration:
// - Reflects AMPA receptor trafficking at synapses
// - Hebbian plasticity at Schaffer collateral synapses in hippocampus
// - Correlation-based learning in visual cortex
// - Activity-dependent synaptic strengthening
// 
// Key features:
// - Correlation-based weight changes
// - Prevents runaway excitation through normalization
// - Supports both potentiation and depression
// - Integrates with STDP for spike-timing dependence
// - Compatible with neuromodulatory signals

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
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
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
// PLACEHOLDER - Phase 2
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
