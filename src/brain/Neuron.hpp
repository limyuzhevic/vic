#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @file Neuron.hpp
 * @brief Header file for the Neuron class implementing spiking neural dynamics
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the Neuron class definition, which implements a single
 * spiking neuron using the Leaky Integrate-and-Fire (LIF) model. Neurons
 * form the fundamental computational units of the neural system, processing
 * inputs and generating spike outputs that drive network activity.
 */

/**
 * @struct NeuronState
 * @brief Internal state representation for Neuron objects
 * @since Version 1.0 (2024)
 *
 * Contains all internal variables needed to simulate neuron dynamics.
 * Stored as a struct for efficient memory access and cache locality.
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
 * @brief Single spiking neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The Neuron class represents a single computational unit in the neural system.
 * It implements the biologically-inspired Leaky Integrate-and-Fire model,
 * which approximates the membrane potential dynamics of real neurons. The
 * neuron integrates incoming synaptic inputs and emits spikes when the
 * membrane potential reaches a threshold.
 *
 * Key features:
 * - LIF dynamics with exponential membrane potential integration
 * - Refractory period to prevent unrealistic firing rates
 * - Spike-frequency adaptation
 * - Multiple input modalities (excitatory, inhibitory, modulatory)
 * - Membership in neural regions and populations
 *
 * @note This class uses the Pimpl idiom for implementation hiding, improving
 * compilation times and encapsulation. All state variables are stored in
 * the NeuronState struct.
 *
 * @warning Neurons should not be directly manipulated; use provided interfaces
 * for state management to ensure consistency.
 *
 * @see Brain, NeuralRegion, NeuralPopulation, Synapse
 */
class Neuron {
public:
    // Create neuron with ID
    /**
     * @brief Constructor for the Neuron class
     * @param id Unique identifier for this neuron
     * @post Neuron is created with default parameters and resting state
     * @note Neuron ID should be unique within the neural system
     * @warning ID collisions will cause undefined behavior
     * @see getId()
     */
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor (deleted)
     * @warning Neurons cannot be copied due to pointer-based internal representation
     * @note Memory management handled through pImpl pattern
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Neuron object to move from
     * @post This neuron owns other's resources; other is in valid but unspecified state
     * @note Enables efficient transfer of neuron ownership
     * @see operator=(Neuron&&)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Neuron object to move from
     * @return Reference to this neuron
     * @post Current neuron's resources are transferred from other
     * @note Enables efficient transfer of neuron ownership
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this neuron
     * @return NeuronId Unique identifier
     * @post Identifier remains constant throughout neuron's lifetime
     * @note Used for indexing and referencing in network operations
     */
    NeuronId getId() const;
    
    // Type
    /**
     * @brief Get the neuron type
     * @return NeuronType Type classification (sensory, motor, interneuron, etc.)
     * @post Type determines neuron's functional role in the network
     * @see setType(), NeuronType
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * @param type Neuron type to set
     * @post Neuron type is updated; may affect network behavior
     * @note Type affects connectivity patterns and processing roles
     * @see getType()
     */
    void setType(NeuronType type);
    
    // State access
    /**
     * @brief Get constant reference to neuron state
     * @return const NeuronState& Constant reference to internal state
     * @post Provides read-only access to all neuron internal variables
     * @note State includes membrane potential, firing rate, refractory period
     * @see NeuronState, getState()
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     * @return NeuronState& Mutable reference to internal state
     * @post Allows modification of internal neuron state
     * @warning Direct state modification can cause inconsistencies
     * @note Provided for advanced use cases and specialized operations
     */
    NeuronState& getState();
    
    // Membrane potential
    /**
     * @brief Get current membrane potential
     * @return MembranePotential Current membrane potential in mV
     * @post Membrane potential reflects recent neural activity
     * @note Typical values: resting ~ -70mV, threshold ~ -55mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * @param potential New membrane potential in mV
     * @post Membrane potential is set to specified value
     * @warning Direct setting may bypass important biophysical constraints
     * @see addToMembranePotential(), getMembranePotential()
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add delta to current membrane potential
     * @param delta Change in membrane potential (mV)
     * @post Membrane potential is increased by delta
     * @note More biologically realistic than direct setting
     * @see setMembranePotential(), getMembranePotential()
     */
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    /**
     * @brief Get firing threshold
     * @return MembranePotential Firing threshold in mV
     * @post Threshold determines when neuron will fire
     * @note Must be higher than resting potential for stable operation
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * @param threshold New firing threshold in mV
     * @post Threshold is updated
     * @warning Changing threshold affects neuron excitability
     * @see getThreshold(), checkThreshold()
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    /**
     * @brief Check if neuron is currently firing
     * @return bool true if neuron is in firing state, false otherwise
     * @post Returns current firing state
     * @note Used to detect neural activity events
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return bool true if neuron cannot fire, false otherwise
     * @post Returns current refractory state
     * @note Refractory period prevents unrealistic firing rates
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * @param state New firing state
     * @post Firing state is updated
     * @note Affects neuron's ability to fire and integration properties
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * @param steps Number of simulation steps for refractory period
     * @post Refractory period is updated
     * @note Longer refractory periods reduce maximum firing rate
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter
     * @post Refractory timer is decreased by one step
     * @note Called automatically during simulation steps
     */
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    /**
     * @brief Get current firing rate
     * @return FiringRate Current firing rate in Hz
     * @post Returns average firing rate over recent history
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate
     * @param rate New firing rate in Hz
     * @post Firing rate is set (affects integration if rate-based)
     */
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    /**
     * @brief Set leak conductance
     * @param conductance Leak conductance in nS
     * @post Leak conductance is updated
     * @note Controls membrane time constant and integration speed
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * @return MembranePotential Leak conductance in nS
     * @post Returns current leak conductance value
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period
     * @return uint32_t Refractory period in simulation steps
     * @post Returns current refractory period setting
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential
     * @param potential Resting potential in mV
     * @post Resting potential is updated
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * @return MembranePotential Resting potential in mV
     * @post Returns current resting potential
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential
     * @param potential Reset potential in mV
     * @post Reset potential is updated
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get reset potential
     * @return MembranePotential Reset potential in mV
     * @post Returns current reset potential
     */
    MembranePotential getResetPotential() const;
    
