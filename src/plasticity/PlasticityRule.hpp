#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Doxygen-style documentation for plasticity rule interface

/**
 * @class PlasticityRule
 * @brief Abstract base class for all synaptic plasticity rules
 * 
 * Defines the interface for synaptic plasticity rules that update synaptic
 * weights based on pre- and post-synaptic activity patterns. Plasticity rules
 * implement various learning algorithms including STDP, Hebbian learning,
 * and reward-modulated learning.
 * 
 * @note Concrete implementations should inherit from this class and override
 * all pure virtual methods to define specific plasticity rules.
 */

class PlasticityRule {
public:
    /**
     * @brief Virtual destructor
     * Ensures proper cleanup of derived class objects
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * This method computes weight changes based on the timing and pattern
     * of pre- and post-synaptic spikes. The specific learning rule is
     * implemented by each concrete subclass.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Duration of current simulation timestep
     */
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply a specific weight change to the synapse
     * 
     * Directly applies a weight delta to the synapse without considering
     * spike timing patterns. Useful for external control of plasticity.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param delta Weight change to apply
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * Returns a human-readable identifier for the plasticity rule type.
     * Used for logging, debugging, and configuration.
     * 
     * @return C-style string describing the rule
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if this plasticity rule is enabled
     * 
     * @return True if the rule is active and contributing to weight changes
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable this plasticity rule
     * 
     * @param enabled True to enable, false to disable the rule
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Protected constructor
     * 
     * Default constructor initializes the rule in enabled state. Derived
     * classes should call the base constructor and then set up their
     * specific parameters.
     */
    PlasticityRule() : enabled_(true) {}
    
private:
    /**
     * @brief Flag indicating whether plasticity is enabled
     * 
     * Controls whether this rule contributes to synaptic weight updates.
     * Can be toggled dynamically during simulation to pause learning.
     */
    bool enabled_;
};

/**
 * @class HebbianRule
 * @brief Hebbian plasticity rule implementing "fire together, wire together"
 * 
 * Implements the classical Hebbian learning rule where coincident pre- and
 * post-synaptic activity strengthens the connection. Uses a simplified
 * covariance-based learning rule with bounds to prevent runaway potentiation.
 * 
 * @note Real Hebbian learning requires tracking running averages of activity
 *       and implementing proper covariance computation.
 */

/**
 * @class AntiHebbianRule
 * @brief Anti-Hebbian plasticity rule implementing "fire together, wire apart"
 * 
 * Reduces synaptic weights when pre- and post-synaptic neurons fire
 * simultaneously. Used for stabilizing network activity and implementing
 * competition.
 */

/**
 * @class BCMRule
 * @brief Bienenstock-Cooper-Munro (BCM) plasticity rule
 * 
 * Implements a sliding threshold mechanism where potentiation occurs when
 * postsynaptic activity exceeds a threshold, and depression occurs when
 * it's below. Helps stabilize firing rates and implement homeostatic
 * plasticity.
 */

// Forward declarations for concrete rule classes
class HebbianRule;
class AntiHebbianRule;
class BCMRule;

} // namespace nlm
