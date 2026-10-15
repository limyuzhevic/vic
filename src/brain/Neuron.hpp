#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <memory>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Neuron class representing a single neuron
 * 
 * @details Implements Leaky Integrate-and-Fire (LIF) dynamics for single neurons
 *          with support for synaptic inputs, plasticity, and adaptive properties.
 *          The Neuron class is the fundamental computational unit of the neural system,
 *          capable of firing action potentials and participating in plasticity mechanisms.
 * 
 * @note Thread-unsafe access requires external synchronization
 * @warning Must be initialized with proper parameters before use
 * @ingroup Neuronal Dynamics
 */
class Neuron {
/**
     * @brief Constructor - Create neuron with unique ID
     * 
     * @details Creates a new neuron with the specified ID and initializes
     *          it to internal neuron type with default parameters. The neuron
     *          is ready for synaptic connections and simulation updates.
     * 
     * @param id Unique identifier for this neuron. Must be unique within
     *           its region and globally across all neurons.
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @note Neuron ID should be valid and unique before neuron is added to region
     * @warning ID must be registered with region before adding connections
     * @thread_safety Thread-unsafe initialization
     * @post Neuron is created and ready for configuration
     * @ingroup Neuronal Dynamics
     */
    explicit Neuron(NeuronId id);

    /**
     * @brief Destructor - Clean up neuron resources
     * 
     * @details Releases all allocated memory and cleans up neuron state.
     *          Performs cleanup of spike history, synaptic connections,
     *          and internal implementation data.
     * 
     * @note All connections should be properly handled before neuron destruction
     * @thread_safety Thread-safe if no concurrent operations
     * @post Neuron resources are freed
     */
    ~Neuron();

    /**
     * @brief Copy constructor (deleted)
     * 
     * @details Copy construction is not supported to prevent accidental copying
     *          and maintain object ownership semantics.
     * 
     * @note Neuron contains non-copyable resources (unique_ptr)
     * @warning Never use copy constructor - explicitly deleted
     */
    Neuron(const Neuron&) = delete;

    /**
     * @brief Copy assignment operator (deleted)
     * 
     * @details Copy assignment is not supported for the same reasons as copy constructor.
     * 
     * @note Prevents shallow copying of internal resources
     * @warning Never use copy assignment - explicitly deleted
     */
    Neuron& operator=(const Neuron&) = delete;

    /**
     * @brief Move constructor (noexcept)
     * 
     * @details Move constructor allows efficient transfer of neuron ownership
     *          by moving the internal pImpl pointer. The source neuron becomes
     *          invalid after move operation.
     * 
     * @param other Neuron to move from
     * 
     * @note Source neuron becomes unusable after move
     * @thread_safety Exception-safe
     * @post Source neuron is in valid but unspecified state
     * @ingroup Neuronal Dynamics
     */
    Neuron(Neuron&&) noexcept;

    /**
     * @brief Move assignment operator (noexcept)
     * 
     * @details Move assignment allows efficient transfer of neuron ownership,
     *          properly cleaning up existing resources before transferring.
     * 
     * @param other Neuron to move from
     * 
     * @return Reference to this neuron after assignment
     * @note Source neuron becomes unusable after move
     * @thread_safety Exception-safe, may throw
     * @post Source neuron is in valid but unspecified state
     * @ingroup Neuronal Dynamics
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron ID
     * 
     * @details Retrieves the unique identifier of this neuron, which is used
     *          for addressing this neuron in connections and state queries.
     * 
     * @return NeuronId Identifier of this neuron
     * 
     * @note ID is immutable after construction
     * @thread_safety Thread-safe - read-only access
     * @return Unique neuron identifier
     * @ingroup Neuronal Dynamics
     */
    NeuronId getId() const;

