#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @brief Implementation details for the Synapse class
 * 
 * This struct contains all implementation details for synaptic connections
 * in the neural network, including weight management, spike history tracking,
 * short-term plasticity mechanisms, and reward-modulated learning capabilities.
 * 
 * The synapse implementation supports:
 * - Static weight bounds (-1.0 to +1.0) for stability
 * - Synaptic delays (1-5 simulation steps)
 * - Spike history tracking for STDP calculations
 * - Short-term plasticity (facilitation and depression)
 * - Eligibility traces for reward-modulated learning
 * - Synaptic efficacy modulation based on usage patterns
 */

struct Synapse::Impl {
    SynapseId id;                          ///< Unique identifier for this synapse
    NeuronId sourceNeuron;                 ///< Neuron ID of pre-synaptic neuron
    NeuronId destinationNeuron;            ///< Neuron ID of post-synaptic neuron
    SynapticWeight weight;                 ///< Current synaptic strength (-1.0 to +1.0)
    Delay delay;                          ///< Synaptic delay in simulation steps (1-5)
    
    // Synapse type determines sign and function
    SynapseType type;                      ///< Excitatory, Inhibitory, or Modulatory
    
    // Spike history for STDP calculations - stores precise spike timing
    std::vector<Timestamp> preSpikeHistory;    ///< Times of pre-synaptic spikes (ms)
    std::vector<Timestamp> postSpikeHistory;   ///< Times of post-synaptic spikes (ms)
    
    // Plasticity control flags for different learning mechanisms
    PlasticityFlags plasticityFlags;         ///< Enables Hebbian, STDP, and reward-modulated learning
    
    // Short-term plasticity state variables (Tsodyks-Markram model)
    float shortTermDepression;              ///< Utilization factor (0-1), decreases with activity
    float shortTermFacilitation;            ///< Facilitation factor, increases with burst firing
    float lastPreSpikeTime;                 ///< Timestamp of last pre-synaptic spike
    float lastPostSpikeTime;                ///< Timestamp of last post-synaptic spike
    
    // Eligibility trace for reward-modulated learning (e.g., dopamine)
    float eligibilityTrace;                 ///< Accumulates reward prediction error signals
    
    // Synaptic efficacy - use-dependent modulation of plasticity strength
    float efficacy;                        ///< Multiplicative factor for learning rate (0-2)
    
    // Synaptic weight bounds ensure network stability
    static constexpr float MIN_WEIGHT = -1.0f;    ///< Absolute lower weight bound
    static constexpr float MAX_WEIGHT = 1.0f;    ///< Absolute upper weight bound
    
    // Short-term plasticity time constants (biological values in milliseconds)
    static constexpr float STP_FACILITATION_TAU = 100.0f;  ///< Facilitation decay time constant
    static constexpr float STP_DEPRESSION_TAU = 200.0f;    ///< Depression recovery time constant  
    static constexpr float STP_U_MAX = 1.0f;               ///< Maximum utilization factor
    
    // Maximum spike history length for STDP calculations
    static constexpr size_t MAX_SPIKE_HISTORY = 100;      ///< Stores ~1 second of spike events at 1kHz
};

/**
 * @brief Constructor for synapse with source and destination neurons
 * 
 * Initializes a synaptic connection between two neurons with biologically
 * plausible default parameters. The synapse starts with zero weight and
 * appropriate initial conditions for short-term plasticity.
 * 
 * @param id Unique identifier for this synapse
 * @param source Source neuron ID (pre-synaptic)
 * @param destination Destination neuron ID (post-synaptic)
 */
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    // Set anatomical identifiers
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    
    // Initialize synaptic strength to neutral value
    pImpl->weight = 0.0f;
    
    // Default physiological delay (1-5 simulation steps)
    pImpl->delay = 1;  // Default: 1 step delay
    
    // Default to excitatory connection
    pImpl->type = SynapseType::Excitatory;
    
    // Initialize reward-modulated learning components
    pImpl->eligibilityTrace = 0.0f;  // No eligibility at start
    pImpl->efficacy = 1.0f;          // Full efficacy for new synapses
    
    // Initialize short-term plasticity state (fully recovered)
    pImpl->shortTermDepression = 1.0f;    // Fully recovered depression state
    pImpl->shortTermFacilitation = 0.0f; // No initial facilitation
    
    // Record no spikes yet
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

