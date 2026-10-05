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

float Synapse::calculateRecentActivity() const {
    // Calculate recent synaptic activity for metaplastic changes
    float activity = 0.0f;
    
    Timestamp currentTime = 0;  // Would need current simulation time
    
    // Count recent spikes in history (last 100ms)
    size_t recentPre = 0, recentPost = 0;
    float window = 100.0f;  // 100ms window
    
    for (const auto& spikeTime : pImpl->preSpikeHistory) {
        if (static_cast<float>(currentTime - spikeTime) <= window) {
            recentPre++;
        }
    }
    
    for (const auto& spikeTime : pImpl->postSpikeHistory) {
        if (static_cast<float>(currentTime - spikeTime) <= window) {
            recentPost++;
        }
    }
    
    // Calculate normalized activity
    activity = (static_cast<float>(recentPre + recentPost) / (2.0f * Impl::MAX_SPIKE_HISTORY));
    return activity;
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Update short-term plasticity state based on recent activity
    // 2. Decay eligibility trace for reward-modulated learning
    // 3. Update short-term depression/facilitation
    
    updateShortTermPlasticity(currentTime);
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Update synaptic efficacy based on recent use (metaplasticity)
    float recentActivity = calculateRecentActivity();
    setEfficacy(std::clamp(efficacy + recentActivity * 0.05f - 0.01f, 0.5f, 2.0f));
    
    // Apply STDP if both pre and post spikes have occurred recently
    if (plasticityFlags.stdp && !preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        // Most recent spikes
        Timestamp preSpike = preSpikeHistory.back();
        Timestamp postSpike = postSpikeHistory.back();
        
        // Apply STDP update
        applySTDP(preSpike, postSpike);
        
        // Clear spike history for this synapse to prevent multiple updates
        clearHistory();
        recordPreSpike(currentTime);
        recordPostSpike(currentTime);
    }
    
    // Clamp weight bounds
    weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
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

// STDP, Hebbian, and Reward-Modulated Learning Implementation

namespace nlm {

bool Synapse::isHebbianEnabled() const {
    return pImpl->plasticityFlags.hebbian;
}

bool Synapse::isSTDPenabled() const {
    return pImpl->plasticityFlags.stdp;
}

bool Synapse::isRewardModulatedEnabled() const {
    return pImpl->plasticityFlags.reward_modulated;
}

void Synapse::applySTDP(Timestamp preSpikeTime, Timestamp postSpikeTime, float learningRate) {
    if (!pImpl->plasticityFlags.stdp) return;
    
    float deltaTime = static_cast<float>(postSpikeTime - preSpikeTime);
    
    // STDP rule: Spike-Timing-Dependent Plasticity
    // Long-Term Potentiation (LTP): pre fires before post (positive deltaTime)
    // Long-Term Depression (LTD): post fires before pre (negative deltaTime)
    
    if (deltaTime > 0) {
        // Pre before post: LTP (strengthen synapse)
        float weightChange = learningRate * (1.0f - std::exp(-std::abs(deltaTime) / 20.0f));
        addToWeight(weightChange);
        NLM_LOG_DEBUG(std::string("STDP LTP: pre+post (Δt=" + std::to_string(deltaTime) + ") weight+=" + std::to_string(weightChange)));
    } else {
        // Post before pre: LTD (weaken synapse)
        float weightChange = -learningRate * (1.0f - std::exp(-std::abs(deltaTime) / 20.0f));
        addToWeight(weightChange);
        NLM_LOG_DEBUG(std::string("STDP LTD: post+pre (Δt=" + std::to_string(deltaTime) + ") weight+=" + std::to_string(weightChange)));
    }
    
    // Update eligibility trace
    float eligibilityIncrement = std::abs(weightChange);
    pImpl->eligibilityTrace += eligibilityIncrement;
}

void Synapse::applyHebbianLearning(float correlation, float learningRate) {
    if (!pImpl->plasticityFlags.hebbian) return;
    
    // Hebbian rule: Correlated activity strengthens connections
    // "Neurons that fire together, wire together"
    
    float weightChange = learningRate * correlation;
    addToWeight(weightChange);
    
    // Hebbian learning affects eligibility trace
    pImpl->eligibilityTrace += std::abs(weightChange) * 0.5f;
    
    NLM_LOG_DEBUG(std::string("Hebbian: correlation=" + std::to_string(correlation) + " weight+=" + std::to_string(weightChange)));
}

void Synapse::applyRewardModulatedLearning(float rewardPredictionError, float learningRate) {
    if (!pImpl->plasticityFlags.reward_modulated) return;
    
    // Reward-modulated learning: combines eligibility trace with reward prediction error
    // Rescorla-Wagner style learning: delta = learning_rate * (reward - prediction)
    
    float weightChange = learningRate * pImpl->eligibilityTrace * rewardPredictionError;
    addToWeight(weightChange);
    
    // Decay eligibility trace after learning
    decayEligibilityTrace(0.1f);
    
    NLM_LOG_DEBUG(std::string("RewardModulated: RPE=" + std::to_string(rewardPredictionError) + " eligibility=" + std::to_string(pImpl->eligibilityTrace) + " weight+=" + std::to_string(weightChange)));
}

void Synapse::updatePlasticity(Timestamp preSpikeTime, Timestamp postSpikeTime, 
                             float correlation, float rewardError) {
    // Combine all plasticity mechanisms for comprehensive learning
    
    if (pImpl->plasticityFlags.stdp) {
        applySTDP(preSpikeTime, postSpikeTime);
    }
    
    if (pImpl->plasticityFlags.hebbian) {
        applyHebbianLearning(correlation);
    }
    
    if (pImpl->plasticityFlags.reward_modulated) {
        applyRewardModulatedLearning(rewardError);
    }
}

SynapticWeight Synapse::getEffectiveWeight() const {
    // Calculate effective synaptic strength considering multiple factors
    float effectiveWeight = pImpl->weight;
    
    // Weight efficacy modulation
    effectiveWeight *= pImpl->efficacy;
    
    // Short-term plasticity modulation
    effectiveWeight *= (1.0f + (pImpl->shortTermFacilitation - 1.0f) * 0.3f - (1.0f - pImpl->shortTermDepression) * 0.2f);
    
    // Ensure bounds
    effectiveWeight = std::clamp(effectiveWeight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    return effectiveWeight;
}

float Synapse::getShortTermDepression() const {
    return pImpl->shortTermDepression;
}

float Synapse::getShortTermFacilitation() const {
    return pImpl->shortTermFacilitation;
}

void Synapse::updateShortTermPlasticity(Timestamp currentTime) {
    // Update short-term plasticity state (Tsodyks-Markram model)
    
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
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

} // namespace nlm