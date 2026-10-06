// Copyright (c) 2024+ NLM Project
// All rights reserved.
// 
// This file is part of the Neural Learning Machine (NLM) project.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file STDP.hpp
 * @brief Spike-Timing-Dependent Plasticity implementation
 * 
 * This file implements the STDP class, representing Spike-Timing-Dependent
 * Plasticity (STDP), a fundamental learning mechanism in biological neural
 * networks. STDP is based on the temporal relationship between pre- and
 * post-synaptic spikes, where synaptic strength is modified based on the
 * timing difference between these spikes.
 * 
 * The STDP implementation includes:
 * - Basic STDP with pair-based learning rules
 * - Reward-modulated STDP (R-STDP)
 * - Triplet STDP with multiple spike interactions
 * - Biophysical modeling of synaptic changes
 * - Neuromodulatory integration
 * - STDP trace management
 * - Plasticity constraint mechanisms
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see PlasticityRule.hpp - Base class for plasticity mechanisms
 * @see Hebbian.hpp - Complementary Hebbian plasticity
 * @see Neuromodulator.hpp - Neuromodulatory control of STDP
 * @see Synapse.hpp - Synaptic connections that undergo STDP
 * @see Neuron.hpp - Neurons that generate spikes for STDP
 * @see src/core/Types/Types.hpp - STDP parameter types
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The STDP implementation supports both biological realism and computational efficiency.
 */

#pragma once

// NLM filename guard
#ifndef NLM_STDP_HPP
#define NLM_STDP_HPP

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>
#include <map>
#include <unordered_map>

namespace nlm {

/**
 * @struct STDPParameters
 * @brief Configuration parameters for STDP learning
 * 
 * This structure contains all the parameters required for STDP learning,
 * including weights for potentiation and depression, time constants,
 * and other biophysical parameters that govern synaptic modification.
 * 
 * The parameters are organized to support:
 * - Biophysical modeling of synaptic dynamics
 * - Multiple STDP variants
 * - Neuromodulatory control
 * - Activity-dependent scaling
 * - Competition between different plasticity rules
 * 
 * Parameter Values (typical biological ranges):
 * - ltp_weight: [0.01, 0.1] (long-term potentiation strength)
 * - ltd_weight: [-0.01, -0.001] (long-term depression strength)
 * - tau: [10.0, 100.0] milliseconds (time constant)
 * - learning_rate: [0.001, 0.1] (overall learning rate)
 * - max_weight: [0.5, 2.0] (maximum synaptic weight)
 */
struct STDPParameters {
    float ltp_weight;        ///< Long-term potentiation weight (LTP)
    float ltd_weight;        ///< Long-term depression weight (LTD)
    float tau;               ///< STDP time constant (ms)
    float learning_rate;     ///< Overall learning rate
    float max_weight;        ///< Maximum synaptic weight
    float min_weight;        ///< Minimum synaptic weight
    float normalization;     ///< Weight normalization factor
    float branch_factor;     ///< Branching factor for triplet STDP
    
    /**
     * @brief Default constructor with biologically plausible parameters
     * 
     * Creates STDP parameters with default values:
     * - LTP weight: 0.01
     * - LTD weight: -0.012
     * - Time constant: 20.0 ms
     * - Learning rate: 0.01
     * - Max/min weight: 2.0, 0.1
     */
    STDPParameters()
        : ltp_weight(0.01f)
        , ltd_weight(-0.012f)
        , tau(20.0f)
        , learning_rate(0.01f)
        , max_weight(2.0f)
        , min_weight(0.1f)
        , normalization(1.0f)
        , branch_factor(1.0f) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param ltp LTP weight
     * @param ltd LTD weight
     * @param time_constant Time constant (ms)
     * @param lr Learning rate
     */
    STDPParameters(float ltp, float ltd, float time_constant, float lr)
        : ltp_weight(ltp)
        , ltd_weight(ltd)
        , tau(time_constant)
        , learning_rate(lr)
        , max_weight(2.0f)
        , min_weight(0.1f)
        , normalization(1.0f)
        , branch_factor(1.0f) {}
    
    /**
     * @brief Update parameters from configuration
     * 
     * @param config Parameters to merge
     * @param overwrite If true, overwrite existing parameters (default: true)
     */
    void update(const STDPParameters& config, bool overwrite = true);
    
    /**
     * @brief Validate parameter values
     * 
     * @return bool true if parameters are valid
     */
    bool validate() const;
    
