#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <stdexcept>

namespace nlm {

// Synapse operation errors
class SynapseError : public std::runtime_error {
public:
    SynapseError(const std::string& message) : std::runtime_error(message) {}
};

class SynapseInitializationError : public SynapseError {
public:
    SynapseInitializationError(const std::string& message) : SynapseError("Synapse initialization failed: " + message) {}
};

class SynapseConnectionError : public SynapseError {
public:
    SynapseConnectionError(const std::string& message) : SynapseError("Synapse connection failed: " + message) {}
};

class SynapseWeightError : public SynapseError {
public:
    SynapseWeightError(const std::string& message) : SynapseError("Synapse weight operation failed: " + message) {}
};

class SynapsePlasticityError : public SynapseError {
public:
    SynapsePlasticityError(const std::string& message) : SynapseError("Synapse plasticity operation failed: " + message) {}
};

// Synapse state validation
class SynapseStateError : public std::runtime_error {
public:
    SynapseStateError(const std::string& message) : std::runtime_error("Synapse state error: " + message) {}
};

// Synapse connection error for invalid neuron pairs
class InvalidConnectionError : public SynapseError {
public:
    InvalidConnectionError(NeuronId pre, NeuronId post) 
        : SynapseError("Invalid connection: pre=" + std::to_string(pre) + ", post=" + std::to_string(post)) {}
};

// Plasticity rule errors
class PlasticityRuleError : public std::runtime_error {
public:
    PlasticityRuleError(const std::string& message) : std::runtime_error("Plasticity rule error: " + message) {}
};

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
