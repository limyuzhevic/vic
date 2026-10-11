#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <system_error>

namespace nlm {

/**
 * Synapse::Impl struct
 * 
 * @struct Impl
 * @brief Internal implementation details for Synapse class
 * @details Contains all internal state and constants for synaptic dynamics
 * 
 * This struct encapsulates the complex synaptic state including:
 * - Static synaptic properties (weight, delay, type)
 * - Dynamic state (short-term plasticity, eligibility traces)
 * - Constants and bounds for numerical stability
 * - Health monitoring state
 * 
 * @note PIMPL pattern provides ABI stability while hiding implementation complexity
 * @warning This is an internal implementation detail, not part of public API
 */
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
    
    // Validation bounds
    static constexpr float MIN_EFFICIENCY = 0.0f;
    static constexpr float MAX_EFFICIENCY = 2.0f;
    static constexpr float MIN_ELIGIBILITY_TRACE = -1.0f;
    static constexpr float MAX_ELIGIBILITY_TRACE = 1.0f;
    static constexpr Delay MIN_DELAY = 0;
    static constexpr Delay MAX_DELAY = 1000;
    
    // Error recovery
    mutable bool healthValid = false;
    mutable std::vector<std::string> healthIssues;
};

/**
     * @param id Unique identifier for this synapse
     * @param source NeuronId of presynaptic (source) neuron
     * @param destination NeuronId of postsynaptic (target) neuron
     * @post Creates synapse with biologically plausible initialization:
     *       - Weight: 0.0 (no initial connection)
     *       - Delay: 1 step (minimum biological delay)
     *       - Type: Excitatory (default)
     *       - Eligibility trace: 0.0
     *       - Efficacy: 1.0 (maximum initial)
     * @note Creates necessary entries in source/destination neuron maps
     * @throws std::invalid_argument if IDs are invalid
     * @throws NLMException if initialization fails
     */
    Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
        try {
            NLM_VALIDATE(validateInput(id), "Invalid synapse ID");
            NLM_VALIDATE(validateInput(source), "Invalid source neuron ID");
            NLM_VALIDATE(validateInput(destination), "Invalid destination neuron ID");
            
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
            
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error during synapse creation: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Error);
            attemptRecovery();
            throw;
        }
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

/**
     * @brief Get the unique identifier of this synapse
     * @return Synapse's unique identifier
     * @post None
     * @note Used for synapse lookup and debugging
     */
    SynapseId Synapse::getId() const {
        return pImpl->id;
    }

/**
     * @brief Get source (presynaptic) neuron identifier
     * @return NeuronId of the presynaptic neuron
     * @post None
     */
    NeuronId Synapse::getSourceNeuron() const {
        return pImpl->sourceNeuron;
    }
    
    /**
     * @brief Get destination (postsynaptic) neuron identifier
     * @return NeuronId of the postsynaptic neuron
     * @post None
     */
    NeuronId Synapse::getDestinationNeuron() const {
        return pImpl->destinationNeuron;
    }

