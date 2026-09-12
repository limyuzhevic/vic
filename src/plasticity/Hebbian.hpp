#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class Hebbian
 * @brief Implementation of Hebbian learning rule
 * 
 * Hebbian learning implements the classic "fire together, wire together" principle
 * from neuroscience, where synaptic strength is increased based on the simultaneous
 * or correlated activity of pre- and post-synaptic neurons. This rule strengthens
 * synapses when both neurons are active together, forming the basis for associative
 * memory and pattern recognition in neural networks.
 * 
 * The Hebbian rule is one of the simplest and most biologically plausible learning
 * mechanisms, originally proposed by Donald Hebb in 1949. It serves as the foundation
 * for many more complex learning rules and is essential for understanding how neural
 * systems develop and learn.
 * 
 * This implementation uses an activity-based covariance approach, computing synaptic
 * weight changes based on the correlation between pre- and post-synaptic spike trains.
 * The rule is particularly effective for stabilizing neural representations and
 * forming coherent memory patterns.
 * 
 * @note This class uses the PImpl idiom for hiding implementation details and
 *       efficiently managing synaptic weight update algorithms.
 * @note Thread safety is ensured for all public methods.
 * @note Current implementation provides the core Hebbian framework with
 *       parameterizable learning dynamics.
 * 
 * @see PlasticityRule
 * @see STDP
 * @see HebbianRule
 * @see AntiHebbianRule
 * @see BCMRule
 * 
 * @ingroup plasticity
 */

class Hebbian : public PlasticityRule {
public:
    /**
     * @brief Constructs a Hebbian learning instance with default parameters
     * 
     * Creates a Hebbian plasticity rule with default learning parameters.
     * The default parameters provide a balanced learning rate for synaptic
     * strengthening while maintaining numerical stability and preventing
     * runaway excitation in the network.
     * 
     * @note The actual implementation details are hidden by the PImpl idiom.
     *       All parameter settings are forwarded to the implementation class.
     * 
     * @see configure
     * @see setLearningRate
     * @see setMaxWeight
     */
    Hebbian();
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up Hebbian resources and performs finalization of synaptic weight
     * update algorithms. Due to the PImpl idiom, the destructor is defined
     * in the implementation file.
     * 
     * @note Properly cleans up any allocated resources used by the internal
     *       synaptic weight update computations.
     */
    ~Hebbian() override;
    
    /**
     * @brief Update synaptic weights based on spike correlation
     * 
     * Implements the core Hebbian learning algorithm by computing weight changes
     * based on the correlation between pre- and post-synaptic spike trains. The
     * rule strengthens synapses when both neurons fire together, implementing the
     * classic "fire together, wire together" principle.
     * 
     * @param synapse Pointer to the synapse whose weight will be modified.
     *               Must be a valid synapse pointer within the neural network.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     *                 Contains timestamps of all spikes that occurred at the
     *                 pre-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     *                  Contains timestamps of all spikes that occurred at the
     *                  post-synaptic neuron since the last plasticity update.
     *                  Sorted in chronological order.
     * @param dt Simulation timestep duration in milliseconds. Used for
     *           rate-based implementations and temporal integration of spike trains.
     * 
     * @note The Hebbian algorithm computes weight changes using a covariance
     *       measure between pre- and post-synaptic activities. Positive correlations
     *       lead to synaptic potentiation, while negative correlations lead to
     *       depression.
     * @note Implementation complexity is O(n×m) where n and m are the numbers
     *       of pre- and post-synaptic spikes respectively.
     * @note This method is thread-safe and can be called concurrently for
     *       different synapses.
     * @note The rule implements the BCM (Bienenstock-Cooper-Munro) learning
     *       heuristic by default, with adjustable threshold dynamics.
     * 
     * @see applyWeightChange
     * @see getName
     * @see setLearningRate
     * @see setMaxWeight
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply a direct weight change to a synapse
     * 
     * Applies a weight change delta directly to the specified synapse,
     * bypassing the correlation computation. This method is used internally
     * by the update() method after computing the Hebbian weight modification.
     * It can also be used for external weight manipulation.
     * 
     * @param synapse Pointer to the synapse to modify. Must be a valid
     *               synapse pointer within the neural network.
     * @param delta Weight change amount in synaptic weight units. Positive
     *             values potentiate the synapse, negative values depress it.
     *             The delta represents the net weight change from the Hebbian
     *             computation.
     * 
     * @note The implementation ensures numerical stability by clipping
     *       delta values to prevent excessive weight changes and maintaining
     *       synaptic weight bounds.
     * @note This method should respect the synaptic weight limits (typically
     *       -1.0 to +1.0 or 0.0 to +1.0 depending on the neural model).
     * 
     * @see update
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of the Hebbian rule
     * 
     * Returns a descriptive string identifying this as a Hebbian learning rule.
     * This is useful for configuration, logging, and debugging purposes.
     * 
     * @return Const char pointer to the string "Hebbian". This is a static
     *         string literal that should not be modified by callers.
     * 
     * @note The name uniquely identifies this plasticity rule among different
     *       Hebbian variants (e.g., "AntiHebbian", "BCM").
     * 
     * @see update
     * @see applyWeightChange
     */
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate parameter
     * 
     * Configures the learning rate that controls the magnitude of synaptic
     * weight changes in the Hebbian rule. This parameter determines how quickly
     * synapses strengthen or weaken in response to correlated activity.
     * 
     * @param rate The learning rate factor. Must be positive (0.0 < rate).
     *            Typical biological values range from 0.001 to 0.1. Larger
     *            values produce faster learning but may cause instability.
     * 
     * @note The learning rate scales the covariance computation between
     *       pre- and post-synaptic activities to determine the final weight change.
     * @note This parameter should be set during rule configuration, typically
     *       through the configure() method if available.
     * @note The learning rate is crucial for balancing exploration vs. exploitation
     *       in learning systems.
     * 
     * @see getLearningRate
     * @see setMaxWeight
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate parameter
     * 
     * Returns the current learning rate that controls the magnitude of
     * synaptic weight changes.
     * 
     * @return Current learning rate factor (positive float value).
     * 
     * @see setLearningRate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set the maximum synaptic weight limit
     * 
     * Configures the upper bound for synaptic weights to prevent runaway
     * excitation and maintain network stability. This parameter defines the
     * saturation point for synaptic strengthening.
     * 
     * @param maxWeight The maximum allowed synaptic weight. Must be positive
     *                 (0.0 < maxWeight). Typical values range from 0.5 to 10.0,
     *                 depending on the neural model and scale.
     * 
     * @note The actual weight range may also have a minimum (typically 0.0
     *       or -1.0) depending on the neural model implementation.
     * @note This parameter is essential for maintaining network stability and
     *       preventing unbounded growth of synaptic weights.
     * 
     * @see getMaxWeight
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get the current maximum synaptic weight limit
     * 
     * Returns the current maximum allowed synaptic weight.
     * 
     * @return Current maximum synaptic weight (positive float value).
     * 
     * @see setMaxWeight
     */
    float getMaxWeight() const;
    
private:
    /**
     * @brief PImpl implementation structure for Hebbian learning
     * 
     * Internal implementation details for Hebbian learning are hidden by the
     * PImpl idiom. This structure contains the actual algorithmic implementation
     * of the spike correlation-based synaptic weight updates.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the Hebbian learning implementation
     * 
     * Forward declaration of the implementation pointer used by the PImpl idiom
     * to hide the concrete implementation of Hebbian from the header file.
     * This improves compilation times and encapsulation.
     */
    Impl* pImpl;
};

} // namespace nlm
