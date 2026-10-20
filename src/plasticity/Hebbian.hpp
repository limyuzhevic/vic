#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class Hebbian
 * @brief Hebbian learning implementation for association formation
 * @details Hebbian learning implements the biological principle "neurons that fire together, wire together."
 * This class provides activity-dependent synaptic strengthening based on correlated neural activity,
 * forming the basis for association learning and pattern completion in neural networks.
 * 
 * Hebbian Mechanism:
 * - Correlated firing: Co-active neurons strengthen connections
 * - Weight potentiation: Synaptic strength increases with coincidence detection
 * - Unbounded growth: Without normalization, synapses can saturate
 * - Pattern learning: Forms distributed representations
 * 
 * Learning Dynamics:
 * - Symmetric updates: Both directions strengthen
 * - Rate-based: Learning depends on firing rates
 * - Competitive: Limited total synaptic resources
 * - Sparse: Learning focused on active neurons
 * 
 * Implementation Features:
 * - Learning rate parameter for control over plasticity speed
 * - Maximum weight limit for stability
 * - Activity-dependent gating
 * - Homosynaptic regulation
 * 
 * Thread Safety: Not thread-safe for concurrent synaptic updates. Hebbian updates
 * should be synchronized with neural simulation.
 * 
 * Performance: O(n) for update where n is number of active neurons.
 * Optimized for sparse neural activity.
 * 
 * Biological Plausibility:
 * - Implements classic Hebbian learning rule
 * - Rate-based learning matches biological observations
 * - Supports competitive learning dynamics
 * - Compatible with spike timing dependence
 * 
 * @code
 * // Initialize Hebbian learning
 * Hebbian hebbian;
 * 
 * // Configure learning parameters
 * hebbian.setLearningRate(0.01f);   // Learning rate (1% per coincidence)
 * hebbian.setMaxWeight(1.0f);       // Maximum synaptic weight
 * 
 * // Update based on neural activity
 * // This happens during simulation steps when neurons are co-active
 * vector<Timestamp> preSpikes = {0.1, 0.3, 0.7};
 * vector<Timestamp> postSpikes = {0.15, 0.35, 0.75};
 * hebbian.update(synapse, preSpikes, postSpikes, dt);
 * 
 * // Apply resulting weight change
 * hebbian.applyWeightChange(synapse, delta);
 * 
 * // Learning trajectory
 * // Repeated co-activation gradually strengthens synapses
 * // Approaches max weight asymptotically
 * @endcode
 * 
 * Learning Dynamics:
 * - Hebbian learning is fundamental for pattern formation
 * - Often combined with STDP for more precise timing-based learning
 * - Requires homeostatic mechanisms to maintain stability
 * - Works best with sparse coding and competition
 * 
 * References:
 * - Hebb, D.O. (1949). The Organization of Behavior. Wiley.
 * - Rumelhart, D.E. et al. (1986). Learning representations by back-propagating errors.
 *   Nature.
 * 
 * @see PlasticityRule
 * @see STDP
 * @see Synapse
 * @see Brain
 */
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
