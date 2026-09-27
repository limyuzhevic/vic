/**
 * @file Synapse.hpp
 * @brief Synaptic connection implementation with transmission dynamics
 * 
 * This header defines the Synapse class, representing a synaptic connection
 * between two neurons in the NLM framework. Synapses are fundamental components
 * of neural networks, transmitting signals from presynaptic to postsynaptic
 * neurons with delay, plasticity, and dynamic modulation.
 * 
 * @details Synapse implementation features:
 * - Chemical synaptic transmission with configurable delay
 * - Excitatory/Inhibitory signal modulation
 * - Spike-timing dependent plasticity (STDP) support
 * - Hebbian and reward-modulated learning rules
 * - Short-term synaptic plasticity mechanisms
 * - Eligibility traces for delayed reward learning
 * - Synaptic efficacy modulation
 * - Complete spike history tracking for plasticity
 * - Integration with neuron connectivity management
 * 
 * @note Synapses implement the core communication mechanism in neural networks:
 * - **Transmission**: Carry signals from source to destination neurons
 * - **Modulation**: Adjust signal strength based on synaptic properties
 * - **Learning**: Modify connection strength based on activity
 * - **Timing**: Introduce delays and timing relationships between neurons
 * 
 * Synaptic dynamics are essential for:
 * - Implementing neural computation and information processing
 * - Creating complex network behaviors from simple components
 * - Supporting learning and adaptation in neural systems
 * - Implementing biological-like neural computation
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Synapse parameters must be consistent with simulation time step
 * (dt). Delay values and plasticity parameters must be validated for numerical
 * stability and biological plausibility.
 * 
 * @see SynapseState for synapse state representation
 * @see Neuron for neural endpoints
 * @see PlasticityRule for learning mechanisms
 * @see RandomGenerator for random parameter initialization
 * @see Brain for synapse network management
 * @see NLMException for error conditions
 * 
 * @ingroup NeuralComponents
 */
#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @struct SynapseState
 * @brief Complete state representation of a synaptic connection
 * 
 * SynapseState provides a comprehensive snapshot of a synaptic connection's
 * current state, including weight, delay, type, and learning state. This
 * structure is designed for efficient storage and fast access during simulation.
 * 
 * @details The state structure contains:
 * - Synaptic weight for signal transmission strength
 * - Delay for transmission timing
 * - Type for excitatory/inhibitory classification
 * - Spike history for plasticity mechanisms
 * - Plasticity flags for learning rule activation
 * - Eligibility trace for reward-modulated learning
 * - Synaptic efficacy for use-dependent modulation
 * 
 * All values use appropriate data types for consistency and unit compatibility.
 * The structure is designed to be copied efficiently and provide thread-safe
 * read access during simulation.
 * 
 * @note SynapseState is designed for vectorization and cache efficiency.
 * Fields are ordered by access frequency and data type for optimal memory layout.
 * 
 * @see Synapse for methods accessing state
 * @see SynapticWeight for weight data type
 * @see Delay for delay data type
 * @see PlasticityFlags for plasticity flag structure
 */
struct SynapseState {
    SynapticWeight weight;              ///< Synaptic weight (dimensionless)
    Delay delay;                        ///< Transmission delay (simulation steps)
    SynapseType type;                   ///< Excitatory/Inhibitory type
    std::vector<Timestamp> preSpikeHistory;   ///< Presynaptic spike timestamps
    std::vector<Timestamp> postSpikeHistory;  ///< Postsynaptic spike timestamps
    PlasticityFlags plasticityFlags;     ///< Plasticity rule activation flags
    float eligibilityTrace;             ///< Eligibility trace for reward learning
    float efficacy;                      ///< Synaptic efficacy (use-dependent modulation)
    
