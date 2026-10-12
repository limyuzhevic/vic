#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class PlasticityRule
 * @brief Abstract base class for synaptic plasticity rules
 * 
 * This class defines the interface for all synaptic plasticity rules in the NLM brain.
 * Synaptic plasticity is the process by which synaptic connections between neurons
 * are strengthened or weakened based on activity, enabling learning and memory.
 * 
 * @details Plasticity rules implement biological learning rules such as:
 * - Hebbian learning: "neurons that fire together, wire together"
 * - Spike-Timing Dependent Plasticity (STDP): timing-based synaptic modification
 * - Reward-modulated plasticity: learning influenced by reward signals
 * - BCM theory: supervised-like learning with sliding threshold
 * 
 * The plasticity rule system allows the brain to:
 * - Adapt its connectivity through experience
 * - Form meaningful synaptic relationships
 * - Maintain stability through homeostatic mechanisms
 * - Implement diverse learning algorithms
 * 
 * Concrete implementations include Hebbian, STDP, BCM, and reward-modulated rules.
 */
class PlasticityRule {
public:
    /**
     * @brief Default constructor
     */
    PlasticityRule();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Copy constructor deleted - plasticity rules cannot be copied
     * @param other Rule to copy (not used)
     */
    PlasticityRule(const PlasticityRule&) = delete;
    
    /**
     * @brief Copy assignment operator deleted - plasticity rules cannot be copied
     * @param other Rule to assign (not used)
     * @return Reference to this rule (never used)
     */
    PlasticityRule& operator=(const PlasticityRule&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Rule to move from
     */
    PlasticityRule(PlasticityRule&&) noexcept = default;
    
    /**
     * @brief Move assignment operator
     * @param other Rule to move from
     * @return Reference to this rule for chaining
     */
    PlasticityRule& operator=(PlasticityRule&&) noexcept = default;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * This is the core learning function that implements the specific plasticity rule.
     * It analyzes spike timing and synaptic history to determine weight changes.
     * 
     * @param synapse Pointer to the synapse being updated
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps  
     * @param dt Timestep duration for numerical integration
     * 
     * @note Concrete implementations should analyze the spike timing differences
     * to determine whether to potentiate or depress the synaptic connection.
     */
    virtual void update(Synapse* synapse, 
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) = 0;
    
    /**
     * @brief Apply accumulated weight change to a synapse
     * 
     * This function applies the computed weight change delta to the synaptic
     * connection, ensuring the weight stays within biological bounds.
     * 
     * @param synapse Pointer to the synapse to update
     * @param delta Weight change to apply (positive for potentiation, negative for depression)
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Human-readable name of the rule
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if the plasticity rule is currently enabled
     * 
     * Some plasticity rules can be globally enabled or disabled, for example
     * during different developmental stages or behavioral states.
     * 
     * @return true if rule is enabled, false otherwise
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable the plasticity rule
     * 
     * @param enabled New enabled state
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Protected constructor for base class
     * 
     * @param enabled Initial enabled state (default: true)
     */
    explicit PlasticityRule(bool enabled = true);
    
private:
    /**
     * @brief Enabled state flag
     * 
     * Determines whether this plasticity rule is active. Rules can be disabled
     * during specific phases (e.g., critical periods) or behavioral states.
     */
    bool enabled_;
};

/**
 * @class HebbianRule
 * @brief Implementation of Hebbian plasticity: "fire together, wire together"
 * 
 * This rule implements the classic Hebbian learning principle where synaptic
 * connections between neurons that fire simultaneously or in close succession
 * are strengthened. This implements the fundamental mechanism for forming
 * meaningful neural associations.
 * 
 * @details The Hebbian rule can be implemented in different ways:
 * - Simple rate-based: average firing rates of pre- and post-synaptic neurons
 * - Spike-based: timing of individual spikes
 * - Window-based: firing rate within a time window
 * 
 * The rule typically strengthens synapses when the post-synaptic neuron
 * fires shortly after the pre-synaptic neuron, reinforcing the causal
 * relationship between the neurons.
 * 
 * @see PlasticityRule For base class interface
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Construct a new HebbianRule with default parameters
     */
    HebbianRule();
    
    /**
     * @brief Destroy the HebbianRule and clean up
     */
    ~HebbianRule() override;
    
    /**
     * @brief Copy constructor deleted
     */
    HebbianRule(const HebbianRule&) = delete;
    
    /**
     * @brief Copy assignment operator deleted
     */
    HebbianRule& operator=(const HebbianRule&) = delete;
    
    /**
     * @brief Move constructor
     */
    HebbianRule(HebbianRule&&) noexcept = default;
    
    /**
     * @brief Move assignment operator
     */
    HebbianRule& operator=(HebbianRule&&) noexcept = default;
    
    /**
     * @brief Update synaptic weights based on Hebbian learning
     * 
     * Implements Hebbian learning by strengthening synapses when pre-synaptic
     * neurons fire before post-synaptic neurons, with decay for larger time gaps.
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for integration
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change to synapse
     * 
     * @param synapse Synapse to update
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name
     * 
     * @return const char* Always returns "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate for Hebbian plasticity
     * 
     * Controls the strength of synaptic modification. Higher learning rates
     * result in faster but less precise learning.
     * 
     * @param rate Learning rate (0.0 to 1.0)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    
private:
    /**
     * @brief Pimpl idiom implementation details
     */
    struct Impl;
    Impl* pImpl;
};

/**
 * @class AntiHebbianRule
 * @brief Anti-Hebbian plasticity: "fire together, weaken together"
 * 
 * This rule implements the opposite of Hebbian learning, where synaptic
 * connections are weakened when the connected neurons fire together.
 * This can be useful for implementing competitive learning, normalization,
 * or implementing specific behavioral constraints where certain connections
 * should be suppressed.
 * 
 * Anti-Hebbian learning is observed in some biological systems and can help
 * maintain network stability by preventing runaway excitation.
 */
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Construct an AntiHebbianRule
     */
    AntiHebbianRule() : PlasticityRule() {}
    
    /**
     * @brief Update synaptic weights based on anti-Hebbian learning
     * 
     * Weakens synapses when pre- and post-synaptic neurons fire together,
     * implementing competitive learning dynamics.
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for integration
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change to synapse
     * 
     * @param synapse Synapse to update
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name
     * 
     * @return const char* Always returns "AntiHebbian"
     */
    const char* getName() const override { return "AntiHebbian"; }
};

/**
 * @class BCMRule
 * @brief Bienenstock-Cooper-Munro (BCM) plasticity rule
 * 
 * This is a pseudo-supervised learning rule that approximates error-driven
 * learning without explicit reward signals. It uses a sliding threshold
 * (theta_m) that moves with the average postsynaptic activity, allowing the
 * network to learn meaningful representations while maintaining stability.
 * 
 * BCM implements synaptic learning rules observed in biological systems:
 * - Long-term potentiation when activity exceeds threshold
 * - Long-term depression when activity is below threshold
 * - Homeostatic regulation through theta_m adaptation
 * 
 * This rule is particularly useful for unsupervised learning and feature
 * extraction in developing neural networks.
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Construct a BCMRule with default parameters
     */
    BCMRule() : PlasticityRule() {}
    
    /**
     * @brief Update synaptic weights based on BCM learning rule
     * 
     * Implements BCM rule: delta_w = eta * (y - theta_m) * x
     * where y is postsynaptic activity, theta_m is sliding threshold,
     * x is presynaptic activity, and eta is learning rate.
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for integration
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change to synapse
     * 
     * @param synapse Synapse to update
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name
     * 
     * @return const char* Always returns "BCM"
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
