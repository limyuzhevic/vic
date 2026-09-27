#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @file Synapse.hpp
 * @brief Header file for the Synapse class implementing synaptic transmission
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the Synapse class definition, which represents a
 * connection between neurons in the neural system. Synapses implement
 * real synaptic transmission with transmission delays and short-term
 * plasticity mechanisms for dynamic learning and adaptation.
 */

/**
 * @class Synapse
 * @brief Represents a synaptic connection between neurons
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The Synapse class implements the biophysical properties of synaptic
 * transmission between neurons. It manages the flow of neural signals
 * from source to destination neurons, including synaptic weight,
 * transmission delay, and various plasticity mechanisms.
 *
 * Key features:
 * - Bi-directional spike transmission with configurable delays
 * - Synaptic weight modulation for learning and memory
 * - Short-term plasticity mechanisms
 * - Spike-timing-dependent plasticity (STDP) support
 * - Reward-modulated learning capabilities
 * - Eligibility traces for delayed reinforcement
 *
 * @note This class uses the Pimpl idiom for implementation hiding,
 * providing efficient compilation times and encapsulation. All synaptic
 * state is stored in private implementation details.
 *
 * @warning Synaptic connections must be properly established before
 * use. Invalid source/destination neuron IDs can cause undefined behavior.
 *
 * @see Brain, Neuron, STDP, Hebbian
 */
class Synapse {
public:
    // Create synapse with IDs
    /**
     * @brief Constructor for the Synapse class
     * @param id Unique identifier for this synapse
     * @param source Neuron identifier that is the source of this connection
     * @param destination Neuron identifier that is the target of this connection
     * @post Synapse is created with initial parameters and default state
     * @note Synapse ID should be unique within the neural system
     * @warning Invalid neuron IDs may cause runtime errors during operation
     * @see getId(), getSourceNeuron(), getDestinationNeuron()
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor (deleted)
     * @warning Synapses cannot be copied due to pointer-based internal representation
     * @note Memory management handled through pImpl pattern
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Synapse object to move from
     * @post This synapse owns other's resources; other is in valid but unspecified state
     * @note Enables efficient transfer of synapse ownership
     * @see operator=(Synapse&&)
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Synapse object to move from
     * @return Reference to this synapse
     * @post Current synapse's resources are transferred from other
     * @note Enables efficient transfer of synapse ownership
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this synapse
     * @return SynapseId Unique identifier
     * @post Identifier remains constant throughout synapse's lifetime
     * @note Used for indexing and referencing in synaptic network operations
     */
    SynapseId getId() const;
    
    // Connection
    /**
     * @brief Get the source neuron identifier
     * @return NeuronId Identifier of the neuron that sends signals
     * @post Returns the presynaptic neuron
     * @note Critical for directional signal flow in neural network
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron identifier
     * @return NeuronId Identifier of the neuron that receives signals
     * @post Returns the postsynaptic neuron
     * @note Determines the direction of synaptic transmission
     */
    NeuronId getDestinationNeuron() const;
    
    // Weight
    /**
     * @brief Get current synaptic weight
     * @return SynapticWeight Current synaptic strength (typically 0.0 to 1.0)
     * @post Weight determines the efficacy of synaptic transmission
     * @note Values > 1.0 represent potentiation, values < 1.0 represent depression
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight
     * @param weight New synaptic strength (typically 0.0 to 1.0)
     * @post Synaptic weight is updated
     * @warning Weight changes affect neural network dynamics
     * @see addToWeight(), getWeight()
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Modify synaptic weight by delta
     * @param delta Change in synaptic weight
     * @post Synaptic weight is updated by delta amount
     * @note More precise than setting absolute weight values
     * @see setWeight(), getWeight()
     */
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    /**
     * @brief Get synaptic transmission delay
     * @return Delay Transmission delay in simulation steps
     * @post Delay determines temporal offset of signal transmission
     * @note Critical for implementing realistic spike timing
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic transmission delay
     * @param delay New transmission delay in simulation steps
     * @post Delay is updated
     * @warning Delay changes affect network temporal dynamics
     * @see getDelay()
     */
    void setDelay(Delay delay);
    