    /**
     * @brief Default constructor initializes synapse to default state
     * 
     * Sets the synapse to a biologically plausible default state:
     * - Weight at 1.0 (standard connection strength)
     * - Delay at 1 step (minimal transmission delay)
     * - Type at Excitatory (default neural communication)
     * - No spike history
     * - No plasticity enabled
     * - Zero eligibility trace
     * - Efficacy at 1.0 (baseline)
     * 
     * @note This represents a standard, unstimulated synaptic connection
     * ready for use in neural simulations.
     */
    SynapseState()
        : weight(1.0f)
        , delay(1)
        , type(SynapseType::Excitatory)
        , preSpikeHistory()
        , postSpikeHistory()
        , plasticityFlags()
        , eligibilityTrace(0.0f)
        , efficacy(1.0f) {}
};

/**
 * @class Synapse
 * @brief Synaptic connection implementing neural transmission and learning
 * 
 * Synapse represents a single synaptic connection between two neurons in the
 * NLM framework. It implements signal transmission with configurable delay,
 * supports multiple plasticity mechanisms, and provides dynamic modulation
 * of synaptic properties based on activity.
 * 
 * @details Synapse capabilities:
 * - **Transmission**: Signal propagation from presynaptic to postsynaptic neuron
 * - **Delay management**: Configurable transmission delay in simulation steps
 * - **Modulation**: Excitatory/inhibitory signal control
 * - **Learning**: STDP, Hebbian, and reward-modulated plasticity
 * - **Dynamic properties**: Eligibility traces and synaptic efficacy
 * - **History tracking**: Complete spike history for learning rules
 * - **Network integration**: Neuron connectivity management
 * - **State management**: Complete synaptic state for simulation
 * - **Random initialization**: Reproducible synapse setup
 * 
 * **Synaptic Transmission**:
 * The synapse implements signal transmission:
 * - Receives spike events from presynaptic neuron
 * - Applies delay based on synapse properties
 * - Transmits to postsynaptic neuron with appropriate effect
 * - Modulates based on synaptic weight and type
 * 
 * **Plasticity Mechanisms**:
 * Synapses support multiple learning rules:
 * - **STDP**: Spike-timing dependent plasticity
 * - **Hebbian**: Co-activity based strengthening
 * - **Reward-modulated**: Delayed reward learning with eligibility traces
 * 
 * **Dynamic Properties**:
 * - **Eligibility trace**: Tracks presynaptic activity for delayed reward
 * - **Synaptic efficacy**: Use-dependent modulation of transmission
 * - **Short-term plasticity**: Activity-dependent modulation
 * 
 * **Usage Example**:
 * @code
 * nlm::Synapse synapse(0, 1, 2); // Synapse ID 0, source neuron 1, destination neuron 2
 * synapse.setWeight(0.5f);
 * synapse.setDelay(2); // 2-step delay
 * synapse.setType(nlm::SynapseType::Inhibitory);
 * synapse.initializeRandom(rng);
 * 
 * // Simulation loop
 * while (simulation_steps-- > 0) {
 *     synapse.step(current_time);
 *     // Process spike transmission
 * }
 * @endcode
 * 
 * **Network Integration**:
 * Synapses connect neurons into computational networks:
 * - Form the fundamental communication links
 * - Enable complex information flow patterns
 * - Support learning and adaptation
 * - Implement biological-like neural computation
 * 
 * @see SynapseState for detailed state representation
 * @see Neuron for neural endpoints
 * @see PlasticityRule for learning mechanisms
 * @see Brain for synapse network management
 * @see NLMException for error conditions
 * 
 * @warning Synapse parameters must be validated for simulation stability.
 * Invalid delays, weights, or plasticity parameters can cause simulation
 * errors or unrealistic behavior.
 * 
 * @note Synapses use the pImpl idiom for efficient memory management and
 * forward declaration of the implementation class.
 * 
 * @ingroup NeuralComponents
 */
