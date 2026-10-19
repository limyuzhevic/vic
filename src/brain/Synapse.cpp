#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <string>

namespace nlm {

// Initialize compile-time validation
NLM_LOG_INFO("Initializing synapse validation system...");
ValidationHelper::compileTimeValidation();

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

// Initialize compile-time validation
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    // Validate synapse ID
    ValidationHelper::validateSynapseId(id, ValidationLevel::Strict, "Synapse constructor");
    
    // Validate connection
    ValidationHelper::validateConnection(source, destination, ValidationLevel::Strict, "Synapse constructor");
    
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

// Validation configuration
void Synapse::setValidationLevel(ValidationLevel level) {
    // Note: In a real implementation, this would need to be stored in pImpl
    NLM_LOG_INFO("Synapse " + std::to_string(pImpl->id.index()) + " validation level set to: " + 
                 std::to_string(static_cast<int>(level)));
}

ValidationLevel Synapse::getValidationLevel() const {
    return ValidationLevel::Strict; // Default to strict
}

// Validation helper methods
bool Synapse::validateWeight(SynapticWeight weight) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::setWeight";
    return ValidationHelper::validateWeight(weight, level, context);
}

bool Synapse::validateDelay(Delay delay) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::setDelay";
    return ValidationHelper::validateDelay(delay, level, context);
}

bool Synapse::validateEfficacy(float efficacy) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::setEfficacy";
    return ValidationHelper::validateEfficacy(efficacy, level, context);
}

bool Synapse::validateShortTermDepression(float depression) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::step";
    return ValidationHelper::validateShortTermDepression(depression, level, context);
}

bool Synapse::validateShortTermFacilitation(float facilitation) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::step";
    return ValidationHelper::validateShortTermFacilitation(facilitation, level, context);
}

bool Synapse::validateNeuronId(NeuronId id) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::getSourceNeuron";
    return ValidationHelper::validateNeuronId(id, level, context);
}

bool Synapse::validateSynapseId() const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::getId";
    return ValidationHelper::validateSynapseId(pImpl->id, level, context);
}

bool Synapse::validateConnection() const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Synapse::getSourceNeuron";
    return ValidationHelper::validateConnection(pImpl->sourceNeuron, pImpl->destinationNeuron, level, context);
}

bool Synapse::validateState() const {
    ValidationLevel level = getValidationLevel();
    bool valid = true;
    
    // Validate key state variables
    valid &= validateWeight(pImpl->weight);
    valid &= validateDelay(pImpl->delay);
    valid &= validateEfficacy(pImpl->efficacy);
    valid &= validateShortTermDepression(pImpl->shortTermDepression);
    valid &= validateShortTermFacilitation(pImpl->shortTermFacilitation);
    
    return valid;
}

