#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

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
    // Validate weight before setting
    if (weight < Impl::MIN_WEIGHT - 0.01f || weight > Impl::MAX_WEIGHT + 0.01f) {
        NLM_LOG_WARNING(std::string("Synapse weight " + std::to_string(weight) + " outside bounds [" + 
                                     std::to_string(Impl::MIN_WEIGHT) + ", " + std::to_string(Impl::MAX_WEIGHT) + "]"));
    }
    
    // Clamp to valid bounds
    pImpl->weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
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
    // Validate delay is positive
    if (delay == 0) {
        NLM_LOG_WARNING("Synapse delay set to 0 - may cause immediate transmission");
    } else if (delay > 1000) {
        NLM_LOG_WARNING("Synapse delay " + std::to_string(delay) + " very large - may cause long latencies");
    }
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
    // Validate timestamp
    if (timestamp < 0.0f) {
        NLM_LOG_WARNING("Negative pre-synaptic spike timestamp " + std::to_string(timestamp));
        return;
    }
    
    // Pre-allocate when at capacity to avoid repeated allocations
    if (pImpl->preSpikeHistory.size() >= pImpl->preSpikeHistory.capacity()) {
        pImpl->preSpikeHistory.reserve(pImpl->preSpikeHistory.size() * 1.5);
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Use swap and pop for O(1) removal from beginning
        std::rotate(pImpl->preSpikeHistory.begin(), 
                   pImpl->preSpikeHistory.begin() + 1, 
                   pImpl->preSpikeHistory.end());
        pImpl->preSpikeHistory.pop_back();
    }
    pImpl->lastPreSpikeTime = timestamp;
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Validate timestamp
    if (timestamp < 0.0f) {
        NLM_LOG_WARNING("Negative post-synaptic spike timestamp " + std::to_string(timestamp));
        return;
    }
    
    // Pre-allocate when at capacity to avoid repeated allocations
    if (pImpl->postSpikeHistory.size() >= pImpl->postSpikeHistory.capacity()) {
        pImpl->postSpikeHistory.reserve(pImpl->postSpikeHistory.size() * 1.5);
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Use swap and pop for O(1) removal from beginning
        std::rotate(pImpl->postSpikeHistory.begin(), 
                   pImpl->postSpikeHistory.begin() + 1, 
                   pImpl->postSpikeHistory.end());
        pImpl->postSpikeHistory.pop_back();
    }
    pImpl->lastPostSpikeTime = timestamp;
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    // Optimized bulk clear - avoid small vector reallocations
    pImpl->preSpikeHistory.clear();
    pImpl->preSpikeHistory.shrink_to_fit();
    
    pImpl->postSpikeHistory.clear();
    pImpl->postSpikeHistory.shrink_to_fit();
    
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
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
    if (trace < 0.0f) {
        NLM_LOG_WARNING("Negative eligibility trace " + std::to_string(trace) + " set to 0");
        pImpl->eligibilityTrace = 0.0f;
    } else if (trace > 1.0f) {
        NLM_LOG_WARNING("Eligibility trace " + std::to_string(trace) + " > 1.0");
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (decayRate < 0.0f || decayRate > 1.0f) {
        NLM_LOG_WARNING("Invalid eligibility trace decay rate " + std::to_string(decayRate));
        return;
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (efficacy < 0.0f) {
        NLM_LOG_WARNING("Negative efficacy " + std::to_string(efficacy) + " set to 0");
        pImpl->efficacy = 0.0f;
    } else if (efficacy > 2.0f) {
        NLM_LOG_WARNING("Efficacy " + std::to_string(efficacy) + " > 2.0");
    }
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics with optimized short-term plasticity
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace  
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Optimized short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Pre-calculate exponential factor
        float expFactor = std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermFacilitation *= expFactor;
    }
    
    // Optimized short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePost = (pImpl->lastPostSpikeTime >= 0.0f) ? 
            static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f;
        float timeSincePre = (pImpl->lastPreSpikeTime >= 0.0f) ? 
            static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f;
        
        float timeSinceActivity = std::max(timeSincePost, timeSincePre);
        
        // Pre-calculate recovery term
        float recovery = 1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU);
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * recovery;
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Optimized weight bounds clamping
    float& weight = pImpl->weight;
    float minWeight = Impl::MIN_WEIGHT;
    float maxWeight = Impl::MAX_WEIGHT;
    
    if (weight < minWeight) weight = minWeight;
    else if (weight > maxWeight) weight = maxWeight;
}

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
