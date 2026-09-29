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
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
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
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

// Advanced querying methods
float Synapse::getAverageFiringRate(float timeWindow) const {
    size_t preCount = getRecentPreSpikeCount(timeWindow);
    size_t postCount = getRecentPostSpikeCount(timeWindow);
    
    if (timeWindow <= 0.0f) return 0.0f;
    
    // Average firing rate (spikes per second) over the time window
    return static_cast<float>(preCount + postCount) / (timeWindow / 1000.0f);
}

bool Synapse::isEligibleForLearning() const {
    // Synapse is eligible for learning if:
    // 1. Plasticity is enabled for any rule
    // 2. Has experienced recent activity (spike within reasonable timeframe)
    // 3. Has non-zero eligibility trace (for reward-modulated learning)
    
    bool plasticityEnabled = pImpl->plasticityFlags.hebbian || 
                           pImpl->plasticityFlags.stdp || 
                           pImpl->plasticityFlags.reward_modulated;
    
    if (!plasticityEnabled) return false;
    
    bool recentActivity = hasRecentActivity(100.0f); // 100ms threshold
    bool hasTrace = std::abs(pImpl->eligibilityTrace) > 0.001f;
    
    return recentActivity || hasTrace;
}

float Synapse::getLearningPotential() const {
    // Combined potential for different learning mechanisms
    float stdpPotential = 0.0f;
    if (pImpl->plasticityFlags.stdp) {
        // STDP potential based on spike timing differences
        if (!pImpl->preSpikeHistory.empty() && !pImpl->postSpikeHistory.empty()) {
            float avgPreTime = 0.0f, avgPostTime = 0.0f;
            for (Timestamp t : pImpl->preSpikeHistory) avgPreTime += static_cast<float>(t);
            for (Timestamp t : pImpl->postSpikeHistory) avgPostTime += static_cast<float>(t);
            avgPreTime /= pImpl->preSpikeHistory.size();
            avgPostTime /= pImpl->postSpikeHistory.size();
            
            float timeDiff = avgPostTime - avgPreTime;
            if (std::abs(timeDiff) < 100.0f) { // Within STDP window
                stdpPotential = std::exp(-std::abs(timeDiff) / 20.0f);
            }
        }
    }
    
    float hebbianPotential = 0.0f;
    if (pImpl->plasticityFlags.hebbian) {
        // Hebbian potential based on coactivity
        hebbianPotential = static_cast<float>(getRecentPreSpikeCount(50.0f) * getRecentPostSpikeCount(50.0f)) / 25.0f;
    }
    
    float rewardPotential = 0.0f;
    if (pImpl->plasticityFlags.reward_modulated) {
        rewardPotential = std::abs(pImpl->eligibilityTrace);
    }
    
    return stdpPotential * 0.4f + hebbianPotential * 0.4f + rewardPotential * 0.2f;
}

bool Synapse::hasRecentActivity(float timeThreshold) const {
    if (timeThreshold <= 0.0f) return true;
    
    float currentTime = static_cast<float>(pImpl->lastPreSpikeTime > 0 ? pImpl->lastPreSpikeTime : pImpl->lastPostSpikeTime);
    if (pImpl->lastPreSpikeTime < 0.0f && pImpl->lastPostSpikeTime < 0.0f) {
        return false;
    }
    
    return (currentTime >= 0.0f && (pImpl->lastPreSpikeTime >= 0.0f || pImpl->lastPostSpikeTime >= 0.0f));
}

// Short-term plasticity state querying
float Synapse::getFacilitationFactor() const {
    return pImpl->shortTermFacilitation;
}

float Synapse::getDepressionFactor() const {
    return pImpl->shortTermDepression;
}

float Synapse::getSynapticStrength() const {
    // Combined strength considering weight, efficacy, and short-term plasticity
    return pImpl->weight * pImpl->efficacy * pImpl->shortTermFacilitation * (2.0f - pImpl->shortTermDepression);
}

// Eligibility trace utilities
bool Synapse::isEligibilityTraceActive() const {
    return std::abs(pImpl->eligibilityTrace) > 0.01f;
}

float Synapse::getNormalizedTrace() const {
    // Normalize by assuming decay rate of 0.001 per timestep
    return pImpl->eligibilityTrace / 0.001f;
}

// Activity analysis
size_t Synapse::getRecentPreSpikeCount(float timeWindow) const {
    if (timeWindow <= 0.0f) return pImpl->preSpikeHistory.size();
    
    size_t count = 0;
    float currentTime = static_cast<float>(pImpl->lastPreSpikeTime);
    
    for (Timestamp t : pImpl->preSpikeHistory) {
        if (currentTime - t <= timeWindow) {
            ++count;
        }
    }
    return count;
}

size_t Synapse::getRecentPostSpikeCount(float timeWindow) const {
    if (timeWindow <= 0.0f) return pImpl->postSpikeHistory.size();
    
    size_t count = 0;
    float currentTime = static_cast<float>(pImpl->lastPostSpikeTime);
    
    for (Timestamp t : pImpl->postSpikeHistory) {
        if (currentTime - t <= timeWindow) {
            ++count;
        }
    }
    return count;
}

float Synapse::getLastPreSpikeInterval() const {
    if (pImpl->preSpikeHistory.size() < 2) return -1.0f;
    
    // Calculate average interval between pre spikes
    float totalInterval = 0.0f;
    size_t count = 0;
    
    for (size_t i = 1; i < pImpl->preSpikeHistory.size(); ++i) {
        totalInterval += static_cast<float>(pImpl->preSpikeHistory[i] - pImpl->preSpikeHistory[i-1]);
        ++count;
    }
    
    return count > 0 ? totalInterval / count : -1.0f;
}

float Synapse::getLastPostSpikeInterval() const {
    if (pImpl->postSpikeHistory.size() < 2) return -1.0f;
    
    // Calculate average interval between post spikes
    float totalInterval = 0.0f;
    size_t count = 0;
    
    for (size_t i = 1; i < pImpl->postSpikeHistory.size(); ++i) {
        totalInterval += static_cast<float>(pImpl->postSpikeHistory[i] - pImpl->postSpikeHistory[i-1]);
        ++count;
    }
    
    return count > 0 ? totalInterval / count : -1.0f;
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
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
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
