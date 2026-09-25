#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

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
    
    // ========== SYNAPTIC DYNAMICS IMPLEMENTATION ==========
    
    // Real synaptic transmission with short-term plasticity
    // Implements Tsodyks-Markram model for synaptic efficacy
    void updateSynapticDynamics(Timestamp currentTime);
    
    // Check if synapse is ready to transmit (considering delay)
    bool isReadyToTransmit(Timestamp currentTime) const;
    
    // Get transmission time for future spike events
    Timestamp getTransmissionTime(Timestamp spikeTime) const;
    
    // Short-term plasticity effects
    void updateShortTermPlasticity(Timestamp currentTime);
    
    // Depression/facilitation dynamics
    void updateDepression(Timestamp currentTime);
    void updateFacilitation(Timestamp currentTime);
    
    // Tsodyks-Markram model implementation
    void tsodyksMarkramUpdate(Timestamp currentTime);
    
    // Vesicle dynamics and release probability
    void updateVesicleDynamics(Timestamp currentTime);
    void updateReleaseProbability();
    
    // Neuromodulation effects
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Learning eligibility trace
    void updateEligibilityTrace(float reward, Timestamp currentTime);
    
    // State snapshot and restoration
    void snapshot(class SynapseSnapshot& snapshot) const;
    void restore(const class SynapseSnapshot& snapshot);
    
    // Resource management
    void clear();
    size_t memoryUsage() const;
    
    // Validation and debugging
    bool validate() const;
    void validateConnection() const;
    
private:
    // Use smart pointer for automatic memory management
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
