#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for synaptic plasticity rules
// 
// Defines the interface for implementing synaptic plasticity mechanisms
// that modify synaptic strengths based on neural activity patterns.
// 
// Plasticity rules are essential for learning and memory in the brain,
// implementing biological learning rules like Hebbian and STDP.
// 
// Phase 2 and later will implement real biological plasticity rules
// with proper mathematical formulations based on neuroscience research.

class PlasticityRule {
public:
    /// \brief Virtual destructor for proper cleanup of derived classes
    virtual ~PlasticityRule() = default;
    
    /// \brief Update synaptic weights based on pre/post synaptic activity patterns
    /// \param synapse The synapse to be modified
    /// \param preSpikes Vector of pre-synaptic spike timestamps
    /// \param postSpikes Vector of post-synaptic spike timestamps  
    /// \param dt Current simulation timestep
    /// \note Derived classes implement specific plasticity rules
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /// \brief Apply a direct weight change to a synapse
    /// \param synapse The synapse to modify
    /// \param delta Weight change to apply (can be positive or negative)
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /// \brief Get the name/identifier of this plasticity rule
    /// \return String name of the rule (e.g., "STDP", "Hebbian")
    virtual const char* getName() const = 0;
    
    /// \brief Check if the plasticity rule is currently enabled
    /// \return true if the rule is active, false otherwise
    bool isEnabled() const;
    
    /// \brief Enable or disable the plasticity rule
    /// \param enabled true to enable, false to disable
    void setEnabled(bool enabled);
    
protected:
    /// \brief Protected constructor - concrete classes should instantiate
    PlasticityRule() : enabled_(true) {}
    
private:
    /// \brief Internal flag indicating whether the rule is active
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
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
