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
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Additional fields for enhanced management
    float lastUpdateTime;     // Track last update for decay calculations
    
    // Spike history trimming state
    size_t spikeHistoryTrimCount;  // Number of spikes trimmed
    
    // Eligibility trace and activity tracking
    float eligibilityTrace;   // For reward-modulated learning
    float totalSpikeActivity; // For efficacy calculations
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Integration constants
    static constexpr float MAX_UPDATE_TIME = 1000.0f;  // Max time between updates
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
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->lastUpdateTime = -1.0f;
    pImpl->spikeHistoryTrimCount = 0;
    pImpl->totalSpikeActivity = 0.0f;
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
    // Enhanced pre-synaptic spike recording with validation and bounds checking
    if (timestamp < 0.0f) {
        NLM_LOG_WARNING("Invalid spike timestamp: " + std::to_string(timestamp));
        return;
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    pImpl->lastPreSpikeTime = timestamp;
    
    // Apply short-term plasticity effects
    if (pImpl->type == SynapseType::Excitatory) {
        // Facilitation for excitatory synapses
        pImpl->shortTermFacilitation = std::min(pImpl->shortTermFacilitation * 1.5f, 1.0f);
        pImpl->shortTermDepression *= 0.8f;
    }
    
    // Clamp bounds
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    
    // Remove old traces for efficiency
    auto cutoff = timestamp - 100.0f;
    pImpl->preSpikeHistory.erase(
        std::remove_if(pImpl->preSpikeHistory.begin(), pImpl->preSpikeHistory.end(),
                     [cutoff](Timestamp t) { return t < cutoff; }),
        pImpl->preSpikeHistory.end()
    );
    
    NLM_LOG_DEBUG("Recorded pre-synaptic spike at " + std::to_string(timestamp) +
                 " (history size: " + std::to_string(pImpl->preSpikeHistory.size()) + ")");
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Enhanced post-synaptic spike recording with validation and effects
    if (timestamp < 0.0f) {
        NLM_LOG_WARNING("Invalid spike timestamp: " + std::to_string(timestamp));
        return;
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    pImpl->lastPostSpikeTime = timestamp;
    
    // Apply plasticity effects
    if (pImpl->type == SynapseType::Inhibitory) {
        // Depression for inhibitory synapses
        pImpl->shortTermDepression = std::min(pImpl->shortTermDepression * 0.8f, 1.0f);
        pImpl->shortTermFacilitation *= 1.5f;
    }
    
    // Clamp bounds
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    
    // Remove old traces for efficiency
    auto cutoff = timestamp - 100.0f;
    pImpl->postSpikeHistory.erase(
        std::remove_if(pImpl->postSpikeHistory.begin(), pImpl->postSpikeHistory.end(),
                     [cutoff](Timestamp t) { return t < cutoff; }),
        pImpl->postSpikeHistory.end()
    );
    
    NLM_LOG_DEBUG("Recorded post-synaptic spike at " + std::to_string(timestamp) +
                 " (history size: " + std::to_string(pImpl->postSpikeHistory.size()) + ")");
}

void Synapse::updateForSTDP(Timestamp preTime, Timestamp postTime, float learningRate) {
    // Enhanced STDP update with better timing and learning rules
    if (!pImpl->plasticityFlags.stdp) {
        return;
    }
    
    float deltaT = postTime - preTime;
    
    if (deltaT > 0.0f) {
        // Pre before post: Long-Term Potentiation (LTP)
        float weightChange = learningRate * std::exp(-std::abs(deltaT) / 20.0f);
        addToWeight(weightChange);
        NLM_LOG_DEBUG("STDP LTP: Δ=" + std::to_string(weightChange) + " (ΔT=" + std::to_string(deltaT) + "ms)");
    } else {
        // Post before pre: Long-Term Depression (LTD)
        float weightChange = -learningRate * std::exp(-std::abs(deltaT) / 20.0f);
        addToWeight(weightChange);
        NLM_LOG_DEBUG("STDP LTD: Δ=" + std::to_string(weightChange) + " (ΔT=" + std::to_string(deltaT) + "ms)");
    }
}

void Synapse::updateForReward(float reward, float predictionError, float decayRate) {
    // Enhanced reward-modulated learning with eligibility traces
    if (!pImpl->plasticityFlags.reward_modulated) {
        return;
    }
    
    // Update eligibility trace based on spike history
    float eligibility = 0.0f;
    
    // Calculate eligibility based on recent spike correlations
    if (!pImpl->preSpikeHistory.empty() || !pImpl->postSpikeHistory.empty()) {
        float preFactor = 0.0f;
        float postFactor = 0.0f;
        
        // Weight recent spikes more heavily
        for (auto& timestamp : pImpl->preSpikeHistory) {
            float age = currentTime - timestamp;
            preFactor += (age <= 50.0f) ? 1.0f : 0.0f;
        }
        
        for (auto& timestamp : pImpl->postSpikeHistory) {
            float age = currentTime - timestamp;
            postFactor += (age <= 50.0f) ? 1.0f : 0.0f;
        }
        
        eligibility = preFactor * postFactor * pImpl->efficacy;
    }
    
    // Update eligibility trace
    pImpl->eligibilityTrace += eligibility;
    
    // Apply reward modulation
    float modulatedLearning = reward * eligibility * predictionError;
    addToWeight(modulatedLearning);
    
    // Decay eligibility trace
    decayEligibilityTrace(decayRate);
    
    NLM_LOG_DEBUG("Reward modulation: reward=" + std::to_string(reward) +
                 " prediction_error=" + std::to_string(predictionError) +
                 " eligibility=" + std::to_string(eligibility) +
                 " delta=" + std::to_string(modulatedLearning));
}

float Synapse::calculateSTDPWeightChange(Timestamp preTime, Timestamp postTime, float ltpRate, float ltdRate) {
    // Calculate STDP weight change without applying it
    float deltaT = postTime - preTime;
    
    if (deltaT > 0.0f) {
        // Pre before post: LTP
        return ltpRate * std::exp(-std::abs(deltaT) / 20.0f);
    } else {
        // Post before pre: LTD
        return -ltdRate * std::exp(-std::abs(deltaT) / 20.0f);
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
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getShortTermFacilitation() const {
    return pImpl->shortTermFacilitation;
}

float Synapse::getShortTermDepression() const {
    return pImpl->shortTermDepression;
}

Timestamp Synapse::getLastPreSpikeTime() const {
    return pImpl->lastPreSpikeTime;
}

Timestamp Synapse::getLastPostSpikeTime() const {
    return pImpl->lastPostSpikeTime;
}

size_t Synapse::getPreSpikeCount(Timestamp sinceTime) const {
    size_t count = 0;
    for (const auto& timestamp : pImpl->preSpikeHistory) {
        if (sinceTime <= 0.0f || timestamp >= sinceTime) {
            count++;
        }
    }
    return count;
}

size_t Synapse::getPostSpikeCount(Timestamp sinceTime) const {
    size_t count = 0;
    for (const auto& timestamp : pImpl->postSpikeHistory) {
        if (sinceTime <= 0.0f || timestamp >= sinceTime) {
            count++;
        }
    }
    return count;
}

float Synapse::getSpikeFrequency(Timestamp sinceTime) const {
    if (sinceTime <= 0.0f) return 0.0f;
    
    size_t preCount = getPreSpikeCount(sinceTime);
    size_t postCount = getPostSpikeCount(sinceTime);
    
    if (sinceTime == 0.0f) return 0.0f;
    
    return static_cast<float>(preCount + postCount) / sinceTime;
}

bool Synapse::isPotentiationExpected(Timestamp preTime, Timestamp postTime) const {
    return (postTime - preTime) > 0.0f;
}

float Synapse::getEstimatedWeightChange(Timestamp preTime, Timestamp postTime) const {
    float deltaT = postTime - preTime;
    float learningRate = 0.001f; // Default rate
    
    if (deltaT > 0.0f) {
        return learningRate * std::exp(-std::abs(deltaT) / 20.0f);  // LTP
    } else {
        return -learningRate * std::exp(-std::abs(deltaT) / 20.0f);  // LTD
    }
}

void Synapse::step(Timestamp currentTime) {
    // Enhanced synaptic dynamics with better plasticity management
    // 1. Update spike histories and decay
    // 2. Update short-term plasticity state
    // 3. Update eligibility trace for reward-modulated learning
    // 4. Validate weight bounds
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Update spike histories and decay
    // Decay pre-post spike traces to maintain relevance
    for (auto& trace : pImpl->preSpikeHistory) {
        trace = trace * 0.99f + (currentTime - trace) * 0.01f;  // Exponential decay
    }
    
    for (auto& trace : pImpl->postSpikeHistory) {
        trace = trace * 0.99f + (currentTime - trace) * 0.01f;
    }
    
    // Remove old spike traces (older than 100ms)
    auto cutoff = currentTime - 100.0f;
    pImpl->preSpikeHistory.erase(
        std::remove_if(pImpl->preSpikeHistory.begin(), pImpl->preSpikeHistory.end(),
                     [cutoff](Timestamp t) { return t < cutoff; }),
        pImpl->preSpikeHistory.end()
    );
    
    pImpl->postSpikeHistory.erase(
        std::remove_if(pImpl->postSpikeHistory.begin(), pImpl->postSpikeHistory.end(),
                     [cutoff](Timestamp t) { return t < cutoff; }),
        pImpl->postSpikeHistory.end()
    );
    
    // Update short-term plasticity state with improved Tsodyks-Markram model
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        
        // Facilitation dynamics (U increases, x increases)
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermDepression *= (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSincePre / Impl::STP_DEPRESSION_TAU));
        
        // Ensure bounds
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    }
    
    // Update post spike effects
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePost = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        
        // Depression recovery
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSincePost / Impl::STP_DEPRESSION_TAU));
        
        // Clamp bounds
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Validate and clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Update efficacy based on recent activity
    float activityFactor = 0.0f;
    if (!pImpl->preSpikeHistory.empty() || !pImpl->postSpikeHistory.empty()) {
        activityFactor = 1.0f;
        
        // Boost efficacy for high activity periods
        if (pImpl->preSpikeHistory.size() > 5) {
            activityFactor *= 1.1f;
        }
    }
    
    pImpl->efficacy = std::clamp(pImpl->efficacy * activityFactor, 0.0f, 2.0f);
}