    /**
     * @brief Get parameter description
     * 
     * @param param_name Name of parameter
     * @return std::string Description of the parameter
     */
    std::string getDescription(const std::string& param_name) const;
};

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity implementation
 * 
 * The STDP class implements a comprehensive spike-timing-dependent plasticity
 * mechanism that modifies synaptic strength based on the relative timing of
 * pre- and post-synaptic spikes. This is one of the most well-established
 * learning rules in computational neuroscience.
 * 
 * Key Features:
 * - Pair-based STDP with symmetric learning windows
 * - STDP traces for efficient computation
 * - Neuromodulatory control and scaling
 * - Biophysical plausibility
 * - Support for multiple STDP variants
 * - Efficient spike history management
 * - Thread-safe operation for parallel simulation
 * - Integration with other plasticity rules
 * 
 * The STDP learning rule follows the basic principle:
 * - If pre-synaptic spike occurs BEFORE post-synaptic spike: LTP (synaptic strengthening)
 * - If pre-synaptic spike occurs AFTER post-synaptic spike: LTD (synaptic weakening)
 * 
 * The magnitude of change depends on the time difference between spikes,
 * with larger changes for closer timing and smaller changes for wider timing.
 * 
 * Usage Examples:
 * @code
 * #include "src/plasticity/STDP.hpp"
 * #include "src/brain/Synapse.hpp"
 * 
 * // Create STDP instance
 * STDP stdp;
 * 
 * // Configure STDP parameters
 * stdp.setLTPWeight(0.01f);
 * stdp.setLTDWeight(-0.012f);
 * stdp.setTimeConstant(20.0f);
 * 
 * // Enable STDP for a synapse
 * synapse->enablePlasticity(true, true, false);  // Hebbian + STDP
 * 
 * // Record pre-synaptic spike
 * stdp.recordPreSpike(time1);
 * 
 * // Record post-synaptic spike
 * stdp.recordPostSpike(time2);
 * 
 * // Update STDP based on spike timing
 * float delta = stdp.computeWeightChange();
 * synapse->addToWeight(delta);
 * 
 * // Apply neuromodulatory scaling
 * Neuromodulator neuromodulator;
 * neuromodulator.setLevel(0.8f);  // Reduce learning by 20%
 * float scaled_delta = delta * neuromodulator.getPlasticityFactor();
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in STDP.cpp.
 */
class STDP : public PlasticityRule {
public:
    /**
     * @brief Default constructor with default parameters
     * 
     * Creates an STDP instance with biologically plausible default parameters.
     */
    STDP();
    
    /**
     * @brief Constructor with parameters
     * 
     * @param params STDP parameters
     */
    explicit STDP(const STDPParameters& params);
    
    /**
     * @brief Destructor
     * 
     * Cleans up all allocated resources and ensures proper cleanup of STDP state.
     */
    ~STDP() override;
    
    /**
     * @brief Disable copy construction
     * 
     * STDP instances cannot be copied because they contain:
     * - Dynamic resources requiring unique ownership
     * - Internal state that should not be shared
     * - Spike traces that would be corrupted by sharing
     */
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * Allows efficient transfer of STDP ownership from one instance
     * to another. After move construction, the source STDP is in a
     * valid but unspecified state.
     */
    STDP(STDP&&) noexcept;
    STDP& operator=(STDP&&) noexcept;
    
    /**
     * @brief Update synaptic weights based on spike timing
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Time step duration in seconds
     * 
     * This method implements the core STDP learning rule, computing weight
     * changes based on the temporal relationship between pre- and post-synaptic
     * spikes. It uses efficient STDP trace management for performance.
     * 
     * @note This method should be called once per timestep when synaptic
     *       plasticity is active.
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change directly to a synapse
     * 
     * @param synapse Synapse to update
     * @param delta Weight change to apply
     * 
     * This method applies a computed weight change directly to a synapse,
     * bypassing the spike timing calculation. Useful for:
     * - Direct weight control
     * - Experimental testing
     * - Hybrid learning rules
     * - External control
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current STDP parameters
     * 
     * @return const STDPParameters& Current parameters
     */
    const STDPParameters& getParameters() const;
    
    /**
     * @brief Set STDP parameters
     * 
     * @param params New parameters
     * 
     * @post Parameters are updated
     */
    void setParameters(const STDPParameters& params);
    
    /**
     * @brief Configure STDP parameters from individual values
     * 
     * @param ltp_weight Long-term potentiation weight
     * @param ltd_weight Long-term depression weight
     * @param tau_time_constant STDP time constant (ms)
     */
    void configure(float ltp_weight, float ltd_weight, float tau_time_constant);
    
