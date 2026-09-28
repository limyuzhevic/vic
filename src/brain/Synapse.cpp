#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace nlm {

// Forward declarations for better organization
class RandomGenerator;

// Improved Synapse::Impl structure with better organization
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
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), eligibilityTrace(0.0f), efficacy(1.0f),
             shortTermDepression(1.0f), shortTermFacilitation(0.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate synapse memory");
    }
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

// Identity accessors
SynapseId Synapse::getId() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    validateWeight(weight);
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    validateWeightChange(delta);
    float newWeight = pImpl->weight + delta;
    validateWeight(newWeight);
    pImpl->weight = newWeight;
}

Delay Synapse::getDelay() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    if (delay < 0) {
        throw std::runtime_error("Delay cannot be negative");
    }
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->type == SynapseType::Inhibitory;
}

// Spike history management for STDP
void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        throw std::runtime_error("Invalid spike timestamp");
    }
    pImpl->preSpikeHistory.push_back(timestamp);
    updateLastPreSpikeTime(timestamp);
    
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        throw std::runtime_error("Invalid spike timestamp");
    }
    pImpl->postSpikeHistory.push_back(timestamp);
    updateLastPostSpikeTime(timestamp);
    
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

void Synapse::clearHistory() {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->postSpikeHistory;
}

// Plasticity flags
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->plasticityFlags;
}

// Eligibility trace for reward-modulated learning
float Synapse::getEligibilityTrace() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    if (decayRate < 0.0f || decayRate > 1.0f) {
        throw std::runtime_error("Eligibility trace decay rate must be between 0 and 1");
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

// Synaptic efficacy
float Synapse::getEfficacy() const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

// ========== SHORT-TERM PLASTICITY ==========

void Synapse::updateLastPreSpikeTime(float timestamp) {
    pImpl->lastPreSpikeTime = timestamp;
    
    // Update short-term facilitation using Tsodyks-Markram model
    // Facilitation increases the utilization factor U
    pImpl->shortTermFacilitation = 1.0f - std::exp(-timestamp / Impl::STP_FACILITATION_TAU);
    if (pImpl->shortTermFacilitation > Impl::STP_U_MAX) {
        pImpl->shortTermFacilitation = Impl::STP_U_MAX;
    }
}

void Synapse::updateLastPostSpikeTime(float timestamp) {
    pImpl->lastPostSpikeTime = timestamp;
    
    // Update short-term depression
    // Depression decreases the utilization factor U toward baseline
    float timeSinceLastActivity = std::max(pImpl->lastPreSpikeTime, pImpl->lastPostSpikeTime);
    if (timeSinceLastActivity >= 0.0f) {
        float recoveryFactor = 1.0f - std::exp(-timeSinceLastActivity / Impl::STP_DEPRESSION_TAU);
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * recoveryFactor;
        
        // Clamp depression factor
        if (pImpl->shortTermDepression > 1.0f) {
            pImpl->shortTermDepression = 1.0f;
        }
    }
}

float Synapse::getSTPUtilizationFactor() const {
    // Current utilization factor U = U_max * F * D
    // Where F = facilitation, D = depression
    float utilization = Impl::STP_U_MAX * pImpl->shortTermFacilitation * pImpl->shortTermDepression;
    return std::clamp(utilization, 0.0f, Impl::STP_U_MAX);
}

// ========== ELIGIBILITY TRACE SYSTEM ==========

void Synapse::updateEligibilityTrace(float delta, float learningRate) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    if (std::isnan(delta) || std::isinf(delta)) {
        throw std::runtime_error("Invalid eligibility trace delta");
    }
    if (learningRate < 0.0f) {
        throw std::runtime_error("Learning rate cannot be negative");
    }
    
    // Update eligibility trace for reward-modulated learning
    pImpl->eligibilityTrace += delta * learningRate;
    
    // Clamp eligibility trace to prevent runaway growth
    if (pImpl->eligibilityTrace > 1.0f) {
        pImpl->eligibilityTrace = 1.0f;
    } else if (pImpl->eligibilityTrace < -1.0f) {
        pImpl->eligibilityTrace = -1.0f;
    }
}

float Synapse::getModulatedWeightChange(float baseDelta, float rewardSignal) const {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    // Reward-modulated STDP: weight change is multiplied by reward signal
    return baseDelta * (1.0f + rewardSignal);
}

// ========== VALIDATION HELPERS ==========

void Synapse::validateWeight(SynapticWeight weight) const {
    if (weight < Impl::MIN_WEIGHT || weight > Impl::MAX_WEIGHT) {
        throw std::runtime_error("Synaptic weight out of bounds: " + std::to_string(weight));
    }
    if (std::isnan(weight) || std::isinf(weight)) {
        throw std::runtime_error("Invalid synaptic weight value");
    }
}

void Synapse::validateWeightChange(SynapticWeight delta) const {
    if (std::isnan(delta) || std::isinf(delta)) {
        throw std::runtime_error("Invalid weight change delta");
    }
    // Prevent extreme weight changes
    if (std::abs(delta) > 0.5f) {
        throw std::runtime_error("Weight change too extreme: " + std::to_string(delta));
    }
}

// ========== SYNAPSE STEP ==========

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
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
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    validateWeight(pImpl->weight);
}

// ========== RESET ==========

void Synapse::reset() {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
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

// ========== INITIALIZATION ==========

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        throw std::runtime_error("Synapse not properly initialized");
    }
    if (!rng.isValid()) {
        throw std::runtime_error("Invalid random generator");
    }
    
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
    
    // Validate initialized weight
    validateWeight(pImpl->weight);
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
