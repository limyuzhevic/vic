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
 * @file Neuron.hpp
 * @brief Single neuron implementation for spiking neural networks
 * 
 * This file implements the Neuron class, a fundamental building block of the
 * NLM system that represents individual neurons with biologically plausible
 * dynamics. The neuron implements Leaky Integrate-and-Fire (LIF) dynamics with
 * support for multiple input types, plasticity mechanisms, and synaptic integration.
 * 
 * The Neuron class is designed with the following key principles:
 * - Biophysical realism with membrane dynamics
 * - Efficient memory usage with the Pimpl idiom
 * - Support for multiple input modalities (excitatory, inhibitory, modulatory)
 * - Integration with spike-timing-dependent plasticity (STDP)
 * - Thread-safe operation for parallel simulation
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see Synapse.hpp - Synaptic connections that communicate with neurons
 * @see Brain.hpp - Brain class that contains and coordinates neurons
 * @see STDP.hpp - Spike-timing-dependent plasticity implementation
 * @see PlasticityRule.hpp - Base class for plasticity mechanisms
 * @see src/core/Types/Types.hpp - Neuron type definitions and enums
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 */

#pragma once

// NLM filename guard
#ifndef NLM_NEURON_HPP
#define NLM_NEURON_HPP

#include "../core/Types/Types.hpp"
#include "../plasticity/STDP.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <array>
#include <optional>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

/**
 * @struct NeuronState
 * @brief Comprehensive state representation for a single neuron
 * 
 * This structure encapsulates all the dynamic state variables of a neuron,
 * enabling efficient updates and persistence. The state is designed for
 * optimal memory locality and cache efficiency.
 * 
 * State Variables:
 * - membranePotential: Current electrical potential across the membrane
 * - restingPotential: Equilibrium potential when neuron is not active
 * - threshold: Voltage at which neuron generates an action potential
 * - resetPotential: Voltage after spike generation (reset value)
 * - leakConductance: Membrane conductance to resting potential
 * - synapseConductance: Total conductance from synaptic inputs
 * - firingRate: Current firing rate (Hz)
 * - firingState: Discrete firing state for simulation
 * - refractoryRemaining: Steps remaining in refractory period
 * - refractoryPeriod: Total refractory period duration
 * - adaptationVariable: Spike-frequency adaptation variable
 * - lastSpikeTime: Timestamp of last spike event
 * 
 * The state is updated through the step() function and can be accessed
 * through getter methods for simulation and analysis purposes.
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV)
    MembranePotential threshold;           ///< Firing threshold (mV)
    MembranePotential resetPotential;       ///< Post-spike reset value (mV)
    MembranePotential leakConductance;      ///< Leak conductance (nS)
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS)
    FiringRate firingRate;                 ///< Current firing rate (Hz)
    FiringState firingState;               ///< Current firing state
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period (steps)
    float adaptationVariable;              ///< For spike-frequency adaptation
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor with biologically plausible initial values
     * 
     * Initializes the neuron to a resting state with typical parameters:
     * - Membrane potential at resting level (-70 mV)
     * - Threshold above resting for spike generation (-55 mV)
     * - Reset potential back to resting (-70 mV)
     * - Leak conductance for exponential decay
     * - All other state variables to neutral/default values
     */
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @class Neuron
 * @brief Single spiking neuron with biologically plausible dynamics
 * 
 * The Neuron class implements a Leaky Integrate-and-Fire (LIF) neuron model
 * with comprehensive support for synaptic integration, plasticity, and multiple
 * input modalities. The neuron can operate in different modes and has extensive
 * configuration options for various neural network scenarios.
 * 
 * Key Features:
 * - Leaky Integrate-and-Fire dynamics with adaptive thresholds
 * - Support for multiple input types (excitatory, inhibitory, modulatory)
 * - Spike-timing-dependent plasticity (STDP) integration
 * - Neuromodulatory signal processing
 * - Efficient synaptic management
 * - Thread-safe operation for parallel processing
 * 
 * The neuron supports different operational modes:
 * - LIF mode: Basic spiking behavior with membrane dynamics
 * - Rate mode: Continuous firing rate representation
 * - Hybrid mode: Combination of both approaches
 * 
 * Usage Examples:
 * @code
 * #include "src/brain/Neuron.hpp"
 * #include "src/core/Types/Types.hpp"
 * 
 * // Create a neuron
 * Neuron neuron(NeuronId(1));
 * neuron.setType(NeuronType::Excitatory);
 * 
 * // Configure neuron parameters
 * neuron.setRestingPotential(-65.0f);
 * neuron.setThreshold(-50.0f);
 * neuron.setLeakConductance(15.0f);
 * 
 * // Initialize with random parameters
 * RandomGenerator rng(42);  // Seed for reproducibility
 * neuron.initializeRandom(rng);
 * 
 * // Simulate neuron activity
 * for (uint64_t step = 0; step < 1000; ++step) {
 *     Timestamp time = static_cast<double>(step) * 0.01;  // 10ms timestep
 *     
 *     // Inject input
 *     neuron.injectCurrent(5.0f);  // Depolarizing current
 *     
 *     // Update neuron state
 *     bool fired = neuron.stepLIF(time, 0.01f);
 *     
 *     if (fired) {
 *         // Record spike for plasticity
 *         neuron.recordSpike(time);
 *     }
 *     
 *     // Reset if needed
 *     if (neuron.checkThreshold()) {
 *         neuron.reset();
 *     }
 * }
 * 
 * // Access neuron state
 * NeuronState state = neuron.getState();
 * float potential = neuron.getMembranePotential();
 * bool active = neuron.isFiring();
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in Neuron.cpp.
 */
