#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @file Synapse.hpp
 * 
 * Individual synapse implementation for NLM
 * 
 * This file provides the fundamental synaptic connection unit of the NLM
 * system. The Synapse class implements real synaptic transmission with
 * delays, short-term plasticity, and eligibility traces for learning. Each
 * synapse represents a connection between two neurons that enables
 * communication and information transfer in the neural network.
 * 
 * Synapses are the essential building blocks of neural computation,
 * implementing the communication mechanism through which neurons
 * influence each other. Each synapse maintains state representing:
 * - Connection parameters (source, destination, weight)
 * - Transmission dynamics (delay, type)
 * - Activity history (pre- and post-synaptic spikes)
 * - Plasticity eligibility (for learning)
 * - Synaptic efficacy (use-dependent modulation)
 * 
 * Key features:
 * - Precise spike timing-dependent transmission
 * - Short-term plasticity dynamics
 * - Eligibility traces for credit assignment
 * - Multiple plasticity mechanisms (Hebbian, STDP, reward-modulated)
 * - Synaptic weight modulation
 * - Delay and type specialization
 * - Excitatory/inhibitory differentiation
 * - Random initialization for reproducible networks
 * 
 * @note Critical for neural computation and learning
 * @note Used by all neurons for communication
 * @note Essential for synaptic plasticity and learning
 * @see Neuron.hpp - For neuron-level synapse management
 * @see STDP.hpp - For spike-timing-dependent plasticity
 * @see RandomGenerator.hpp - For random initialization
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @brief Forward declaration for Synapse dependencies
 */
class Neuron;

/**
 * @class Synapse
 * 
 * Individual synaptic connection implementing transmission between neurons.
 * 
 * The Synapse class represents a biological synapse that connects two
 * neurons and enables communication through synaptic transmission.
 * Each synapse implements realistic synaptic dynamics including
 * precise spike timing, short-term plasticity, and multiple learning
 * mechanisms. The synapse is the fundamental unit of neural computation
 * and learning in NLM.
 * 
 * The synapse model includes:
 * - **Connection**: Physical connection between source and destination neurons
 * - **Transmission**: Spike propagation with realistic delays
 * - **Plasticity**: Synaptic weight modification based on activity
 * - **Dynamics**: Use-dependent modulation of synaptic efficacy
 * - **History**: Recording of pre- and post-synaptic activity
 * - **Learning**: Eligibility traces for reward-modulated learning
 * 
 * Synaptic computation in NLM follows this basic cycle:
 * 1. Receive pre-synaptic spike (or inhibition)
 * 2. Apply transmission delay and dynamics
 * 3. Generate post-synaptic response
 * 4. Update eligibility traces (if learning enabled)
 * 5. Update spike history for plasticity
 * 6. Apply plasticity rules
 * 7. Update synaptic efficacy
 * 
 * @note Essential for neural computation and communication
 * @note Used by all neurons for synaptic transmission
 * @note Critical for learning and adaptation
 * @see Neuron - For neuron-level synapse management
 * @see STDP - For spike-timing-dependent plasticity
 * @see Hebbian - For Hebbian learning
 * 
 * @author NLM Development Team
 */
class Synapse {
public:
    /**
     * @brief Constructor with connection parameters
     * 
     * Creates a new synapse with specified source and destination neurons.
     * The synapse is initialized with default parameters and ready for
     * synaptic transmission and plasticity.
     * 
     * @param id Unique identifier for the synapse
     * @param source Neuron ID that provides input to this synapse
     * @param destination Neuron ID that receives input from this synapse
     * 
     * @pre source and destination must be valid neuron IDs
     * @pre source != destination (no self-connections)
     * @post Synapse created with specified connection
     * @post State initialized to default values
     * @post Ready for synaptic transmission
     * 
     * @note Self-connections are not allowed
     * @see getSourceNeuron() - For source neuron
     * @see getDestinationNeuron() - For destination neuron
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor
     * 
     * Cleans up synapse resources and performs any necessary
     * cleanup operations. The synapse is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Synapse cannot be used after destruction
     */
    ~Synapse();
    
