#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class MetaPlasticity
 * @brief Meta-plasticity: plasticity that changes plasticity
 * 
 * MetaPlasticity implements the concept of "plasticity of plasticity" by dynamically
 * adjusting the parameters and effectiveness of other plasticity rules. This rule
 * provides meta-learning capabilities where learning itself becomes a learnable
 * parameter, allowing neural systems to adapt their own learning processes.
 * 
 * Meta-plasticity mechanisms work through several complementary processes:
 * 
 * 1. Learning rate adaptation: Dynamic adjustment of plasticity learning rates
 *    based on learning progress, stability, or performance.
 * 
 * 2. Parameter optimization: Meta-learning of plasticity rule parameters
 *    to optimize learning efficiency and stability.
 * 
 * 3. Plasticity modulation: Contextual modulation of plasticity based on
 *    network state, developmental stage, or task demands.
 * 
 * Learning rate adaptation:
 * dη/dt = α * (target_rate - η) where:
 *   η = current learning rate
 *   target_rate = desired learning rate
 *   α = meta-learning rate
 * 
 * Parameter optimization:
 * Δθ = β * ∂L/∂θ where:
 *   θ = plasticity parameters
 *   L = loss function measuring learning performance
 *   β = meta-learning rate
 * 
 * Biological inspiration:
 *   - Developmental changes in learning capacity
 *   - Adaptive learning strategies in different contexts
 *   - Meta-learning for optimizing learning efficiency
 *   - Critical period plasticity modulation
 * 
 * Applications:
 *   - Critical period closing/opening in development
 *   - Task-specific learning rate adaptation
 *   - Meta-learning for transfer learning
 *   - Optimizing learning strategies based on performance
 * 
 * @note This rule provides the foundation for adaptive plasticity
 *       and enables self-organizing learning systems.
 * @note Thread safety is ensured for all public methods.
 * 
 * @see HomeostaticPlasticity
 * @see NeuromodulatorPlasticity
 * @see PlasticityRule
 * 
 * @ingroup plasticity
 */

class MetaPlasticity : public PlasticityRule {
public:
    /**
     * @brief Constructs a MetaPlasticity instance with default parameters
     * 
     * Creates a meta-plasticity rule with default parameters for dynamic
     * adjustment of plasticity parameters. The default parameters provide
     * a balanced initial condition for meta-learning control.
     * 
     * @note The actual implementation details are hidden by the PImpl idiom.
     *       All parameter settings are forwarded to the implementation class.
     * 
     * @see configure
     * @see setMetaLearningRate
     * @see setTargetLearningRate
     */
    MetaPlasticity();
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up MetaPlasticity resources and performs finalization of
     * meta-learning algorithms.
     */
    ~MetaPlasticity() override;
    
    /**
     * @brief Update meta-learning control based on plasticity effectiveness
     * 
     * Implements the meta-learning mechanism by adjusting plasticity parameters
     * based on learning performance and stability. This method computes parameter
     * changes that optimize the plasticity process itself.
     * 
     * @param synapse Pointer to the synapse whose plasticity parameters will be adjusted.
     *               Must be a valid synapse pointer within the neural network.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     *                 Contains timestamps of all spikes that occurred at the
     *                 pre-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     *                  Contains timestamps of all spikes that occurred at the
     *                 post-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param dt Simulation timestep duration in milliseconds. Used for
     *           integration of the meta-learning control differential equation.
     * 
     * @note The meta-learning mechanism evolves according to:
     *       Δη = η_meta * (η_target - η_current) where:
     *       η = current learning rate
     *       η_target = target learning rate
     *       η_meta = meta-learning rate
     *       Δη = change in learning rate
     * @note This method is thread-safe and can be called concurrently for
     *       different synapses.
     * @note The rule maintains stability by preventing runaway learning
     *       through negative feedback on learning rate.
     * 
     * @see applyWeightChange
     * @see setMetaLearningRate
     * @see configure
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply meta-plastic weight change to a synapse
     * 
     * Applies synaptic weight changes based on meta-learning control to optimize
     * the plasticity process itself. This method adjusts learning rates and other
     * plasticity parameters to achieve optimal learning performance.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount, computed as η_meta * (η_target - η_current) * w
     *             where η_meta is meta-learning rate, η_target is target learning rate,
     *             η_current is current learning rate, and w is current synaptic weight.
     * 
     * @note The implementation ensures that plasticity parameters are adjusted
     *       in the direction that brings learning closer to the target rate.
     * @note Meta-plastic changes are typically slower than regular learning to
     *       maintain stable plasticity dynamics.
     * 
     * @see update
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of the meta-plasticity rule
     * 
     * Returns a descriptive string identifying this as a meta-plasticity
     * rule. This is useful for configuration, logging, and debugging.
     * 
     * @return Const char pointer to the string "MetaPlasticity".
     * 
     * @see update
     * @see applyWeightChange
     */
    const char* getName() const override;
    
