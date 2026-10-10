#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Neuron state structure for efficient storage and access
 * 
 * This structure contains all state variables needed for integrate-and-fire
 * neuron dynamics, organized for efficient memory access and computation.
 * Each neuron maintains its complete state in this structure for easy
 * access and modification by the brain system.
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
     * @note All state variables are set to their typical initial values
     *       for a standard integrate-and-fire neuron model.
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
 * @brief Individual neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * This class represents a single biological neuron model with realistic biophysical
 * properties and learning capabilities. Each neuron maintains its own state
 * and responds to synaptic inputs, generating spikes according to integrate-and-fire
 * dynamics. Neurons are the fundamental computational units of the NLM brain.
 * 
 * Key features:
 * - Realistic LIF dynamics with membrane potential integration
 * - Spike generation and refractory period handling
 * - Synaptic input processing (excitatory, inhibitory, modulatory)
 * - Plasticity rule application (STDP, Hebbian, reward-modulated)
 * - Developmental stage awareness and adaptation
 * - Region and population membership tracking
 * - Activity-dependent firing rate modulation
 */
class Neuron {
public:
    /**
     * @brief Construct a new Neuron with specified identifier
     * 
     * @param id Unique identifier for this neuron
     * 
     * @note The neuron is initialized to resting state with default
     *       physiological parameters. Further initialization (random parameters,
     *       synaptic connections) should be done through initialization methods.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron instance
     * 
     * @note Ensures proper cleanup of neuron state and resources.
     *       The neuron should be in a clean state (typically reset) before destruction.
     */
    ~Neuron();
    
    // Disable copying, enable moving for efficient resource management
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId The neuron's identifier
     * 
     * @note Used for neuron identification in brain-wide operations and
     *       to maintain the type-safe neural reference system.
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (Excitatory, Inhibitory, Modulatory, etc.)
     * 
     * @return NeuronType The neuron's functional type
     * 
     * @note Neuron type determines the neuron's behavior and role in the network.
     *       Affects synaptic transmission, plasticity eligibility, and computational role.
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * @param type New neuron type
     * 
     * @note Changing neuron type affects its behavior and interaction patterns.
     *       Use with caution as it may disrupt network dynamics.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state structure
     * 
     * @return const NeuronState& Reference to the neuron's state
     * 
     * @note Provides read-only access to the neuron's state. For modification,
     *       use specific setter methods or the non-const getState() overload.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to the neuron's state structure
     * 
     * @return NeuronState& Mutable reference to the neuron's state
     * 
     * @note Use with caution to avoid corrupting neuron state. Prefer specific
     *       setter methods for controlled state modification.
     */
    NeuronState& getState();
    
    // -------------------------------------------------------------------------
    // MEMBRANE DYNAMICS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential (mV)
     * 
     * @note Range: typically -80mV to +40mV. Firing threshold is usually around -55mV.
     *       Positive values indicate depolarization, negative values indicate hyperpolarization.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential (mV)
     * 
     * @note Use this method for controlled state setting or testing. For typical
     *       neuron operation, use synaptic inputs and step() methods instead.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a delta to the membrane potential (synaptic current input)
     * 
     * @param delta Change in membrane potential (mV)
     * 
     * @note Represents synaptic current injection. Positive values cause depolarization
     *       (excitation), negative values cause hyperpolarization (inhibition).
     *       This is the primary way neurons respond to synaptic inputs.
     */
    void addToMembranePotential(MembranePotential delta);
    
