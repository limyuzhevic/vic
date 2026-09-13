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

void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001f;  // 1ms timestep
    
    // Apply activity-dependent plasticity based on recent spikes
    if (pImpl->plasticityFlags.stdp && !pImpl->preSpikeHistory.empty() && !pImpl->postSpikeHistory.empty()) {
        // STDP weight updates based on spike timing
        float maxPreTime = *std::max_element(pImpl->preSpikeHistory.begin(), pImpl->preSpikeHistory.end());
        float maxPostTime = *std::max_element(pImpl->postSpikeHistory.begin(), pImpl->postSpikeHistory.end());
        
        float deltaT = maxPostTime - maxPreTime;
        
        // Apply STDP weight change
        if (deltaT > 0.0f) {
            // Post after pre: LTP (long-term potentiation)
            float ltpChange = pImpl->weight * 0.01f * std::exp(-deltaT / 20.0f);  // tau = 20ms
            addToWeight(ltpChange);
        } else {
            // Pre after post: LTD (long-term depression)
            float ltdChange = pImpl->weight * 0.012f * std::exp(deltaT / 20.0f);  // tau = 20ms
            addToWeight(ltdChange);
        }
    }
    
    if (pImpl->plasticityFlags.hebbian && !pImpl->preSpikeHistory.empty() && !pImpl->postSpikeHistory.empty()) {
        // Hebbian learning: co-activation strengthens synapses
        size_t minHistorySize = std::min(pImpl->preSpikeHistory.size(), pImpl->postSpikeHistory.size());
        if (minHistorySize > 0) {
            // Use most recent spike pair
            auto preIt = pImpl->preSpikeHistory.end();
            auto postIt = pImpl->postSpikeHistory.end();
            std::advance(preIt, -1);
            std::advance(postIt, -1);
            
            float deltaT = *postIt - *preIt;
            
            if (std::abs(deltaT) < 10.0f) {  // Co-activation window
                float hebbianChange = 0.005f * (isExcitatory() ? 1.0f : -0.5f);
                addToWeight(hebbianChange);
            }
        }
    }
    
    // Reward-modulated learning
    if (pImpl->plasticityFlags.reward_modulated && pImpl->eligibilityTrace != 0.0f) {
        // Apply reward modulation to eligibility trace
        float rewardFactor = 0.1f;  // Simplified reward factor
        float modulatedUpdate = pImpl->eligibilityTrace * rewardFactor * (isExcitatory() ? 1.0f : 0.5f);
        addToWeight(modulatedUpdate);
        
        // Decay eligibility trace
        decayEligibilityTrace(0.01f);
    }
    
    // Decay eligibility trace for STDP (slow decay)
    decayEligibilityTrace(0.001f);
    
    // Short-term plasticity: Tsodyks-Markram model
    // Update utilization based on presynaptic spike
    float x = pImpl->shortTermDepression;  // Depression variable (fraction of resources available)
    float u = pImpl->shortTermFacilitation;  // Utilization (fraction of available resources used)
    
    // Decay dynamics toward baseline
    const float tau_x = Impl::STP_DEPRESSION_TAU;  // Recovery time constant
    const float tau_u = Impl::STP_FACILITATION_TAU;  // Utilization decay time constant
    
    float timeSincePre = 0.0f;
    float timeSincePost = 0.0f;
    
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
    }
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        timeSincePost = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
    }
    
    // Update depression variable (recovery from depression)
    float recoveryRate = (1.0f - x) * (1.0f - std::exp(-timeSincePre / tau_x));
    pImpl->shortTermDepression = x + recoveryRate * (1.0f - x);
    
    // Update utilization (facilitation)
    u *= std::exp(-timeSincePre / tau_u);
    pImpl->shortTermFacilitation = u;
    
    // Apply efficacy modulation based on use history
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    
    // Apply weight bounds with C++20 clamp for stability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Add small decay term for weight homeostasis
    const float weightDecayRate = 0.00001f;  // 0.001% per ms
    if (pImpl->weight > 0.0f) {
        pImpl->weight *= (1.0f - weightDecayRate * static_cast<float>(dt));
    } else if (pImpl->weight < 0.0f) {
        pImpl->weight *= (1.0f + weightDecayRate * static_cast<float>(dt));
    }
    
    // Apply bounds again after decay
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
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
