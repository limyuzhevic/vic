#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

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

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination) {
    // Validate input parameters
    if (!Validation::isValidSynapseId(id)) {
        ExceptionManager::throwError<NLMException>(
            "Invalid synapse ID for synapse creation");
    }
    if (!Validation::isValidNeuronId(source)) {
        ExceptionManager::throwError<NLMException>(
            "Invalid source neuron ID for synapse creation");
    }
    if (!Validation::isValidNeuronId(destination)) {
        ExceptionManager::throwError<NLMException>(
            "Invalid destination neuron ID for synapse creation");
    }
    if (source.value == destination.value) {
        ExceptionManager::throwError<NLMException>(
            "Synapse cannot connect a neuron to itself");
    }
    
    try {
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
    } catch (const std::exception& e) {
        // If initialization fails, clean up and throw
        delete pImpl;
        pImpl = nullptr;
        ExceptionManager::throwError<MemoryError>(
            "Failed to initialize synapse: " + std::string(e.what()),
            MemoryError::Type::MemoryError);
    }
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        try {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        } catch (const std::exception& e) {
            ExceptionManager::getInstance().logError(NLMException(
                "Failed to move-assign synapse: " + std::string(e.what())));
        }
    }
    return *this;
}

SynapseId Synapse::getId() const {
    try {
        return pImpl->id;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get synapse ID: " + std::string(e.what()));
    }
}

NeuronId Synapse::getSourceNeuron() const {
    try {
        return pImpl->sourceNeuron;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get source neuron: " + std::string(e.what()));
    }
}

NeuronId Synapse::getDestinationNeuron() const {
    try {
        return pImpl->destinationNeuron;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get destination neuron: " + std::string(e.what()));
    }
}

SynapticWeight Synapse::getWeight() const {
    try {
        return pImpl->weight;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get synapse weight: " + std::string(e.what()));
    }
}

void Synapse::setWeight(SynapticWeight weight) {
    try {
        if (!Validation::isValidWeight(weight)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid synapse weight: " + std::to_string(weight));
        }
        
        pImpl->weight = weight;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set synapse weight: " + std::string(e.what()));
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    try {
        if (!Validation::isValidWeight(delta)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid weight delta: " + std::to_string(delta));
        }
        
        pImpl->weight += delta;
        // Clamp to reasonable bounds to prevent instability
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to add to synapse weight: " + std::string(e.what()));
    }
}

Delay Synapse::getDelay() const {
    try {
        return pImpl->delay;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get synapse delay: " + std::string(e.what()));
    }
}

void Synapse::setDelay(Delay delay) {
    try {
        if (delay == 0) {
            ExceptionManager::throwError<NLMException>(
                "Synapse delay cannot be zero");
        }
        if (delay > 1000) {  // Reasonable upper limit
            ExceptionManager::throwError<NLMException>(
                "Synapse delay too large: " + std::to_string(delay));
        }
        
        pImpl->delay = delay;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set synapse delay: " + std::string(e.what()));
    }
}

SynapseType Synapse::getType() const {
    try {
        return pImpl->type;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get synapse type: " + std::string(e.what()));
    }
}

void Synapse::setType(SynapseType type) {
    try {
        if (static_cast<uint8_t>(type) > static_cast<uint8_t>(SynapseType::GapJunction)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid synapse type: " + std::to_string(static_cast<int>(type)));
        }
        
        pImpl->type = type;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set synapse type: " + std::string(e.what()));
    }
}

bool Synapse::isExcitatory() const {
    try {
        return pImpl->type == SynapseType::Excitatory;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to check if synapse is excitatory: " + std::string(e.what()));
    }
}

bool Synapse::isInhibitory() const {
    try {
        return pImpl->type == SynapseType::Inhibitory;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to check if synapse is inhibitory: " + std::string(e.what()));
    }
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    try {
        if (!Validation::isValidTimestamp(timestamp)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid spike timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->preSpikeHistory.push_back(timestamp);
        if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to record pre-synaptic spike: " + std::string(e.what()));
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    try {
        if (!Validation::isValidTimestamp(timestamp)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid spike timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->postSpikeHistory.push_back(timestamp);
        if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to record post-synaptic spike: " + std::string(e.what()));
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    try {
        return pImpl->preSpikeHistory;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get pre-spike history: " + std::string(e.what()));
    }
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    try {
        return pImpl->postSpikeHistory;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get post-spike history: " + std::string(e.what()));
    }
}

void Synapse::clearHistory() {
    try {
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to clear spike history: " + std::string(e.what()));
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    try {
        return pImpl->plasticityFlags;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get plasticity flags: " + std::string(e.what()));
    }
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    try {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to enable plasticity: " + std::string(e.what()));
    }
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    try {
        return pImpl->plasticityFlags;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get plasticity flags: " + std::string(e.what()));
    }
}

float Synapse::getEligibilityTrace() const {
    try {
        return pImpl->eligibilityTrace;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get eligibility trace: " + std::string(e.what()));
    }
}

void Synapse::setEligibilityTrace(float trace) {
    try {
        if (!Validation::isValidMembranePotential(trace)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid eligibility trace: " + std::to_string(trace));
        }
        
        pImpl->eligibilityTrace = trace;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set eligibility trace: " + std::string(e.what()));
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    try {
        if (!Validation::isValidMembranePotential(decayRate) || decayRate < 0.0f || decayRate > 1.0f) {
            ExceptionManager::throwError<NLMException>(
                "Invalid decay rate: " + std::to_string(decayRate));
        }
        
        pImpl->eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to decay eligibility trace: " + std::string(e.what()));
    }
}

float Synapse::getEfficacy() const {
    try {
        return pImpl->efficacy;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to get synaptic efficacy: " + std::string(e.what()));
    }
}

void Synapse::setEfficacy(float efficacy) {
    try {
        if (!Validation::isValidMembranePotential(efficacy)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid synaptic efficacy: " + std::to_string(efficacy));
        }
        
        pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set synaptic efficacy: " + std::string(e.what()));
    }
}

void Synapse::step(Timestamp currentTime) {
    try {
        // Validate input parameters
        if (!Validation::isValidTimestamp(currentTime)) {
            ExceptionManager::throwError<NLMException>(
                "Invalid current time: " + std::to_string(currentTime));
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
        
        // Clamp weight bounds
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to step synapse: " + std::string(e.what()));
    }
}

void Synapse::reset() {
    try {
        pImpl->weight = 0.0f;
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->eligibilityTrace = 0.0f;
        pImpl->efficacy = 1.0f;
        pImpl->shortTermDepression = 1.0f;
        pImpl->shortTermFacilitation = 0.0f;
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to reset synapse: " + std::string(e.what()));
    }
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    try {
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
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to initialize synapse randomly: " + std::string(e.what()),
            RandomError::Type::OutOfBounds);
    }
}

} // namespace nlm
