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
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Update short-term plasticity using Tsodyks-Markram model
    // This models both facilitation and depression based on spike history
    
    // Handle pre-synaptic spike (if this is the pre-synaptic neuron)
    if (currentTime == pImpl->lastPreSpikeTime || currentTime == pImpl->lastPostSpikeTime) {
        // Update synaptic efficacy based on spike timing
        if (pImpl->lastPreSpikeTime >= 0.0f) {
            // Facilitation: use current facilitation state to increase efficacy
            float utilization = pImpl->shortTermFacilitation;
            pImpl->efficacy += (1.0f - pImpl->efficacy) * utilization;
            pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
            
            // Depression due to post-synaptic spike
            if (pImpl->lastPostSpikeTime >= 0.0f) {
                float timeSincePost = currentTime - pImpl->lastPostSpikeTime;
                pImpl->shortTermDepression *= std::exp(-timeSincePost / Impl::STP_DEPRESSION_TAU);
            }
        }
    }
    
    // Decay short-term facilitation (exponential decay)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = currentTime - pImpl->lastPreSpikeTime;
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    }
    
    // Recovery from depression (exponential recovery toward baseline)
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? currentTime - pImpl->lastPostSpikeTime : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? currentTime - pImpl->lastPreSpikeTime : 0.0f
        );
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
            (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay (1% per timestep)
    
    // Apply synaptic efficacy to weight modulation (short-term plasticity effect)
    float stpEffect = pImpl->efficacy * pImpl->shortTermDepression * pImpl->shortTermFacilitation;
    stpEffect = std::clamp(stpEffect, 0.0f, 2.0f);
    
    // Apply STDP learning rule if enabled
    if (pImpl->plasticityFlags.stdp) {
        // Simplified STDP: weight changes based on spike timing
        float stdpEffect = 0.0f;
        
        // Record recent spikes for STDP computation
        float timeSincePre = pImpl->lastPreSpikeTime >= 0.0f ? currentTime - pImpl->lastPreSpikeTime : 0.0f;
        float timeSincePost = pImpl->lastPostSpikeTime >= 0.0f ? currentTime - pImpl->lastPostSpikeTime : 0.0f;
        
        // If pre fires before post (causal), strengthen (LTP)
        if (timeSincePre > 0.0f && timeSincePre < timeSincePost) {
            float timingDifference = std::min(timeSincePre, 20.0f); // 20ms window
            stdpEffect += Impl::STP_U_MAX * std::exp(-timingDifference / 10.0f);
        }
        // If post fires before pre (anti-causal), weaken (LTD)
        else if (timeSincePost > 0.0f && timeSincePost < timeSincePre) {
            float timingDifference = std::min(timeSincePost, 20.0f);
            stdpEffect -= Impl::STP_U_MAX * std::exp(-timingDifference / 10.0f);
        }
        
        // Apply STDP to weight with bounds
        pImpl->weight += stdpEffect * 0.01f; // Scale STDP effect
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
    
    // Apply Hebbian learning if enabled
    if (pImpl->plasticityFlags.hebbian) {
        // Simple Hebbian rule: correlated activity strengthens synapse
        if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPostSpikeTime >= 0.0f) {
            float correlation = std::exp(-(std::abs(currentTime - pImpl->lastPreSpikeTime) + 
                                         std::abs(currentTime - pImpl->lastPostSpikeTime)) / 5.0f);
            pImpl->weight += correlation * 0.02f;
            pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        }
    }
    
    // Update last spike times for next timestep
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPreSpikeTime < currentTime) {
        pImpl->lastPreSpikeTime = currentTime;
    }
    
    if (pImpl->lastPostSpikeTime >= 0.0f && pImpl->lastPostSpikeTime < currentTime) {
        pImpl->lastPostSpikeTime = currentTime;
    }
    
    // Clamp weight bounds to maintain stability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Log significant synaptic events for debugging
    if (std::abs(stdpEffect) > 0.1f || std::abs(stpEffect - 1.0f) > 0.1f) {
        // NLM_LOG_DEBUG("Synapse event: STDP=" + std::to_string(stdpEffect) + 
        //             " STP_effect=" + std::to_string(stpEffect) + 
        //             " weight=" + std::to_string(pImpl->weight));
    }
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