class Synapse {
public:
    /**
     * @brief Construct a synaptic connection
     * 
     * Creates a new synaptic connection between two neurons with a unique
     * identifier. The synapse is initialized with default parameters and
     * ready for use in neural simulations.
     * 
     * @param id Unique identifier for this synapse
     * @param source NeuronId of the presynaptic neuron (source of signal)
     * @param destination NeuronId of the postsynaptic neuron (destination of signal)
     * 
     * @throws NLMAssertionError if source or destination neurons are invalid
     * 
     * @see getId() for retrieving synapse identifier
     * @see getSourceNeuron() for retrieving source neuron
     * @see getDestinationNeuron() for retrieving destination neuron
     * @see initializeRandom() for random parameter initialization
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2); // Source neuron 1, destination neuron 2
     * synapse.setWeight(0.8f); // 80% transmission strength
     * synapse.setDelay(3); // 3-step delay
     * @endcode
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor
     * 
     * Cleans up synapse resources and performs any necessary cleanup operations.
     * Memory management is handled via the pImpl pattern.
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor is deleted (use move semantics instead)
     * 
     * Synapses cannot be copied due to their complex internal state and
     * implementation details. Use move semantics for efficient transfer.
     * 
     * @param other Source synapse to copy (deleted)
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     * 
     * Synapses cannot be assigned due to their complex internal state and
     * implementation details. Use move assignment for efficient transfer.
     * 
     * @param other Source synapse to assign (deleted)
     * @return Synapse& Reference to assigned synapse (never returned)
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor (efficient)
     * 
     * Transfers ownership of synapse implementation from source to new synapse
     * without copying complex internal state. This provides efficient synapse
     * transfer and reassignment.
     * 
     * @param other Source synapse to move from
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient)
     * 
     * Transfers ownership of synapse implementation from source to existing synapse
     * without copying complex internal state. This provides efficient synapse
     * reassignment and transfer.
     * 
     * @param other Source synapse to move from
     * @return Synapse& Reference to this synapse
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId Unique identifier for this synapse
     * 
     * @see getSourceNeuron() for retrieving source neuron
     * @see getDestinationNeuron() for retrieving destination neuron
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(123, 1, 2);
     * SynapseId id = synapse.getId();
     * std::cout << "Synapse ID: " << id << std::endl;
     * @endcode
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron's identifier
     * 
     * @return NeuronId Identifier of the presynaptic neuron (signal source)
     * 
     * @see getDestinationNeuron() for retrieving destination neuron
     * @see getId() for retrieving synapse identifier
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * NeuronId source = synapse.getSourceNeuron();
     * std::cout << "Source neuron: " << source << std::endl;
     * @endcode
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron's identifier
     * 
     * @return NeuronId Identifier of the postsynaptic neuron (signal destination)
     * 
     * @see getSourceNeuron() for retrieving source neuron
     * @see getId() for retrieving synapse identifier
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * NeuronId dest = synapse.getDestinationNeuron();
     * std::cout << "Destination neuron: " << dest << std::endl;
     * @endcode
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the synaptic weight
     * 
     * @return SynapticWeight Current synaptic weight (dimensionless)
     * 
     * @see setWeight() for setting synaptic weight
     * @see addToWeight() for incremental weight changes
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * float weight = synapse.getWeight();
     * std::cout << "Synapse weight: " << weight << std::endl;
     * @endcode
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic weight (dimensionless)
     * 
     * @see getWeight() for retrieving synaptic weight
     * @see addToWeight() for incremental weight changes
     * 
     * @warning Weight values should typically be between 0.0 and 1.0 for
     * biological plausibility, but can be outside this range for certain
     * computational models.
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.setWeight(0.75f); // 75% transmission strength
     * @endcode
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add delta to synaptic weight
     * 
     * Performs incremental update to synaptic weight, useful for plasticity
     * learning rules that modify connection strength gradually.
     * 
     * @param delta Change in synaptic weight
     * 
     * @see getWeight() for retrieving synaptic weight
     * @see setWeight() for setting absolute weight
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.addToWeight(0.1f);  // Strengthen connection
     * synapse.addToWeight(-0.05f); // Weaken connection
     * @endcode
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic delay
     * 
     * @return Delay Transmission delay in simulation steps
     * 
     * @see setDelay() for setting synaptic delay
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic delay
     * 
     * @param delay Transmission delay in simulation steps
     * 
     * @see getDelay() for retrieving synaptic delay
     * 
     * @warning Delay values must be positive and compatible with the
     * simulation time step. Excessive delays can cause simulation instability
     * or memory issues.
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.setDelay(5); // 5-step transmission delay
     * @endcode
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType Synapse type (Excitatory or Inhibitory)
     * 
     * @see setType() for setting synapse type
     * @see isExcitatory() for checking excitatory type
     * @see isInhibitory() for checking inhibitory type
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * nlm::SynapseType type = synapse.getType();
     * if (type == nlm::SynapseType::Excitatory) {
     *     std::cout << "Excitatory synapse" << std::endl;
     * }
     * @endcode
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type Synapse type (Excitatory or Inhibitory)
     * 
     * @see getType() for retrieving synapse type
     * @see isExcitatory() for checking excitatory type
     * @see isInhibitory() for checking inhibitory type
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.setType(nlm::SynapseType::Inhibitory);
     * @endcode
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return bool true if synapse type is Excitatory, false otherwise
     * 
     * @see getType() for retrieving synapse type
     * @see isInhibitory() for checking inhibitory type
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return bool true if synapse type is Inhibitory, false otherwise
     * 
     * @see getType() for retrieving synapse type
     * @see isExcitatory() for checking excitatory type
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike event
     * 
     * Records a spike from the presynaptic neuron in the synapse's history.
     * This spike event is used for STDP (Spike-Timing-Dependent Plasticity)
     * and other timing-dependent learning rules.
     * 
     * @param timestamp Time of presynaptic spike (ms)
     * 
     * @see recordPostSpike() for recording postsynaptic spikes
     * @see getPreSpikeHistory() for retrieving presynaptic spike history
     * @see clearHistory() for clearing spike history
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.recordPreSpike(100.0f); // Presynaptic spike at 100 ms
     * @endcode
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike event
     * 
     * Records a spike from the postsynaptic neuron in the synapse's history.
     * This spike event is used for STDP and other timing-dependent learning rules.
     * 
     * @param timestamp Time of postsynaptic spike (ms)
     * 
     * @see recordPreSpike() for recording presynaptic spikes
     * @see getPostSpikeHistory() for retrieving postsynaptic spike history
     * @see clearHistory() for clearing spike history
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.recordPostSpike(102.0f); // Postsynaptic spike at 102 ms
     * @endcode
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the presynaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of presynaptic spike timestamps
     * 
     * @see recordPreSpike() for recording presynaptic spikes
     * @see getPostSpikeHistory() for retrieving postsynaptic spike history
     * @see clearHistory() for clearing spike history
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the postsynaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike timestamps
     * 
     * @see recordPostSpike() for recording postsynaptic spikes
     * @see getPreSpikeHistory() for retrieving presynaptic spike history
     * @see clearHistory() for clearing spike history
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Removes all recorded spike events from both presynaptic and postsynaptic
     * history. This is useful for resetting synapse state between trials or
     * simulation modes.
     * 
     * @see recordPreSpike() for recording presynaptic spikes
     * @see recordPostSpike() for recording postsynaptic spikes
     * @see getPreSpikeHistory() for retrieving presynaptic spike history
     * @see getPostSpikeHistory() for retrieving postsynaptic spike history
     */
    void clearHistory();
    
