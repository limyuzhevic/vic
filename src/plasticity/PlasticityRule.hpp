#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules
// 
// PlasticityRule defines the interface for synaptic weight changes based on neural activity.
// It implements the core learning mechanisms of the brain including:
// - Hebbian learning (fire together, wire together)
// - Spike-Timing-Dependent Plasticity (STDP)
// - Reward-modulated plasticity
// 
// Plasticity is fundamental to learning, memory, and adaptation in neural systems.
// Different rules implement different biological learning mechanisms.

class PlasticityRule {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Applies a synaptic weight change
     * 
     * @param synapse Target synapse to modify
     * @param delta Weight change amount (positive for strengthening, negative for weakening)
     * 
     * @details
     * This pure virtual function must be implemented by derived classes.
     * Weight changes are bounded to prevent runaway excitation/inhibition.
     * Typical bounds: [-1.0, 1.0] for normalized weights.
     * 
     * TODO PHASE 2: Implement real plasticity
     * 
     * Biological basis: Synaptic weight changes via molecular mechanisms including:
     * - LTP (Long-Term Potentiation)
     * - LTD (Long-Term Depression)
     * - Structural changes in synaptic architecture
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Gets the name of this plasticity rule
     * 
     * @return const char* Human-readable name of the rule
     * 
     * @details
     * Returns a descriptive name that identifies the learning mechanism.
     * Useful for debugging, logging, and configuration.
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Checks if this plasticity rule is currently enabled
     * 
     * @return bool True if rule is active (default: true)
     * 
     * @details
     * Rules can be individually enabled/disabled to control learning.
     * This is useful for implementing developmental stages where different
     * learning mechanisms become active at different times.
     */
    bool isEnabled() const;
    
    /**
     * @brief Enables or disables this plasticity rule
     * 
     * @param enabled New enabled state
     * 
     * @details
     * Disables plasticity rules during phases where learning should not occur,
     * such as during development (when connectivity is being refined) or during
     * memory consolidation (when learned connections should stabilize).
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Default constructor initializes rule as enabled
     */
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;  /**< Current enabled state of the plasticity rule */
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
// 
// Hebbian learning strengthens synapses when pre- and post-synaptic neurons
// fire in close temporal proximity. It implements the basic principle of
// associative learning in neural systems.
// 
// Mathematical formulation:
// Δw = η * x_pre * x_post
// 
// Where η is learning rate, x_pre and x_post are pre- and post-synaptic activities.
// 
// Biological basis: Synaptic strengthening through NMDA receptor activation
// and calcium influx, leading to long-term potentiation (LTP).
// 
// References:
// - Hebb (1949) "The Organization of Behavior"
// - Caporale & Danjw (2008) "Synaptic plasticity: Localization and timing"
// 
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructs a Hebbian rule instance
     */
    HebbianRule();
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    ~HebbianRule() override;
    
    /**
     * @brief Updates synaptic weights based on pre- and post-synaptic activity
     * 
     * @param synapse Target synapse to update
     * @param preSpikes Timestamps of presynaptic spikes
     * @param postSpikes Timestamps of postsynaptic spikes
     * @param dt Timestep duration
     * 
     * @details
     * Computes weight changes using pre- and post-synaptic activity history.
     * This is a placeholder implementation. Real implementation will use
     * spike timing information to compute precise weight changes.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a Hebbian weight change
     * 
     * @param synapse Target synapse
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Gets rule name
     * 
     * @return const char* Always returns "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Sets the learning rate for Hebbian plasticity
     * 
     * @param rate Learning rate (typically 0.01-0.1)
     * 
     * @details
     * Learning rate controls the strength of synaptic changes. Higher values
     * lead to faster learning but can cause instability. The optimal value
     * depends on network size, activity patterns, and desired learning speed.
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Gets the learning rate
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
// 
// Anti-Hebbian learning weakens synapses when pre- and post-synaptic neurons
// fire in close temporal proximity. This implements competition and separation
// of neural representations.
// 
// Mathematical formulation:
// Δw = -η * x_pre * x_post
// 
// Biological basis: Long-term depression (LTD) mechanisms that weaken synapses.
// 
// References:
// - Wang & Zhou (2008) "Anti-Hebbian learning"
// - Song et al. (2000) "Competitive learning in recurrent networks"
// 
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     */
    AntiHebbianRule() = default;
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    ~AntiHebbianRule() override = default;
    
    /**
     * @brief Updates synaptic weights (placeholder implementation)
     * 
     * @param synapse Target synapse
     * @param preSpikes Presynaptic spike timestamps
     * @param postSpikes Postsynaptic spike timestamps
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Applies anti-Hebbian weight change (placeholder)
     * 
     * @param synapse Target synapse
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Gets rule name
     * 
     * @return const char* Returns "AntiHebbian"
     */
    const char* getName() const override { return "AntiHebbian"; }
};

/**
 * @class BCMRule
 * @brief Implements Bienenstock-Cooper-Munro (BCM) plasticity rule
 * 
 * @details
 * The BCM rule is a homeostatically regulated learning rule that combines
 * Hebbian-like learning with a threshold mechanism that stabilizes synaptic
 * weights. It implements "sliding threshold" learning where the threshold
 * adapts to the average postsynaptic activity.
 * 
 * Mathematical formulation:
 * θ = <x²>  (sliding threshold)
 * Δw = x * (x - θ)  for x > θ
 * Δw = x * (x - θ)  for x < θ  (scaled)
 * 
 * Biological basis: Applications of BCM theory to cortical development and
 * visual system plasticity (Blum & Gözükara, 1992).
 * 
 * References:
 * - Bienenstock et al. (1982) "Theory for the development of neuron selectivity"
 * - Song et al. (2000) "Competitive learning in recurrent networks"
 * 
 * PLACEHOLDER - Phase 2
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     */
    BCMRule() = default;
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    ~BCMRule() override = default;
    
    /**
     * @brief Updates synaptic weights (placeholder implementation)
     * 
     * @param synapse Target synapse
     * @param preSpikes Presynaptic spike timestamps
     * @param postSpikes Postsynaptic spike timestamps
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Applies BCM weight change (placeholder)
     * 
     * @param synapse Target synapse
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Gets rule name
     * 
     * @return const char* Returns "BCM"
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
