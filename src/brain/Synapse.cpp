#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <random>

namespace nlm {

// Synapse implementation with enhanced bounds checking and initialization

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : id_(id), sourceNeuron_(source), destinationNeuron_(destination),
      weight_(0.0f), delay_(1), type_(SynapseType::Excitatory),
      shortTermDepression_(1.0f), shortTermFacilitation_(0.0f),
      lastPreSpikeTime_(-1.0f), lastPostSpikeTime_(-1.0f),
      eligibilityTrace_(0.0f), efficacy_(SynapseConstants::DEFAULT_EFFICIENCY),
      advancedFeaturesEnabled_(false), noiseLevel_(0.1f) {
}

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination, const Config& config)
    : id_(id), sourceNeuron_(source), destinationNeuron_(destination),
      weight_(0.0f), delay_(config.initialDelay), type_(SynapseType::Excitatory),
      shortTermDepression_(1.0f), shortTermFacilitation_(0.0f),
      lastPreSpikeTime_(-1.0f), lastPostSpikeTime_(-1.0f),
      eligibilityTrace_(0.0f), efficacy_(config.initialEfficacy),
      config_(config), advancedFeaturesEnabled_(false), noiseLevel_(0.1f) {
    
    // Validate initial parameters
    validateWeight();
    validateEfficacy();
    validateHistorySizes();
    
    NLM_LOG_INFO("Synapse created: ID=" + std::to_string(id) +
                 " source=" + std::to_string(source) +
                 " dest=" + std::to_string(destination) +
                 " type=Excitatory");
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept
    : id_(other.id_), sourceNeuron_(other.sourceNeuron_), destinationNeuron_(other.destinationNeuron_),
      weight_(other.weight_), delay_(other.delay_), type_(other.type_),
      preSpikeHistory_(std::move(other.preSpikeHistory_)),
      postSpikeHistory_(std::move(other.postSpikeHistory_)),
      plasticityFlags_(std::move(other.plasticityFlags_)),
      shortTermDepression_(other.shortTermDepression_),
      shortTermFacilitation_(other.shortTermFacilitation_),
      lastPreSpikeTime_(other.lastPreSpikeTime_),
      lastPostSpikeTime_(other.lastPostSpikeTime_),
      eligibilityTrace_(other.eligibilityTrace_),
      efficacy_(other.efficacy_),
      config_(std::move(other.config_)),
      advancedFeaturesEnabled_(other.advancedFeaturesEnabled_),
      noiseLevel_(other.noiseLevel_) {}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        sourceNeuron_ = other.sourceNeuron_;
        destinationNeuron_ = other.destinationNeuron_;
        weight_ = other.weight_;
        delay_ = other.delay_;
        type_ = other.type_;
        preSpikeHistory_ = std::move(other.preSpikeHistory_);
        postSpikeHistory_ = std::move(other.postSpikeHistory_);
        plasticityFlags_ = std::move(other.plasticityFlags_);
        shortTermDepression_ = other.shortTermDepression_;
        shortTermFacilitation_ = other.shortTermFacilitation_;
        lastPreSpikeTime_ = other.lastPreSpikeTime_;
        lastPostSpikeTime_ = other.lastPostSpikeTime_;
        eligibilityTrace_ = other.eligibilityTrace_;
        efficacy_ = other.efficacy_;
        config_ = std::move(other.config_);
        advancedFeaturesEnabled_ = other.advancedFeaturesEnabled_;
        noiseLevel_ = other.noiseLevel_;
    }
    return *this;
}

void Synapse::setWeight(SynapticWeight weight) {
    weight_ = weight;
    validateWeight();
    clampWeight();
}

void Synapse::addToWeight(SynapticWeight delta) {
    weight_ += delta;
    validateWeight();
    clampWeight();
}

void Synapse::setDelay(Delay delay) {
    if (delay < 0) {
        throw std::invalid_argument("Delay cannot be negative");
    }
    delay_ = delay;
}

void Synapse::setType(SynapseType type) {
    type_ = type;
    // Adjust weight bounds based on synapse type
    validateWeight();
}

