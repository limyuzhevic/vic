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
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::getId(): pImpl is null");
        return INVALID_SYNAPSE_ID;
    }
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::getSourceNeuron(): pImpl is null");
        return INVALID_NEURON_ID;
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::getDestinationNeuron(): pImpl is null");
        return INVALID_NEURON_ID;
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::getWeight(): pImpl is null");
        return 0.0f;
    }
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::setWeight(): pImpl is null");
        return;
    }
    
    // Validate weight bounds before setting
    if (weight < Impl::MIN_WEIGHT || weight > Impl::MAX_WEIGHT) {
        NLM_LOG_ERROR("Synapse::setWeight(): Weight " + std::to_string(weight) + 
                     " is out of valid range [" + std::to_string(Impl::MIN_WEIGHT) + ", " + 
                     std::to_string(Impl::MAX_WEIGHT) + "]");
        return;
    }
    
    pImpl->weight = weight;
    NLM_LOG_DEBUG("Synapse::setWeight(): Successfully set weight to " + std::to_string(weight));
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::addToWeight(): pImpl is null");
        return;
    }
    
    float newWeight = pImpl->weight + delta;
    if (newWeight < Impl::MIN_WEIGHT || newWeight > Impl::MAX_WEIGHT) {
        NLM_LOG_WARNING("Synapse::addToWeight(): Weight change would exceed bounds. "
                       "New weight would be " + std::to_string(newWeight) +
                       " (current: " + std::to_string(pImpl->weight) + ")");
    }
    
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    NLM_LOG_DEBUG("Synapse::addToWeight(): Added " + std::to_string(delta) + " to weight, new value: " + std::to_string(pImpl->weight));
}

Delay Synapse::getDelay() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::getDelay(): pImpl is null");
        return 1;  // Default delay
    }
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::setDelay(): pImpl is null");
        return;
    }
    
    // Validate delay is reasonable (greater than 0)
    if (delay <= 0) {
        NLM_LOG_WARNING("Synapse::setDelay(): Delay must be positive, got " + std::to_string(delay) +
                       ". Setting to minimum value of 1.");
        pImpl->delay = 1;
    } else {
        pImpl->delay = delay;
    }
    NLM_LOG_DEBUG("Synapse::setDelay(): Set delay to " + std::to_string(pImpl->delay));
}

SynapseType Synapse::getType() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::getType(): pImpl is null");
        return SynapseType::Excitatory;  // Default
    }
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::setType(): pImpl is null");
        return;
    }
    pImpl->type = type;
    NLM_LOG_DEBUG("Synapse::setType(): Set type to " + std::to_string(static_cast<int>(type)));
}

bool Synapse::isExcitatory() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::isExcitatory(): pImpl is null");
        return false;
    }
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::isInhibitory(): pImpl is null");
        return false;
    }
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::recordPreSpike(): pImpl is null");
        return;
    }
    
    // Validate timestamp is reasonable (not negative or excessively large)
    if (timestamp < 0.0) {
        NLM_LOG_WARNING("Synapse::recordPreSpike(): Timestamp cannot be negative, got " + std::to_string(timestamp));
        return;
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        NLM_LOG_DEBUG("Synapse::recordPreSpike(): Spike history overflow, trimmed to max size");
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::recordPostSpike(): pImpl is null");
        return;
    }
    
    // Validate timestamp
    if (timestamp < 0.0) {
        NLM_LOG_WARNING("Synapse::recordPostSpike(): Timestamp cannot be negative, got " + std::to_string(timestamp));
        return;
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        NLM_LOG_DEBUG("Synapse::recordPostSpike(): Spike history overflow, trimmed to max size");
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
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
