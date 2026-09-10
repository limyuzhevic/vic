#pragma once

/**
 * @file Synapse.hpp
 * @brief Neural synapse implementation - synaptic transmission between neurons
 * @author NLM Synapse Team
 * @date 2026
 * @version 1.0
 * 
 * This file implements the Synapse class, which represents a connection between
 * two neurons in the neural network. Synapses are fundamental to neural communication,
 * implementing synaptic transmission with delays, short-term plasticity, and various
 * modulation mechanisms.
 * 
 * The Synapse class supports multiple synapse types (excitatory, inhibitory, modulatory),
 * transmission delays, plasticity rules (STDP, Hebbian, reward-modulated), and
 * short-term plasticity mechanisms. Each synapse maintains its own state including
 * spike history, eligibility traces, and synaptic efficacy.
 * 
 * Key features:
 * - Multi-type synapses: excitatory, inhibitory, modulatory, electrical, gap junction
 * - Configurable transmission delays for temporal dynamics
 * - Comprehensive plasticity support: STDP, Hebbian, reward-modulated learning
 * - Spike recording and eligibility trace mechanisms
 * - Synaptic efficacy modulation for use-dependent plasticity
 * - Efficient implementation using Pimpl idiom
 * 
 * @note This is a critical component of the neural computation pipeline, enabling
 * communication between neurons in both local and long-range connections.
 * 
 * @warning Synaptic connections are immutable once created (except for weight and
 * plasticity parameters). To create new connections, use the Brain::addInterRegionConnection()
 * method or other creation mechanisms.
 */

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
/** @brief Neuron class representing a single computational unit in the network */
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

/**
 * @class Synapse
 * @brief Neural synapse implementation - synaptic transmission between neurons
 * 
 * The Synapse class models a synaptic connection between a source neuron and a
 * destination neuron. It implements the fundamental mechanism of neural communication,
 * transmitting electrical signals with configurable delays and implementing various
 * plasticity and modulation mechanisms.
 * 
 * Synapses are the primary medium of information transfer in neural networks,
 * converting presynaptic spikes into postsynaptic responses through transduction
 * mechanisms. Each synapse maintains its own state including synaptic weight,
 * transmission delay, spike history, and plasticity parameters.
 * 
 * Key aspects:
 * - **Transmission**: Converts presynaptic spikes into postsynaptic potentials
 * - **Delay**: Configurable transmission delay in simulation steps
 * - **Type**: Multiple synapse types for different functional roles
 * - **Plasticity**: Learning rules that modify synaptic strength over time
 * - **Modulation**: Short-term plasticity and eligibility traces
 * 
 * The synapse implementation supports:
 * - Event-driven transmission with precise timing
 * - Multiple plasticity mechanisms: STDP, Hebbian, reward-modulated learning
 * - Short-term plasticity effects (facilitation, depression)
 * - Eligibility traces for reward-modulated learning
 * - Synaptic efficacy modulation for use-dependent plasticity
 * 
 * @note The Synapse class uses the Pimpl idiom for efficient implementation and
 * to reduce compilation dependencies. The actual implementation details are
 * hidden in a separate translation unit.
 * 
 * @warning Synchronous synapses (gap junctions and electrical synapses) bypass
 * the delay mechanisms and transmit immediately. These require special handling
 * in the simulation.
 * 
 * @see Neuron - the presynaptic and postsynaptic neurons
 * @see PlasticityRule - base class for synaptic plasticity mechanisms
 * @see STDP - spike-timing-dependent plasticity implementation
 * @see Brain - creates and manages synapses
 */

class Synapse {
public:
    /**
     * @brief Constructor - creates a synapse with unique identifiers
     * 
     * Creates a new synapse connecting two neurons with specified identifiers.
     * The synapse is initially configured with default parameters and ready for
     * simulation.
     * 
     * @param id Unique identifier for this synapse
     * @param source Source neuron ID - the presynaptic neuron
     * @param destination Destination neuron ID - the postsynaptic neuron
     * 
     * @throws NLMError if neuron IDs are invalid or IDs conflict with existing synapses
     * 
     * @note Synapse IDs must be unique across the entire brain simulation and
     * should be monotonically increasing to maintain consistency.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor - cleans up synapse resources
     */
    ~Synapse();
    
