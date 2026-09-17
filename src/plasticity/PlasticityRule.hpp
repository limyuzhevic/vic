#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on pre/post synaptic activity
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike times
     * @param postSpikes Vector of post-synaptic spike times
     * @param dt Timestep duration for the update
     */n    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply a weight change to a synapse
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (positive for potentiation, negative for depression)
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return Human-readable name of the plasticity rule
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if the plasticity rule is enabled
     * 
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable the plasticity rule
     * 
     * @param enabled New enabled state
     */
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    /**
     * @brief Whether this plasticity rule is currently active
     */
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    /**
     * @brief Update synaptic weights using Hebbian learning
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike times
     * @param postSpikes Vector of post-synaptic spike times
     * @param dt Timestep duration for the update
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply a weight change directly to a synapse
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the rule name
     * 
     * @return "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Set the learning rate for this rule
     * 
     * @param rate Learning rate (typically 0.01-0.1)
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the learning rate
     * 
     * @return Current learning rate
     */
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Update synaptic weights using Anti-Hebbian learning
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike times
     * @param postSpikes Vector of post-synaptic spike times
     * @param dt Timestep duration for the update
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply a weight change (always opposite sign of input)
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (will be negated)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get the rule name
     * 
     * @return "AntiHebbian"
     */
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Update synaptic weights using BCM learning rule
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike times
     * @param postSpikes Vector of post-synaptic spike times
     * @param dt Timestep duration for the update
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply a weight change according to BCM theory
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (theta-dependent)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get the rule name
     * 
     * @return "BCM"
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
