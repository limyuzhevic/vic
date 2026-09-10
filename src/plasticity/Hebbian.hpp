#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class Hebbian
 * @brief Implements Hebbian plasticity: "neurons that fire together, wire together"
 * 
 * This rule strengthens synapses when pre- and post-synaptic neurons
 * fire in close temporal proximity, implementing the classical Hebbian
 * learning rule.
 * 
 * This is a covariance-based learning rule where the weight change is proportional
 * to the correlation between pre- and post-synaptic activity.
 */
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * @param synapse The synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply a direct weight change to a synapse
     * 
     * @param synapse The synapse to update
     * @param delta The weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of the plasticity rule
     * 
     * @return String identifier for the rule type
     */
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate for Hebbian updates
     * 
     * @param rate Learning rate (typically 0.01-0.05)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set the maximum synaptic weight
     * 
     * @param maxWeight Maximum allowed weight
     */
    void setMaxWeight(float maxWeight);
    
    /**
     * @brief Get the maximum synaptic weight
     * 
     * @return Maximum weight
     */
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
