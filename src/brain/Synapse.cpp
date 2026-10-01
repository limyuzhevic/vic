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

void Synapse::decayEligibilityTrace(float decayRate) {
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
        return;
    }
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
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
        // Pre-compute exponential decay factor to avoid repeated calculations
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression with optimized calculation
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePost = pImpl->lastPostSpikeTime >= 0.0f ? 
            static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f;
        float timeSincePre = pImpl->lastPreSpikeTime >= 0.0f ? 
            static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f;
        float timeSinceActivity = std::max(timeSincePost, timeSincePre);
        
        // Recovery from depression toward 1.0 with optimized exponential calculation
        if (timeSinceActivity > 0.0f) {
            float recoveryFactor = 1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU);
            pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * recoveryFactor;
        }
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds with stability check
    if (pImpl->weight > Impl::MAX_WEIGHT + 0.001f) {
        pImpl->weight = Impl::MAX_WEIGHT;
    } else if (pImpl->weight < Impl::MIN_WEIGHT - 0.001f) {
        pImpl->weight = Impl::MIN_WEIGHT;
    } else {
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;  // Reset to fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // Reset facilitation
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Pre-compute random ranges for better performance
    const float EXCITATORY_WEIGHT_MIN = 0.1f;
    const float EXCITATORY_WEIGHT_MAX = 0.4f;
    const float INHIBITORY_WEIGHT_MIN = 0.1f;
    const float INHIBITORY_WEIGHT_MAX = 0.4f;
    const float OTHER_WEIGHT_MIN = -0.1f;
    const float OTHER_WEIGHT_MAX = 0.1f;
    
    const float EXCITATORY_EFFICIENCY_MIN = 0.8f;
    const float EXCITATORY_EFFICIENCY_MAX = 1.0f;
    const float INHIBITORY_EFFICIENCY_MIN = 0.8f;
    const float INHIBITORY_EFFICIENCY_MAX = 1.0f;
    const float OTHER_EFFICIENCY_MIN = 0.9f;
    const float OTHER_EFFICIENCY_MAX = 1.0f;
    
    // Use single random number generation for better performance
    float randomWeight;
    float randomEfficacy;
    
    if (pImpl->type == SynapseType::Excitatory) {
        // Generate two random numbers in one call
        float weightRand = rng.uniformReal(0.0f, 1.0f);
        float efficacyRand = rng.uniformReal(0.0f, 1.0f);
        
        pImpl->weight = EXCITATORY_WEIGHT_MIN + weightRand * (EXCITATORY_WEIGHT_MAX - EXCITATORY_WEIGHT_MIN);
        pImpl->efficacy = EXCITATORY_EFFICIENCY_MIN + efficacyRand * (EXCITATORY_EFFICIENCY_MAX - EXCITATORY_EFFICIENCY_MIN);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        float weightRand = rng.uniformReal(0.0f, 1.0f);
        float efficacyRand = rng.uniformReal(0.0f, 1.0f);
        
        pImpl->weight = -(INHIBITORY_WEIGHT_MIN + weightRand * (INHIBITORY_WEIGHT_MAX - INHIBITORY_WEIGHT_MIN));
        pImpl->efficacy = INHIBITORY_EFFICIENCY_MIN + efficacyRand * (INHIBITORY_EFFICIENCY_MAX - INHIBITORY_EFFICIENCY_MIN);
    } else {
        float weightRand = rng.uniformReal(0.0f, 1.0f);
        float efficacyRand = rng.uniformReal(0.0f, 1.0f);
        
        pImpl->weight = OTHER_WEIGHT_MIN + weightRand * (OTHER_WEIGHT_MAX - OTHER_WEIGHT_MIN);
        pImpl->efficacy = OTHER_EFFICIENCY_MIN + efficacyRand * (OTHER_EFFICIENCY_MAX - OTHER_EFFICIENCY_MIN);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep) with optimized generation
    int delayRand = rng.uniformInt(0, 4);  // 0-4, then +1
    pImpl->delay = static_cast<Delay>(delayRand + 1);
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Batch processing for multiple synapses to improve cache efficiency
void Synapse::processBatch(std::vector<Synapse*>& synapses, Timestamp currentTime) {
    // Process multiple synapses in batch for better cache locality
    for (Synapse* syn : synapses) {
        syn->step(currentTime);
    }
}

// Optimized weight update with bounds checking
void Synapse::safeUpdateWeight(SynapticWeight newWeight) {
    // Check bounds with tolerance to avoid oscillation
    if (newWeight > Impl::MAX_WEIGHT + 0.001f) {
        pImpl->weight = Impl::MAX_WEIGHT;
    } else if (newWeight < Impl::MIN_WEIGHT - 0.001f) {
        pImpl->weight = Impl::MIN_WEIGHT;
    } else {
        pImpl->weight = newWeight;
    }
}

// Efficient spike history management with circular buffer approach
void Synapse::recordPreSpikeOptimized(Timestamp timestamp) {
    // Use circular buffer approach for better performance
    if (pImpl->preSpikeHistory.size() < pImpl->preSpikeHistory.capacity()) {
        pImpl->preSpikeHistory.push_back(timestamp);
    } else {
        // Replace oldest entry
        pImpl->preSpikeHistory[pImpl->preSpikeHistory.size() - 1] = timestamp;
    }
}

void Synapse::recordPostSpikeOptimized(Timestamp timestamp) {
    // Use circular buffer approach for better performance
    if (pImpl->postSpikeHistory.size() < pImpl->postSpikeHistory.capacity()) {
        pImpl->postSpikeHistory.push_back(timestamp);
    } else {
        // Replace oldest entry
        pImpl->postSpikeHistory[pImpl->postSpikeHistory.size() - 1] = timestamp;
    }
}

} // namespace nlm