void Synapse::clearHistory() {
    preSpikeHistory_.clear();
    postSpikeHistory_.clear();
    lastPreSpikeTime_ = -1.0f;
    lastPostSpikeTime_ = -1.0f;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    preSpikeHistory_.push_back(timestamp);
    lastPreSpikeTime_ = timestamp;
    validateHistorySizes();
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    postSpikeHistory_.push_back(timestamp);
    lastPostSpikeTime_ = timestamp;
    validateHistorySizes();
}

void Synapse::setEligibilityTrace(float trace) {
    eligibilityTrace_ = trace;
    if (config_.enableEligibilityTrace) {
        if (std::abs(eligibilityTrace_) < config_.minEligibility) {
            eligibilityTrace_ = 0.0f;
        }
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (config_.enableEligibilityTrace) {
        eligibilityTrace_ *= (1.0f - decayRate);
        if (std::abs(eligibilityTrace_) < config_.minEligibility) {
            eligibilityTrace_ = 0.0f;
        }
    }
}

void Synapse::setEfficacy(float efficacy) {
    efficacy_ = efficacy;
    validateEfficacy();
    normalizeEfficacy();
}

void Synapse::reset() {
    weight_ = 0.0f;
    preSpikeHistory_.clear();
    postSpikeHistory_.clear();
    eligibilityTrace_ = 0.0f;
    resetShortTermPlasticity();
    validateWeight();
    validateEfficacy();
}

void Synapse::resetShortTermPlasticity() {
    shortTermDepression_ = 1.0f;
    shortTermFacilitation_ = 0.0f;
    lastPreSpikeTime_ = -1.0f;
    lastPostSpikeTime_ = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (type_ == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        weight_ = rng.uniformReal(SynapseConstants::EXCITATORY_MIN_WEIGHT, 
                                SynapseConstants::EXCITATORY_MAX_WEIGHT);
        // Excitatory synapses have moderate initial efficacy
        efficacy_ = rng.uniformReal(0.8f, 1.0f);
    } else if (type_ == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        weight_ = -rng.uniformReal(SynapseConstants::EXCITATORY_MIN_WEIGHT, 
                                 SynapseConstants::EXCITATORY_MAX_WEIGHT);
        efficacy_ = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        weight_ = rng.uniformReal(SynapseConstants::MODULATORY_MIN_WEIGHT, 
                                 SynapseConstants::MODULATORY_MAX_WEIGHT);
        efficacy_ = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    delay_ = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    resetShortTermPlasticity();
    
    // Initialize eligibility trace to 0
    eligibilityTrace_ = 0.0f;
    
    // Validate initial state
    validateWeight();
    validateEfficacy();
    
    NLM_LOG_INFO("Synapse initialized: weight=" + std::to_string(weight_) +
                 " delay=" + std::to_string(delay_) +
                 " efficacy=" + std::to_string(efficacy_));
}

void Synapse::validateWeight() const {
    float minWeight, maxWeight;
    
    switch (type_) {
        case SynapseType::Excitatory:
            minWeight = SynapseConstants::EXCITATORY_MIN_WEIGHT;
            maxWeight = SynapseConstants::EXCITATORY_MAX_WEIGHT;
            break;
        case SynapseType::Inhibitory:
            minWeight = SynapseConstants::INHIBITORY_MIN_WEIGHT;
            maxWeight = SynapseConstants::INHIBITORY_MAX_WEIGHT;
            break;
        case SynapseType::Modulatory:
            minWeight = SynapseConstants::MODULATORY_MIN_WEIGHT;
            maxWeight = SynapseConstants::MODULATORY_MAX_WEIGHT;
            break;
        default:
            minWeight = SynapseConstants::MIN_WEIGHT;
            maxWeight = SynapseConstants::MAX_WEIGHT;
            break;
    }
    
    if (weight_ < minWeight || weight_ > maxWeight) {
        NLM_LOG_WARNING("Synapse weight out of bounds: " + std::to_string(weight_));
    }
}

void Synapse::validateEfficacy() const {
    if (efficacy_ < SynapseConstants::MIN_EFFICIENCY || efficacy_ > SynapseConstants::MAX_EFFICIENCY) {
        NLM_LOG_WARNING("Synapse efficacy out of bounds: " + std::to_string(efficacy_));
    }
}

void Synapse::validateHistorySizes() const {
    if (preSpikeHistory_.size() > config_.maxSpikeHistory) {
        NLM_LOG_WARNING("Pre-spike history exceeds maximum size");
    }
    if (postSpikeHistory_.size() > config_.maxSpikeHistory) {
        NLM_LOG_WARNING("Post-spike history exceeds maximum size");
    }
}

bool Synapse::isSafe() const {
    return std::isfinite(weight_) &&
           std::isfinite(efficacy_) &&
           weight_ >= SynapseConstants::MIN_WEIGHT &&
           weight_ <= SynapseConstants::MAX_WEIGHT &&
           efficacy_ >= SynapseConstants::MIN_EFFICIENCY &&
           efficacy_ <= SynapseConstants::MAX_EFFICIENCY;
}

void Synapse::enableAdvancedFeatures(bool enable, float noiseLevel) {
    advancedFeaturesEnabled_ = enable;
    if (enable) {
        noiseLevel_ = std::clamp(noiseLevel, 0.0f, 1.0f);
        NLM_LOG_INFO("Advanced features enabled for synapse with noise level: " + std::to_string(noiseLevel_));
    } else {
        NLM_LOG_INFO("Advanced features disabled for synapse");
    }
}

void Synapse::updateShortTermPlasticity(Timestamp currentTime) {
    if (!config_.enableShortTermPlasticity) return;
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (lastPreSpikeTime_ >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - lastPreSpikeTime_);
        shortTermFacilitation_ *= std::exp(-timeSincePre / config_.stpFacilitationTau);
    }
    
    // Decay short-term depression
    if (lastPostSpikeTime_ >= 0.0f || lastPreSpikeTime_ >= 0.0f) {
        float timeSinceActivity = std::max(
            lastPostSpikeTime_ >= 0.0f ? static_cast<float>(currentTime - lastPostSpikeTime_) : 0.0f,
            lastPreSpikeTime_ >= 0.0f ? static_cast<float>(currentTime - lastPreSpikeTime_) : 0.0f
        );
        // Recovery from depression toward 1.0
        shortTermDepression_ += (1.0f - shortTermDepression_) * (1.0f - std::exp(-timeSinceActivity / config_.stpDepressionTau));
    }
    
    // Clamp values
    shortTermFacilitation_ = std::max(0.0f, std::min(config_.stpUMax, shortTermFacilitation_));
    shortTermDepression_ = std::max(0.0f, std::min(1.0f, shortTermDepression_));
}

void Synapse::clampWeight() {
    float minWeight, maxWeight;
    
    switch (type_) {
        case SynapseType::Excitatory:
            minWeight = SynapseConstants::EXCITATORY_MIN_WEIGHT;
            maxWeight = SynapseConstants::EXCITATORY_MAX_WEIGHT;
            break;
        case SynapseType::Inhibitory:
            minWeight = SynapseConstants::INHIBITORY_MIN_WEIGHT;
            maxWeight = SynapseConstants::INHIBITORY_MAX_WEIGHT;
            break;
        case SynapseType::Modulatory:
            minWeight = SynapseConstants::MODULATORY_MIN_WEIGHT;
            maxWeight = SynapseConstants::MODULATORY_MAX_WEIGHT;
            break;
        default:
            minWeight = SynapseConstants::MIN_WEIGHT;
            maxWeight = SynapseConstants::MAX_WEIGHT;
            break;
    }
    
    weight_ = std::clamp(weight_, minWeight, maxWeight);
}

void Synapse::normalizeEfficacy() {
    efficacy_ = std::clamp(efficacy_, SynapseConstants::MIN_EFFICIENCY, SynapseConstants::MAX_EFFICIENCY);
}

float Synapse::calculateShortTermEffectiveness() const {
    return shortTermDepression_ * shortTermFacilitation_;
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    updateShortTermPlasticity(currentTime);
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(config_.eligibilityDecayRate);
    
    // Clamp weight bounds
    clampWeight();
    
    // Validate state
    if (!isSafe()) {
        NLM_LOG_WARNING("Synapse safety check failed");
    }
}

} // namespace nlm