    /**
     * @brief Get long-term potentiation weight
     * 
     * @return float LTP weight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term potentiation weight
     * 
     * @param weight New LTP weight
     */
    void setLTPWeight(float weight);
    
    /**
     * @brief Get long-term depression weight
     * 
     * @return float LTD weight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set long-term depression weight
     * 
     * @param weight New LTD weight
     */
    void setLTDWeight(float weight);
    
    /**
     * @brief Get STDP time constant
     * 
     * @return float Time constant (ms)
     */
    float getTimeConstant() const;
    
    /**
     * @brief Set STDP time constant
     * 
     * @param tau New time constant (ms)
     */
    void setTimeConstant(float tau);
    
    /**
     * @brief Get overall learning rate
     * 
     * @return float Learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set overall learning rate
     * 
     * @param rate New learning rate
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get maximum synaptic weight
     * 
     * @return float Maximum weight
     */
    float getMaxWeight() const;
    
    /**
     * @brief Set maximum synaptic weight
     * 
     * @param weight New maximum weight
     */
    void setMaxWeight(float weight);
    
    /**
     * @brief Get minimum synaptic weight
     * 
     * @return float Minimum weight
     */
    float getMinWeight() const;
    
    /**
     * @brief Set minimum synaptic weight
     * 
     * @param weight New minimum weight
     */
    void setMinWeight(float weight);
    
    /**
     * @brief Get weight normalization factor
     * 
     * @return float Normalization factor
     */
    float getNormalization() const;
    
    /**
     * @brief Set weight normalization factor
     * 
     * @param factor New normalization factor
     */
    void setNormalization(float factor);
    
    /**
     * @brief Get STDP trace decay rate
     * 
     * @return float Trace decay rate
     */
    float getTraceDecayRate() const;
    
    /**
     * @brief Set STDP trace decay rate
     * 
     * @param rate New trace decay rate
     */
    void setTraceDecayRate(float rate);
    
    /**
     * @brief Get current STDP trace strength
     * 
     * @return float Trace strength
     */
    float getTraceStrength() const;
    
    /**
     * @brief Get pre-synaptic STDP trace
     * 
     * @return const std::vector<float>& Pre-synaptic trace
     */
    const std::vector<float>& getPreTrace() const;
    
    /**
     * @brief Get post-synaptic STDP trace
     * 
     * @return const std::vector<float>& Post-synaptic trace
     */
    const std::vector<float>& getPostTrace() const;
    
    /**
     * @brief Record a pre-synaptic spike
     * 
     * @param timestamp Spike timestamp
     * @param weight Synaptic weight at time of spike
     */
    void recordPreSpike(Timestamp timestamp, SynapticWeight weight = 1.0f);
    
    /**
     * @brief Record a post-synaptic spike
     * 
     * @param timestamp Spike timestamp
     * @param weight Synaptic weight at time of spike
     */
    void recordPostSpike(Timestamp timestamp, SynapticWeight weight = 1.0f);
    
    /**
     * @brief Update STDP traces
     * 
     * @param dt Time step duration in seconds
     * 
     * This method decays the STDP traces and updates their values
     * based on incoming spikes.
     */
    void updateTraces(TimestepDuration dt);
    
    /**
     * @brief Compute weight change from current traces
     * 
     * @return float Computed weight change
     * 
     * This method computes the weight change based on the current
     * pre- and post-synaptic STDP traces.
     */
    float computeWeightChange() const;
    
    /**
     * @brief Apply neuromodulatory scaling to weight change
     * 
     * @param delta Original weight change
     * @param neuromodulator Neuromodulatory signal
     * @return float Scaled weight change
     * 
     * This method applies neuromodulatory control to the STDP weight change,
     * scaling it based on neuromodulatory signals such as dopamine levels.
     */
    float applyNeuromodulation(float delta, const Neuromodulator& neuromodulator) const;
    
    /**
     * @brief Get STDP statistics
     * 
     * @return STDPStats Statistics about STDP activity
     */
    struct STDPStats {
        size_t pre_spike_count;        ///< Number of pre-synaptic spikes
        size_t post_spike_count;       ///< Number of post-synaptic spikes
        float average_trace_strength; ///< Average STDP trace strength
        float total_weight_change;    ///< Total weight change from this session
        float learning_efficiency;   ///< Efficiency of learning
        Timestamp last_pre_spike;    ///< Time of last pre-synaptic spike
        Timestamp last_post_spike;   ///< Time of last post-synaptic spike
    };
    
    STDPStats getStats() const;
    