    /**
     * @brief Get neuron type
     * 
     * @details Retrieves the functional type of this neuron (Sensory, Motor,
     *          or Internal). Neuron type affects its behavior and role in
     *          the neural network.
     * 
     * @return NeuronType Type of this neuron
     * 
     * @note Type can be modified for flexibility in network construction
     * @thread_safety Thread-safe - read-only access
     * @return Neuron type
     * @ingroup Neuronal Dynamics
     */
    NeuronType getType() const;

    /**
     * @brief Set neuron type
     * 
     * @details Changes the functional type of this neuron. This affects
     *          neuron's behavior, plasticity rules, and processing role.
     *          Should be set before neuron is added to simulation.
     * 
     * @param type New neuron type to set
     * 
     * @throws std::runtime_error if neuron is in simulation
     * @note Type change affects plasticity and processing behavior
     * @thread_safety Thread-unsafe if neuron is in simulation
     * @post Neuron type is updated
     * @ingroup Neuronal Dynamics
     */
    void setType(NeuronType type);

    /**
     * @brief Get neuron state (const version)
     * 
     * @details Retrieves complete neuron state including membrane potential,
     *          firing state, refractory period, and all membrane properties.
     *          This provides read-only access to neuron internal state.
     * 
     * @return Const reference to NeuronState structure containing all state
     * 
     * @note State is updated during simulation steps
     * @thread_safety Thread-safe - read-only access
     * @return Complete neuron state
     * @ingroup Neuronal Dynamics
     */
    const NeuronState& getState() const;

    /**
     * @brief Get neuron state (non-const version)
     * 
     * @details Retrieves mutable reference to neuron state, allowing direct
     *          modification of neuron properties. Use with caution as this
     *          bypasses simulation update mechanisms.
     * 
     * @return Reference to NeuronState structure containing all state
     * 
     * @note Use getState() for read-only access, this for direct modification
     * @thread_safety Thread-unsafe - modification requires external sync
     * @return Mutable neuron state reference
     * @ingroup Neuronal Dynamics
     */
    NeuronState& getState();

    /**
     * @brief Get membrane potential
     * 
     * @details Retrieves current membrane potential in millivolts (mV).
     *          This is the primary variable governing neuron excitability and
     *          firing behavior.
     * 
     * @return MembranePotential Current membrane potential
     * 
     * @note Values typically range from -100mV (hyperpolarized) to +50mV (depolarized)
     * @thread_safety Thread-safe - read-only access
     * @return Membrane potential in mV
     * @ingroup Neuronal Dynamics
     */
    MembranePotential getMembranePotential() const;

    /**
     * @brief Set membrane potential
     * 
     * @details Direct modification of membrane potential. This bypasses the
     *          normal integration dynamics and should be used with care.
     *          Typically used for testing or reset operations.
     * 
     * @param potential New membrane potential in mV
     * 
     * @note This does not affect firing state or refractory period
     * @thread_safety Thread-unsafe - bypasses normal integration
     * @post Membrane potential is set directly
     * @ingroup Neuronal Dynamics
     */
    void setMembranePotential(MembranePotential potential);

    /**
     * @brief Add to membrane potential
     * 
     * @details Incrementally changes membrane potential by the specified delta.
     *          This simulates the effect of synaptic inputs or external current.
     * 
     * @param delta Amount to change membrane potential (positive for depolarization,
     *              negative for hyperpolarization)
     * 
     * @note This is the primary way neurons respond to inputs
     * @thread_safety Thread-unsafe if concurrent with other membrane operations
     * @post Membrane potential is changed by delta
     * @ingroup Neuronal Dynamics
     */
    void addToMembranePotential(MembranePotential delta);

    /**
     * @brief Get firing threshold
     * 
     * @details Retrieves the membrane potential threshold at which the neuron
     *          will fire an action potential. This determines neuronal excitability.
     * 
     * @return MembranePotential Firing threshold in mV
     * 
     * @note Typical threshold is around -55mV for biological neurons
     * @thread_safety Thread-safe - read-only access
     * @return Threshold in mV
     * @ingroup Neuronal Dynamics
     */
    MembranePotential getThreshold() const;

