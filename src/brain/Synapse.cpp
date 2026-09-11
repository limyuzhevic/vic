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
    
    // 1. Update short-term plasticity state (Tsodyks-Markram model)
    // Decay facilitation
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Apply Tsodyks-Markram dynamics
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        // When presynaptic spike occurs, update utilization factor
        // U = U_max * F * D, where U_max is STP_U_MAX
        pImpl->shortTermFacilitation = std::min(pImpl->shortTermFacilitation + pImpl->shortTermDepression, Impl::STP_U_MAX);
        
        // Record spike for plasticity
        recordPreSpike(currentTime);
    }
    
    // 2. Handle synaptic transmission and post-synaptic effects
    // Process spikes that have reached their destination based on delay
    for (size_t i = 0; i < pImpl->preSpikeHistory.size();) {
        float timeSinceSpike = static_cast<float>(currentTime - pImpl->preSpikeHistory[i]);
        // Check if this spike should be delivered to post-synaptic neuron
        if (timeSinceSpike >= pImpl->delay * dt) {
            // Deliver spike to post-synaptic neuron
            // This would normally be handled by the brain module
            // For now, we record the post-synaptic spike for plasticity
            recordPostSpike(currentTime);
            
            // Update short-term depression due to post-synaptic spike
            // Depression factor: D = D_prev + (1 - D_prev) * (1 - exp(-Δt/τ))
            if (pImpl->lastPreSpikeTime >= 0.0f) {
                float spikeInterval = currentTime - pImpl->lastPreSpikeTime;
                if (spikeInterval > 0) {
                    pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-spikeInterval / Impl::STP_DEPRESSION_TAU));
                }
            }
            
            pImpl->lastPostSpikeTime = currentTime;
            
            // Remove delivered spike from history
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin() + i);
        } else {
            i++;
        }
    }
    
    // 3. Apply STDP and Hebbian plasticity rules
    if (pImpl->plasticityFlags.stdp) {
        // STDP: synaptic weight change depends on spike timing differences
        // Analyze recent spike pairs
        float latestPre = -1.0f;
        float latestPost = -1.0f;
        
        if (!pImpl->preSpikeHistory.empty()) {
            latestPre = pImpl->preSpikeHistory.back();
        }
        if (!pImpl->postSpikeHistory.empty()) {
            latestPost = pImpl->postSpikeHistory.back();
        }
        
        if (latestPre >= 0.0f && latestPost >= 0.0f) {
            float timeDiff = latestPost - latestPre;  // postsynaptic - presynaptic
            
            if (timeDiff > 0) {
                // Late post-synaptic spike (LTP)
                float deltaW = 0.01f * std::exp(timeDiff / 20.0f);  // Typical STDP curve
                addToWeight(deltaW);
            } else if (timeDiff < 0) {
                // Early pre-synaptic spike (LTD)
                float deltaW = -0.01f * std::exp(-timeDiff / 20.0f);  // Typical STDP curve
                addToWeight(deltaW);
            }
            
            // Clear recent spike pairs after applying STDP
            pImpl->preSpikeHistory.clear();
            pImpl->postSpikeHistory.clear();
        } else if (latestPre >= 0.0f) {
            // Only pre-synaptic spike (LTD)
            float deltaW = -0.01f * std::exp(-static_cast<float>(currentTime - latestPre) / 50.0f);
            addToWeight(deltaW);
        } else if (latestPost >= 0.0f) {
            // Only post-synaptic spike (LTP)
            float deltaW = 0.01f * std::exp(static_cast<float>(currentTime - latestPost) / 50.0f);
            addToWeight(deltaW);
        }
    }
    
    if (pImpl->plasticityFlags.hebbian) {
        // Hebbian co-activation rule: simultaneous spikes strengthen connections
        // Simple implementation: when pre and post spike within a time window, potentiate
        for (size_t i = 0; i < pImpl->preSpikeHistory.size(); ++i) {
            for (size_t j = 0; j < pImpl->postSpikeHistory.size(); ++j) {
                float timeDiff = std::abs(pImpl->postSpikeHistory[j] - pImpl->preSpikeHistory[i]);
                if (timeDiff < 20.0f) {  // Co-activation within 20ms
                    float deltaW = 0.005f * (1.0f - timeDiff / 20.0f);  // Stronger for closer spikes
                    addToWeight(deltaW);
                    
                    // Record that we've applied Hebbian rule
                    break;
                }
            }
        }
    }
    
    // 4. Apply reward-modulated learning through eligibility traces
    if (pImpl->plasticityFlags.reward_modulated && pImpl->eligibilityTrace > 0.0f) {
        // Weight update = eligibility * reward * learning rate
        float reward = 1.0f;  // In real scenarios, this would come from global reward signal
        float learningRate = 0.001f;
        float deltaW = pImpl->eligibilityTrace * reward * learningRate;
        addToWeight(deltaW);
    }
    
    // 5. Decay eligibility trace
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // 6. Update synaptic efficacy based on usage
    // Efficacy modulates synaptic response strength based on recent activity
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePost = currentTime - pImpl->lastPostSpikeTime;
        // Recent co-activation increases efficacy
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy + 0.01f * (1.0f - timeSincePost / 100.0f));
    } else {
        // Decay efficacy back to baseline when not used
        pImpl->efficacy *= 0.99f;
    }
    
    // Clamp weight bounds to prevent instability
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
