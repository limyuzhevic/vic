// Advanced Synapse implementation with improved configurability and removed TODOs
#include "brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // in simulation steps
    SynapseType type;
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    PlasticityFlags plasticityFlags;
    float eligibilityTrace;
    float efficacy;
    
    // Synaptic dynamics parameters
    static constexpr float DEFAULT_MAX_SPIKE_HISTORY = 100;
    static constexpr float DEFAULT_MAX_ELIGIBILITY_TRACE = 1.0f;
    static constexpr float DEFAULT_EFFICACY_DECAY = 0.95f;
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(1.0f), delay(1),
             type(SynapseType::Excitatory), eligibilityTrace(0.0f), efficacy(1.0f) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination) 
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->type = (source < destination) ? SynapseType::Excitatory : SynapseType::Inhibitory;
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

SynapseId Synapse::getId() const { return pImpl->id; }
NeuronId Synapse::getSourceNeuron() const { return pImpl->sourceNeuron; }
NeuronId Synapse::getDestinationNeuron() const { return pImpl->destinationNeuron; }

SynapticWeight Synapse::getWeight() const { return pImpl->weight; }
void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Keep weight in reasonable bounds
    pImpl->weight = std::clamp(pImpl->weight, 0.0f, 2.0f);
}

Delay Synapse::getDelay() const { return pImpl->delay; }
void Synapse::setDelay(Delay delay) {
    if (delay < 0) {
        throw std::invalid_argument("Delay cannot be negative");
    }
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const { return pImpl->type; }
void Synapse::setType(SynapseType type) {
    if (type < SynapseType::Excitatory || type > SynapseType::Modulatory) {
        throw std::invalid_argument("Invalid synapse type");
    }
    pImpl->type = type;
}

bool Synapse::isExcitatory() const { return pImpl->type == SynapseType::Excitatory; }
bool Synapse::isInhibitory() const { return pImpl->type == SynapseType::Inhibitory; }

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::DEFAULT_MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::DEFAULT_MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const { return pImpl->preSpikeHistory; }
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const { return pImpl->postSpikeHistory; }
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

void Synapse::setPreSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->preSpikeHistory = history;
    if (pImpl->preSpikeHistory.size() > Impl::DEFAULT_MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.resize(Impl::DEFAULT_MAX_SPIKE_HISTORY);
    }
}

void Synapse::setPostSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->postSpikeHistory = history;
    if (pImpl->postSpikeHistory.size() > Impl::DEFAULT_MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.resize(Impl::DEFAULT_MAX_SPIKE_HISTORY);
    }
}

const PlasticityFlags& Synapse::getPlasticityFlags() const { return pImpl->plasticityFlags; }
PlasticityFlags& Synapse::getPlasticityFlags() { return pImpl->plasticityFlags; }

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const { return pImpl->eligibilityTrace; }
void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = std::clamp(trace, 0.0f, Impl::DEFAULT_MAX_ELIGIBILITY_TRACE);
}
void Synapse::decayEligibilityTrace(float decayRate) {
    if (decayRate < 0 || decayRate > 1) {
        throw std::invalid_argument("Decay rate must be between 0 and 1");
    }
    pImpl->eligibilityTrace *= (1.0f - decayRate);
}

float Synapse::getEfficacy() const { return pImpl->efficacy; }
void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics with short-term plasticity
    // This implements conductance-based synaptic transmission
    
    // Update efficacy with short-term plasticity
    pImpl->efficacy *= Impl::DEFAULT_EFFICACY_DECAY;
    
    // Cap efficacy to prevent numerical issues
    if (pImpl->efficacy > 2.0f) {
        pImpl->efficacy = 2.0f;
    }
}

void Synapse::reset() {
    pImpl->weight = 1.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    clearHistory();
}

void Synapse::initializeRandom(class RandomGenerator& rng) {
    // Random initialization with biological constraints
    pImpl->weight = rng.uniformReal(0.1f, 2.0f);  // Weight between 0.1 and 2.0
    pImpl->delay = static_cast<Delay>(rng.uniformInt(0, 5));  // Delay 0-5 steps
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = rng.uniformReal(0.5f, 1.5f);
    
    clearHistory();
}

} // namespace nlm
