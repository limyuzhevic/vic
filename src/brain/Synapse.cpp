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
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::setWeight: Attempting to access null implementation");
        return;
    }
    // Validate weight is within reasonable bounds
    if (weight < Impl::MIN_WEIGHT - 0.1f || weight > Impl::MAX_WEIGHT + 0.1f) {
        NLM_LOG_WARNING("Synapse::setWeight: Weight " + std::to_string(weight) + 
                       " is outside valid range [" + std::to_string(Impl::MIN_WEIGHT) + ", " +
                       std::to_string(Impl::MAX_WEIGHT) + "]");
    }
    pImpl->weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::addToWeight: Attempting to access null implementation");
        return;
    }
    // Validate delta is reasonable
    if (std::abs(delta) > 10.0f) {  // Prevent unrealistic weight changes
        NLM_LOG_WARNING("Synapse::addToWeight: Delta " + std::to_string(delta) + 
                       " is outside reasonable range");
    }
    
    float newWeight = pImpl->weight + delta;
    if (newWeight < Impl::MIN_WEIGHT - 0.001f || newWeight > Impl::MAX_WEIGHT + 0.001f) {
        NLM_LOG_WARNING("Synapse::addToWeight: Weight change would result in unstable value " + 
                       std::to_string(newWeight) + ". Clamping to bounds.");
    }
    
    pImpl->weight = std::clamp(newWeight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::setDelay: Attempting to access null implementation");
        return;
    }
    // Validate delay is within reasonable bounds
    if (delay < 0 || delay > 1000) {
        NLM_LOG_WARNING("Synapse::setDelay: Delay " + std::to_string(delay) + 
                       " is outside reasonable range [0, 1000] steps");
    }
    pImpl->delay = delay;
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::setType: Attempting to access null implementation");
        return;
    }
    pImpl->type = type;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::recordPreSpike: Attempting to access null implementation");
        return;
    }
    if (timestamp < 0.0f) {
        NLM_LOG_WARNING("Synapse::recordPreSpike: Negative timestamp " + std::to_string(timestamp));
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        NLM_LOG_WARNING("Synapse::recordPreSpike: Spike history exceeds maximum capacity, trimming");
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::recordPostSpike: Attempting to access null implementation");
        return;
    }
    if (timestamp < 0.0f) {
        NLM_LOG_WARNING("Synapse::recordPostSpike: Negative timestamp " + std::to_string(timestamp));
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        NLM_LOG_WARNING("Synapse::recordPostSpike: Spike history exceeds maximum capacity, trimming");
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

void Synapse::clearHistory() {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::clearHistory: Attempting to access null implementation");
        return;
    }
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse::decayEligibilityTrace: Attempting to access null implementation");
        return;
    }
    if (decayRate < 0.0f || decayRate > 1.0f) {
        NLM_LOG_WARNING("Synapse::decayEligibilityTrace: Decay rate " + std::to_string(decayRate) + 
                       " is outside valid range [0, 1]");
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
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