    /**
     * @brief Disable copying - synapses are not copyable
     * 
     * The Synapse class manages unique identifiers and internal state that
     * cannot be safely copied. Use move semantics instead.
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Disable assignment - synapses are not assignable
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor - transfer ownership of synapse resources
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfer ownership of synapse resources
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the synapse's unique identifier
     * 
     * @return SynapseId Unique identifier for this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron (presynaptic neuron)
     * 
     * @return NeuronId ID of the presynaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron (postsynaptic neuron)
     * 
     * @return NeuronId ID of the postsynaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current synaptic weight (positive for excitatory,
     * negative for inhibitory, zero for no connection)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic weight
     * @throws NLMError if weight is NaN or infinite
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to the current synaptic weight
     * 
     * @param delta Weight change to add
     * @throws NLMError if resulting weight would be NaN or infinite
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the transmission delay
     * 
     * @return Delay Transmission delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the transmission delay
     * 
     * @param delay Transmission delay in simulation steps
     * @throws NLMError if delay is negative or exceeds maximum allowed value
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType Type of synapse (excitatory, inhibitory, modulatory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New synapse type
     * @throws NLMError if type is invalid for the synapse state
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if this is an excitatory synapse
     * 
     * @return bool true if synapse type is excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if this is an inhibitory synapse
     * 
     * @return bool true if synapse type is inhibitory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record a presynaptic spike event
     * 
     * @param timestamp Time when the presynaptic neuron spiked
     * @throws NLMError if timestamp is invalid
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a postsynaptic spike event
     * 
     * @param timestamp Time when the postsynaptic neuron spiked
     * @throws NLMError if timestamp is invalid
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of presynaptic spikes
     * 
     * @return const std::vector<Timestamp>& Vector of presynaptic spike timestamps
     * @note Used by plasticity rules (e.g., STDP) to compute timing-dependent changes
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the history of postsynaptic spikes
     * 
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike timestamps
     * @note Used by plasticity rules to compute timing-dependent changes
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     * 
     * Resets both presynaptic and postsynaptic spike history to empty.
     * This is typically called when plasticity rules need to be reset or
     * when the synapse is being disabled.
     */
    void clearHistory();
    
    /**
     * @brief Get the current plasticity flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags indicating
     * which plasticity rules are enabled for this synapse
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get reference to plasticity flags (non-const version)
     * 
     * @return PlasticityFlags& Reference to plasticity flags for modification
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable or disable specific plasticity rules
     * 
     * @param hebbian Enable Hebbian plasticity rule
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @throws NLMError if invalid combination of rules is specified
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace value
     * 
     * @return float Current eligibility trace value (typically between 0 and 1)
     * @note Used in reward-modulated learning to gate weight changes based on
     * temporal proximity to reward signals
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace New eligibility trace value
     * @throws NLMError if trace is negative or exceeds 1.0
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace
     * 
     * @param decayRate Decay factor (typically between 0 and 1)
     * @throws NLMError if decayRate is not in [0, 1]
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the current synaptic efficacy
     * 
     * @return float Synaptic efficacy value (use-dependent modulation factor)
     * @note Efficacy can be modified by recent activity patterns and reflects
     * the current functional state of the synapse
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * @param efficacy New efficacy value
     * @throws NLMError if efficacy is negative
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse state for one simulation step
     * 
     * Performs the synaptic computation for one timestep, including:
     * - Decay of eligibility traces
     * - Short-term plasticity effects
     * - Transmission of spike effects
     * - Recording of timing information for plasticity
     * 
     * @param currentTime Current simulation time in seconds
     * @throws NLMError if currentTime is negative
     * 
     * @note This method is called by the simulation during the step() phase.
     * The TODO comment in the original code indicates that real synaptic dynamics
     * need to be implemented for this placeholder version.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * Clears all state including spike history, eligibility traces, and resets
     * to initial parameter values. This is useful for starting over with a new
     * simulation or resetting a synapse to its default configuration.
     */
    void reset();
    
    /**
     * @brief Initialize synapse parameters with random values
     * 
     * @param rng Reference to random number generator
     * @throws NLMError if rng is invalid
     * 
     * @note Random initialization is used when creating new synapses or
     * reinitializing synapses for a new simulation.
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Pointer to implementation (Pimpl idiom)
     * 
     * The actual implementation is hidden in a separate translation unit to
     * reduce compilation dependencies and allow implementation changes without
     * affecting the public interface.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the synapse implementation
     */
    Impl* pImpl;
};

} // namespace nlm