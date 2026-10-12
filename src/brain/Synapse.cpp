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
    
    // Short-term plasticity state (Tsodyks-Markram model)
    float utilization;          // Utilization factor (0-1)
    float recovery;             // Recovery rate (1/tau_recovery)
    float facilitation;        // Facilitation rate (1/tau_facilitation)
    float lastPreSpikeTime;    // Time of last pre-synaptic spike
    float lastPostSpikeTime;   // Time of last post-synaptic spike
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Synaptic noise parameters
    float noiseLevel;           // Standard deviation of synaptic noise
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters (ms)
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // Fast facilitation
    static constexpr float STP_DEPRESSION_TAU = 200.0f;   // Slower depression
    static constexpr float STP_RECOVERY_TAU = 150.0f;     // Recovery from depression
    static constexpr float STP_U_MAX = 1.0f;               // Max utilization
    
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
    pImpl->utilization = 1.0f;           // Fully recovered initially
    pImpl->facilitation = 0.0f;         // No initial facilitation
    pImpl->recovery = 1.0f / Impl::STP_RECOVERY_TAU;
    pImpl->noiseLevel = 0.01f;
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
    pImpl->lastPreSpikeTime = timestamp;
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    pImpl->lastPostSpikeTime = timestamp;
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
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
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

float Synapse::getUtilization() const {
    return pImpl->utilization;
}

float Synapse::getFacilitation() const {
    return pImpl->facilitation;
}

float Synapse::getDepression() const {
    // Depression is 1.0 - utilization for Tsodyks-Markram model
    return 1.0f - pImpl->utilization;
}

void Synapse::setUtilization(float utilization) {
    pImpl->utilization = std::clamp(utilization, 0.0f, 1.0f);
}

void Synapse::setFacilitation(float facilitation) {
    pImpl->facilitation = facilitation;
}

void Synapse::setDepression(float depression) {
    // Inverse relationship with utilization
    pImpl->utilization = 1.0f - std::clamp(depression, 0.0f, 1.0f);
}

float Synapse::getSynapticNoiseLevel() const {
    return pImpl->noiseLevel;
}

void Synapse::setSynapticNoiseLevel(float noiseLevel) {
    pImpl->noiseLevel = std::max(0.0f, noiseLevel);
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state according to Tsodyks-Markram model
    // 2. Apply synaptic noise for biological realism
    // 3. Update eligibility trace for plasticity
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term plasticity (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Update utilization based on presynaptic spike history
        if (timeSincePre < Impl::STP_FACILITATION_TAU) {
            // Facilitation: utilization increases after presynaptic spike
            pImpl->utilization = std::min(1.0f, pImpl->utilization + 
                                         (1.0f - pImpl->utilization) * 
                                         (1.0f - std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU)));
        } else {
            // Recovery towards baseline
            pImpl->utilization *= (1.0f - dt / Impl::STP_RECOVERY_TAU);
        }
    }
    
    // Depression based on postsynaptic activity
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePost = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        // Depression: utilization decreases after postsynaptic spike
        pImpl->utilization *= std::exp(-timeSincePost / Impl::STP_DEPRESSION_TAU);
    }
    
    // Apply synaptic noise for biological realism
    if (pImpl->noiseLevel > 0.0f) {
        float noise = pImpl->noiseLevel * (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
        pImpl->utilization += noise;
        pImpl->utilization = std::clamp(pImpl->utilization, 0.0f, 1.0f);
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay with 1ms time constant
    
    // Update efficacy based on use (activity-dependent modulation)
    float activityFactor = pImpl->utilization * pImpl->facilitation;
    pImpl->efficacy = 1.0f + activityFactor * 0.5f;  // Can increase up to 1.5x baseline
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.5f, 2.0f);
    
    // Clamp weight bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::applySpikeInput(Timestamp currentTime, SynapticWeight weight, bool isExcitatory) {
    // Apply synaptic input with realistic dynamics
    // This is called when the synapse receives a spike from presynaptic neuron
    
    // Apply weight with noise for biological realism
    float noisyWeight = weight;
    if (pImpl->noiseLevel > 0.0f) {
        float noise = pImpl->noiseLevel * (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
        noisyWeight += noise;
    }
    
    // Update utilization based on presynaptic spike (short-term plasticity)
    float utilizationIncrease = pImpl->utilization * 0.2f;  // 20% increase per spike
    pImpl->utilization = std::min(1.0f, pImpl->utilization + utilizationIncrease);
    
    // Update eligibility trace for plasticity
    pImpl->eligibilityTrace += 1.0f;  // Increment on spike
    
    // Record the spike
    recordPreSpike(currentTime);
}

void Synapse::calculateSynapticConductance(Timestamp currentTime, float& g_exp, float& g_alpha) {
    // Calculate synaptic conductance using both exponential and alpha models
    float timeSinceSpike = 0.0f;
    
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        timeSinceSpike = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
    }
    
    // Exponential conductance: g(t) = g0 * exp(-t / tau)
    float tau_exp = 5.0f;  // ms, exponential time constant
    g_exp = pImpl->utilization * std::exp(-timeSinceSpike / tau_exp);
    
    // Alpha conductance: g(t) = (g0 / tau_alpha) * t * exp(-t / tau_alpha)
    float tau_alpha = 10.0f;  // ms, alpha time constant
    g_alpha = (pImpl->utilization / tau_alpha) * timeSinceSpike * std::exp(-timeSinceSpike / tau_alpha);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->utilization = 1.0f;     // Fully recovered
    pImpl->facilitation = 0.0f;   // No initial facilitation
    pImpl->noiseLevel = 0.01f;
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
        pImpl->utilization = rng.uniformReal(0.8f, 1.0f);
        pImpl->facilitation = rng.uniformReal(0.0f, 0.2f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        pImpl->utilization = rng.uniformReal(0.8f, 1.0f);
        pImpl->facilitation = rng.uniformReal(0.0f, 0.2f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
        pImpl->utilization = rng.uniformReal(0.7f, 1.0f);
        pImpl->facilitation = rng.uniformReal(0.0f, 0.1f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize noise level based on synapse type
    pImpl->noiseLevel = rng.uniformReal(0.005f, 0.02f);
    
    // Initialize short-term plasticity state
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
