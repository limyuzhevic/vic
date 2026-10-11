#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include "../core/Error/Error.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Represents a connection between neurons implementing synaptic transmission
 * @details This class models biological synapses with realistic properties including synaptic weight,
 * transmission delay, short-term plasticity, and various plasticity mechanisms (STDP, Hebbian, reward-modulated).
 * Synapses are the fundamental communication channels in the neural network, transmitting spikes from
 * presynaptic to postsynaptic neurons with configurable dynamics and learning capabilities.
 * 
 * The synapse implements multiple levels of biological realism:
 * - Static properties: weight, type, delay
 * - Short-term dynamics: depression and facilitation
 * - Plasticity mechanisms: STDP, Hebbian, and reward-modulated learning
 * - Eligibility traces for credit assignment
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI and efficient memory management.
 * Thread safety: Methods are not thread-safe; external synchronization required for concurrent access.
 * 
 * @warning Synaptic weights outside [-1.0, 1.0] may cause numerical instability
 * @note Default parameters are biologically inspired but application-specific tuning may be required
 */
class Synapse {
public:
    /**
     * @brief Constructor - creates synapse with full specification
     * @param id Unique identifier for this synapse (must be valid and non-conflicting)
     * @param source NeuronId of presynaptic (source) neuron
     * @param destination NeuronId of postsynaptic (target) neuron
     * @post Synapse initialized with biologically plausible defaults:
     *       - Weight: 0.0 (no initial connection)
     *       - Delay: 1 step (minimum biological delay)
     *       - Type: Excitatory (default for new connections)
     *       - Eligibility trace: 0.0 (no learning signal)
     *       - Efficacy: 1.0 (maximum initial synaptic efficacy)
     * @warning Creates new synapse entry in source/destination neuron maps
     * @throws std::invalid_argument if source/destination IDs are invalid
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor - cleans up synapse resources
     * @details Removes synapse from source/destination neuron maps
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor - deleted to prevent copying and ensure unique synapse identity
     * @details Synapses are identified by their unique ID and must not be duplicated
     * @warning Attempting to copy will result in compilation error
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor - enables efficient synapse transfer
     * @param other Synapse to move from (will be invalidated after move)
     * @post this->id = other->id, other->pImpl = nullptr (source is invalid)
     * @note Move semantics provide efficient transfer of ownership without deep copying
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient synapse reassignment
     * @param other Synapse to move from (will be invalidated after move)
     * @post this->id = other->id, other->pImpl = nullptr (source is invalid)
     * @return Reference to this synapse for chaining operations
     * @note Move assignment provides efficient transfer of ownership without deep copying
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this synapse
     * @return Synapse's unique identifier (guaranteed unique within the brain)
     * @post None
     * @note Synapse ID is immutable after creation and cannot be changed
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source (presynaptic) neuron identifier
     * @return NeuronId of the presynaptic neuron that sends signals
     * @post None
     * @note Defines directionality of synaptic transmission
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination (postsynaptic) neuron identifier
     * @return NeuronId of the postsynaptic neuron that receives signals
     * @post None
     * @note Defines directionality of synaptic transmission
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get current synaptic weight
     * @return Synaptic weight (positive = excitatory, negative = inhibitory)
     * @post None
     * @note Typical range: [-1.0, 1.0]. Weight zero = no connection
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight directly
     * @param weight New synaptic weight
     * @post Weight updated; eligibility trace reset; health status invalid
     * @warning Large weight changes may cause numerical instability
     * @throws NLMException if weight validation fails (outside allowed range)
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Increment synaptic weight by delta value
     * @param delta Weight change (positive for potentiation, negative for depression)
     * @post Weight updated by delta; automatically clamped to valid range
     * @note Used for gradual weight changes and plasticity implementations
     * @throws NLMException if delta causes invalid weight values
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get synaptic transmission delay
     * @return Delay in simulation steps (how many timesteps between spike emission and reception)
     * @post None
     * @note Higher values = slower synaptic transmission, can affect network dynamics
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic transmission delay
     * @param delay New delay in simulation steps
     * @post Delay updated; health status invalid
     * @warning Changes to delay affect temporal dynamics and spike timing
     * @throws NLMException if delay validation fails (negative or excessively large)
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get synapse type classification
     * @return Synapse type indicating functional role
     * @post None
     * @note Common types: Excitatory, Inhibitory, GapJunction
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     * @post Type updated; health status invalid
     * @warning Type changes affect synaptic dynamics and plasticity
     * @throws NLMException if type validation fails
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * @return true if synapse type is excitatory
     * @post None
     * @note Excitatory synapses increase postsynaptic membrane potential
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return true if synapse type is inhibitory
     * @post None
     * @note Inhibitory synapses decrease postsynaptic membrane potential
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike event for plasticity
     * @param timestamp Time of the presynaptic spike
     * @post Spike timestamp added to history; last pre-spike time updated
     * @warning Spike history used for STDP calculations and synaptic efficacy updates
     * @throws NLMException if timestamp validation fails
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike event for plasticity
     * @param timestamp Time of the postsynaptic spike
     * @post Spike timestamp added to history; last post-spike time updated
     * @warning Spike history used for STDP and Hebbian plasticity calculations
     * @throws NLMException if timestamp validation fails
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * @return Const reference to vector of presynaptic spike timestamps
     * @post History unchanged
     * @note Used for temporal difference calculations in STDP
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * @return Const reference to vector of postsynaptic spike timestamps
     * @post History unchanged
     * @note Used for temporal difference calculations in STDP
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     * @post Clears pre- and post-spike histories; resets last spike times
     * @note Used to reset synaptic plasticity state
     */
    void clearHistory();
    