/**
 * @brief Destructor - cleans up implementation memory
 * 
 * Ensures proper cleanup of the synapse implementation object.
 */
Synapse::~Synapse() = default;

/**
 * @brief Move constructor - transfers implementation ownership efficiently
 * 
 * Implements Pimpl idiom pattern for efficient copying without compilation
 * dependencies. Transfers ownership from source synapse to new synapse.
 * 
 * @param other Source synapse to move implementation from
 */
Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator - transfers implementation ownership
 * 
 * Efficient transfer of implementation ownership with proper cleanup.
 * Prevents memory leaks and satisfies Pimpl idiom requirements.
 * 
 * @param other Source synapse to move implementation from
 * @return Reference to this synapse for method chaining
 */
Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;  // Clean up existing implementation
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * @brief Gets the unique synapse identifier
 * 
 * @return SynapseId The unique ID for this synapse
 */
SynapseId Synapse::getId() const {
    return pImpl->id;
}

/**
 * @brief Gets the source neuron ID (pre-synaptic neuron)
 * 
 * @return NeuronId ID of the neuron sending signals through this synapse
 */
NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

/**
 * @brief Gets the destination neuron ID (post-synaptic neuron)
 * 
 * @return NeuronId ID of the neuron receiving signals through this synapse
 */
NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

/**
 * @brief Gets the current synaptic weight
 * 
 * @return SynapticWeight Current weight (-1.0 to +1.0)
 */
SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

/**
 * @brief Sets the synaptic weight with bounds checking
 * 
 * Ensures synaptic weights remain within biologically plausible bounds
 * to maintain network stability and prevent runaway excitation or inhibition.
 * 
 * @param weight New synaptic weight value
 */
void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

/**
 * @brief Adds to synaptic weight with clamping to maintain stability
 * 
 * Used by plasticity rules (STDP, Hebbian) to modify synaptic strength.
 * The weight is clamped to MIN_WEIGHT/MAX_WEIGHT to ensure stability.
 * 
 * @param delta Change in synaptic weight (positive for potentiation, negative for depression)
 */
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
 * @brief Gets the synaptic delay in simulation steps
 * 
 * Synaptic delay accounts for transmission time between neurons,
 * a biologically realistic property that affects spike timing.
 * 
 * @return Delay Synaptic delay (typically 1-5 simulation steps)
 */
Delay Synapse::getDelay() const {
    return pImpl->delay;
}

/**
 * @brief Sets the synaptic delay
 * 
 * @param delay New synaptic delay in simulation steps
 */
void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

/**
 * @brief Gets the synapse type (Excitatory, Inhibitory, Modulatory)
 * 
 * @return SynapseType The functional type of this synapse
 */
SynapseType Synapse::getType() const {
    return pImpl->type;
}

/**
 * @brief Sets the synapse type
 * 
 * @param type The new synapse type
 */
void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

/**
 * @brief Checks if this is an excitatory synapse
 * 
 * @return bool true if synapse is excitatory, false otherwise
 */
bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

/**
 * @brief Checks if this is an inhibitory synapse
 * 
 * @return bool true if synapse is inhibitory, false otherwise
 */
bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

/**
 * @brief Records a pre-synaptic spike for STDP calculations
 * 
 * Stores spike timing information for spike-timing dependent plasticity.
 * Maintains sliding window of recent spikes to compute pairwise spike timing differences.
 * 
 * @param timestamp Simulation time when pre-synaptic spike occurred (ms)
 */
void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

/**
 * @brief Records a post-synaptic spike for STDP calculations
 * 
 * Stores spike timing information for spike-timing dependent plasticity.
 * Maintains sliding window of recent spikes to compute pairwise spike timing differences.
 * 
 * @param timestamp Simulation time when post-synaptic spike occurred (ms)
 */