    // -------------------------------------------------------------------------
    // THRESHOLD AND FIRING CONTROL
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron's firing threshold
     * 
     * @return MembranePotential Firing threshold (mV)
     * 
     * @note Typically between -55mV and -40mV. When membrane potential exceeds
     *       this threshold, the neuron generates a spike and enters refractory period.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the neuron's firing threshold
     * 
     * @param threshold New firing threshold (mV)
     * 
     * @note Adjusting threshold affects neuron excitability and firing patterns.
     *       Higher thresholds make neurons harder to fire, lower thresholds make them more excitable.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing (above threshold)
     * 
     * @return bool true if neuron is firing, false otherwise
     * 
     * @note This method checks the current membrane potential against the
     *       firing threshold without modifying neuron state.
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * @return bool true if neuron is refractory, false otherwise
     * 
     * @note During refractory period, the neuron cannot fire spikes regardless
     *       of membrane potential. Prevents unrealistic firing patterns.
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state directly
     * 
     * @param state New firing state
     * 
     * @note Use this method for controlled state management. Normally,
     *       firing state changes automatically through step() methods.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the neuron's refractory period duration
     * 
     * @param steps Number of simulation steps for refractory period
     * 
     * @note Controls how long the neuron remains unable to fire after a spike.
     *       Longer refractory periods prevent high-frequency firing and allow
     *       recovery of synaptic resources.
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter by one step
     * 
     * @note This is typically called by the brain system during simulation steps
     *       to progress neurons from refractory to active states.
     */
    void decrementRefractory();
    
    // -------------------------------------------------------------------------
    // FIRING RATE DYNAMICS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron's current firing rate
     * 
     * @return FiringRate Current firing rate (Hz)
     * 
     * @note Represents the average firing rate over recent time. Used for
     *       rate-based computation and activity monitoring.
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the neuron's firing rate directly
     * 
     * @param rate New firing rate (Hz)
     * 
     * @note Used for rate-based neuron models or controlled testing scenarios.
     *       May produce unrealistic behavior if not coordinated with spike generation.
     */
    void setFiringRate(FiringRate rate);
    
