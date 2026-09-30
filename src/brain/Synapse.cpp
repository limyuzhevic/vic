#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Synapse constants class for synaptic parameters
class SynapseConstants {
public:
    // Weight bounds
    static constexpr float DEFAULT_MIN_WEIGHT = -1.0f;
    static constexpr float DEFAULT_MAX_WEIGHT = 1.0f;
    static constexpr float WEIGHT_EPSILON = 0.001f;  // For stability checks
    
    // Short-term plasticity parameters
    static constexpr float DEFAULT_STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float DEFAULT_STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float DEFAULT_STP_U_MAX = 1.0f;  // Max utilization
    static constexpr float DEFAULT_STP_INITIAL_DEPRESSION = 1.0f;  // Fully recovered
    static constexpr float DEFAULT_STP_INITIAL_FACILITATION = 0.0f;  // No initial facilitation
    
    // Eligibility trace
    static constexpr float DEFAULT_ELIGIBILITY_DECAY_RATE = 0.001f;  // Fast decay
    static constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;  // When to consider trace zero
    
    // Synaptic efficacy
    static constexpr float DEFAULT_EFFICACY_MIN = 0.0f;
    static constexpr float DEFAULT_EFFICACY_MAX = 2.0f;
    static constexpr float DEFAULT_EFFICACY = 1.0f;
    
    // Delay
    static constexpr Delay DEFAULT_DELAY_MIN = 1;   // 1 step minimum
    static constexpr Delay DEFAULT_DELAY_MAX = 5;   // 5 steps maximum
    static constexpr Delay DEFAULT_DELAY = 1;       // default: 1 step delay
    
    // Spike history
    static constexpr size_t DEFAULT_MAX_SPIKE_HISTORY = 100;
    
    // Randomization ranges for initialization
    static constexpr float EXCITATORY_WEIGHT_MIN = 0.1f;
    static constexpr float EXCITATORY_WEIGHT_MAX = 0.4f;
    static constexpr float INHIBITORY_WEIGHT_MIN = 0.1f;
    static constexpr float INHIBITORY_WEIGHT_MAX = 0.4f;
    static constexpr float OTHER_WEIGHT_MIN = -0.1f;
    static constexpr float OTHER_WEIGHT_MAX = 0.1f;
    
    static constexpr float EXCITATORY_EFFICACY_MIN = 0.8f;
    static constexpr float EXCITATORY_EFFICACY_MAX = 1.0f;
    static constexpr float INHIBITORY_EFFICACY_MIN = 0.8f;
    static constexpr float INHIBITORY_EFFICACY_MAX = 1.0f;
    static constexpr float OTHER_EFFICACY_MIN = 0.9f;
    static constexpr float OTHER_EFFICACY_MAX = 1.0f;
};

// Synapse configuration structure for fine-tuneable parameters
struct SynapseConfig {
    float minWeight = SynapseConstants::DEFAULT_MIN_WEIGHT;
    float maxWeight = SynapseConstants::DEFAULT_MAX_WEIGHT;
    
    float stpFacilitationTau = SynapseConstants::DEFAULT_STP_FACILITATION_TAU;
    float stpDepressionTau = SynapseConstants::DEFAULT_STP_DEPRESSION_TAU;
    float stpUMax = SynapseConstants::DEFAULT_STP_U_MAX;
    
    float eligibilityDecayRate = SynapseConstants::DEFAULT_ELIGIBILITY_DECAY_RATE;
    float efficacyMin = SynapseConstants::DEFAULT_EFFICACY_MIN;
    float efficacyMax = SynapseConstants::DEFAULT_EFFICACY_MAX;
    
    Delay delayMin = SynapseConstants::DEFAULT_DELAY_MIN;
    Delay delayMax = SynapseConstants::DEFAULT_DELAY_MAX;
    Delay defaultDelay = SynapseConstants::DEFAULT_DELAY;
};

// Helper functions for synapse operations
// Helper function to clamp weight to valid bounds
void clampWeight(float& weight) {
    weight = std::clamp(weight, SynapseConstants::DEFAULT_MIN_WEIGHT, 
                        SynapseConstants::DEFAULT_MAX_WEIGHT);
}

// Helper function to initialize short-term plasticity state
void initializeSTPState(float& depression, float& facilitation) {
    depression = SynapseConstants::DEFAULT_STP_INITIAL_DEPRESSION;
    facilitation = SynapseConstants::DEFAULT_STP_INITIAL_FACILITATION;
}

// Helper function to calculate time difference between spikes
defloat timeDifference(float time1, float time2) {
    return time1 >= 0.0f && time2 >= 0.0f ? std::abs(time1 - time2) : 0.0f;
}