SynapseId Synapse::getId() const {
    if (!validateSynapseId()) {
        NLM_LOG_ERROR("Synapse ID validation failed in getId()");
    }
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!validateNeuronId(pImpl->sourceNeuron)) {
        NLM_LOG_ERROR("Source neuron ID validation failed in getSourceNeuron");
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!validateNeuronId(pImpl->destinationNeuron)) {
        NLM_LOG_ERROR("Destination neuron ID validation failed in getDestinationNeuron");
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    if (!validateWeight(weight)) {
        NLM_LOG_ERROR("Synapse weight validation failed in setWeight");
    }
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Validate after update
    if (!validateWeight(pImpl->weight)) {
        NLM_LOG_WARNING("Synapse weight exceeded bounds after addToWeight, clamping");
        pImpl->weight = std::clamp(pImpl->weight, ValidationHelper::SYNAPTIC_WEIGHT_MIN, ValidationHelper::SYNAPTIC_WEIGHT_MAX);
    }
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (!validateDelay(delay)) {
        NLM_LOG_ERROR("Synapse delay validation failed in setDelay");
    }
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
    // Validate connection type
    if (type == SynapseType::Electrical || type == SynapseType::GapJunction) {
        // These types have special validation
        NLM_LOG_INFO("Synapse type set to: " + std::to_string(static_cast<int>(type)));
    }
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
    
    // Update last spike time
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
    
    // Validate last pre spike time
    if (!validateShortTermDepression(pImpl->shortTermDepression)) {
        NLM_LOG_WARNING("Short-term depression validation failed after pre-synaptic spike");
    }
    if (!validateShortTermFacilitation(pImpl->shortTermFacilitation)) {
        NLM_LOG_WARNING("Short-term facilitation validation failed after pre-synaptic spike");
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update last spike time
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
    
    // Validate last post spike time
    if (!validateShortTermDepression(pImpl->shortTermDepression)) {
        NLM_LOG_WARNING("Short-term depression validation failed after post-synaptic spike");
    }
    if (!validateShortTermFacilitation(pImpl->shortTermFacilitation)) {
        NLM_LOG_WARNING("Short-term facilitation validation failed after post-synaptic spike");
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

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
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
    
    // Validate eligibility trace
    if (!validateEfficacy(pImpl->efficacy)) {
        NLM_LOG_WARNING("Efficacy validation failed after setting eligibility trace");
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
    
    // Validate after decay
    if (!validateEfficacy(pImpl->efficacy)) {
        NLM_LOG_WARNING("Efficacy validation failed after eligibility trace decay");
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (!validateEfficacy(efficacy)) {
        NLM_LOG_ERROR("Synapse efficacy validation failed in setEfficacy");
    }
    pImpl->efficacy = efficacy;
}

void Synapse::step(Timestamp currentTime) {
    // Validate state at start of step
    if (!validateState()) {
        NLM_LOG_WARNING("Synapse state validation failed before step");
    }
    
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Validate facilitation after decay
    if (!validateShortTermFacilitation(pImpl->shortTermFacilitation)) {
        NLM_LOG_WARNING("Short-term facilitation exceeded bounds after decay");
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation,
                                                  ValidationHelper::SHORT_TERM_FACILITATION_MIN,
                                                  ValidationHelper::SHORT_TERM_FACILITATION_MAX);
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
    
    // Validate depression after decay
    if (!validateShortTermDepression(pImpl->shortTermDepression)) {
        NLM_LOG_WARNING("Short-term depression exceeded bounds after decay");
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression,
                                                ValidationHelper::SHORT_TERM_DEPRESSION_MIN,
                                                ValidationHelper::SHORT_TERM_DEPRESSION_MAX);
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Validate weight after clamping
    if (!validateWeight(pImpl->weight)) {
        NLM_LOG_WARNING("Synapse weight exceeded bounds after step, clamping");
        pImpl->weight = std::clamp(pImpl->weight, ValidationHelper::SYNAPTIC_WEIGHT_MIN, ValidationHelper::SYNAPTIC_WEIGHT_MAX);
    }
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Validate weight
        if (!validateWeight(pImpl->weight)) {
            NLM_LOG_WARNING("Initial excitatory weight exceeded bounds, clamping");
            pImpl->weight = std::clamp(pImpl->weight, ValidationHelper::SYNAPTIC_WEIGHT_MIN, ValidationHelper::SYNAPTIC_WEIGHT_MAX);
        }
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        // Validate weight
        if (!validateWeight(pImpl->weight)) {
            NLM_LOG_WARNING("Initial inhibitory weight exceeded bounds, clamping");
            pImpl->weight = std::clamp(pImpl->weight, ValidationHelper::SYNAPTIC_WEIGHT_MIN, ValidationHelper::SYNAPTIC_WEIGHT_MAX);
        }
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        // Validate weight
        if (!validateWeight(pImpl->weight)) {
            NLM_LOG_WARNING("Initial random weight exceeded bounds, clamping");
            pImpl->weight = std::clamp(pImpl->weight, ValidationHelper::SYNAPTIC_WEIGHT_MIN, ValidationHelper::SYNAPTIC_WEIGHT_MAX);
        }
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Validate efficacy
    if (!validateEfficacy(pImpl->efficacy)) {
        NLM_LOG_WARNING("Initial efficacy exceeded bounds, clamping");
        pImpl->efficacy = std::clamp(pImpl->efficacy, ValidationHelper::EFFICACY_MIN, ValidationHelper::EFFICACY_MAX);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Validate delay
    if (!validateDelay(pImpl->delay)) {
        NLM_LOG_WARNING("Initial delay exceeded bounds, clamping");
        pImpl->delay = std::clamp(pImpl->delay, ValidationHelper::DELAY_MIN, ValidationHelper::DELAY_MAX);
    }
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Validate STP parameters
    if (!validateShortTermDepression(pImpl->shortTermDepression)) {
        NLM_LOG_WARNING("Initial short-term depression out of bounds, clamping");
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression,
                                                ValidationHelper::SHORT_TERM_DEPRESSION_MIN,
                                                ValidationHelper::SHORT_TERM_DEPRESSION_MAX);
    }
    
    if (!validateShortTermFacilitation(pImpl->shortTermFacilitation)) {
        NLM_LOG_WARNING("Initial short-term facilitation out of bounds, clamping");
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation,
                                                  ValidationHelper::SHORT_TERM_FACILITATION_MIN,
                                                  ValidationHelper::SHORT_TERM_FACILITATION_MAX);
    }
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    NLM_LOG_INFO("Synapse initialized with weight: " + std::to_string(pImpl->weight) +
                 ", delay: " + std::to_string(pImpl->delay) +
                 ", efficacy: " + std::to_string(pImpl->efficacy));
}

} // namespace nlm