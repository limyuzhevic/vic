/**
 * @file PlasticityRule.hpp
 * @brief Plasticity rules for synaptic weight modification
 * @namespace nlm
 * 
 * This file implements the plasticity rule framework for the NLM neural simulation
 * framework. Plasticity rules define how synaptic connections are modified based
 * on neural activity patterns, enabling learning and adaptation in neural networks.
 * 
 * The plasticity rule system provides abstract mechanisms for implementing various
 * learning algorithms, from simple Hebbian learning to complex spike-timing-dependent
 * plasticity (STDP) and reward-modulated learning. Each rule operates on synaptic
 * connections to strengthen or weaken them based on presynaptic and postsynaptic
 * activity patterns.
 * 
 * @note PlasticityRule.hpp provides the foundation for synaptic learning and memory
 *       in the NLM framework. The abstract base class defines the interface for all
 *       plasticity mechanisms, while concrete implementations provide specific
 *       learning algorithms with biological plausibility and computational efficiency.
 * 
 * @see Synapse.hpp - Synapses modified by plasticity rules
 * @see Neuron.hpp - Neurons that generate plasticity-related activity
 * @see Neuromodulator.hpp - Neuromodulatory signals affecting plasticity
 * @see PlasticityRule.cpp - Implementations of concrete plasticity rules
 */

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class PlasticityRule
 * @brief Abstract base class for all synaptic plasticity rules
 * @namespace nlm
 * 
 * This class defines the interface for synaptic plasticity mechanisms that modify
 * connection strengths based on neural activity. Plasticity rules implement various
 * learning algorithms, from simple correlation-based learning to complex
 * spike-timing-dependent and reward-modulated processes.
 * 
 * The abstract interface ensures that all plasticity rules provide essential
 * functionality for synaptic modification while allowing for specific implementations
 * tailored to different learning scenarios. Each concrete rule captures different
 * aspects of biological learning and adaptation.
 * 
 * @note PlasticityRule uses virtual methods to allow runtime polymorphism, enabling
 *       the NLM framework to support different plasticity mechanisms without changing
 *       the core neural simulation code. The enabled/disabled flag allows dynamic
 *       control over which plasticity rules are active in the network.
 * 
 * @see HebbianRule - Implements "fire together, wire together" learning
 * @see AntiHebbianRule - Implements "fire together, wire apart" learning
 * @see BCMRule - Implements Bienenstock-Cooper-Munro homeostatic plasticity
 * @see Synapse::enablePlasticity - Configures which rules are active for a synapse
 */
class PlasticityRule {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived implementations
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on pre- and postsynaptic activity
     * 
     * This is the primary learning mechanism that computes weight changes based on
     * the timing and patterns of neural activity. Different implementations capture
     * different learning principles, from simple correlation to precise spike timing.
     * 
     * @note PHASE 2: Enhanced implementations with biological realism and
     *       detailed computational models are planned for future development.
     *       The placeholder provides the basic interface and conceptual framework
     *       for synaptic learning.
     * 
     * @param synapse The synaptic connection to be modified
     * @param preSpikes History of presynaptic spike times (for STDP)
     * @param postSpikes History of postsynaptic spike times (for STDP)
     * @param dt Simulation timestep duration
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply a specific weight change to a synapse
     * 
     * This method directly applies a computed weight delta to a synaptic connection,
     * implementing the actual synaptic modification. It handles weight clamping
     * to biologically plausible ranges to ensure stability.
     * 
     * @param synapse The synaptic connection to modify
     * @param delta Weight change to apply (positive for potentiation, negative for depression)
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the name/identifier of this plasticity rule
     * 
     * @return const char* String name of the plasticity rule
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if this plasticity rule is currently enabled
     * 
     * @return bool true if the rule is enabled and can be applied, false otherwise
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable this plasticity rule
     * 
     * @param enabled New enabled state (true to enable, false to disable)
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Protected constructor ensuring proper initialization
     * 
     * By default, plasticity rules are enabled upon creation to maintain
     * backward compatibility and expected learning behavior.
     */
    PlasticityRule() : enabled_(true) {}
    
private:
    /**
     * @brief Flag indicating whether this rule is currently enabled
     * 
     * This flag controls whether the plasticity rule will be applied during
     * synaptic modification. It allows dynamic control over learning processes
     * without requiring removal of rules from the network.
     */
    bool enabled_;
};

