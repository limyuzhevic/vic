#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
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
    
    // Spike history for STDP - use arrays for better performance
    std::array<Timestamp, 100> preSpikeHistoryArray;
    std::array<Timestamp, 100> postSpikeHistoryArray;
    size_t preSpikeCount = 0;
    size_t postSpikeCount = 0;
    size_t writeIndex = 0;
    
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
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), eligibilityTrace(0.0f), efficacy(1.0f),
             shortTermDepression(1.0f), shortTermFacilitation(0.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             preSpikeCount(0), postSpikeCount(0), writeIndex(0) {
        // Initialize spike history arrays with sentinel values
        std::fill(std::begin(preSpikeHistoryArray), std::end(preSpikeHistoryArray), -1.0f);
        std::fill(std::begin(postSpikeHistoryArray), std::end(postSpikeHistoryArray), -1.0f);
    }
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    if (!id.index() || !source.index() || !destination.index()) {
        throw std::invalid_argument("Invalid neuron or synapse IDs");
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

SynapseId Synapse::getId() const {
    return pImpl ? pImpl->id : INVALID_SYNAPSE_ID;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl ? pImpl->sourceNeuron : INVALID_NEURON_ID;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl ? pImpl->destinationNeuron : INVALID_NEURON_ID;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl ? pImpl->weight : 0.0f;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (!pImpl) return;
    
    // Validate input weight
    if (std::isnan(weight)) {
        weight = 0.0f;
    }
    
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) return;
    
    // Validate delta
    if (std::isnan(delta)) delta = 0.0f;
    
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl ? pImpl->delay : 1;
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) return;
    
    // Validate delay
    if (delay < 1) delay = 1;
    if (delay > 1000) delay = 1000;  // Reasonable maximum
    
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl ? pImpl->type : SynapseType::Excitatory;
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) return;
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    return pImpl && pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl && pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) return;
    
    // Validate timestamp
    if (std::isnan(timestamp)) return;
    
    // Use circular buffer for efficient spike history management
    pImpl->preSpikeHistoryArray[pImpl->writeIndex] = timestamp;
    pImpl->writeIndex = (pImpl->writeIndex + 1) % 100;
    if (pImpl->preSpikeCount < 100) {
        pImpl->preSpikeCount++;
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) return;
    
    // Validate timestamp
    if (std::isnan(timestamp)) return;
    
    // Use circular buffer for efficient spike history management
    size_t postWriteIndex = (pImpl->writeIndex + 50) % 100;  // Use different index for post
    pImpl->postSpikeHistoryArray[postWriteIndex] = timestamp;
    if (pImpl->postSpikeCount < 100) {
        pImpl->postSpikeCount++;
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    static std::vector<Timestamp> history;
    history.clear();
    if (!pImpl) return history;
    
    // Convert circular buffer to vector (costly but preserves API compatibility)
    history.reserve(pImpl->preSpikeCount);
    for (size_t i = 0; i < pImpl->preSpikeCount; ++i) {
        size_t idx = (pImpl->writeIndex + 100 - pImpl->preSpikeCount + i) % 100;
        history.push_back(pImpl->preSpikeHistoryArray[idx]);
    }
    return history;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    static std::vector<Timestamp> history;
    history.clear();
    if (!pImpl) return history;
    
    // Convert circular buffer to vector (costly but preserves API compatibility)
    history.reserve(pImpl->postSpikeCount);
    size_t postWriteIndex = (pImpl->writeIndex + 50) % 100;
    for (size_t i = 0; i < pImpl->postSpikeCount; ++i) {
        size_t idx = (postWriteIndex + 100 - pImpl->postSpikeCount + i) % 100;
        history.push_back(pImpl->postSpikeHistoryArray[idx]);
    }
    return history;
}

void Synapse::clearHistory() {
    if (!pImpl) return;
    
    std::fill(std::begin(pImpl->preSpikeHistoryArray), std::end(pImpl->preSpikeHistoryArray), -1.0f);
    std::fill(std::begin(pImpl->postSpikeHistoryArray), std::end(pImpl->postSpikeHistoryArray), -1.0f);
    pImpl->preSpikeCount = 0;
    pImpl->postSpikeCount = 0;
    pImpl->writeIndex = 0;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    static PlasticityFlags emptyFlags;
    return pImpl ? pImpl->plasticityFlags : emptyFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) return;
    
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    static const PlasticityFlags emptyFlags;
    return pImpl ? pImpl->plasticityFlags : emptyFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl ? pImpl->eligibilityTrace : 0.0f;
}

void Synapse::setEligibilityTrace(float trace) {
    if (!pImpl) return;
    
    // Validate trace value
    if (std::isnan(trace)) trace = 0.0f;
    pImpl->eligibilityTrace = std::clamp(trace, -1.0f, 1.0f);
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) return;
    
    // Validate decay rate
    if (decayRate < 0.0f) decayRate = 0.0f;
    if (decayRate > 1.0f) decayRate = 1.0f;
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl ? pImpl->efficacy : 1.0f;
}

void Synapse::setEfficacy(float efficacy) {
    if (!pImpl) return;
    
    // Validate efficacy
    if (std::isnan(efficacy)) efficacy = 1.0f;
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) return;
    
    // Validate current time
    if (std::isnan(currentTime)) return;
    
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        if (timeSincePre >= 0.0f) {
            pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        }
    }
    
    // Decay short-term depression
    float timeSinceActivity = 0.0f;
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        timeSinceActivity = std::max(timeSinceActivity, static_cast<float>(currentTime - pImpl->lastPostSpikeTime));
    }
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        timeSinceActivity = std::max(timeSinceActivity, static_cast<float>(currentTime - pImpl->lastPreSpikeTime));
    }
    
    if (timeSinceActivity > 0.0f) {
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Clamp efficacy bounds
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

void Synapse::reset() {
    if (!pImpl) return;
    
    pImpl->weight = 0.0f;
    clearHistory();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) return;
    
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
    
    // Validate weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
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
