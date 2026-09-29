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
    
    // Advanced querying methods
    float getAverageFiringRate(float timeWindow) const;  // Average firing rate over time window
    bool isEligibleForLearning() const;  // Check if synapse is eligible for plasticity
    float getLearningPotential() const;  // Combined potential for various learning mechanisms
    bool hasRecentActivity(float timeThreshold) const;  // Check for recent pre or post spikes
    
    // Short-term plasticity state querying
    float getFacilitationFactor() const;  // Current facilitation state (0-1)
    float getDepressionFactor() const;   // Current depression state (0-1)
    float getSynapticStrength() const;   // Combined efficacy * weight for transmission
    
    // Eligibility trace utilities
    bool isEligibilityTraceActive() const;  // Check if trace exceeds threshold
    float getNormalizedTrace() const;      // Normalize trace by decay rate
    
    // Activity analysis
    size_t getRecentPreSpikeCount(float timeWindow) const;  // Count pre spikes in window
    size_t getRecentPostSpikeCount(float timeWindow) const; // Count post spikes in window
    float getLastPreSpikeInterval() const;  // Time since last pre spike
    float getLastPostSpikeInterval() const; // Time since last post spike
    
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