/**
 * @class HebbianRule
 * @brief Implements Hebbian plasticity: "neurons that fire together, wire together"
 * @namespace nlm
 * 
 * This class implements Hebbian learning, one of the most fundamental principles
 * of synaptic plasticity. According to Hebb's rule, synaptic connections are
 * strengthened when the presynaptic and postsynaptic neurons fire together
 * in close temporal proximity.
 * 
 * The implementation captures the essence of Hebbian learning while providing
 * a foundation for more sophisticated variants that incorporate biological realism
 * and additional constraints such as homeostasis and normalization.
 * 
 * @note PHASE 2: Enhanced implementation with biological realism and
 *       detailed synaptic dynamics is planned for future development. The current
 *       placeholder provides the basic conceptual framework and interface
 *       for Hebbian learning in the NLM framework.
 * 
 * @see PlasticityRule - Abstract base class for all plasticity rules
 * @see AntiHebbianRule - Complementary rule implementing "fire together, wire apart"
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructor creating a Hebbian plasticity rule with default parameters
     */
    HebbianRule();
    
    /**
     * @brief Destructor for proper cleanup of Pimpl implementation
     */
    ~HebbianRule() override;
    
    /**
     * @brief Compute weight changes based on correlated firing
     * 
     * This method implements Hebbian learning by detecting coincident spikes
     * and applying weight potentiation proportional to the correlation strength.
     * The rule strengthens synapses when presynaptic and postsynaptic neurons
     * fire within a specified time window.
     * 
     * @param synapse The synaptic connection to modify
     * @param preSpikes History of presynaptic spike times
     * @param postSpikes History of postsynaptic spike times
     * @param dt Simulation timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply a specific weight change computed by the Hebbian rule
     * 
     * @param synapse The synaptic connection to modify
     * @param delta Weight change to apply (typically positive for Hebbian learning)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* String "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate parameter for Hebbian learning
     * 
     * The learning rate controls how strongly synapses are strengthened in response
     * to correlated activity. It must be positive and is typically between 0.0 and 1.0.
     * 
     * @param rate Learning rate (0.0 <= rate <= 1.0)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate parameter
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    
private:
    /**
     * @brief Pimpl implementation structure (pointer to implementation)
     * 
     * The HebbianRule class uses the Pimpl idiom to hide implementation details
     * and allow efficient copying and comparison. The actual implementation is
     * contained in this private structure, which maintains the learning rate
     * and other computational parameters.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation structure
     */
    Impl* pImpl;
};

/**
 * @class AntiHebbianRule
 * @brief Implements anti-Hebbian plasticity: "neurons that fire together, wire apart"
 * @namespace nlm
 * 
 * This class implements anti-Hebbian learning, the complement to Hebbian learning.
 * According to anti-Hebbian principles, synaptic connections are weakened when
 * the presynaptic and postsynaptic neurons fire together, promoting diversity
 * and preventing runaway excitation in neural networks.
 * 
 * Anti-Hebbian learning is biologically plausible and important for maintaining
 * network stability, promoting competitive processes, and implementing certain
 * forms of unsupervised learning where divergent firing patterns are preferred.
 * 
 * @note This implementation is a placeholder with stub methods to be completed
 *       in future development phases. The conceptual framework is established
 *       for future implementation with biological realism.
 * 
 * @see PlasticityRule - Abstract base class for all plasticity rules
 * @see HebbianRule - Complementary rule implementing standard Hebbian learning
 */
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Compute weight changes based on correlated firing (placeholder)
     * 
     * This placeholder implementation follows the anti-Hebbian principle of weakening
     * connections when neurons fire together, but is currently implemented as a stub
     * to be completed in future development phases.
     * 
     * @param synapse The synaptic connection to modify
     * @param preSpikes History of presynaptic spike times
     * @param postSpikes History of postsynaptic spike times
     * @param dt Simulation timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply a weight change (placeholder)
     * 
     * This placeholder implementation currently does nothing but should apply
     * negative weight changes in a completed implementation.
     * 
     * @param synapse The synaptic connection to modify
     * @param delta Weight change to apply (typically negative for anti-Hebbian)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* String "AntiHebbian"
     */
    const char* getName() const override { return "AntiHebbian"; }
};

/**
 * @class BCMRule
 * @brief Implements Bienenstock-Cooper-Munro (BCM) homeostatic plasticity
 * @namespace nlm
 * 
 * This class implements the BCM learning rule, a homeostatic plasticity mechanism
 * that maintains synaptic stability by scaling synaptic weights based on the
 * postsynaptic neuron's average activity level (the sliding threshold).
 * 
 * BCM theory proposes that synapses stabilize when the postsynaptic firing rate
 * matches a dynamically adjusted threshold. When activity exceeds the threshold,
 * excitatory synapses are potentiated; when activity is below, they are depressed.
 * This implements a form of homeostatic regulation that prevents runaway excitation
 * and maintains network stability.
 * 
 * @note This implementation is a placeholder with stub methods to be completed
 *       in future development phases. The conceptual framework is established
 *       for future implementation with biological realism and detailed dynamics.
 * 
 * @see PlasticityRule - Abstract base class for all plasticity rules
 * @see HebbianRule - Associative learning rule for specific spike timing
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Compute weight changes based on BCM homeostatic mechanism (placeholder)
     * 
     * This placeholder implementation follows the BCM theory of synaptic stability
     * by comparing postsynaptic activity to a sliding threshold, but is currently
     * implemented as a stub to be completed in future development phases.
     * 
     * @param synapse The synaptic connection to modify
     * @param preSpikes History of presynaptic spike times
     * @param postSpikes History of postsynaptic spike times
     * @param dt Simulation timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply a weight change (placeholder)
     * 
     * This placeholder implementation currently does nothing but should apply
     * weight changes based on the BCM rule in a completed implementation.
     * 
     * @param synapse The synaptic connection to modify
     * @param delta Weight change to apply (based on BCM dynamics)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* String "BCM"
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
