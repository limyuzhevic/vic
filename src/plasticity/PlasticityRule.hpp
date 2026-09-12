#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class PlasticityRule
 * @brief Abstract base class defining the interface for synaptic plasticity rules
 * 
 * PlasticityRule is the fundamental interface for implementing synaptic modification
 * rules in the neural learning system. It defines the pure virtual methods that all
 * concrete plasticity implementations must provide to govern how synaptic weights
 * change in response to neural activity patterns.
 * 
 * The base class implements the core plasticity rule concept from neuroscience,
 * where synaptic strength is modified based on the timing and correlation of
 * pre- and post-synaptic neural activity. This interface supports both Hebbian
 * and anti-Hebbian learning principles, as well as more complex spike-timing
 * dependent plasticity mechanisms.
 * 
 * The class uses the PImpl idiom to hide implementation details and provides
 * basic functionality for enabling/disabling plasticity rules.
 * 
 * @note This class follows the CRTP (Curiously Recurring Template Pattern) for
 *       extensibility, allowing easy addition of new plasticity rules.
 * @note Thread safety is ensured for enabled() and setEnabled() methods.
 * 
 * @see STDP
 * @see Hebbian
 * @see HebbianRule
 * @see AntiHebbianRule
 * @see BCMRule
 * 
 * @ingroup plasticity
 */

class PlasticityRule {
public:
    /**
     * @brief Virtual destructor
     * 
     * Cleans up PlasticityRule resources and performs finalization of
     * derived class implementations. The virtual destructor ensures proper
     * cleanup of any resources allocated by concrete plasticity rule
     * implementations.
     * 
     * @note Due to the PImpl idiom, the destructor is typically defined
     *       in the implementation file.
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * This is the primary method that implements the core plasticity computation.
     * Concrete implementations must compute weight updates based on the provided
     * spike timing data and current synaptic state. This method is typically
     * called during the plasticity update phase of neural simulation steps.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     *               Must be a valid synapse pointer within the neural network.
     * @param preSpikes Vector of pre-synaptic spike times. Contains timestamps
     *                 of all spikes that occurred at the pre-synaptic neuron
     *                 since the last plasticity update. Empty if no spikes.
     * @param postSpikes Vector of post-synaptic spike times. Contains timestamps
     *                  of all spikes that occurred at the post-synaptic neuron
     *                  since the last plasticity update. Empty if no spikes.
     * @param dt Simulation timestep duration in milliseconds. Used for
     *           rate-based plasticity implementations and for temporal
     *           derivative calculations in continuous-time models.
     * 
     * @note The order of spike times in the vectors should be chronological.
     *       Concrete implementations may need to sort them or assume a specific
     *       ordering.
     * @note This method must be thread-safe if the plasticity rule is used
     *       in multi-threaded contexts.
     * @note Concrete implementations should handle edge cases such as
     *       simultaneous pre- and post-synaptic spikes, which may require
     *       special handling depending on the plasticity rule.
     * 
     * @see applyWeightChange
     * @see getName
     * @see isEnabled
     * 
     * @ingroup plasticity
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply accumulated weight change to a synapse
     * 
     * Applies a weight change delta to the specified synapse. This method
     * is typically called by concrete implementations of the update() method
     * after computing the weight modification. Some plasticity rules may
     * accumulate updates over multiple spikes before applying them.
     * 
     * @param synapse Pointer to the synapse to modify. Must be a valid
     *               synapse pointer within the neural network.
     * @param delta Weight change amount. Positive values strengthen (potentiate)
     *             the synapse, negative values weaken (depress) it. The exact
     *             magnitude and units depend on the specific plasticity rule
     *             and its parameter scaling.
     * 
     * @note The implementation should ensure numerical stability and
     *       respect synaptic weight bounds (typically -1.0 to +1.0 or 0.0 to +1.0
     *       depending on the model).
     * @note Concrete implementations should clip delta values to prevent
     *       numerical overflow or excessive weight changes in single steps.
     * 
     * @see update
     * @see getName
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the human-readable name of the plasticity rule
     * 
     * Returns a descriptive string identifying the plasticity rule implementation.
     * This is useful for configuration, logging, and debugging purposes.
     * 
     * @return Const char pointer to the rule name string. Returns a static
     *         string literal that should not be modified by callers.
     * 
     * @note The name should be unique across different plasticity rule types.
     *       Examples: "STDP", "Hebbian", "AntiHebbian", "BCM".
     * 
     * @see update
     * @see applyWeightChange
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if the plasticity rule is currently enabled
     * 
     * Determines whether this plasticity rule is active and should be applied
     * during neural simulation. Disabled rules will skip their update computations
     * for performance optimization or to temporarily disable specific learning
     * mechanisms.
     * 
     * @return true if the rule is enabled and active, false if disabled.
     * 
     * @note This flag is separate from the concrete implementation's internal
     *       logic and can be used for runtime control without recreating rules.
     * 
     * @see setEnabled
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable the plasticity rule
     * 
     * Enables or disables this plasticity rule globally. When disabled, the
     * rule will not participate in plasticity updates, allowing for runtime
     * control of learning mechanisms without recreating the rule object.
     * 
     * @param enabled true to enable the rule, false to disable it.
     * 
     * @note This method is thread-safe and can be called from any thread.
     *       Changes take effect immediately for subsequent update() calls.
     * 
     * @see isEnabled
     * @see update
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Default constructor
     * 
     * Initializes the plasticity rule with default settings. The default
     * constructor initializes the enabled_ flag to true, meaning the rule
     * starts in an active state.
     * 
     * @note Concrete implementations typically use the protected constructor
     *       to initialize their internal state and resources.
     */
    PlasticityRule() : enabled_(true) {}
    
private:
    /**
     * @brief Flag indicating whether the plasticity rule is enabled
     * 
     * This flag controls whether the plasticity rule is active. When false,
     * the rule will skip its update computations during simulation steps,
     * providing a lightweight way to disable learning without destroying
     * the rule object.
     * 
     * @note This flag is read and written by isEnabled() and setEnabled()
     *       methods, which are thread-safe.
     */
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructs a HebbianRule instance with default parameters
     * 
     * Creates a Hebbian plasticity rule implementation with default learning parameters.
     * This class provides a traditional Hebbian learning mechanism where synaptic
     * strength is potentiated when pre- and post-synaptic neurons fire together.
     * 
     * @note The actual implementation details are hidden by the PImpl idiom.
     *       All parameter settings are forwarded to the implementation class.
     * @note This class implements the classic Hebbian learning principle and serves
     *       as a foundation for more complex plasticity rules.
     * 
     * @see PlasticityRule
     * @see Hebbian
     * @see AntiHebbianRule
     */
    HebbianRule();
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up HebbianRule resources and performs finalization of synaptic weight
     * update algorithms. Due to the PImpl idiom, the destructor is defined
     * in the implementation file.
     */
    ~HebbianRule() override;
    
