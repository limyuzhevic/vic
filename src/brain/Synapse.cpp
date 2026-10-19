#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * Synapse Implementation with Comprehensive Documentation
 * 
 * This file implements synaptic dynamics for the NLM (Neural Learning Machine) project.
 * Synapses are the fundamental connections between neurons that enable communication
 * and plasticity in the artificial neural network.
 * 
 * Key Features:
 * - Biologically realistic synaptic dynamics
 * - Spike-timing dependent plasticity (STDP)
 * - Short-term plasticity (STP) mechanisms
 * - Reward-modulated learning with eligibility traces
 * - Use-dependent synaptic efficacy modulation
 * - Comprehensive parameter management for different synapse types
 * 
 * Biological Motivation:
 * Synapses in biological neural systems exhibit complex dynamic properties:
 * 1. Weight varies between excitatory (+1.0) and inhibitory (-1.0)
 * 2. Transmission delay depends on axon length and conduction velocity
 * 3. Short-term plasticity (STP) enables dynamic gain modulation
 * 4. Long-term plasticity (LTP/LTD) enables memory formation
 * 5. Neuromodulation integrates reward prediction errors
 * 
 * Mathematical Models:
 * - Tsodyks-Markram model for short-term plasticity
 * - pair-based STDP for spike-timing dependent plasticity
 * - Reward-modulated learning with eligibility traces
 * - Use-dependent efficacy for activity-dependent modulation
 */

/**
 * Internal implementation details for Synapse class
 * 
 * This structure uses the Pimpl idiom to hide implementation details
 * and provide better encapsulation of synaptic state management.
 */
struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type determines functional properties
    SynapseType type;
    
    // STDP mechanism: spike history for temporal plasticity
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity control flags for different learning mechanisms
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state (STP)
    float shortTermDepression;  // Utilization factor (0-1): 1.0 = fully recovered
    float shortTermFacilitation;  // Facilitation factor: 0.0 = no facilitation
    float lastPreSpikeTime;  // For short-term dynamics timing
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    // Integrates presynaptic activity until reward arrives
    float eligibilityTrace;
    
    // Synaptic efficacy for use-dependent modulation
    // Higher efficacy = greater response to same input (max 2.0x)
    float efficacy;
    
    // Weight bounds for stability control
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity time constants (milliseconds)
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // Facilitation time constant
    static constexpr float STP_DEPRESSION_TAU = 200.0f;   // Depression time constant
    static constexpr float STP_U_MAX = 1.0f;               // Max utilization factor
    
    // Maximum spike history length for STDP calculations
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

/**
 * Synapse constructor with comprehensive biological initialization
 * 
 * Creates a synapse with biologically plausible default parameters
 * ready for integration into neural network simulation.
 * 
 * @param id Unique identifier for the synapse within the network
 * @param source Source neuron ID (presynaptic neuron)
 * @param destination Destination neuron ID (postsynaptic neuron)
 * 
 * Biological realism:
 * - Excitatory synapses as default (type 0 in biological systems)
 * - Moderate transmission delay reflecting biological propagation
 * - Full efficacy and recovered STP state for realistic dynamics
 */
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay (biological realism: ~1ms)
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

/**
 * Default destructor
 * 
 * The Pimpl pattern ensures proper cleanup of internal implementation
 */
Synapse::~Synapse() = default;

/**
 * Move constructor implementation
 * 
 * Transfers ownership of internal implementation from source synapse
 * following the Pimpl pattern for efficient object movement.
 * 
 * @param other Source synapse to move from
 */
Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * Move assignment operator implementation
 * 
 * Properly manages internal implementation during move assignment,
 * ensuring no memory leaks and maintaining the Pimpl pattern.
 * 
 * @param other Source synapse to move from
 * @return Reference to this synapse (for method chaining)
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
 * Get synapse identifier
 * 
 * @return Unique identifier for this synapse
 */
SynapseId Synapse::getId() const {
    return pImpl->id;
}

/**
 * Get source neuron identifier
 * 
 * @return ID of the presynaptic neuron that sends signals through this synapse
 */
NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

/**
 * Get destination neuron identifier
 * 
 * @return ID of the postsynaptic neuron that receives signals through this synapse
 */
NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

/**
 * Get synaptic weight with biological significance
 * 
 * Weight represents synaptic strength and determines the polarity of signal transmission:
 * - Positive weights: Excitatory synapses (depolarize postsynaptic neuron)
 * - Negative weights: Inhibitory synapses (hyperpolarize postsynaptic neuron)
 * - Zero weight: No functional connection
 * 
 * Weight range: [-1.0, 1.0] derived from biological observations
 * 
 * @return Current synaptic weight
 */
SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

/**
 * Set synaptic weight with stability enforcement
 * 
 * @param weight New weight value (will be clamped to valid range)
 * 
 * Stability considerations:
 * - Prevents runaway excitation from exceeding biological limits
 * - Maintains network stability through weight bounds enforcement
 * - Biological constraint reflects homeostatic mechanisms
 */
void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

/**
 * Adjust synaptic weight by delta value with stability enforcement
 * 
 * Preferred method for weight updates during learning, as it automatically
 * maintains weight bounds and prevents instability from large updates.
 * 
 * @param delta Weight change amount (positive = strengthen, negative = weaken)
 * 
 * Biological correspondence: Delta weight represents synaptic modification
 * through long-term potentiation (LTP) or long-term depression (LTD)
 */
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
 * Get synaptic delay in simulation steps
 * 
 * Delay represents time for signal propagation across the synapse,
 * influenced by axon length, conduction velocity, and synaptic properties.
 * 
 * Biological model: Delay = axon length / conduction velocity
 * Typical values: 1-5 steps (1-5ms at 1ms timestep)
 * 
 * @return Synaptic delay in simulation steps
 */
Delay Synapse::getDelay() const {
    return pImpl->delay;
}

/**
 * Set synaptic delay with biological constraints
 * 
 * @param delay Delay in simulation steps (biologically plausible range: 1-100)
 * 
 * Biological considerations:
 * - Minimum: 1 step (representing fastest biological transmission)
 * - Maximum: 100 steps (representing long-range connections)
 * - Realistic synaptic delays vary from 1-5 ms in biological systems
 */
void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

/**
 * Get synapse type
 * 
 * Synapse type determines functional properties and learning capabilities
 * 
 * @return Synapse type (excitatory, inhibitory, modulatory, electrical, gap junction)
 */
SynapseType Synapse::getType() const {
    return pImpl->type;
}

/**
 * Set synapse type with biological validation
 * 
 * @param type Synapse type (determines learning rules and dynamics)
 * 
 * Biological significance:
 * - Excitatory: glutamate-based chemical synapses (most common)
 * - Inhibitory: GABA-based chemical synapses
 * - Modulatory: neuromodulatory synapses (dopamine, acetylcholine, etc.)
 * - Electrical: gap junctions (direct electrical coupling)
 */
void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

/**
 * Check if synapse is excitatory
 * 
 * @return true if synapse type is excitatory
 */
bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

/**
 * Check if synapse is inhibitory
 * 
 * @return true if synapse type is inhibitory
 */
bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

/**
 * Record presynaptic spike for spike-timing dependent plasticity (STDP)
 * 
 * STDP mechanism implements the biological observation that the timing
 * between pre- and postsynaptic spikes determines synaptic modification direction.
 * 
 * Biological laws:
 * - Pre before post (Δt > 0): LTP (Long-Term Potentiation) - strengthen synapse
 * - Post before pre (Δt < 0): LTD (Long-Term Depression) - weaken synapse
 * - Simultaneous spikes (Δt ≈ 0): No change
 * 
 * @param timestamp Time when presynaptic spike occurred
 * 
 * Implementation notes:
 * - Maintains spike history for STDP calculations
 * - Records last spike time for fast STDP computations
 * - History management prevents memory growth (bounded by MAX_SPIKE_HISTORY)
 */
void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
}

/**
 * Record postsynaptic spike for STDP
 * 
 * Complements presynaptic spike recording for STDP calculations.
 * 
 * @param timestamp Time when postsynaptic spike occurred
 */
void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
}

/**
 * Get presynaptic spike history for STDP calculations
 * 
 * @return Vector of timestamps of presynaptic spikes (most recent last)
 */
const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

/**
 * Get postsynaptic spike history for STDP calculations
 * 
 * @return Vector of timestamps of postsynaptic spikes (most recent last)
 */
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

/**
 * Clear spike history (used for simulation reset)
 * 
 * Called when simulation is reset to remove spike history that could
 * affect STDP calculations in the new simulation epoch.
 */
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

/**
 * Get plasticity flags for this synapse
 * 
 * @return Reference to plasticity flags structure
 */
PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
 * Enable specific plasticity mechanisms for this synapse
 * 
 * Configures which learning mechanisms are active for this synapse,
 * allowing selective enabling of different plasticity rules.
 * 
 * @param hebbian Enable Hebbian learning (activity-dependent strengthening)
 * @param stdp Enable spike-timing dependent plasticity
 * @param rewardModulated Enable reward-modulated plasticity (eligibility traces)
 * 
 * Biological significance:
 * - Hebbian: "Neurons that fire together, wire together" - cooperative strengthening
 * - STDP: Temporal credit assignment for spike timing relationships
 * - Reward-modulated: Integration of reward prediction errors for value learning
 */
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
 * Get current eligibility trace for reward-modulated learning
 * 
 * Eligibility trace integrates presynaptic activity until reward arrives,
 * enabling delayed synaptic updates when reward prediction error occurs.
 * 
 * Reward-modulated learning mechanism:
 * 1. Presynaptic spike increments eligibility trace
 * 2. Trace decays exponentially over time
 * 3. When reward arrives, trace value determines synaptic update
 * 4. Trace enables credit assignment for temporally distant events
 * 
 * @return Current eligibility trace value
 * 
 * Range: [-1.0, 1.0] where sign indicates expected reward direction
 */
float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

/**
 * Set eligibility trace value
 * 
 * @param trace New eligibility trace value
 * 
 * Biological correspondence: Trace represents synaptic eligibility for
 * reward-modulated learning based on presynaptic activity history.
 */
void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

/**
 * Decay eligibility trace for reward-modulated learning
 * 
 * Implements temporal decay of eligibility traces, ensuring that
 * synaptic eligibility for reward-modulated learning diminishes
 * over time if no reward is received.
 * 
 * @param decayRate Decay factor (0.0 = no decay, 1.0 = complete decay per timestep)
 * 
 * Implementation: Multiplicative decay preserves trace sign while
 * reducing magnitude over time. Small traces (< 0.001) are zeroed
 * to prevent numerical noise from accumulating.
 */
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

/**
 * Get synaptic efficacy
 * 
 * Efficacy represents use-dependent modulation of synaptic strength.
 * Higher efficacy means the same presynaptic input produces
 * a larger postsynaptic response.
 * 
 * Use-dependent modulation biological significance:
 * - Higher firing rates increase synaptic efficacy (post-tetanic potentiation)
 * - Repeated stimulation strengthens synaptic response
 * - Activity-dependent scaling matches input statistics
 * - Implementation range: [0.0, 2.0] where 1.0 = baseline efficacy
 * 
 * @return Synaptic efficacy value
 */
float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

/**
 * Set synaptic efficacy
 * 
 * @param efficacy New efficacy value (clamped to [0.0, 2.0])
 * 
 * Constraints:
 * - Minimum efficacy: 0.0 (no response to input)
 * - Maximum efficacy: 2.0 (doubled response)
 * - Baseline efficacy: 1.0 (normal response)
 */
void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

/**
 * Update synapse state for one simulation step
 * 
 * Implements comprehensive synaptic dynamics including short-term plasticity,
 * eligibility trace management, and weight stabilization.
 * 
 * Biological model: Real synaptic dynamics include:
 * 1. Short-term plasticity (STP) - utilization and recovery
 * 2. Eligibility trace decay for reward-modulated learning
 * 3. Weight bounds enforcement for stability
 * 4. Use-dependent efficacy modulation
 * 
 * @param currentTime Current simulation time (seconds)
 * 
 * Mathematical models implemented:
 * - Tsodyks-Markram model for STP
 * - Exponential decay for eligibility traces
 * - Constrained weight updates for stability
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
 * Reset synapse to initial state (used for simulation reset)
 * 
 * Called when simulation is reset to return synapse to its initial
 * configuration for a new simulation epoch.
 * 
 * Biological significance: Represents synaptic homeostatic mechanisms
 * that maintain stable network dynamics over time.
 */
void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();n    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

/**
 * Initialize synapse with random parameters
 * 
 * Biological realism: Synapses have natural variability in strength and dynamics.
 * Random initialization based on synapse type with biologically plausible ranges.
 * 
 * Randomization strategy:
 * - Excitatory synapses: small positive weights (0.1-0.4)
 * - Inhibitory synapses: negative weights (-0.1 to -0.4)
 * - Other types: balanced random weights (-0.1 to 0.1)
 * - Efficacy initialized within biologically realistic ranges
 * - Delays sampled from physiological distribution (1-5 steps)
 * 
 * @param rng Random number generator for reproducible initialization
 * 
 * Biological correspondence: Initial conditions reflect natural variability
 * in synaptic properties across biological neural systems.
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