class Neuron {
public:
    /**
     * @brief Constructor for neuron creation
     * 
     * Creates a neuron with the specified identifier and initializes it
     * to a default resting state.
     * 
     * @param id Unique identifier for the neuron within its network
     * @throw std::invalid_argument if id has an invalid value
     * 
     * @post Neuron is in a valid resting state and ready for configuration
     * 
     * Example:
     * @code
     * Neuron neuron(NeuronId(42));  // Create neuron with ID 42
     * @endcode
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor for neuron cleanup
     * 
     * Cleans up all allocated resources including spike history,
     * synaptic connections, and any allocated memory.
     */
    ~Neuron();
    
    /**
     * @brief Disable copy construction for neuron safety
     * 
     * Neurons cannot be copied because they contain:
     * - Unique IDs that would conflict
     * - Dynamic resources that shouldn't be shared
     * - Internal state that requires coordination
     * 
     * @note Use move construction instead for transferring ownership
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move construction for neuron transfer
     * 
     * Allows efficient transfer of neuron ownership from one instance
     * to another. After move construction, the source neuron is in a
     * valid but unspecified state.
     * 
     * @param other Neuron to move from
     * @post Source neuron is in valid but unspecified state
     */
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId Unique identifier for this neuron
     * 
     * The neuron ID is immutable after creation and serves as the primary
     * key for referencing this neuron within the network.
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type
     * 
     * @return NeuronType Type of this neuron (Excitatory, Inhibitory, etc.)
     * 
     * @see NeuronType enum in src/core/Types/Types.hpp
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type
     * 
     * @see NeuronType enum in src/core/Types/Types.hpp
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete neuron state
     * 
     * @return const NeuronState& Reference to the current neuron state
     * 
     * This method provides read-only access to the neuron's complete state.
     * For modifications, use the individual setter methods or getState() const_cast.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get a mutable reference to the neuron state
     * 
     * @return NeuronState& Mutable reference to the neuron state
     * 
     * @warning Modifying state directly can lead to inconsistent state.
     *         Use the provided getter/setter methods for thread-safe updates.
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     * 
     * The membrane potential represents the electrical charge difference
     * across the neuron's membrane. Typical values are:
     * - Resting: ~ -70 mV
     * - Firing threshold: ~ -55 mV
     * - Depolarized: > -55 mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential in mV
     * 
     * @warning This bypasses normal dynamics and should only be used for:
     *         - Initialization
     *         - Testing/debugging
     *         - External control
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the membrane potential
     * 
     * @param delta Change in membrane potential in mV
     * 
     * This is more efficient and biologically plausible than setMembranePotential
     * for gradual changes caused by synaptic inputs.
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * 
     * @return MembranePotential Current firing threshold in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New firing threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool true if neuron is in active firing state
     * 
     * @note A neuron can be firing without generating spikes (rate-based mode).
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool true if neuron is refractory (cannot spike)
     * 
     * During the refractory period, the neuron cannot generate new action
     * potentials to prevent excessive firing rates.
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state directly
     * 
     * @param state New firing state
     * 
     * @see FiringState enum for valid states
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps to be refractory
     * 
     * @pre Steps must be greater than 0
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     * 
     * This should be called once per timestep when the neuron is refractory.
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance
     * 
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential
     * 
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return bool true if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Timestamp of last spike, or -1.0 if no spike recorded
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF integration step
     * 
     * This is the core neural computation step that updates the neuron's
     * membrane potential according to the Leaky Integrate-and-Fire model:
     * 
     * dV/dt = (-g_L * (V - V_L) + I_syn + I_ext) / C
     * 
     * where:
     * - g_L is leak conductance
     * - V_L is resting potential
     * - I_syn is synaptic current
     * - I_ext is external current injection
     * - C is membrane capacitance (implicit in time constants)
     * 
     * @param currentTime Current simulation timestamp in seconds
     * @param dt Time step duration in seconds
     * @return bool true if neuron fired (membrane potential crossed threshold)
     * 
     * @note This is the main neural computation method and should be called
     *       once per simulation timestep for each neuron.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Amplitude of excitatory input in mV
     * 
     * Excitatory inputs increase the membrane potential toward threshold.
     * They typically originate from other excitatory neurons or external
     * sensory inputs.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Amplitude of inhibitory input in mV
     * 
     * Inhibitory inputs decrease the membrane potential away from threshold,
     * making it harder for the neuron to fire. They originate from inhibitory
     * interneurons or GABAergic synapses.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Amplitude of modulatory input in mV
     * 
     * Modulatory inputs can:
     * - Change neuron excitability
     * - Modify synaptic plasticity
     * - Alter firing patterns
     * - Influence attention and salience
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current into the neuron
     * 
     * @param current External current in nA (positive for depolarization,
     *               negative for hyperpolarization)
     * 
     * External current represents:
     * - Input from sensory systems
     * - Neuromodulatory effects
     * - Artificial stimulation for testing
     * - Learning-driven modifications
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total injected current
     * 
     * @return MembranePotential Total injected current in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the injected current
     * 
     * Resets the external current to zero. This should be called after
     * each simulation step to prevent current accumulation.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event
     * 
     * @param timestamp Timestamp when the spike occurred
     * 
     * Records the spike time for plasticity calculations and statistical
     * analysis. Spikes are used by STDP and other plasticity mechanisms.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * 
     * @note The spike history is maintained for plasticity computations and
     *       can be cleared with clearSpikeHistory().
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history
     * 
     * Removes all recorded spike times. This is useful for:
     * - Resetting plasticity state
     * - Statistical analysis
     * - Debugging simulation issues
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * @param handle Synapse handle of the incoming connection
     * 
     * This synapse will deliver input to this neuron when the source
     * neuron fires.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * @param handle Synapse handle of the outgoing connection
     * 
     * This neuron will send output to the destination neuron when it fires.
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     * 
     * Plasticity flags control which plasticity mechanisms are active
     * for this neuron, including Hebbian, STDP, and reward-modulated learning.
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
     * This method configures the plasticity mechanisms that will be applied
     * when spikes occur and synaptic events happen.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID
     * 
     * @param region Region identifier for this neuron
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID
     * 
     * @param population Population identifier for this neuron
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Perform one simulation step
     * 
     * This method advances the neuron by one simulation timestep, updating
     * all internal state variables including membrane potential, firing state,
     * refractory period, and applying any plasticity rules that might be active.
     * 
     * @param currentTime Current simulation timestamp in seconds
     * 
     * @note PHASE 2: This method currently only implements basic resetting.
     *       Full integrate-and-fire dynamics are planned for Phase 2.
     * 
     * @warning This method should be called exactly once per timestep per neuron
     *         for proper simulation timing.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to initial state
     * 
     * This method resets the neuron to its initial resting state, clearing
     * any accumulated state changes and preparing the neuron for the next
     * phase of activity.
     * 
     * The reset process involves:
     * 1. Restoring membrane potential to resting level
     * 2. Clearing any injected current
     * 3. Resetting firing rate to zero
     * 4. Setting firing state to resting
     * 5. Clearing spike history
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * @param rng Random number generator for reproducible initialization
     * 
     * This method sets neuron parameters to random values drawn from
     * biologically plausible distributions. This is useful for:
     * - Creating diverse neural populations
     * - Testing network behavior
     * - Reproducible experiments
     */
    void initializeRandom(RandomGenerator& rng);
    
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
}; // class Neuron

} // namespace nlm

// Close NLM filename guard
#endif // NLM_NEURON_HPP

