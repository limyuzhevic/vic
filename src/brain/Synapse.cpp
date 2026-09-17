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

/**
     * @brief Constructs a synapse with specified source and destination neurons
     * 
     * Initializes a new synapse connection between two neurons with default parameters:
     * - Zero initial weight
     * - Default delay of 1 simulation step
     * - Excitatory synapse type
     * - Zero eligibility trace and efficacy
     * - Reset short-term plasticity state
     * - Invalid last spike times
     * 
     * @param id Unique identifier for the synapse
     * @param source Neuron ID of the presynaptic neuron
     * @param destination Neuron ID of the postsynaptic neuron
     * 
     * @ingroup Synapse
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
}

/**
     * @brief Destroys the synapse and cleans up resources
     * 
     * The destructor ensures proper cleanup of the pImpl instance pointer.
     */
Synapse::~Synapse() = default;

/**
     * @brief Move constructor for Synapse
     * 
     * Transfers ownership of the internal implementation from another Synapse instance.
     * The source Synapse's implementation pointer is set to nullptr to prevent double deletion.
     * 
     * @param other Synapse instance to transfer ownership from
     */
Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
     * @brief Move assignment operator for Synapse
     * 
     * Transfers ownership from another Synapse instance and cleans up the current instance.
     * Follows the rule of four with move semantics support.
     * 
     * @param other Synapse instance to transfer ownership from
     * @return Reference to this Synapse instance
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
     * @brief Get the unique identifier of the synapse
     * 
     * @return SynapseId Unique identifier used to reference this synapse
     * 
     * @ingroup Synapse
     */
SynapseId Synapse::getId() const {
    return pImpl->id;
}

/**
     * @brief Get the source neuron ID (presynaptic neuron)
     * 
     * @return NeuronId ID of the neuron that sends spikes through this synapse
     * 
     * @ingroup Synapse
     */
NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

/**
     * @brief Get the destination neuron ID (postsynaptic neuron)
     * 
     * @return NeuronId ID of the neuron that receives spikes through this synapse
     * 
     * @ingroup Synapse
     */
NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

/**
     * @brief Get the current synaptic weight
     * 
     * The synaptic weight determines the strength of the connection between
     * source and destination neurons. Positive weights indicate excitatory
     * connections, negative weights indicate inhibitory connections.
     * 
     * @return SynapticWeight Current synaptic weight (typically between -1.0 and 1.0)
     * 
     * @ingroup Synapse
     */
SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

/**
     * @brief Set the synaptic weight
     * 
     * Changes the synaptic strength. The weight is automatically clamped to
     * prevent instability (MIN_WEIGHT = -1.0f, MAX_WEIGHT = 1.0f).
     * 
     * @param weight New synaptic weight value
     * 
     * @ingroup Synapse
     */
void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

/**
     * @brief Add delta to the synaptic weight
     * 
     * Increments the synaptic weight by the specified delta. Used by plasticity
     * mechanisms like STDP to implement weight changes.
     * 
     * @param delta Weight change amount (positive for potentiation, negative for depression)
     * 
     * @ingroup Synapse
     */
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
     * @brief Get the synaptic delay in simulation steps
     * 
     * Synaptic delay represents the time delay between spike generation in the
     * presynaptic neuron and activation of the postsynaptic neuron. This is crucial
     * for realistic spike timing and temporal dynamics.
     * 
     * @return Delay Number of simulation steps to delay spike transmission
     * 
     * @ingroup Synapse
     */
Delay Synapse::getDelay() const {
    return pImpl->delay;
}

/**
     * @brief Set the synaptic delay
     * 
     * Changes the synaptic transmission delay. Delay affects spike timing and
     * contributes to temporal coordination in neural circuits.
     * 
     * @param delay New delay value in simulation steps
     * 
     * @ingroup Synapse
     */
void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

/**
     * @brief Get the synapse type
     * 
     * Synapse type determines whether the connection is excitatory or inhibitory,
     * which affects its computational role in the neural network.
     * 
     * @return SynapseType Type of synapse (Excitatory, Inhibitory, etc.)
     * 
     * @ingroup Synapse
     */
SynapseType Synapse::getType() const {
    return pImpl->type;
}

/**
     * @brief Set the synapse type
     * 
     * Changes the synapse from excitatory to inhibitory or vice versa. This affects
     * the sign of its influence on the postsynaptic neuron.
     * 
     * @param type New synapse type
     * 
     * @ingroup Synapse
     */
void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

/**
     * @brief Check if synapse is excitatory
     * 
     * Determines if the synapse has positive weight, indicating it will increase
     * the postsynaptic neuron's membrane potential.
     * 
     * @return bool true if synapse is excitatory, false otherwise
     * 
     * @ingroup Synapse
     */
bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

/**
     * @brief Check if synapse is inhibitory
     * 
     * Determines if the synapse has negative weight, indicating it will decrease
     * the postsynaptic neuron's membrane potential.
     * 
     * @return bool true if synapse is inhibitory, false otherwise
     * 
     * @ingroup Synapse
     */
bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

/**
     * @brief Record a presynaptic spike timestamp for plasticity
     * 
     * Stores the timing of presynaptic spikes in a circular buffer for use by
     * spike-timing dependent plasticity (STDP) mechanisms. The most recent spikes
     * are retained for learning.
     * 
     * @param timestamp Time when the presynaptic spike occurred (in ms)
     * 
     * @ingroup Synapse
     */
void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

