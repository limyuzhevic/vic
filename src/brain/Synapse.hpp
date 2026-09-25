#pragma once

#include "../core/Types/Types.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <vector>
#include "CommonUtilities.hpp"

namespace nlm {

/**
 * @file Synapse.hpp
 * @brief Definition of the Synapse class for NLM synaptic transmission
 *
 * This file contains the definition of the Synapse class, which implements
 * real synaptic transmission between neurons in the Neural Learning Machine framework.
 * The synapse model includes weight-based transmission, delay mechanisms, and
 * short-term plasticity dynamics for biologically realistic neural computation.
 *
 * @note This implementation provides:
 * - Real synaptic dynamics with weight-based transmission
 * - Transmission delays for temporal dynamics
 * - Spike history tracking for STDP and other plasticity rules
 * - Plasticity state management for learning mechanisms
 * - Short-term plasticity (STP) for dynamic synaptic efficacy modulation
 *
 * The synapse is a fundamental component for neural communication and learning,
 * enabling the implementation of complex neural circuits with realistic transmission
 * properties and dynamic synaptic behavior.
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 */

namespace nlm {

/**
 * @struct Synapse::Impl
 * @brief Internal implementation details for the Synapse class
 *
 * The PImpl (Pointer to Implementation) idiom is used to hide the implementation
 * details of the Synapse class, allowing efficient compilation and binary
 * compatibility while maintaining encapsulation of the complex synaptic dynamics.
 *
 * This struct contains all the data members required for synaptic transmission,
 * including:
 * - Basic synaptic identity and connectivity information
 * - Synaptic weight and delay parameters
 * - Synaptic type and transmission properties
 * - Spike history for plasticity and learning
 * - Plasticity state for learning mechanisms
 * - Short-term plasticity state for dynamic modulation
 *
 * @note The Impl struct is designed to be compact and efficient, with all the
 *       synaptic dynamics implemented in-place to minimize memory overhead and
 *       maximize computational efficiency in large neural networks.
 */
struct Synapse::Impl {
    /** Unique identifier for this synapse within the neural network */
    SynapseId id;
    /** Identifier of the presynaptic neuron that generates action potentials */
    NeuronId sourceNeuron;
    /** Identifier of the postsynaptic neuron that receives action potentials */
    NeuronId destinationNeuron;
    /** Synaptic weight determines transmission strength (-1.0 to 1.0) */
    SynapticWeight weight;
    /** Synaptic delay in simulation steps for temporal dynamics */
    Delay delay;
    /** Type classification of synaptic transmission properties */
    SynapseType type;
    /** History of presynaptic spike timestamps for STDP computation */
    std::vector<Timestamp> preSpikeHistory;
    /** History of postsynaptic spike timestamps for STDP computation */
    std::vector<Timestamp> postSpikeHistory;
    /** Plasticity state for learning mechanisms */
    PlasticityFlags plasticityFlags;
    /** Short-term depression state (Tsodyks-Markram model) */
    float shortTermDepression;
    /** Short-term facilitation state (Tsodyks-Markram model) */
    float shortTermFacilitation;
    /** Timestamp of last presynaptic spike for STP dynamics */
    float lastPreSpikeTime;
    /** Timestamp of last postsynaptic spike for STP dynamics */
    float lastPostSpikeTime;
    /** Eligibility trace for reward-modulated learning */
    float eligibilityTrace;
    /** Synaptic efficacy (use-dependent modulation factor) */
    float efficacy;
    
    /**
     * @brief Minimum allowed synaptic weight (preventing infinite negative weights)
     * @note -1.0 corresponds to strong inhibitory transmission
     */
    static constexpr float MIN_WEIGHT = -1.0f;
    /**
     * @brief Maximum allowed synaptic weight (preventing infinite positive weights)
     * @note 1.0 corresponds to strong excitatory transmission
     */
    static constexpr float MAX_WEIGHT = 1.0f;
    
    /**
     * @brief Short-term depression time constant (ms)
     * @note Governs recovery from depression toward baseline (200ms typical)
     */
    static constexpr float STP_DEPRESSION_TAU = 200.0f;
    /**
     * @brief Short-term facilitation time constant (ms)
     * @note Governs decay of facilitation (100ms typical)
     */
    static constexpr float STP_FACILITATION_TAU = 100.0f;
    /**
     * @brief Maximum utilization factor for short-term plasticity
     * @note U_max represents the maximum probability of vesicle release
     */
    static constexpr float STP_U_MAX = 1.0f;
    