    /**
     * @brief Reset STDP state
     * 
     * This method resets all STDP state, including traces and statistics.
     */
    void reset();
    
    /**
     * @brief Check if STDP has been initialized
     * 
     * @return bool true if STDP is initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Initialize STDP with parameters
     * 
     * @param params STDP parameters
     * 
     * @post STDP is initialized with the specified parameters
     */
    void initialize(const STDPParameters& params);
    
private:
    /**
     * @brief Implementation details structure
     * 
     * Contains the actual implementation details using the Pimpl idiom.
     * This allows efficient compilation and reduces coupling between
     * header and implementation.
     */
    struct Impl;
    
    Impl* pImpl; ///< Pointer to implementation details
}; // class STDP

/**
 * @class RewardModulatedSTDP
 * @brief Reward-modulated STDP (R-STDP)
 * 
 * This class extends STDP to include reward-modulated learning, where
 * synaptic changes are gated by reward prediction errors or other
 * neuromodulatory signals. R-STDP combines the temporal precision of
 * STDP with the motivational influence of reward signals.
 * 
 * R-STDP is particularly important for:
 * - Reinforcement learning
 * - Reward prediction error coding
 * - Neuromodulatory control of learning
 * - Action selection and value learning
 * 
 * The R-STDP learning rule:
 * - Uses STDP-like spike timing windows
 * - Scales weight changes based on reward signals
 * - Implements eligibility traces for delayed rewards
 * - Provides stability through reward prediction errors
 * 
 * Usage Examples:
 * @code
 * #include "src/plasticity/STDP.hpp"
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create R-STDP instance
 * RewardModulatedSTDP rstdp;
 * 
 * // Configure with reward prediction error signals
 * Neuromodulator reward_signal;
 * reward_signal.setLevel(0.5f);  // 50% reward prediction error
 * 
 * // Process spikes
 * rstdp.recordPreSpike(time1);
 * rstdp.recordPostSpike(time2);
 * 
 * // Compute reward-modulated weight change
 * float delta = rstdp.computeWeightChange(reward_signal);
 * 
 * // Apply to synapse
 * synapse->addToWeight(delta);
 * @endcode
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     */
    RewardModulatedSTDP();
    
    /**
     * @brief Destructor
     */
    ~RewardModulatedSTDP() override;
    
    /**
     * @brief Disable copy construction
     */
    RewardModulatedSTDP(const RewardModulatedSTDP&) = delete;
    RewardModulatedSTDP& operator=(const RewardModulatedSTDP&) = delete;
    
    /**
     * @brief Enable move construction
     */
    RewardModulatedSTDP(RewardModulatedSTDP&&) noexcept;
    RewardModulatedSTDP& operator=(RewardModulatedSTDP&&) noexcept;
    
    /**
     * @brief Update synaptic weights with reward modulation
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Time step duration in seconds
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change with reward modulation
     * 
     * @param synapse Synapse to update
     * @param delta Base weight change
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule
     */
    const char* getName() const override;
    
    /**
     * @brief Set reward-modulated learning rate
     * 
     * @param rate Learning rate for reward-modulated STDP
     */
    void setRewardLearningRate(float rate);
    
    /**
     * @brief Get reward-modulated learning rate
     * 
     * @return float Learning rate for reward-modulated STDP
     */
    float getRewardLearningRate() const;
    
    /**
     * @brief Set reward prediction error signal
     * 
     * @param error Reward prediction error signal
     */
    void setRewardPredictionError(float error);
    
    /**
     * @brief Get reward prediction error signal
     * 
     * @return float Reward prediction error signal
     */
    float getRewardPredictionError() const;
    
    /**
     * @brief Record reward signal
     * 
     * @param reward Reward value (positive for reward, negative for punishment)
     */
    void recordReward(float reward);
    
    /**
     * @brief Update eligibility traces with reward
     * 
     * @param dt Time step duration
     */
    void updateEligibilityTraces(TimestepDuration dt);
    
    /**
     * @brief Get current reward-modulated weight change
     * 
     * @return float Current weight change
     */
    float getWeightChange() const;
    
    /**
     * @brief Reset reward-modulated learning state
     */
    void reset();
    