    // Disable copying - PImpl idiom
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get synapse identifier
     * 
     * Returns the unique identifier assigned to this synapse.
     * This ID is used for addressing synapses in the neural network
     * and is essential for synaptic management and debugging.
     * 
     * @return SynapseId Unique identifier
     * 
     * @post Return value is constant for synapse lifetime
     * @complexity O(1)
     * 
     * @see Synapse(SynapseId, NeuronId, NeuronId) - Constructor with ID
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source neuron
     * 
     * Returns the neuron that provides input to this synapse.
     * This neuron is the origin of synaptic transmission.
     * 
     * @return NeuronId Source neuron ID
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see getDestinationNeuron() - For destination neuron
     * @see Neuron - For source neuron class
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination neuron
     * 
     * Returns the neuron that receives input from this synapse.
     * This neuron is the target of synaptic transmission.
     * 
     * @return NeuronId Destination neuron ID
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see getSourceNeuron() - For source neuron
     * @see Neuron - For destination neuron class
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get synaptic weight
     * 
     * Returns the current synaptic weight, which determines the
     * strength of the connection between source and destination neurons.
     * Synaptic weight is the primary parameter controlling synaptic
     * transmission efficacy.
     * 
     * @return SynapticWeight Current synaptic weight
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setWeight() - For setting synaptic weight
     * @see addToWeight() - For modifying synaptic weight
     * @see getEfficacy() - For use-dependent efficacy
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight
     * 
     * Changes the synaptic weight, which directly affects the strength
     * of the connection. This is the primary method for synaptic
     * plasticity and learning, allowing fine control over synaptic
     * strength.
     * 
     * @param weight New synaptic weight
     * 
     * @post Synaptic weight updated
     * @post Transmission strength changed
     * @post Learning state affected
     * 
     * @note Weight changes are permanent unless reset
     * @see getWeight() - For getting current weight
     * @see addToWeight() - For incremental weight changes
     * @see step() - For automatic weight changes
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to synaptic weight
     * 
     * Incrementally adds to the current synaptic weight. This is useful
     * for implementing learning rules that require gradual weight
     * adjustments, such as Hebbian learning and STDP.
     * 
     * @param delta Change in synaptic weight
     * 
     * @pre delta should be reasonable for learning rule
     * @post Synaptic weight increased by delta
     * @post Learning progress achieved
     * 
     * @note Used by learning algorithms for fine control
     * @see setWeight() - For direct weight setting
     * @see step() - For automatic weight updates
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get synaptic delay
     * 
     * Returns the transmission delay in simulation steps. This determines
     * how long it takes for a spike to travel from the source neuron
     * to the destination neuron. Delays are essential for:
     * - Temporal precision in neural computation
     * - Realistic synaptic dynamics
     * - Implementing timing-dependent processes
     * - Creating temporal filters
     * 
     * @return Delay Transmission delay in simulation steps
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setDelay() - For setting transmission delay
     * @see step() - For effect on timing
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic delay
     * 
     * Changes the transmission delay in simulation steps. This affects
     * the timing of synaptic transmission and is used for:
     * - Implementing different connection types
     * - Creating temporal dynamics
     * - Adjusting neural computation timing
     * - Experimental parameter studies
     * 
     * @param delay New transmission delay (steps)
     * 
     * @post Transmission delay updated
     * @post Neural timing affected
     * @post Temporal dynamics changed
     * 
     * @note Delay affects spike timing and synchronization
     * @see getDelay() - For getting current delay
     * @see step() - For effect on simulation timing
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get synapse type
     * 
     * Returns the synapse type, which determines its computational
     * properties and transmission characteristics. Different synapse
     * types have different biophysical parameters and computational roles.
     
