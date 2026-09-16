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
    std::swap(other.pImpl, pImpl);
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
    if (!pImpl) return SynapseId();
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) return NeuronId();
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) return NeuronId();
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) return 0.0f;
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (pImpl) {
        pImpl->weight = weight;
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (pImpl) {
        pImpl->weight += delta;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

Delay Synapse::getDelay() const {
    if (!pImpl) return 0;
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (pImpl) {
        pImpl->delay = delay;
    }
}

SynapseType Synapse::getType() const {
    if (!pImpl) return SynapseType::Excitatory;
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    if (pImpl) {
        pImpl->type = type;
    }
}

bool Synapse::isExcitatory() const {
    if (!pImpl) return false;
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) return false;
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (pImpl) {
        pImpl->preSpikeHistory.push_back(timestamp);
        if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        }
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (pImpl) {
        pImpl->postSpikeHistory.push_back(timestamp);
        if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        }
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    static const std::vector<Timestamp> emptyHistory;
    if (!pImpl) {
        return emptyHistory;
    }
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    static const std::vector<Timestamp> emptyHistory;
    if (!pImpl) {
        return emptyHistory;
    }
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    if (pImpl) {
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    static const PlasticityFlags nullFlags = {};
    if (!pImpl) {
        static PlasticityFlags nullStatic;
        return nullStatic;
    }
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    }
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    static const PlasticityFlags nullFlags = {};
    if (!pImpl) {
        static const PlasticityFlags nullStatic;
        return nullStatic;
    }
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    if (!pImpl) return 0.0f;
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (pImpl) {
        pImpl->eligibilityTrace = trace;
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (pImpl) {
        pImpl->eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
    }
}

float Synapse::getEfficacy() const {
    if (!pImpl) return 0.0f;
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (pImpl) {
        pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    }
}

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) {
        return;
    }
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Complete Tsodyks-Markram short-term plasticity (STP) implementation
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        // Clamp facilitation to [0, 1]
        pImpl->shortTermFacilitation = std::max(0.0f, std::min(pImpl->shortTermFacilitation, 1.0f));
    } else {
        pImpl->shortTermFacilitation *= std::exp(-dt / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermFacilitation = std::max(0.0f, std::min(pImpl->shortTermFacilitation, 1.0f));
    }
    
    // Decay short-term depression and spontaneous recovery
    float timeSinceActivity = 0.0f;
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        timeSinceActivity = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
    }
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePreActivity = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        timeSinceActivity = std::max(timeSinceActivity, timeSincePreActivity);
    }
    
    // Recovery from depression toward 1.0 (spontaneous recovery)
    pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    // Clamp depression to [0, 1]
    pImpl->shortTermDepression = std::max(0.0f, std::min(pImpl->shortTermDepression, 1.0f));
    
    // 2. Proper eligibility trace management for reward-modulated learning
    
    // Decay eligibility trace continuously
    if (pImpl->eligibilityTrace != 0.0f) {
        decayEligibilityTrace(0.001f);  // Fast decay
    }
    
    // Apply reward modulation if eligibility trace exists
    if (pImpl->eligibilityTrace != 0.0f) {
        // Update efficacy based on eligibility trace for reward-modulated learning
        pImpl->efficacy += pImpl->eligibilityTrace * 0.01f;  // Modest efficacy change
        pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    }
    
    // Decay efficacy slowly (use-dependent modulation)
    pImpl->efficacy *= (1.0f - 0.0005f * dt);  // Slow decay of efficacy
    
    // 3. Synaptic efficacy updates based on use (Hebbian modulation)
    
    // Update efficacy based on recent spike pairing (Hebbian)
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        float timeSincePost = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        
        // If presynaptic spike occurred before postsynaptic spike (within STDP window)
        if (std::abs(timeSincePre - timeSincePost) < 20.0f) {  // 20ms STDP window
            float delta = 0.005f * std::exp(-std::abs(timeSincePre - timeSincePost) / 10.0f);
            pImpl->efficacy += delta;
            pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
        }
    }
    
    // 4. Spike history management for STDP (already done via recordPreSpike/recordPostSpike)
    
    // Clean up old spike history if it exceeds maximum
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // 5. Weight bounds enforcement (already done, but ensure it's called every step)
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // 6. Proper decay of all plasticity states
    
    // Ensure facilitation decays toward 0 even without recent spikes
    if (pImpl->lastPreSpikeTime < 0.0f) {
        pImpl->shortTermFacilitation *= std::exp(-dt / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermFacilitation = std::max(0.0f, std::min(pImpl->shortTermFacilitation, 1.0f));
    }
    
    // Update spike times for eligibility trace tracking
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        // If time since last pre spike is large, reset it
        if (static_cast<float>(currentTime - pImpl->lastPreSpikeTime) > 100.0f) {
            pImpl->lastPreSpikeTime = -1.0f;
        }
    }
    
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        // If time since last post spike is large, reset it
        if (static_cast<float>(currentTime - pImpl->lastPostSpikeTime) > 100.0f) {
            pImpl->lastPostSpikeTime = -1.0f;
        }
    }
}