    // -------------------------------------------------------------------------
    // LIF NEURON PARAMETERS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Set the neuron's leak conductance
     * 
     * @param conductance Leak conductance (nS)
     * 
     * @note Controls how quickly the neuron returns to resting potential
     *       in the absence of input. Higher conductance = faster decay.
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the neuron's leak conductance
     * 
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the neuron's refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the neuron's resting potential
     * 
     * @param potential New resting potential (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the neuron's resting potential
     * 
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the neuron's reset potential (post-spike)
     * 
     * @param potential New reset potential (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the neuron's reset potential
     * 
     * @return MembranePotential Reset potential (mV)
     */
    MembranePotential getResetPotential() const;
    
    // -------------------------------------------------------------------------
    // SPIKE DETECTION AND HISTORY
    // -------------------------------------------------------------------------
    
    /**
     * @brief Check if neuron has crossed firing threshold
     * 
     * @return bool true if neuron should fire this step, false otherwise
     * 
     * @note This method checks membrane potential against threshold without
     *       modifying neuron state. Called by stepLIF() to determine spike generation.
     *       
     * @see stepLIF() for the complete spike detection and generation process
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Timestamp of last spike (seconds), or -1.0 if no spikes yet
     * 
     * @note Used for spike timing-dependent plasticity (STDP) and
     *       activity-dependent computations.
     */
    float getLastSpikeTime() const;
    
    // -------------------------------------------------------------------------
    // LIF DYNAMICS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Execute one step of Leaky Integrate-and-Fire dynamics
     * 
     * @param currentTime Current simulation time (seconds)
     * @param dt Timestep duration (seconds)
     * @return bool true if neuron fired this step, false otherwise
     * 
     * @note This is the core computational method for LIF neurons. It:
     * 1. Integrates membrane potential over time (Leaky Integrate)
     * 2. Adds synaptic inputs
     * 3. Checks for threshold crossing
     * 4. Generates spike if threshold crossed
     * 5. Resets membrane potential if spike occurred
     * 6. Updates adaptation variables
     * 
     * @see step() for the general simulation step (Phase 3+)
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // -------------------------------------------------------------------------
    // SYNAPTIC INPUTS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Input amplitude (mV)
     * 
     * @note Excitatory inputs typically cause depolarization (membrane potential increase).
     *       This is usually the result of glutamatergic synaptic transmission.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Input amplitude (mV, typically negative)
     * 
     * @note Inhibitory inputs cause hyperpolarization (membrane potential decrease).
     *       This is usually the result of GABAergic synaptic transmission.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Input amplitude (mV)
     * 
     * @note Modulatory inputs affect plasticity, gain, or other internal properties.
     *       They may cause subtler changes compared to excitatory/inhibitory inputs.
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // -------------------------------------------------------------------------
    // EXTERNAL CURRENTS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Inject external current directly into the neuron
     * 
     * @param current Current value to inject (microsiemens)
     * 
     * @note External current represents inputs from outside the brain system,
     *       such as sensory-driven inputs or external control signals.
     *       Different from synaptic inputs in origin and control mechanism.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total external current injected into the neuron
     * 
     * @return MembranePotential Total injected current (microsiemens)
     * 
     * @note Sum of all external current injections since last clear.
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all external current injections
     * 
     * @note Resets the injected current accumulator to zero.
     *       External currents are typically cleared each simulation step.
     */
    void clearTotalCurrent();
    
    // -------------------------------------------------------------------------
    // PLASTICITY AND LEARNING
    // -------------------------------------------------------------------------
    
    /**
     * @brief Record a spike event for plasticity computations
     * 
     * @param timestamp Timestamp when the spike occurred (seconds)
     * 
     * @note Spike history is essential for spike-timing-dependent plasticity (STDP)
     *       and other timing-based learning mechanisms. Maintains both pre- and
     *       post-synaptic spike timing for plasticity rules.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the neuron's spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * 
     * @note Used by plasticity rules to compute timing relationships between
     *       pre- and post-synaptic activity. May be cleared during reset operations.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the neuron's spike history
     * 
     * @note Resets spike timing records. Called during reset() operations
     *       and when plasticity eligibility needs to be reset.
     */
    void clearSpikeHistory();
    
    // -------------------------------------------------------------------------
    // SYNAPSE MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Add an incoming synapse to the neuron
     * 
     * @param handle Synapse handle of the incoming connection
     * 
     * @note Updates internal synapse management for plasticity eligibility
     *       and activity-dependent computations. Tracks incoming connections
     *       for synaptic integration and plasticity rule application.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from the neuron
     * 
     * @param handle Synapse handle of the outgoing connection
     * 
     * @note Tracks outgoing connections for spike propagation and
     *       synaptic transmission responsibilities.
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     * 
     * @note Used for plasticity eligibility tracking and activity-dependent computations.
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     * 
     * @note Used for spike propagation and synaptic transmission coordination.
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // -------------------------------------------------------------------------
    // PLASTICITY STATE MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron's plasticity flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     * 
     * @note Plasticity flags determine which plasticity rules apply to this neuron,
     *       affecting synaptic learning and weight updates.
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to the neuron's plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * 
     * @note Allows direct modification of plasticity eligibility and rule activation.
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity rules for this neuron
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @note Controls which learning mechanisms can modify synaptic weights
     *       based on neural activity and external signals.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // -------------------------------------------------------------------------
    // DEVELOPMENT AND CLASSIFICATION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Set the neuron's region identifier
     * 
     * @param region Region identifier
     * 
     * @note Neurons belong to specific brain regions, which affects their
     *       computational properties and connectivity patterns. Region membership
     *       is used for developmental and regional computations.
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the neuron's region identifier
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the neuron's population identifier
     * 
     * @param population Population identifier
     * 
     * @note Neurons belong to neural populations, which share similar properties
     *       and dynamics. Population membership enables population-level computations.
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the neuron's population identifier
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    // -------------------------------------------------------------------------
    // SIMULATION OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update neuron state for one simulation step
     * 
     * @param currentTime Current simulation time (seconds)
     * 
     * @note This method implements the general simulation step for neurons
     *       in Phase 3+. It combines LIF dynamics, synaptic inputs, plasticity,
     *       and developmental changes into a comprehensive update.
     * 
     * @note Currently a placeholder for Phase 3 implementation
     * @see stepLIF() for the complete LIF implementation
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * @note Clears all state variables, spike history, and internal counters.
     *       Resets neuron to resting state with default parameters.
     *       Used to prepare neurons for new simulation runs.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * @param rng Reference to random number generator
     * 
     * @note Sets neuron-specific parameters (membrane potential, threshold,
     *       conductances, etc.) using random values from the provided generator.
     *       Used during brain initialization to create variability.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

} // namespace nlm