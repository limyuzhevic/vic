#pragma once

#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
/**
 * @class Hebbian
 * @brief Hebbian learning rule implementation
 * 
 * This class implements Hebbian learning, the fundamental learning mechanism
 * where synaptic strength is increased when pre- and post-synaptic neurons
 * fire together in close temporal proximity. Often summarized as "fire together,
 * wire together", Hebbian learning forms the basis of many neural network
 * learning algorithms.
 * 
 * The core Hebbian principle: Δw = η * x_pre * x_post
 * where η is learning rate, x_pre and x_post are pre- and post-synaptic activities.
 * 
 * This implementation includes homeostatic regulation to maintain synaptic balance
 * and prevent runaway excitation in neural networks.
 * 
 * @note Homeostatic mechanisms maintain network stability by scaling weights
 *       based on overall network activity levels.
 * @see AntiHebbian for competitive learning (opposite of Hebbian)
 * @see BCM for threshold-based learning with stability guarantees
 */

class Hebbian : public PlasticityRule {
public:
    /**
     * @brief Construct a new Hebbian learning object
     * 
     * Initializes Hebbian learning with default parameters and internal implementation.
     */
    Hebbian();
    
    /**
     * @brief Destroy the Hebbian learning object
     * 
     * Cleans up internal Hebbian implementation and resources.
     */
    ~Hebbian() override;
    
    /**
     * @brief Update synaptic weights based on Hebbian learning
     * 
     * Implements Hebbian learning rule by analyzing pre- and post-synaptic
     * activity and strengthening synapses that contribute to correlated firing.
     * 
     * @param synapse Pointer to synapse to be updated
     * @param preSpikes Vector of pre-synaptic spike timestamps (in seconds)
     * @param postSpikes Vector of post-synaptic spike timestamps (in seconds)
     * @param dt Simulation timestep duration (in seconds)
     * 
     * @note Includes homeostatic regulation to prevent weight explosion
     * @see setLearningRate() for configuring learning rate
     * @see setMaxWeight() for configuring weight bounds
     */
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change with Hebbian constraints
     * 
     * Applies the calculated Hebbian weight change to the synapse with proper
     * bounds checking and homeostatic regulation.
     * 
     * @param synapse Pointer to synapse to modify
     * @param delta Calculated weight change (typically positive for Hebbian)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return String identifier for Hebbian rule
     * 
     * @note Returns "Hebbian" for identification in logging and debugging
     */
    const char* getName() const override;
    
    /**
     * @brief Set Hebbian learning rate
     * 
     * Configures the learning rate η that controls the magnitude of weight
     * updates in Hebbian learning.
     * 
     * @param rate Learning rate (must be non-negative)
     * @see getLearningRate() for current value
     * @see update() for usage in learning calculations
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set maximum synaptic weight
     * 
     * Configures the upper bound for synaptic weights in Hebbian learning.
     * Higher weights indicate stronger synaptic connections.
     * 
     * @param maxWeight Maximum allowed weight (must be greater than minimum weight)
     * @see getMaxWeight() for current value
     * @see setMinWeight() for configuring lower bound
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get maximum synaptic weight
     * 
     * @return Current maximum synaptic weight
     */
    float getMaxWeight() const;
    
private:
    /**
     * @brief Internal implementation (PIMPL pattern)
     * 
     * All actual Hebbian implementation details are hidden in this private struct,
     * including learning rate management, homeostatic regulation, and efficient
     * weight update calculations.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * Pimpl pointer for implementation hiding and reduced compilation dependencies.
     */
    Impl* pImpl;
};

} // namespace nlm
