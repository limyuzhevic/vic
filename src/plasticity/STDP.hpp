#pragma once

/**
 * @file STDP.hpp
 * @brief Spike-Timing-Dependent Plasticity (STDP) implementations.
 * 
 * This header defines STDP implementations for the NLM neural network.
 * STDP is a fundamental plasticity mechanism where the timing of pre- and
 * post-synaptic spikes determines whether a synapse strengthens (LTP) or
 * weakens (LTD). This temporal learning rule is crucial for developing
 * coordinated activity patterns and learning associations.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Types/Types.hpp
 * @see src/brain/Neuron.hpp
 * @see src/plasticity/PlasticityRule.hpp
 * @see src/plasticity/Hebbian.hpp
 * @see src/plasticity/StructuralPlasticity.hpp
 * @see TODO Complete STDP implementations (Phase 2)
 */

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * Classic Spike-Timing-Dependent Plasticity (STDP) rule.
 * 
 * Implements the standard STDP mechanism where:
 * - Pre-synaptic spike before post-synaptic spike → LTP (strengthening)
 * - Post-synaptic spike before pre-synaptic spike → LTD (weakening)
 * The effect decays exponentially with time difference between spikes.
 * 
 * STDP is essential for developing temporal associations and competitive
 * dynamics in neural networks. This implementation is a placeholder awaiting
 * full development in Phase 2.
 * 
 * @see PlasticityRule
 * @see RewardModulatedSTDP
 * @see TripletSTDP
 * @see HebbianRule
 * @see PlasticityFlags
 * @see TODO Implement full STDP (Phase 2)
 */
class STDP : public PlasticityRule {
public:
    /**
     * Default constructor initializes STDP with biologically plausible defaults.
     */
    STDP();
    
    /**
     * Destructor ensures proper cleanup of PImpl implementation.
     */
    ~STDP() override;
    
    /**
     * @name Pure Virtual Method Implementation
     * 
     * These methods provide the concrete implementation of the PlasticityRule interface.
     */
    /// @{
    
    /**
     * @copydoc PlasticityRule::update()
     * 
     * Implements STDP update rule based on spike timing differences.
     * Computes weight change based on time differences between pre- and
     * post-synaptic spikes using exponential decay functions.
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
     * Applies the computed STDP weight change to the synapse.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change to apply (LTP or LTD)
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
     * Methods for configuring STDP learning parameters.
     * These parameters control the strength and timing of STDP effects.
     */
    /// @{
    
    /**
     * @brief Set long-term potentiation weight
     * 
     * @param weight LTP weight (typically positive, 0.0 to 1.0)
     * @see getLTPWeight()
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * @brief Get current LTP weight
     * 
     * @return float Current LTP weight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term depression weight
     * 
     * @param weight LTD weight (typically negative, -1.0 to 0.0)
     * @see getLTDWeight()
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * @brief Get current LTD weight
     * 
     * @return float Current LTD weight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set STDP time constant
     * 
     * @param tau Time constant in milliseconds (typically 10-100 ms)
     * @see getTimeConstant()
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * @brief Get current time constant
     * 
     * @return float Current time constant in milliseconds
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration
     * 
     * @param ltpWeight LTP weight parameter
     * @param ltdWeight LTD weight parameter
     * @param tau Time constant parameter
     * @see configureFromConfig()
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * STDP integrates with multiple neural network components.
     */
    /// @{
    
    /**
     * @see PlasticityRule for base class interface
     * @see HebbianRule for complementary Hebbian learning
     * @see RewardModulatedSTDP for reward-modulated variant
     * @see TripletSTDP for spike triplet interactions
     * @see PlasticityFlags for enabling/disabling
     * @see TODO Implement full STDP (Phase 2)
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

/**
 * Reward-modulated STDP (R-STDP).
 * 
 * Extends STDP with neuromodulatory signal (typically dopamine) that gates
 * plasticity based on reward prediction error. This variant links learning
 * directly to reward signals, enabling reward-based learning and exploration/exploitation
 * trade-offs.
 * 
 * @see PlasticityRule
 * @see STDP for base STDP implementation
 * @see TODO PHASE 2: Complete implementation
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @copydoc PlasticityRule::update()
     * 
     * Empty implementation (placeholder).
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    
    /**
     * @copydoc PlasticityRule::applyWeightChange()
     * 
     * Empty implementation (placeholder).
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @copydoc PlasticityRule::getName()
     */
    const char* getName() const override { return "R-STDP"; }
};

/**
 * Triplet STDP with spike interaction effects.
 * 
 * Extends STDP to include triplet-based plasticity where pairs of spikes
 * interact with a third spike to produce more complex learning rules.
 * This model can capture more nuanced timing-dependent plasticity effects.
 * 
 * @see PlasticityRule
 * @see STDP for simpler STDP implementation
 * @see TODO PHASE 2: Complete implementation
 */
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @copydoc PlasticityRule::update()
     * 
     * Empty implementation (placeholder).
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    
    /**
     * @copydoc PlasticityRule::applyWeightChange()
     * 
     * Empty implementation (placeholder).
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @copydoc PlasticityRule::getName()
     */
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
