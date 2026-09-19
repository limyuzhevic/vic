#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// Implements biologically plausible learning rule where synaptic
// strengthening depends on precise timing of pre- and post-synaptic spikes
// "/neurons that fire together, wire together" with timing precision

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    /**
     * Update synaptic weights based on spike timing relationships
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike times (ms)
     * @param postSpikes Vector of post-synaptic spike times (ms)
     * @param dt Simulation timestep (ms)
     * 
     * @details Implements STDP rule: pre-before-post = potentiation,
     *          post-before-pre = depression. Uses exponential window
     *          function with O(n) sliding window algorithm for efficiency.
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
     * @return Name string ("STDP")
     */
    const char* getName() const override;
    
    /**
     * Set the Long-Term Potentiation (LTP) weight (A+)
     * 
     * @param weight LTP weight, typically 0.01-0.02
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * Get the current LTP weight
     * 
     * @return LTP weight value
     */
    float getLTPWeight() const;
    
    /**
     * Set the Long-Term Depression (LTD) weight (A-)
     * 
     * @param weight LTD weight, typically 0.012-0.015
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * Get the current LTD weight
     * 
     * @return LTD weight value
     */
    float getLTDWeight() const;
    
    /**
     * Set the STDP time constant (tau)
     * 
     * @param tau Time constant in milliseconds (typical: 20-30ms)
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * Get the current STDP time constant
     * 
     * @return Time constant in milliseconds
     */
    float getTimeConstant() const;
    
    /**
     * Configure STDP parameters from configuration values
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau STDP time constant
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
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
