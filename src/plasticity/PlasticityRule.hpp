#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @brief Abstract base class for all synaptic plasticity rules
 * 
 * PlasticityRule defines the fundamental interface for synaptic learning and
 * adaptation mechanisms in the NLM brain. It implements the core principles
 * of biological synaptic plasticity, including spike-timing-dependent learning,
 * Hebbian learning, and reward-modulated learning.
 * 
 * This abstract base class establishes the contract that all concrete plasticity
 * rules must follow, ensuring consistent behavior and integration with the
 * brain system's plasticity management infrastructure.
 * 
 * Key features:
 * - Abstract interface: Concrete rules must implement all pure virtual methods
 * - State management: Enables/ disables plasticity through flag system
 * - Timing information: Supports spike-timing-dependent mechanisms
 * - Synaptic integration: Provides synapse access for weight modification
 * - Learning coordination: Integrates with brain-wide plasticity management
 * - Extensible design: Easy to add new plasticity rule types
 * 
 * Plasticity rules are essential for:
 * - Learning from experience
 * - Memory formation and consolidation
 * - Adaptation to environmental changes
 * - Development and maturation
 * - Neuromodulation integration
 * 
 * The rule execution model:
 * 1. Update rules receive pre/post synaptic activity and timing
 * 2. Apply weight changes based on activity patterns
 * 3. Maintain eligibility traces for reward-modulated learning
 * 4. Integrate with neuromodulatory signals when available
 */
class PlasticityRule {
public:
    /**
     * @brief Virtual destructor for proper polymorphic destruction
     * 
     * @note Ensures that concrete plasticity rule objects are properly
     *       destroyed when deleted through base class pointers.
     */
    virtual ~PlasticityRule() = default;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHODS (MUST BE IMPLEMENTED)
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on pre- and post-synaptic activity
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This is the core plasticity computation method. It analyzes the
     *       timing relationships between pre- and post-synaptic spikes and
     *       computes appropriate weight changes based on the specific plasticity rule.
     *       
     * @note The method should handle the plasticity rule's specific learning
     *       mechanism, whether it's STDP, Hebbian, reward-modulated, or another variant.
     *       
     * @note This method may be called multiple times per simulation step
     *       depending on the rule's implementation and activity patterns.
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply a specific weight change to the synapse
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount (positive for potentiation, negative for depression)
     * 
     * @note This method implements the actual weight modification for the synapse.
     *       It should apply the computed delta to the synapse's current weight,
     *       taking into account any bounds or constraints specific to the rule.
     *       
     * @note This method may be called directly by learning algorithms or
     *       indirectly through the update() method. Concrete implementations
     *       must ensure that weight changes are applied within acceptable bounds
     *       and respect the synapse's plasticity flags.
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the human-readable name of this plasticity rule
     * 
     * @return const char* String name identifying the rule type
     * 
     * @note This is used for debugging, logging, and system configuration.
     *       Common rule names include "STDP", "Hebbian", "R-STDP", etc.
     */
    virtual const char* getName() const = 0;
    
    // -------------------------------------------------------------------------
    // CONCRETE METHODS (PROVIDED IMPLEMENTATION)
    // -------------------------------------------------------------------------
    
    /**
     * @brief Check if this plasticity rule is currently enabled
     * 
     * @return bool true if the rule is enabled and can modify synapses, false otherwise
     * 
     * @note Plasticity rules can be globally enabled or disabled through the
     *       enabled flag. This provides a mechanism to temporarily suspend
     *       learning while maintaining the rule's internal state.
     *       
     * @note Rules are enabled by default in the constructor but can be disabled
     *       through setEnabled(false) for specific learning periods or conditions.
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable this plasticity rule
     * 
     * @param enabled New enabled state (true to enable, false to disable)
     * 
     * @note Disabling a plasticity rule prevents it from modifying synaptic weights,
     *       but preserves its internal state and eligibility traces for future use.
     *       
     * @note This method is typically called by the brain system to control
     *       learning during different developmental stages, behavioral states,
     *       or experimental conditions.
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Protected constructor - PlasticityRule is abstract base class
     * 
     * @note Concrete plasticity rule classes should use their own constructors
     *       to initialize rule-specific parameters and state.
     *       
     * @note The enabled_ flag is initialized to true, allowing rules to start
     *       in an active learning state.
     */
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;  ///< Flag indicating whether this plasticity rule is currently enabled
};

