#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Represents a connection between two neurons with transmission dynamics.
 * @details Models synaptic transmission including:
 * - Chemical transmission with delays
 * - Short-term plasticity (facilitation/depression)
 * - Spike-timing dependent plasticity (STDP) eligibility
 * - Reward-modulated learning capabilities
 * - Synaptic weight dynamics and efficacy modulation
 * 
 * The synapse implements: V_post = weight * PSP_amplitude * delay_factor
 * where PSP_amplitude depends on presynaptic spike timing and history.
 * 
 * Key features:
 * - Transmission delay (in simulation steps)
 * - Spike history tracking for STDP
 * - Eligibility trace for reward-modulated learning
 * - Use-dependent efficacy modulation
 * - Synaptic type classification (excitatory/inhibitory)
 * 
 * @ingroup brain
 * @author NLM Development Team
 * @version 0.1.0
 */
class Synapse {
public:
    /**
     * @brief Constructor with connection details.
     * @param[in] id Unique identifier for this synapse.
     * @param[in] source NeuronId of the presynaptic (source) neuron.
     * @param[in] destination NeuronId of the postsynaptic (target) neuron.
     * @details Creates synapse with default parameters:
     * - Weight: 1.0 (normalized)
     * - Type: Excitatory
     * - Delay: 1 step
     * - Plasticity: Enabled for STDP and reward modulation
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
/**
     * @brief Virtual destructor.
     * @details Cleans up implementation and releases resources.
     */
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
/**
     * @brief Returns the synapse's unique identifier.
     * @return SynapseId The unique identifier.
     */
    SynapseId getId() const;

    /**
     * @brief Get the source neuron ID (presynaptic).
     * @return NeuronId The source neuron identifier.
     * @details This is the neuron that releases neurotransmitters.
     */
    NeuronId getSourceNeuron() const;

    /**
     * @brief Get the destination neuron ID (postsynaptic).
     * @return NeuronId The destination neuron identifier.
     * @details This is the neuron that receives neurotransmitters.
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the synaptic weight (connection strength).
     * @return SynapticWeight The weight value (typically 0.0 to 1.0).
     * @details Weight determines the efficacy of synaptic transmission:
     * - Positive weights: excitatory connections
 * - Negative weights: inhibitory connections
     * - Zero weight: no connection
     */
    SynapticWeight getWeight() const;

    /**
     * @brief Set the synaptic weight.
     * @param[in] weight New weight value.
     * @details Weight directly scales the postsynaptic potential.
     * Valid range: typically [-10.0, 10.0].
     */
    void setWeight(SynapticWeight weight);

    /**
     * @brief Add delta to the synaptic weight.
     * @param[in] delta Weight increment (for STDP or reward-modulated learning).
     * @details Used by plasticity rules to increment/decrement weights.
     * Positive delta strengthens, negative weakens.
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic transmission delay.
     * @return Delay Transmission delay in simulation steps.
     * @details Delay determines when the postsynaptic neuron receives the signal:
     * - Delay = 0: instantaneous transmission (unrealistic)
     * - Delay = 1: one-step transmission (typical)
     * - Delay > 1: longer transmission delays
     * @note Delay > 0 ensures causality in discrete-time simulation.
     */
    Delay getDelay() const;

    /**
     * @brief Set the synaptic transmission delay.
     * @param[in] delay New transmission delay in steps.
     * @details Must be >= 0 for physically meaningful simulation.
     * @warning Delay = 0 may cause infinite loops in some network configurations.
     */
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
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