     * @return SynapseType Synapse type
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setType() - For setting synapse type
     * @see SynapseType - Available synapse types
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * 
     * Changes the synapse type, which affects its transmission properties
     * and computational characteristics. This is useful for:
     * - Creating specialized synapse populations
     * - Adjusting synapse properties for different roles
     * - Dynamic synapse type changes
     * - Experimental parameter studies
     * 
     * @param type New synapse type
     * 
     * @post Synapse type updated
     * @post Transmission properties changed
     * @post Dynamics updated
     * 
     * @note Type affects transmission and learning
     * @see getType() - For getting synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * Determines whether this synapse is excitatory. Excitatory synapses
     * depolarize the postsynaptic neuron, increasing the likelihood of
     * spike generation. This is fundamental for neural computation.
     * 
     * @return true if synapse is excitatory, false otherwise
     * 
     * @post Return value reflects synapse properties
     * @complexity O(1)
     * 
     * @see isInhibitory() - For inhibitory check
     * @see getType() - For synapse type
     * @see step() - For transmission effect
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * Determines whether this synapse is inhibitory. Inhibitory synapses
     * hyperpolarize the postsynaptic neuron, decreasing the likelihood of
     * spike generation. This is essential for:
     * - Stabilizing neural networks
     * - Preventing runaway excitation
     * - Implementing neural inhibition
     * - Creating balanced networks
     * 
     * @return true if synapse is inhibitory, false otherwise
     * 
     * @post Return value reflects synapse properties
     * @complexity O(1)
     * 
     * @see isExcitatory() - For excitatory check
     * @see getType() - For synapse type
     * @see step() - For transmission effect
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record pre-synaptic spike
     * 
     * Records the current timestamp when the source neuron generates a
     * spike. This is essential for spike-timing-dependent plasticity (STDP)
     * and other spike-timing dependent computations. Each pre-synaptic
     * spike is stored for future plasticity computations.
     * 
     * @param timestamp Time when source neuron spiked (ms)
     * 
     * @post Pre-synaptic spike recorded
     * @post STDP computations enabled
     * 
     * @note Critical for STDP and learning
     * @see recordPostSpike() - For post-synaptic spike
     * @see getPreSpikeHistory() - For accessing spike history
     * @see step() - For plasticity updates
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record post-synaptic spike
     * 
     * Records the current timestamp when the destination neuron generates
     * a spike. This, combined with pre-synaptic spike timing, enables
     * spike-timing-dependent plasticity (STDP). The relative timing of
     * pre- and post-synaptic spikes determines synaptic weight changes.
     * 
     * @param timestamp Time when destination neuron spiked (ms)
     * 
     * @post Post-synaptic spike recorded
     * @post STDP computations enabled
     * 
     * @note Critical for STDP learning
     * @see recordPreSpike() - For pre-synaptic spike
     * @see getPostSpikeHistory() - For accessing spike history
     * @see step() - For plasticity updates
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get pre-synaptic spike history
     * 
     * Returns a const reference to the pre-synaptic spike history vector,
     * which contains timestamps of all spikes generated by the source neuron.
     * This is used for spike-timing-dependent plasticity (STDP) and other
     * spike-timing dependent computations.
     * 
     * @return Const reference to pre-synaptic spike history
     * 
     * @post Return value updates with each pre-synaptic spike
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Essential for STDP and learning
     * @see recordPreSpike() - For recording spikes
     * @see getPostSpikeHistory() - For post-synaptic history
     * @see clearHistory() - For clearing history
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get post-synaptic spike history
     * 
     * Returns a const reference to the post-synaptic spike history vector,
     * which contains timestamps of all spikes generated by the destination
     * neuron. This, combined with pre-synaptic history, enables STDP.
     * 
     * @return Const reference to post-synaptic spike history
     * 
     * @post Return value updates with each post-synaptic spike
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Essential for STDP learning
     * @see recordPostSpike() - For recording spikes
     * @see getPreSpikeHistory() - For pre-synaptic history
     * @see clearHistory() - For clearing history
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Clears both pre- and post-synaptic spike history vectors, removing
     * all recorded spike timestamps. This is used to:
     * - Reset STDP computations
     * - Reset learning processes
     * - Prepare for new learning epochs
     * - Debugging and testing
     * 
     * @post Both spike histories cleared
     * @post STDP computations reset
     * 
     * @see recordPreSpike() - For recording spikes
     * @see recordPostSpike() - For recording spikes
     * @see getPreSpikeHistory() - For accessing history
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity flags
     * 
     * Returns a const reference to the plasticity flags for this synapse,
     * which determine which plasticity rules are enabled for this synapse.
     * This is essential for controlling synaptic learning processes.
     * 
     * @return Const reference to plasticity flags
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see PlasticityFlags - Flags structure
     * @see enablePlasticity() - For setting plasticity
     * @see getPlasticityFlags() - Non-const version
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (non-const version)
     * 
     * Returns a non-const reference to the plasticity flags for this synapse,
     * allowing direct modification of plasticity settings. This is useful
     * for experimental control and debugging.
     * 
     * @return Reference to plasticity flags
     * 
     * @post Return value valid as long as synapse exists
     * @complexity O(1)
     * 
     * @see getPlasticityFlags() const - Const version
     * @see PlasticityFlags - Flags structure
     * @see enablePlasticity() - For setting plasticity
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity rules
     * 
     * Enables or disables specific plasticity rules for this synapse.
     * This controls which learning mechanisms are active for the
     * synapse, allowing fine-grained control over synaptic learning.
     * 
     * @param hebbian Enable Hebbian learning (co-incidence detection)
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @post Plasticity flags updated
     * @post Learning rules enabled/disabled
     * 
     * @note Multiple plasticity rules can be enabled simultaneously
     * @see getPlasticityFlags() - For accessing flags
     * @see STDP - For spike-timing-dependent plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace
     * 
     * Returns the current eligibility trace value, which is used for
     * reward-modulated learning. The eligibility trace accumulates
     * pre-synaptic activity and is updated by post-synaptic events,
     * enabling credit assignment for delayed rewards.
     * 
     * @return Current eligibility trace value
     * 
     * @post Return value fluctuates during simulation
     * @complexity O(1)
     * 
     * @see setEligibilityTrace() - For setting trace
     * @see decayEligibilityTrace() - For trace decay
     * @see step() - For trace updates
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace
     * 
     * Sets the eligibility trace to a specific value. The eligibility trace
     * is used in reward-modulated learning to assign credit to neural
     * activity that occurred before a reward.
     * 
     * @param trace New eligibility trace value
     * 
     * @post Eligibility trace updated
     * @post Reward-modulated learning enabled
     * 
     * @note Typically initialized to 0.0
     * @see getEligibilityTrace() - For getting trace
     * @see decayEligibilityTrace() - For trace decay
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace
     * 
     * Decays the eligibility trace by a factor determined by the decay rate.
     * This implements the biological process of trace decay, which ensures
     * that eligibility is time-limited and only applies to recent activity.
     * 
     * @param decayRate Decay factor (0.0 to 1.0)
     * 
     * @pre 0.0 <= decayRate <= 1.0
     * @post Eligibility trace decayed
     * @post Trace value reduced
     * 
     * @note Implements biological trace decay
     * @see getEligibilityTrace() - For getting trace
     * @see setEligibilityTrace() - For setting trace
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy
     * 
     * Returns the current synaptic efficacy, which is a use-dependent
     * modulation of synaptic strength. Efficacy reflects recent use
     * and can implement short-term plasticity, depression, or facilitation.
     * 
     * @return Current synaptic efficacy
     * 
     * @post Return value fluctuates with use
     * @complexity O(1)
     * 
     * @see setEfficacy() - For setting efficacy
     * @see step() - For efficacy updates
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * 
     * Sets the synaptic efficacy directly. Synaptic efficacy is a
     * use-dependent modulation that affects synaptic transmission without
     * changing the underlying weight. This implements short-term plasticity.
     * 
     * @param efficacy New synaptic efficacy
     * 
     * @post Synaptic efficacy updated
     * @post Short-term plasticity applied
     * @post Transmission affected
     * 
     * @note Different from synaptic weight
     * @see getEfficacy() - For getting efficacy
     * @see getWeight() - For synaptic weight
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     * 
     * Implements one timestep of synaptic dynamics, including:
     * - Eligibility trace decay
     * - Synaptic efficacy updates
     * - Spike history management
     * - Plasticity rule application
     * - Learning computations
     * 
     * This is the core computation function for the synapse that:
     * 1. Updates eligibility traces based on pre-synaptic activity
     * 2. Applies post-synaptic spike influences
     * 3. Updates synaptic efficacy based on recent use
     * 4. Applies plasticity rules (if enabled)
     * 5. Computes synaptic weight changes
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @pre currentTime must be non-negative
     * @post Synapse state updated for timestep
     * @post Learning processes completed
     * @post Transmission dynamics applied
     * 
     * @note Primary computation function for synapse dynamics
     * @note Called once per timestep by simulation
     * @see getEligibilityTrace() - For eligibility trace
     * @see getEfficacy() - For synaptic efficacy
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * Resets the synapse to its initial state, undoing all changes
     * accumulated during simulation. This includes:
     * - Spike history cleared
     * - Eligibility trace reset
     * - Synaptic efficacy reset
     * - Plasticity state reset
     * - Weight changes preserved (if applicable)
     * 
     * @post Synapse returned to initial state
     * @post Ready for new simulation
     * @post All dynamic variables reset
     * 
     * @note Weight changes typically preserved
     * @see initializeRandom() - For custom initialization
     * @see step() - For simulation stepping
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     * 
     * Initializes the synapse with random biophysical parameters,
     * typically used for generating biologically realistic neural
     * networks. This function randomizes various parameters to create
     * diversity across synapses while maintaining functional properties.
     * 
     * @param rng Random number generator for reproducible initialization
     * 
     * @pre rng must be valid and seeded
     * @post Synapse parameters randomized
     * @post State initialized with random values
     * 
     * @note Used for creating diverse synaptic connectivity
     * @see reset() - For state reset without parameter changes
     * @see RandomGenerator - For random number generation
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
