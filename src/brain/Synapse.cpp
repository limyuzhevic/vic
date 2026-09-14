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
    
    Impl(SynapseId id, NeuronId source, NeuronId dest) 
        : id(id), sourceNeuron(source), destinationNeuron(dest)
        , weight(0.0f), delay(1), type(SynapseType::Excitatory)
        , eligibilityTrace(0.0f), efficacy(1.0f)
        , shortTermDepression(1.0f), shortTermFacilitation(0.0f)
        , lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f) {}
};

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : PImplBase(std::move(other.pImpl)) {}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        PImplBase::operator=(std::move(other));
    }
    return *this;
}

SynapseId Synapse::getId() const {
    return getImpl().id;
}

NeuronId Synapse::getSourceNeuron() const {
    return getImpl().sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return getImpl().destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return getImpl().weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    auto& impl = getImpl();
    impl.weight = weight;
    // Clamp to reasonable bounds to prevent instability
    impl.weight = std::clamp(impl.weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::addToWeight(SynapticWeight delta) {
    auto& impl = getImpl();
    impl.weight += delta;
    impl.weight = std::clamp(impl.weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return getImpl().delay;
}

void Synapse::setDelay(Delay delay) {
    getImpl().delay = delay;
}

SynapseType Synapse::getType() const {
    return getImpl().type;
}

void Synapse::setType(SynapseType type) {
    getImpl().type = type;
}

bool Synapse::isExcitatory() const {
    return getImpl().type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return getImpl().type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    auto& impl = getImpl();
    impl.preSpikeHistory.push_back(timestamp);
    impl.lastPreSpikeTime = static_cast<float>(timestamp);
    if (impl.preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        impl.preSpikeHistory.erase(impl.preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    auto& impl = getImpl();
    impl.postSpikeHistory.push_back(timestamp);
    impl.lastPostSpikeTime = static_cast<float>(timestamp);
    if (impl.postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        impl.postSpikeHistory.erase(impl.postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return getImpl().preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return getImpl().postSpikeHistory;
}

void Synapse::clearHistory() {
    auto& impl = getImpl();
    impl.preSpikeHistory.clear();
    impl.postSpikeHistory.clear();
    impl.lastPreSpikeTime = -1.0f;
    impl.lastPostSpikeTime = -1.0f;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return getImpl().plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return getImpl().plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    auto& impl = getImpl();
    impl.plasticityFlags.hebbian = hebbian;
    impl.plasticityFlags.stdp = stdp;
    impl.plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    return getImpl().eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    getImpl().eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    auto& impl = getImpl();
    impl.eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(impl.eligibilityTrace) < 0.001f) {
        impl.eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return getImpl().efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    getImpl().efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    auto& impl = getImpl();
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (impl.lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - impl.lastPreSpikeTime);
        impl.shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (impl.lastPostSpikeTime >= 0.0f || impl.lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            impl.lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - impl.lastPostSpikeTime) : 0.0f,
            impl.lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - impl.lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        impl.shortTermDepression += (1.0f - impl.shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    impl.weight = std::clamp(impl.weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    auto& impl = getImpl();
    impl.weight = 0.0f;
    impl.preSpikeHistory.clear();
    impl.postSpikeHistory.clear();
    impl.eligibilityTrace = 0.0f;
    impl.efficacy = 1.0f;
    impl.shortTermDepression = 1.0f;
    impl.shortTermFacilitation = 0.0f;
    impl.lastPreSpikeTime = -1.0f;
    impl.lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    auto& impl = getImpl();
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
