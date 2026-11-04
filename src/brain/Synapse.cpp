// Enhanced Synapse Implementation with Advanced Synaptic Dynamics
#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

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
    
    // Short-term plasticity state (Tsodyks-Markram model)
    float u;                    // Utilization factor (0-1)
    float x;                    // Resource availability (0-1)
    float y;                    // Recovery from depression (0-1)
    float lastPreSpikeTime;     // For short-term dynamics
    float lastPostSpikeTime;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters (physiologically plausible)
    static constexpr float TAU_F = 5.0f;    // Facilitation time constant (ms)
    static constexpr float TAU_D = 50.0f;   // Depression time constant (ms)
    static constexpr float U_MAX = 0.95f;    // Maximum utilization
    static constexpr float X_0 = 0.95f;     // Baseline resource availability
    
    // Eligibility trace parameters
    static constexpr float ELIGIBILITY_DECAY_RATE = 0.001f;  // Fast decay per step
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Synaptic dynamics monitoring
    float totalInputHistory;    // Track total input for adaptive learning
    float maxInputObserved;     // Track maximum input for scaling
    uint32_t spikeCount;        // Count of spikes for plasticity modulation
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->efficacy = 1.0f;
    pImpl->u = 0.0f;
    pImpl->x = Impl::X_0;
    pImpl->y = 1.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->totalInputHistory = 0.0f;
    pImpl->maxInputObserved = 0.0f;
    pImpl->spikeCount = 0;
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

// Getter methods (unchanged but kept for clarity)
SynapseId Synapse::getId() const { return pImpl->id; }
NeuronId Synapse::getSourceNeuron() const { return pImpl->sourceNeuron; }
NeuronId Synapse::getDestinationNeuron() const { return pImpl->destinationNeuron; }
SynapticWeight Synapse::getWeight() const { return pImpl->weight; }
void Synapse::setWeight(SynapticWeight weight) { pImpl->weight = weight; }
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}
Delay Synapse::getDelay() const { return pImpl->delay; }
void Synapse::setDelay(Delay delay) { pImpl->delay = delay; }
SynapseType Synapse::getType() const { return pImpl->type; }
void Synapse::setType(SynapseType type) { pImpl->type = type; }
bool Synapse::isExcitatory() const { return pImpl->type == SynapseType::Excitatory; }
bool Synapse::isInhibitory() const { return pImpl->type == SynapseType::Inhibitory; }

// Spike recording with enhanced tracking
void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update last pre spike time and short-term plasticity
    pImpl->lastPreSpikeTime = timestamp;
    pImpl->spikeCount++;
    
    // Update short-term facilitation (Tsodyks-Markram model)
    pImpl->u = pImpl->u * (1.0f - 1.0f/Impl::TAU_F) + Impl::U_MAX * (1.0f - pImpl->u);
    pImpl->u = std::min(pImpl->u, Impl::U_MAX);
    
    // Apply depression due to previous post spikes
    pImpl->x = pImpl->x * (1.0f - 1.0f/Impl::TAU_D) + pImpl->y;
    pImpl->x = std::clamp(pImpl->x, 0.0f, 1.0f);
    
    // Track usage for adaptive learning
    pImpl->totalInputHistory += pImpl->weight * pImpl->u * pImpl->x;
    pImpl->maxInputObserved = std::max(pImpl->maxInputObserved, std::abs(pImpl->weight * pImpl->u * pImpl->x));
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    pImpl->lastPostSpikeTime = timestamp;
    
    // Strong depression for post spikes (STDP mechanism)
    pImpl->y = pImpl->y * (1.0f - 1.0f/Impl::TAU_D) + (1.0f - pImpl->y) * 0.1f;
    pImpl->y = std::clamp(pImpl->y, 0.0f, 1.0f);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const { return pImpl->preSpikeHistory; }
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const { return pImpl->postSpikeHistory; }
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::setPreSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->preSpikeHistory = history;
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        auto new_size = Impl::MAX_SPIKE_HISTORY;
        pImpl->preSpikeHistory.erase(
            pImpl->preSpikeHistory.begin(),
            pImpl->preSpikeHistory.begin() + static_cast<std::ptrdiff_t>(pImpl->preSpikeHistory.size() - new_size));
    }
    // Update last spike time
    if (!pImpl->preSpikeHistory.empty()) {
        pImpl->lastPreSpikeTime = pImpl->preSpikeHistory.back();
    }
}

void Synapse::setPostSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->postSpikeHistory = history;
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        auto new_size = Impl::MAX_SPIKE_HISTORY;
        pImpl->postSpikeHistory.erase(
            pImpl->postSpikeHistory.begin(),
            pImpl->postSpikeHistory.begin() + static_cast<std::ptrdiff_t>(pImpl->postSpikeHistory.size() - new_size));
    }
    // Update last spike time
    if (!pImpl->postSpikeHistory.empty()) {
        pImpl->lastPostSpikeTime = pImpl->postSpikeHistory.back();
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() { return pImpl->plasticityFlags; }
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}
const PlasticityFlags& Synapse::getPlasticityFlags() const { return pImpl->plasticityFlags; }

