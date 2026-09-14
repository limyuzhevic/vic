#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

class STDP : public PlasticityRule {
public:
    /**
     * @brief Create STDP rule with default parameters
     */
    STDP();
    
    /**
     * @brief Destroy STDP rule
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weight based on spike timing differences
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply direct weight change to synapse
     * @param synapse Synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get name of plasticity rule
     * @return const char* "STDP"
     */
    const char* getName() const override;
    
    /**
     * @brief Set long-term potentiation (LTP) weight
     * @param weight LTP weight (typically ~0.01)
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * @brief Get long-term potentiation weight
     * @return float LTP weight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term depression (LTD) weight
     * @param weight LTD weight (typically ~0.012)
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * @brief Get long-term depression weight
     * @return float LTD weight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set STDP time constant
     * @param tau Time constant in milliseconds
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * @brief Get STDP time constant
     * @return float STDP time constant in ms
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau Time constant
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// PLACEHOLDER - Phase 2
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @brief Update synaptic weight with reward modulation
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply direct weight change with reward modulation
     * @param synapse Synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get name of plasticity rule
     * @return const char* "R-STDP"
     */
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @brief Update synaptic weight with triplet interactions
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    /**
     * @brief Apply direct weight change with triplet interactions
     * @param synapse Synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    /**
     * @brief Get name of plasticity rule
     * @return const char* "TripletSTDP"
     */
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
