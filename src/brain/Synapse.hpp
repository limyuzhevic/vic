#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
//
// The Synapse class models biological synaptic connections between neurons,
// implementing spike-timing-dependent plasticity (STDP), Hebbian learning,
// and reward-modulated learning mechanisms. Each synapse tracks:
// - Transmission delays (time between pre- and post-synaptic spikes)
// - Synaptic weight (excitatory/inhibitory strength)
// - Spike history for pre- and post-synaptic neurons
// - Plasticity flags (which learning rules apply)
// - Eligibility traces for reward-modulated learning
// - Synaptic efficacy (short-term plasticity and use-dependent modulation)
//
// Synaptic Transmission:
// When a pre-synaptic neuron spikes, a spike event is generated and scheduled
// for delivery to the post-synaptic neuron after the synaptic delay.
// This models the biological process of synaptic transmission where the
// postsynaptic neuron receives input after a time lag.
//
// Plasticity Mechanisms:
// - STDP (Spike-Timing-Dependent Plasticity): Strengthens synapses when
//   pre-synaptic spike precedes post-synaptic spike (LTP), weakens when
//   post-synaptic spike precedes pre-synaptic spike (LTD)
// - Hebbian Learning: Strengthens synapses when both neurons are co-active
// - Reward-Modulated Learning: Modulates plasticity based on reward prediction
//   errors, implementing reinforcement learning at the synaptic level
//
// Usage example:
// @code
// class Brain {
// public:
//     void addSynapseToBrain(NeuronId source, NeuronId destination) {
//         SynapseId synId = brain.addSynapse(source, destination);
//         auto* synapse = brain.getSynapse(synId);
//         if (synapse) {
//             // Configure synaptic properties
//             synapse->setWeight(0.5f);  // Excitatory connection
//             synapse->setDelay(1);    // 1 timestep delay
//             // Enable STDP learning
//             synapse->enablePlasticity(false, true, false);
//         }
//     }
// };
// @endcode
class Synapse {
public:
    /**
     * Create a new synaptic connection
     * 
     * Establishes a directed connection from source to destination neuron.
     * The synapse is initialized with default parameters:
     * - Weight: 0.0 (neutral)
     * - Delay: 1 timestep
     * - Type: Excitatory (default)
     * - Plasticity: All rules disabled by default
     * 
     * @param id Unique identifier for this synapse
     * @param source Neuron ID that will send spikes to this synapse
     * @param destination Neuron ID that will receive spikes from this synapse
     * 
     * @note Synapse IDs must be unique across all synapses in the brain
     * @see reset()  // For resetting to initial state
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);

    /**
     * Destructor - cleans up synapse resources
     */
    ~Synapse();

    // Disable copying, enable moving (for performance and memory management)
    Synapse(const Synapse&) = delete;                     ///< Copy constructor disabled (non-copyable)
    Synapse& operator=(const Synapse&) = delete;          ///< Copy assignment disabled (non-copyable)
    Synapse(Synapse&&) noexcept;                          ///< Move constructor
    Synapse& operator=(Synapse&&) noexcept;               ///< Move assignment

    // Identity and classification

    /**
     * Get synapse's unique identifier
     * 
     * @return SynapseId Unique ID assigned at creation
     */
    SynapseId getId() const;

    /**
     * Get source neuron ID
     * 
     * @return NeuronId ID of the presynaptic neuron that sends spikes
     */
    NeuronId getSourceNeuron() const;

    /**
     * Get destination neuron ID
     * 
     * @return NeuronId ID of the postsynaptic neuron that receives spikes
     */
    NeuronId getDestinationNeuron() const;

    // Synaptic weight and strength

    /**
     * Get current synaptic weight
     * 
     * @return SynapticWeight Current weight (-1.0 to 1.0)
     * @see setWeight()
     * @see addToWeight()
     */
    SynapticWeight getWeight() const;

    /**
     * Set synaptic weight
     * 
     * Positive weights represent excitatory connections, negative weights represent
     * inhibitory connections. Weight magnitude controls connection strength.
     * 
     * @param weight New synaptic weight (-1.0 to 1.0)
     * @see getWeight()
     * @see addToWeight()
     */
    void setWeight(SynapticWeight weight);

    /**
     * Modify synaptic weight by delta
     * 
     * Useful for incremental weight updates (e.g., during learning).
     * New weight = current weight + delta, clamped to valid range.
     * 
     * @param delta Weight change amount
     * @see setWeight()
     */
    void addToWeight(SynapticWeight delta);

    // Transmission delay

    /**
     * Get synaptic transmission delay
     * 
     * @return Delay Number of simulation steps for spike transmission
     * @see setDelay()
     */
    Delay getDelay() const;

    /**
     * Set synaptic transmission delay
     * 
     * Determines how many timesteps elapse between pre- and post-synaptic spikes.
     * Larger delays create temporal dynamics and can affect learning.
     * 
     * @param delay New delay in timesteps (must be >= 0)
     * @see getDelay()
     */
    void setDelay(Delay delay);

    // Connection type

    /**
     * Get synaptic type
     * 
     * @return SynapseType Connection type (Excitatory, Inhibitory, Modulatory)
     */
    SynapseType getType() const;

    /**
     * Set synaptic type
     * 
     * Determines how the synapse affects the postsynaptic neuron:
     * - Excitatory: Positive weight increases membrane potential
     * - Inhibitory: Negative weight decreases membrane potential
     * - Modulatory: May have complex effects depending on neuronal state
     * 
     * @param type New synaptic type
     */
    void setType(SynapseType type);

