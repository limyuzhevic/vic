#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

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
    
    // Enhanced short-term plasticity state
    float u;                    // Utilization factor (0-1)
    float x;                    // Resource availability (0-1)
    float lastPreSpikeTime;     // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Enhanced short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;   // ms
    static constexpr float STP_U_MAX = 1.0f;                // Max utilization
    static constexpr float STP_X_MAX = 1.0f;                // Max resource availability
    static constexpr float STP_U_MIN = 0.0f;                // Min utilization
    static constexpr float STP_X_MIN = 0.0f;                // Min resource availability
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // State tracking
    bool isPreSpikeRecorded;
    bool isPostSpikeRecorded;
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), u(0.0f), x(1.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             eligibilityTrace(0.0f), efficacy(1.0f), isPreSpikeRecorded(false), isPostSpikeRecorded(false) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->isPreSpikeRecorded = false;
    pImpl->isPostSpikeRecorded = false;
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(std::exchange(other.pImpl, nullptr)) {}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = std::exchange(other.pImpl, nullptr);
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
    pImpl->weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
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
    pImpl->lastPreSpikeTime = timestamp;
    pImpl->isPreSpikeRecorded = true;
    
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update short-term plasticity for pre-synaptic spike (Tsodyks-Markram model)
    if (pImpl->u < Impl::STP_U_MAX) {
        // Increase utilization
        pImpl->u += (1.0f - pImpl->u) * 0.02f;  // Increment with small factor
    }
    
    // Decrease resource availability (consumption)
    pImpl->x = std::max(0.0f, pImpl->x - pImpl->u);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    pImpl->lastPostSpikeTime = timestamp;
    pImpl->isPostSpikeRecorded = true;
    
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Short-term depression: increase resource availability recovery
    pImpl->x = std::min(Impl::STP_X_MAX, pImpl->x + 0.01f);
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
    pImpl->isPreSpikeRecorded = false;
    pImpl->isPostSpikeRecorded = false;
}

void Synapse::setPreSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->preSpikeHistory = history;
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(
            pImpl->preSpikeHistory.begin(),
            pImpl->preSpikeHistory.begin() +
                static_cast<std::ptrdiff_t>(pImpl->preSpikeHistory.size() -
                                            Impl::MAX_SPIKE_HISTORY));
    }
    if (!history.empty()) {
        pImpl->lastPreSpikeTime = history.back();
        pImpl->isPreSpikeRecorded = true;
    }
}

void Synapse::setPostSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->postSpikeHistory = history;
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(
            pImpl->postSpikeHistory.begin(),
            pImpl->postSpikeHistory.begin() +
                static_cast<std::ptrdiff_t>(pImpl->postSpikeHistory.size() -
                                            Impl::MAX_SPIKE_HISTORY));
    }
    if (!history.empty()) {
        pImpl->lastPostSpikeTime = history.back();
        pImpl->isPostSpikeRecorded = true;
    }
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
    if (decayRate > 0.0f) {
        pImpl->eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    // Enhanced synaptic dynamics:
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    // 2. Decay eligibility trace for reward-modulated learning
    // 3. Update efficacy based on use
    // 4. Apply homeostatic regulation
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term plasticity state (more stable Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = currentTime - pImpl->lastPreSpikeTime;
        pImpl->u = std::max(Impl::STP_U_MIN, 
                           pImpl->u * std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU));
    }
    
    // Recovery of resource availability (facilitation)
    pImpl->x = std::min(Impl::STP_X_MAX, 
                       pImpl->x + (Impl::STP_X_MAX - pImpl->x) * (1.0f - std::exp(-dt / 1.0f)));
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Update synaptic efficacy based on usage patterns
    if (pImpl->isPreSpikeRecorded || pImpl->isPostSpikeRecorded) {
        // Increase efficacy with spike activity (use-dependent modulation)
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy + 0.005f);
    } else {
        // Decay efficacy when not used (homeostatic regulation)
        pImpl->efficacy = std::max(0.8f, pImpl->efficacy * 0.995f);
    }
    
    // Clamp weight bounds with safety margins
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Reset spike recording flags for next step
    pImpl->isPreSpikeRecorded = false;
    pImpl->isPostSpikeRecorded = false;
}

void Synapse::reset() {
    // Reset short-term state for new episode but preserve long-term weight
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;  // Restore baseline efficacy
    pImpl->u = 0.0f;
    pImpl->x = Impl::STP_X_MAX;
    
    // Reset spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->isPreSpikeRecorded = false;
    pImpl->isPostSpikeRecorded = false;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.5f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.2f);
        pImpl->u = rng.uniformReal(0.01f, 0.1f);  // Initial utilization
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.5f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.2f);
        pImpl->u = rng.uniformReal(0.01f, 0.1f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.2f, 0.2f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.1f);
        pImpl->u = rng.uniformReal(0.0f, 0.05f);
    }
    
    // Random delay: 1-10 steps (better biological range)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(uint32_t(1), uint32_t(10)));
    
    // Initialize short-term plasticity state with biological values
    pImpl->x = rng.uniformReal(0.5f, 1.0f);  // Resource availability
    pImpl->u = rng.uniformReal(0.0f, 0.15f); // Initial utilization
    
    // Initialize eligibility trace
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->isPreSpikeRecorded = false;
    pImpl->isPostSpikeRecorded = false;
}

// Additional optimized methods for better performance and integration

float Synapse::calculateSTDP(const Timestamp& preTime, const Timestamp& postTime,
                            float learningRate) const {
    // Calculate STDP contribution with enhanced biophysics
    if (preTime < 0 || postTime < 0) return 0.0f;
    
    float deltaT = postTime - preTime;
    
    if (deltaT > 0) {
        // Post before pre: Long-term depression
        return -learningRate * std::exp(-deltaT / 20.0f);
    } else {
        // Pre before post: Long-term potentiation
        return learningRate * std::exp(deltaT / 20.0f);
    }
}

float Synapse::getEffectiveWeight() const {
    // Calculate effective weight considering short-term plasticity
    return pImpl->weight * pImpl->efficacy * pImpl->x * (1.0f - pImpl->u);
}

bool Synapse::isActive() const {
    // Check if synapse is eligible for plasticity
    return (pImpl->plasticityFlags.hebbian || pImpl->plasticityFlags.stdp || 
            pImpl->plasticityFlags.reward_modulated) &&
           (std::abs(pImpl->weight) > 0.01f);
}

void Synapse::applyHomeostaticRegulation(float targetWeight, float learningRate) {
    // Apply homeostatic regulation to maintain target weight
    float delta = targetWeight - pImpl->weight;
    if (std::abs(delta) > 0.001f) {
        pImpl->weight += delta * learningRate * 0.01f;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

void Synapse::batchReset(const std::vector<Synapse*>& synapses) {
    // Efficiently reset multiple synapses
    for (auto* syn : synapses) {
        if (syn) {
            syn->reset();
        }
    }
}

float Synapse::calculateTotalSynapticDrive(const std::vector<Synapse*>& synapses, 
                                          const std::vector<float>& weights) {
    // Calculate total synaptic drive from multiple synapses
    float totalDrive = 0.0f;
    for (size_t i = 0; i < synapses.size() && i < weights.size(); ++i) {
        if (synapses[i] && synapses[i]->isExcitatory()) {
            totalDrive += synapses[i]->getWeight() * weights[i];
        } else if (synapses[i] && synapses[i]->isInhibitory()) {
            totalDrive -= synapses[i]->getWeight() * weights[i];
        }
    }
    return totalDrive;
}

} // namespace nlm