/**
 * @brief Hebbian plasticity rule: "neurons that fire together, wire together"
 * 
 * This concrete implementation of PlasticityRule implements Hebbian learning,
 * the fundamental associative learning mechanism in neural networks. According to
 * Hebb's rule, synaptic connections between neurons that fire together increase in
 * strength, while connections between neurons that fire asynchronously decrease.
 * 
 * Key features:
 * - Activity-dependent potentiation: Co-activation strengthens connections
 * - Slow decay: Synapses weaken over time when not used
 * - Competitive learning: Resources are allocated based on co-activation patterns
 * - Integration with network dynamics: Responds to actual neural activity
 * 
 * Biological inspiration:
 * - Synaptic strengthening through repeated co-activation
 * - Critical period learning in development
 * - Memory formation through pattern completion
 * - Association learning in cognitive processes
 * 
 * Phase 2 TODO: Implement real Hebbian learning with:
 * - Practical learning rate parameters
 * - Activity-dependent decay mechanisms
 * - Integration with spike-timing information
 * - Normalization to prevent runaway excitation
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Construct a new HebbianRule instance
     * 
     * @note Initializes the rule with default parameters. The learning rate
     *       and other rule-specific parameters can be configured through
     *       setter methods after construction.
     */
    HebbianRule();
    
    /**
     * @brief Destroy the HebbianRule instance
     * 
     * @note Ensures proper cleanup of rule-specific state and resources.
     */
    ~HebbianRule() override;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHOD IMPLEMENTATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on pre- and post-synaptic co-activation
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This implementation computes weight changes based on the correlation
     *       between pre- and post-synaptic activity. The strength of the association
     *       determines the magnitude and sign of the weight change.
     *       
     * @note Phase 2: Real implementation will include:
     * - Co-activation detection algorithms
     * - Learning rate parameter tuning
     * - Activity-dependent normalization
     * - Integration with existing synaptic state
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a Hebbian weight change to the synapse
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount (positive for potentiation)
     * 
     * @note This implementation applies the computed delta to the synapse's
     *       current weight, respecting the synapse's plasticity flags and
     *       any rule-specific constraints.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return const char* Always returns "Hebbian"
     */
    const char* getName() const override;
    
    // -------------------------------------------------------------------------
    // HEBBIAN-SPECIFIC METHODS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Set the learning rate parameter
     * 
     * @param rate Learning rate (typically 0.0 to 1.0)
     * 
     * @note Learning rate controls the strength of weight changes.
     *       Higher rates lead to faster learning but may cause instability.
     *       Lower rates are more stable but require more time to learn.
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return float Current learning rate
     * 
     * @note This parameter can be adjusted over time to control learning speed
     *       and prevent runaway excitation or runaway inhibition.
     */
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern containing learning state
};

/**
 * @brief Anti-Hebbian rule: decrease weight when neurons fire together
 * 
 * This plasticity rule implements the opposite of Hebbian learning, decreasing
 * synaptic strength when pre- and post-synaptic neurons fire together. Anti-Hebbian
 * learning promotes competition and can implement inhibitory relationships,
 * stabilizing network dynamics, and implementing specific computational patterns
 * like edge detection or feature selection.
 * 
 * Key features:
 * - Competitive learning: Stronger connections suppress weaker ones
 * - Stabilization: Prevents runaway excitation in networks
 * - Feature selection: Enhances contrast between active and inactive patterns
 * - Normalization: Maintains overall network stability
 * 
 * Applications:
 * - Contrast enhancement in neural processing
 * - Competitive neural circuits
 * - Feature detection and selection
 * - Stabilization of network dynamics
 * 
 * Biological plausibility:
 * - Inhibitory interneurons implementing anti-Hebbian effects
 * - Competitive synaptic competition in neural development
 * - Homeostatic mechanisms preventing runaway excitation
 * - Activity-dependent normalization processes
 * 
 * Phase 2 TODO: Implement real anti-Hebbian learning with:
 * - Competitive weight modification algorithms
 * - Normalization mechanisms
 * - Integration with existing synaptic state
 * - Competition with Hebbian processes
 */
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Construct a new AntiHebbianRule instance
     * 
     * @note Initializes the rule with default parameters. Anti-Hebbian learning
     *       will be active by default unless disabled through setEnabled(false).
     */
    AntiHebbianRule();
    
    /**
     * @brief Destroy the AntiHebbianRule instance
     */
    ~AntiHebbianRule() override;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHOD IMPLEMENTATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on anti-Hebbian learning
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This implementation decreases synaptic weight when pre- and post-
     *       synaptic neurons fire together, implementing competitive learning.
     *       Phase 2: Real implementation will include competitive weight modification
     *       and normalization mechanisms.
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply an anti-Hebbian weight change
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount (typically negative for depression)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return const char* Always returns "AntiHebbian"
     */
    const char* getName() const override;
};

/**
 * @brief Bienenstock-Cooper-Munro (BCM) rule: homeostatics and sliding threshold
 * 
 * The BCM rule implements a homeostatic learning mechanism with a sliding threshold
 * that adapts based on recent average activity. This rule combines Hebbian learning
 * with homeostatic regulation to maintain stable network activity while allowing
 * learning and memory formation.
 * 
 * Key features:
 * - Sliding threshold: Theta phase changes based on recent activity
 * - Homeostatic regulation: Maintains stable overall firing rates
 * - Competition: Stronger synapses compete for limited resources
 * - Stability-plasticity balance: Preserves learned information while enabling adaptation
 * 
 * Biological inspiration:
 * - Synaptic scaling and homeostatic plasticity
 * - Activity-dependent threshold modulation
 * - BCM theory of neural development and learning
 * - Synaptic competition in cortical circuits
 * 
 * Phase 2 TODO: Implement real BCM rule with:
 * - Sliding threshold computation based on average activity
 * - Homeostatic weight normalization
 * - Competitive learning dynamics
 * - Integration with other plasticity mechanisms
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Construct a new BCMRule instance
     * 
     * @note Initializes the rule with default BCM parameters including
     *       initial theta threshold and learning rates.
     */
    BCMRule();
    
    /**
     * @brief Destroy the BCMRule instance
     */
    ~BCMRule() override;
    
    // -------------------------------------------------------------------------
    // ABSTRACT VIRTUAL METHOD IMPLEMENTATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update synaptic weights based on BCM learning rule
     * 
     * @param synapse The synaptic connection to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for the current update (seconds)
     * 
     * @note This implementation computes weight changes based on the BCM rule,
     *       which includes a sliding threshold theta that adapts based on recent
     *       post-synaptic activity. Phase 2: Real implementation will include
     *       the complete BCM learning dynamics with theta adaptation.
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a BCM weight change
     * 
     * @param synapse The synaptic connection to modify
     * @param delta The weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return const char* Always returns "BCM"
     */
    const char* getName() const override;
};

} // namespace nlm