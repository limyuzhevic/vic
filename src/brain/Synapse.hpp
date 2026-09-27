#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

/**
 * @class Synapse
 * @brief Represents a synaptic connection between two neurons
 * 
 * The Synapse class models the biological process of synaptic transmission, where
 * electrical signals from a presynaptic neuron propagate to a postsynaptic neuron
 * through chemical or electrical mechanisms. This implementation supports:
 * 
 * Key Features:
 * - Realistic synaptic transmission with time delays
 * - Multiple synaptic types (excitatory, inhibitory, modulatory, electrical)
 * - Short-term plasticity mechanisms for dynamic weight modulation
 * - Spike-timing dependent plasticity (STDP) support
 * - Reward-modulated learning capabilities
 * - Eligibility traces for delayed reward signaling
 * 
 * Biological Plausibility:
 * - Synaptic delays reflect biological transmission times (milliseconds to seconds)
 * - Weight dynamics mirror synaptic strength changes observed in learning
 * - Spike history recording enables temporal learning rules
 * - Plasticity flags allow selective rule application per synapse
 * 
 * Performance Characteristics:
 * - Lightweight: O(1) for most operations
 * - Event-driven: Only updates on spike events
 * - Memory efficient: Uses opaque implementation pointer
 * 
 * @note Phase 2 TODO: Implement complete synaptic dynamics in step() method
 * @see Neuron
 * @see PlasticityFlags
 * @see STDP
 * @see Hebbian
 * @ingroup core
 */

class Synapse {
public:
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
