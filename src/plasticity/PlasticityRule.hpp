#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

/**
 * @class PlasticityRule
 * @brief Abstract base class for synaptic plasticity rules
 * 
 * This class defines the interface for synaptic plasticity rules that modify
 * synaptic weights based on pre- and post-synaptic neural activity. Plasticity
 * rules implement learning mechanisms in neural networks, enabling adaptive
 * behavior through weight modification.
 * 
 * This class uses the PIMPL pattern for implementation hiding, with actual
 * plasticity implementations in derived classes (STDP, Hebbian, AntiHebbian, BCM).
 * 
 * @note All plasticity rules inherit from this abstract base class and must
 *       implement the pure virtual methods for weight update.
 * 
 * @see STDP for spike-timing-dependent plasticity
 * @see Hebbian for Hebbian ("fire together, wire together") learning
 * @see AntiHebbian for anti-Hebbian ("fire together, wire apart") learning
 * @see BCM for Bayesian Cognitive Model plasticity
 */

class PlasticityRule {
public:
    /**
     * @brief Default constructor
     * 
     * Initializes plasticity rule as enabled by default.
     * 
     * @post Rule is enabled and ready for weight updates
     */
    PlasticityRule() : enabled_(true) {}
    
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived class implementations
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Disable copying
     * 
     * PlasticityRule objects cannot be copied to prevent shallow copies
     * of implementation pointers.
     */
    PlasticityRule(const PlasticityRule&) = delete;
    PlasticityRule& operator=(const PlasticityRule&) = delete;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * This pure virtual method must be implemented by derived classes to
     * perform actual weight updates based on neural activity patterns.
     * 
     * @param synapse Pointer to synapse to be modified
     * @param preSpikes Vector of pre-synaptic spike times (in seconds)
     * @param postSpikes Vector of post-synaptic spike times (in seconds)
     * @param dt Simulation timestep duration
     * 
     * @note Derived classes implement specific learning rules:
     *       - STDP: Time-dependent plasticity based on spike timing differences
     *       - Hebbian: Activity-dependent strengthening ("fire together, wire together")
     *       - AntiHebbian: Activity-dependent weakening ("fire together, wire apart")
     *       - BCM: Threshold-based learning with adaptive plasticity
     * 
     * @see SpikeTiming dependent plasticity rules for specific implementations
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply weight change to synapse
     * 
     * Applies a specific weight change delta to the synapse, with derived
     * classes implementing weight bounds and additional constraints.
     * 
     * @param synapse Pointer to synapse to modify
     * @param delta Weight change to apply (can be positive or negative)
     * 
     * @note This method is called by update() implementations to apply
     *       calculated weight changes with appropriate bounds checking
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get rule name
     * 
     * @return Human-readable name of the plasticity rule
     * 
     * @note Used for logging, debugging, and rule identification
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if rule is enabled
     * 
     * @return true if rule is currently enabled for plasticity updates
     * 
     * @post Can be used to temporarily disable learning without destroying object
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable the plasticity rule
     * 
     * @param enabled New enabled state (true to enable, false to disable)
     * 
     * @post Rule will no longer apply weight updates when disabled
     * @note Useful for controlling learning during specific phases of simulation
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Protected constructor (to enforce use of derived classes)
     * 
     * @param enabled Initial enabled state
     */
    PlasticityRule(bool enabled) : enabled_(enabled) {}
    
private:
    /**
     * @brief Rule enabled state
     * 
     * @private Internal state, not intended for direct access
     */
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
