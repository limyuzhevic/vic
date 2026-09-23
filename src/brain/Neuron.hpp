#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * @brief Complete state representation of a neuron for efficient storage
 * 
 * NeuronState contains all parameters necessary to describe the complete
 * instantaneous state of a neuron. This structure is used for both current state
 * storage and state persistence/restoration. All values are in physical units
 * appropriate for biological realism and numerical stability.
 * 
 * Key state variables:
 * - **Membrane Potential**: Current electrical potential across membrane
 * - **Resting Potential**: Equilibrium potential when neuron is inactive
 * - **Threshold**: Firing threshold for action potential generation
 * - **Reset Potential**: Membrane potential after spike generation
 * - **Leak Conductance**: Membrane conductance governing decay
 * - **Synaptic Conductance**: Total inhibitory/excitatory conductance
 * - **Firing Rate**: Current firing rate (Hz) for rate-based computations
 * - **Firing State**: Current state (Resting, Firing, Refractory)
 * - **Refractory Timing**: Refractory period management
 * - **Adaptation Variable**: Spike-frequency adaptation control
 * - **Spike Timing**: Timestamp of last spike for STDP
 * 
 * This structure enables efficient neuron state management and persistence
 * for checkpoint/restore functionality and simulation debugging.
 * 
 * @see Neuron Complete neuron implementation using NeuronState
 * @see Brain Neuron management and population control
 * 
 * @par Implementation Notes:
 * @code
 * The NeuronState structure is designed for cache efficiency with related
 * membrane potential variables grouped together and separate timing/integrator
 * variables in other sections. All units are consistent (mV, nS, Hz, steps).
 * @endcode
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
     * @brief Default constructor - creates a resting neuron
     * 
     * Initializes neuron to physiological resting state with typical parameters
     * for mammalian cortical neurons. This represents the default state for new
     * neurons before any synaptic input or stimulation.
     * 
     * Default values based on experimental data:
     * - Membrane potential: -70 mV (typical resting potential)
     * - Threshold: -55 mV (typical threshold)
     * - Resting potential: -70 mV (same as initial)
     * - Reset potential: -70 mV (clamp after spike)
     * - Leak conductance: 10 nS (typical value)
     * - Refractory period: 5 steps (typical)
     * 
     * @note These are baseline values that can be modified by initialization
     * functions, configuration, or synaptic plasticity.
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
 * @brief Individual neuron implementation with Leaky Integrate-and-Fire dynamics
 * 
 * The Neuron class represents a single biological neuron implementing the
 * Leaky Integrate-and-Fire (LIF) model, one of the most widely used neuron models
 * in computational neuroscience and neuromorphic systems. This model balances
 * biological plausibility with computational efficiency.
 * 
 * Core neural dynamics:
 * - **Integration**: Membrane potential integrates incoming synaptic currents
 * - **Leak**: Exponential decay toward resting potential
 * - **Threshold**: Action potential (spike) generation when threshold crossed
 * - **Reset**: Post-spike membrane potential reset
 * - **Refractory**: Recovery period after spike with reduced excitability
 * - **Plasticity**: Synaptic weight modulation based on activity patterns
 * 
 * Key features:
 * - Real-time spiking dynamics with millisecond precision
 * - Event-driven spike processing for computational efficiency
 * - Configurable synaptic inputs (excitatory, inhibitory, modulatory)
 * - Spike history recording for STDP plasticity
 * - Support for rate-based and time-based computations
 * - Integration with brain-wide plasticity and neuromodulation systems
 * 
 * The Neuron class is fundamental to the NLM architecture, forming the basic
 * computational unit for all neural processing. It uses the pImpl pattern
 * to separate interface from implementation for better performance and maintainability.
 * 
 * @warning This implementation uses simplified LIF dynamics. More complex neuron
 * models (e.g., adaptive exponential integrate-and-fire) could be implemented
 * in future versions for enhanced biological fidelity.
 * 
 * @see NeuronState Complete neuron state representation
 * @see Brain Neural region and population management
 * @see Synapse Synaptic connections to this neuron
 * @see STDP Spike-timing dependent plasticity implementation
 * 
 * @par Example Usage:
 * @code
 * // Create neuron with specific ID
 * Neuron neuron(neuronId);
 * 
 * // Configure neuron parameters
 * neuron.setThreshold(-50.0f);
 * neuron.setLeakConductance(15.0f);
 * neuron.setRestingPotential(-65.0f);
 * 
 * // Initialize with random parameters
 * neuron.initializeRandom(rng);
 * 
 * // Simulation loop
 * while (simulating) {
 *     // Process inputs
 *     neuron.receiveExcitatoryInput(5.0f);
 *     
 *     // Update neural dynamics
 *     neuron.step(currentTime);
 *     
 *     // Check for spikes
 *     if (neuron.checkThreshold()) {
 *         neuron.recordSpike(currentTime);
 *     }
 * }
 * 
 * // Save state for checkpointing
 * NeuronState state = neuron.getState();
 * @endcode
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with unique identifier
     * 
     * Creates a new neuron instance with the specified ID. The neuron is initialized
     * to its default resting state with typical physiological parameters. The neuron
     * is ready for synaptic connections and simulation after construction.
     * 
     * @param id Unique identifier for this neuron (typically from a global pool)
     * @note The neuron ID should be globally unique within the brain to avoid
     * confusion and ensure proper addressing. Consider using the provided
     * NeuronId type for type safety.
     * 
     * @see NeuronId Type definition for neuron identifiers
     * @see reset() Reset neuron to default state
     * @see initializeRandom() Initialize with random parameters
     * 
     * @par Implementation Notes:
     * @code
     * This constructor uses the pImpl pattern, delegating actual implementation
     * to a hidden implementation structure to reduce compilation dependencies
     * and improve maintainability.
     * @endcode
     */
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    /**
     * @brief Destructor - cleans up neuron resources and connections
     * 
     * Properly cleans up all neuron resources including synaptic connections,
     * spike history buffers, and implementation details. Ensures graceful
     * shutdown of the neural element and prevents resource leaks.
     * 
     * @note The pImpl pattern ensures proper cleanup of the implementation
     * structure without exposing internal details to the interface.
     */
    ~Neuron();
    
    // Disable copying, enable moving
    
    /**
     * @brief Disable copying - Neuron contains non-copyable resources
     * 
     * Neurons contain complex internal structures including spike history
     * buffers, synaptic connection lists, and implementation pointers that
     * cannot be safely copied. Copying would result in double-free or
     * resource corruption. Use move semantics instead.
     * 
     * @param other Source neuron to copy from (parameter kept for syntax)
     * @warning This constructor is deleted and will cause compilation errors
     * if attempted to be used. Use the move constructor instead.
     * 
     * @see Neuron(Neuron&&) Move constructor for safe transfer
     * @see operator=(Neuron&&) Move assignment operator
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disable assignment - Neuron contains non-copyable resources
     * 
     * Neurons contain non-shareable resources that cannot be safely assigned.
     * This operation would lead to resource corruption or double-free.
     * 
     * @param other Source neuron to assign from (parameter kept for syntax)
     * @return Reference to this neuron (parameter kept for syntax)
     * @warning This operator is deleted and will cause compilation errors
     * if attempted to be used.
     * 
     * @see Neuron& operator=(Neuron&&) Move assignment for safe transfer
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - efficiently transfers neuron ownership
     * 
     * This constructor allows efficient transfer of neuron ownership, moving
     * the implementation pointer and all associated resources (spike history,
     * synaptic connections, etc.) from the source neuron to this neuron.
     * The source neuron is left in a valid but unspecified state after the move.
     * 
     * @param other Neuron to move from (invalidated after move)
     * @warning After moving from a Neuron object, the source neuron should not
     * be used as it no longer owns its resources. It should be treated as empty.
     * 
     * @see Neuron(const Neuron&) Deleted copy constructor
     * @see operator=(Neuron&&) Move assignment operator
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - efficiently transfers neuron ownership
     * 
     * Transfers ownership of the implementation structure and all associated
     * resources from the source neuron to this neuron. Properly cleans up
     * existing resources before assignment. This operation has strong exception
     * safety guarantees.
     * 
     * @param other Neuron to move from
     * @return Reference to this neuron (allows chaining)
     * @warning After assignment, the source neuron should not be used.
     * 
     * @see Neuron& operator=(const Neuron&) Deleted copy assignment operator
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Gets the unique identifier of this neuron
     * 
     * Returns the neuron's unique identifier, which can be used for addressing
     * this neuron in the brain, debugging, or logging purposes.
     * 
     * @return NeuronId Unique identifier for this neuron
     * @note The neuron ID is set at construction and remains constant throughout
     * the neuron's lifetime, providing stable addressing for connections and
     * state management.
     * 
     * @see NeuronId Type definition for neuron identifiers
     * @see setRegionId() Set brain region for this neuron
     * @see setPopulationId() Set population for this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron type classification
     * 
     * Returns the functional type of this neuron, which determines its role
     * in the network and expected behavior. Neuron types categorize neurons
     * into functional groups with different properties and connectivity patterns.
     * 
     * @return NeuronType The functional type of this neuron
     * @note Neuron types include Sensory, Internal, Motor, and others. Each type
     * may have different default parameters and connectivity constraints.
     * 
     * @see NeuronType Enumerated neuron types
     * @see setType() Set neuron type
     * @see Brain Neural region and population management
     * 
     * @warning Changing neuron type after initialization may require reconfiguration
     * of connectivity and parameters to maintain network consistency.
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron type classification
     * 
     * Changes the functional type of this neuron, affecting its expected behavior,
     * connectivity, and potentially its default parameters. Use with caution as
     * type changes may have cascading effects throughout the network.
     * 
     * @param type New functional type for this neuron
     * @note Valid types include Sensory, Internal, Motor, etc. The available types
     * depend on the brain configuration and network architecture.
     * 
     * @see getType() Get current neuron type
     * @see NeuronType Enumerated neuron types
     * @warning Changing neuron type may affect synaptic connections and network dynamics.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the complete current state of this neuron
     * 
     * Returns a constant reference to the neuron's current state structure, providing
     * full access to all membrane potential, firing state, and timing variables.
     * This is useful for debugging, monitoring, and state inspection.
     * 
     * @return Constant reference to NeuronState
     * @note The state includes membrane potential, firing rate, refractory timing,
     * and spike history. Changes to the returned state structure are not permitted
     * as it represents a snapshot of the current state.
     * 
     * @see NeuronState Complete state structure definition
     * @see getState() Non-constant version for mutable access
     * @see reset() Reset neuron to default state
     * 
     * @warning Reading large numbers of neuron states simultaneously may impact
     * performance. For bulk state access, consider more efficient data structures.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets mutable reference to this neuron's current state
     * 
     * Returns a mutable reference to the neuron's state structure, allowing
     * direct modification of state variables. This should be used with caution
     * as direct state manipulation may bypass neural dynamics and cause inconsistencies.
     * 
     * @return Mutable reference to NeuronState
     * @note This method provides low-level state access for advanced use cases
     * such as state manipulation, debugging, or custom simulation logic.
     * Normal simulation should use the neuron's step() method and spike detection
     * methods rather than direct state manipulation.
     * 
     * @see getState() Constant version for read-only access
     * @see step() Normal neural dynamics update method
     * @see reset() Proper method for resetting state
     * 
     * @warning Direct state manipulation can cause inconsistent neural dynamics.
     * Use only for debugging, testing, or specialized simulation scenarios.
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential of this neuron
     * 
     * Returns the neuron's current membrane potential, which is the primary variable
     * representing the neuron's internal state. This potential integrates all
     * incoming synaptic currents and decays according to the neuron's biophysical
     * properties.
     * 
     * @return MembranePotential Current membrane potential (mV)
     * @note Values are typically in the range of -70 mV (resting) to -40 mV (near threshold).
     * Positive values indicate depolarization (excitation), negative values indicate
     * hyperpolarization (inhibition).
     * 
     * @see setMembranePotential() Set membrane potential directly
     * @see addToMembranePotential() Add current to membrane potential
     * @see getThreshold() Get firing threshold for comparison
     * @see checkThreshold() Check if neuron should fire
     * 
     * @warning Monitoring membrane potential is useful for understanding neural
     * dynamics but should not be used as the primary method of controlling neuron
     * behavior except for specialized debugging or control applications.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential directly
     * 
     * Sets the membrane potential to an explicit value, bypassing normal neural
     * dynamics. This should be used with caution as it can cause unrealistic
     * neural states and bypass important biophysical constraints.
     * 
     * @param potential New membrane potential (mV)
     * @note This method is primarily useful for:
     *       - Resetting neurons to known states
     *       - Debugging and testing
     *       - Specialized simulation scenarios
     *       - State manipulation for advanced use cases
     * 
     * @see getMembranePotential() Get current membrane potential
     * @see addToMembranePotential() Add current to membrane potential (normal approach)
     * @see step() Normal dynamics update method
     * 
     * @warning Direct membrane potential manipulation can bypass important neural
     * constraints and lead to biologically implausible states. Use with caution
     * and preferably use the neuron's step() method for normal simulation.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds current to membrane potential (normal synaptic integration)
     * 
     * Increments the neuron's membrane potential by the specified amount, simulating
     * the effect of synaptic input. This is the normal way for neurons to receive
     * input and integrate it over time.
     * 
     * @param delta Change to add to membrane potential (mV)
     * @note Positive delta represents excitatory input (depolarization),
     * negative delta represents inhibitory input (hyperpolarization).
     * 
     * @see setMembranePotential() Direct setting (bypasses dynamics)
     * @see getMembranePotential() Get current membrane potential
     * @see step() Normal dynamics include leak integration
     * 
     * @warning This method models synaptic current injection but does not include
     * the neuron's biophysical properties (leak, capacitance, etc.). For realistic
     * neural dynamics, this should be followed by a call to step() or have the step()
     * method handle the integration internally.
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the current firing threshold of this neuron
     * 
     * Returns the neuron's firing threshold, which is the membrane potential level
     * at which the neuron generates an action potential (spike). This threshold
     * determines the neuron's excitability and response properties.
     * 
     * @return MembranePotential Firing threshold (mV)
     * @note Typical threshold values range from -55 mV to -30 mV. Lower (more negative)
     * thresholds indicate higher excitability (easier to fire), higher (less negative)
     * thresholds indicate lower excitability (harder to fire).
     * 
     * @see setThreshold() Set firing threshold
     * @see getMembranePotential() Get current membrane potential for comparison
     * @see checkThreshold() Check if neuron should fire based on current potential
     * @see NeuronState::threshold Neuron's state threshold field
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold of this neuron
     * 
     * Sets the neuron's firing threshold, determining the membrane potential level
     * at which the neuron will generate action potentials. This is a key parameter
     * controlling neuronal excitability and network dynamics.
     * 
     * @param threshold New firing threshold (mV)
     * @note Threshold changes affect neuronal excitability throughout the network.
     * Lower thresholds make neurons more likely to fire, higher thresholds make them
     * less likely to fire. This can have network-wide effects on activity patterns.
     * 
     * @see getThreshold() Get current firing threshold
     * @see checkThreshold() Check firing condition
     * @see getMembranePotential() Compare with current membrane potential
     * 
     * @warning Changing threshold during simulation can cause abrupt changes in
     * network dynamics. For controlled threshold changes, consider using plasticity
     * mechanisms or developmental programming.
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
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
    Impl* pImpl;
};

} // namespace nlm