    /**
     * Check if synapse is excitatory
     * 
     * @return true if weight > 0, false otherwise
     */
    bool isExcitatory() const;

    /**
     * Check if synapse is inhibitory
     * 
     * @return true if weight < 0, false otherwise
     */
    bool isInhibitory() const;

    // Activity history and spike timing

    /**
     * Record pre-synaptic spike time
     * 
     * Stores the timestamp of when the source neuron spiked for plasticity calculations.
     * Used by STDP and other spike-timing dependent learning rules.
     * 
     * @param timestamp Simulation time when pre-synaptic neuron spiked
     * @see recordPostSpike()
     * @see getPreSpikeHistory()
     */
    void recordPreSpike(Timestamp timestamp);

    /**
     * Record post-synaptic spike time
     * 
     * Stores the timestamp of when the destination neuron spiked for plasticity calculations.
     * Used by STDP to determine spike timing order for weight updates.
     * 
     * @param timestamp Simulation time when post-synaptic neuron spiked
     * @see recordPreSpike()
     * @see getPostSpikeHistory()
     */
    void recordPostSpike(Timestamp timestamp);

    /**
     * Get pre-synaptic spike history
     * 
     * Retrieves vector of timestamps when the source neuron last spiked.
     * Used by STDP to calculate spike timing differences.
     * 
     * @return const reference to pre-synaptic spike history
     * @see recordPreSpike()
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;

    /**
     * Get post-synaptic spike history
     * 
     * Retrieves vector of timestamps when the destination neuron last spiked.
     * Used by STDP and other plasticity rules that consider postsynaptic activity.
     * 
     * @return const reference to post-synaptic spike history
     * @see recordPostSpike()
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;

    /**
     * Clear spike history
     * 
     * Removes all recorded spike timestamps. Useful for:
     * - Testing and debugging plasticity mechanisms
     * - Resetting synapse state for controlled experiments
     * - Removing plasticity effects from historical spikes
     * 
     * @see recordPreSpike()
     * @see recordPostSpike()
     */
    void clearHistory();

    // Plasticity control

    /**
     * Get plasticity flags
     * 
     * Indicates which plasticity rules apply to this synapse.
     * Determines how synaptic weight changes based on neural activity and rewards.
     * 
     * @return const reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;

    /**
     * Get mutable plasticity flags
     * 
     * @return Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();

    /**
     * Enable/disable plasticity rules
     * 
     * Controls which learning mechanisms affect this synapse:
     * - Hebbian: Spike-timing independent strengthening with co-activity
     * - STDP: Precise spike-timing dependent plasticity
     * - Reward-modulated: Plasticity modulated by reward prediction errors
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated learning
     * 
     * @note Plasticity rules can be enabled/disabled independently
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    // Learning and adaptation mechanisms

    /**
     * Get eligibility trace
     * 
     * Tracks recent synaptic activity for reward-modulated learning.
     * Represents the synaptic eligibility for plasticity at the time a reward
     * is received. Decay reflects the fading of plasticity eligibility over time.
     * 
     * @return float Current eligibility trace value (0.0 to 1.0)
     */
    float getEligibilityTrace() const;

    /**
     * Set eligibility trace
     * 
     * Manually set the eligibility trace value.
     * Typically updated automatically based on recent spike activity.
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);

    /**
     * Decay eligibility trace
     * 
     * Reduces eligibility trace value based on decay rate.
     * Models the biological process where plasticity eligibility fades
     * over time without reinforcement.
     * 
     * @param decayRate Decay factor per timestep (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);

    /**
     * Get synaptic efficacy
     * 
     * Represents short-term synaptic efficacy and use-dependent modulation.
     * Can model:
     * - Short-term potentiation/depression
     * - Neuromodulatory effects
     * - Activity-dependent changes
     * 
     * @return float Current synaptic efficacy (0.0 to 1.0)
     */
    float getEfficacy() const;

    /**
     * Set synaptic efficacy
     * 
     * Directly control synaptic efficacy. This can be used to simulate:
     * - Neuromodulatory states
     * - Learning stages
     * - Experimental conditions
     * 
     * @param efficacy New synaptic efficacy value
     */
    void setEfficacy(float efficacy);

    // Simulation integration

    /**
     * Update synapse for one simulation step
     * 
     * Performs synapse-specific computations:
     * 1. Decay eligibility trace
     * 2. Update spike history (remove old spikes)
     * 3. Apply learning rules (STDP, Hebbian, reward-modulated)
     * 4. Calculate synaptic weight changes
     * 5. Apply neuromodulatory effects
     * 
     * @param currentTime Current simulation time (seconds)
     * 
     * @note This is the main computational step called by Brain::step()
     */
    void step(Timestamp currentTime);

    // State management

    /**
     * Reset synapse to initial state
     * 
     * Clears all synaptic state including:
     * - Spike history
     * - Eligibility trace
     * - Synaptic efficacy
     * - Current weight (if reset enabled)
     * 
     * Useful for:
     * - Testing and debugging
     * - Starting new episodes
     * - Experimental control
     */
    void reset();

    /**
     * Initialize synapse with random parameters
     * 
     * Sets synaptic parameters from random distributions:
     * - Weight: random value in [-1.0, 1.0]
     * - Delay: random value in [1, 10] timesteps
     * - Type: randomly excitatory or inhibitory
     * - Plasticity flags: random combination
     * 
     * Used during brain initialization for random connectivity.
     * 
     * @param rng Random number generator for parameter sampling
     */
    void initializeRandom(class RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;  ///< Pointer to implementation details (PIMPL idiom)
};

} // namespace nlm