    /**
     * @brief Update synaptic weights based on spike coincidence
     * 
     * Implements the classic Hebbian learning algorithm by computing weight changes
     * based on the correlation between pre- and post-synaptic spike trains. The
     * rule strengthens synapses when both neurons are active simultaneously, providing
     * the foundation for associative memory formation.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     * @param dt Simulation timestep duration in milliseconds.
     * 
     * @note This implements the traditional Hebbian learning rule: Δw ∝ pre × post,
     *       where synaptic strengthening occurs with correlated pre- and post-synaptic
     *       activity.
     * @note Unlike STDP, this rule does not consider precise spike timing differences,
     *       only the coincidence of spikes within a tolerance window.
     * 
     * @see applyWeightChange
     * @see getName
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change to synapse
     * 
     * Applies the computed weight change to the specified synapse.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount from Hebbian computation.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * Returns "HebbianRule" to identify this as the classic Hebbian
     * plasticity implementation.
     * 
     * @return Const char pointer to the string "HebbianRule".
     */
    const char* getName() const override;
    
    /**
     * @brief Set learning rate for Hebbian updates
     * 
     * Configures the learning rate that controls the magnitude of synaptic
     * weight changes in HebbianRule. This parameter determines how strongly
     * correlated activity influences synaptic strength.
     * 
     * @param rate Learning rate factor. Positive values strengthen synapses
     *             when pre- and post-synaptic neurons fire together.
     * 
     * @see getLearningRate
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * 
     * Returns the current learning rate parameter.
     * 
     * @return Current learning rate factor.
     */
    float getLearningRate() const;
    
private:
    /**
     * @brief PImpl implementation structure for HebbianRule
     * 
     * Internal implementation details for HebbianRule are hidden by the
     * PImpl idiom. This structure contains the actual algorithmic implementation
     * of the classic Hebbian learning rule.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the HebbianRule implementation
     * 
     * Forward declaration of the implementation pointer used by the PImpl idiom
     * to hide the concrete implementation of HebbianRule.
     */
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an AntiHebbian plasticity rule with default parameters.
     * This rule implements the anti-Hebbian learning principle where synaptic
     * strength is decreased when pre- and post-synaptic neurons fire together,
     * providing stability and preventing runaway excitation.
     */
    AntiHebbianRule() = default;
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up AntiHebbianRule resources.
     */
    ~AntiHebbianRule() override = default;
    
    /**
     * @brief Update synaptic weights with anti-correlation
     * 
     * Implements the anti-Hebbian learning algorithm by computing weight changes
     * that decrease synaptic strength when pre- and post-synaptic neurons fire
     * together. This provides homeostatic regulation and network stability.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     * @param dt Simulation timestep duration in milliseconds.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply anti-Hebbian weight change
     * 
     * Applies weight changes that weaken synapses when neurons fire together.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount from anti-Hebbian computation.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get rule name
     * 
     * Returns "AntiHebbianRule" to identify this as the anti-Hebbian
     * plasticity implementation.
     * 
     * @return Const char pointer to the string "AntiHebbianRule".
     */
    const char* getName() const override { return "AntiHebbianRule"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a BCM plasticity rule with default parameters.
     * BCM rule implements a sliding threshold mechanism for synaptic learning,
     * providing stability and homeostatic properties to neural networks.
     */
    BCMRule() = default;
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up BCMRule resources.
     */
    ~BCMRule() override = default;
    
    /**
     * @brief Update synaptic weights with BCM learning rule
     * 
     * Implements the Bienenstock-Cooper-Munro learning rule, which uses a
     * sliding threshold mechanism to control synaptic plasticity. This rule
     * provides stability by implementing a form of homeostatic learning.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     * @param dt Simulation timestep duration in milliseconds.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply BCM weight change
     * 
     * Applies weight changes using the BCM learning rule with sliding threshold.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount from BCM computation.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get rule name
     * 
     * Returns "BCM" to identify this as the Bienenstock-Cooper-Munro
     * plasticity rule.
     * 
     * @return Const char pointer to the string "BCM".
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