    /**
     * @brief Set firing threshold
     * 
     * @details Changes the firing threshold. Higher thresholds make neurons
     *          less excitable, lower thresholds make them more excitable.
     *          Should be set before neuron is added to simulation.
     * 
     * @param threshold New firing threshold in mV
     * 
     * @throws std::runtime_error if neuron is in simulation
     * @note Affects neuronal excitability and firing dynamics
     * @thread_safety Thread-unsafe if neuron is in simulation
     * @post Threshold is updated
     * @ingroup Neuronal Dynamics
     */
    void setThreshold(MembranePotential threshold);

    /**
     * @brief Check if neuron is currently firing
     * 
     * @details Determines if the neuron is in active firing state (has just
     *          spiked or is in refractory period following a spike).
     * 
     * @return bool true if neuron is firing, false otherwise
     * 
     * @note Used to detect recent spiking activity
     * @thread_safety Thread-safe - read-only access
     * @return Firing status
     * @ingroup Neuronal Dynamics
     */
    bool isFiring() const;

    /**
     * @brief Check if neuron is in refractory period
     * 
     * @details Determines if the neuron is in its refractory period following
     *          a spike. During refractory period, the neuron cannot fire again.
     * 
     * @return bool true if neuron is refractory, false otherwise
     * 
     * @note Refractory period prevents unrealistic firing rates
     * @thread_safety Thread-safe - read-only access
     * @return Refractory status
     * @ingroup Neuronal Dynamics
     */
    bool isRefractory() const;

    /**
     * @brief Set firing state
     * 
     * @details Directly sets the firing state of the neuron. This bypasses
     *          the normal state transitions and should be used with caution.
     *          Typically used for testing or reset operations.
     * 
     * @param state Target firing state
     * 
     * @note This does not update spike timing or refractory period
     * @thread_safety Thread-unsafe - bypasses normal state machine
     * @post Firing state is set directly
     * @ingroup Neuronal Dynamics
     */
    void setFiringState(FiringState state);

    /**
     * @brief Set refractory period
     * 
     * @details Changes the duration of the refractory period after each spike.
     *          Longer refractory periods reduce firing rates.
     * 
     * @param steps Number of simulation steps for refractory period
     * 
     * @note Should be set before neuron is added to simulation
     * @thread_safety Thread-unsafe if neuron is in simulation
     * @post Refractory period duration is updated
     * @ingroup Neuronal Dynamics
     */
    void setRefractoryPeriod(uint32_t steps);

    /**
     * @brief Decrement refractory counter
     * 
     * @details Decrements the remaining refractory steps by one. This is
     *          called automatically during simulation steps to progress
     *          from refractory to resting state.
     * 
     * @note This is called automatically by simulation, user intervention
     *       should only be for testing or debugging
     * @thread_safety Thread-unsafe - called by simulation
     * @post Refractory remaining is decremented
     * @ingroup Neuronal Dynamics
     */
    void decrementRefractory();

    /**
     * @brief Get firing rate
     * 
     * @details Retrieves the current firing rate in Hertz (spikes per second).
     *          This is a derived quantity based on recent spiking activity.
     * 
     * @return FiringRate Current firing rate
     * 
     * @note This is a computed value, not a stored state variable
     * @thread_safety Thread-safe - read-only access
     * @return Firing rate in Hz
     * @ingroup Neuronal Dynamics
     */
    FiringRate getFiringRate() const;

    /**
     * @brief Set firing rate
     * 
     * @details Directly sets the firing rate. This bypasses the normal
     *          integration-based firing rate calculation and should be used
     *          with caution.
     * 
     * @param rate New firing rate in Hz
     * 
     * @note This is a computed value in normal operation
     * @thread_safety Thread-unsafe - bypasses normal computation
     * @post Firing rate is set directly
     * @ingroup Neuronal Dynamics
     */
    void setFiringRate(FiringRate rate);

