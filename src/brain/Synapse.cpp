/**
 * @file Synapse.cpp
 * @brief Implementation of the Synapse class - synaptic transmission with short-term plasticity
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full synaptic dynamics implementation planned for Phase 2.
 */

#include "Synapse.hpp"

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @struct Synapse::Impl
 * @brief Private implementation details for the Synapse class (Pimpl idiom)
 * @details This struct contains all the actual implementation details of the Synapse class,
 *          including synaptic state, spike history, short-term plasticity, eligibility traces,
 *          and synaptic efficacy. This allows the Synapse class to be forward-declared and
 *          maintains binary compatibility.
 *
 * The implementation includes:
 * - Synapse identity and connectivity (source/destination neurons)
 * - Synaptic weight, type, and delay parameters
 * - Spike history for STDP plasticity
 * - Plasticity state and flags
 * - Short-term plasticity state (facilitation and depression)
 * - Eligibility trace for reward-modulated learning
 * - Synaptic efficacy for use-dependent modulation
 * - Physical constraints and parameters
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
    
    // ========== SHORT-TERM PLASTICITY STATE ==========
    
    /**
     * @brief Short-term depression factor (0-1)
     * @details Represents the utilization of synaptic resources.
     *          Values close to 1.0 indicate high utilization (more depression).
     *          This evolves based on presynaptic activity history.
     */
    float shortTermDepression;  // Utilization factor (0-1)
    
    /**
     * @brief Short-term facilitation factor (0-2+)
     * @details Represents the accumulation of synaptic resources.
     *          Higher values indicate more facilitation.
     *          This evolves based on presynaptic activity history.
     */
    float shortTermFacilitation;  // Facilitation factor
    
    /**
     * @brief Timestamp of last presynaptic spike (seconds)
     * @details Used to compute time since last presynaptic activity
     *          for short-term plasticity dynamics.
     */
    float lastPreSpikeTime;  // For short-term dynamics
    
    /**
     * @brief Timestamp of last postsynaptic spike (seconds)
     * @details Used to compute time since last postsynaptic activity
     *          for short-term plasticity dynamics.
     */
    float lastPostSpikeTime;
    
    // ========== REWARD-MODULATED LEARNING ==========
    
    /**
     * @brief Eligibility trace for reward-modulated learning
     * @details Represents the synapse's readiness to undergo weight changes
     *          based on reward signals. This trace accumulates during learning
     *          and decays over time when no reward is received.
     */
    float eligibilityTrace;
    
    // ========== USE-DEPENDENT MODULATION ==========
    
    /**
     * @brief Synaptic efficacy (use-dependent modulation)
     * @details Represents how effectively the synapse can transmit signals
     *          based on usage history. This modulates synaptic strength
     *          independently of the baseline weight.
     */
    float efficacy;
    
    // ========== SYNAPTIC CONSTRAINTS ==========
    
    /**
     * @brief Minimum synaptic weight (-1.0)
     * @details Lower bound for synaptic weights to prevent excessive inhibition
     *          and ensure numerical stability.
     */
    static constexpr float MIN_WEIGHT = -1.0f;
    
    /**
     * @brief Maximum synaptic weight (1.0)
     * @details Upper bound for synaptic weights to prevent excessive excitation
     *          and ensure numerical stability.
     */
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // ========== SHORT-TERM PLASTICITY PARAMETERS ==========
    
    /**
     * @brief Short-term plasticity time constant for facilitation (ms)
     * @details Determines how quickly synaptic facilitation recovers.
     *          Higher values mean slower recovery and longer-lasting effects.
     */
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    
    /**
     * @brief Short-term plasticity time constant for depression (ms)
     * @details Determines how quickly synaptic depression recovers.
     *          Higher values mean slower recovery and longer-lasting effects.
     */
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    
    /**
     * @brief Maximum utilization factor for short-term plasticity
     * @details Upper bound for the short-term depression factor.
     *          Represents maximum synaptic resource utilization.
     */
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    /**
     * @brief Maximum spike history size for STDP
     * @details Limits memory usage for spike timing information.
     *          STDP algorithms require recent spike timing data.
     */
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    /**
     * @brief Constructs a synapse implementation with default parameters
     * @details Initializes synapse state with biologically plausible defaults:
     *          - Type: Excitatory (default)
     *          - Delay: 1 step (minimum realistic delay)
     *          - Eligibility trace: 0.0
     *          - Efficacy: 1.0 (normal transmission efficiency)
     *          - Short-term plasticity: Fully recovered (no depression, no facilitation)
     *          - Last spike times: -1.0 (no spikes yet)
     * @param id Synapse identifier
     * @param source Source neuron identifier
     * @param destination Destination neuron identifier
     */
    Impl(SynapseId id, NeuronId source, NeuronId destination)
        : id(id)
        , sourceNeuron(source)
        , destinationNeuron(destination)
        , weight(0.0f)
        , delay(1)
        , type(SynapseType::Excitatory)
        , preSpikeHistory()
        , postSpikeHistory()
        , plasticityFlags()
        , shortTermDepression(1.0f)
        , shortTermFacilitation(0.0f)
        , lastPreSpikeTime(-1.0f)
        , lastPostSpikeTime(-1.0f)
        , eligibilityTrace(0.0f)
        , efficacy(1.0f)
    {
        // Initialize spike history capacity
        preSpikeHistory.reserve(MAX_SPIKE_HISTORY);
        postSpikeHistory.reserve(MAX_SPIKE_HISTORY);
    }
};

/**
 * @brief Constructs a synapse with specified connection
 * @param id Unique identifier for the synapse
 * @param source Neuron ID of the presynaptic (source) neuron
 * @param destination Neuron ID of the postsynaptic (target) neuron
 * @details Creates a new synapse instance and validates the connection.
 *          The synapse is created in an uninitialized state with sensible defaults:
 *          - Weight: 0.0 (neutral)
 *          - Delay: 1 step (minimum realistic delay)
 *          - Type: Excitatory (default)
 *          - Short-term plasticity: Fully recovered
 *          - No spike history
 * @pre The synapse ID should be unique within its region
 * @pre Source and destination neuron IDs should be valid and distinct
 */
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl(id, source, destination)) {
    try {
        /**
         * @brief Validate synapse identifiers
         * @details Performs validation on all synapse identifiers to ensure
         *          they are valid and well-formed. Invalid identifiers are logged
         *          but don't throw exceptions to maintain robustness.
         */
        // Validate IDs
        auto idValidation = ValidationUtils::validateNeuronId(id, "Synapse::constructor");
        if (!idValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Synapse", "constructor", idValidation.getSummary(), "Synapse::constructor"
                )
            );
        }
        
        auto sourceValidation = ValidationUtils::validateNeuronId(source, "Synapse::constructor source");
        if (!sourceValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Synapse", "constructor", sourceValidation.getSummary(), "Synapse::constructor"
                )
            );
        }
        
        auto destValidation = ValidationUtils::validateNeuronId(destination, "Synapse::constructor destination");
        if (!destValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Synapse", "constructor", destValidation.getSummary(), "Synapse::constructor"
                )
            );
        }
        
        /**
         * @brief Initialize synapse identity
         * @details Stores the synapse identifiers in the implementation struct.
         *          These identifiers are used for synaptic routing and management.
         */
        pImpl->id = id;
        pImpl->sourceNeuron = source;
        pImpl->destinationNeuron = destination;
        
        /**
         * @brief Initialize default synaptic parameters
         * @details Sets initial values for all synaptic parameters. The constructor
         *          establishes a neutral starting state that can be modified through
         *          plasticity rules and external inputs.
         */
        pImpl->weight = 0.0f;
        pImpl->delay = 1;  // Default: 1 step delay
        pImpl->type = SynapseType::Excitatory;
        pImpl->eligibilityTrace = 0.0f;
        pImpl->efficacy = 1.0f;
        pImpl->shortTermDepression = 1.0f;
        pImpl->shortTermFacilitation = 0.0f;
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
        
        /**
         * @brief Validate initial weight
         * @details Validates the initial synaptic weight to ensure it's within
         *          acceptable bounds. Invalid weights are logged but clamped
         *          to prevent numerical issues.
         */
        auto weightValidation = ValidationUtils::validateWeight(pImpl->weight, "Synapse::constructor weight");
        if (!weightValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "constructor", weightValidation.getSummary(), "Synapse::constructor"
                )
            );
            pImpl->weight = ValidationUtils::clampWeight(pImpl->weight);
        }
        
    } catch (...) {
        delete pImpl;
        pImpl = nullptr;
        throw NeuralException("Failed to create synapse", ErrorSeverity::ERROR, "Synapse::constructor");
    }
}

/**
 * @brief Destructor cleans up synapse resources
 * @details Deletes the private implementation (pImpl) and cleans up
 *          any allocated resources. This ensures proper memory management.
 */
Synapse::~Synapse() {
    delete pImpl;
}

