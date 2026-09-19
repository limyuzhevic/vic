#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
// Implements the classic Hebbian learning rule "neurons that fire together, wire together"
// Uses covariance-based learning for spike timing correlation

class Hebbian : public PlasticityRule {
public:
    /**
     * Construct a Hebbian plasticity rule instance
     */
    Hebbian();
    
    /**
     * Destroy the Hebbian plasticity rule instance
     */
    ~Hebbian() override;
    
    /**
     * Update synaptic weights based on spike correlation
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike times (ms)
     * @param postSpikes Vector of post-synaptic spike times (ms)
     * @param dt Simulation timestep (ms)
     * 
     * @details Implements covariance learning: Δw = η × (⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
     *          Uses sliding window correlation detection for efficiency.
     *          Affects strengthening of connections between co-active neurons.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * Apply a weight change to a synapse with bounds checking
     * 
     * @param synapse Synapse to update
     * @param delta Weight change (positive = strengthen, negative = weaken)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * Get the name of this plasticity rule
     * 
     * @return Name string ("Hebbian")
     */
    const char* getName() const override;
    
    /**
     * Set the learning rate for Hebbian plasticity
     * 
     * @param rate Learning rate, typically 0.01-0.05
     */
    void setLearningRate(float rate);
    
    /**
     * Get the current learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
    /**
     * Set the maximum synaptic weight allowed
     * 
     * @param maxWeight Maximum weight (typically 1.0-2.0)
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * Get the current maximum weight
     * 
     * @return Current maximum weight
     */
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
