#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
// Synapses are the fundamental communication channels in the NLM artificial brain
// Each synapse transmits signals from source to destination neuron with configurable
// strength, delay, and plasticity rules based on biological neural principles

class Synapse {
public:
    // Create synapse with source and destination neuron IDs
    // Establish connection between two neurons with unique identifiers
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    // Prevent copying to avoid memory issues, allow moving for efficient transfers
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    // Get synapse unique identifier
    SynapseId getId() const;
    
    // Connection endpoints
    // Get source and destination neurons
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Synaptic weight (strength of connection)
    // Weight determines the strength of signal transmission
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Transmission delay (in simulation steps)
    // Delay models synaptic transmission time between neurons
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Synapse type (excitatory, inhibitory, modulatory)
    // Determines the nature of synaptic effect on postsynaptic neuron
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Functional classification
    // Check synaptic effect type
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for synaptic plasticity
    // Record spike times for spike-timing dependent plasticity (STDP)
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    // Enable/disable different plasticity rules
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace for reward-modulated learning
    // Track learning eligibility over time for reward-based plasticity
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    // Represents current synaptic strength based on usage patterns
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics with STDP and short-term plasticity
    // This will implement realistic synaptic behavior including depression/facilitation
    void step(Timestamp currentTime);
    
    // Reset to initial state
    // Reset synapse to its initial configuration
    void reset();
    
    // Initialize with random parameters
    // Set up random synaptic properties during network construction
    void initializeRandom(class RandomGenerator& rng);
    
    // Get synaptic strength metrics
    // Statistical information about synapse weights
    float getAverageWeight() const;
    float getMaxWeight() const;
    float getMinWeight() const;
    float getWeightRange() const;
    
    // Check if synapse is functional
    // Determine if synapse is healthy and operational
    bool isFunctional() const;
    
    // Export synapse state for checkpointing
    // Save synapse state for session persistence
    void exportState(std::ostream& out) const;
    
    // Import synapse state from checkpoint
    // Restore synapse state from session persistence
    void importState(std::istream& in);
    
    // Biological context methods
    // Convert delay from simulation steps to milliseconds
    void setSynapticDelay(float delayMs);
    float getSynapticDelay() const;
    
    // Computational complexity
    // Memory footprint for this synapse
    static constexpr size_t getMemoryFootprint() { return sizeof(Synapse); }
    // Computational cost per simulation step
    static constexpr size_t getComputationalCost() { return 50; }
    
    // Temporal dynamics
    // Update time-dependent synaptic properties
    void updateTemporalDynamics(float currentTime);
    // Get time since last spike activity
    float getLastActivityTime() const;
    // Check if synapse has been active recently
    bool hasRecentActivity(float timeThreshold) const;
    
    // Network integration
    // Register synapse with source and destination neurons
    void registerWithSourceNeuron(std::shared_ptr<Neuron> sourceNeuron);
    void registerWithDestinationNeuron(std::shared_ptr<Neuron> destinationNeuron);
    
    // Debugging and diagnostics
    // Get formatted debugging information
    std::string getDebugInfo() const;
    // Check if synapse is healthy
    bool isHealthy() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
