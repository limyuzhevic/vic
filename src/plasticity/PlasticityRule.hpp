#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for synaptic plasticity rules
// 
// Plasticity rules implement activity-dependent modifications to synaptic
// strengths based on spike timing or co-activity patterns. This forms the
// foundation of learning in the NLM brain.
// 
// Biological inspiration:
// - STDP mimics NMDA receptor dynamics and spike timing correlations
// - Hebbian learning reflects LTP/LTD processes in hippocampus
// - Reward modulation implements dopamine-based reinforcement learning
// 
// Implementation notes:
// - Rules are applied during brain::updatePlasticity()
// - Synapses can have multiple plasticity rules enabled simultaneously
// - Weight changes are bounded to maintain stability
// 
// TODO PHASE 2: Implement comprehensive plasticity rule framework
// - Add eligibility traces for reward-modulated learning
// - Implement synaptic resource constraints
// - Add metaplasticity (plasticity of plasticity)

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // 
    // Parameters:
    // - synapse: Pointer to the synapse being updated
    // - preSpikes: Vector of pre-synaptic spike timestamps (ms)
    // - postSpikes: Vector of post-synaptic spike timestamps (ms)
    // - dt: Time step duration (ms)
    // 
    // The rule computes weight changes based on spike timing patterns
    // and applies them via applyWeightChange().
    // 
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change to a synapse
    // 
    // Parameters:
    // - synapse: Pointer to the synapse to modify
    // - delta: Weight change to apply (can be positive or negative)
    // 
    // Rules should clamp weights to biologically plausible ranges
    // and handle eligibility traces for reward-modulated learning.
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get the human-readable name of this plasticity rule
    // 
    // Returns:
    // - Descriptive name (e.g., "STDP", "Hebbian", "BCM")
    virtual const char* getName() const = 0;
    
    // Check if plasticity rule is currently enabled
    // 
    // Returns:
    // - true if rule is active, false otherwise
    bool isEnabled() const;
    
    // Enable or disable this plasticity rule
    // 
    // Parameters:
    // - enabled: true to enable, false to disable
    void setEnabled(bool enabled);
    
protected:
    // Constructor - all new PlasticityRule objects are enabled by default
    PlasticityRule() : enabled_(true) {}
    
private:
    // Flag indicating whether this rule is currently enabled
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// 
// Implements the classic Hebbian learning rule based on correlated spike
// activity. This rule strengthens synapses when pre- and post-synaptic
// neurons fire within a specific time window.
// 
// Biological inspiration:
// - Reflects LTP (Long-Term Potentiation) processes
// - Mediates memory formation in hippocampus
// - Implements "fire together, wire together" principle
// 
// Mathematical formulation:
// Δw = η × (coincidence count - baseline)
// 
// Where:
// - η = learning rate parameter
// - coincidence count = number of correlated pre/post spike pairs
// - baseline = expected coincidences under independence
// 
// Implementation details:
// - Uses 100ms correlation window for spike pairing
// - Weight changes are bounded to maintain stability
// - Can be combined with STDP for complementary learning
// 
// TODO PHASE 2: Implement real Hebbian learning
// - Add covariance-based learning rule (BCM theory)
// - Implement synaptic eligibility traces
// - Add activity-dependent learning rates

class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    // Update synaptic weights based on correlated pre/post spike activity
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change with bounds checking
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Learning parameters
    // Learning rate controls strength of weight changes
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: "neurons that fire together, weaken together"
// 
// Anti-Hebbian plasticity is the opposite of Hebbian learning - it weakens
// synapses when pre- and post-synaptic neurons fire together. This rule
// implements depression for correlated activity, supporting regulatory
// mechanisms in neural circuits.
// 
// Biological inspiration:
// - Hemispheric lateral inhibition in cortical circuits
// - Competitive learning in neural development
// - Stabilization of network activity levels
// - Preventing runaway excitation in networks
// 
// Mathematical formulation:
// Δw = -η × (coincidence count - baseline)
// 
// Same as Hebbian but with negative sign for depression instead of potentiation
// 
// Applications:
// - Competitive learning algorithms
// - Stabilizing plasticity in developing networks
// - Implementing lateral inhibition
// - Balancing excitation and inhibition

class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
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
