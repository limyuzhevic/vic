// ===== CONSTANTS AND CONFIGURATION =====

// Synaptic weight bounds
static constexpr float SYNAPSE_MIN_WEIGHT = -1.0f;
static constexpr float SYNAPSE_MAX_WEIGHT = 1.0f;

// Short-term plasticity parameters
static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
static constexpr float STP_DEPRESSION_TAU = 200.0f;    // ms
static constexpr float STP_U_MAX = 1.0f;                // Max utilization

// Spike history limits
static constexpr size_t MAX_SPIKE_HISTORY = 100;

// ===== FORWARD DECLARATIONS AND INCLUDES =====

#include "../core/Types/Types.hpp"
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// ===== HELPER FUNCTIONS =====

// Clamps a weight between min and max
inline float clampWeight(float weight) {
    return std::max(SYNAPSE_MIN_WEIGHT, std::min(weight, SYNAPSE_MAX_WEIGHT));
}

// Calculates exponential decay
inline float exponentialDecay(float value, float timeConstant, float deltaTime) {
    return value * std::exp(-deltaTime / timeConstant);
}

// Checks if weight change is significant
inline bool isSignificantChange(float delta, float threshold = 0.01f) {
    return std::abs(delta) >= threshold;
}

// ===== SYNAPSE CLASS =====

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
    const std gắn<>(Timestamp>& getPreSpikeHistory() const;
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
