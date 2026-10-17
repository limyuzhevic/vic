/**
 * @file Synapse.hpp
 * @brief Synapse class representing a neural connection between neurons
 * @namespace nlm
 * 
 * This file implements the synaptic connection model for the NLM neural simulation
 * framework. Synapses are the fundamental communication channels between neurons,
 * transmitting signals through electrical and chemical processes with various
 * dynamics, delays, and plasticity mechanisms.
 * 
 * The synapse implementation models biological synaptic transmission including
 * short-term plasticity, spike-timing-dependent plasticity (STDP), and eligibility
 * traces for reward-modulated learning. Each synapse maintains state for its
 * current efficacy, history of activity, and learning-related variables.
 * 
 * @note Synapse.hpp provides the synaptic infrastructure necessary for building
 *       complex neural networks with dynamic connectivity and learning capabilities.
 *       The class is designed for both efficiency and extensibility to support
 *       various synaptic models and plasticity rules.
 * 
 * @see Neuron.hpp - Neurons connected by synapses
 * @see PlasticityRule.hpp - Rules for modifying synaptic weights
 * @see Neuromodulator.hpp - Neuromodulatory signals affecting synaptic plasticity
 * @see Brain.hpp - Brain regions containing networks of interconnected neurons
 */

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Model of a synaptic connection between two neurons
 * @namespace nlm
 * 
 * This class implements a comprehensive synaptic model that captures the essential
 * aspects of biological synaptic transmission. Synapses transmit signals from
 * presynaptic to postsynaptic neurons with configurable delays, conductance changes,
 * and dynamic efficacy modulation. The implementation includes support for
 * short-term plasticity, eligibility traces, and multiple plasticity mechanisms.
 * 
 * The synapse model is designed for biological realism while maintaining
 * computational efficiency suitable for large-scale neural network simulation.
 * It supports various synapse types (excitatory, inhibitory, modulatory) and
 * maintains detailed activity history for learning algorithms.
 * 
 * @note Synapse uses the Pimpl idiom to hide implementation details and allow
 *       efficient copying and comparison. The actual implementation is contained
 *       in the private Impl structure, which maintains all synaptic state
 *       variables and computational parameters.
 * 
 * @see Neuron - Postsynaptic and presynaptic neurons
 * @see PlasticityFlags - Plasticity configuration for this synapse
 * @see TimestepDuration - Time units for synaptic dynamics
 */
class Synapse {
public:
    /**
     * @brief Constructor creating a synaptic connection with identification
     * 
     * Creates a new synaptic connection between source and destination neurons.
     * The synapse is initialized with default biological parameters including
     * type classification, default delay, and initial state variables.
     * 
     * @param id Unique identifier for this synapse
     * @param source Neuron from which signals originate (presynaptic)
     * @param destination Neuron that receives signals (postsynaptic)
     * 
     * @note Synapse IDs should be assigned by the neural network implementation
     *       to ensure uniqueness across all synapses. Each connection has a single
     *       unique identifier for addressing and reference purposes.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Virtual destructor for proper cleanup of Pimpl implementation
     */
    ~Synapse();
    
