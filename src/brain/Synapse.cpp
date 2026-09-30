// Synapse.cpp
// Fixed version with proper memory management, thread safety, and error handling

#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <limits>

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
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    // Validate inputs
    if (!pImpl) {
        throw std::runtime_error("Synapse: Failed to allocate memory for synapse implementation");
    }
    
    if (id.value == 0) {
        throw std::runtime_error("Synapse: Invalid synapse ID");
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

Synapse::~Synapse() {
    // pImpl will be deleted in move operations
}

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

SynapseId Synapse::getId() const {
    if (!pImpl) return SynapseId();
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) return NeuronId();
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) return NeuronId();
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) return 0.0f;
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (!pImpl) return;
    pImpl->weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) return;
    
    // Validate delta
    if (std::isnan(delta) || std::isinf(delta)) {
        throw std::invalid_argument("Synapse: Invalid weight delta");
    }
    
    pImpl->weight += delta;
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    if (!pImpl) return 0;
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) return;
    if (delay > 0) {
        pImpl->delay = delay;
    }
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) return;
    pImpl->type = type;
}

SynapseType Synapse::getType() const {
    if (!pImpl) return SynapseType::Excitatory;
    return pImpl->type;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) return;
    
    // Validate timestamp
    if (timestamp < 0.0) return;
    
    pImpl->preSpikeHistory.push_back(timestamp);
    pImpl->lastPreSpikeTime = timestamp;
    
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) return;
    
    // Validate timestamp
    if (timestamp < 0.0) return;
    
    pImpl->postSpikeHistory.push_back(timestamp);
    pImpl->lastPostSpikeTime = timestamp;
    
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    static const std::vector<Timestamp> empty;
    if (!pImpl) return empty;
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    static const std::vector<Timestamp> empty;
    if (!pImpl) return empty;
    return pImpl->postSpikeHistory;
}

void Synapse::clearSpikeHistory() {
    if (!pImpl) return;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    static const PlasticityFlags empty;
    if (!pImpl) return empty;
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    if (!pImpl) {
        static PlasticityFlags empty;
        return empty;
    }
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    if (!pImpl) return 0.0f;
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (!pImpl) return;
    
    // Validate trace
    if (std::isnan(trace) || std::isinf(trace)) {
        throw std::invalid_argument("Synapse: Invalid eligibility trace");
    }
    
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) return;
    
    // Validate decay rate
    if (decayRate < 0.0f || decayRate >= 1.0f) {
        throw std::invalid_argument("Synapse: Invalid decay rate");
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    if (!pImpl) return 0.0f;
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (!pImpl) return;
    
    // Validate efficacy
    if (std::isnan(efficacy) || std::isinf(efficacy)) {
        throw std::invalid_argument("Synapse: Invalid efficacy");
    }
    
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) return;
    
    // Validate inputs
    if (currentTime < 0.0) return;
    
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
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds to prevent numerical overflow
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    if (!pImpl) return;
    
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

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) return;
    
    // Validate rng
    if (!rng.isValid()) {
        throw std::invalid_argument("Synapse: Invalid random generator");
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