    /**
     * @brief Set the target learning rate for meta-learning control
     * 
     * Configures the desired learning rate that the meta-learning mechanism
     * aims to maintain for the plasticity process. This parameter determines
     * the equilibrium point for the meta-learning control system.
     * 
     * @param targetLearningRate Target learning rate (0.0 < rate < 1.0).
     *                         Typical values range from 0.01 to 0.1.
     *                         Must be positive and less than 1.0.
     * 
     * @note The target learning rate determines when meta-learning adjustments
     *       are applied. When current learning rates deviate significantly from
     *       target, stronger meta-learning corrections are applied.
     * @note This parameter is crucial for determining the operating point of
     *       the network's meta-learning system.
     * 
     * @see getTargetLearningRate
     * @see configure
     */
    void setTargetLearningRate(float targetLearningRate);    // Target learning rate
    
    /**
     * @brief Get the current target learning rate
     * 
     * Returns the current target learning rate that the meta-learning mechanism
     * aims to maintain.
     * 
     * @return Current target learning rate (positive float value < 1.0).
     * 
     * @see setTargetLearningRate
     */
    float getTargetLearningRate() const;
    
    /**
     * @brief Set the meta-learning rate
     * 
     * Configures the learning rate that controls the speed of meta-learning
     * adjustments. This parameter determines how quickly plasticity parameters
     * are modified in response to deviations from target learning rates.
     * 
     * @param rate Meta-learning rate factor. Must be positive (0.0 < rate < 1.0).
     *            Typical biological values range from 0.0001 to 0.01.
     *            Smaller values produce slower, more gradual meta-learning adjustments.
     * 
     * @note The meta-learning rate should be significantly slower than
     *       regular learning rates to maintain stable plasticity dynamics.
     * @note This parameter is crucial for balancing stability vs. adaptability
     *       in the meta-learning control system.
     * 
     * @see getMetaLearningRate
     * @see configure
     */
    void setMetaLearningRate(float rate);
    
    /**
     * @brief Get the current meta-learning rate
     * 
     * Returns the current meta-learning rate that controls the speed
     * of meta-learning adjustments.
     * 
     * @return Current meta-learning rate (positive float value < 1.0).
     * 
     * @see setMetaLearningRate
     */
    float getMetaLearningRate() const;
    
    /**
     * @brief Configure all meta-plasticity parameters in a single call
     * 
     * Convenience method to set all meta-plasticity parameters
     * (target learning rate and meta-learning rate) simultaneously.
     * This ensures consistent parameter configuration and reduces the number
     * of separate setter calls.
     * 
     * @param targetLearningRate Target learning rate. Must be positive and < 1.0.
     * @param rate Meta-learning rate factor. Must be positive and < 1.0.
     * 
     * @note All parameters must have valid values before the configuration is applied.
     * @note This method is thread-safe and provides atomic configuration of
     *       all parameters.
     */
    void configure(float targetLearningRate, float rate);
    
    /**
     * @brief Estimate current learning rate from plasticity effectiveness
     * 
     * Estimates the current learning rate of a synapse based on its recent
     * plasticity effectiveness. This is used by the meta-learning mechanism
     * to determine how far the learning rate is from its target.
     * 
     * @param synapse Pointer to the synapse whose learning rate will be estimated.
     * @param dt Simulation timestep duration in milliseconds.
     * 
     * @return Estimated learning rate (positive float value < 1.0).
     * 
     * @note This method provides a simple effectiveness-based learning rate estimate
     *       that can be used by the meta-learning control system.
     * @note The learning rate is estimated based on recent weight changes
     *       relative to the current weight magnitude.
     */
    static float estimateLearningRate(Synapse* synapse, TimestepDuration dt);
    
private:
    /**
     * @brief PImpl implementation structure for MetaPlasticity
     * 
     * Internal implementation details for meta-plasticity are hidden by the
     * PImpl idiom. This structure contains the actual algorithmic implementation
     * of the meta-learning control and plasticity parameter optimization.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the MetaPlasticity implementation
     * 
     * Forward declaration of the implementation pointer used by the PImpl idiom
     * to hide the concrete implementation of MetaPlasticity from the header file.
     * This improves compilation times and encapsulation.
     */
    Impl* pImpl;
};

} // namespace nlm