    /**
     * @brief Maximum spike history size for plasticity computation
     * @note Limits memory usage while preserving sufficient history for STDP
     */
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    /**
     * @brief Default constructor for Synapse::Impl
     * @note Initializes synapse with biologically plausible default values
     *       representing an excitatory connection with standard parameters.
     */
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), shortTermDepression(1.0f),
             shortTermFacilitation(0.0f), lastPreSpikeTime(-1.0f),
             lastPostSpikeTime(-1.0f), eligibilityTrace(0.0f), efficacy(1.0f) {}
};

/**
 * @class Synapse
 * @brief Synaptic connection implementing real transmission between neurons
 *
 * The Synapse class represents a biological synaptic connection between two neurons,
 * implementing realistic synaptic transmission with weight-based signaling, transmission
 * delays, and short-term plasticity dynamics. It provides the fundamental mechanism
 * for neural communication and information processing in the NLM framework.
 *
 * Key features:
 * - Weight-based transmission with excitatory/inhibitory properties
 * - Transmission delays for temporal dynamics and propagation
 * - Short-term plasticity (STP) for dynamic synaptic efficacy modulation
 * - Spike history tracking for STDP and other plasticity rules
 * - Plasticity state for learning mechanisms (Hebbian, STDP, reward-modulated)
 * - Eligibility traces for reward-modulated learning
 * - Use-dependent efficacy modulation for activity-dependent transmission
 *
 * The synapse is designed for:
 * - Implementing realistic neural circuits with temporal dynamics
 * - Studying synaptic plasticity and learning mechanisms
 * - Simulating neural development and maturation processes
 * - Analyzing information flow in neural networks
 * - Building complex cognitive architectures
 *
 * @note Synapse uses the PImpl pattern for efficient memory management and
 *       thread-safe operations. The actual synaptic dynamics are implemented
 *       in the private Impl struct to allow for optimization and binary compatibility.
 *
 * @warning Synaptic weights outside the valid range [-1.0, 1.0] can cause
 *          numerical instability or unrealistic neural dynamics. Always validate
 *          synaptic parameters before use.
 *
 * @see Neuron
 * @see STDP
 * @see PlasticityRule
 * @see SpikeSystem
 *
 * @todo Add support for synaptic compartmentalization
 * @todo Implement gap junction connections
 * @todo Add support for neuromodulatory effects on synapses
 * @todo Implement spike-timing dependent synapse strengthening/weakening
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 *
 * Examples:
 * @code
 * // Create a synapse connecting neuron 1 to neuron 2
 * Synapse synapse(SynapseId(1), NeuronId(1), NeuronId(2));
 *
 * // Set synaptic properties
 * synapse.setType(SynapseType::Excitatory);
 * synapse.setWeight(0.5f);
 * synapse.setDelay(5); // 5ms delay
 *
 * // Initialize with random biological parameters
 * RandomGenerator rng(42);
 * synapse.initializeRandom(rng);
 *
 * // Record spike events for plasticity
 * synapse.recordPreSpike(currentTime);
 * synapse.recordPostSpike(currentTime);
 *
 * // Update synapse dynamics
 * synapse.step(currentTime);
 * @endcode
 */
class Synapse {
public:
    /**
     * @brief Constructor implementation for the Synapse class
     *
     * Creates a new synapse with specified source and destination neurons.
     * The constructor performs comprehensive validation of input parameters
     * and initializes the synapse with biologically plausible default values.
     *
     * @param id Unique identifier for the synapse (must be positive)
     * @param source Neuron that generates action potentials (presynaptic)
     * @param destination Neuron that receives action potentials (postsynaptic)
     * @throws NLMErrors::InvalidParameter if any ID is invalid or neurons are identical
     * @throws NLMErrors::MemoryAllocation if memory allocation fails
     *
     * @note The constructor implements:
     * - Parameter validation for synapse identity and connectivity
     * - Initialization with default excitatory transmission properties
     * - Setting of default delay (1 step) and weight (0.0)
     * - Initialization of short-term plasticity state
     * - Setting of spike history tracking flags
     * - Logging for synapse creation tracking
     *
     * The synapse is initialized with biologically plausible default parameters
     * suitable for typical neural transmission dynamics.
     *
     * @see setType
     * @see setWeight
     * @see setDelay
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Default destructor for the Synapse class
     *
     * Cleans up the synapse by destroying the internal implementation structure.
     * The destructor is explicitly defaulted to ensure proper cleanup of the
     * unique_ptr smart pointer while maintaining exception safety.
     *
     * @note The destructor is defaulted to avoid unnecessary code and ensure that
     *       the unique_ptr smart pointer properly cleans up the implementation
     *       structure when the Synapse object is destroyed.
     */
    ~Synapse();
    