/**
     * @brief Record a postsynaptic spike timestamp for plasticity
     * 
     * Stores the timing of postsynaptic spikes in a circular buffer for use by
     * spike-timing dependent plasticity (STDP) mechanisms. The most recent spikes
     * are retained for learning.
     * 
     * @param timestamp Time when the postsynaptic spike occurred (in ms)
     * 
     * @ingroup Synapse
     */
void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

/**
     * @brief Get the presynaptic spike history
     * 
     * Returns the timestamps of recent presynaptic spikes, used by plasticity
     * rules to compute weight changes based on spike timing.
     * 
     * @return const std::vector<Timestamp>& Vector of presynaptic spike timestamps
     * 
     * @ingroup Synapse
     */
const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

/**
     * @brief Get the postsynaptic spike history
     * 
     * Returns the timestamps of recent postsynaptic spikes, used by plasticity
     * rules to compute weight changes based on spike timing.
     * 
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike timestamps
     * 
     * @ingroup Synapse
     */
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

/**
     * @brief Clear spike history for both pre- and post-synaptic events
     * 
     * Resets the plasticity history buffers, effectively forgetting recent spike timing
     * information. Used for synaptic reset or when plasticity should be disabled.
     * 
     * @ingroup Synapse
     */
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

/**
     * @brief Get reference to plasticity flags structure
     * 
     * Returns a reference to the PlasticityFlags structure that controls which
     * plasticity mechanisms are enabled for this synapse (Hebbian, STDP, reward-modulated).
     * 
     * @return PlasticityFlags& Reference to plasticity flags
     * 
     * @ingroup Synapse
     */
PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
     * @brief Enable or disable specific plasticity rules for this synapse
     * 
     * Configures which plasticity mechanisms should be applied to this synapse.
     * Each rule represents a different learning algorithm based on different
     * temporal or reward-based signals.
     * 
     * @param hebbian Enable Hebbian (simultaneous spike) learning
     * @param stdp Enable spike-timing dependent plasticity (STDP)
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @ingroup Synapse
     */
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
     * @brief Get const reference to plasticity flags structure
     * 
     * Const version that returns a const reference to the plasticity flags.
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags
     * 
     * @ingroup Synapse
     */
const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

/**
     * @brief Get the eligibility trace value
     * 
     * The eligibility trace represents the accumulated potential for learning
     * that persists after a spike event. Used in reward-modulated learning
     * to bridge the delay between action and reward.
     * 
     * @return float Current eligibility trace value (typically between -1.0 and 1.0)
     * 
     * @ingroup Synapse
     */
float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

/**
     * @brief Set the eligibility trace value
     * 
     * Manually sets the eligibility trace, typically used when implementing
     * reward-modulated learning algorithms.
     * 
     * @param trace New eligibility trace value
     * 
     * @ingroup Synapse
     */
void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

/**
     * @brief Decay the eligibility trace by a factor
     * 
     * Implements the decay of eligibility trace over time, representing the
     * fading of learning potential when rewards are not immediately received.
     * 
     * @param decayRate Decay factor (0.0 to 1.0, where 1.0 = 100% decay)
     * 
     * @ingroup Synapse
     */
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

/**
     * @brief Get the synaptic efficacy value
     * 
     * Synaptic efficacy represents the use-dependent modulation of synaptic strength,
     * often related to short-term plasticity effects like facilitation and depression.
     * 
     * @return float Current synaptic efficacy (typically between 0.0 and 2.0)
     * 
     * @ingroup Synapse
     */
float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

/**
     * @brief Set the synaptic efficacy value
     * 
     * Changes the synaptic efficacy, which modulates how effectively the synapse
     * transmits signals. This is use-dependent and can be dynamically adjusted.
     * 
     * @param efficacy New efficacy value, will be clamped to [0.0, 2.0]
     * 
     * @ingroup Synapse
     */
void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

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
    
    /**
     * @brief Update synapse state for one simulation step
     * 
     * Implements real synaptic dynamics including:
     * - Decay of short-term plasticity state (facilitation and depression)
     * - Decay of eligibility trace for reward-modulated learning
     * - Clamping of synaptic weight to prevent instability
     * 
     * This represents the biological reality that synapses change continuously,
     * not just at spike times. Short-term plasticity affects transmission
     * properties on timescales of milliseconds to seconds.
     * 
     * @param currentTime Current simulation time (in ms)
     * 
     * @ingroup Synapse
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
     * 
     * Clears all synaptic state variables, returning the synapse to its
     * initial configuration after construction. Used for synaptic reset
     * or when synaptic changes should be undone.
     * 
     * @details Resets:
     * - Weight to 0.0
     * - Spike history buffers (pre and post)
     * - Eligibility trace to 0.0
     * - Efficacy to 1.0
     * - Short-term plasticity state
     * - Last spike times
     * 
     * @ingroup Synapse
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
     * @brief Initialize synapse with random parameters based on synapse type
     * 
     * Sets up initial synaptic parameters using biological constraints and
     * random variations:
     * - Excitatory synapses: small positive weights (0.1-0.4)
     * - Inhibitory synapses: negative weights (-0.1 to -0.4)
     * - Other types: small random weights (-0.1 to 0.1)
     * - Random delays (1-5 simulation steps)
     * - Appropriate short-term plasticity state
     * 
     * This is called during network initialization to create realistic
     * connectivity patterns.
     * 
     * @param rng Random number generator for stochastic initialization
     * 
     * @ingroup Synapse
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
}

} // namespace nlm