float Synapse::getEligibilityTrace() const { return pImpl->eligibilityTrace; }
void Synapse::setEligibilityTrace(float trace) { pImpl->eligibilityTrace = trace; }
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const { return pImpl->efficacy; }
void Synapse::setEfficacy(float efficacy) { pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f); }

// Enhanced synaptic dynamics with real physiological models
void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(Impl::ELIGIBILITY_DECAY_RATE);
    
    // Update short-term plasticity state (Tsodyks-Markram model)
    // Adaptation variable tracks usage for long-term plasticity modulation
    float adaptation = pImpl->u * pImpl->x;  // Current synaptic efficacy
    pImpl->totalInputHistory += adaptation;
    pImpl->maxInputObserved = std::max(pImpl->maxInputObserved, adaptation);
    
    // Normalize adaptation based on observed maximum (adaptive scaling)
    if (pImpl->maxInputObserved > 0.0f) {
        adaptation /= pImpl->maxInputObserved;
    }
    
    // Update efficacy based on usage (metaplasticity)
    pImpl->efficacy = std::clamp(pImpl->efficacy * (1.0f + adaptation * 0.05f), 0.5f, 2.0f);
    
    // Clamp weight bounds with safety margins
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT + 0.01f, Impl::MAX_WEIGHT - 0.01f);
    
    // Add small noise for biological realism (0.1% of weight range)
    float noise = (static_cast<float>(rng.randomBit()) / static_cast<float>(rng.max())) * 0.02f - 0.01f;
    pImpl->weight += noise;
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    // Reset all state except weight (long-term connectivity)
    clearHistory();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->u = 0.0f;
    pImpl->x = Impl::X_0;
    pImpl->y = 1.0f;
    pImpl->totalInputHistory = 0.0f;
    pImpl->maxInputObserved = 0.0f;
    pImpl->spikeCount = 0;
    
    // Reset last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Enhanced random initialization with physiological realism
void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type and biological constraints
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights with biological constraints
        pImpl->weight = rng.uniformReal(0.05f, 0.3f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.7f, 1.0f);
        pImpl->u = rng.uniformReal(0.3f, 0.6f);  // Moderate initial utilization
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights for balance
        pImpl->weight = -rng.uniformReal(0.05f, 0.3f);
        pImpl->efficacy = rng.uniformReal(0.7f, 1.0f);
        pImpl->u = rng.uniformReal(0.3f, 0.6f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.05f, 0.05f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
        pImpl->u = rng.uniformReal(0.1f, 0.3f);
    }
    
    // Random delay: 1-3 steps (more realistic for biological synapses)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(uint32_t(1), uint32_t(3)));
    
    // Initialize short-term plasticity state with realistic starting values
    pImpl->x = Impl::X_0;  // High baseline resource availability
    pImpl->y = 1.0f;       // Fully recovered from depression
    pImpl->u = rng.uniformReal(0.1f, 0.3f);  // Low initial utilization
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize monitoring variables
    pImpl->totalInputHistory = 0.0f;
    pImpl->maxInputObserved = std::abs(pImpl->weight) * pImpl->u * pImpl->x;
    pImpl->spikeCount = 0;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Additional utility methods for advanced usage
float Synapse::getSynapticStrength() const {
    // Combined measure of synaptic strength considering weight and short-term dynamics
    float shortTermEffect = pImpl->u * pImpl->x;
    return pImpl->weight * shortTermEffect * pImpl->efficacy;
}

bool Synapse::needsStdpUpdate(float currentTime) const {
    // Check if enough time has passed for meaningful STDP updates
    if (pImpl->preSpikeHistory.empty() || pImpl->postSpikeHistory.empty()) {
        return false;
    }
    
    float lastPre = pImpl->preSpikeHistory.back();
    float lastPost = pImpl->postSpikeHistory.back();
    float timeDiff = std::abs(lastPre - lastPost);
    
    return timeDiff > 1.0f;  // Update if spikes are far apart in time
}

float Synapse::getAdaptationLevel() const {
    // Get current adaptation level based on usage history
    if (pImpl->maxInputObserved > 0.0f && pImpl->totalInputHistory > 0.0f) {
        return pImpl->totalInputHistory / (pImpl->spikeCount * pImpl->maxInputObserved);
    }
    return 0.0f;
}

std::map<std::string, float> Synapse::getDiagnosticInfo() const {
    std::map<std::string, float> info;
    info["weight"] = pImpl->weight;
    info["efficacy"] = pImpl->efficacy;
    info["u"] = pImpl->u;
    info["x"] = pImpl->x;
    info["y"] = pImpl->y;
    info["spike_count"] = static_cast<float>(pImpl->spikeCount);
    info["total_input"] = pImpl->totalInputHistory;
    info["max_input"] = pImpl->maxInputObserved;
    info["adaptation"] = getAdaptationLevel();
    return info;
}

} // namespace nlm
