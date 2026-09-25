#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>

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
    float eligibilityIncrement;  // Increment per STDP event
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    float useHistory;  // Accumulated usage for efficacy modulation
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // STDP parameters
    static constexpr float STDP_LTP_WEIGHT = 0.01f;  // Long-term potentiation
    static constexpr float STDP_LTD_WEIGHT = -0.012f;  // Long-term depression
    static constexpr float STDP_TIME_CONSTANT = 20.0f;  // STDP time window (ms)
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
    pImpl->eligibilityIncrement = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->useHistory = 0.0f;
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
    if (pImpl) {
        pImpl->weight = weight;
        // Ensure weight stays within biological bounds
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) return;
    
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl ? pImpl->delay : 0;
}

void Synapse::setDelay(Delay delay) {
    if (pImpl) {
        pImpl->delay = std::max(1, delay);  // Minimum delay of 1 step
    }
}

SynapseType Synapse::getType() const {
    return pImpl ? pImpl->type : SynapseType::Excitatory;
}

void Synapse::setType(SynapseType type) {
    if (pImpl) {
        pImpl->type = type;
    }
}

bool Synapse::isExcitatory() const {
    return pImpl && pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl && pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) return;
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update last spike time for short-term plasticity
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
    
    // Apply STDP for pre-synaptic spike
    if (pImpl->plasticityFlags.stdp) {
        applySTDP(timestamp, true);
    }
    
    // Update eligibility trace for Hebbian learning
    if (pImpl->plasticityFlags.hebbian) {
        pImpl->eligibilityIncrement += 0.1f;  // Hebbian increment
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) return;
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update last spike time for short-term plasticity
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
    
    // Apply STDP for post-synaptic spike
    if (pImpl->plasticityFlags.stdp) {
        applySTDP(timestamp, false);
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    static const std::vector<Timestamp> empty;
    return pImpl ? pImpl->preSpikeHistory : empty;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    static const std::vector<Timestamp> empty;
    return pImpl ? pImpl->postSpikeHistory : empty;
}

void Synapse::clearHistory() {
    if (pImpl) {
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->eligibilityTrace = 0.0f;
        pImpl->eligibilityIncrement = 0.0f;
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    static PlasticityFlags empty;
    return pImpl ? pImpl->plasticityFlags : empty;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    }
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    static const PlasticityFlags empty;
    return pImpl ? pImpl->plasticityFlags : empty;
}

float Synapse::getEligibilityTrace() const {
    return pImpl ? pImpl->eligibilityTrace : 0.0f;
}

void Synapse::setEligibilityTrace(float trace) {
    if (pImpl) {
        pImpl->eligibilityTrace = std::clamp(trace, 0.0f, 10.0f);
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) return;
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl ? pImpl->efficacy : 1.0f;
}

void Synapse::setEfficacy(float efficacy) {
    if (pImpl) {
        pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    }
}

// STDP implementation: Spike-Timing-Dependent Plasticity
void Synapse::applySTDP(Timestamp spikeTime, bool isPreSynaptic) {
    if (!pImpl || !pImpl->plasticityFlags.stdp) return;
    
    // Get opposite side spike history
    const std::vector<Timestamp>& oppositeHistory = isPreSynaptic ? 
        pImpl->postSpikeHistory : pImpl->preSpikeHistory;
    
    if (oppositeHistory.empty()) return;
    
    // Find the most recent opposite spike
    float latestOppositeTime = static_cast<float>(oppositeHistory.back());
    
    // Calculate time difference (pre - post)
    double timeDiff = (isPreSynaptic ? 
        (static_cast<double>(spikeTime) - latestOppositeTime) : 
        (latestOppositeTime - static_cast<double>(spikeTime)));
    
    // Apply STDP rules
    if (timeDiff > 0 && timeDiff < Impl::STDP_TIME_CONSTANT) {
        // Pre before post: Long-Term Potentiation (LTP)
        float weightChange = Impl::STDP_LTP_WEIGHT * static_cast<float>(std::exp(-timeDiff / 10.0f));
        addToWeight(weightChange);
        
        // Update eligibility trace
        pImpl->eligibilityTrace += weightChange;
    } else if (timeDiff < 0 && timeDiff > -Impl::STDP_TIME_CONSTANT) {
        // Post before pre: Long-Term Depression (LTD)
        float weightChange = Impl::STDP_LTD_WEIGHT * static_cast<float>(std::exp(timeDiff / 10.0f));
        addToWeight(weightChange);
        
        // Update eligibility trace (negative contribution)
        pImpl->eligibilityTrace += weightChange;
    }
}

// Update synapse dynamics for one simulation step
void Synapse::step(Timestamp currentTime) {
    if (!pImpl) return;
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Decay short-term plasticity state (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // 2. Apply short-term plasticity to compute synaptic efficacy
    float utilization = pImpl->shortTermFacilitation * pImpl->shortTermDepression;
    utilization = std::clamp(utilization, 0.0f, Impl::STP_U_MAX);
    
    // Update synaptic efficacy based on utilization
    pImpl->efficacy = 1.0f + (utilization - 1.0f) * 0.5f;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.1f, 2.0f);
    
    // 3. Update use history for long-term efficacy modulation
    pImpl->useHistory += utilization * dt;
    
    // 4. Apply eligibility trace decay
    decayEligibilityTrace(0.001f);  // Fast decay for reward-modulated learning
    
    // 5. Apply synaptic weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // 6. Update eligibility increment
    if (pImpl->eligibilityIncrement > 0.0f) {
        pImpl->eligibilityTrace += pImpl->eligibilityIncrement * dt;
        pImpl->eligibilityIncrement *= 0.9f; // Decay increment
    }
}

void Synapse::reset() {
    if (!pImpl) return;
    
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->eligibilityIncrement = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->useHistory = 0.0f;
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
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    pImpl->eligibilityIncrement = 0.0f;
    
    // Initialize use history
    pImpl->useHistory = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
