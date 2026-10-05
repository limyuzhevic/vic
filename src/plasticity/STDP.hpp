#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity implementation
 * 
 * STDP is a biologically-inspired learning rule where synaptic modification
 * depends on the relative timing of pre- and post-synaptic spikes:
 * 
 * - If pre-synaptic spike precedes post-synaptic spike: potentiation (strengthening)
 * - If post-synaptic spike precedes pre-synaptic spike: depression (weakening)
 * 
 * The magnitude follows an exponential decay with time difference:
 *   Δw = A± * exp(-|Δt| / τ)
 * 
 * @note PLACEHOLDER - Phase 2 will implement real STDP with enhanced features:
 * - Triplet interactions (more biologically accurate)
 * - Reward-modulated variants
 * - Neuromodulator integration
 * - Eligibility traces for delayed reward
 * 
 * @see https://en.wikipedia.org/wiki/Spike-timing_dependent_plasticity
 */
class STDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new STDP object with default parameters
     * 
     * Default parameters:
     * - LTP weight (A+): 0.01
     * - LTD weight (A-): 0.012  
     * - Time constant (τ): 20.0ms
     * - Min weight: -1.0
     * - Max weight: 1.0
     */
    STDP();
    
    /**
     * @brief Destroy the STDP object
     * 
     * Virtual destructor for proper cleanup of derived classes.
     */
    ~STDP() override;
    
    // Delete copy operations, allow move operations
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    STDP(STDP&&) noexcept;
    STDP& operator=(STDP&&) noexcept;

    /**
     * @brief Update synaptic weights based on spike timing
     * 
     * @param synapse Target synapse to update
     * @param preSpikes Vector of timestamps of pre-synaptic spikes
     * @param postSpikes Vector of timestamps of post-synaptic spikes  
     * @param dt Simulation timestep duration (ms)
     * 
     * @pre synapse != nullptr
     * @pre dt > 0 (positive timestep)
     * 
     * @note O(n²) complexity for n = |preSpikes| + |postSpikes|
     * @note Optimized in Phase 3 with spike sorting and vectorization
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;

    /**
     * @brief Apply a direct weight change to a synapse
     * 
     * @param synapse Target synapse to modify
     * @param delta Weight change to apply
     * 
     * @pre synapse != nullptr
     * @post Weight is clamped to [minWeight, maxWeight] bounds
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;

    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Rule name: "STDP"
     */
    const char* getName() const override;

    // ============================================================================
    // STDP Parameter Configuration
    // ============================================================================

    /**
     * @brief Set the Long-Term Potentiation (LTP) weight (A+)
     * 
     * Controls the strength of synaptic strengthening when pre fires before post.
     * Typical range: [0.0, 1.0], default: 0.01
     * 
     * @param weight LTP weight value
     */
    void setLTPWeight(float weight);    // Long-term potentiation weight

    /**
     * @brief Get the current LTP weight
     * 
     * @return float Current LTP weight value
     */
    float getLTPWeight() const;

    /**
     * @brief Set the Long-Term Depression (LTD) weight (A-)
     * 
     * Controls the strength of synaptic weakening when post fires before pre.
     * Typical range: [0.0, 1.0], default: 0.012
     * 
     * @param weight LTD weight value
     */
    void setLTDWeight(float weight);    // Long-term depression weight

    /**
     * @brief Get the current LTD weight
     * 
     * @return float Current LTD weight value
     */
    float getLTDWeight() const;

    /**
     * @brief Set the STDP time constant (τ)
     * 
     * Controls the width of the timing window for spike pairing.
     * Larger values = longer effective window, typical: [1.0, 100.0]ms, default: 20.0
     * 
     * @param tau Time constant in milliseconds
     */
    void setTimeConstant(float tau);    // STDP time constant (ms)

    /**
     * @brief Get the current time constant
     * 
     * @return float Current time constant in milliseconds
     */
    float getTimeConstant() const;

    /**
     * @brief Configure STDP parameters from configuration file
     * 
     * Convenience method to set all parameters at once.
     * 
     * @param ltpWeight LTP weight value
     * @param ltdWeight LTD weight value
     * @param tau Time constant in milliseconds
     */
    void configure(float ltpWeight, float ltdWeight, float tau);

    // ============================================================================
    // Derived Class Support Methods
    // ============================================================================

    /**
     * @brief Get the current synaptic weight bounds
     * 
     * @return std::pair<float, float> Minimum and maximum allowed weights
     */
    std::pair<float, float> getWeightBounds() const;

    /**
     * @brief Check if current parameters are within valid ranges
     * 
     * @return true Parameters are valid, false otherwise
     */
    bool isValid() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class RewardModulatedSTDP
 * @brief STDP with reward modulation (R-STDP)
 * 
 * Enhanced STDP that incorporates reward signals to modulate learning rates.
 * When reward is received, learning is enhanced for recently active synapses.
 * 
 * @note PLACEHOLDER - Phase 2 implementation planned
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override = default;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;

    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;

    const char* getName() const override { return "R-STDP"; }

    // Reward modulation methods
    void signalReward(float reward);
    void signalError(float error);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class TripletSTDP
 * @brief Advanced STDP with triplet spike interactions
 * 
 * More biologically accurate STDP that includes triplet spike interactions,
 * capturing the effects of three spikes instead of just pairs. This better
 * reflects the complexity of real synaptic plasticity mechanisms.
 * 
 * @note PLACEHOLDER - Phase 2 implementation planned
 */
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override = default;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;

    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;

    const char* getName() const override { return "TripletSTDP"; }

    // Triplet interaction configuration
    void setTripletWeights(float tripletLTP, float tripletLTD);
    void setTraceTimeConstants(float preTraceTau, float postTraceTau);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
