#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class PlasticityRule
 * @brief Abstract base class for synaptic plasticity rules
 * 
 * This class defines the interface for all synaptic plasticity mechanisms,
 * including Hebbian learning, STDP, and homeostatic plasticity.
 * 
 * Plasticity rules are responsible for updating synaptic weights based on
 * pre-synaptic and post-synaptic neural activity.
 */
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * @param synapse The synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply a direct weight change to a synapse
     * 
     * @param synapse The synapse to update
     * @param delta The weight change to apply
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the name of the plasticity rule
     * 
     * @return String identifier for the rule type
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if the plasticity rule is enabled
     * 
     * @return True if the rule is active
     */
    virtual bool isEnabled() const = 0;
    
    /**
     * @brief Enable or disable the plasticity rule
     * 
     * @param enabled Whether the rule should be active
     */
    virtual void setEnabled(bool enabled) = 0;
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

/**
 * @class HebbianRule
 * @brief Implements Hebbian plasticity: "neurons that fire together, wire together"
 * 
 * This rule strengthens synapses when pre- and post-synaptic neurons
 * fire in close temporal proximity, implementing the classical Hebbian
 * learning rule.
 */
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
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

/**
 * @class AntiHebbianRule
 * @brief Implements anti-Hebbian plasticity for stability
 * 
 * This rule weakens synapses when pre- and post-synaptic neurons
 * fire together, promoting homeostasis and preventing runaway excitation.
 */
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate for anti-Hebbian updates
     * 
     * @param rate Learning rate (typically 0.001-0.01)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class BCMRule
 * @brief Implements Bienenstock-Cooper-Munro (BCM) theory
 * 
 * This rule implements synaptic modification based on a sliding threshold
 * that adapts according to the history of post-synaptic activity,
 * balancing potentiation and depression.
 */
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate for BCM updates
     * 
     * @param rate Learning rate (typically 0.0001-0.001)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