    /**
     * @brief Get current plasticity flags
     * @return Const reference to plasticity flags structure
     * @post None
     * @note Controls which plasticity rules are active for this synapse
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable access to plasticity flags
     * @return Non-constant reference to plasticity flags
     * @post Flags may be modified by caller
     * @warning Direct flag manipulation bypasses validation
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity mechanisms
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @post Plasticity flags updated; health status invalid
     * @warning Disabeling plasticity may affect learning capabilities
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace value
     * @return Eligibility trace value (-1.0 to 1.0)
     * @post None
     * @note Used for reward-modulated learning and credit assignment
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace value
     * @param trace New eligibility trace value
     * @post Trace updated; health status invalid
     * @warning Trace values outside [-1.0, 1.0] may cause instability
     * @throws NLMException if trace validation fails
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace over time
     * @param decayRate Decay rate (0.0 = no decay, 1.0 = complete decay)
     * @post Trace value reduced by factor (1 - decayRate)
     * @note Implements temporal credit assignment for reward-modulated learning
     * @throws NLMException if decayRate validation fails
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy (use-dependent modulation)
     * @return Efficacy value (typically 0.0-2.0)
     * @post None
     * @note Represents how synaptic efficacy changes with usage history
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New efficacy value
     * @post Efficacy updated; health status invalid
     * @warning Efficacy affects synaptic strength and transmission
     * @throws NLMException if efficacy validation fails
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Main synaptic transmission and plasticity update
     * @param currentTime Current simulation time step
     * @post Updates synaptic state including short-term plasticity and eligibility traces
     * @note Implements complete synaptic dynamics:
     *       - Short-term plasticity (depression and facilitation)
     *       - Eligibility trace decay for reward-modulated learning
     *       - Weight clamping to prevent instability
     * @see recordPreSpike() for spike recording
     * @see decayEligibilityTrace() for explicit trace management
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * @post Resets all synaptic parameters to default values
     * @note Used for synaptic reset and error recovery
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random biologically plausible parameters
     * @param rng Reference to random number generator
     * @post Sets random weight, delay, and plasticity state
     * @note Based on synapse type:
     *       - Excitatory: positive weights (0.1-0.4)
     *       - Inhibitory: negative weights (-0.1 to -0.4)
     *       - Other types: small random weights (-0.1 to 0.1)
     * @throws std::exception if random generation fails
     */
    void initializeRandom(class RandomGenerator& rng);
    
    // Error handling and validation methods
    bool validateState() const;
    bool validateWeight() const;
    bool validateDelay() const;
    bool validateType() const;
    bool validateHistory() const;
    std::vector<std::string> getValidationIssues() const;
    
    // Error recovery methods
    void attemptRecovery();
    bool isRecoverable() const;
    
    // Health checking
    bool isHealthy() const;
    std::vector<std::string> getHealthIssues() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