    // Spike detection
    /**
     * @brief Check if membrane potential exceeds threshold
     * @return bool true if neuron should fire, false otherwise
     * @post Checks threshold condition without changing state
     * @note Used in stepLIF() function
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get time of last spike
     * @return float Timestamp of last spike in simulation time
     * @post Returns time of most recent spike
     * @note Returns -1.0 if neuron has never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Execute Leaky Integrate-and-Fire step
     * @param currentTime Current simulation time
     * @param dt Timestep duration in simulation steps
     * @return bool true if neuron fired, false otherwise
     * @post Updates neuron state according to LIF dynamics
     * @note Core computation method for neuron simulation
     * @warning Must be called at appropriate simulation timestep
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    /**
     * @brief Receive excitatory input
     * @param amplitude Synaptic current amplitude in nA
     * @post Membrane potential is increased by excitatory input
     * @note Represents glutamatergic or other excitatory transmission
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory input
     * @param amplitude Synaptic current amplitude in nA
     * @post Membrane potential is decreased by inhibitory input
     * @note Represents GABAergic or other inhibitory transmission
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory input
     * @param amplitude Synaptic current amplitude in nA
     * @post Membrane potential is modulated by neuromodulatory input
     * @note Represents dopaminergic, serotonergic, or other modulatory transmission
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    /**
     * @brief Inject current directly into neuron
     * @param current Current to inject in nA
     * @post Neuron membrane potential is increased by injected current
     * @note External current injection (e.g., from brain stimulation)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total injected current
     * @return MembranePotential Total injected current in nA
     * @post Returns sum of all injected currents
     * @note Separate from synaptic currents
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear total injected current
     * @post Total current is reset to zero
     * @note Should be called after processing injected currents
     */
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    /**
     * @brief Record spike event
     * @param timestamp Time when spike occurred
     * @post Spike timestamp is added to spike history
     * @note Used for spike-timing-dependent plasticity (STDP)
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * @post Returns all recorded spike timestamps
     * @note History is cleared when clearSpikeHistory() is called
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @post All spike timestamps are removed
     * @note Used to reset spike recording
     */
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    /**
     * @brief Add incoming synapse
     * @param handle Synapse handle for incoming connection
     * @post Incoming synapse is added to connection list
     * @note Used for STDP and plasticity computations
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse
     * @param handle Synapse handle for outgoing connection
     * @post Outgoing synapse is added to connection list
     * @note Used for backward connections and information flow
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapses
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     * @post Returns all incoming synaptic connections
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapses
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     * @post Returns all outgoing synaptic connections
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    /**
     * @brief Get plasticity flags
     * @return const PlasticityFlags& Reference to plasticity flags
     * @post Returns plasticity rules enabled for this neuron
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * @post Allows modification of plasticity rules
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity rules
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @post Plasticity flags are updated
     * @note Controls which learning rules apply to this neuron
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    /**
     * @brief Set region ID
     * @param region Region identifier
     * @post Neuron is assigned to specified region
     * @note Used for organizing neurons into functional groups
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region ID
     * @return RegionId Region identifier
     * @post Returns current region assignment
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population ID
     * @param population Population identifier
     * @post Neuron is assigned to specified population
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID
     * @return PopulationId Population identifier
     * @post Returns current population assignment
     */
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    /**
     * @brief Update neuron state for one simulation step
     * @param currentTime Current simulation time
     * @post Neuron state is updated according to connected dynamics
     * @note Phase 2: Implement real integrate-and-fire dynamics
     * @todo Implement proper LIF integration with continuous time
     * @bug Current implementation is a placeholder
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset neuron to initial state
     * @post All state variables are reset to initial values
     * @note Clears firing state, membrane potential, and history
     * @warning Loses all spike history and learned connections
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize neuron with random parameters
     * @param rng Reference to random number generator
     * @post Neuron parameters (threshold, conductance, etc.) are randomized
     * @note Used for generating diverse neural populations
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm