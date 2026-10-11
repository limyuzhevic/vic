#include "BasePimpl.hpp"
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
    pImpl->get()->id = id;
    pImpl->get()->sourceNeuron = source;
    pImpl->get()->destinationNeuron = destination;
    pImpl->get()->weight = 0.0f;
    pImpl->get()->delay = 1;  // Default: 1 step delay
    pImpl->get()->type = SynapseType::Excitatory;
    pImpl->get()->eligibilityTrace = 0.0f;
    pImpl->get()->efficacy = 1.0f;
    pImpl->get()->shortTermDepression = 1.0f;
    pImpl->get()->shortTermFacilitation = 0.0f;
    pImpl->get()->lastPreSpikeTime = -1.0f;
    pImpl->get()->lastPostSpikeTime = -1.0f;
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
    return pImpl->get()->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->get()->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->get()->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->get()->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->get()->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->get()->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->get()->weight = std::clamp(pImpl->get()->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->get()->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->get()->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl->get()->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->get()->type = type;
}

bool Synapse::isExcitatory() const {
    return pImpl->get()->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->get()->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->get()->preSpikeHistory.push_back(timestamp);
    if (pImpl->get()->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->get()->preSpikeHistory.erase(pImpl->get()->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->get()->postSpikeHistory.push_back(timestamp);
    if (pImpl->get()->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->get()->postSpikeHistory.erase(pImpl->get()->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->get()->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->get()->postSpikeHistory;
}

void Synapse::clearHistory() {
    pImpl->get()->preSpikeHistory.clear();
    pImpl->get()->postSpikeHistory.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->get()->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->get()->plasticityFlags.hebbian = hebbian;
    pImpl->get()->plasticityFlags.stdp = stdp;
    pImpl->get()->plasticityFlags.reward_modulated = rewardModulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->get()->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->get()->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->get()->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->get()->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->get()->eligibilityTrace) < 0.001f) {
        pImpl->get()->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->get()->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->get()->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    Impl& impl = pImpl->get();
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (impl.lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - impl.lastPreSpikeTime);
        impl.shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression - optimized calculation
    if (impl.lastPostSpikeTime >= 0.0f || impl.lastPreSpikeTime >= 0.0f) {
        float timeSincePost = (impl.lastPostSpikeTime >= 0.0f) 
            ? static_cast<float>(currentTime - impl.lastPostSpikeTime) 
            : 0.0f;
        float timeSincePre = (impl.lastPreSpikeTime >= 0.0f) 
            ? static_cast<float>(currentTime - impl.lastPreSpikeTime) 
            : 0.0f;
        float timeSinceActivity = std::max(timeSincePost, timeSincePre);
        impl.shortTermDepression += (1.0f - impl.shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    impl.weight = std::clamp(impl.weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->get()->weight = 0.0f;
    pImpl->get()->preSpikeHistory.clear();
    pImpl->get()->postSpikeHistory.clear();
    pImpl->get()->eligibilityTrace = 0.0f;
    pImpl->get()->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    Impl& impl = pImpl->get();
    
    // Proper random initialization based on synapse type
    if (impl.type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        impl.weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        impl.efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (impl.type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        impl.weight = -rng.uniformReal(0.1f, 0.4f);
        impl.efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        impl.weight = rng.uniformReal(-0.1f, 0.1f);
        impl.efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    impl.delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    impl.shortTermDepression = 1.0f;  // Fully recovered
    impl.shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    impl.eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    impl.lastPreSpikeTime = -1.0f;
    impl.lastPostSpikeTime = -1.0f;
}

} // namespace nlm
