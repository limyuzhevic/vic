#pragma once

/**
 * @file Neuron.hpp
 * @brief Neuron class implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * This file defines the Neuron class which represents a single biological neuron in the NLM brain.
 * Neurons are the fundamental computational units that perform spiking dynamics and communicate
 * via synaptic connections. This implementation uses the Leaky Integrate-and-Fire model,
 * which is a widely used abstraction in computational neuroscience.
 * 
 * The neuron model includes:
 * - Subthreshold dynamics with exponential decay (membrane potential)
 * - Threshold-based spike generation
 * - Refractory periods to enforce spike timing constraints
 * - Synaptic inputs (excitatory, inhibitory, modulatory)
 * - Plasticity mechanisms (STDP, Hebbian learning)
 * - Spike-frequency adaptation
 * 
 * Key concepts:
 * - Membrane potential dynamics follow the LIF equation:
 *   τ_m * dV/dt = -g_L * (V - E_L) + I_total
 * - Spikes are generated when V ≥ V_th
 * - After spiking, V resets to V_reset and enters refractory period
 * 
 * @author NLM Project
 * @date Phase 2: Real Neural Computation
 * @version 0.1.0
 */

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Neuron state structure for efficient storage and management
 * 
 * This structure stores all the dynamic state variables of a neuron.
 * It's designed for cache efficiency and minimal memory overhead.
 * All variables are reset to default values in the constructor.
 * 
 * @struct NeuronState
 * @ingroup brain_neuron
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV), typically -70 to -40 mV
    MembranePotential restingPotential;    ///< Resting potential (mV), typically -70 mV
    MembranePotential threshold;           ///< Firing threshold (mV), typically -55 mV
    MembranePotential resetPotential;       ///< Post-spike reset value (mV), typically -70 mV
    MembranePotential leakConductance;      ///< Leak conductance (nS), controls membrane time constant
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS), summed synaptic inputs
    FiringRate firingRate;                 ///< Instantaneous firing rate (Hz) for rate-based analysis
    FiringState firingState;               ///< Current firing state (Resting, Active, Refractory, Inhibited)
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period in steps (prevents spike trains)
    float adaptationVariable;              ///< For spike-frequency adaptation, typically positive
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if no spike yet)
    
    /**
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets all membrane potential variables to typical resting values:
     * - membranePotential = -70 mV
     * - threshold = -55 mV
     * - resetPotential = -70 mV
     * - refractoryPeriod = 5 steps
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
 * @brief Single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * This is the core neuron class that performs the actual neural computation.
 * It handles:
 * - Subthreshold integration of membrane potential
 * - Spike generation and refractory periods
 * - Synaptic input processing
 * - Plasticity rule application
 * - State management and initialization
 * 
 * The neuron uses a pImpl idiom for efficient compilation and memory management.
 * This allows the implementation to be changed without recompiling headers.
 * 
 * @class Neuron
 * @ingroup brain_neuron
 * @ingroup brain_core
 */
class Neuron {
public:
    /**
     * @brief Construct a new neuron with a unique ID
     * 
     * @param id Unique identifier for this neuron
     * @throws std::bad_alloc if memory allocation fails
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the neuron and clean up resources
     */
    ~Neuron();
    
    /**
     * @brief Disable copy construction
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * This allows neurons to be efficiently moved between containers
     * without copying their internal implementation.
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Enable move assignment
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId The neuron's identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type (excitatory, inhibitory, etc.)
     * 
     * Neuron types affect:
     * - Initial membrane potential settings
     * - Synaptic weight signs
     * - Plasticity rules
     * - Functional role in networks
     * 
     * @return NeuronType The neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get a constant reference to the neuron's current state
     * 
     * @return const NeuronState& Reference to internal state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get a mutable reference to the neuron's state
     * 
     * Note: Direct modification of state is discouraged. Use the provided
     * methods instead for proper state management.
     * 
     * @return NeuronState& Mutable reference to internal state
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential
     * 
     * Note: This bypasses normal LIF dynamics. For external inputs, use
     * injectCurrent() instead. This is primarily for testing and initialization.
     * 
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a current input to the membrane potential
     * 
     * This represents external or synaptic input current. The actual change
     * in membrane potential depends on the neuron's leak conductance.
     * 
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * 
     * When membrane potential reaches or exceeds this value, the neuron fires.
     * 
     * @return MembranePotential Threshold voltage in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New threshold voltage in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing
     * 
     * @return true if neuron has recently fired and is in active state
     * @return false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * During refractory period, the neuron cannot fire regardless of input.
     * 
     * @return true if neuron is in refractory period
     * @return false if neuron can fire
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state
     * 
     * This is a low-level method. Use higher-level methods like checkThreshold()
     * and stepLIF() instead.
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps the neuron should remain refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the refractory period counter
     * 
     * This should be called once per simulation step while the neuron is refractory.
     */
    void decrementRefractory();
    
    /**
     * @brief Get the firing rate
     * 
     * @return FiringRate Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * 
     * This is primarily for rate-based analysis or artificial neuron types.
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance
     * 
     * Controls the membrane time constant (τ_m = 1/g_L). Higher conductance
     * means faster potential decay.
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
     * This is the membrane potential the neuron naturally decays to when
     * no inputs are present.
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
     * After a spike, the membrane potential is reset to this value.
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
     * This performs a threshold check without modifying state.
     * 
     * @return true if membrane potential ≥ threshold
     * @return false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Last spike time in seconds, or -1.0 if no spike
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one step of LIF dynamics
     * 
     * This is the core computational step for the neuron. It:
     * 1. Updates membrane potential based on LIF equation
     * 2. Checks for threshold crossing
     * 3. Handles spike generation and refractory periods
     * 
     * The LIF equation: τ_m * dV/dt = -g_L * (V - E_L) + I_total
     * Discretized: V(t+dt) = V(t) + dt * (-g_L/τ_m * (V - E_L) + I_total/τ_m)
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Time step duration in seconds
     * @return true if neuron fired this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * Excitatory inputs increase membrane potential (depolarize).
     * Typical values: +0.5 to +20 mV.
     * 
     * @param amplitude Input amplitude in mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Inhibitory inputs decrease membrane potential (hyperpolarize).
     * Typical values: -0.5 to -20 mV.
     * 
     * @param amplitude Input amplitude in mV (positive value indicates inhibition)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Modulatory inputs can have complex effects on neuron dynamics,
     * such as changing threshold, reset potential, or firing rate.
     * 
     * @param amplitude Input amplitude in mV
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current into the neuron
     * 
     * This represents external input from sensory inputs, other neurons,
     * or controlled experimental inputs.
     * 
     * @param current Current in pA (picoamperes)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total injected current
     * 
     * @return MembranePotential Total current in pA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the total injected current
     * 
     * This resets the current accumulator to zero. Current is accumulated
     * across simulation steps.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event for plasticity computation
     * 
     * This should be called when the neuron fires, allowing plasticity
     * mechanisms to track spike timing for STDP and other rules.
     * 
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the spike history for plasticity rules
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history
     * 
     * This is useful between simulation phases or experiments.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * @param handle Handle to the synapse (used for fast lookup)
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * @param handle Handle to the synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapses
     * 
     * @return const std::vector<SynapseHandle>& List of incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapses
     * 
     * @return const std::vector<SynapseHandle>& List of outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * Plasticity flags control which plasticity rules apply to this neuron:
     * - hebbian: Hebbian learning rules
     * - stdp: Spike-timing-dependent plasticity
     * - rewardModulated: Reward-modulated plasticity
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity rules for this neuron
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the neural region ID for this neuron
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the neural region ID
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID for this neuron
     * 
     * Populations are groups of neurons with similar properties.
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Perform one simulation step for the neuron
     * 
     * This is a high-level interface that calls stepLIF() and handles
     * spike recording and plasticity timing.
     * 
     * Note: This method is part of Phase 2 implementation (currently TODO).
     * For Phase 2, see stepLIF() for the actual implementation.
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     * 
     * This resets all membrane potential variables to resting values,
     * clears spike history, and resets firing state to Resting.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * Sets random values for neuron parameters such as threshold,
     * leak conductance, and other biophysical properties.
     * 
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl implementation pointer
     * 
     * Uses the pImpl idiom to hide implementation details and improve
     * compilation speed. The actual implementation is in Neuron.cpp.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#endif // NLM_NEURON_HPP