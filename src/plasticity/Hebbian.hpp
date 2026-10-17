#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @brief Hebbian learning implementation
 * 
 * Hebbian plasticity implements the fundamental learning rule "neurons that fire
 * together, wire together". This correlation-based learning rule strengthens
 * synaptic connections when pre- and post-synaptic neurons are active together,
 * forming the basis for many forms of associative learning and pattern recognition.
 * 
 * @details The Hebbian rule is biologically inspired and computationally simple,
 * yet powerful for learning statistical regularities in neural activity. While
 * not as temporally precise as STDP, it's effective for non-temporal correlation
 * learning and forms the foundation for many cognitive processes including:
 * - Pattern completion and recall
 * - Memory consolidation
 * - Concept formation
 * - Predictive coding
 * 
 * @note In biological systems, Hebbian learning is often modulated by
 * neuromodulators and constrained by other mechanisms to prevent runaway
 * excitation while maintaining useful learning capabilities.
 */
class Hebbian : public PlasticityRule {
public:
    /**
     * @brief Constructor - Creates Hebbian rule with default parameters
     * 
     * @details Initializes with biologically plausible defaults:
     * - learningRate: 0.01 (typical Hebbian learning rate)
     * - maxWeight: 1.0 (maximum synaptic strength)
     * 
     * @note These defaults can be adjusted based on specific application
     * requirements and neural system characteristics.
     */
    Hebbian();
    
    /**
     * @brief Destructor
     */
    ~Hebbian() override;
    
    /**
     * @brief Copy constructor - Deleted (Hebbian is not copyable)
     */
    Hebbian(const Hebbian&) = delete;
    
    /**
     * @brief Copy assignment operator - Deleted (Hebbian is not copyable)
     */
    Hebbian& operator=(const Hebbian&) = delete;
    
    /**
     * @brief Move constructor - Enabled for efficient transfer
     */
    Hebbian(Hebbian&&) noexcept;
    
    /**
     * @brief Move assignment operator - Enabled for efficient transfer
     */
    Hebbian& operator=(Hebbian&&) noexcept;
    
    /**
     * @brief Update synaptic weights based on spike correlation
     * 
     * @param synapse Pointer to the synapse being updated
     * @param preSpikes Vector of timestamps for pre-synaptic spikes
     * @param postSpikes Vector of timestamps for post-synaptic spikes
     * @param dt Simulation timestep duration (ms)
     * 
     * @details Computes Hebbian weight changes based on spike correlation:
     * - When both pre and post neurons fire, connection strengthens
     * - When one fires but not the other, no change (unlike STDP)
     * - Simpler than STDP but effective for pattern association
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply computed weight change to synapse
     * 
     * @param synapse Target synapse for weight modification
     * @param delta Weight change amount to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name identifier
     * 
     * @return const char* String identifier "Hebbian" used for logging
     */
    const char* getName() const override;
    
    /**
     * @brief Set Hebbian learning rate parameter
     * 
     * @param rate Learning rate for synaptic modification (default: 0.01)
     * 
     * @details Learning rate controls the magnitude of weight changes per spike
     * pair. Higher values make learning faster but may cause instability,
     * while lower values make learning slower but more stable.
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current Hebbian learning rate
     * 
     * @return float Current learning rate parameter
     */
    float getLearningRate() const;
    
    /**
     * @brief Set maximum synaptic weight limit
     * 
     * @param maxWeight Maximum allowed synaptic weight (default: 1.0)
     * 
     * @details Constrains the maximum synaptic strength to prevent runaway
     * excitation and maintain system stability. Typically positive for excitatory
     * synapses and negative or small for inhibitory synapses.
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get current maximum synaptic weight
     * 
     * @return float Current maximum weight parameter
     */
    float getMaxWeight() const;
    
    /**
     * @brief Get mathematical formulation of Hebbian rule
     * 
     * @return std::string Detailed explanation of Hebbian equations
     */
    std::string getMathematicalFormulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