    /**
     * @brief Get the synapse's plasticity flags
     * 
     * @return const PlasticityFlags& Constant reference to plasticity flags
     * 
     * @see getPlasticityFlags() for mutable access
     * @see enablePlasticity() for enabling plasticity rules
     * @see PlasticityFlags for detailed flag structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to synapse's plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * 
     * @see getPlasticityFlags() for constant access
     * @see enablePlasticity() for enabling plasticity rules
     * @see PlasticityFlags for detailed flag structure
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules
     * 
     * Configures which plasticity mechanisms are active for this synapse.
     * Plasticity allows synapses to strengthen or weaken based on activity.
     * 
     * @param hebbian Enable Hebbian plasticity (true) or disable (false)
     * @param stdp Enable STDP (true) or disable (false)
     * @param rewardModulated Enable reward-modulated plasticity (true) or disable (false)
     * 
     * @see getPlasticityFlags() for retrieving plasticity flags
     * @see PlasticityFlags for detailed flag structure
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace
     * 
     * @return float Current eligibility trace value
     * 
     * @see setEligibilityTrace() for setting eligibility trace
     * @see decayEligibilityTrace() for decaying eligibility trace
     * 
     * @note Eligibility trace is used for reward-modulated learning
     * to track presynaptic activity for delayed reward signals.
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace
     * 
     * @param trace New eligibility trace value
     * 
     * @see getEligibilityTrace() for retrieving eligibility trace
     * @see decayEligibilityTrace() for decaying eligibility trace
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * synapse.setEligibilityTrace(0.5f);
     * @endcode
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace
     * 
     * Applies decay to the eligibility trace, typically based on simulation
     * time step. This implements the temporal decay of eligibility signals
     * in reward-modulated learning.
     * 
     * @param decayRate Decay rate (0.0 to 1.0)
     * 
     * @see getEligibilityTrace() for retrieving eligibility trace
     * @see setEligibilityTrace() for setting eligibility trace
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the synaptic efficacy
     * 
     * @return float Current synaptic efficacy (use-dependent modulation)
     * 
     * @see setEfficacy() for setting synaptic efficacy
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * @param efficacy New synaptic efficacy value
     * 
     * @see getEfficacy() for retrieving synaptic efficacy
     * 
     * @note Synaptic efficacy represents use-dependent modulation of
     * synaptic transmission, often used to implement short-term plasticity.
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Perform one simulation step update
     * 
     * Updates the synapse for one simulation step, implementing synaptic
     * dynamics including transmission, plasticity, and dynamic modulation.
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @note This method implements the core synaptic dynamics:
     * - Process spike transmission with delay
     * - Update plasticity based on spike timing
     * - Decay eligibility traces
     * - Modulate synaptic efficacy
     * 
     * @see getPreSpikeHistory() for presynaptic spike history
     * @see getPostSpikeHistory() for postsynaptic spike history
     * @see getEligibilityTrace() for eligibility trace
     * @see getEfficacy() for synaptic efficacy
     * @see PlasticityRule for plasticity implementation
     * 
     * @warning Current TODO comment indicates this method needs implementation.
     * It currently serves as a placeholder for future development.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * Resets all synapse state variables to their initial values:
     * - Weight to default value (1.0)
     * - Delay to default value (1 step)
     * - Type to default (Excitatory)
     * - Spike history to empty
     * - Plasticity flags to default
     * - Eligibility trace to zero
     * - Efficacy to default (1.0)
     * 
     * This method is useful for:
     * - Resetting synapse state between trials
     * - Initializing synapse for new simulation
     * - Clearing effects of previous simulation
     * 
     * @see getState() for current state (if available)
     * @see reset() for reset behavior
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     * 
     * Sets synapse parameters to random values using the provided random number
     * generator. This ensures reproducible synapse initialization and helps
     * explore parameter space.
     * 
     * @param rng Reference to random number generator
     * 
     * @see RandomGenerator for random number generation
     * @see getId() for synapse identification
     * @see getSourceNeuron() for source neuron identification
     * @see getDestinationNeuron() for destination neuron identification
     * 
     * @par Example
     * @code
     * nlm::Synapse synapse(42, 1, 2);
     * nlm::RandomGenerator rng(12345); // Fixed seed for reproducibility
     * synapse.initializeRandom(rng);
     * 
     * // Verify initialization
     * std::cout << "Synapse initialized with ID: " << synapse.getId() << std::endl;
     * std::cout << "Source neuron: " << synapse.getSourceNeuron() << std::endl;
     * std::cout << "Destination neuron: " << synapse.getDestinationNeuron() << std::endl;
     * @endcode
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
