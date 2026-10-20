#include "SynapseCore.hpp"
#include "SynapseSTDP.hpp"
#include "SynapseHebbian.hpp"
#include "SynapseShortTermPlasticity.hpp"
#include "SynapseEligibility.hpp"

namespace nlm {

// Constructor implementations
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
    // Initialize components
    pImpl->synapseCore = std::make_unique<SynapseCore>(pImpl.get());
    pImpl->synapseSTDP = std::make_unique<SynapseSTDP>(pImpl.get());
    pImpl->synapseHebbian = std::make_unique<SynapseHebbian>(pImpl.get());
    pImpl->synapseShortTermPlasticity = std::make_unique<SynapseShortTermPlasticity>(pImpl.get());
    pImpl->synapseEligibility = std::make_unique<SynapseEligibility>(pImpl.get());
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
    pImpl->synapseCore->setWeight(weight);
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->synapseCore->addToWeight(delta);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->synapseCore->setDelay(delay);
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->synapseCore->setType(type);
}

bool Synapse::isExcitatory() const {
    return pImpl->synapseCore->isExcitatory();
}

bool Synapse::isInhibitory() const {
    return pImpl->synapseCore->isInhibitory();
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->synapseSTDP->recordPreSpike(timestamp);
    pImpl->synapseHebbian->recordPreSpike(timestamp);
    pImpl->synapseEligibility->recordPreSpike(timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->synapseSTDP->recordPostSpike(timestamp);
    pImpl->synapseHebbian->recordPostSpike(timestamp);
    pImpl->synapseEligibility->recordPostSpike(timestamp);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->synapseSTDP->getPreSpikeHistory();
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->synapseSTDP->getPostSpikeHistory();
}

void Synapse::clearHistory() {
    pImpl->synapseSTDP->clearHistory();
    pImpl->synapseHebbian->clearHistory();
    pImpl->synapseEligibility->clearHistory();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->synapseSTDP->enablePlasticity(stdp);
    pImpl->synapseHebbian->enablePlasticity(hebbian);
    pImpl->synapseEligibility->enablePlasticity(rewardModulated);
}

float Synapse::getEligibilityTrace() const {
    return pImpl->synapseEligibility->getEligibilityTrace();
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->synapseEligibility->setEligibilityTrace(trace);
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->synapseEligibility->decayEligibilityTrace(decayRate);
}

float Synapse::getEfficacy() const {
    return pImpl->synapseCore->getEfficacy();
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->synapseCore->setEfficacy(efficacy);
}

void Synapse::step(Timestamp currentTime) {
    pImpl->synapseShortTermPlasticity->step(currentTime);
    pImpl->synapseEligibility->step(currentTime);
}

void Synapse::reset() {
    pImpl->synapseCore->reset();
    pImpl->synapseSTDP->reset();
    pImpl->synapseHebbian->reset();
    pImpl->synapseShortTermPlasticity->reset();
    pImpl->synapseEligibility->reset();
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    pImpl->synapseCore->initializeRandom(rng);
}

} // namespace nlm

#include "Synapse.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

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

} // namespace nlm