void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

/**
 * @brief Gets the history of pre-synaptic spikes
 * 
 * @return const std::vector<Timestamp>& Vector of pre-synaptic spike times
 */
const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

/**
 * @brief Gets the history of post-synaptic spikes
 * 
 * @return const std::vector<Timestamp>& Vector of post-synaptic spike times
 */
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

/**
 * @brief Clears all spike history records
 * 
 * Used for experiment restarts or when STDP calculations need to be reset.
 */
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

/**
 * @brief Accesses reference to plasticity control flags
 * 
 * Allows configuration of which plasticity mechanisms are active for this synapse.
 * Note: Individual neurons also have plasticity flags that modulate synapse behavior.
 * 
 * @return PlasticityFlags& Reference to plasticity configuration flags
 */
PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
 * @brief Enables specific plasticity mechanisms for this synapse
 * 
 * Configure which learning rules affect this synapse. Note that both synapse-level
 * and neuron-level plasticity flags must be enabled for plasticity to occur.
 * 
 * @param hebbian Enable Hebbian (Hebb's rule) learning
 * @param stdp Enable Spike-Timing-Dependent Plasticity
 * @param rewardModulated Enable reward-modulated learning via dopamine
 */
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
 * @brief Gets reference to constant plasticity flags
 * 
 * Const version for read-only access to plasticity configuration.
 * 
 * @return const PlasticityFlags& Constant reference to plasticity flags
 */
const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

/**
 * @brief Gets the current eligibility trace value
 * 
 * Eligibility traces accumulate reward prediction error signals over time,
 * bridging the gap between delayed rewards and immediate actions in reinforcement learning.
 * 
 * @return float Current eligibility trace value
 */
float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

/**
 * @brief Sets the eligibility trace value
 * 
 * @param trace New eligibility trace value
 */
void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

/**
 * @brief Decays the eligibility trace toward zero
 * 
 * Implements the decay of reward prediction error signals over time,
 * typically exponential decay with time constant around 100-200ms.
 * 
 * @param decayRate Decay rate per timestep (0.0 to 1.0)
 */
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

/**
 * @brief Gets the current synaptic efficacy
 * 
 * Synaptic efficacy modulates the strength of plasticity based on usage history.
 * Frequently used synapses may have enhanced learning capabilities.
 * 
 * @return float Synaptic efficacy (0.0 to 2.0)
 */
float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

/**
 * @brief Sets the synaptic efficacy with bounds checking
 * 
 * @param efficacy New efficacy value (will be clamped to [0.0, 2.0])
 */
void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

/**
 * @brief Executes one timestep of synaptic dynamics
 * 
 * This is the core synaptic update function called each simulation step.
 * It handles:
 * 1. Decay of short-term facilitation state
 * 2. Decay of short-term depression state  
 * 3. Decay of eligibility trace for reward learning
 * 4. Weight bounds clamping
 * 
 * This function is called by the SpikeSystem each timestep for all synapses.
 * 
 * @param currentTime Current simulation time in milliseconds
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
 * @brief Resets synapse to initial conditions
 * 
 * Used for experiment restarts or when synapse properties need to be
 * reinitialized. Resets weight, spike history, and plasticity state
 * but preserves anatomical connectivity (source/destination neurons).
 * 
 * @see initializeRandom() for biologically realistic initialization
 */
void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

/**
 * @brief Randomly initializes synapse parameters based on type
 * 
 * Creates biologically plausible initial synaptic properties based on
 * the synapse type (excitatory, inhibitory, or modulatory). This provides
 * realistic starting conditions that reflect known biological differences
 * between synapse types.
 * 
 * Biological realism:
 * - Excitatory synapses: small positive weights (0.1-0.4)
 * - Inhibitory synapses: negative weights (-0.1 to -0.4)
 * - Other types: neutral weights (-0.1 to +0.1)
 * - Random delays (1-5 simulation steps)
 * - Initial efficacy based on type-specific constraints
 * 
 * @param rng Random number generator for stochastic initialization
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
    pImpl->shortTermFacilitation = 0.0f; // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
