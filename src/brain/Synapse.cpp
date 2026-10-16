#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <numeric>
#include "constants.h"

namespace nlm {

// Synapse implementation

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
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), shortTermDepression(SynapseConstants::INITIAL_DEPRESSION),
             shortTermFacilitation(SynapseConstants::INITIAL_FACILITATION),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             eligibilityTrace(0.0f), efficacy(1.0f) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    if (!id.isValid()) {
        throw std::invalid_argument("Synapse ID must be valid");
    }
    if (!source.isValid() || !destination.isValid()) {
        throw std::invalid_argument("Synapse source and destination neurons must be valid");
    }
    
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = SynapseConstants::MIN_DELAY;  // Default: minimal delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = SynapseConstants::INITIAL_DEPRESSION;
    pImpl->shortTermFacilitation = SynapseConstants::INITIAL_FACILITATION;
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
    if (std::isnan(weight) || std::isinf(weight)) {
        NLM_LOG_WARN("Invalid weight value: " + std::to_string(weight));
        return;
    }
    pImpl->weight = weight;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, SynapseConstants::MIN_WEIGHT, SynapseConstants::MAX_WEIGHT);
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (std::isnan(delta) || std::isinf(delta)) {
        NLM_LOG_WARN("Invalid weight delta: " + std::to_string(delta));
        return;
    }
    
    float newWeight = pImpl->weight + delta;
    if (std::isnan(newWeight) || std::isinf(newWeight)) {
        NLM_LOG_WARN("Weight would become NaN or infinite");
        return;
    }
    
    pImpl->weight = std::clamp(newWeight, SynapseConstants::MIN_WEIGHT, SynapseConstants::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (delay < SynapseConstants::MIN_DELAY || delay > SynapseConstants::MAX_DELAY) {
        NLM_LOG_WARN("Synapse delay " + std::to_string(delay) + " outside bounds [" +
                    std::to_string(SynapseConstants::MIN_DELAY) + "," +
                    std::to_string(SynapseConstants::MAX_DELAY) + "]");
        return;
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
    if (timestamp < 0.0f) {
        NLM_LOG_WARN("Recording pre spike with negative timestamp: " + std::to_string(timestamp));
    }
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > SynapseConstants::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        NLM_LOG_WARN("Recording post spike with negative timestamp: " + std::to_string(timestamp));
    }
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > SynapseConstants::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
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
    if (std::isnan(trace) || std::isinf(trace)) {
        NLM_LOG_WARN("Invalid eligibility trace value: " + std::to_string(trace));
        return;
    }
    pImpl->eligibilityTrace = std::clamp(trace, -SynapseConstants::MAX_ELIGIBILITY_TRACE, 
                                          SynapseConstants::MAX_ELIGIBILITY_TRACE);
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (decayRate < 0.0f || decayRate > 1.0f) {
        NLM_LOG_WARN("Invalid eligibility trace decay rate: " + std::to_string(decayRate));
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
    if (std::isnan(efficacy) || std::isinf(efficacy)) {
        NLM_LOG_WARN("Invalid efficacy value: " + std::to_string(efficacy));
        return;
    }
    pImpl->efficacy = std::clamp(efficacy, SynapseConstants::MIN_EFFICIENCY, SynapseConstants::MAX_EFFICIENCY);
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
        
        if (timeSincePre >= 0.0f) {  // Ensure non-negative time difference
            pImpl->shortTermFacilitation *= std::exp(-timeSincePre / SynapseConstants::STP_FACILITATION_TAU);
        }
    }
    
    // Decay short-term depression and recover toward baseline
    if (pImpl->lastPreSpikeTime >= 0.0f || pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSinceActivity = 0.0f;
        
        if (pImpl->lastPostSpikeTime >= 0.0f && pImpl->lastPreSpikeTime >= 0.0f) {
            timeSinceActivity = std::max(
                static_cast<float>(currentTime - pImpl->lastPostSpikeTime),
                static_cast<float>(currentTime - pImpl->lastPreSpikeTime)
            );
        } else if (pImpl->lastPostSpikeTime >= 0.0f) {
            timeSinceActivity = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        } else if (pImpl->lastPreSpikeTime >= 0.0f) {
            timeSinceActivity = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        }
        
        // Recovery from depression toward 1.0 (full availability)
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
            (1.0f - std::exp(-timeSinceActivity / SynapseConstants::STP_DEPRESSION_TAU));
    }
    
    // Clamp STP values to valid ranges
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, SynapseConstants::STP_U_MAX);
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, SynapseConstants::STP_U_MAX);
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(SynapseConstants::ELIGIBILITY_TRACE_DECAY);
    
    // Clamp weight bounds again (in case external changes occurred)
    pImpl->weight = std::clamp(pImpl->weight, SynapseConstants::MIN_WEIGHT, SynapseConstants::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = SynapseConstants::INITIAL_DEPRESSION;
    pImpl->shortTermFacilitation = SynapseConstants::INITIAL_FACILITATION;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!rng.isSeeded()) {
        NLM_LOG_WARN("Random generator not properly seeded in Synapse::initializeRandom");
    }
    
    // Proper random initialization based on synapse type with biological constraints
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(SynapseConstants::EXCITATORY_WEIGHT_MIN, 
                                       SynapseConstants::EXCITATORY_WEIGHT_MAX);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(SynapseConstants::INHIBITORY_WEIGHT_MIN, 
                                        SynapseConstants::INHIBITORY_WEIGHT_MAX);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(SynapseConstants::OTHER_WEIGHT_MIN, 
                                       SynapseConstants::OTHER_WEIGHT_MAX);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(SynapseConstants::MIN_DELAY, 
                                                    SynapseConstants::MAX_DELAY));
    
    // Initialize short-term plasticity state with recovery from depression
    pImpl->shortTermDepression = SynapseConstants::INITIAL_DEPRESSION;  // Fully recovered
    pImpl->shortTermFacilitation = SynapseConstants::INITIAL_FACILITATION;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm