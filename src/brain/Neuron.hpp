#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
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
     * @brief Neuron class representing a single neuron in the neural network
     * 
     * Implements the Leaky Integrate-and-Fire (LIF) model for neural computation,
     * providing biologically realistic spiking dynamics with threshold-based firing.
     * Each neuron maintains internal state including membrane potential, synaptic
     * inputs, firing history, and plasticity flags.
     * 
     * Biological inspiration:
     * - LIF dynamics model real cortical neuron behavior
     * - Membrane potential integrates incoming currents
     * - Firing threshold leads to spike generation
     * - Refractory period prevents excessive firing rates
     * - Synaptic plasticity enables learning
     * 
     * Physical implementation:
     * - Neuron has individual state variables managed by internal implementation
     * - Fast state access methods for performance-critical paths
     * - Memory pools enable efficient allocation
     * - Thread-safe operations for parallel simulation
     */
class Neuron {
public:
    /**
     * @brief Construct a neuron with a unique identifier
     * 
     * @param id Unique identifier for this neuron within the brain
     * @pre id must be valid and not represent a reserved neuron ID
     * @post Neuron object is created with initial state and ID set
     * @post Membrane potential is initialized to resting potential (-70.0 mV)
     * @post All other state variables are set to default values
     * 
     * @note Neuron IDs should be unique within a brain region or across the brain
     * @see NeuronId for identifier structure
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the neuron and clean up associated resources
     * 
     * This destructor ensures proper cleanup of all neuron resources,
     * including removal from parent region/population and deallocation
     * of synaptic connections. Neuron state is preserved for debugging
     * if saving is enabled.
     * 
     * @post Neuron object is fully destroyed
     * @post All synaptic connections are removed from connected neurons
     * @post Neuron is removed from region/population membership lists
     * @post Memory pools are notified of deallocation
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return The neuron's identifier
     * @pre Neuron object must be valid
     * @post Identifier is returned unchanged
     * 
     * @note NeuronId is a strongly-typed wrapper for uint64_t
     * @see NeuronId for structure details
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type classification
     * 
     * Neurons are classified by their functional role in the network:
     * Excitatory, Inhibitory, Modulatory, Sensory, Motor, or Internal.
     * Type determines synaptic interactions and processing characteristics.
     * 
     * @return Current neuron type
     * @pre Neuron object must be valid
     * @post Type is returned unchanged
     * 
     * @see NeuronType for type definitions
     * @see setType() for modification
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type classification
     * 
     * Changing neuron type alters its functional role in the network,
     * affecting synaptic behavior and processing characteristics.
     * 
     * @param type The new neuron type to assign
     * @pre Neuron object must be valid
     * @post Neuron type is updated
     * 
     * @note Some type changes may require recomputation of synaptic weights
     * @see getType() for current type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the current state of the neuron
     * 
     * Returns a const reference to the neuron's state structure containing
     * all internal variables. This provides efficient read-only access to
     * membrane potential, synaptic conductances, firing state, and other
     * dynamic variables.
     * 
     * @return Const reference to the neuron state structure
     * @pre Neuron object must be valid
     * @post State reference is returned
     * 
     * @note State is updated by the step() function
     * @see NeuronState for state structure details
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable access to the neuron state structure
     * 
     * Returns a non-const reference to the neuron state structure, allowing
     * direct modification of state variables. Use with caution as improper
     * state modification can violate simulation invariants.
     * 
     * @return Non-const reference to the neuron state structure
     * @pre Neuron object must be valid
     * @post Mutable state reference is returned
     * 
     * @note Prefer using dedicated methods for state modification
     * @see getState() for const version
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * The membrane potential represents the electrical state of the neuron,
     * integrating incoming synaptic currents and driving firing decisions.
     * Normal values range from -70 mV (resting) to -55 mV (threshold).
     * 
     * @return Current membrane potential in millivolts
     * @pre Neuron object must be valid
     * @post Membrane potential value is returned
     * 
     * @see setMembranePotential() for modification
     * @see addToMembranePotential() for incremental changes
     * @see checkThreshold() for firing condition
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential to an absolute value
     * 
     * Direct membrane potential setting is used for initialization, testing,
     * or external control. This overrides the normal integration process.
     * 
     * @param potential The desired membrane potential in millivolts
     * @pre Neuron object must be valid
     * @post Membrane potential is set to specified value
     * 
     * @note May cause immediate threshold crossing if potential is too high
     * @see getMembranePotential() for reading
     * @see addToMembranePotential() for incremental changes
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add an increment to the membrane potential
     * 
     * This method supports synaptic integration by adding to the current
     * membrane potential. It preserves the membrane potential dynamics
     * and does not bypass the normal integration process.
     * 
     * @param delta The amount to add to membrane potential (mV)
     * @pre Neuron object must be valid
     * @post Membrane potential is increased by delta
     * 
     * @note Synaptic inputs call this method with excitatory or inhibitory values
     * @see setMembranePotential() for absolute setting
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * 
     * When the membrane potential reaches this threshold, the neuron will
     * fire a spike and reset its membrane potential. Typical value is -55 mV.
     * 
     * @return Firing threshold in millivolts
     * @pre Neuron object must be valid
     * @post Threshold value is returned
     * 
     * @see setThreshold() for modification
     * @see getMembranePotential() for comparison
     * @see checkThreshold() for threshold testing
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold voltage
     * 
     * Adjusting the threshold changes the neuron's excitability and firing rate.
     * Lower thresholds increase firing probability, higher thresholds decrease it.
     * 
     * @param threshold The new firing threshold in millivolts
     * @pre Neuron object must be valid
     * @post Threshold value is updated
     * 
     * @note Threshold changes affect firing probability
     * @see getThreshold() for current value
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing
     * 
     * Determines if the neuron has just fired a spike based on its firing state.
     * This is true when the neuron is in the Refractory firing state.
     * 
     * @return true if neuron is firing, false otherwise
     * @pre Neuron object must be valid
     * @post Returns current firing status
     * 
     * @note Firing status is updated by the step() function
     * @see isRefractory() for refractory state
     * @see setFiringState() for state modification
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * The refractory period is a recovery period after firing during which
     * the neuron cannot fire again. This period models the absolute refractory
     * period in real neurons.
     * 
     * @return true if neuron is in refractory period, false otherwise
     * @pre Neuron object must be valid
     * @post Returns current refractory status
     * 
     * @see decrementRefractory() for progression
     * @see setRefractoryPeriod() for configuration
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state
     * 
     * Direct firing state manipulation is provided for system control,
     * testing, and special processing scenarios. Normal operation uses
     * the step() function to update firing state based on membrane potential.
     * 
     * @param state The firing state to set
     * @pre Neuron object must be valid
     * @post Firing state is updated
     * 
     * @note May cause invalid state transitions if used improperly
     * @see FiringState for valid states
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the duration of the refractory period
     * 
     * The refractory period determines how long the neuron cannot fire after
     * generating a spike. Longer periods reduce maximum firing rates.
     * 
     * @param steps Number of simulation steps in the refractory period
     * @pre Neuron object must be valid
     * @post Refractory period duration is updated
     * 
     * @note Default value is 5 steps (BrainConstants.hpp)
     * @see isRefractory() for status checking
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the refractory period counter
     * 
     * This method is called during the step() function to progress the neuron's
     * recovery from firing. It decreases the remaining refractory steps by 1.
     * 
     * @pre Neuron object must be valid
     * @post Remaining refractory steps are decremented
     * 
     * @note Called automatically during simulation
     * @see isRefractory() for status
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate of the neuron
     * 
     * Firing rate represents the neuron's activity level in Hertz, providing
     * a continuous measure of neural activity complementary to the binary
     * spiking output. This is useful for rate-based neural computations.
     * 
     * @return Current firing rate in Hz
     * @pre Neuron object must be valid
     * @post Firing rate is returned
     * 
     * @see setFiringRate() for modification
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the neuron's firing rate
     * 
     * Direct firing rate setting is used for initialization, testing, or
     * rate-based neural processing. This overrides the normal spiking-based
     * firing rate calculation.
     * 
     * @param rate The firing rate in Hz
     * @pre Neuron object must be valid
     * @post Firing rate is set
     * 
     * @note May conflict with spike-based firing rate calculation
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance parameter
     * 
     * Leak conductance controls the rate at which membrane potential
     * decays toward resting potential. Higher conductance means faster
     * decay. Typical value is 10.0 nS.
     * 
     * @param conductance The leak conductance in nanoSiemens
     * @pre Neuron object must be valid
     * @post Leak conductance is updated
     * 
     * @note Affects membrane time constant
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance parameter
     * 
     * @return Current leak conductance in nanoSiemens
     * @pre Neuron object must be valid
     * @post Value is returned
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the current refractory period duration
     * 
     * @return Refractory period in simulation steps
     * @pre Neuron object must be valid
     * @post Duration is returned
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential voltage
     * 
     * Resting potential is the equilibrium voltage to which the neuron
     * naturally decays when no inputs are present. Typical value is -70 mV.
     * 
     * @param potential The resting potential in millivolts
     * @pre Neuron object must be valid
     * @post Resting potential is updated
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential voltage
     * 
     * @return Resting potential in millivolts
     * @pre Neuron object must be valid
     * @post Value is returned
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential voltage
     * 
     * Reset potential is the membrane potential value to which the neuron
     * is set after firing a spike. Typical value is -70 mV (same as resting).
     * 
     * @param potential The reset potential in millivolts
     * @pre Neuron object must be valid
     * @post Reset potential is updated
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential voltage
     * 
     * @return Reset potential in millivolts
     * @pre Neuron object must be valid
     * @post Value is returned
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if membrane potential has reached firing threshold
     * 
     * This method determines if the neuron should fire a spike based on
     * its current membrane potential and threshold. It does not modify
     * any state.
     * 
     * @return true if membrane potential >= threshold, false otherwise
     * @pre Neuron object must be valid
     * @post Returns threshold check result
     * 
     * @note This is a read-only check that doesn't affect neuron state
     * @see getMembranePotential() for current potential
     * @see getThreshold() for firing threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * Returns the simulation time when the neuron last fired, or -1.0 if
     * the neuron has never fired. This is used for STDP and other timing-based
     * plasticity mechanisms.
     * 
     * @return Last spike timestamp in seconds, or -1.0 if never fired
     * @pre Neuron object must be valid
     * @post Last spike time is returned
     * 
     * @note Value is -1.0 initially and when not in firing state
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform LIF (Leaky Integrate-and-Fire) computation for one timestep
     * 
     * This method implements the core LIF dynamics, updating the membrane potential
     * based on the differential equation and determining whether the neuron fires.
     * It provides the fundamental neural computation for the LIF model.
     * 
     * Mathematical model (per neuron):
     *   dV/dt = (V_rest - V)/tau + I/C
     *   V_new = V + dt * 1000 * ((V_rest - V)/tau + I/C)
     *   if V_new >= threshold:
     *       spike = true
     *       V = V_reset
     *       enter refractory period
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Timestep duration in seconds
     * @return true if neuron fired a spike, false otherwise
     * @pre Neuron object must be valid
     * @post Membrane potential is updated according to LIF dynamics
     * @post Firing state may be updated
     * @post Refractory period may be entered
     * 
     * @note This is the fundamental neural computation method
     * @see step() for high-level simulation step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * Excitatory inputs depolarize the neuron, increasing the membrane
     * potential and making firing more likely. Typical synaptic weights
     * range from 0.1 to 10.0 mV.
     * 
     * @param amplitude The synaptic input amplitude in millivolts
     * @pre Neuron object must be valid
     * @post Membrane potential is increased by amplitude
     * 
     * @note Positive amplitude increases firing probability
     * @see receiveInhibitoryInput() for inhibitory inputs
     * @see receiveModulatoryInput() for modulatory inputs
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Inhibitory inputs hyperpolarize the neuron, decreasing the membrane
     * potential and making firing less likely. Typical synaptic weights
     * range from -10.0 to -0.1 mV.
     * 
     * @param amplitude The synaptic input amplitude in millivolts
     * @pre Neuron object must be valid
     * @post Membrane potential is decreased by amplitude (negative value)
     * 
     * @note Negative amplitude decreases firing probability
     * @see receiveExcitatoryInput() for excitatory inputs
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Modulatory inputs can either excite or inhibit depending on the
     * modulatory signal type and internal state. They provide gain control
     * and state-dependent modulation of neural activity.
     * 
     * @param amplitude The synaptic input amplitude in millivolts
     * @pre Neuron object must be valid
     * @post Modulatory input is applied to neuron
     * 
     * @note Effects depend on neuromodulator type and neuron state
     * @see Neuromodulator for modulatory signal details
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject direct current into the neuron (external control)
     * 
     * External current injection bypasses synaptic transmission and directly
     * affects membrane potential. This is used for testing, control, or
     * implementing external stimulation protocols.
     * 
     * @param current The current to inject in nanoamps
     * @pre Neuron object must be valid
     * @post Total current is increased by current value
     * 
     * @note External currents can override normal synaptic inputs
     * @see getTotalCurrent() for reading injected current
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total injected current (external + synaptic)
     * 
     * Returns the sum of all external current injections and synaptic currents
     * that have not yet been integrated into membrane potential. This represents
     * the total driving force on the neuron.
     * 
     * @return Total current in nanoamps
     * @pre Neuron object must be valid
     * @post Current value is returned
     * 
     * @note Reset to zero after each step() call
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all accumulated injected currents
     * 
     * This method resets the total current to zero, typically called at the
     * start of a simulation step to ensure currents are properly accumulated
     * during the step.
     * 
     * @pre Neuron object must be valid
     * @post All injected currents are cleared
     * 
     * @note Called automatically by step() at start of computation
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike occurrence at the given timestamp
     * 
     * This method records the timing of a spike for plasticity mechanisms,
     * particularly Spike-Timing-Dependent Plasticity (STDP). The spike
     * timestamp is stored for later use in weight updates.
     * 
     * @param timestamp The simulation time when the spike occurred (seconds)
     * @pre Neuron object must be valid
     * @post Spike timestamp is recorded in spike history
     * 
     * @note Spike history is cleared by clearSpikeHistory()
     * @see getSpikeHistory() for accessing recorded spikes
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of recent spike timestamps
     * 
     * Returns a const reference to the vector of spike timestamps, allowing
     * access to recent spike times for plasticity computations and analysis.
     * The vector is updated by recordSpike() and cleared by clearSpikeHistory().
     * 
     * @return Const reference to spike timestamp vector
     * @pre Neuron object must be valid
     * @post Spike history is returned
     * 
     * @note Vector may be empty if neuron has never fired
     * @see recordSpike() for adding spikes
     * @see clearSpikeHistory() for clearing
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history vector
     * 
     * Removes all recorded spike timestamps, typically called at the start
     * of a new simulation segment or when plasticity memory should be reset.
     * 
     * @pre Neuron object must be valid
     * @post Spike history vector is empty
     * 
     * @see recordSpike() for adding spikes
     * @see getSpikeHistory() for accessing
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * This method registers a synapse that connects to this neuron as a
     * postsynaptic neuron. It's used for maintaining synaptic connectivity
     * information and for efficient synapse traversal during simulation.
     * 
     * @param handle Unique identifier for the incoming synapse
     * @pre Neuron object must be valid
     * @post Synapse handle is added to incoming synapses list
     * 
     * @note Used for connectivity queries and STDP computation
     * @see SynapseHandle for handle structure
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * This method registers a synapse that originates from this neuron as a
     * presynaptic neuron. It's used for maintaining synaptic connectivity
     * information and for efficient spike transmission.
     * 
     * @param handle Unique identifier for the outgoing synapse
     * @pre Neuron object must be valid
     * @post Synapse handle is added to outgoing synapses list
     * 
     * @note Used for spike propagation and synaptic weight updates
     * @see SynapseHandle for handle structure
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get the list of incoming synapse handles
     * 
     * Returns a const reference to the vector of synapse handles that connect
     * to this neuron as postsynaptic targets. This is used for connectivity
     * queries and plasticity computations.
     * 
     * @return Const reference to incoming synapse handles vector
     * @pre Neuron object must be valid
     * @post Incoming synapse handles are returned
     * 
     * @note Vector is populated by addIncomingSynapse()
     * @see addIncomingSynapse() for adding incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get the list of outgoing synapse handles
     * 
     * Returns a const reference to the vector of synapse handles that originate
     * from this neuron as presynaptic sources. This is used for spike transmission
     * and synaptic weight updates.
     * 
     * @return Const reference to outgoing synapse handles vector
     * @pre Neuron object must be valid
     * @post Outgoing synapse handles are returned
     * 
     * @note Vector is populated by addOutgoingSynapse()
     * @see addOutgoingSynapse() for adding outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * Returns a const reference to the plasticity flags structure that controls
     * which plasticity rules are active for this neuron. Flags determine whether
     * Hebbian learning, STDP, reward-modulated plasticity, and structural changes
     * can occur.
     * 
     * @return Const reference to plasticity flags structure
     * @pre Neuron object must be valid
     * @post Plasticity flags are returned
     * 
     * @see PlasticityFlags for flag definitions
     * @see enablePlasticity() for modification
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable access to plasticity flags
     * 
     * Returns a non-const reference to the plasticity flags structure, allowing
     * modification of plasticity rules. Use with caution as this can affect
     * learning behavior.
     * 
     * @return Non-const reference to plasticity flags structure
     * @pre Neuron object must be valid
     * @post Mutable plasticity flags are returned
     * 
     * @note Plasticity rules control learning and memory formation
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity rules for this neuron
     * 
     * This method activates or deactivates different forms of synaptic plasticity,
     * controlling how synaptic weights change based on neural activity. Each
     * plasticity rule implements a different learning mechanism.
     * 
     * @param hebbian Enable Hebbian (co-activation) learning
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @pre Neuron object must be valid
     * @post Plasticity flags are updated accordingly
     * 
     * @note Multiple plasticity rules can be enabled simultaneously
     * @see PlasticityFlags for rule definitions
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID for this neuron
     * 
     * Assigns the neuron to a specific brain region, providing hierarchical
     * organization of neural populations. Region membership affects connectivity
     * patterns and functional specialization.
     * 
     * @param region The region identifier to assign
     * @pre Neuron object must be valid
     * @post Region ID is set
     * 
     * @see RegionId for identifier structure
     * @see getRegionId() for retrieval
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID for this neuron
     * 
     * @return The neuron's region identifier
     * @pre Neuron object must be valid
     * @post Region ID is returned
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID for this neuron
     * 
     * Assigns the neuron to a specific population within its region.
     * Populations group neurons with similar properties and functions.
     * 
     * @param population The population identifier to assign
     * @pre Neuron object must be valid
     * @post Population ID is set
     * 
     * @see PopulationId for identifier structure
     * @see getPopulationId() for retrieval
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID for this neuron
     * 
     * @return The neuron's population identifier
     * @pre Neuron object must be valid
     * @post Population ID is returned
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Perform one simulation step for the neuron
     * 
     * This is the main simulation method that updates the neuron for one
     * time step. It coordinates all neural computations including membrane
     * potential integration, spike detection, state transitions, and
     * activity recording.
     * 
     * @param currentTime The current simulation time in seconds
     * @pre Neuron object must be valid and initialized
     * @post Neuron state is updated for currentTime
     * @post Membrane potential is integrated according to LIF dynamics
     * @post Firing state may transition
     * @post Spike may be recorded
     * 
     * @note This method calls stepLIF() for fundamental computations
     * @note Spike history is updated if neuron fires
     * @see stepLIF() for detailed LIF computation
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     * 
     * This method returns the neuron to its default state as specified
     * in the constructor. All dynamic state variables are reset to their
     * initial values, and the neuron is prepared for a new simulation segment.
     * 
     * @pre Neuron object must be valid
     * @post Membrane potential = resting potential (-70.0 mV)
     * @post Firing state = Resting
     * @post Refractory period = 0
     * @post Spike history = empty
     * @post Total current = 0
     * @post Last spike time = -1.0
     * 
     * @note This is equivalent to neuron initialization after construction
     * @see initializeRandom() for random parameter initialization
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * This method sets neuron parameters to random values within biologically
     * plausible ranges, creating variability across neurons in a population.
     * This is typically called during brain initialization to create diverse
     * neural populations.
     * 
     * @param rng Reference to the random number generator
     * @pre Neuron object must be valid
     * @post Neuron parameters are set to random values
     * @post Membrane potential = resting potential (-70.0 mV)
     * @post Threshold = random value between -60 and -50 mV
     * @post Leak conductance = random value between 5 and 15 nS
     * @post Refractory period = random value between 2 and 10 steps
     * @post Adaptation variable = 0
     * @post Last spike time = -1.0
     * 
     * @note Random values are drawn from uniform distributions
     * @see RandomGenerator for random number generation
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