    /**
     * @brief Move constructor for the Synapse class
     *
     * Transfers ownership of the internal implementation structure from the source
     * Synapse object to this new Synapse object. The source object's implementation
     * pointer is set to nullptr to prevent double deletion.
     *
     * @param other The source Synapse object to move from
     * @note This operation provides exception safety and efficient transfer of
     *       ownership of the internal implementation structure.
     */
    Synapse(Synapse&& other) noexcept;
    
    /**
     * @brief Move assignment operator for the Synapse class
     *
     * Transfers ownership of the internal implementation structure from the source
     * Synapse object to this Synapse object. Properly handles self-assignment and
     * ensures safe resource transfer.
     *
     * @param other The source Synapse object to move from
     * @return Reference to this Synapse object after assignment
     * @note This operation provides strong exception safety and efficient transfer
     *       of ownership while properly handling self-assignment cases.
     */
    Synapse& operator=(Synapse&& other) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     *
     * Retrieves the unique ID assigned to this synapse within the neural network.
     * This identifier is used for synaptic circuit mapping and management.
     *
     * @return SynapseId The unique identifier of this synapse
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note The synapse ID is set during construction and remains constant throughout
     *       the synapse's lifetime. It is used for addressing synapses in neural circuits
     *       and synaptic management operations.
     *
     * @see getSourceNeuron
     * @see getDestinationNeuron
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the presynaptic neuron identifier
     *
     * Retrieves the unique ID of the neuron that generates action potentials
     * transmitted through this synapse (presynaptic neuron).
     *
     * @return NeuronId The identifier of the presynaptic neuron
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note The presynaptic neuron is the source of spikes that activate
     *       postsynaptic neurons through this synaptic connection. This information
     *       is essential for understanding neural circuit connectivity and flow.
     *
     * @see getId
     * @see getDestinationNeuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the postsynaptic neuron identifier
     *
     * Retrieves the unique ID of the neuron that receives action potentials
     * transmitted through this synapse (postsynaptic neuron).
     *
     * @return NeuronId The identifier of the postsynaptic neuron
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note The postsynaptic neuron is the target of spikes that travel through
     *       this synaptic connection. This information is essential for understanding
     *       neural circuit connectivity and information flow.
     *
     * @see getId
     * @see getSourceNeuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     *
     * Retrieves the current synaptic weight, which determines the strength
     * and sign of transmission through this synapse. Weight is normalized
     * to the range [-1.0, 1.0].
     *
     * @return SynapticWeight The current synaptic weight
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Synaptic weight influences:
     * - Transmission strength (magnitude)
     * - Sign of transmission (excitatory vs. inhibitory)
     * - Plasticity rules and learning dynamics
     * - Network stability and dynamics
     *
     * @see setWeight
     * @see addToWeight
     * @see getType
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     *
     * Assigns a new value to the synaptic weight, which determines the strength
     * and sign of transmission through this synapse. The weight is constrained
     * to the valid range [-1.0, 1.0] and undergoes comprehensive validation.
     *
     * @param weight The synaptic weight to set (must be in range [-1.0, 1.0])
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if weight is out of valid range
     *
     * @note Weight assignment includes:
     * - Range validation to prevent numerical instability
     * - Type checking for valid weight values
     * - Logging for weight modification tracking
     * - Context information for debugging
     *
     * Synaptic weight is a fundamental parameter that determines:
     * - How strongly neurons influence each other
     * - The sign of synaptic transmission
     * - The dynamics of neural networks
     * - The potential for learning and plasticity
     *
     * @see getWeight
     * @see addToWeight
     * @see MIN_WEIGHT
     * @see MAX_WEIGHT
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add delta to the current synaptic weight
     *
     * Modifies the synaptic weight by adding a delta value, useful for implementing
     * incremental weight changes in plasticity rules. The resulting weight is
     * clamped to the valid range to prevent invalid values.
     *
     * @param delta The weight change to add (can be positive or negative)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if resulting weight would be out of range
     *
     * @note Weight modification includes:
     * - Adding delta to current weight
     * - Clamping to valid range [-1.0, 1.0]
     * - Logging for weight change tracking
     * - Validation for numerical stability
     *
     * @see getWeight
     * @see setWeight
     * @see applyWeightChange
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic transmission delay
     *
     * Retrieves the current transmission delay for this synapse, which determines
     * the temporal delay between presynaptic spike generation and postsynaptic
     * response. Delay is measured in simulation steps.
     *
     * @return Delay The current transmission delay (in steps)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Transmission delay is essential for:
     * - Implementing temporal dynamics in neural computation
     * - Creating realistic propagation delays
     * - Modeling neural circuit timing
     * - Controlling information flow timing
     *
     * @see setDelay
     * @see STP_FACILITATION_TAU
     * @see STP_DEPRESSION_TAU
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic transmission delay
     *
     * Assigns a new value to the synaptic transmission delay, which controls
     * the temporal delay between presynaptic spike generation and postsynaptic
     * response. Delay is constrained to non-negative values.
     *
     * @param delay The transmission delay to set (must be non-negative)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if delay is invalid
     *
     * @note Delay validation includes:
     * - Range checking (0 to 10000 steps typical)
     * - Biological plausibility checks
     * - Logging for delay modification tracking
     * - Context information for debugging
     *
     * @see getDelay
     * @see getPreSpikeHistory
     * @see getPostSpikeHistory
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type classification
     *
     * Retrieves the synapse type that determines its transmission properties
     * and behavior in neural computation. Different synapse types have different
     * characteristics and transmission rules.
     *
     * @return SynapseType The type of this synapse
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Synapse types include:
     * - Excitatory: Increases postsynaptic neuron excitability
     * - Inhibitory: Decreases postsynaptic neuron excitability
     * - GapJunction: Electrical coupling between neurons
     * - Special: Specialized synaptic mechanisms
     *
     * @see setType
     * @see isExcitatory
     * @see isInhibitory
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type classification
     *
     * Assigns a type to the synapse that determines its transmission properties
     * and behavior in neural computation. Type assignment includes comprehensive
     * validation to ensure biological plausibility.
     *
     * @param type The synapse type to assign
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if the synapse type is invalid
     *
     * @note Valid synapse types are constrained by the SynapseType enumeration:
     * - SynapseType::Excitatory: Standard chemical excitatory synapse
     * - SynapseType::Inhibitory: Standard chemical inhibitory synapse
     * - SynapseType::GapJunction: Electrical coupling synapse
     * - SynapseType::Specialized: Specialized synaptic mechanism
     *
     * @see getType
     * @see isExcitatory
     * @see isInhibitory
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     *
     * Determines whether this synapse is excitatory based on its type and weight.
     * An excitatory synapse increases the excitability of the postsynaptic neuron.
     *
     * @return true if the synapse is excitatory, false otherwise
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Excitatory synapses typically:
     * - Have positive weight values
     * - Release excitatory neurotransmitters
     * - Depolarize postsynaptic membrane
     * - Increase firing probability
     *
     * @see getType
     * @see isInhibitory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     *
     * Determines whether this synapse is inhibitory based on its type and weight.
     * An inhibitory synapse decreases the excitability of the postsynaptic neuron.
     *
     * @return true if the synapse is inhibitory, false otherwise
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Inhibitory synapses typically:
     * - Have negative weight values
     * - Release inhibitory neurotransmitters
     * - Hyperpolarize postsynaptic membrane
     * - Decrease firing probability
     *
     * @see getType
     * @see isExcitatory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike event
     *
     * Records a spike from the presynaptic neuron (source neuron) for plasticity
     * and learning mechanisms. The spike timestamp is stored in the pre-spike
     * history for use in STDP and other spike-timing dependent plasticity rules.
     *
     * @param timestamp The spike timestamp (in simulation time)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if timestamp is invalid
     *
     * @note Spike recording includes:
     * - Timestamp validation
     * - History size management (prevents overflow)
     * - Updating last spike time for STP dynamics
     * - Logging for spike event tracking
     *
     * @see recordPostSpike
     * @see getPreSpikeHistory
     * @see getPostSpikeHistory
     * @see clearHistory
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike event
     *
     * Records a spike from the postsynaptic neuron (destination neuron) for plasticity
     * and learning mechanisms. The spike timestamp is stored in the post-spike
     * history for use in STDP and other spike-timing dependent plasticity rules.
     *
     * @param timestamp The spike timestamp (in simulation time)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if timestamp is invalid
     *
     * @note Spike recording includes:
     * - Timestamp validation
     * - History size management (prevents overflow)
     * - Updating last spike time for STP dynamics
     * - Logging for spike event tracking
     *
     * @see recordPreSpike
     * @see getPreSpikeHistory
     * @see getPostSpikeHistory
     * @see clearHistory
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     *
     * Retrieves the history of spike events from the presynaptic neuron (source neuron)
     * for plasticity computation. The history contains timestamps of all recorded
     * presynaptic spikes within the storage window.
     *
     * @return const std::vector<Timestamp>& Reference to the presynaptic spike history
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Pre-spike history is used for:
     * - STDP computation
     * - Spike-timing dependent plasticity
     * - Short-term plasticity modulation
     * - Learning mechanism implementation
     *
     * @see recordPreSpike
     * @see recordPostSpike
     * @see clearHistory
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     *
     * Retrieves the history of spike events from the postsynaptic neuron (destination neuron)
     * for plasticity computation. The history contains timestamps of all recorded
     * postsynaptic spikes within the storage window.
     *
     * @return const std::vector<Timestamp>& Reference to the postsynaptic spike history
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Post-spike history is used for:
     * - STDP computation
     * - Spike-timing dependent plasticity
     * - Short-term plasticity modulation
     * - Learning mechanism implementation
     *
     * @see recordPreSpike
     * @see recordPostSpike
     * @see clearHistory
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     *
     * Removes all recorded spike events from both pre- and post-spike history.
     * This is used for resetting synaptic state or starting new plasticity computations.
     *
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Clearing spike history is used for:
     * - Resetting synaptic state
     * - Starting new learning epochs
     * - Preventing memory accumulation
     * - Managing computational resources
     *
     * @see recordPreSpike
     * @see recordPostSpike
     * @see getPreSpikeHistory
     * @see getPostSpikeHistory
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity flags
     *
     * Retrieves the current plasticity state flags that determine which learning
     * mechanisms are enabled for this synapse. Flags control Hebbian learning,
     * STDP, and reward-modulated learning.
     *
     * @return const PlasticityFlags& Constant reference to the plasticity flags
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Plasticity flags include:
     * - hebbian: Enable Hebbian learning
     * - stdp: Enable spike-timing dependent plasticity
     * - rewardModulated: Enable reward-modulated learning
     *
     * @see getPlasticityFlags (non-const version)
     * @see enablePlasticity
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     *
     * Retrieves a non-constant reference to the plasticity flags, allowing modification
     * of plasticity state. This is used for enabling/disabling learning mechanisms
     * and for internal plasticity computations.
     *
     * @return PlasticityFlags& Non-constant reference to the plasticity flags
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note This method provides controlled access to the synapse's plasticity state
     *       for use in learning algorithms and plasticity management.
     *
     * @see getPlasticityFlags (const version)
     * @see enablePlasticity
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules
     *
     * Configures which plasticity mechanisms are enabled for this synapse. This
     * controls learning based on different principles: Hebbian learning, STDP,
     * and reward-modulated learning.
     *
     * @param hebbian Enable Hebbian learning (neurons that fire together wire together)
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated learning
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Plasticity configuration includes:
     * - Hebbian learning for correlated firing
     * - STDP for precise spike timing relationships
     * - Reward-modulated learning for reinforcement
     * - Independent control of different mechanisms
     *
     * @see getPlasticityFlags
     * @see enablePlasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace for reward-modulated learning
     *
     * Retrieves the current eligibility trace value, which is used in reward-modulated
     * learning to determine how synaptic weights should be updated based on reward signals.
     *
     * @return float The current eligibility trace value
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Eligibility traces:
     * - Store recent synaptic activity
     * - Decay over time
     * - Scale reward signals
     * - Enable delayed reward effects
     *
     * @see setEligibilityTrace
     * @see decayEligibilityTrace
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace for reward-modulated learning
     *
     * Assigns a new value to the eligibility trace, which is used in reward-modulated
     * learning to determine how synaptic weights should be updated based on reward signals.
     *
     * @param trace The eligibility trace value to set
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if trace is invalid
     *
     * @note Eligibility traces are used for:
     * - Reward-modulated synaptic plasticity
     * - Delayed reward effects
     * - Credit assignment in reinforcement learning
     * - Temporal difference learning
     *
     * @see getEligibilityTrace
     * @see decayEligibilityTrace
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace
     *
     * Gradually reduces the eligibility trace value over time, simulating the
     * decay of synaptic eligibility in reward-modulated learning. This is essential
     * for implementing temporal credit assignment.
     *
     * @param decayRate The decay rate (0.0 to 1.0, where 1.0 means complete decay per step)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if decayRate is invalid
     *
     * @note Decay formula: trace = trace * (1.0 - decayRate)
     * - decayRate = 0.0: No decay (trace remains constant)
     * - decayRate = 1.0: Complete decay per step
     * - Typical values: 0.001-0.1 for biological plausibility
     *
     * @see getEligibilityTrace
     * @see setEligibilityTrace
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy for use-dependent modulation
     *
     * Retrieves the current synaptic efficacy value, which represents the use-dependent
     * modulation of synaptic transmission based on recent activity patterns. Efficacy
     * can vary independently of synaptic weight.
     *
     * @return float The current synaptic efficacy (typically 0.0-2.0)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Synaptic efficacy is influenced by:
     * - Recent spike patterns
     * - Short-term plasticity state
     * - Activity history
     * - Learning mechanisms
     *
     * @see setEfficacy
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy for use-dependent modulation
     *
     * Assigns a new value to the synaptic efficacy, which represents the use-dependent
     * modulation of synaptic transmission. Efficacy affects weight changes during
     * plasticity but operates independently of the baseline synaptic weight.
     *
     * @param efficacy The synaptic efficacy to set (typically 0.0-2.0)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if efficacy is invalid
     *
     * @note Efficacy modulation includes:
     * - Use-dependent changes based on activity
     * - Independence from baseline weight
     * - Impact on plasticity rules
     * - Temporary modulation of transmission
     *
     * @see getEfficacy
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     *
     * Advances the synapse's state by one simulation step, updating short-term
     * plasticity dynamics, eligibility traces, and implementing synaptic transmission
     * rules. This is the core method for synaptic dynamics in neural computation.
     *
     * @param currentTime The current simulation time (must be finite and non-negative)
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::InvalidParameter if currentTime is invalid
     * @throws NLMErrors::IntegrationFailed if synaptic dynamics computation fails
     *
     * @note The step method implements:
     * - Short-term plasticity dynamics (facilitation and depression)
     * - Eligibility trace decay for reward-modulated learning
     * - Weight bounds clamping to prevent instability
     * - Logging for synaptic activity tracking
     *
     * @see getPreSpikeHistory
     * @see getPostSpikeHistory
     * @see getEligibilityTrace
     * @see decayEligibilityTrace
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     *
     * Resets the synapse to its initial state, clearing all dynamic variables
     * and resetting synaptic parameters to default values. This is used for
     * simulation restarts, clearing synaptic memory, or preparing for new
     * computational tasks.
     *
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     *
     * @note Reset includes:
     * - Clearing spike history
     * - Resetting weight to zero
     * - Resetting efficacy to 1.0
     * - Resetting short-term plasticity state
     * - Resetting eligibility trace to zero
     * - Logging for reset tracking
     *
     * @see step
     * @see resetToDefault
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     *
     * Initializes the synapse with biologically plausible random parameters
     * based on the synapse type and biological constraints. This is used for
     * network initialization, developmental processes, or generating diverse
     * synaptic connectivity patterns.
     *
     * @param rng Random number generator for biological parameter variability
     * @throws NLMErrors::SystemNotInitialized if the Synapse object is not properly initialized
     * @throws NLMErrors::DevelopmentError if random initialization fails
     *
     * @note Random initialization includes:
     * - Type-specific parameter generation
     * - Biologically plausible ranges
     * - Connectivity pattern establishment
     * - Developmental stage constraints
     *
     * For excitatory synapses: weight (0.1-0.4), efficacy (0.8-1.0)
     * For inhibitory synapses: weight (-0.1 to -0.4), efficacy (0.8-1.0)
     * For other types: weight (-0.1 to 0.1), efficacy (0.9-1.0)
     *
     * @see getType
     * @see setWeight
     * @see setEfficacy
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
