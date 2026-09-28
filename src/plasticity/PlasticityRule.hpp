#pragma once

/**
 * @file PlasticityRule.hpp
 * @brief Abstract base class and concrete implementations for synaptic plasticity rules.
 * 
 * This header defines the plasticity rule framework for the NLM neural network.
 * Plasticity rules govern how synaptic weights change based on pre- and post-synaptic
 * activity. Each rule implements a different learning principle and contributes to
 * the network's ability to adapt and learn.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Types/Types.hpp
 * @see src/brain/Neuron.hpp
 * @see src/brain/Synapse.hpp
 * @see src/plasticity/STDP.hpp
 * @see src/plasticity/Hebbian.hpp
 * @see src/plasticity/StructuralPlasticity.hpp
 * @see TODO Complete plasticity implementations (Phase 2)
 */

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * Abstract base class for all synaptic plasticity rules.
 * 
 * Defines the interface that all plasticity rules must implement. A plasticity rule
 * specifies how synaptic weights should change based on pre-synaptic and post-synaptic
 * spike activity. Rules can be enabled/disabled dynamically during simulation.
 * 
 * @note This class uses the PImpl idiom to allow implementation changes without
 * recompilation. Derived classes should follow the same pattern.
 * 
 * @see PlasticityFlags
 * @see STDP
 * @see HebbianRule
 * @see AntiHebbianRule
 * @see BCMRule
 * @see RewardModulatedSTDP
 * @see TripletSTDP
 * @see StructuralPlasticity
 */
class PlasticityRule {
public:
    /**
     * Virtual destructor ensures proper cleanup of derived class implementations.
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @name Pure Virtual Methods
     * 
     * These methods must be implemented by all derived plasticity rules.
     * They define the core learning dynamics.
     */
    /// @{
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * This method computes weight changes based on spike timing and history.
     * The update rule depends on the specific plasticity mechanism implemented
     * by the derived class.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param preSpikes Vector of timestamps of pre-synaptic spikes
     * @param postSpikes Vector of timestamps of post-synaptic spikes
     * @param dt Simulation timestep duration
     * @see applyWeightChange()
     * @see TODO PHASE 2: Implement real plasticity
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply accumulated weight change to a synapse
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change to apply (positive for potentiation, negative for depression)
     * @see update()
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Human-readable name of the rule
     */
    virtual const char* getName() const = 0;
    /// @}
    
    /**
     * @name Enable/Disable Control
     * 
     * These methods allow plasticity rules to be enabled or disabled dynamically.
     * When disabled, the rule's update method is not called during simulation.
     */
    /// @{
    
    /**
     * @brief Check if plasticity rule is currently enabled
     * 
     * @return true if rule is active, false if disabled
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable the plasticity rule
     * 
     * @param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled);
    /// @}
    
    /**
     * @name Object Lifetime Management
     * 
     * Default constructor and destructor are defined here for convenience.
     */
    /// @{
    
    /**
     * Default constructor initializes rule as enabled.
     */
    PlasticityRule() : enabled_(true) {}
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * Plasticity rules integrate with multiple components of the NLM system.
     */
    /// @{
    
    /**
     * @see src/brain/Neuron.hpp for PlasticityFlags
     * @see src/plasticity/STDP.hpp for spike-timing dependent plasticity
     * @see src/plasticity/Hebbian.hpp for Hebbian learning
     * @see src/plasticity/StructuralPlasticity.hpp for synapse creation/removal
     * @see TODO Implement concrete plasticity rules (Phase 2)
     */
    /// @}
    
protected:
private:
    /**
     * @name Implementation Details
     * 
     * Internal state for plasticity rule control.
     */
    /// @{
    
    bool enabled_;  /**< Current enabled state of the plasticity rule */
    /// @}

}; // class PlasticityRule

/**
 * Concrete implementation of Hebbian plasticity rule.
 * 
 * Implements the classic Hebbian learning principle: "neurons that fire together,
 * wire together." This is a placeholder implementation awaiting full development
 * in Phase 2. The rule strengthens synapses when pre- and post-synaptic neurons
 * fire in close temporal proximity.
 * 
 * @see PlasticityRule
 * @see Hebbian
 * @see AntiHebbianRule
 * @see TODO PHASE 2: Implement real Hebbian learning
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * Construct Hebbian rule with default parameters.
     */
    HebbianRule();
    
    /**
     * Destructor ensures proper cleanup of PImpl implementation.
     */
    ~HebbianRule() override;
    
    /**
     * @name Pure Virtual Method Implementation
     * 
     * These methods provide the concrete implementation of the PlasticityRule interface.
     */
    /// @{
    
    /**
     * @copydoc PlasticityRule::update()
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @copydoc PlasticityRule::applyWeightChange()
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @copydoc PlasticityRule::getName()
     */
    const char* getName() const override;
    /// @}
    
    /**
     * @name Parameter Access and Control
     * 
     * Methods for configuring Hebbian learning parameters.
     */
    /// @{
    
    /**
     * @brief Set Hebbian learning rate
     * 
     * @param rate Learning rate (typically 0.0 to 1.0)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * The HebbianRule integrates with other plasticity mechanisms.
     */
    /// @{
    
    /**
     * @see PlasticityRule for interface details
     * @see STDP for spike-timing dependent plasticity
     * @see AntiHebbianRule for anti-Hebbian learning
     * @see Hebbian for alternative implementation
     * @see TODO Implement full Hebbian learning (Phase 2)
     */
    /// @}
    
private:
    /**
     * @name Implementation Details
     * 
     * PImpl idiom for efficient implementation changes.
     */
    /// @{
    
    struct Impl;
    Impl* pImpl;  /**< Private implementation pointer */
    /// @}
};

/**
 * Anti-Hebbian plasticity rule.
 * 
 * Implements the anti-Hebbian learning principle: "neurons that fire together,
 * weaken together." This is a placeholder implementation. Anti-Hebbian learning
 * is useful for developing competitive dynamics and preventing runaway excitation.
 * 
 * @see PlasticityRule
 * @see HebbianRule
 * @see TODO PHASE 2: Complete implementation
 */
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @copydoc PlasticityRule::update()
     * 
     * Empty implementation (placeholder).
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    
    /**
     * @copydoc PlasticityRule::applyWeightChange()
     * 
     * Empty implementation (placeholder).
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @copydoc PlasticityRule::getName()
     */
    const char* getName() const override { return "AntiHebbian"; }
};

/**
 * Bienenstock-Cooper-Munro (BCM) plasticity rule.
 * 
 * Implements the BCM theory of synaptic modification, which combines elements
 * of Hebbian learning with a sliding threshold. This placeholder implementation
 * awaits full development in Phase 2.
 * 
 * @see PlasticityRule
 * @see TODO PHASE 2: Complete implementation
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @copydoc PlasticityRule::update()
     * 
     * Empty implementation (placeholder).
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    
    /**
     * @copydoc PlasticityRule::applyWeightChange()
     * 
     * Empty implementation (placeholder).
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @copydoc PlasticityRule::getName()
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
