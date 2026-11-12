#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synapse representing a connection between neurons
 * @details Implements real synaptic transmission with delays and short-term plasticity
 * @warning This class uses the Pimpl pattern for implementation details
 */

class Synapse {
public:
    /**
     * @brief Create synapse with unique identifiers
     * @param id Unique identifier for this synapse
     * @param source Source neuron identifier (presynaptic)
     * @param destination Destination neuron identifier (postsynaptic)
     * 
     * @post Synapse created with initial state. Weight and delay use default
     *       values from configuration or initializeRandom().
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destroy synapse and cleanup resources
     * @post All connection references cleared and implementation memory freed
     */
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    // Phase 5 checkpoint support: restore histories recorded by
    // recordPre/PostSpike (capped at the same MAX_SPIKE_HISTORY).
    void setPreSpikeHistory(const std::vector<Timestamp>& history);
    void setPostSpikeHistory(const std::vector<Timestamp>& history);
    
    // Identity
    /**
     * @brief Get unique identifier of this synapse
     * @return SynapseId Unique identifier for this synapse
     * 
     * @post Id remains constant throughout synapse's lifetime.
     */
    SynapseId getId() const;
    
    // Connection
    /**
     * @brief Get source neuron identifier (presynaptic neuron)
     * @return NeuronId Identifier of neuron that sends signals through this synapse
     * 
     * @post Source neuron generates spikes that propagate through this synapse.
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination neuron identifier (postsynaptic neuron)
     * @return NeuronId Identifier of neuron that receives signals through this synapse
     * 
     * @post Destination neuron receives inputs from source neuron via this synapse.
     */
    NeuronId getDestinationNeuron() const;
    
    // Weight
    /**
     * @brief Get current synaptic weight
     * @return SynapticWeight Current synaptic weight (typically 0.0 to 1.0)
     * 
     * @post Weight determines signal strength transmitted through synapse.
     *       Higher weight = stronger signal transmission.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight directly
     * @param weight New synaptic weight (typically 0.0 to 1.0)
     * 
     * @post Used for initialization, testing, or direct weight manipulation.
     *       May trigger plasticity mechanisms depending on implementation.
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add delta to synaptic weight
     * @param delta Weight change amount
     * 
     * @post More efficient than set() for incremental changes. Used by
     *       plasticity rules (STDP, Hebbian) for weight updates.
     */
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    /**
     * @brief Get synaptic transmission delay
     * @return Delay Transmission delay in simulation steps
     * 
     * @post Realistic synapses have transmission delays. Affects spike timing
     *       and temporal interactions in the network.
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic transmission delay
     * @param delay New transmission delay (steps)
     * 
     * @post Affects spike propagation timing. Longer delays can create
     *       temporal separation between pre- and post-synaptic spikes.
     */
    void setDelay(Delay delay);
    
    // Type
    /**
     * @brief Get synapse type
     * @return SynapseType Type classification (Excitatory, Inhibitory, Modulatory)
     * 
     * @post Type determines sign of effect on postsynaptic neuron:
     *       Excitatory increases firing probability, Inhibitory decreases it.
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     * 
     * @post Changes effect of synaptic transmission. Some types may be
     *       determined by biological constraints.
     */
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    /**
     * @brief Check if synapse is excitatory
     * @return bool true if synapse increases postsynaptic firing probability
     * 
     * @post Excitatory synapses typically have positive weights that
     *       depolarize postsynaptic neuron.
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return bool true if synapse decreases postsynaptic firing probability
     * 
     * @post Inhibitory synapses typically have negative weights that
     *       hyperpolarize postsynaptic neuron.
     */
    bool isInhibitory() const;
    
    // Activity history for plasticity
    /**
     * @brief Record presynaptic spike occurrence
     * @param timestamp Time of spike (seconds)
     * 
     * @post Maintains spike timing history for spike-timing dependent plasticity
     *       (STDP). History window typically bounded by eligibility trace decay.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike occurrence
     * @param timestamp Time of spike (seconds)
     * 
     * @post Maintains spike timing history for STDP. Combined with pre-synaptic
     *       history to compute spike timing differences.
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * @return const std::vector<Timestamp>& Vector of presynaptic spike times
     * 
     * @post Used by STDP and other temporal learning rules to compute
     *       spike timing differences (Δt = t_post - t_pre).
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike times
     * 
     * @post Used by STDP and other temporal learning rules.
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history for both pre- and postsynaptic spikes
     * @post Used when resetting synapse state or starting new learning epochs.
     *       Also used by checkpoint/restore mechanisms.
     */
    void clearHistory();
    
    // Phase 5 checkpoint support: restore histories recorded by
    // recordPre/PostSpike (capped at the same MAX_SPIKE_HISTORY).
    /**
     * @brief Restore presynaptic spike history (for checkpoint/restore)
     * @param history Vector of presynaptic spike timestamps
     * 
     * @post Used during checkpoint save/restore to maintain learning state.
     *       History may be truncated to maximum allowed size.
     */
    void setPreSpikeHistory(const std::vector<Timestamp>& history);
    
    /**
     * @brief Restore postsynaptic spike history (for checkpoint/restore)
     * @param history Vector of postsynaptic spike timestamps
     * 
     * @post Used during checkpoint save/restore to maintain learning state.
     */
    void setPostSpikeHistory(const std::vector<Timestamp>& history);
    
    // Plasticity flags
    /**
     * @brief Get plasticity configuration flags
     * @return const PlasticityFlags& Reference to plasticity configuration
     * 
     * @post Determines whether each plasticity rule (Hebbian, STDP, reward-modulated)
     *       is enabled for this synapse.
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity configuration flags
     * @return PlasticityFlags& Mutable reference to plasticity configuration
     * 
     * @post Allows direct modification of plasticity settings for testing scenarios.
     */
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    /**
     * @brief Configure which plasticity rules are enabled for this synapse
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @post Hebbian and STDP are the primary local plasticity rules. Reward-modulated
     *       plasticity implements credit assignment for reinforcement learning.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    /**
     * @brief Get current eligibility trace value
     * @return float Current eligibility trace value
     * 
     * @post Used in reward-modulated plasticity. Represents synaptic eligibility
     *       for weight changes when reward is received.
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace value
     * @param trace New eligibility trace value
     * 
     * @post Typically used during learning phases. Reset to zero when weight
     *       changes occur or trace decays to zero.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace over time
     * @param decayRate Decay factor (typically 0.0 to 1.0)
     * 
     * @post Implements temporal decay of learning eligibility. Higher decayRate
     *       means faster decay. Usually applied each simulation step.
     */
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    /**
     * @brief Get current synaptic efficacy
     * @return float Synaptic efficacy value (typically 0.0 to 1.0)
     * 
     * @post Represents use-dependent modulation of synaptic strength.
     *       Can be influenced by short-term plasticity mechanisms.
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New efficacy value (typically 0.0 to 1.0)
     * 
     * @post Used for short-term plasticity effects like depression or facilitation.
     *       Models how recent activity affects current signal transmission.
     */
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    /**
     * @brief Execute one simulation step for synaptic dynamics
     * @param currentTime Current simulation timestamp (seconds)
     * 
     * @post Updates synaptic state including:
     *       - Decay of eligibility trace
     *       - Short-term plasticity effects (efficacy changes)
     *       - Transmission of spikes with delays
     *       - Weight changes from plasticity rules
     * 
     * @todo PHASE 2: Implement real synaptic dynamics
     * Currently serves as placeholder for advanced synaptic modeling.
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset synapse to initial state
     * 
     * @post Restores weight to initial value, clears spike history,
     *       resets eligibility trace and efficacy to default values.
     *       Used when restarting simulation from same initial conditions.
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize synapse with random parameters
     * @param rng Random number generator reference
     * 
     * @post Sets random values for:
     *       - Initial weight (typically 0.1 to 0.9)
     *       - Transmission delay (typically 1 to 10 steps)
     *       - Synapse type (Excitatory/Inhibitory based on probability)
     *       - Plasticity configuration based on random flags
     * 
     * @post Used for creating diverse initial network configurations
     *       and randomizing synapse properties for statistical studies.
     */
    void initializeRandom(class RandomGenerator& rng);
    
    private:
        struct Impl;
        Impl* pImpl;
    };
    
} // namespace nlm