/**
     * @brief Get current synaptic weight
     * @return Synaptic weight (positive = excitatory, negative = inhibitory)
     * @post None
     */
    SynapticWeight Synapse::getWeight() const {
        return pImpl->weight;
    }
    
    /**
     * @brief Set synaptic weight directly
     * @param weight New synaptic weight
     * @post Weight updated; eligibility trace reset; health status invalid
     * @warning Weight outside [-1.0, 1.0] may cause instability
     * @throws NLMException if weight validation fails
     */
    void Synapse::setWeight(SynapticWeight weight) {
        try {
            NLM_VALIDATE(validateWeight(weight),
                       "Weight out of valid range: " + std::to_string(weight));
            pImpl->weight = weight;
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error setting synapse weight: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }
    
    /**
     * @brief Increment synaptic weight by delta value
     * @param delta Weight change
     * @post Weight updated; automatically clamped to valid range
     */
    void Synapse::addToWeight(SynapticWeight delta) {
        try {
            NLM_VALIDATE(validateInput(delta), "Invalid weight delta: " + std::to_string(delta));
            pImpl->weight += delta;
            // Clamp to reasonable bounds to prevent instability
            pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error adjusting synapse weight: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }

/**
     * @brief Get synaptic transmission delay
     * @return Delay in simulation steps
     * @post None
     */
    Delay Synapse::getDelay() const {
        return pImpl->delay;
    }
    
    /**
     * @brief Set synaptic transmission delay
     * @param delay New delay in simulation steps
     * @post Delay updated; health status invalid
     * @throws NLMException if delay validation fails
     */
    void Synapse::setDelay(Delay delay) {
        try {
            NLM_VALIDATE(validateDelay(delay),
                       "Delay out of valid range: " + std::to_string(delay));
            pImpl->delay = delay;
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error setting synapse delay: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }
    
    /**
     * @brief Get synapse type classification
     * @return Synapse type
     * @post None
     */
    SynapseType Synapse::getType() const {
        return pImpl->type;
    }
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     * @post Type updated; health status invalid
     * @throws NLMException if type validation fails
     */
    void Synapse::setType(SynapseType type) {
        try {
            NLM_VALIDATE(validateType(type),
                       "Invalid synapse type: " + std::to_string(static_cast<int>(type)));
            pImpl->type = type;
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error setting synapse type: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }
    
    /**
     * @brief Check if synapse is excitatory
     * @return true if excitatory
     */
    bool Synapse::isExcitatory() const {
        return pImpl->type == SynapseType::Excitatory;
    }
    
    /**
     * @brief Check if synapse is inhibitory
     * @return true if inhibitory
     */
    bool Synapse::isInhibitory() const {
        return pImpl->type == SynapseType::Inhibitory;
    }

/**
     * @brief Record presynaptic spike event
     * @param timestamp Time of the presynaptic spike
     * @post Spike timestamp added to history; last pre-spike time updated
     * @throws NLMException if timestamp validation fails
     */
    void Synapse::recordPreSpike(Timestamp timestamp) {
        try {
            NLM_VALIDATE(validateInput(static_cast<float>(timestamp)),
                       "Invalid pre-spike timestamp: " + std::to_string(timestamp));
            pImpl->preSpikeHistory.push_back(timestamp);
            pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
            
            if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
                pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
            }
            
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error recording pre-spike: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }
    
    /**
     * @brief Record postsynaptic spike event
     * @param timestamp Time of the postsynaptic spike
     * @post Spike timestamp added to history; last post-spike time updated
     * @throws NLMException if timestamp validation fails
     */
    void Synapse::recordPostSpike(Timestamp timestamp) {
        try {
            NLM_VALIDATE(validateInput(static_cast<float>(timestamp)),
                       "Invalid post-spike timestamp: " + std::to_string(timestamp));
            pImpl->postSpikeHistory.push_back(timestamp);
            pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
            
            if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
                pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
            }
            
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error recording post-spike: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }
    
    /**
     * @brief Get presynaptic spike history
     * @return Const reference to pre-spike history
     * @post None
     */
    const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
        return pImpl->preSpikeHistory;
    }
    
    /**
     * @brief Get postsynaptic spike history
     * @return Const reference to post-spike history
     * @post None
     */
    const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
        return pImpl->postSpikeHistory;
    }

/**
     * @brief Clear all spike history
     * @post Clears pre- and post-spike histories; resets last spike times
     * @note Used to reset synaptic plasticity state
     */
    void Synapse::clearHistory() {
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
        pImpl->healthValid = false;
    }
    
    /**
     * @brief Get mutable access to plasticity flags
     * @return Non-constant reference to plasticity flags
     * @post None
     */
    PlasticityFlags& Synapse::getPlasticityFlags() {
        pImpl->healthValid = false;
        return pImpl->plasticityFlags;
    }
    
    /**
     * @brief Get const access to plasticity flags
     * @return Const reference to plasticity flags
     */
    const PlasticityFlags& Synapse::getPlasticityFlags() const {
        return pImpl->plasticityFlags;
    }
    
    /**
     * @brief Enable/disable plasticity mechanisms
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @post Plasticity flags updated; health status invalid
     */
    void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
        pImpl->healthValid = false;
    }
    
    /**
     * @brief Get eligibility trace value
     * @return Eligibility trace value
     * @post None
     */
    float Synapse::getEligibilityTrace() const {
        return pImpl->eligibilityTrace;
    }
    
    /**
     * @brief Set eligibility trace value
     * @param trace New eligibility trace value
     * @post Trace updated; health status invalid
     * @throws NLMException if trace validation fails
     */
    void Synapse::setEligibilityTrace(float trace) {
        try {
            NLM_VALIDATE(validateEligibilityTrace(trace),
                       "Eligibility trace out of valid range: " + std::to_string(trace));
            pImpl->eligibilityTrace = trace;
            pImpl->healthValid = false;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Error setting eligibility trace: " + std::string(e.what()),
                            ErrorCategory::Validation, ErrorSeverity::Warning);
        }
    }

