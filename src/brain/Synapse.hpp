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
 * @file Synapse.hpp
 * @brief Synaptic connections between neurons with activity-dependent plasticity
 * 
 * This file implements the Synapse class, representing biological synaptic
 * connections that transmit signals between neurons. The synapse model includes
 * realistic dynamics with delay, short-term plasticity, and multiple plasticity
 * mechanisms including STDP, Hebbian learning, and reward-modulated learning.
 * 
 * The Synapse class captures the essential biophysical and computational
 * properties of real synapses:
 * - Chemical transmission with delay
 * - Activity-dependent facilitation and depression
 * - Weight modulation through plasticity rules
 * - Neuromodulatory control of synaptic strength
 * - Eligibility trace for reward-modulated learning
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see Neuron.hpp - Neuron class that synapses connect to
 * @see Brain.hpp - Brain class that manages synapses
 * @see STDP.hpp - Spike-timing-dependent plasticity implementation
 * @see Hebbian.hpp - Hebbian plasticity implementation
 * @see Neuromodulator.hpp - Neuromodulatory signal processing
 * @see src/core/Types/Types.hpp - Synapse type definitions and enums
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The synapse model supports both biological realism and computational efficiency.
 */

#pragma once

// NLM filename guard
#ifndef NLM_SYNAPSE_HPP
#define NLM_SYNAPSE_HPP

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>

namespace nlm {

/**
 * @struct SynapseState
 * @brief Complete state representation for synaptic transmission
 * 
 * This structure contains all dynamic state variables necessary for realistic
 * synaptic transmission, including activity history, plasticity traces,
 * and short-term modulation effects.
 * 
 * State Variables:
 * - preSpikeHistory: Timestamps of pre-synaptic spikes for STDP
 * - postSpikeHistory: Timestamps of post-synaptic spikes for STDP
 * - eligibilityTrace: Reward-modulated learning trace
 * - efficacy: Activity-dependent synaptic efficacy
 * - recentActivity: Recent spike count for short-term plasticity
 * - averageWeight: Running average of synaptic weight
 * - weightVariance: Variance of weight fluctuations
 * - facilitationTrace: Short-term facilitation variable
 * - depressionTrace: Short-term depression variable
 * 
 * The state is optimized for efficient updates and memory access patterns
 * during synaptic transmission and plasticity calculations.
 */
struct SynapseState {
    std::vector<Timestamp> preSpikeHistory;     ///< Pre-synaptic spike timestamps for STDP
    std::vector<Timestamp> postSpikeHistory;    ///< Post-synaptic spike timestamps for STDP
    float eligibilityTrace;                      ///< Reward-modulated eligibility trace
    float efficacy;                              ///< Activity-dependent synaptic efficacy
    uint32_t recentActivity;                     ///< Recent spike count for STP
    float averageWeight;                         ///< Running average of synaptic weight
    float weightVariance;                        ///< Variance of weight fluctuations
    float facilitationTrace;                     ///< Short-term facilitation variable
    float depressionTrace;                       ///< Short-term depression variable
    
    /**
     * @brief Default constructor with neutral initial values
     * 
     * Initializes synaptic state to a resting but responsive state,
     * with empty activity history and neutral trace values.
     */
    SynapseState()
        : eligibilityTrace(0.0f)
        , efficacy(1.0f)                           ///< Normalized baseline efficacy
        , recentActivity(0)
        , averageWeight(1.0f)
        , weightVariance(0.0f)
        , facilitationTrace(1.0f)
        , depressionTrace(1.0f) {}
};

/**
 * @class Synapse
 * @brief Biological synaptic connection with realistic dynamics
 * 
 * The Synapse class implements a comprehensive synaptic model that captures
 * the essential computational properties of biological synapses, including:
 * - Transmission with realistic delay
 * - Activity-dependent short-term plasticity (STP)
 * - Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
 * - Neuromodulatory control
 * - Weight normalization and stability
 * 
 * The synapse model is designed for both biological plausibility and
 * computational efficiency, making it suitable for large-scale neural
 * network simulations.
 * 
 * Key Features:
 * - Realistic transmission delay (0-100 ms)
 * - Short-term facilitation and depression
 * - Spike-timing-dependent plasticity
 * - Reward-modulated learning
 * - Neuromodulatory signal integration
 * - Weight clamping to prevent instability
 * - Thread-safe operation for parallel simulation
 * 
 * Usage Examples:
 * @code
 * #include "src/brain/Synapse.hpp"
 * #include "src/brain/Neuron.hpp"
 * 
 * // Create a synapse between two neurons
 * Synapse synapse(SynapseId(1), NeuronId(10), NeuronId(20));
 * synapse.setType(SynapseType::Excitatory);
 * synapse.setWeight(0.5f);
 * synapse.setDelay(5);  // 5 ms delay
 * 
 * // Configure synaptic dynamics
 * synapse.setFacilitationParameters(0.1f, 10.0f);  // τ_f=0.1ms, U=10
 * synapse.setDepressionParameters(0.05f, 200.0f); // τ_d=0.05ms, U=200
 * 
 * // Configure STDP
 * synapse.enablePlasticity(true, true, false);  // Hebbian + STDP enabled
 * synapse.configureSTDP(0.01f, -0.012f, 20.0f);  // LTP=0.01, LTD=-0.012, τ=20ms
 * 
 * // Simulate synaptic transmission
 * neuron1.step(time, dt);  // Pre-synaptic neuron fires
 * // Synapse automatically delivers post-synaptic input after delay
 * 
 * // Update synapse state
 * synapse.step(time);  // Apply STP and decay traces
 * 
 * // Access synaptic state
 * SynapseState state = synapse.getState();
 * float weight = synapse.getWeight();
 * float efficacy = synapse.getEfficacy();
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and
 *       reduced coupling. The actual implementation is in Synapse.cpp.
 */
class Synapse {
public:
    /**
     * @brief Constructor for synapse creation
     * 
     * Creates a synaptic connection between a source and destination neuron
     * with initial parameters and state.
     * 
     * @param id Unique identifier for the synapse within the network
     * @param source Neuron ID of the pre-synaptic neuron
     * @param destination Neuron ID of the post-synaptic neuron
     * 
     * @throw std::invalid_argument if IDs are invalid or source == destination
     * 
     * @post Synapse is in a valid initial state with default parameters
     * 
     * Example:
     * @code
     * SynapseId synId(1);
     * NeuronId preId(10);
     * NeuronId postId(20);
     * Synapse synapse(synId, preId, postId);
     * @endcode
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor for synapse cleanup
     * 
     * Cleans up all allocated resources and maintains proper network state
     * by removing the synapse from connected neurons' synaptic lists.
     */
    ~Synapse();
    
