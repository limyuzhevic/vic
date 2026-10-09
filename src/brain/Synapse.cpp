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
    
    // Spike history managers
    NeuronSpikeHistoryManager spikeHistoryManager;
    SynapseEfficacyManager efficacyManager;
    SynapseEligibilityTraceManager eligibilityTraceManager;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
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
    pImpl->eligibilityTraceManager.setEligibilityTrace(0.0f);
    pImpl->efficacyManager.setEfficacy(1.0f);
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
    pImpl->spikeHistoryManager.recordPreSpike(timestamp);
    pImpl->efficacyManager.setLastSpikeTimes(timestamp, pImpl->efficacyManager.getLastPostSpikeTime());
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->spikeHistoryManager.recordPostSpike(timestamp);
    pImpl->efficacyManager.setLastSpikeTimes(pImpl->efficacyManager.getLastPreSpikeTime(), timestamp);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->spikeHistoryManager.getPreSpikeHistory();
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->spikeHistoryManager.getPostSpikeHistory();
}

void Synapse::clearHistory() {
    pImpl->spikeHistoryManager.clearHistory();
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTraceManager.getEligibilityTrace();
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTraceManager.setEligibilityTrace(trace);
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTraceManager.decayEligibilityTrace(decayRate);
}

float Synapse::getEfficacy() const {
    return pImpl->efficacyManager.getEfficacy();
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacyManager.setEfficacy(efficacy);
}

void Synapse::step(Timestamp currentTime) {
    // Update efficacy based on recent spike activity using new base class
    pImpl->efficacyManager.updateEfficacy(
        pImpl->efficacyManager.getLastPreSpikeTime(),
        pImpl->efficacyManager.getLastPostSpikeTime(),
        currentTime
    );
    
    // Decay eligibility trace using new base class
    pImpl->eligibilityTraceManager.decayEligibilityTrace(0.001f);
    
    // Clamp weight bounds
    if (pImpl->weight < Impl::MIN_WEIGHT || pImpl->weight > Impl::MAX_WEIGHT) {
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->spikeHistoryManager.clearHistory();
    pImpl->efficacyManager.reset();
    pImpl->eligibilityTraceManager.reset();
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type using new base classes
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Use base class method for setting efficacy
        pImpl->efficacyManager.setEfficacy(rng.uniformReal(0.8f, 1.0f));
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacyManager.setEfficacy(rng.uniformReal(0.8f, 1.0f));
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacyManager.setEfficacy(rng.uniformReal(0.9f, 1.0f));
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->efficacyManager.reset();
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTraceManager.reset();
    
    // Initialize last spike times via efficacy manager
    pImpl->efficacyManager.setLastSpikeTimes(-1.0f, -1.0f);
}

} // namespace nlm