/**
 * @brief Move constructor - transfers ownership of the implementation
 * @param other The synapse being moved from
 * @note After move construction, the source synapse's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator - transfers ownership of the implementation
 * @param other The synapse being moved from
 * @return Synapse& Reference to this synapse (for chaining)
 * @note After assignment, the source synapse's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
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
    try {
        // Validate weight
        auto validation = ValidationUtils::validateWeight(weight, "Synapse::setWeight");
        if (!validation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "setWeight", validation.getSummary(), "Synapse::setWeight"
                )
            );
            weight = ValidationUtils::clampWeight(weight);
        }
        
        pImpl->weight = weight;
    } catch (...) {
        throw NeuralException("Failed to set synapse weight", ErrorSeverity::ERROR, "Synapse::setWeight");
    }
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
    try {
        // Validate timestamp
        if (std::isnan(timestamp) || timestamp < 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "recordPreSpike", "Invalid timestamp: " + std::to_string(timestamp), "Synapse::recordPreSpike"
                )
            );
            return;
        }
        
        pImpl->preSpikeHistory.push_back(timestamp);
        if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        }
    } catch (...) {
        throw NeuralException("Failed to record pre spike", ErrorSeverity::ERROR, "Synapse::recordPreSpike");
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    try {
        // Validate timestamp
        if (std::isnan(timestamp) || timestamp < 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "recordPostSpike", "Invalid timestamp: " + std::to_string(timestamp), "Synapse::recordPostSpike"
                )
            );
            return;
        }
        
        pImpl->postSpikeHistory.push_back(timestamp);
        if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        }
    } catch (...) {
        throw NeuralException("Failed to record post spike", ErrorSeverity::ERROR, "Synapse::recordPostSpike");
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    try {
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
    } catch (...) {
        throw NeuralException("Failed to clear synapse history", ErrorSeverity::ERROR, "Synapse::clearHistory");
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    try {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    } catch (...) {
        throw NeuralException("Failed to enable synapse plasticity", ErrorSeverity::ERROR, "Synapse::enablePlasticity");
    }
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    try {
        // Validate trace
        if (std::isnan(trace)) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "setEligibilityTrace", "NaN trace value", "Synapse::setEligibilityTrace"
                )
            );
            trace = 0.0f;
        }
        
        pImpl->eligibilityTrace = std::clamp(trace, -1.0f, 1.0f);
    } catch (...) {
        throw NeuralException("Failed to set eligibility trace", ErrorSeverity::ERROR, "Synapse::setEligibilityTrace");
    }
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
    try {
        // Validate efficacy
        if (std::isnan(efficacy)) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "setEfficacy", "NaN efficacy value", "Synapse::setEfficacy"
                )
            );
            efficacy = 1.0f;
        }
        
        pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    } catch (...) {
        throw NeuralException("Failed to set synapse efficacy", ErrorSeverity::ERROR, "Synapse::setEfficacy");
    }
}

void Synapse::step(Timestamp currentTime) {
    try {
        // Validate currentTime
        if (std::isnan(currentTime) || currentTime < 0.0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "step", "Invalid currentTime: " + std::to_string(currentTime), "Synapse::step"
                )
            );
            return;
        }
        
        TimestepDuration dt = 0.001;  // 1ms timestep
        
        // Decay short-term facilitation (Tsodyks-Markram model)
        if (pImpl->lastPreSpikeTime >= 0.0f) {
            float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
            if (std::isnan(timeSincePre) || timeSincePre < 0.0) {
                ErrorReporter::getInstance().log(
                    std::make_shared<NeuralBoundaryError>(
                        "Synapse", "step", "Invalid timeSincePre: " + std::to_string(timeSincePre), "Synapse::step"
                    )
                );
                timeSincePre = 0.0f;
            }
            
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
        
    } catch (const NeuralException& e) {
        throw;
    } catch (...) {
        throw NeuralException("Synapse step failed", ErrorSeverity::ERROR, "Synapse::step");
    }
}

void Synapse::reset() {
    try {
        pImpl->weight = 0.0f;
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->eligibilityTrace = 0.0f;
        pImpl->efficacy = 1.0f;
    } catch (...) {
        throw NeuralException("Synapse reset failed", ErrorSeverity::ERROR, "Synapse::reset");
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
        
        // Validate weights
        auto weightValidation = ValidationUtils::validateWeight(pImpl->weight, "Synapse::initializeRandom weight");
        if (!weightValidation.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryError>(
                    "Synapse", "initializeRandom", weightValidation.getSummary(), "Synapse::initializeRandom"
                )
            );
            pImpl->weight = ValidationUtils::clampWeight(pImpl->weight);
        }
        
        // Random delay: 1-5 steps (1-5ms at 1ms timestep)
        if (!rng.isValid()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "Synapse", "initializeRandom", "Invalid random generator", "Synapse::initializeRandom"
                )
            );
            pImpl->delay = 1;
        } else {
            pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
        }
        
        // Initialize short-term plasticity state
        pImpl->shortTermDepression = 1.0f;  // Fully recovered
        pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
        
        // Initialize eligibility trace to 0
        pImpl->eligibilityTrace = 0.0f;
        
        // Initialize last spike times
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
        
    } catch (const NeuralException& e) {
        throw;
    } catch (...) {
        throw NeuralException("Synapse random initialization failed", ErrorSeverity::ERROR, "Synapse::initializeRandom");
    }
}

} // namespace nlm
