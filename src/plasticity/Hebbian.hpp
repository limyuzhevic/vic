#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class Hebbian
 * @brief Hebbian learning rule implementing covariance-based plasticity
 * @details Implements Hebbian learning through covariance computation,
 * modifying synaptic strength based on statistical relationships between
 * pre- and post-synaptic activity patterns.
 * 
 * The covariance rule extends basic Hebbian learning by subtracting the
 * mean activity contributions, implementing the formal covariance algorithm:
 * 
 * Δw_ij = η * (⟨x_i y_j⟩ - ⟨x_i⟩⟨y_j⟩)
 * 
 * Where:
 * - x_i = pre-synaptic activity (0 or 1 spike)
 * - y_j = post-synaptic activity (0 or 1 spike)  
 * - ⟨·⟩ = average over time window
 * - η = learning rate (η > 0)
 * 
 * Design Rationale:
 * - Pimpl idiom provides implementation flexibility and binary compatibility
 * - Event-driven architecture enables efficient handling of sparse neural activity
 * - Modular plasticity system supports concurrent learning mechanisms
 * 
 * Biological Inspiration:
 * - Reflects synaptic weight changes driven by correlated activity patterns
 * - Implements covariance rule observed in cortical and hippocampal synapses
 * - Matches temporal correlation learning in sensory processing pathways
 * 
 * References:
 * - Oja, E. (1982): Simplified neuron model as principal component analyzer
 * - Sanger, T.P. (1989): Neural network learning algorithm
 * - Cowan & Moss (1997): Synapses with correlated activity
 */

class Hebbian : public PlasticityRule {
public:
    /**
     * @brief Construct Hebbian plasticity rule with default parameters
     * @details Default parameters:
     *   - Learning rate η: 0.01 (sensitivity to activity correlations)
     *   - Maximum weight: 1.0 (synaptic strength upper bound)
     *   - Minimum weight: -1.0 (synaptic strength lower bound)
     *   - Covariance threshold: 0.0 (baseline activity correction)
     */
    Hebbian();
    
    /**
     * @brief Destroy Hebbian plasticity rule
     */
    ~Hebbian() override;
    
    /**
     * @brief Update synaptic weights based on activity covariance
     * @param synapse Pointer to synapse to modify
     * @param preSpikes Timestamps of pre-synaptic neuron spikes
     * @param postSpikes Timestamps of post-synaptic neuron spikes
     * @param dt Simulation timestep duration
     * 
     * @details Covariance Rule Implementation:
     * 
     * The covariance rule captures the formal mathematical definition:
     * 
     * **1. Algorithm Derivation**:
     * Hebbian learning alone would be: Δw ∝ ⟨x_i y_j⟩
     * This causes runaway excitation because neurons with positive mean
     * activity always potentiate.
     * 
     * **Covariance correction**:
     * Δw_ij = η * (⟨x_i y_j⟩ - ⟨x_i⟩⟨y_j⟩)
     * 
     * **2. Biological Interpretation**:
     * - ⟨x_i y_j⟩: Co-activity (neurons fire together)
     * - ⟨x_i⟩⟨y_j⟩: Independent activity (neurons fire independently)
     * - The subtraction implements normalization against baseline activity
     * 
     * **3. Implementation Details**:
     * 
     * **Simplified Covariance Algorithm**:
     *   - Count correlated spike pairs within time window
     *   - Compute baseline as learningRate × mean activity
     *   - Δw = learningRate × (correlationCount - baseline)
     * 
     * **4. Biological Inspiration**:
     * - Reflects AMPA receptor trafficking dynamics
     * - Matches Hebbian plasticity at excitatory synapses
     * - Implements covariance rule in visual cortex processing
     * - Observed in synaptic weight normalization mechanisms
     * 
     * **5. Implementation Notes**:
     * - Uses simplified correlation counting for efficiency
     * - Time window (100ms) captures short-term plasticity correlations
     * - Weight clamping prevents runaway excitation
     * - Learning rate controls stability vs. learning speed trade-off
     * 
     * **6. Performance Considerations**:
     * - O(n²) complexity for pairwise spike correlation counting
     * - Vectorized implementation for parallel correlation computation
     * - Spike history compression for long-running simulations
     * - Adaptive learning rate for stability maintenance
     * 
     * **7. Limitations**:
     * - Assumes stationary statistics (may not capture non-stationary dynamics)
     * - Single learning rate (doesn't capture separate potentiation/depression)
     * - Doesn't account for eligibility traces for delayed reward signals
     * - Simplified baseline computation ignores higher-order statistics
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply direct weight change to synapse
     * @param synapse Pointer to synapse to modify
     * @param delta Weight change value (can be positive or negative)
     * 
     * @details Used for non-covariance-based plasticity or when external
     * learning signals override Hebbian learning.
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name identifier
     * @return String describing this plasticity rule
     */
    const char* getName() const override;
    
    /**
     * @brief Set learning rate (η)
     * @param rate Learning rate (typically 0.001-0.1)
     * 
     * @details Controls the sensitivity to activity correlations:
     * - High learning rate: Fast learning, potential instability
     * - Low learning rate: Stable learning, requires longer training
     * - Optimal rate balances speed and stability
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * @return Current learning rate value
     */
    float getLearningRate() const;
    
    /**
     * @brief Set maximum synaptic weight
     * @param maxWeight Upper bound for synaptic strength
     * 
     * @details Prevents runaway excitation during learning.
     * Values typically between 0.5 and 10.0 depending on neuron model.
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get maximum synaptic weight
     * @return Current maximum weight value
     */
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
