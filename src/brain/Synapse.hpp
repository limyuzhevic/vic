#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Represents a synaptic connection between neurons with transmission dynamics
 * 
 * The Synapse class models biological synaptic transmission between pre- and post-synaptic
 * neurons. It implements comprehensive synaptic dynamics including:
 * 
 * **Transmission Properties:**
 * - Synaptic weight representing connection strength
 * - Transmission delay in simulation steps
 * - Synaptic type (excitatory, inhibitory, modulatory)
 * 
 * **Plasticity Mechanisms:**
 * - Spike-timing dependent plasticity (STDP) eligibility traces
 * - Hebbian learning rules
 * - Reward-modulated plasticity
 * - Synaptic efficacy (use-dependent modulation)
 * 
 * **Short-Term Plasticity:**
 * - Synaptic depression (utilization factor)
 * - Synaptic facilitation (recovery factor)
 * 
 * **Biological Plausibility:**
 * - Real spike history tracking for both pre- and post-synaptic neurons
 * - Activity-dependent synaptic dynamics
 * - Weight normalization to maintain stability
 * 
 * The synapse operates as a fundamental information transmission unit in the neural network,
 * propagating signals from source to target neurons while maintaining plasticity state for learning.
 * 
 * @note Uses Pimpl idiom for efficient implementation and memory management
 * @warning Thread safety must be ensured by the caller when accessing synapse state concurrently
 * 
 * @tparam SynapseId Type for unique synapse identification
 * @tparam NeuronId Type for neuron identification
 * @tparam SynapticWeight Type for synaptic strength (typically float)
 * @tparam Timestamp Type for time tracking
 * @tparam PlasticityFlags Type for plasticity state flags
 * 
 * @see Neuron for pre-synaptic dynamics
 * @see PlasticityRule for plasticity mechanisms
 * @see STDP for spike-timing dependent plasticity implementation
 */

class Synapse {
public:
    /**
     * @brief Construct a new Synapse object
     * 
     * Creates a synaptic connection with specified identity and connectivity. This constructor
     * establishes the fundamental connection parameters between pre- and post-synaptic neurons.
     * 
     * @param id Unique identifier for this synapse
     * @param source Neuron identifier of the pre-synaptic neuron (sender)
     * @param destination Neuron identifier of the post-synaptic neuron (receiver)
     * 
     * @note The synapse is initialized with default parameters:
     *       - Weight: 0.0f
     *       - Delay: 1 simulation step  
     *       - Type: Excitatory
     *       - Plasticity factors: Disabled by default
     * 
     * @pre Both source and destination neuron IDs must be valid and unique
     * @post Synapse object is fully initialized and ready for transmission
     * 
     * @see setWeight() for adjusting connection strength
     * @see setDelay() for modifying transmission timing
     * @see setType() for specifying synaptic effect type
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    /**
     * @brief Copy constructor is deleted to prevent copying
     * 
     * Synapses are managed objects with unique identities. Copying would violate
     * the one-synapse-per-connection model and lead to duplicate state.
     * 
     * @param other Another synapse object (unused)
     * 
     * @throws std::runtime_error if attempted copy
     * @note Use move semantics for transferring synapse ownership
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * Efficiently transfers synapse ownership from source to destination without copying.
     * This is the preferred way to transfer synapse objects.
     * 
     * @param other Source synapse object to transfer ownership from
     * @post Source synapse becomes invalid (null implementation pointer)
     * @throws None - noexcept ensures no exceptions
     * 
     * @note After move, the source synapse cannot be used
     * @see operator=() for move assignment
     */
    Synapse(Synapse&&) noexcept;
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * Efficiently transfers synapse ownership from source to destination without copying.
     * Self-assignment is handled safely.
     * 
     * @param other Source synapse object to transfer ownership from
     * @return Reference to this synapse (for chaining)
     * @post Source synapse becomes invalid (null implementation pointer)
     * @throws None - noexcept ensures no exceptions
     * 
     * @note After move, the source synapse cannot be used
     * @see Synapse(Synapse&&) for move constructor
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this synapse
     * 
     * @return SynapseId The unique identifier for this synapse
     * @note This ID remains constant throughout the synapse's lifetime
     * @see getSourceNeuron() to get the pre-synaptic neuron
     * @see getDestinationNeuron() to get the post-synaptic neuron
     */
    SynapseId getId() const;
    