    /**
     * @brief Check if reward-modulated STDP has been initialized
     * 
     * @return bool true if initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Initialize reward-modulated STDP
     * 
     * @param base_stdp Base STDP parameters
     * @param reward_rate Reward-modulated learning rate
     */
    void initialize(const STDPParameters& base_stdp, float reward_rate);
    
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class RewardModulatedSTDP

/**
 * @class TripletSTDP
 * @brief Triplet-based STDP with spike interactions
 * 
 * This class implements triplet STDP, which considers the interactions
 * between multiple spikes in close temporal proximity. Triplet STDP
 * accounts for:
 * - Pre-post spike interactions (standard STDP)
 * - Pre-pre spike facilitation (presynaptic spike pairing)
 * - Post-post spike depression (postsynaptic spike pairing)
 * 
 * Triplet STDP is particularly important for:
 * - Burst firing patterns
 * - Synaptic modulation during bursts
 * - Activity-dependent plasticity
 * - Spike-dependent facilitation and depression
 * 
 * The triplet STDP learning rule:
 * - Combines pair-based and triplet-based learning
 * - Accounts for multiple spike interactions
 * - Models synaptic dynamics during burst activity
 * - Provides a more comprehensive learning rule
 * 
 * Usage Examples:
 * @code
 * #include "src/plasticity/STDP.hpp"
 * 
 * // Create triplet STDP instance
 * TripletSTDP triplet_stdp;
 * 
 * // Configure triplet parameters
 * triplet_stdp.setBranchFactor(1.5f);
 * 
 * // Process spikes with burst patterns
 * // Record multiple spikes in quick succession
 * triplet_stdp.recordPreSpike(time1);
 * triplet_stdp.recordPreSpike(time2);  // Second spike close to first
 * triplet_stdp.recordPostSpike(time3);
 * 
 * // Compute triplet-based weight change
 * float delta = triplet_stdp.computeWeightChange();
 * @endcode
 */
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @brief Default constructor
     */
    TripletSTDP();
    
    /**
     * @brief Destructor
     */
    ~TripletSTDP() override;
    
    /**
     * @brief Disable copy construction
     */
    TripletSTDP(const TripletSTDP&) = delete;
    TripletSTDP& operator=(const TripletSTDP&) = delete;
    
    /**
     * @brief Enable move construction
     */
    TripletSTDP(TripletSTDP&&) noexcept;
    TripletSTDP& operator=(TripletSTDP&&) noexcept;
    
    /**
     * @brief Update synaptic weights with triplet interactions
     * 
     * @param synapse Synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Time step duration in seconds
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply weight change with triplet interactions
     * 
     * @param synapse Synapse to update
     * @param delta Base weight change
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule
     */
    const char* getName() const override;
    
    /**
     * @brief Set triplet branching factor
     * 
     * @param factor Branching factor (>1.0 for facilitation)
     */
    void setBranchFactor(float factor);
    
    /**
     * @brief Get triplet branching factor
     * 
     * @return float Branching factor
     */
    float getBranchFactor() const;
    
    /**
     * @brief Set triplet time constants
     * 
     * @param tau_pre Pre-synaptic time constant
     * @param tau_post Post-synaptic time constant
     */
    void setTimeConstants(float tau_pre, float tau_post);
    
    /**
     * @brief Get pre-synaptic time constant
     * 
     * @return float Pre-synaptic time constant
     */
    float getPreTimeConstant() const;
    
    /**
     * @brief Get post-synaptic time constant
     * 
     * @return float Post-synaptic time constant
     */
    float getPostTimeConstant() const;
    
    /**
     * @brief Record a pre-synaptic spike for triplet processing
     * 
     * @param timestamp Spike timestamp
     * @param weight Synaptic weight at time of spike
     */
    void recordPreSpike(Timestamp timestamp, SynapticWeight weight = 1.0f);
    
    /**
     * @brief Record a post-synaptic spike for triplet processing
     * 
     * @param timestamp Spike timestamp
     * @param weight Synaptic weight at time of spike
     */
    void recordPostSpike(Timestamp timestamp, SynapticWeight weight = 1.0f);
    
    /**
     * @brief Update triplet traces
     * 
     * @param dt Time step duration
     */
    void updateTraces(TimestepDuration dt);
    
    /**
     * @brief Compute triplet-based weight change
     * 
     * @return float Computed weight change
     */
    float computeWeightChange() const;
    
    /**
     * @brief Reset triplet STDP state
     */
    void reset();
    
    /**
     * @brief Check if triplet STDP has been initialized
     * 
     * @return bool true if initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Initialize triplet STDP
     * 
     * @param base_stdp Base STDP parameters
     * @param branch_factor Branching factor
     */
    void initialize(const STDPParameters& base_stdp, float branch_factor);
    
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class TripletSTDP

} // namespace nlm

// Close NLM filename guard
#endif // NLM_STDP_HPP

