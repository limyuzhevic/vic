#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination) 
        : id(id), sourceNeuron(source), destinationNeuron(destination),
          weight(0.0f), delay(1), type(SynapseType::Excitatory),
          eligibilityTrace(0.0f), efficacy(1.0f),
          shortTermDepression(1.0f), shortTermFacilitation(0.0f),
          lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f) {}
    
    ~Synapse() = default;
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&& other) noexcept : id(other.id), sourceNeuron(other.sourceNeuron),
        destinationNeuron(other.destinationNeuron), weight(other.weight), delay(other.delay),
        type(other.type), preSpikeHistory(std::move(other.preSpikeHistory)),
        postSpikeHistory(std::move(other.postSpikeHistory)),
        plasticityFlags(std::move(other.plasticityFlags)),
        shortTermDepression(other.shortTermDepression),
        shortTermFacilitation(other.shortTermFacilitation),
        lastPreSpikeTime(other.lastPreSpikeTime),
        lastPostSpikeTime(other.lastPostSpikeTime),
        eligibilityTrace(other.eligibilityTrace), efficacy(other.efficacy) {
        other.id = INVALID_SYNAPSE_ID;
        other.sourceNeuron = INVALID_NEURON_ID;
        other.destinationNeuron = INVALID_NEURON_ID;
    }
    Synapse& operator=(Synapse&& other) noexcept {
        if (this != &other) {
            id = other.id;
            sourceNeuron = other.sourceNeuron;
            destinationNeuron = other.destinationNeuron;
            weight = other.weight;
            delay = other.delay;
            type = other.type;
            preSpikeHistory = std::move(other.preSpikeHistory);
            postSpikeHistory = std::move(other.postSpikeHistory);
            plasticityFlags = std::move(other.plasticityFlags);
            shortTermDepression = other.shortTermDepression;
            shortTermFacilitation = other.shortTermFacilitation;
            lastPreSpikeTime = other.lastPreSpikeTime;
            lastPostSpikeTime = other.lastPostSpikeTime;
            eligibilityTrace = other.eligibilityTrace;
            efficacy = other.efficacy;
            other.id = INVALID_SYNAPSE_ID;
            other.sourceNeuron = INVALID_NEURON_ID;
            other.destinationNeuron = INVALID_NEURON_ID;
        }
        return *this;
    }
    
    // Identity
    SynapseId getId() const { return id; }
    
    // Connection
    NeuronId getSourceNeuron() const { return sourceNeuron; }
    NeuronId getDestinationNeuron() const { return destinationNeuron; }
    
    // Weight
    SynapticWeight getWeight() const { return weight; }
    void setWeight(SynapticWeight weight) { this->weight = weight; }
    void addToWeight(SynapticWeight delta) {
        weight += delta;
        weight = std::clamp(weight, MIN_WEIGHT, MAX_WEIGHT);
    }
    
    // Delay (in simulation steps)
    Delay getDelay() const { return delay; }
    void setDelay(Delay delay) { this->delay = delay; }
    
    // Type
    SynapseType getType() const { return type; }
    void setType(SynapseType type) { this->type = type; }
    
    // Excitatory/Inhibitory
    bool isExcitatory() const { return type == SynapseType::Excitatory; }
    bool isInhibitory() const { return type == SynapseType::Inhibitory; }
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp) {
        preSpikeHistory.push_back(timestamp);
        lastPreSpikeTime = static_cast<float>(timestamp);
        if (preSpikeHistory.size() > MAX_SPIKE_HISTORY) {
            preSpikeHistory.erase(preSpikeHistory.begin());
        }
    }
    
    void recordPostSpike(Timestamp timestamp) {
        postSpikeHistory.push_back(timestamp);
        lastPostSpikeTime = static_cast<float>(timestamp);
        if (postSpikeHistory.size() > MAX_SPIKE_HISTORY) {
            postSpikeHistory.erase(postSpikeHistory.begin());
        }
    }
    
    const std::vector<Timestamp>& getPreSpikeHistory() const { return preSpikeHistory; }
    const std::vector<Timestamp>& getPostSpikeHistory() const { return postSpikeHistory; }
    
    void clearHistory() {
        preSpikeHistory.clear();
        postSpikeHistory.clear();
    }
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const { return plasticityFlags; }
    PlasticityFlags& getPlasticityFlags() { return plasticityFlags; }
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
        plasticityFlags.hebbian = hebbian;
        plasticityFlags.stdp = stdp;
        plasticityFlags.reward_modulated = rewardModulated;
    }
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const { return eligibilityTrace; }
    void setEligibilityTrace(float trace) { eligibilityTrace = trace; }
    void decayEligibilityTrace(float decayRate) {
        eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(eligibilityTrace) < 0.001f) {
            eligibilityTrace = 0.0f;
        }
    }
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const { return efficacy; }
    void setEfficacy(float efficacy) { this->efficacy = std::clamp(efficacy, 0.0f, 2.0f); }
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime) {
        // Real synaptic dynamics:
        // 1. Decay short-term plasticity state
        // 2. Decay eligibility trace
        // 3. Update efficacy based on use
        
        // Decay short-term facilitation (Tsodyks-Markram model)
        if (lastPreSpikeTime >= 0.0f) {
            float timeSincePre = static_cast<float>(currentTime - lastPreSpikeTime);
            shortTermFacilitation *= std::exp(-timeSincePre / STP_FACILITATION_TAU);
        }
        
        // Decay short-term depression
        if (lastPostSpikeTime >= 0.0f || lastPreSpikeTime >= 0.0f) {
            float timeSinceActivity = std::max(
                lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - lastPostSpikeTime) : 0.0f,
                lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - lastPreSpikeTime) : 0.0f
            );
            // Recovery from depression toward 1.0
            shortTermDepression += (1.0f - shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / STP_DEPRESSION_TAU));
        }
        
        // Decay eligibility trace for reward-modulated learning
        decayEligibilityTrace(0.001f);  // Fast decay
        
        // Clamp weight bounds
        weight = std::clamp(weight, MIN_WEIGHT, MAX_WEIGHT);
    }
    
    // Reset to initial state
    void reset() {
        weight = 0.0f;
        preSpikeHistory.clear();
        postSpikeHistory.clear();
        eligibilityTrace = 0.0f;
        efficacy = 1.0f;
        shortTermDepression = 1.0f;
        shortTermFacilitation = 0.0f;
        lastPreSpikeTime = -1.0f;
        lastPostSpikeTime = -1.0f;
    }
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng) {
        // Proper random initialization based on synapse type
        if (type == SynapseType::Excitatory) {
            // Excitatory synapses: small positive weights
            weight = rng.uniformReal(0.1f, 0.4f);
            // Excitatory synapses have moderate initial efficacy
            efficacy = rng.uniformReal(0.8f, 1.0f);
        } else if (type == SynapseType::Inhibitory) {
            // Inhibitory synapses: negative weights
            weight = -rng.uniformReal(0.1f, 0.4f);
            efficacy = rng.uniformReal(0.8f, 1.0f);
        } else {
            // Other types: small random weights
            weight = rng.uniformReal(-0.1f, 0.1f);
            efficacy = rng.uniformReal(0.9f, 1.0f);
        }
        
        // Random delay: 1-5 steps (1-5ms at 1ms timestep)
        delay = static_cast<Delay>(rng.uniformInt(static_cast<uint64_t>(1), static_cast<uint64_t>(5)));
        
        // Initialize short-term plasticity state
        shortTermDepression = 1.0f;  // Fully recovered
        shortTermFacilitation = 0.0f;  // No initial facilitation
        
        // Initialize eligibility trace to 0
        eligibilityTrace = 0.0f;
        
        // Initialize last spike times
        lastPreSpikeTime = -1.0f;
        lastPostSpikeTime = -1.0f;
    }
    
private:
    // Synapse implementation details (moved from pImpl)
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
};

} // namespace nlm
