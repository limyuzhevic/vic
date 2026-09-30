#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Synapse::Impl {
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
    
    // Weight bounds with configurable ranges
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    static constexpr float MIN_EFFICACY = 0.0f;
    static constexpr float MAX_EFFICACY = 2.0f;
    static constexpr float MIN_DELAY = 1;
    static constexpr float MAX_DELAY = 100;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Integration constants
    static constexpr float EFFICACY_DECAY = 0.95f;  // Efficacy decay per time step
    static constexpr float ELIGIBILITY_DECAY = 0.99f;  // Eligibility trace decay per time step
};

// Factory function for creating synapses
Synapse createSynapse(SynapseId id, NeuronId source, NeuronId destination, SynapseType type) {
    Synapse synapse(id, source, destination);
    synapse.setType(type);
    return synapse;
}

// Validation function for synapse parameters
bool validateSynapseParameters(SynapseType type, SynapticWeight weight, Delay delay) {
    // Validate weight range
    if (weight < Impl::MIN_WEIGHT || weight > Impl::MAX_WEIGHT) {
        return false;
    }
    
    // Validate delay range
    if (delay < Impl::MIN_DELAY || delay > Impl::MAX_DELAY) {
        return false;
    }
    
    // Validate synapse type
    switch (type) {
        case SynapseType::Excitatory:
        case SynapseType::Inhibitory:
        case SynapseType::Modulatory:
            return true;
        default:
            return false;
    }
}

// Constructor
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Validate parameters
    if (!validateSynapseParameters(pImpl->type, pImpl->weight, pImpl->delay)) {
        throw std::invalid_argument("Invalid synapse parameters");
    }
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Getters
SynapseId Synapse::getId() const {
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

bool Synapse::isModulatory() const {
    return pImpl->type == SynapseType::Modulatory;
}

// Setters with validation
void Synapse::setWeight(SynapticWeight weight) {
    if (!validateSynapseParameters(pImpl->type, weight, pImpl->delay)) {
        throw std::invalid_argument("Invalid weight value");
    }
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::setDelay(Delay delay) {
    if (!validateSynapseParameters(pImpl->type, pImpl->weight, delay)) {
        throw std::invalid_argument("Invalid delay value");
    }
    pImpl->delay = delay;
}

void Synapse::setType(SynapseType type) {
    if (!validateSynapseParameters(type, pImpl->weight, pImpl->delay)) {
        throw std::invalid_argument("Invalid synapse type");
    }
    pImpl->type = type;
}

// Spike history management
void Synapse::recordPreSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Timestamp cannot be negative");
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update last spike time for short-term plasticity
    pImpl->lastPreSpikeTime = timestamp;
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Timestamp cannot be negative");
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update last spike time for short-term plasticity
    pImpl->lastPostSpikeTime = timestamp;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

// Plasticity management
PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

// Short-term plasticity management
float Synapse::getShortTermDepression() const {
    return pImpl->shortTermDepression;
}

float Synapse::getShortTermFacilitation() const {
    return pImpl->shortTermFacilitation;
}

void Synapse::setShortTermDepression(float depression) {
    pImpl->shortTermDepression = std::clamp(depression, 0.0f, Impl::STP_U_MAX);
}

void Synapse::setShortTermFacilitation(float facilitation) {
    pImpl->shortTermFacilitation = std::clamp(facilitation, 0.0f, Impl::STP_U_MAX);
}

// Eligibility trace management
float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (trace < 0.0f || trace > 1.0f) {
        throw std::invalid_argument("Eligibility trace must be between 0.0 and 1.0");
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (decayRate < 0.0f || decayRate >= 1.0f) {
        throw std::invalid_argument("Decay rate must be between 0.0 and 1.0");
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

// Efficacy management
float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (!validateSynapseParameters(pImpl->type, pImpl->weight, std::max(Impl::MIN_DELAY, std::min(Impl::MAX_DELAY, pImpl->delay)))) {
        throw std::invalid_argument("Invalid efficacy value");
    }
    pImpl->efficacy = std::clamp(efficacy, Impl::MIN_EFFICACY, Impl::MAX_EFFICACY);
}

void Synapse::updateEfficacy(float delta) {
    // Update synaptic efficacy based on use (Hebbian-like process)
    pImpl->efficacy += delta;
    pImpl->efficacy = std::clamp(pImpl->efficacy, Impl::MIN_EFFICACY, Impl::MAX_EFFICACY);
}

// Step processing for synaptic dynamics
void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(Impl::ELIGIBILITY_DECAY);  // Fast decay
    
    // Decay efficacy over time
    pImpl->efficacy *= Impl::EFFICACY_DECAY;
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    pImpl->efficacy = std::clamp(pImpl->efficacy, Impl::MIN_EFFICACY, Impl::MAX_EFFICACY);
}

// Reset synapse to initial state
void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Initialize with random parameters based on type
void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Validate parameters
    if (!validateSynapseParameters(pImpl->type, pImpl->weight, pImpl->delay)) {
        throw std::runtime_error("Invalid random synapse parameters generated");
    }
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Utility functions
std::string SynapseTypeToString(SynapseType type) {
    switch (type) {
        case SynapseType::Excitatory: return "Excitatory";
        case SynapseType::Inhibitory: return "Inhibitory";
        case SynapseType::Modulatory: return "Modulatory";
        default: return "Unknown";
    }
}

SynapseType StringToSynapseType(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "excitatory" || lower == "exc") return SynapseType::Excitatory;
    if (lower == "inhibitory" || lower == "inh") return SynapseType::Inhibitory;
    if (lower == "modulatory" || lower == "mod") return SynapseType::Modulatory;
    
    throw std::invalid_argument("Unknown synapse type: " + str);
}

} // namespace nlm
