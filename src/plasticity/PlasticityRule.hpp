#pragma once

/**
 * @file PlasticityRule.hpp
 * @brief Plasticity rules for synaptic learning and adaptation
 * @author NLM Project
 * @date Phase 6 Implementation
 * @version 0.1.0
 * 
 * This file defines the plasticity rule system for NLM, implementing various
 * forms of synaptic learning and adaptation. Plasticity rules govern how
 * synaptic weights change in response to neural activity, enabling learning
 * and memory formation.
 * 
 * The plasticity system includes:
 * - Hebbian learning (STDP-like spike timing-dependent plasticity)
 * - Anti-Hebbian learning (for disinhibition and competition)
 * - BCM (Bienenstock-Cooper-Munro) learning (sliding threshold)
 * - Neuromodulation integration (reward-based learning)
 * - Structural plasticity (connection growth and pruning)
 * 
 * These rules work together with neuromodulatory signals (dopamine, curiosity,
 * novelty) to implement biologically plausible learning mechanisms.
 * 
 * Plasticity is fundamental to NLM's ability to:
 * - Adapt to environmental changes
 * - Form associations between events
 * - Learn through reward and prediction error
 * - Develop through developmental stages
 * - Consolidate and replay memories
 */

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @brief Abstract base class for all synaptic plasticity rules
 * 
 * PlasticityRule defines the interface for synaptic learning mechanisms.
 * All plasticity rules must implement:
 * - weight update based on pre/post synaptic activity
 * - weight change application
 * - rule identification
 * - enable/disable functionality
 * 
 * Plasticity rules are applied during neural processing and respond to
 * spike timing, neural activity patterns, and neuromodulatory signals.
 * They enable the brain to learn from experience and adapt its connectivity.
 * 
 * Biological plausibility is maintained through:
 * - Spike timing-dependent mechanisms
 * - Weight normalization and saturation
 * - Neuromodulation integration
 * - Competition between synapses
 * - Developmental regulation
 * 
 * Implementation Notes:
 * - Rules are applied to individual synapses
 * - Multiple plasticity rules can operate simultaneously
 * - Neuromodulatory signals modulate plasticity effectiveness
 * - Developmental stages affect plasticity rates
 * - Structural changes occur separately from weight changes
 */
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on neural activity
     * 
     * This is the main plasticity update function, called during each
     * simulation step. It analyzes pre-synaptic and post-synaptic spike
     * activity and computes appropriate weight changes.
     * 
     * @param synapse Target synapse for weight updates
     * @param preSpikes Timestamps of recent presynaptic spikes
     * @param postSpikes Timestamps of recent postsynaptic spikes
     * @param dt Time step duration for the update
     * 
     * @note The exact learning mechanism depends on the specific rule:
     *       - HebbianRule: Spike timing-dependent plasticity
     *       - AntiHebbianRule: Opposite of Hebbian learning
     *       - BCMRule: Sliding threshold mechanism
     *       - Other rules: Various learning algorithms
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply a specific weight change to a synapse
     * 
     * Applies a computed weight change directly to the synapse, ensuring
     * the result remains within biologically plausible bounds.
     * 
     * @param synapse Target synapse
     * @param delta Weight change to apply (can be positive or negative)
     * 
     * @note This method typically includes:
     *       - Weight clamping to prevent runaway excitation
     *       - Normalization for competitive interactions
     *       - Recording eligibility traces for neuromodulation
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the human-readable name of this plasticity rule
     * 
     * @return String identifier for the rule (e.g., "Hebbian", "STDP", "BCM")
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if this plasticity rule is currently enabled
     * 
     * Rules can be enabled or disabled dynamically, allowing for:
     * - Turning off learning during certain phases
     * - Implementing developmental stages with different plasticity levels
     * - Experimental control over learning mechanisms
     * 
     * @return True if rule is enabled, false otherwise
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable this plasticity rule
     * 
     * @param enabled New enabled state
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Default constructor
     * 
     * Plasticity rules start enabled by default, allowing for fine-grained
     * control by explicitly disabling specific rules.
     */
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_; /**< Whether this plasticity rule is active */
};

