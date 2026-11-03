#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;
    SynapseType type;
    PlasticityFlags plasticityFlags;
    float efficacy;  // Synaptic efficacy for use-dependent modulation
    float eligibilityTrace;  // For reward-modulated learning
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Synaptic parameters
    static constexpr SynapticWeight MAX_WEIGHT = 10.0f;
    static constexpr SynapticWeight MIN_WEIGHT = 0.0f;
    static constexpr float MAX_EFFICIENCY = 2.0f;
    static constexpr float MIN_EFFICIENCY = 0.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(1.0f), delay(1),
             type(SynapseType::Excitatory), efficacy(1.0f), eligibilityTrace(0.0f) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
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

void Synapse::setWeight(SynapticWeight weight) {
    if (std::isnan(weight) || std::isinf(weight)) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid weight value: " + std::to_string(weight));
    }
    
    if (weight < Impl::MIN_WEIGHT || weight > Impl::MAX_WEIGHT) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Weight out of valid range: " + std::to_string(weight));
    }
    
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    validateInputParameters(pImpl->weight + delta, "addToWeight");
    setWeight(pImpl->weight + delta);
}

void Synapse::setDelay(Delay delay) {
    if (delay < 0 || delay > 1000) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid delay value: " + std::to_string(delay));
    }
    pImpl->delay = delay;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid timestamp for pre-spike");
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid timestamp for post-spike");
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (decayRate < 0.0f || decayRate > 1.0f) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid decay rate: " + std::to_string(decayRate));
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (pImpl->eligibilityTrace < 0.0f) pImpl->eligibilityTrace = 0.0f;
}

void Synapse::stepProtected(Timestamp currentTime) {
    // Validate current time
    if (std::isnan(currentTime) || std::isinf(currentTime)) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid current time: " + std::to_string(currentTime));
    }
    
    // Validate that we have a valid source and destination
    if (pImpl->sourceNeuron == INVALID_NEURON_ID || 
        pImpl->destinationNeuron == INVALID_NEURON_ID) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid neuron IDs");
    }
    
    // Perform the step
    step(currentTime);
    
    // Validate post-step state
    validate();
}

void Synapse::validate() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer");
    }
    
    // Validate weight
    if (std::isnan(pImpl->weight) || std::isinf(pImpl->weight)) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid weight: " + std::to_string(pImpl->weight));
    }
    
    // Validate efficacy
    if (pImpl->efficacy < Impl::MIN_EFFICIENCY || pImpl->efficacy > Impl::MAX_EFFICIENCY) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid efficacy: " + std::to_string(pImpl->efficacy));
    }
    
    // Validate spike history sizes
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY ||
        pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": Spike history exceeds maximum size");
    }
}

void Synapse::validateInputParameters(SynapticWeight weight, const std::string& operation) const {
    if (std::isnan(weight) || std::isinf(weight)) {
        throw SynapseValidationError("Synapse " + std::to_string(pImpl->id.get()) + 
                                   ": " + operation + " - Invalid weight value: " + std::to_string(weight));
    }
}

void Synapse::reset() {
    if (!pImpl) return;
    
    pImpl->weight = 1.0f;
    pImpl->efficacy = 1.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

void Synapse::initializeRandom(class RandomGenerator& rng) {
    // Initialize with biologically plausible values
    pImpl->weight = rng.uniformReal(Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    pImpl->efficacy = rng.uniformReal(Impl::MIN_EFFICIENCY, Impl::MAX_EFFICIENCY);
    pImpl->delay = static_cast<Delay>(rng.uniformInt(0, 10)); // Short delays typical
    pImpl->type = rng.uniformInt(0, 1) ? SynapseType::Excitatory : SynapseType::Inhibitory;
}

// SynapseId getter implementation
SynapseId Synapse::getId() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getId()");
    }
    return pImpl->id;
}

// Other getters implementation (continuing)
NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getSourceNeuron()");
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getDestinationNeuron()");
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getWeight()");
    }
    return pImpl->weight;
}

Delay Synapse::getDelay() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getDelay()");
    }
    return pImpl->delay;
}

SynapseType Synapse::getType() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getType()");
    }
    return pImpl->type;
}

bool Synapse::isExcitatory() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in isExcitatory()");
    }
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in isInhibitory()");
    }
    return pImpl->type == SynapseType::Inhibitory;
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getPreSpikeHistory()");
    }
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getPostSpikeHistory()");
    }
    return pImpl->postSpikeHistory;
}

void Synapse::setPreSpikeHistory(const std::vector<Timestamp>& history) {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in setPreSpikeHistory()");
    }
    pImpl->preSpikeHistory = history;
}

void Synapse::setPostSpikeHistory(const std::vector<Timestamp>& history) {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in setPostSpikeHistory()");
    }
    pImpl->postSpikeHistory = history;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getPlasticityFlags()");
    }
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getPlasticityFlags() (non-const)");
    }
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getEligibilityTrace()");
    }
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in setEligibilityTrace()");
    }
    if (std::isnan(trace) || std::isinf(trace)) {
        throw SynapseValidationError("Synapse: Invalid eligibility trace value");
    }
    pImpl->eligibilityTrace = trace;
}

float Synapse::getEfficacy() const {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in getEfficacy()");
    }
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (!pImpl) {
        throw SynapseValidationError("Synapse: Null implementation pointer in setEfficacy()");
    }
    if (std::isnan(efficacy) || std::isinf(efficacy)) {
        throw SynapseValidationError("Synapse: Invalid efficacy value");
    }
    if (efficacy < Impl::MIN_EFFICIENCY || efficacy > Impl::MAX_EFFICIENCY) {
        throw SynapseValidationError("Synapse: Efficacy out of valid range");
    }
    pImpl->efficacy = efficacy;
}

} // namespace nlm
