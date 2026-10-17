// Include all necessary headers for NLM operation
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <limits>
#include <cfloat>
#include <deque>

namespace nlm {

// Forward declaration of Impl for Synapse
struct Synapse::Impl {
    // Core identity
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    
    // Synaptic properties
    SynapticWeight weight;
    Delay delay;
    SynapseType type;
    float eligibilityTrace;
    float efficacy;
    float shortTermDepression;
    float shortTermFacilitation;
    float lastPreSpikeTime;
    float lastPostSpikeTime;
    
    // History management for STDP
    std::deque<Timestamp> preSpikeHistory;
    std::deque<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Constants
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;
    static constexpr SynapticWeight MIN_WEIGHT = -1.0f;
    static constexpr SynapticWeight MAX_WEIGHT = 1.0f;
    static constexpr float STP_FACILITATION_TAU = 5.0f;      // ms
    static constexpr float STP_DEPRESSION_TAU = 50.0f;       // ms
};

/**
     * @brief Create a synaptic connection between neurons
     * @details Creates a synapse implementing biological synaptic transmission with
     *          delays and short-term plasticity. Synapses are the fundamental
     *          communication channels in the neural network.
     * @param id Unique identifier for the synapse
     * @param source Source neuron ID
     * @param destination Target neuron ID
     */
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
    pImpl->preSpikeHistory.reserve(Impl::MAX_SPIKE_HISTORY);
    pImpl->postSpikeHistory.reserve(Impl::MAX_SPIKE_HISTORY);
}

/**
     * @brief Default destructor
     */
Synapse::~Synapse() = default;

/**
     * @brief Move constructor
     * @details Transfer ownership of implementation from another synapse
     * @param other Synapse to transfer from
     */
Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
     * @brief Copy constructor (deleted)
     */
Synapse::Synapse(const Synapse&) = delete;

/**
     * @brief Copy assignment operator (deleted)
     */
Synapse& Synapse::operator=(const Synapse&) = delete;

/**
     * @brief Move assignment operator
     * @details Transfer ownership of implementation from another synapse
     * @param other Synapse to transfer from
     * @return Reference to this synapse
     */
Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
     * @brief Get synapse identifier
     * @return Unique identifier
     */
SynapseId Synapse::getId() const {
    return pImpl->id;
}

/**
     * @brief Get source neuron identifier
     * @return ID of neuron sending the signal
     */
NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

/**
     * @brief Get destination neuron identifier
     * @return ID of neuron receiving the signal
     */
NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

/**
     * @brief Get current synaptic weight
     * @details Synaptic weight determines connection strength. Positive weights
     *          are excitatory, negative weights are inhibitory.
     * @return Synaptic weight
     */
SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

/**
     * @brief Set synaptic weight
     * @details Directly sets the weight of the connection. Synapses have
     *          bounds (-1.0 to 1.0) to prevent instability.
     * @param weight New synaptic weight
     */
void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

/**
     * @brief Adjust synaptic weight by delta
     * @details Useful for plasticity rules that need to modify weights incrementally.
     *          Automatically clamps to valid range.
     * @param delta Amount to add to current weight
     */
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
     * @brief Get synaptic transmission delay
     * @details Delay represents biological synaptic transmission time.
     *          Typical values are 1-5 simulation steps.
     * @return Delay in simulation steps
     */
Delay Synapse::getDelay() const {
    return pImpl->delay;
}

/**
     * @brief Set synaptic transmission delay
     * @param delay Delay in simulation steps
     */
void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

/**
     * @brief Get synapse type
     * @return Synapse type (excitatory, inhibitory, neuromodulatory)
     */
SynapseType Synapse::getType() const {
    return pImpl->type;
}

/**
     * @brief Set synapse type
     * @param type Synapse type
     */
void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

/**
     * @brief Check if synapse is excitatory
     * @return true if synapse type is excitatory
     */
bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

/**
     * @brief Check if synapse is inhibitory
     * @return true if synapse type is inhibitory
     */
bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

/**
     * @brief Record pre-synaptic spike
     * @details STDP uses spike timing to modify synaptic strength. Pre-synaptic
     *          spikes mark when the sending neuron fired.
     * @param timestamp Time of spike
     */
void Synapse::recordPreSpike(Timestamp timestamp) {
    // Use circular buffer approach with deque for O(1) operations
    if (pImpl->preSpikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.pop_front();
    }
    pImpl->preSpikeHistory.push_back(timestamp);
}

/**
     * @brief Record post-synaptic spike
     * @details STDP uses spike timing to modify synaptic strength. Post-synaptic
     *          spikes mark when the receiving neuron fired.
     * @param timestamp Time of spike
     */
void Synapse::recordPostSpike(Timestamp timestamp) {
    // Use circular buffer approach with deque for O(1) operations
    if (pImpl->postSpikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.pop_front();
    }
    pImpl->postSpikeHistory.push_back(timestamp);
}

/**
     * @brief Get pre-synaptic spike history
     * @return Vector of spike times for STDP computation
     */
const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

/**
     * @brief Get post-synaptic spike history
     * @return Vector of spike times for STDP computation
     */
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

/**
     * @brief Clear spike history
     * @details Used for resetting STDP traces or starting new simulation episodes.
     */
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

/**
     * @brief Get plasticity flags
     * @details Configures which plasticity rules apply to this synapse
     * @return Reference to PlasticityFlags structure
     */
const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

/**
     * @brief Get plasticity flags (non-const version)
     * @details Non-const version for modifying plasticity settings
     * @return Reference to PlasticityFlags structure
     */
PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
     * @brief Enable specific plasticity rules
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated learning
     */
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
     * @brief Get eligibility trace for reward-modulated learning
     * @details Tracks synaptic contribution to long-term changes for reward-based
     *          learning. Required for dopamine-modulated plasticity.
     * @return Current eligibility trace value
     */
float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

/**
     * @brief Set eligibility trace
     * @param trace Eligibility trace value
     */
void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

/**
     * @brief Decay eligibility trace over time
     * @details Implements forgetting mechanism for reward-modulated learning.
     *          Eligibility traces decay exponentially.
     * @param decayRate Fraction to decay per timestep
     */
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

/**
     * @brief Get synaptic efficacy (use-dependent modulation)
     * @details Represents how effectively the synapse transmits signals based
     *          on recent activity patterns. Implements short-term plasticity.
     * @return Efficacy value (typically 0.0 to 2.0)
     */
float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

/**
     * @brief Set synaptic efficacy
     * @details Modifies synaptic transmission probability based on activity history.
     * @param efficacy Efficacy value
     */
void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

/**
     * @brief Execute one simulation step
     * @details Updates synaptic state including short-term plasticity, eligibility
     *          trace decay, and weight bounds. Called every simulation step.
     * @param currentTime Current simulation time
     */
void Synapse::step(Timestamp currentTime) {
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
}

/**
     * @brief Reset synapse to initial state
     * @details Clears all synaptic state for new simulation episode.
     */
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

/**
     * @brief Initialize synapse with biologically realistic parameters
     * @details Sets up synapse properties based on type and biological constraints.
     * @param rng Random number generator for reproducible initialization
     */
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
    
    // Clear history
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

} // namespace nlm
