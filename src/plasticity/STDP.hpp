#pragma once

#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity (STDP) implementation
 * 
 * This class implements Spike-Timing-Dependent Plasticity (STDP), a powerful
 * learning mechanism where synaptic strength is adjusted based on the relative
 * timing of pre- and post-synaptic spikes. STDP is biologically plausible and
 * enables temporal credit assignment in neural networks.
 * 
 * The core STDP principle:
 * - If a post-synaptic spike occurs after a pre-synaptic spike: Long-Term Potentiation (LTP)
 * - If a post-synaptic spike occurs before a pre-synaptic spike: Long-Term Depression (LTD)
 * - The magnitude of change depends on the time difference between spikes
 * 
 * STDP is event-driven and maintains synaptic balance through asymmetric weight changes,
 * making it ideal for online learning in spiking neural networks.
 * 
 * @note This implementation uses exponential decay for spike timing windows
 * @see Hebbian for activity-dependent learning
 * @see BCM for threshold-based plasticity
 */

class STDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new STDP object
     * 
     * Initializes STDP with default parameters and internal implementation.
     */
    STDP();
    
    /**
     * @brief Destroy the STDP object
     * 
     * Cleans up internal STDP implementation and resources.
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weights based on spike timing
     * 
     * Implements STDP learning rule by analyzing the timing relationship between
     * pre- and post-synaptic spikes and adjusting synaptic weights accordingly.
     * 
     * @param synapse Pointer to synapse to be updated
     * @param preSpikes Vector of pre-synaptic spike timestamps (in seconds)
     * @param postSpikes Vector of post-synaptic spike timestamps (in seconds)
     * @param dt Simulation timestep duration (in seconds)
     * 
     * @note Uses exponential decay windows: weight change ∝ exp(-|Δt|/τ)
     *       where τ is the time constant and Δt is spike time difference
     * @see setTimeConstant() for configuring time constant
     */
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change with STDP constraints
     * 
     * Applies the calculated STDP weight change to the synapse with proper
     * bounds checking and normalization.
     * 
     * @param synapse Pointer to synapse to modify
     * @param delta Calculated weight change (can be positive for LTP or negative for LTD)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return String identifier for STDP rule
     * 
     * @note Returns "STDP" for identification in logging and debugging
     */
    const char* getName() const override;
    
    /**
     * @brief Set long-term potentiation (LTP) weight
     * 
     * Configures the maximum weight increase for pre→post spike ordering (LTP).
     * 
     * @param weight LTP weight (must be non-negative)
     * @see setLTDWeight() for LTD configuration
     * @see setTimeConstant() for timing configuration
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight
    
    /**
     * @brief Get current LTP weight
     * 
     * @return Current long-term potentiation weight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term depression (LTD) weight
     * 
     * Configures the maximum weight decrease for post→pre spike ordering (LTD).
     * 
     * @param weight LTD weight (must be non-negative)
     * @see setLTPWeight() for LTP configuration
     */
    void setLTDWeight(float weight);    // Long-term depression weight
    
    /**
     * @brief Get current LTD weight
     * 
     * @return Current long-term depression weight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set STDP time constant
     * 
     * Configures the exponential decay time constant for spike timing windows.
     * Larger values make the learning window wider in time.
     * 
     * @param tau Time constant in milliseconds (must be positive)
     * @see getTimeConstant() for current value
     * @see update() for usage in learning calculations
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)
    
    /**
     * @brief Get current time constant
     * 
     * @return Current time constant in milliseconds
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration
     * 
     * Batch configuration method for setting all STDP parameters at once,
     * typically used during brain initialization from configuration file.
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau Time constant in milliseconds
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    /**
     * @brief Internal implementation (PIMPL pattern)
     * 
     * All actual STDP implementation details are hidden in this private struct,
     * including spike history tracking, eligibility traces, and efficient
     * weight update calculations.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * Pimpl pointer for implementation hiding and reduced compilation dependencies.
     */
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