    /**
     * @brief Set leak conductance
     * 
     * @details Sets the leak conductance parameter that controls how quickly
     *          the membrane potential returns to resting potential in the
     *          absence of inputs. Higher conductance means faster membrane
     *          charging.
     * 
     * @param conductance Leak conductance in nS (nanoSiemens)
     * 
     * @note Typical value is around 10 nS for biological neurons
     * @thread_safety Thread-unsafe if neuron is in simulation
     * @post Leak conductance is updated
     * @ingroup Neuronal Dynamics
     */
    void setLeakConductance(MembranePotential conductance);

    /**
     * @brief Get leak conductance
     * 
     * @details Retrieves the leak conductance parameter.
     * 
     * @return MembranePotential Leak conductance in nS
     * 
     * @note Controls membrane time constant
     * @thread_safety Thread-safe - read-only access
     * @return Leak conductance in nS
     * @ingroup Neuronal Dynamics
     */
    MembranePotential getLeakConductance() const;

    /**
     * @brief Get refractory period
     * 
     * @details Retrieves the current refractory period duration.
     * 
     * @return uint32_t Refractory period in simulation steps
     * 
     * @note Controls minimum inter-spike interval
     * @thread_safety Thread-safe - read-only access
     * @return Refractory period in steps
     * @ingroup Neuronal Dynamics
     */
    uint32_t getRefractoryPeriod() const;

    /**
     * @brief Set resting potential
     * 
     * @details Sets the resting membrane potential, which is the baseline
     *          potential to which the neuron tends in the absence of inputs.
     * 
     * @param potential Resting potential in mV
     * 
     * @note Typical value is -70mV for biological neurons
     * @thread_safety Thread-unsafe if neuron is in simulation
     * @post Resting potential is updated
     * @ingroup Neuronal Dynamics
     */
    void setRestingPotential(MembranePotential potential);

    /**
     * @brief Get resting potential
     * 
     * @details Retrieves the resting membrane potential.
     * 
     * @return MembranePotential Resting potential in mV
     * 
     * @note This is the baseline membrane potential
     * @thread_safety Thread-safe - read-only access
     * @return Resting potential in mV
     * @ingroup Neuronal Dynamics
     */
    MembranePotential getRestingPotential() const;

    /**
     * @brief Set reset potential
     * 
     * @details Sets the reset potential, which is the membrane potential value
     *          to which the neuron is reset after firing a spike. This typically
     *          represents the afterhyperpolarization potential.
     * 
     * @param potential Reset potential in mV
     * 
     * @note Typically slightly more negative than resting potential
     * @thread_safety Thread-unsafe if neuron is in simulation
     * @post Reset potential is updated
     * @ingroup Neuronal Dynamics
     */
    void setResetPotential(MembranePotential potential);

    /**
     * @brief Get reset potential
     * 
     * @details Retrieves the reset potential.
     * 
     * @return MembranePotential Reset potential in mV
     * 
     * @note This is the post-spike reset value
     * @thread_safety Thread-safe - read-only access
     * @return Reset potential in mV
     * @ingroup Neuronal Dynamics
     */
    MembranePotential getResetPotential() const;

    /**
     * @brief Check threshold
     * 
     * @details Checks if the current membrane potential has reached or exceeded
     *          the firing threshold, indicating the neuron is ready to fire.
     *          This is used during simulation to detect spiking.
     * 
     * @return bool true if membrane potential >= threshold
     * 
     * @note This is the primary spike detection method
     * @thread_safety Thread-safe - read-only access
     * @return Threshold crossing status
     * @ingroup Neuronal Dynamics
     */
    bool checkThreshold() const;

    /**
     * @brief Get last spike time
     * 
     * @details Retrieves the timestamp of the most recent spike. Returns -1.0f
     *          if the neuron has never spiked.
     * 
     * @return float Timestamp of last spike in simulation time, or -1.0f if never spiked
     * 
     * @note Used for spike timing-dependent plasticity
     * @thread_safety Thread-safe - read-only access
     * @return Last spike time or -1.0f
     * @ingroup Neuronal Dynamics
     */
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
