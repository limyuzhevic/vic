#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Represents a synaptic connection between neurons
 * 
 * The Synapse class implements a real biological synapse with key properties:
 * - Synaptic weight (strength of connection)
 * - Transmission delay (temporal propagation)
 * - Connection type (excitatory or inhibitory)
 * - Spike timing history (for STDP plasticity)
 * - Eligibility traces (for reward-modulated learning)
 * - Synaptic efficacy (use-dependent modulation)
 * 
 * Synapses are the fundamental communication channels in the NLM brain,
 * transmitting signals from presynaptic to postsynaptic neurons with
 * realistic biophysical properties including delays and activity-dependent
 * plasticity mechanisms.
 * 
 * @note Synapses are organized into regions and can be subject to multiple
 *       plasticity rules simultaneously (STDP, Hebbian, reward-modulated).
 * 
 * @see Brain for integration with higher-level brain systems
 * @see Neuron for pre- and post-synaptic neurons
 */
class Synapse {
public:
    /**
     * @brief Create synapse with unique identifier and neuron connections
     * 
     * @param id Unique identifier for this synapse
     * @param source Neuron ID of presynaptic neuron
     * @param destination Neuron ID of postsynaptic neuron
     * 
     * @note The synapse does not automatically connect to actual neurons.
     *       Connection establishment happens during brain initialization
     *       through the brain's connectivity setup.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
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