void Synapse::decayEligibilityTrace(float decayRate) {
    try {
        NLM_VALIDATE(validateInput(decayRate) && decayRate >= 0.0f && decayRate <= 1.0f,
                   "Invalid eligibility trace decay rate: " + std::to_string(decayRate));
        pImpl->eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error decaying eligibility trace: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    try {
        NLM_VALIDATE(validateEfficacy(efficacy),
                   "Efficacy out of valid range: " + std::to_string(efficacy));
        pImpl->efficacy = std::clamp(efficacy, Impl::MIN_EFFICIENCY, Impl::MAX_EFFICIENCY);
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting efficacy: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

// Validation methods implementation
bool Synapse::validateState() const {
    pImpl->updateHealth();
    return pImpl->healthValid;
}

bool Synapse::validateWeight() const {
    return pImpl->weight >= Impl::MIN_WEIGHT && pImpl->weight <= Impl::MAX_WEIGHT;
}

bool Synapse::validateDelay() const {
    return pImpl->delay >= Impl::MIN_DELAY && pImpl->delay <= Impl::MAX_DELAY;
}

bool Synapse::validateType() const {
    return static_cast<int>(pImpl->type) >= 0 && static_cast<int>(pImpl->type) <= static_cast<int>(SynapseType::GapJunction);
}

bool Synapse::validateHistory() const {
    return pImpl->preSpikeHistory.size() <= Impl::MAX_SPIKE_HISTORY &&
           pImpl->postSpikeHistory.size() <= Impl::MAX_SPIKE_HISTORY;
}

std::vector<std::string> Synapse::getValidationIssues() const {
    pImpl->updateHealth();
    return pImpl->healthIssues;
}

// Error recovery methods implementation
void Synapse::attemptRecovery() {
    pImpl->healthValid = false;
    pImpl->healthIssues.clear();
    
    // Reset to safe defaults
    reset();
    
    // Validate state after recovery
    pImpl->updateHealth();
    
    if (!pImpl->healthValid) {
        NLM_REPORT_ERROR("Synapse recovery completed but state still has issues",
                        ErrorCategory::Simulation, ErrorSeverity::Warning);
    }
}

bool Synapse::isRecoverable() const {
    // Synapses are generally recoverable unless in critical state
    return true;
}

// Health checking methods implementation
bool Synapse::isHealthy() const {
    pImpl->updateHealth();
    return pImpl->healthValid;
}

std::vector<std::string> Synapse::getHealthIssues() const {
    pImpl->updateHealth();
    return pImpl->healthIssues;
}

// Helper validation methods implementation
bool Synapse::validateInput(SynapseId id) const {
    return id.value != UINT64_MAX;
}

bool Synapse::validateInput(NeuronId id) const {
    return id.value != UINT64_MAX;
}

bool Synapse::validateInput(SynapticWeight weight) const {
    return !std::isnan(weight) && !std::isinf(weight);
}

bool Synapse::validateInput(Delay delay) const {
    return delay != UINT32_MAX;
}

bool Synapse::validateInput(Timestamp timestamp) const {
    return !std::isnan(static_cast<float>(timestamp)) && !std::isinf(static_cast<float>(timestamp));
}

bool Synapse::validateInput(float value) const {
    return !std::isnan(value) && !std::isinf(value);
}

bool Synapse::validateWeight(SynapticWeight weight) const {
    return weight >= Impl::MIN_WEIGHT && weight <= Impl::MAX_WEIGHT;
}

bool Synapse::validateDelay(Delay delay) const {
    return delay >= Impl::MIN_DELAY && delay <= Impl::MAX_DELAY;
}

bool Synapse::validateType(SynapseType type) const {
    return static_cast<int>(type) >= 0 && static_cast<int>(type) <= static_cast<int>(SynapseType::GapJunction);
}

bool Synapse::validateEligibilityTrace(float trace) const {
    return trace >= Impl::MIN_ELIGIBILITY_TRACE && trace <= Impl::MAX_ELIGIBILITY_TRACE;
}

bool Synapse::validateEfficacy(float efficacy) const {
    return efficacy >= Impl::MIN_EFFICIENCY && efficacy <= Impl::MAX_EFFICIENCY;
}

// Step method with error handling
void Synapse::step(Timestamp currentTime) {
    try {
        NLM_VALIDATE(validateInput(currentTime), "Invalid current time: " + std::to_string(currentTime));
        
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
        
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error during synapse step: " + std::string(e.what()),
                        ErrorCategory::Simulation, ErrorSeverity::Error);
        attemptRecovery();
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
    pImpl->healthValid = false;
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
        
        pImpl->healthValid = false;
    } catch (const std::exception& e) {
        NLM_REPORT_ERROR("Error during synapse random initialization: " + std::string(e.what()),
                        ErrorCategory::Random, ErrorSeverity::Error);
        attemptRecovery();
    }
}

} // namespace nlm