/**
 * @brief Hebbian plasticity rule: "fire together, wire together"
 * 
 * Implements Hebbian learning, one of the most fundamental forms of
 * synaptic plasticity in biological systems. The principle states that
 * if a presynaptic neuron repeatedly contributes to firing a postsynaptic
 * neuron, the synaptic connection between them will strengthen.
 * 
 * This implementation includes:
 * - Spike timing-dependent plasticity (STDP-like mechanisms)
 * - Both long-term potentiation (LTP) and long-term depression (LTD)
 * - Biologically realistic time windows and decay functions
 * - Integration with neuromodulation and reward signals
 * - Weight normalization to prevent runaway excitation
 * 
 * Biological Features:
 * - Time window for spike pairing (~20ms)
 * - Asymmetric decay for LTP vs LTD
 * - Saturation limits (-1.0 to +1.0)
 * - Competition between synapses
 * - Developmental regulation
 * 
 * Applications:
 * - Pattern recognition and association formation
 * - Motor skill learning
 * - Memory consolidation
 * - Behavioral adaptation
 * 
 * The Hebbian rule is essential for:
 * - Simple stimulus-response learning
 * - Feature detection in sensory processing
 * - Development of behavioral repertoires
 * - Formation of cognitive associations
 */
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    /**
     * @brief Update synaptic weights using Hebbian learning
     * 
     * Analyzes spike timing patterns and implements spike timing-dependent
     * plasticity, strengthening synapses when presynaptic activity
     * consistently precedes postsynaptic activity.
     * 
     * @param synapse Target synapse
     * @param preSpikes Presynaptic spike timestamps
     * @param postSpikes Postsynaptic spike timestamps
     * @param dt Time step
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change with saturation and normalization
     * 
     * @param synapse Target synapse
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this rule
     * 
     * @return "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Configure Hebbian learning rate
     * 
     * Sets the learning rate within biologically plausible ranges. Hebbian
     * learning rates in biological systems typically range from 0.001 to 0.1.
     * 
     * @param rate Learning rate (0.001 to 0.1 recommended)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl; /**< Private implementation for forward declaration */
};

/**
 * @brief Anti-Hebbian rule: "fire together, weaken together"
 * 
 * Implements the opposite of Hebbian learning, where concurrent activation
 * of presynaptic and postsynaptic neurons leads to synaptic depression.
 * This rule is important for:
 * - Response inhibition and competition
 * - Dynamic gain control
 * - Balanced network activity
 * - Preventing runaway excitation
 * 
 * Anti-Hebbian mechanisms are found in biological systems and help:
 * - Implement lateral inhibition
 * - Maintain network stability
 * - Enhance contrast and salience
 * - Support disinhibitory circuits
 * 
 * This is a simplified placeholder implementation that should be
 * expanded with biological realism in future phases.
 */
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "AntiHebbian"; }
};

/**
 * @brief Bienenstock-Cooper-Munro (BCM) plasticity rule
 * 
 * Implements the BCM theory of synaptic modification, which includes a
 * sliding threshold that determines whether synaptic changes are positive
 * (potentiation) or negative (depression) based on postsynaptic activity.
 * 
 * Key features:
 * - Dynamic threshold that moves with average activity
 * - Symmetry-breaking for stable activity patterns
 * - Applications in orientation selectivity
 * - Developmental tuning
 * 
 * BCM rules are particularly good at:
 * - Forming stable activity patterns
 * - Implementing homeostatic plasticity
 * - Supporting feature detection
 * - Maintaining network stability
 * 
 * This is a placeholder implementation that should be expanded
 * with the full BCM mechanism in future phases.
 */
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
