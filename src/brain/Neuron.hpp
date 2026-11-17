#pragma once

/**
 * @file Neuron.hpp
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation for NLM
 * 
 * This file defines the Neuron class, which implements a biologically inspired
 * spiking neural model using Leaky Integrate-and-Fire dynamics. Neurons are the
 * fundamental computational units in the NLM brain, processing inputs and
 * generating output spikes that drive learning and behavior.
 * 
 * The LIF model captures essential properties of real neurons:
 * - Membrane potential integration of synaptic inputs
 * - Exponential leak towards resting potential
 * - Threshold-based spike generation
 * - Refractory period after spiking
 * - Spike-frequency adaptation
 * 
 * Each neuron belongs to a specific type (excitatory, inhibitory, sensory, etc.)
 * and participates in neural populations within regions of the brain.
 */

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * @brief Forward declarations for neuron-related classes
 */
class Synapse;
class RandomGenerator;

/**
 * @brief Complete neuron state for real-time simulation
 * 
 * This structure contains all the dynamic state variables needed to simulate
 * a single LIF neuron. The state is updated on each simulation timestep
 * to reflect the neuron's current activity and inputs.
 * 
 * @note This structure is copied for performance reasons - neurons use
 *       copy-on-write patterns in some contexts to minimize memory allocation.
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
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none
    
    /**
     * @brief Default constructor - initializes neuron to resting state
     * 
     * All neurons start at resting potential with default parameters:
     * - Membrane potential: -70 mV (typical resting)
     * - Threshold: -55 mV (typical excitation threshold)
     * - Refractory period: 9 steps (Phase 6 adjustment for burst behavior)
     * - No adaptation or recent spikes
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
        // Phase 6: 9 steps, not 5. One action presentation is 10
        // brain steps, so a 5-step refractory let a neuron fire
        // twice inside a single burst; 9 steps caps it at about
        // one spike per presentation, which is the population's
        // natural burst rhythm anyway. This lowers the reported
        // rate without changing any drive constant (drive
        // scaling was tried and reverted: -30% drive silenced
        // the network entirely, 0 Hz, because this is a
        // threshold system with no gentle middle ground).
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @brief Leaky Integrate-and-Fire (LIF) neuron model
 * 
 * This class implements a single spiking neuron that can process synaptic inputs,
 * integrate them over time, and generate action potentials (spikes) when the
 * membrane potential reaches threshold. The neuron follows biophysical dynamics
 * with exponential integration and refractory periods.
 * 
 * @details The neuron uses a Pimpl pattern (pointer to implementation) to
 *          reduce compilation dependencies and improve compilation speed.
 *          All heavy-weight operations are implemented in the .cpp file.
 * 
 * @note This neuron model is inspired by biological neurons but simplified
 *       for computational efficiency and to focus on system-level behavior
 *       rather than detailed biophysics.
 */
class Neuron {
public:
    /**
     * @brief Constructor
     * 
     * Creates a new neuron with a unique identifier. The neuron starts in a
     * resting state with default parameters.
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (neurons cannot be copied)
     * 
     * Neurons contain unique IDs and cannot be meaningfully copied.
     * Use move operations instead if neuron ownership transfer is needed.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disable copy assignment (neurons cannot be assigned)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move construction (efficient transfer of ownership)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Enable move assignment (efficient transfer of ownership)
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return Neuron ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Set the neuron's type (Excitatory, Inhibitory, Sensory, etc.)
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's type
     * 
     * @return Neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Get mutable access to neuron state
     * 
     * @return Reference to neuron state (use carefully - state can change)
     */
    NeuronState& getState();
    
    /**
     * @brief Get const access to neuron state
     * 
     * @return Const reference to neuron state (safe for reading)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get membrane potential
     * 
     * @return Membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential (low-level operation)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to membrane potential (synaptic input)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * 
     * @return Threshold potential in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return true if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get last spike time
     * 
     * @return Timestamp of last spike, or -1.0 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Set firing state (internal use)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Get firing state
     * 
     * @return Current firing state
     */
    FiringState getFiringState() const;
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return true if neuron is in Active state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return true if neuron cannot fire yet
     */
    bool isRefractory() const;
    
    /**
     * @brief Set refractory period duration
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Get refractory period duration
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Decrement refractory period (called by simulation)
     */
    void decrementRefractory();
    
    /**
     * @brief Get firing rate (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate (for rate-based computation)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Get leak conductance
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Set leak conductance
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get resting potential
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set resting potential
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get reset potential
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Set reset potential
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get synaptic input
     * 
     * @return Accumulated synaptic input (nA)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Inject direct current (external input)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Clear synaptic input (end of integration period)
     */
    void clearTotalCurrent();
    
    /**
     * @brief Receive excitatory synaptic input
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory input (neuromodulators)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Record a spike event
     * 
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history (recent spikes for STDP)
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse connections
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse connections
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region membership
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region membership
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population membership
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population membership
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Perform one LIF simulation step
     * 
     * This is the core neuron computation: integrate inputs, update membrane
     * potential, check for threshold crossing, handle refractory period.
     * 
     * @param currentTime Current simulation time
     * @param dt Timestep duration (seconds)
     * @return true if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Perform default LIF step with standard timestep
     * 
     * Convenience method for 1ms timesteps used in most simulations.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * Sets initial membrane potential, thresholds, and other parameters
     * to biologically plausible random values based on the given RNG.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl implementation (pointer to implementation idiom)
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm