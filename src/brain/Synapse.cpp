// Enhanced Synapse implementation with improved error handling and memory management
#include "Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;
    SynapseType type;
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    float eligibilityTrace;
    float efficacy;
    PlasticityFlags plasticityFlags;
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), 
             delay(10), type(SynapseType::Excitatory),
             eligibilityTrace(0.0f), efficacy(1.0f) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination) : pImpl(new Impl()) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate synapse memory");
    }
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    // Initialize with biological values
    pImpl->weight = 0.5f;  // Typical initial weight
    pImpl->delay = 10;     // 10ms delay
    pImpl->type = SynapseType::Excitatory;
}

Synapse::~Synapse() {
    delete pImpl;
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
    if (!pImpl) {
        return SynapseId();
    }
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) {
        return NeuronId();
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) {
        return NeuronId();
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) {
        return 0.0f;
    }
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (!pImpl) {
        return;
    }
    // Validate weight range
    if (weight < -1.0f || weight > 1.0f) {
        return; // Ignore invalid weight
    }
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) {
        return;
    }
    // Validate weight change
    if (std::isnan(delta) || std::isinf(delta)) {
        return;
    }
    float newWeight = pImpl->weight + delta;
    // Clamp weight to valid range
    if (newWeight < -1.0f) {
        newWeight = -1.0f;
    } else if (newWeight > 1.0f) {
        newWeight = 1.0f;
    }
    pImpl->weight = newWeight;
}

Delay Synapse::getDelay() const {
    if (!pImpl) {
        return 0;
    }
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) {
        return;
    }
    // Validate delay
    if (delay < 0 || delay > 1000) {
        return; // Ignore invalid delay
    }
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    if (!pImpl) {
        return SynapseType::Excitatory;
    }
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) {
        return;
    }
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    if (!pImpl) {
        return true;
    }
    return pImpl->type == SynapseType::Excitatory || 
           pImpl->type == SynapseType::Modulatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) {
        return false;
    }
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) {
        return;
    }
    // Validate timestamp
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        return;
    }
    pImpl->preSpikeHistory.push_back(timestamp);
    // Limit history size to prevent memory issues
    if (pImpl->preSpikeHistory.size() > 100) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) {
        return;
    }
    // Validate timestamp
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        return;
    }
    pImpl->postSpikeHistory.push_back(timestamp);
    // Limit history size to prevent memory issues
    if (pImpl->postSpikeHistory.size() > 100) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    if (!pImpl) {
        static const std::vector<Timestamp> emptyVector;
        return emptyVector;
    }
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    if (!pImpl) {
        static const std::vector<Timestamp> emptyVector;
        return emptyVector;
    }
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    if (!pImpl) {
        return;
    }
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    if (!pImpl) {
        static const PlasticityFlags emptyFlags;
        return emptyFlags;
    }
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    if (!pImpl) {
        throw std::runtime_error("Synapse implementation is null");
    }
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        return;
    }
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    if (!pImpl) {
        return 0.0f;
    }
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (!pImpl) {
        return;
    }
    if (std::isnan(trace) || std::isinf(trace)) {
        return;
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) {
        return;
    }
    if (decayRate <= 0 || decayRate >= 1) {
        return; // Invalid decay rate
    }
    pImpl->eligibilityTrace *= (1.0f - decayRate);
}

float Synapse::getEfficacy() const {
    if (!pImpl) {
        return 1.0f;
    }
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (!pImpl) {
        return;
    }
    if (std::isnan(efficacy) || std::isinf(efficacy)) {
        return;
    }
    pImpl->efficacy = efficacy;
}

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) {
        return;
    }
    // Decay eligibility trace
    if (pImpl->eligibilityTrace > 0.001f) {
        pImpl->eligibilityTrace *= 0.95f;
    }
}

void Synapse::reset() {
    if (!pImpl) {
        return;
    }
    pImpl->weight = 0.5f;
    pImpl->delay = 10;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->plasticityFlags = PlasticityFlags();
}

void Synapse::initializeRandom(class RandomGenerator& rng) {
    if (!pImpl) {
        return;
    }
    // Initialize with biological parameters
    pImpl->weight = rng.uniformReal(-1.0f, 1.0f);
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 50));
    pImpl->efficacy = 1.0f;
}

} // namespace nlm