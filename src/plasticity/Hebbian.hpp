#pragma once

/**
 * @file Hebbian.hpp
 * @brief Hebbian learning implementations.
 * 
 * This header defines Hebbian plasticity implementations for the NLM neural network.
 * Hebbian learning implements the principle "neurons that fire together, wire together."
 * It strengthens connections between neurons that activate simultaneously, forming
 * associations and patterns of coordinated activity.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Types/Types.hpp
 * @see src/plasticity/PlasticityRule.hpp
 * @see src/plasticity/STDP.hpp
 * @see src/plasticity/StructuralPlasticity.hpp
 * @see TODO Complete Hebbian implementations (Phase 2)
 */

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * Concrete implementation of Hebbian learning.
 * 
 * Implements the classic Hebbian learning rule where synaptic weight changes
 * are proportional to the product of pre- and post-synaptic activities.
 * This implementation supports both rate-based and spike-based Hebbian learning
 * with configurable learning rate and weight bounds.
 * 
 * Hebbian learning is fundamental for developing pattern associations,
 * memory formation, and competitive dynamics in neural networks. This
 * implementation is a placeholder awaiting full development in Phase 2.
 * 
 * @see PlasticityRule
 * @see Hebbian
 * @see AntiHebbianRule
 * @see STDP
 * @see PlasticityFlags
 * @see TODO Implement full Hebbian learning (Phase 2)
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * Default constructor initializes Hebbian rule with biologically plausible defaults.
     */
    HebbianRule();
    
    /**
     * Destructor ensures proper cleanup of PImpl implementation.
     */
    ~HebbianRule() override;
    
    /**
     * @name Pure Virtual Method Implementation
     * 
     * These methods provide the concrete implementation of the PlasticityRule interface.
     */
    /// @{
    
    /**
     * @copydoc PlasticityRule::update()
     * 
     * Implements Hebbian update rule based on synaptic activities.
     * The weight change is proportional to the product of pre- and post-synaptic
     * activities (or correlation of spike timing).
     * 
     * @param synapse Pointer to the synapse being modified
     * @param preSpikes Vector of timestamps of pre-synaptic spikes
     * @param postSpikes Vector of timestamps of post-synaptic spikes
     * @param dt Simulation timestep duration
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @copydoc PlasticityRule::applyWeightChange()
     * 
     * Applies the computed Hebbian weight change to the synapse.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change to apply (positive for potentiation)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @copydoc PlasticityRule::getName()
     */
    const char* getName() const override;
    /// @}
    
    /**
     * @name Parameter Configuration
     * 
     * Methods for configuring Hebbian learning parameters.
     * These parameters control the strength and bounds of Hebbian learning.
     */
    /// @{
    
    /**
     * @brief Set Hebbian learning rate
     * 
     * @param rate Learning rate (typically 0.0 to 1.0)
     * @see getLearningRate()
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set maximum synaptic weight for Hebbian learning
     * 
     * @param maxWeight Maximum weight value (typically positive)
     * @see getMaxWeight()
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get current maximum synaptic weight
     * 
     * @return float Current maximum weight
     */
    float getMaxWeight() const;
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * The HebbianRule integrates with multiple plasticity mechanisms.
     */
    /// @{
    
    /**
     * @see PlasticityRule for base class interface
     * @see STDP for spike-timing dependent plasticity
     * @see AntiHebbianRule for anti-Hebbian learning
     * @see RewardModulatedSTDP for reward-modulated variant
     * @see PlasticityFlags for enabling/disabling
     * @see TODO Implement full Hebbian learning (Phase 2)
     */
    /// @}
    
private:
    /**
     * @name Implementation Details
     * 
     * PImpl idiom for efficient implementation changes.
     */
    /// @{
    
    struct Impl;
    Impl* pImpl;  /**< Private implementation pointer */
    /// @}
};

} // namespace nlm