// Helper function to decay eligibility trace
void decayEligibilityTrace(float& trace, float decayRate) {
    trace *= (1.0f - decayRate);
    if (std::abs(trace) < SynapseConstants::ELIGIBILITY_TRACE_THRESHOLD) {
        trace = 0.0f;
    }
}

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
    
    Impl() : id(), sourceNeuron(), destinationNeuron()
        , weight(0.0f), delay(SynapseConstants::DEFAULT_DELAY)
        , type(SynapseType::Excitatory)
        , shortTermDepression(SynapseConstants::DEFAULT_STP_INITIAL_DEPRESSION)
        , shortTermFacilitation(SynapseConstants::DEFAULT_STP_INITIAL_FACILITATION)
        , lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f)
        , eligibilityTrace(0.0f)
        , efficacy(SynapseConstants::DEFAULT_EFFICACY) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = SynapseConstants::DEFAULT_DELAY;
    pImpl->type = SynapseType::Excitatory;
    initializeSTPState(pImpl->shortTermDepression, pImpl->shortTermFacilitation);
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
    // Validate weight bounds
    if (weight < SynapseConstants::DEFAULT_MIN_WEIGHT) {
        NLM_LOG_WARN("Synapse weight too low ({}), clamping to minimum", weight);
        pImpl->weight = SynapseConstants::DEFAULT_MIN_WEIGHT;
    } else if (weight > SynapseConstants::DEFAULT_MAX_WEIGHT) {
        NLM_LOG_WARN("Synapse weight too high ({}), clamping to maximum", weight);
        pImpl->weight = SynapseConstants::DEFAULT_MAX_WEIGHT;
    } else {
        pImpl->weight = weight;
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    clampWeight(pImpl->weight);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    // Validate delay bounds
    if (delay < SynapseConstants::DEFAULT_DELAY_MIN) {
        NLM_LOG_WARN("Synapse delay too short ({}), clamping to minimum", delay);
        pImpl->delay = SynapseConstants::DEFAULT_DELAY_MIN;
    } else if (delay > SynapseConstants::DEFAULT_DELAY_MAX) {
        NLM_LOG_WARN("Synapse delay too long ({}), clamping to maximum", delay);
        pImpl->delay = SynapseConstants::DEFAULT_DELAY_MAX;
    } else {
        pImpl->delay = delay;
    }
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
    if (pImpl->preSpikeHistory.size() > SynapseConstants::DEFAULT_MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > SynapseConstants::DEFAULT_MAX_SPIKE_HISTORY) {
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
    decayEligibilityTrace(pImpl->eligibilityTrace, decayRate);
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    // Validate efficacy bounds
    if (efficacy < SynapseConstants::DEFAULT_EFFICACY_MIN) {
        NLM_LOG_WARN("Synapse efficacy too low ({}), clamping to minimum", efficacy);
        pImpl->efficacy = SynapseConstants::DEFAULT_EFFICACY_MIN;
    } else if (efficacy > SynapseConstants::DEFAULT_EFFICACY_MAX) {
        NLM_LOG_WARN("Synapse efficacy too high ({}), clamping to maximum", efficacy);
        pImpl->efficacy = SynapseConstants::DEFAULT_EFFICACY_MAX;
    } else {
        pImpl->efficacy = efficacy;
    }
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / 
                                               SynapseConstants::DEFAULT_STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = timeDifference(pImpl->lastPostSpikeTime, 
                                               pImpl->lastPreSpikeTime);
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
            (1.0f - std::exp(-timeSinceActivity / SynapseConstants::DEFAULT_STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(pImpl->eligibilityTrace, 
                          SynapseConstants::DEFAULT_ELIGIBILITY_DECAY_RATE);
    
    // Clamp weight bounds
    clampWeight(pImpl->weight);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = SynapseConstants::DEFAULT_EFFICACY;
    initializeSTPState(pImpl->shortTermDepression, pImpl->shortTermFacilitation);
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Better abstraction: Use configuration structure
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(SynapseConstants::EXCITATORY_WEIGHT_MIN, 
                                        SynapseConstants::EXCITATORY_WEIGHT_MAX);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(SynapseConstants::EXCITATORY_EFFICACY_MIN, 
                                          SynapseConstants::EXCITATORY_EFFICACY_MAX);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(SynapseConstants::INHIBITORY_WEIGHT_MIN, 
                                         SynapseConstants::INHIBITORY_WEIGHT_MAX);
        pImpl->efficacy = rng.uniformReal(SynapseConstants::INHIBITORY_EFFICACY_MIN, 
                                          SynapseConstants::INHIBITORY_EFFICACY_MAX);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(SynapseConstants::OTHER_WEIGHT_MIN, 
                                        SynapseConstants::OTHER_WEIGHT_MAX);
        pImpl->efficacy = rng.uniformReal(SynapseConstants::OTHER_EFFICACY_MIN, 
                                          SynapseConstants::OTHER_EFFICACY_MAX);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(
        SynapseConstants::DEFAULT_DELAY_MIN, 
        SynapseConstants::DEFAULT_DELAY_MAX
    ));
    
    // Initialize short-term plasticity state
    initializeSTPState(pImpl->shortTermDepression, pImpl->shortTermFacilitation);
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm