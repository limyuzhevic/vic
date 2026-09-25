#include "Synapse.hpp"
#include "CommonUtilities.hpp"
#include "../core/Random/Random.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>

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

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination) : pImpl(new Impl) {
    NLM_VALIDATE(id.value > 0, id, NLMErrors::InvalidParameter, "Synapse ID must be positive");
    NLM_VALIDATE(source.value > 0, source, NLMErrors::InvalidParameter, "Source neuron ID must be positive");
    NLM_VALIDATE(destination.value > 0, destination, NLMErrors::InvalidParameter, "Destination neuron ID must be positive");
    NLM_VALIDATE(source.value != destination.value, source,
        NLMErrors::InvalidParameter, "Source and destination neurons must be different");
    
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
    
    NLM_LOG_DEBUG("Created synapse " + std::to_string(id.value) + 
                 " (source: " + std::to_string(source.value) + 
                 ", destination: " + std::to_string(destination.value) + ")");
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
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(weight), weight, NLMErrors::InvalidParameter, "Weight must be finite");
    NLM_VALIDATE(weight >= Impl::MIN_WEIGHT && weight <= Impl::MAX_WEIGHT, weight,
        NLMErrors::InvalidParameter, "Weight out of valid range [-1.0, 1.0]");
    
    pImpl->weight = weight;
    NLM_LOG_DEBUG("Set synapse " + std::to_string(pImpl->id.value) + 
                 " weight to " + std::to_string(weight));
}

void Synapse::setType(SynapseType type) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(static_cast<int>(type) >= 0 && static_cast<int>(type) <= static_cast<int>(SynapseType::GapJunction),
        type, NLMErrors::InvalidParameter, "Invalid synapse type");
    
    pImpl->type = type;
}

SynapseType Synapse::getType() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->type;
}

Delay Synapse::getDelay() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(delay >= 0, delay, NLMErrors::InvalidParameter, "Delay must be non-negative");
    NLM_VALIDATE(delay <= 10000, delay, NLMErrors::InvalidParameter, "Delay too large (max 10000 steps)");
    
    pImpl->delay = delay;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(timestamp), timestamp, NLMErrors::InvalidParameter, "Spike timestamp must be finite");
    NLM_VALIDATE(timestamp >= 0.0f, timestamp, NLMErrors::InvalidParameter, "Spike timestamp must be non-negative");
    
    // Prevent spike history overflow
    if (pImpl->preSpikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    pImpl->lastPreSpikeTime = timestamp;
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(timestamp), timestamp, NLMErrors::InvalidParameter, "Spike timestamp must be finite");
    NLM_VALIDATE(timestamp >= 0.0f, timestamp, NLMErrors::InvalidParameter, "Spike timestamp must be non-negative");
    
    // Prevent spike history overflow
    if (pImpl->postSpikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    pImpl->lastPostSpikeTime = timestamp;
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->postSpikeHistory;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.rewardModulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(trace), trace, NLMErrors::InvalidParameter, "Eligibility trace must be finite");
    
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(decayRate), decayRate, NLMErrors::InvalidParameter, "Decay rate must be finite");
    NLM_VALIDATE(decayRate >= 0.0f && decayRate <= 1.0f, decayRate, NLMErrors::InvalidParameter, "Decay rate must be between 0 and 1");
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(efficacy), efficacy, NLMErrors::InvalidParameter, "Efficacy must be finite");
    
    // Clamp using utility function
    pImpl->efficacy = WeightClamper::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    NLM_VALIDATE(std::isfinite(currentTime), currentTime, NLMErrors::InvalidParameter, "Current time must be finite");
    NLM_VALIDATE(currentTime >= 0.0f, currentTime, NLMErrors::InvalidParameter, "Current time must be non-negative");
    
    try {
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
        
        // Decay eligibility trace for reward-modulated learning
        decayEligibilityTrace(0.001f);  // Fast decay
        
        // Clamp weight bounds using utility function
        pImpl->weight = WeightClamper::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        
        NLM_LOG_DEBUG("Stepped synapse " + std::to_string(pImpl->id.value) + 
                     " at time " + std::to_string(currentTime));
        
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::IntegrationFailed, "Error in synapse dynamics: " + std::string(e.what()));
    }
}

void Synapse::reset() {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    
    NLM_LOG_DEBUG("Resetting synapse " + std::to_string(pImpl->id.value));
    
    pImpl->weight = 0.0f;
    nlm::clearHistory(pImpl->preSpikeHistory);
    nlm::clearHistory(pImpl->postSpikeHistory);
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    NLM_VALIDATE(pImpl != nullptr, pImpl, NLMErrors::SystemNotInitialized, "Synapse not properly initialized");
    
    try {
        // Proper random initialization based on synapse type
        if (pImpl->type == SynapseType::Excitatory) {
            // Excitatory synapses: small positive weights
            pImpl->weight = rng.uniformReal(0.1f, 0.4f);
            // Excitatory synapses have moderate initial efficacy
            pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
            NLM_LOG_DEBUG("Initialized excitatory synapse with weight: " + std::to_string(pImpl->weight));
        } else if (pImpl->type == SynapseType::Inhibitory) {
            // Inhibitory synapses: negative weights
            pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
            pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
            NLM_LOG_DEBUG("Initialized inhibitory synapse with weight: " + std::to_string(pImpl->weight));
        } else {
            // Other types: small random weights
            pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
            pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
            NLM_LOG_DEBUG("Initialized other-type synapse with weight: " + std::to_string(pImpl->weight));
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
        
        NLM_LOG_DEBUG("Randomly initialized synapse " + std::to_string(pImpl->id.value));
        
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::DevelopmentError, "Failed to randomly initialize synapse: " + std::string(e.what()));
    }
}

} // namespace nlm