    // Connection
    /**
     * @brief Get the pre-synaptic neuron identifier
     * 
     * @return NeuronId Identifier of the neuron that sends signals through this synapse
     * @see getDestinationNeuron() to get the receiving neuron
     * @see getId() to get the synapse's own identifier
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the post-synaptic neuron identifier
     * 
     * @return NeuronId Identifier of the neuron that receives signals through this synapse
     * @see getSourceNeuron() to get the sending neuron
     * @see getId() to get the synapse's own identifier
     */
    NeuronId getDestinationNeuron() const;
    
    // Weight
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current connection strength (-1.0 to 1.0 typical range)
     * @see setWeight() to modify connection strength
     * @see addToWeight() to adjust connection strength incrementally
     * @note Weight represents the efficacy of synaptic transmission
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight (connection strength)
     * 
     * @param weight New connection strength
     * @pre Weight must be within valid bounds [-1.0, 1.0]
     * @post Synaptic transmission strength updated immediately
     * @see getWeight() to retrieve current weight
     * @see addToWeight() for incremental adjustment
     * @warning This affects neural computation directly
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Incrementally adjust synaptic weight
     * 
     * @param delta Change in connection strength (positive or negative)
     * @post Weight updated by delta amount, clamped to valid bounds
     * @see setWeight() for absolute weight setting
     * @see getWeight() to verify new weight value
     * @note Preserves plasticity state while updating connection strength
     */
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    /**
     * @brief Get the synaptic transmission delay
     * 
     * @return Delay Number of simulation steps before signal reaches post-synaptic neuron
     * @see setDelay() to modify transmission timing
     * @note Shorter delays enable faster network dynamics
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic transmission delay
     * 
     * @param delay Number of simulation steps before signal reaches post-synaptic neuron
     * @pre Delay must be non-negative
     * @post Signal transmission timing updated immediately
     * @see getDelay() to retrieve current delay
     * @note Delays introduce temporal dynamics in neural processing
     */
    void setDelay(Delay delay);
    
    // Type
    /**
     * @brief Get the synaptic type
     * 
     * @return SynapseType Type of this synaptic connection
     * @see setType() to modify synaptic effect type
     * @note Different types have different transmission characteristics
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synaptic type
     * 
     * @param type Type of synaptic connection (excitatory, inhibitory, modulatory)
     * @post Synaptic transmission characteristics updated
     * @see getType() to retrieve current type
     * @note Type affects both transmission and plasticity rules
     */
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    /**
     * @brief Check if this is an excitatory synapse
     * 
     * @return bool true if synapse releases positive current to post-synaptic neuron
     * @see getType() to get the detailed synaptic type
     * @see isInhibitory() to check for inhibitory effect
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if this is an inhibitory synapse
     * 
     * @return bool true if synapse releases negative current (hyperpolarizes) to post-synaptic neuron
     * @see getType() to get the detailed synaptic type
     * @see isExcitatory() to check for excitatory effect
     */
    bool isInhibitory() const;
    
    // Activity history for plasticity
    /**
     * @brief Record a pre-synaptic spike time for plasticity computation
     * 
     * @param timestamp Time when pre-synaptic neuron spiked
     * @post Spike timestamp added to history buffer (if not duplicate)
     * @see recordPostSpike() for post-synaptic spike recording
     * @see getPreSpikeHistory() to retrieve spike times
     * @see step() for when this history is used in plasticity
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a post-synaptic spike time for plasticity computation
     * 
     * @param timestamp Time when post-synaptic neuron spiked
     * @post Spike timestamp added to history buffer (if not duplicate)
     * @see recordPreSpike() for pre-synaptic spike recording
     * @see getPostSpikeHistory() to retrieve spike times
     * @see step() for when this history is used in plasticity
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of pre-synaptic spike times
     * 
     * @return const std::vector<Timestamp>& Reference to vector containing spike times
     * @see recordPreSpike() to add spike times
     * @see clearHistory() to reset history
     * @note History is used by STDP and other plasticity rules
     * @warning Vector reference is invalidated by clearHistory()
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the history of post-synaptic spike times
     * 
     * @return const std::vector<Timestamp>& Reference to vector containing spike times
     * @see recordPostSpike() to add spike times
     * @see clearHistory() to reset history
     * @note History is used by STDP and other plasticity rules
     * @warning Vector reference is invalidated by clearHistory()
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history for plasticity computation
     * 
     * @post All pre- and post-synaptic spike histories are cleared
     * @see recordPreSpike() to start recording spikes
     * @see recordPostSpike() to start recording spikes
     * @note Used when synapse is reset or deactivated
     * @warning Clears both pre- and post-synaptic spike histories
     */
    void clearHistory();
    
