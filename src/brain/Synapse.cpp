#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP - using circular buffer for efficiency
    std::vector<Timestamp> preSpikeHistory;
    size_t preSpikeHistoryHead;
    size_t preSpikeHistoryCount;
    
    std::vector<Timestamp> postSpikeHistory;
    size_t postSpikeHistoryHead;
    size_t postSpikeHistoryCount;
    
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
    
    Impl()
        : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1)
        , type(SynapseType::Excitatory), preSpikeHistoryHead(0), preSpikeHistoryCount(0)
        , postSpikeHistoryHead(0), postSpikeHistoryCount(0)
        , shortTermDepression(1.0f), shortTermFacilitation(0.0f)
        , lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f), eligibilityTrace(0.0f), efficacy(1.0f)
    {
        preSpikeHistory.resize(MAX_SPIKE_HISTORY);
        postSpikeHistory.resize(MAX_SPIKE_HISTORY);
    }
};

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
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    // Transfer ownership of all resources from other
    if (other.pImpl) {
        // Transfer ownership of spike history and other resources
        pImpl->preSpikeHistory = std::move(other.pImpl->preSpikeHistory);
        pImpl->postSpikeHistory = std::move(other.pImpl->postSpikeHistory);
        pImpl->plasticityFlags = other.pImpl->plasticityFlags;
        // other resources stay with the moved-from object for safety
        other.pImpl = nullptr;
    }
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        // Transfer ownership of all resources from other
        if (other.pImpl) {
            pImpl->preSpikeHistory = std::move(other.pImpl->preSpikeHistory);
            pImpl->postSpikeHistory = std::move(other.pImpl->postSpikeHistory);
            pImpl->plasticityFlags = other.pImpl->plasticityFlags;
            // other resources stay with the moved-from object for safety
            other.pImpl = nullptr;
        }
    }
    return *this;
}

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

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    // Validate input
    if (timestamp < 0.0) {
        // Invalid timestamp, ignore
        return;
    }
    
    // Store spike in circular buffer
    pImpl->preSpikeHistory[pImpl->preSpikeHistoryHead] = timestamp;
    pImpl->preSpikeHistoryHead = (pImpl->preSpikeHistoryHead + 1) % pImpl->preSpikeHistory.size();
    if (pImpl->preSpikeHistoryCount < pImpl->preSpikeHistory.size()) {
        pImpl->preSpikeHistoryCount++;
    }
    
    // Track last pre-spike time for short-term plasticity
    pImpl->lastPreSpikeTime = timestamp;
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Validate input
    if (timestamp < 0.0) {
        // Invalid timestamp, ignore
        return;
    }
    
    // Store spike in circular buffer
    pImpl->postSpikeHistory[pImpl->postSpikeHistoryHead] = timestamp;
    pImpl->postSpikeHistoryHead = (pImpl->postSpikeHistoryHead + 1) % pImpl->postSpikeHistory.size();
    if (pImpl->postSpikeHistoryCount < pImpl->postSpikeHistory.size()) {
        pImpl->postSpikeHistoryCount++;
    }
    
    // Track last post-spike time for short-term plasticity
    pImpl->lastPostSpikeTime = timestamp;
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    // Return only the valid entries (not the entire buffer)
    static std::vector<Timestamp> validHistory;
    validHistory.clear();
    validHistory.reserve(pImpl->preSpikeHistoryCount);
    
    if (pImpl->preSpikeHistoryCount == 0) {
        return validHistory;
    }
    
    // Copy entries from circular buffer
    if (pImpl->preSpikeHistoryHead == 0 && pImpl->preSpikeHistoryCount == pImpl->preSpikeHistory.size()) {
        // Buffer is full
        validHistory = pImpl->preSpikeHistory;
    } else {
        // Buffer is partially filled - copy from head to end, then from beginning to head
        size_t elementsAfterHead = pImpl->preSpikeHistory.size() - pImpl->preSpikeHistoryHead;
        size_t elementsToCopy = std::min(pImpl->preSpikeHistoryCount, elementsAfterHead);
        
        validHistory.insert(validHistory.end(),
                           pImpl->preSpikeHistory.begin() + pImpl->preSpikeHistoryHead,
                           pImpl->preSpikeHistory.begin() + pImpl->preSpikeHistoryHead + elementsToCopy);
        
        if (pImpl->preSpikeHistoryCount > elementsToCopy) {
            validHistory.insert(validHistory.end(),
                               pImpl->preSpikeHistory.begin(),
                               pImpl->preSpikeHistory.begin() + (pImpl->preSpikeHistoryCount - elementsToCopy));
        }
    }
    
    return validHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    // Return only the valid entries (not the entire buffer)
    static std::vector<Timestamp> validHistory;
    validHistory.clear();
    validHistory.reserve(pImpl->postSpikeHistoryCount);
    
    if (pImpl->postSpikeHistoryCount == 0) {
        return validHistory;
    }
    
    // Copy entries from circular buffer
    if (pImpl->postSpikeHistoryHead == 0 && pImpl->postSpikeHistoryCount == pImpl->postSpikeHistory.size()) {
        // Buffer is full
        validHistory = pImpl->postSpikeHistory;
    } else {
        // Buffer is partially filled - copy from head to end, then from beginning to head
        size_t elementsAfterHead = pImpl->postSpikeHistory.size() - pImpl->postSpikeHistoryHead;
        size_t elementsToCopy = std::min(pImpl->postSpikeHistoryCount, elementsAfterHead);
        
        validHistory.insert(validHistory.end(),
                           pImpl->postSpikeHistory.begin() + pImpl->postSpikeHistoryHead,
                           pImpl->postSpikeHistory.begin() + pImpl->postSpikeHistoryHead + elementsToCopy);
        
        if (pImpl->postSpikeHistoryCount > elementsToCopy) {
            validHistory.insert(validHistory.end(),
                               pImpl->postSpikeHistory.begin(),
                               pImpl->postSpikeHistory.begin() + (pImpl->postSpikeHistoryCount - elementsToCopy));
        }
    }
    
    return validHistory;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

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
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

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
