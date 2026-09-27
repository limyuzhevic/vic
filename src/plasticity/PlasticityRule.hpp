#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules
// 
// Plasticity rules implement learning algorithms that modify synaptic weights
// based on pre- and post-synaptic neural activity patterns
// 
// Examples of plasticity rules:
// - HebbianRule: "neurons that fire together, wire together"
// - AntiHebbianRule: "neurons that fire together, don't wire together" (prevents runaway excitation)
// - STDP: Spike-timing dependent plasticity (STDP)
// - RewardModulatedSTDP: STDP modulated by reward prediction error
// 
// All plasticity rules follow the same interface for consistency
// This allows for plug-and-play plasticity rule systems

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Called during simulation steps when plasticity is enabled
    // 
    // Parameters:
    // - synapse: The synapse to update (must be valid)
    // - preSpikes: Vector of pre-synaptic spike timestamps
    // - postSpikes: Vector of post-synaptic spike timestamps  
    // - dt: Simulation time step duration
    // 
    // Returns: void (weight changes applied directly to synapse)
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply accumulated weight change to synapse
    // 
    // Parameters:
    // - synapse: The synapse to modify (must be valid)
    // - delta: Weight change value (positive for potentiation, negative for depression)
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get human-readable name of plasticity rule
    // Used for debugging, logging, and rule selection
    virtual const char* getName() const = 0;
    
    // Check if plasticity rule is enabled
    // Rules can be enabled/disabled dynamically to control learning
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;  // Whether this rule is currently active
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
// 
// Hebbian learning is the most basic form of synaptic plasticity
// It strengthens synapses when pre- and post-synaptic neurons fire together
// 
// Mathematical formulation (covariance rule):
// Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
// 
// Where:
// - η is the learning rate
// - ⟨⟩ denotes average over time
// - This implements "cells that fire together, wire together"

class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    HebbianRule(HebbianRule&&) noexcept;
    ~HebbianRule() override;
    
    HebbianRule& operator=(HebbianRule&&) noexcept;
    
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
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    AntiHebbianRule(AntiHebbianRule&&) noexcept;
    AntiHebbianRule& operator=(AntiHebbianRule&&) noexcept;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Anti-Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setDepressionRate(float rate);
    float getDepressionRate() const;
    
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
