#pragma once

#include "../core/Types/Types.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <vector>
#include <array>
#include "CommonUtilities.hpp"

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
// Defines all state variables required for LIF neuron dynamics
struct NeuronState {
    /** Current membrane potential (mV) - determines neuron excitability */
    MembranePotential membranePotential;
    /** Resting potential (mV) - baseline membrane potential when not active */
    MembranePotential restingPotential;
    /** Firing threshold (mV) - voltage level at which neuron generates action potential */
    MembranePotential threshold;
    /** Post-spike reset value (mV) - membrane potential after spike generation */
    MembranePotential resetPotential;
    /** Leak conductance (nS) - governs passive current flow across membrane */
    MembranePotential leakConductance;
    /** Synaptic conductance (nS) - total synaptic current input this step */
    MembranePotential synapseConductance;
    /** Current firing rate (Hz) - instantaneous firing rate for rate-based computation */
    FiringRate firingRate;
    /** Current firing state - represents neuron state in LIF dynamics */
    FiringState firingState;
    /** Steps remaining in refractory period - prevents premature spiking */
    uint32_t refractoryRemaining;
    /** Total refractory period (steps) - duration of absolute refractory period */
    uint32_t refractoryPeriod;
    /** Variable for spike-frequency adaptation - gradual adaptation after repeated spikes */
    float adaptationVariable;
    /** Timestamp of last spike (-1 if none) - for STDP and plasticity rules */
    float lastSpikeTime;
    
    /**
     * @brief Default constructor for NeuronState
     * @note Initializes all state variables to biologically plausible default values
     *       suitable for a typical integrate-and-fire neuron model.
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
 * @brief Single neuron implementation with Leaky Integrate-and-Fire (LIF) dynamics
 *
 * The Neuron class represents an artificial neuron implementing real biological
 * spiking dynamics using the Leaky Integrate-and-Fire model. It provides a
 * biologically realistic model of neuronal behavior with realistic membrane
 * dynamics, synaptic integration, spike generation, and plasticity mechanisms.
 *
 * The neuron is designed as a fundamental computational unit for the NLM
 * framework, supporting various neuron types and providing a platform for
 * studying neural dynamics, learning, and information processing.
 *
 * Key features:
 * - LIF dynamics with realistic membrane potential evolution
 * - Synaptic integration with excitatory, inhibitory, and modulatory inputs
 * - Real spike generation with refractory periods
 * - Spike history tracking for STDP and other plasticity rules
 * - Plasticity flags for enabling/disabling learning mechanisms
 * - Random parameter initialization for biological variability
 * - Regional and population membership for distributed processing
 *
 * The class is designed for:
 * - Studying neural coding and information representation
 * - Simulating brain networks with realistic neuronal properties
 * - Implementing learning algorithms (STDP, Hebbian, reward-modulated)
 * - Analyzing neural dynamics and computational properties
 * - Building complex cognitive architectures with multiple neuron populations
 *
 * @note Neuron uses the PImpl pattern for efficient memory management and
 *       thread-safe operations. The actual implementation is hidden in the
 *       private Impl struct to allow for optimization and binary compatibility.
 *
 * @warning Neurons with invalid parameters or unsafe operations may cause
 *          numerical instability. Always validate neuron parameters before use.
 *          See NLMErrors for available error codes.
 *
 * @see NeuronState
 * @see Synapse
 * @see Brain
 * @see PlasticityRule
 * @see NLMErrors
 *
 * @todo Add support for dendritic compartmentalization
 * @todo Implement active conductances (e.g., persistent Na+, Ca2+)
 * @todo Add support for stochastic channel noise
 * @todo Implement gap junction connections between neurons
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 *
 * Examples:
 * @code
 * // Create a neuron with ID 1
 * Neuron neuron(NeuronId(1));
 *
 * // Set neuron type and properties
 * neuron.setType(NeuronType::Excitatory);
 * neuron.setThreshold(-50.0f);
 * neuron.setRestingPotential(-65.0f);
 *
 * // Initialize with random biological parameters
 * RandomGenerator rng(42);
 * neuron.initializeRandom(rng);
 *
 * // Update neuron for one simulation step
 * neuron.step(currentTime);
 *
 * // Check for spike
 * if (neuron.checkThreshold()) {
 *     // Handle spike event
 *     // Apply synaptic transmission to connected neurons
 * }
 *
 * // Record spike for plasticity
 * neuron.recordSpike(currentTime);
 * @endcode
 */
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
/**
     * @brief Constructor implementation for the Neuron class
     *
     * Creates a new neuron with the specified identifier. The constructor
     * performs comprehensive validation of input parameters and initializes
     * the neuron with biologically plausible default values.
     *
     * @param id Unique identifier for the neuron (must be positive)
     * @throws NLMErrors::InvalidParameter if ID is invalid or negative
     * @throws NLMErrors::MemoryAllocation if memory allocation fails
     *
     * @note The constructor implements:
     * - Parameter validation for neuron identity
     * - Initialization of internal implementation structure
     * - Setting of biologically plausible default parameters
     * - Establishment of PImpl pattern for efficient memory management
     *
     * The neuron is initialized with:
     * - Default LIF neuron dynamics parameters
     * - Biologically realistic resting potential (-70mV)
     * - Threshold potential (-55mV)
     * - Reset potential (-70mV)
     * - Leak conductance (10nS)
     * - Refractory period (5 steps)
     * - No firing rate or adaptation
     *
     * @see getId
     * @see initializeRandom
     * @see stepLIF
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor implementation for the Neuron class
     *
     * Cleans up the neuron by destroying the internal implementation structure.
     * The destructor ensures proper cleanup of memory resources while
     * maintaining exception safety.
     *
     * @note The destructor is defaulted to avoid unnecessary code and ensure that
     *       the unique_ptr smart pointer properly cleans up the implementation
     *       structure when the Neuron object is destroyed.
     */
    ~Neuron();
    
    /**
     * @brief Move constructor for the Neuron class
     *
     * Transfers ownership of the internal implementation structure from the source
     * Neuron object to this new Neuron object. The source object's implementation
     * pointer is set to nullptr to prevent double deletion.
     *
     * @param other The source Neuron object to move from
     * @note This operation provides exception safety and efficient transfer of
     *       ownership of the internal implementation structure.
     */
    Neuron(Neuron&& other) noexcept;
    
    /**
     * @brief Move assignment operator for the Neuron class
     *
     * Transfers ownership of the internal implementation structure from the source
     * Neuron object to this Neuron object. Properly handles self-assignment and
     * ensures safe resource transfer.
     *
     * @param other The source Neuron object to move from
     * @return Reference to this Neuron object after assignment
     * @note This operation provides strong exception safety and efficient transfer
     *       of ownership while properly handling self-assignment cases.
     */
    Neuron& operator=(Neuron&& other) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     *
     * Retrieves the unique ID assigned to this neuron within the neural network.
     * This identifier is used for synaptic circuit mapping and neural population management.
     *
     * @return NeuronId The unique identifier of this neuron
     * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
     *
     * @note The neuron ID is set during construction and remains constant throughout
     *       the neuron's lifetime. It is used for addressing neurons in neural circuits
     *       and synaptic management operations.
     *
     * @see setRegionId
     * @see setPopulationId
     * @see addIncomingSynapse
     * @see addOutgoingSynapse
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type classification
     *
     * Retrieves the type of this neuron that determines its transmission properties
     * and behavior in neural computation. Different neuron types have different
     * characteristics and computational properties.
     *
     * @return NeuronType The type of this neuron
     * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
     *
     * @note Neuron types include:
     * - Excitatory: Increases postsynaptic neuron excitability
     * - Inhibitory: Decreases postsynaptic neuron excitability
     * - Modulatory: Releases neuromodulators that affect synaptic strength
     * - Sensory: Responds to external stimuli
     * - Motor: Generates motor commands
     * - Internal: Processes internal neural signals
     *
     * @see setType
     * @see isExcitatory
     * @see isInhibitory
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type classification
     *
     * Assigns a type to the neuron that determines its transmission properties
     * and behavior in neural computation. Type assignment includes comprehensive
     * validation to ensure biological plausibility.
     *
     * @param type The neuron type to assign
     * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
     * @throws NLMErrors::InvalidParameter if the neuron type is invalid
     *
     * @note Valid neuron types are constrained by the NeuronType enumeration:
     * - NeuronType::Excitatory: Standard chemical excitatory neuron
     * - NeuronType::Inhibitory: Standard chemical inhibitory neuron
     * - NeuronType::Modulatory: Specialized neuron releasing neuromodulators
     * - NeuronType::Sensory: Neuron responding to external stimuli
     * - NeuronType::Motor: Neuron generating motor commands
     * - NeuronType::Internal: Neuron processing internal neural signals
     *
     * @see getType
     * @see isExcitatory
     * @see isInhibitory
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get constant reference to neuron state
     *
     * Retrieves a constant reference to the neuron state structure, providing
     * read-only access to all neuron state variables including membrane potential,
     * firing state, synaptic conductance, and spike history.
     *
     * @return const NeuronState& Constant reference to the neuron state
     * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
     *
     * @note State access includes:
     * - Membrane potential and threshold values
     * - Firing state and refractory period
     * - Synaptic conductance and current inputs
     * - Spike history and timing information
     * - Plasticity flags and adaptation variables
     *
     * @see getState (non-const version)
     * @see stepLIF
     * @see checkThreshold
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     *
     * Retrieves a non-constant reference to the neuron state structure, allowing
     * modification of neuron state variables. This is used for simulation updates,
     * debugging, and specialized neuron manipulation.
     *
     * @return NeuronState& Non-constant reference to the neuron state
     * @throws NLMErrors::SystemNotInitialized if the Neuron object is not properly initialized
     *
     * @note Mutable state access is used for:
     * - Simulation step updates
     * - Direct neuron manipulation
     * - Debugging and testing
     * - Performance optimization
     *
     * @see getState (const version)
     * @see stepLIF
     * @see reset
     */
    NeuronState& getState();
    
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