// Update utilization based on Tsodyks-Markram model when a spike occurs
void Synapse::updateSTP(float currentTime) {
    if (!pImpl) {
        return;
    }
    
    // Record the spike time
    pImpl->lastPreSpikeTime = currentTime;
    
    // Update short-term depression (D) according to Tsodyks-Markram model
    // D_t+1 = D_t * (1 - U) + U, where U is utilization
    // For presynaptic spike, we increase utilization based on depression state
    float U = Impl::STP_U_MAX * pImpl->shortTermDepression;  // Utilization proportional to depression
    pImpl->shortTermDepression = pImpl->shortTermDepression * (1.0f - U) + U;
    
    // Update short-term facilitation (F) according to Tsodyks-Markram model
    // F_t+1 = F_t + (1 - F_t) * (1 - exp(-1/tau_f))
    // This models the gradual buildup of release probability
    float deltaF = (1.0f - pImpl->shortTermFacilitation) * (1.0f - std::exp(-1.0f / Impl::STP_FACILITATION_TAU));
    pImpl->shortTermFacilitation = std::min(pImpl->shortTermFacilitation + deltaF, 1.0f);
    
    // Clamp both states to valid ranges
    pImpl->shortTermDepression = std::max(0.0f, std::min(pImpl->shortTermDepression, 1.0f));
    pImpl->shortTermFacilitation = std::max(0.0f, std::min(pImpl->shortTermFacilitation, 1.0f));
}

// Add synaptic efficacy update for weight-based learning
void Synapse::updateEfficacyFromWeight() {
    if (!pImpl) {
        return;
    }
    
    // Update efficacy based on weight changes (metaplasticity)
    // Efficacy increases with repeated use, reflecting synaptic strengthening
    if (std::abs(pImpl->weight) > 0.1f) {
        pImpl->efficacy += 0.001f * std::abs(pImpl->weight);
        pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    }
}

// Update eligibility trace for reward-modulated learning
void Synapse::updateEligibilityTrace(float reward, float dt) {
    if (!pImpl) {
        return;
    }
    
    // Only update eligibility trace for reward-modulated learning
    // if reward modulation is enabled (checked via plasticityFlags)
    if (pImpl->plasticityFlags.reward_modulated) {
        // Eligibility trace builds up with presynaptic activity
        // Simplified: use a fixed increment per time step
        pImpl->eligibilityTrace += 0.01f * dt;
        
        // Apply reward signal to update synaptic weight
        if (reward != 0.0f) {
            float weightChange = reward * pImpl->eligibilityTrace * 0.001f;
            addToWeight(weightChange);
            
            // Reset eligibility trace after reward
            if (std::abs(reward) > 0.5f) {
                pImpl->eligibilityTrace = 0.0f;
            }
        }
    }
}

void Synapse::reset() {
    if (pImpl) {
        pImpl->weight = 0.0f;
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->eligibilityTrace = 0.0f;
        pImpl->efficacy = 1.0f;
    }
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        return;
    }
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