    /**
     * @brief Disable copy construction (synapses are not copyable)
     * 
     * Synapses cannot be copied due to the Pimpl idiom and internal state management.
     * Attempting to copy a synapse will result in a compile-time error.
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Disable copy assignment (synapses are not assignable)
     * 
     * Synapses cannot be assigned due to the Pimpl idiom and internal state management.
     * Attempting to assign to a synapse will result in a compile-time error.
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor (enables efficient synapse transfer)
     * 
     * Move constructor allows transferring synapse ownership without deep copying.
     * After moving, the source synapse is left in a valid but unspecified state.
     * 
     * @param other Synapse to move from (left in valid but unspecified state)
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator (enables efficient synapse transfer)
     * 
     * Move assignment allows transferring synapse ownership without deep copying.
     * After assignment, the source synapse is left in a valid but unspecified state.
     * 
     * @param other Synapse to move from (left in valid but unspecified state)
     * @return Reference to this synapse after assignment
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this synapse
     * 
     * @return SynapseId identifier for this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron identifier
     * 
     * @return NeuronId identifier of the presynaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron identifier
     * 
     * @return NeuronId identifier of the postsynaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * The synaptic weight represents the strength of the connection,
     * determining how effectively presynaptic spikes influence the postsynaptic neuron.
     * Positive weights indicate excitatory connections, negative weights indicate
     * inhibitory connections.
     * 
     * @return SynapticWeight current synaptic weight
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight directly
     * 
     * @param weight New synaptic weight (clamped to valid range)
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add a delta value to the current synaptic weight
     * 
     * This is the primary method for plasticity rules to modify synaptic strength.
     * The weight change is automatically clamped to biologically plausible ranges
     * to prevent numerical instability.
     * 
     * @param delta Weight change to apply (positive for potentiation, negative for depression)
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic transmission delay
     * 
     * The delay represents the time (in simulation steps) required for a signal
     * to travel from the presynaptic to postsynaptic neuron. This models the
     * finite speed of neural transmission and synaptic processes.
     * 
     * @return Delay synaptic delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic transmission delay
     * 
     * @param delay New synaptic delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type classification
     * 
     * @return SynapseType type of this synapse (Excitatory, Inhibitory, or Modulatory)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type classification
     * 
     * @param type New synapse type classification
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if this is an excitatory synapse
     * 
     * Excitatory synapses depolarize the postsynaptic neuron, increasing the
     * likelihood of firing.
     * 
     * @return bool true if synapse is excitatory, false otherwise
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if this is an inhibitory synapse
     * 
     * Inhibitory synapses hyperpolarize the postsynaptic neuron, decreasing the
     * likelihood of firing.
     * 
     * @return bool true if synapse is inhibitory, false otherwise
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record a presynaptic spike event
     * 
     * This records the timing of presynaptic spikes for use in spike-timing-dependent
     * plasticity (STDP) and other learning algorithms that depend on precise spike timing.
     * The spike time is added to a rolling history buffer.
     * 
     * @param timestamp Time of the presynaptic spike (simulation time)
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a postsynaptic spike event
     * 
     * This records the timing of postsynaptic spikes for use in STDP and other
     * spike-timing-dependent plasticity rules.
     * 
     * @param timestamp Time of the postsynaptic spike (simulation time)
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of presynaptic spike times
     * 
     * @return const reference to vector of presynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the history of postsynaptic spike times
     * 
     * @return const reference to vector of postsynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history for this synapse
     * 
     * This removes all recorded spike times, resetting the synapse to a state
     * with no activity history. This is typically called when the synapse is
     * reset or when spike history is no longer needed.
     */
    void clearHistory();
    
    /**
     * @brief Get the plasticity flags for this synapse
     * 
     * @return const reference to plasticity configuration flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return Reference to plasticity configuration flags (for direct modification)
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure which plasticity mechanisms are enabled for this synapse
     * 
     * This sets the plasticity flags that determine which plasticity rules
     * will be applied to this synaptic connection. Different mechanisms capture
     * different aspects of synaptic learning and adaptation.
     * 
     * @param hebbian Enable Hebbian learning (correlational plasticity)
     * @param stdp Enable spike-timing-dependent plasticity (STDP)
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace value
     * 
     * The eligibility trace is a learning signal that persists after reward signals
     * in reward-modulated plasticity. It accumulates synaptic activity and decays
     * over time, representing the synaptic readiness for weight changes.
     * 
     * @return float Current eligibility trace value
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace over time
     * 
     * This implements the exponential decay of the eligibility trace, which models
     * the time-limited nature of synaptic eligibility for reward-modulated learning.
     * The trace approaches zero over time, implementing a form of memory decay.
     * 
     * @param decayRate Decay rate per timestep (typically between 0.0 and 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the current synaptic efficacy
     * 
     * Synaptic efficacy represents the use-dependent modulation of synaptic strength,
     * where repeated activity can temporarily increase or decrease synaptic effectiveness.
     * This is a key component of short-term plasticity mechanisms.
     * 
     * @return float Current synaptic efficacy (typically between 0.0 and 2.0)
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * @param efficacy New synaptic efficacy value
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Perform one complete simulation step for this synapse
     * 
     * This implements the complete synaptic dynamics for one simulation timestep,
     * including short-term plasticity (facilitation and depression), eligibility
     * trace decay, and weight clamping. This method should be called once per
     * simulation step to update the synapse's internal state.
     * 
     * @note PHASE 2: Enhanced implementation with biological realism and
     *       detailed Tsodyks-Markram short-term plasticity model is planned
     *       for future development. This placeholder provides the basic stepping
     *       functionality with configurable dynamics and stability constraints.
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the synapse to its initial state
     * 
     * This clears all internal state variables and resets the synapse to its
     * default initialization state. After reset, the synapse will have default
     * biological parameters and no activity history.
     */
    void reset();
    
    /**
     * @brief Initialize synapse parameters with biologically plausible values
     * 
     * This sets up the synapse with biologically realistic parameters based on
     * its type (excitatory, inhibitory, or other). The initialization includes
     * appropriate weight ranges, short-term plasticity state, and other
     * computational parameters drawn from biological observations.
     * 
     * @param rng Reference to random number generator for stochastic initialization
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl implementation structure (pointer to implementation)
     * 
     * The Synapse class uses the Pimpl idiom to hide implementation details
     * and allow efficient copying and comparison. All actual implementation is
     * contained in this private structure, which maintains all synaptic state
     * variables and computational parameters.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation structure
     */
    Impl* pImpl;
};

} // namespace nlm
