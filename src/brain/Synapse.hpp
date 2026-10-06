#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Forward declaration
class Neuron;

// Constants for synaptic dynamics
namespace SynapseConstants {
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    static constexpr float MIN_EFFICIENCY = 0.0f;
    static constexpr float MAX_EFFICIENCY = 2.0f;
    static constexpr float DEFAULT_EFFICIENCY = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;   // ms
    static constexpr float STP_U_MAX = 1.0f;               // Max utilization
    
    // Eligibility trace
    static constexpr float ELIGIBILITY_DECAY = 0.001f;    // Fast decay
    static constexpr float MIN_ELIGIBILITY = 0.001f;
    
    // Spike history
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Weight bounds for different synapse types
    static constexpr float EXCITATORY_MIN_WEIGHT = 0.0f;
    static constexpr float EXCITATORY_MAX_WEIGHT = 1.0f;
    static constexpr float INHIBITORY_MIN_WEIGHT = -1.0f;
    static constexpr float INHIBITORY_MAX_WEIGHT = 0.0f;
    static constexpr float MODULATORY_MIN_WEIGHT = -0.5f;
    static constexpr float MODULATORY_MAX_WEIGHT = 0.5f;
}

// Configuration structure for advanced users
struct Synapse::Config {
    Delay initialDelay = 1;                           // Initial delay in steps
    float initialEfficacy = SynapseConstants::DEFAULT_EFFICIENCY;  // Initial efficacy
    bool enableShortTermPlasticity = true;           // Enable STP
    bool enableEligibilityTrace = true;             // Enable reward-modulated learning
    float weightClampingStrength = 0.99f;            // Strength of weight clamping
    
    // STP parameters
    float stpFacilitationTau = SynapseConstants::STP_FACILITATION_TAU;
    float stpDepressionTau = SynapseConstants::STP_DEPRESSION_TAU;
    float stpUMax = SynapseConstants::STP_U_MAX;
    
    // Eligibility trace parameters
    float eligibilityDecayRate = SynapseConstants::ELIGIBILITY_DECAY;
    float minEligibility = SynapseConstants::MIN_ELIGIBILITY;
    
    // Spike history parameters
    size_t maxSpikeHistory = SynapseConstants::MAX_SPIKE_HISTORY;
};

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    // Create synapse with full configuration
    Synapse(SynapseId id, NeuronId source, NeuronId destination, const Config& config);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const { return id_; }
    
    // Connection
    NeuronId getSourceNeuron() const { return sourceNeuron_; }
    NeuronId getDestinationNeuron() const { return destinationNeuron_; }
    
    // Weight
    SynapticWeight getWeight() const { return weight_; }
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const { return delay_; }
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const { return type_; }
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const { return type_ == SynapseType::Excitatory; }
    bool isInhibitory() const { return type_ == SynapseType::Inhibitory; }
    bool isModulatory() const { return type_ == SynapseType::Modulatory; }
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const { return preSpikeHistory_; }
    const std::vector<Timestamp>& getPostSpikeHistory() const { return postSpikeHistory_; }
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const { return plasticityFlags_; }
    PlasticityFlags& getPlasticityFlags() { return plasticityFlags_; }
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const { return eligibilityTrace_; }
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const { return efficacy_; }
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Validation functions
    void validateWeight() const;
    void validateEfficacy() const;
    void validateHistorySizes() const;
    bool isSafe() const;
    
    // Configuration methods
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }
    
    // Advanced features
    void enableAdvancedFeatures(bool enable, float noiseLevel = 0.1f);
    bool isAdvancedFeaturesEnabled() const { return advancedFeaturesEnabled_; }
    
private:
    // Helper functions
    void updateShortTermPlasticity(Timestamp currentTime);
    void clampWeight();
    void normalizeEfficacy();
    float calculateShortTermEffectiveness() const;
    
    // Member variables (no more PIMPL!)
    SynapseId id_;
    NeuronId sourceNeuron_;
    NeuronId destinationNeuron_;
    SynapticWeight weight_;
    Delay delay_;
    SynapseType type_;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory_;
    std::vector<Timestamp> postSpikeHistory_;
    
    // Plasticity state
    PlasticityFlags plasticityFlags_;
    
    // Short-term plasticity state
    float shortTermDepression_;  // Utilization factor (0-1)
    float shortTermFacilitation_;  // Facilitation factor
    float lastPreSpikeTime_;     // For short-term dynamics
    float lastPostSpikeTime_;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace_;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy_;
    
    // Configuration
    Config config_;
    
    // Advanced features
    bool advancedFeaturesEnabled_;
    float noiseLevel_;
};

} // namespace nlm