void Synapse::reset() {
    // Do NOT zero the weight here. Brain::reset() is called at the start
    // of every Phase 3 episode, and zeroing weights there disconnected the
    // whole network: every sensory spike then delivered weight 0, so
    // internal and motor neurons stayed exactly at rest and the agent
    // could never move. Weight is long-term connectivity / learned state;
    // reset only clears short-term state (spike histories, eligibility)
    // and restores efficacy, so a new episode starts with an intact brain.
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
    pImpl->delay = static_cast<Delay>(rng.uniformInt(uint32_t(1), uint32_t(5)));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

Timestamp Synapse::getLastPreSpikeTime() const {
    return pImpl->lastPreSpikeTime;
}

Timestamp Synapse::getLastPostSpikeTime() const {
    return pImpl->lastPostSpikeTime;
}

size_t Synapse::getPreSpikeCount(Timestamp sinceTime) const {
    size_t count = 0;
    for (const auto& timestamp : pImpl->preSpikeHistory) {
        if (sinceTime <= 0.0f || timestamp >= sinceTime) {
            count++;
        }
    }
    return count;
}

size_t Synapse::getPostSpikeCount(Timestamp sinceTime) const {
    size_t count = 0;
    for (const auto& timestamp : pImpl->postSpikeHistory) {
        if (sinceTime <= 0.0f || timestamp >= sinceTime) {
            count++;
        }
    }
    return count;
}

float Synapse::getSpikeFrequency(Timestamp sinceTime) const {
    if (sinceTime <= 0.0f) return 0.0f;
    
    size_t preCount = getPreSpikeCount(sinceTime);
    size_t postCount = getPostSpikeCount(sinceTime);
    
    if (sinceTime == 0.0f) return 0.0f;
    
    return static_cast<float>(preCount + postCount) / sinceTime;
}

bool Synapse::isPotentiationExpected(Timestamp preTime, Timestamp postTime) const {
    return (postTime - preTime) > 0.0f;
}

float Synapse::getEstimatedWeightChange(Timestamp preTime, Timestamp postTime) const {
    float deltaT = postTime - preTime;
    float learningRate = 0.001f; // Default rate
    
    if (deltaT > 0.0f) {
        return learningRate * std::exp(-std::abs(deltaT) / 20.0f);  // LTP
    } else {
        return -learningRate * std::exp(-std::abs(deltaT) / 20.0f);  // LTD
    }
}

// Implement the missing setOutputFile and closeOutputFile methods in Logger.cpp
void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}
