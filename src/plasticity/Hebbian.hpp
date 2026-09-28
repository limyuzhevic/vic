#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>
#include <string>

namespace nlm {

/**
 * @class Hebbian
 * @brief Real Hebbian learning implementation for spiking neural networks
 * 
 * This class implements a biologically plausible Hebbian plasticity rule based on
 * the covariance rule: Δw = η · (⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
 * 
 * Key features:
 * - Uses spike timing-based covariance calculation
 * - Includes synaptic normalization to prevent runaway potentiation
 * - Supports both symmetric and asymmetric Hebbian learning
 * - Includes recent spike history for temporal credit assignment
 * 
 * Biological inspiration:
 * - Reflects AMPA receptor trafficking at synapses
 * - Implements "neurons that fire together, wire together" with stability constraints
 * - Based on experimental findings in hippocampal and cortical networks
 * 
 * @note This is Phase 6 implementation - fully functional for real neural computation
 */
class Hebbian : public PlasticityRule {
public:
    /**
     * @brief Construct a new Hebbian learning instance
     * 
     * @param learningRate Base learning rate for Hebbian updates (default: 0.01)
     * @param maxWeight Maximum synaptic weight (default: 1.0)
     * @param minWeight Minimum synaptic weight (default: -1.0)
     * @param normalizationEnabled Enable synaptic normalization (default: true)
     */
    explicit Hebbian(float learningRate = 0.01f, 
                    float maxWeight = 1.0f, 
                    float minWeight = -1.0f,
                    bool normalizationEnabled = true);
    
    ~Hebbian() override;
    
    /**
     * @brief Update synaptic weight based on spike timing correlations
     * 
     * @param synapse Synapse to update
     * @param preSpikes Timestamps of pre-synaptic spikes
     * @param postSpikes Timestamps of post-synaptic spikes
     * @param dt Simulation timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change with bounds checking
     * 
     * @param synapse Synapse to update
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Rule name
     */
    const char* getName() const override;
    
    /**
     * @brief Set learning rate for Hebbian updates
     * 
     * @param rate Learning rate (0.0 to 1.0)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set maximum synaptic weight
     * 
     * @param maxWeight Maximum weight limit
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get maximum synaptic weight
     * 
     * @return float Maximum weight
     */
    float getMaxWeight() const;
    
    /**
     * @brief Enable or disable synaptic normalization
     * 
     * @param enabled Enable normalization
     */
    void setNormalizationEnabled(bool enabled);
    
    /**
     * @brief Check if synaptic normalization is enabled
     * 
     * @return bool Normalization enabled status
     */
    bool isNormalizationEnabled() const;
    
    /**
     * @brief Set symmetric Hebbian mode
     * 
     * @param symmetric If true, depression occurs for anti-correlated activity
     */
    void setSymmetric(bool symmetric);
    
    /**
     * @brief Check if symmetric Hebbian mode is enabled
     * 
     * @return bool Symmetric mode enabled
     */
    bool isSymmetric() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