    /**
     * @brief Disable copy construction for synapse safety
     * 
     * Synapses cannot be copied because they:
     * - Reference specific neurons that shouldn't have duplicate connections
     * - Contain dynamic state that shouldn't be shared
     * - Have network topology constraints that would be violated
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Enable move construction for synapse transfer
     * 
     * Allows efficient transfer of synapse ownership from one connection
     * to another. After move construction, the source synapse is in a
     * valid but unspecified state.
     * 
     * @param other Synapse to move from
     */
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId Unique identifier for this synapse
     * 
     * The synapse ID is immutable after creation and serves as the primary
     * key for referencing this synapse within the neural network.
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron ID
     * 
     * @return NeuronId ID of the pre-synaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron ID
     * 
     * @return NeuronId ID of the post-synaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current synaptic weight
     * 
     * @note Weight typically ranges from 0.0 (no connection) to 1.0 (strong connection),
     *       but can exceed these bounds to represent depression or facilitation states.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic weight
     * @param clamp If true, clamp weight to valid range (default: true)
     * 
     * @see getWeight()
     */
    void setWeight(SynapticWeight weight, bool clamp = true);
    
    /**
     * @brief Add to the synaptic weight
     * 
     * @param delta Change in weight
     * 
     * This is more efficient than setWeight() for plasticity updates.
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic delay
     * 
     * @return Delay Synaptic delay in simulation steps
     * 
     * @note Delay represents the transmission time from pre-synaptic to
     *       post-synaptic neuron. Typical values range from 0 to 100 steps.
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic delay
     * 
     * @param delay New synaptic delay in simulation steps
     * 
     * @pre Delay must be non-negative
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType Type of this synapse (Excitatory, Inhibitory, etc.)
     * 
     * @see SynapseType enum in src/core/Types/Types.hpp
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return bool true if synapse is excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return bool true if synapse is inhibitory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Get the complete synapse state
     * 
     * @return const SynapseState& Reference to the current synapse state
     * 
     * This method provides read-only access to the synapse's complete state.
     * For modifications, use the individual setter methods.
     */
    const SynapseState& getState() const;
    
    /**
     * @brief Get a mutable reference to the synapse state
     * 
     * @return SynapseState& Mutable reference to the synapse state
     * 
     * @warning Modifying state directly can lead to inconsistent state.
     *         Use the provided getter/setter methods for thread-safe updates.
     */
    SynapseState& getState();
    
    /**
     * @brief Record a pre-synaptic spike
     * 
     * @param timestamp Timestamp when the pre-synaptic neuron fired
     * 
     * This method updates the spike history and triggers STDP calculations
     * for eligible plasticity mechanisms.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a post-synaptic spike
     * 
     * @param timestamp Timestamp when the post-synaptic neuron fired
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get pre-synaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of pre-synaptic spike timestamps
     * 
     * @note Used by STDP and other spike-timing dependent plasticity mechanisms.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get post-synaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of post-synaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Removes all recorded spike times. This is useful for:
     * - Resetting plasticity state
     * - Memory consolidation
     * - Simulation restart
     */
    void clearHistory();
    