    // Type
    /**
     * @brief Get synapse type
     * @return SynapseType Type classification (excitatory, inhibitory, etc.)
     * @post Type determines sign and effect of transmitted signals
     * @note Different synapse types produce different post-synaptic effects
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     * @post Synapse type is updated
     * @note Changes how transmitted signals affect postsynaptic neuron
     * @warning Type changes may affect neural computation
     */
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    /**
     * @brief Check if synapse is excitatory
     * @return bool true if synapse releases excitatory neurotransmitters
     * @post Indicates positive effect on postsynaptic neuron
     * @note Typically glutamatergic synapses
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return bool true if synapse releases inhibitory neurotransmitters
     * @post Indicates negative effect on postsynaptic neuron
     * @note Typically GABAergic synapses
     */
    bool isInhibitory() const;
    
    // Activity history for plasticity
    /**
     * @brief Record presynaptic spike event
     * @param timestamp Time when presynaptic neuron spiked
     * @post Presynaptic spike timestamp is added to history
     * @note Used for spike-timing-dependent plasticity (STDP)
     * @see recordPostSpike(), getPreSpikeHistory()
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike event
     * @param timestamp Time when postsynaptic neuron spiked
     * @post Postsynaptic spike timestamp is added to history
     * @note Used for STDP and other spike-timing dependent mechanisms
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * @return const std::vector<Timestamp>& Vector of presynaptic spike times
     * @post Returns all recorded presynaptic spike timestamps
     * @note Essential for STDP and temporal learning rules
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike times
     * @post Returns all recorded postsynaptic spike timestamps
     * @note Used for STDP and synaptic eligibility computation
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     * @post All spike timestamps are removed from history
     * @note Resets plasticity eligibility for this synapse
     */
    void clearHistory();
    
    // Plasticity flags
    /**
     * @brief Get plasticity flags
     * @return const PlasticityFlags& Reference to plasticity rule flags
     * @post Returns which plasticity rules are enabled for this synapse
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return PlasticityFlags& Mutable reference to plasticity rule flags
     * @post Allows modification of enabled plasticity rules
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules
     * @param hebbian Enable Hebbian plasticity (fire together, wire together)
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @post Plasticity flags are updated
     * @note Controls which learning mechanisms apply to this synapse
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    /**
     * @brief Get eligibility trace value
     * @return float Current eligibility trace (typically 0.0 to 1.0)
     * @post Represents synaptic readiness to be modified by reward
     * @note Used in reward-modulated learning algorithms
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace value
     * @param trace New eligibility trace value
     * @post Eligibility trace is set
     * @warning Direct manipulation may bypass proper learning dynamics
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace over time
     * @param decayRate Rate at which trace decays (typically 0.0 to 1.0)
     * @post Eligibility trace is reduced by decay factor
     * @note Models temporal forgetting of plasticity eligibility
     */
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    /**
     * @brief Get synaptic efficacy
     * @return float Current synaptic efficacy (typically 0.0 to 1.0)
     * @post Represents activity-dependent synaptic strength modulation
     * @note Often reflects short-term plasticity effects
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New synaptic efficacy value
     * @post Synaptic efficacy is updated
     * @warning Efficacy changes affect synaptic transmission
     */
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    /**
     * @brief Update synapse state for one simulation step
     * @param currentTime Current simulation time
     * @post Synapse state is updated according to synaptic dynamics
     * @note Phase 2: Implement real synaptic dynamics (STP, learning, etc.)
     * @todo Implement spike transmission and short-term plasticity
     * @bug Current implementation is a placeholder for Phase 2
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset synapse to initial state
     * @post All synaptic state variables are reset to initial values
     * @note Clears spike history, eligibility traces, and temporary states
     * @warning Loses learned synaptic properties and eligibility
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize synapse with random parameters
     * @param rng Reference to random number generator
     * @post Synapse parameters (weight, delay, type) are randomized
     * @note Used for generating diverse synaptic connectivity patterns
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm