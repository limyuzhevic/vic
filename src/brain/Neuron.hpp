#pragma once

/**
 * @file Neuron.hpp
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation
 * @author NLM Project
 * @date Phase 2: Real Neural Computation
 * 
 * This file defines the Neuron class representing a single biological neuron
 * implementing the Leaky Integrate-and-Fire (LIF) mathematical model.
 * 
 * The neuron models key electrophysiological properties:
 * - Membrane potential dynamics with exponential decay
 * - Action potential generation and refractory periods
 * - Synaptic integration and spike-frequency adaptation
 * - Plasticity state management for learning
 * 
 * @note This is Phase 2 of the NLM project - Real Neural Computation
 */

#include "../core/Types/Types.hpp"
#include "../core/Constants/Constants.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * @brief Complete state of a neuron for simulation purposes
 * 
 * This structure contains all state variables needed to simulate
 * Leaky Integrate-and-Fire neuron dynamics. The state is stored
 * in a contiguous structure for cache efficiency.
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
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets initial values based on biological neuron properties:
     * - Membrane potential near resting potential (-70mV)
     * - Threshold above resting potential (-55mV)
     * - Short initial refractory period (5 steps)
     * - Zero adaptation variable
     */
    NeuronState()
        : membranePotential(NeuronConstants::DEFAULT_MEMBRANE_POTENTIAL)
        , restingPotential(NeuronConstants::DEFAULT_RESTING_POTENTIAL)
        , threshold(NeuronConstants::DEFAULT_THRESHOLD)
        , resetPotential(NeuronConstants::DEFAULT_RESET_POTENTIAL)
        , leakConductance(NeuronConstants::DEFAULT_LEAK_CONDUCTANCE)
        , synapseConductance(SynapseConstants::DEFAULT_SYNAPSE_CONDUCTANCE)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @class Neuron
 * @brief Single biological neuron implementing Leaky Integrate-and-Fire dynamics
 * 
 * This class models a real biological neuron with:
 * - Membrane potential dynamics following LIF equations
 * - Action potential generation with refractory periods
 * - Synaptic integration from incoming and outgoing connections
 * - Spike-frequency adaptation mechanisms
 * - Plasticity state for learning (Hebbian, STDP, reward-modulated)
 * 
 * The neuron uses the pImpl pattern for efficient compilation and
 * implements real electrophysiology based on the LIF mathematical model.
 * 
 * @tparam NeuronId Type for neuron identification (typically uint32_t)
 * @tparam NeuronType Enum type for neuron classification (Sensory, Internal, Motor)
 * 
 * @note This implementation is optimized for biological realism and
 * computational efficiency. The class is non-copyable but movable.
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with unique identifier
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted (neurons are not copied)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor (transfers ownership of implementation)
     * 
     * @param other Neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator (transfers ownership)
     * 
     * @param other Neuron to move from
     * @return Reference to this neuron
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Gets the neuron's unique identifier
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron's type
     * 
     * @return NeuronType Type of neuron (Sensory, Internal, Motor)
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron's type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the current membrane potential state
     * 
     * @return const NeuronState& Complete neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets mutable reference to neuron state for modification
     * 
     * @return NeuronState& Mutable neuron state
     * @note Use with caution - may affect simulation correctness
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential
     * 
     * @return MembranePotential Current potential (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential directly
     * 
     * @param potential New membrane potential (mV)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds a delta to the membrane potential
     * 
     * @param delta Change in potential (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the firing threshold potential
     * 
     * @return MembranePotential Threshold potential (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold
     * 
     * @param threshold New threshold potential (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if neuron is currently firing
     * 
     * @return bool true if neuron is firing or active
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if neuron is in refractory period
     * 
     * @return bool true if neuron cannot fire
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements refractory period counter
     * 
     * Called automatically during simulation to track remaining
     * refractory time. Updates firing state when complete.
     */
    void decrementRefractory();
    
    /**
     * @brief Gets the neuron's firing rate
     * 
     * @return FiringRate Current firing rate (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the neuron's firing rate
     * 
     * @param rate New firing rate (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the leak conductance parameter
     * 
     * @param conductance Leak conductance (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the leak conductance parameter
     * 
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Gets the refractory period duration
     * 
     * @return uint32_t Refractory period (steps)
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential
     * 
     * @param potential Resting potential (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential
     * 
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (post-spike value)
     * 
     * @param potential Reset potential (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Checks if membrane potential has reached threshold
     * 
     * @return bool true if neuron should fire
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the timestamp of last spike
     * 
     * @return float Last spike time (-1.0f if never spiked)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Performs one LIF integration step
     * 
     * Implements the Leaky Integrate-and-Fire mathematical model:
     * dV/dt = (V_rest - V)/tau + I/C
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Time step (ms)
     * @return bool true if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receives excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (nA)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (nA)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory input (neuromodulators, etc.)
     * 
     * @param amplitude Modulatory input amplitude (nA)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects external current into neuron
     * 
     * Used for sensory input or external stimulation.
     * 
     * @param current External current (nA)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets total injected current for the current step
     * 
     * @return MembranePotential Total injected current (nA)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears the current injection accumulator
     * 
     * Called at the start of each simulation step to prepare
     * for new input during the upcoming timestep.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike event
     * 
     * @param timestamp Time of spike (ms)
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets recent spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike times
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds incoming synaptic connection
     * 
     * @param handle Handle to the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds outgoing synaptic connection
     * 
     * @param handle Handle to the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets list of incoming synapses
     * 
     * @return const std::vector<SynapseHandle>& Incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets list of outgoing synapses
     * 
     * @return const std::vector<SynapseHandle>& Outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Flags indicating enabled plasticity types
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the neuron's region identifier
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the neuron's region identifier
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the neuron's population identifier
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the neuron's population identifier
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Performs one simulation step with default timestep
     * 
     * @param currentTime Current simulation time (ms)
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets neuron to initial state
     * 
     * Restores all state variables to initial values as if
     * the neuron were just created.
     */
    void reset();
    
    /**
     * @brief Initializes neuron parameters with random values
     * 
     * Sets biologically realistic initial values using the provided
     * random number generator for stochastic initialization.
     * 
     * @param rng Random number generator
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief PImpl pattern implementation details
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