    /**
     * @brief Get the plasticity flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     * 
     * Plasticity flags control which plasticity mechanisms are active
     * for this synapse, including Hebbian, STDP, and reward-modulated learning.
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get a mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * This method configures which plasticity rules will be applied when
     * synaptic activity occurs.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace
     * 
     * @return float Current eligibility trace value
     * 
     * The eligibility trace is used for reward-modulated learning,
     * maintaining a record of recent synaptic activity that can be
     * strengthened or weakened by reward signals.
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace
     * 
     * @param trace New eligibility trace value
     * 
     * @note The eligibility trace is typically updated automatically
     *       based on synaptic activity, not set directly.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace
     * 
     * @param decayRate Trace decay rate per timestep
     * 
     * This method applies the standard exponential decay to the
     * eligibility trace, used in reward-modulated learning.
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the synaptic efficacy
     * 
     * @return float Current synaptic efficacy
     * 
     * Efficacy represents the current strength of the synaptic connection
     * accounting for short-term modulation (facilitation and depression).
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * @param efficacy New synaptic efficacy
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Get the short-term facilitation time constant
     * 
     * @return float Facilitation time constant in milliseconds
     */
    float getFacilitationTimeConstant() const;
    
    /**
     * @brief Set the short-term facilitation time constant
     * 
     * @param tau New facilitation time constant in milliseconds
     */
    void setFacilitationTimeConstant(float tau);
    
    /**
     * @brief Get the short-term depression time constant
     * 
     * @return float Depression time constant in milliseconds
     */
    float getDepressionTimeConstant() const;
    
    /**
     * @brief Set the short-term depression time constant
     * 
     * @param tau New depression time constant in milliseconds
     */
    void setDepressionTimeConstant(float tau);
    
    /**
     * @brief Get the short-term facilitation parameter
     * 
     * @return float Facilitation parameter (U), typically in [0, 1]
     */
    float getFacilitationParameter() const;
    
    /**
     * @brief Set the short-term facilitation parameter
     * 
     * @param u New facilitation parameter (U), typically clamped to [0, 1]
     */
    void setFacilitationParameter(float u);
    
    /**
     * @brief Get the short-term depression parameter
     * 
     * @return float Depression parameter (U), typically in [0, 1]
     */
    float getDepressionParameter() const;
    
    /**
     * @brief Set the short-term depression parameter
     * 
     * @param u New depression parameter (U), typically clamped to [0, 1]
     */
    void setDepressionParameter(float u);
    
    /**
     * @brief Configure short-term plasticity parameters
     * 
     * @param tau_f Facilitation time constant (ms)
     * @param u_f Facilitation parameter (U, 0-1)
     * @param tau_d Depression time constant (ms)
     * @param u_d Depression parameter (U, 0-1)
     * 
     * These parameters control the short-term dynamics of synaptic efficacy
     * through the model of Abbott, Regehr, and O'Donnell (1997).
     */
    void configureSTP(float tau_f, float u_f, float tau_d, float u_d);
    
    /**
     * @brief Apply one simulation step
     * 
     * This method updates the synapse's state according to all active plasticity
     * mechanisms and short-term dynamics. The transmission and update process
     * involves:
     * 
     * 1. Check for spike delivery (if delayed spike event pending)
     * 2. Apply spike transmission effects
     * 3. Update spike timing history
     * 4. Apply short-term plasticity (facilitation and depression)
     * 5. Decay eligibility traces
     * 6. Update STDP weights based on spike timing
     * 7. Apply neuromodulatory signals
     * 8. Normalize and clamp weights
     * 
     * @param currentTime Current simulation timestamp in seconds
     * 
     * @note PHASE 2: This method currently implements placeholder logic.
     *       Full synaptic dynamics are planned for Phase 2 implementation.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the synapse to initial state
     * 
     * This method resets the synapse to its initial state, clearing all
     * activity history and returning to baseline parameters.
     * 
     * The reset process involves:
     * 1. Clearing spike history
     * 2. Resetting eligibility trace
     * 3. Resetting facilitation/depression traces
     * 4. Restoring original weight
     * 5. Resetting efficacy to baseline
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     * 
     * @param rng Random number generator for reproducible initialization
     * 
     * This method sets synapse parameters to random values drawn from
     * biologically plausible distributions. This is useful for:
     * - Creating diverse synaptic populations
     * - Testing network behavior
     * - Reproducible experiments
     */
    void initializeRandom(class RandomGenerator& rng);
    
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

protected:
    /**
     * @brief Configure STDP parameters
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param timeConstant STDP time constant (ms)
     * 
     * This protected method allows subclasses to configure STDP parameters.
     */
    void configureSTDP(float ltpWeight, float ltdWeight, float timeConstant);
}; // class Synapse

} // namespace nlm

// Close NLM filename guard
#endif // NLM_SYNAPSE_HPP