    // Plasticity flags
    /**
     * @brief Get the current plasticity state flags
     * 
     * @return const PlasticityFlags& Constant reference to plasticity flags
     * @see getPlasticityFlags() to get mutable reference
     * @see enablePlasticity() to modify plasticity rules
     * @note Flags control which plasticity mechanisms are active
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * @see getPlasticityFlags() const for constant version
     * @note Modifying flags affects plasticity computation
     * @warning Ensure thread safety when modifying flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable or disable plasticity rules
     * 
     * @param hebbian Enable/disable Hebbian learning
     * @param stdp Enable/disable spike-timing dependent plasticity
     * @param rewardModulated Enable/disable reward-modulated plasticity
     * @post Plasticity flags updated accordingly
     * @see getPlasticityFlags() to check current settings
     * @see step() to apply plasticity during simulation
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    /**
     * @brief Get the current eligibility trace value
     * 
     * @return float Current eligibility trace (-1.0 to 1.0 typical range)
     * @see setEligibilityTrace() to set eligibility trace
     * @see decayEligibilityTrace() to decay the trace
     * @note Tracks synapse's readiness for weight changes based on reward signals
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace New eligibility trace value
     * @pre Trace should be within valid bounds [-1.0, 1.0]
     * @post Eligibility trace updated immediately
     * @see getEligibilityTrace() to retrieve current trace
     * @see decayEligibilityTrace() to apply temporal decay
     * @note Used in reward-modulated plasticity for credit assignment
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace over time
     * 
     * @param decayRate Rate of decay per simulation step (0.0 to 1.0)
     * @post Eligibility trace reduced by decay factor
     * @see setEligibilityTrace() to set initial trace
     * @see getEligibilityTrace() to check current value
     * @note Implements temporal forgetting of plasticity eligibility
     */
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    /**
     * @brief Get the current synaptic efficacy
     * 
     * @return float Current synaptic efficacy (0.0 to 1.0 typical range)
     * @see setEfficacy() to set efficacy
     * @note Represents use-dependent modulation of synaptic strength
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy (use-dependent modulation)
     * 
     * @param efficacy New synaptic efficacy value (0.0 to 1.0 typical range)
     * @pre Efficacy must be within valid bounds [0.0, 1.0]
     * @post Synaptic modulation updated immediately
     * @see getEfficacy() to retrieve current efficacy
     * @note Models activity-dependent changes in synaptic responsiveness
     */
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    /**
     * @brief Update synapse state for one simulation timestep
     * 
     * @param currentTime Current simulation time (seconds)
     * @post Synapse state updated based on current dynamics
     * @see recordPreSpike() to record pre-synaptic spikes
     * @see recordPostSpike() to record post-synaptic spikes
     * @see decayEligibilityTrace() to apply temporal decay
     * @note Implements short-term plasticity and transmission dynamics
     * @warning This is called automatically during brain simulation
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset synapse to initial state
     * 
     * @post All dynamic state variables reset to initial values:
     *       - Weight restored to initial value (typically 0.0)
     *       - Spike histories cleared
     *       - Eligibility trace reset to 0.0
     *       - Synaptic efficacy restored to 1.0
     *       - Short-term plasticity state reset
     * @see step() for normal simulation updates
     * @see initializeRandom() for re-initialization with new parameters
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize synapse with random biologically plausible parameters
     * 
     * @param rng Random number generator for parameter sampling
     * @post Synapse parameters set to random but plausible values:
     *       - Weight: Uniform(-1.0, 1.0)
     *       - Delay: Discrete uniform(1, 10) steps
     *       - Type: Random selection from SynapseType
     *       - Efficacy: Uniform(0.5, 1.0)
     * @see reset() for returning to default state
     * @see setWeight() for manual weight setting
     * @see setDelay() for manual delay setting
     * @note Uses biologically realistic parameter ranges
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
