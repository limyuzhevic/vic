#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * 
 * **Synaptic Dynamics Implementation**
 * ================================
 * 
 * This file implements the complete synaptic dynamics for biological neural networks,
 * modeling both long-term and short-term plasticity mechanisms.
 * 
 * **Synaptic Computation Overview**
 * ------------------------------
 * Synapses are the fundamental communication channels in neural networks, implementing:
 * 1. **Long-term plasticity**: Activity-dependent weight changes (STDP, Hebbian, reward-modulated)
 * 2. **Short-term plasticity**: Use-dependent changes in synaptic efficacy (STP)
 * 3. **Temporal dynamics**: Spike transmission delays and timing-dependent processes
 * 
 * **Synaptic Mathematical Models**
 * -----------------------------
 * 
 * **1. Spike-Timing-Dependent Plasticity (STDP)**
 * -----------------------------------------------
 * STDP implements the fundamental biological principle that synaptic strength
 * changes based on the precise timing of pre- and post-synaptic spikes:
 * 
 * For a synapse with weight w(t):
 * 
 * If pre-synaptic spike occurs at time t_pre and post-synaptic spike at t_post:
 * 
 * Δw = {
 *    A_LTP * exp(-(t_post - t_pre) / τ_LTP)   if t_post > t_pre (LTP)
 *    A_LTD * exp(-(t_pre - t_post) / τ_LTD)   if t_pre > t_post (LTD)
 *    0                                        otherwise
 * }
 * 
 * Where:
 * - A_LTP: Maximum LTP magnitude (typically 0.01-0.02)
 * - A_LTD: Maximum LTD magnitude (typically 0.012-0.016)
 * - τ_LTP: LTP time constant (~16-20ms)
 * - τ_LTD: LTD time constant (~33-35ms)
 * 
 * **Biological Rationale**: STDP implements Hebbian learning "fire together,
 * wire together" while maintaining synaptic balance through anti-Hebbian effects.
 * 
 * **2. Tsodyks-Markram Short-Term Plasticity (STP)**
 * --------------------------------------------------
 * STP models the dynamic modulation of synaptic efficacy based on recent activity:
 * 
 * The model implements three main mechanisms:
 * - **Facilitation**: Progressive increase in efficacy with repeated spikes
 * - **Depression**: Decrease in efficacy due to sustained activity
 * - **Recovery**: Return to baseline between activity periods
 * 
 * Mathematical formulation:
 * 
 * Let x be the utilization factor (0-1), U be the release probability,
 * and R be the recovery variable (0-1).
 * 
 * Dynamics equations:
 * 
 * dx/dt = -x/τ_x + U(1-x)δ(t - t_spike)
 * dR/dt = (1-R)/τ_R
 * 
 * Where:
 * - x: utilization (fraction of vesicles released)
 * - U: baseline release probability
 * - R: recovery variable (vesicle availability)
 * - τ_x: depression time constant (200ms typical)
 * - τ_R: recovery time constant (1000ms typical)
 * 
 * Synaptic efficacy: x × R
 * 
 * **Biological Rationale**: STP captures various synaptic behaviors observed
 * in real neurons, from depression to facilitation, depending on neurotransmitter
 * type and receptor dynamics.
 * 
 * **3. Reward-Modulated Plasticity**
 * ---------------------------------
 * Integrates behavioral reinforcement signals into synaptic learning:
 * 
 * Eligibility trace: Z(t) tracks recent activity
 * 
 * dZ/dt = -Z/τ_Z + δ(t - t_spike)
 * 
 * Weight update: Δw = θ(t) × Z(t) × (1 - |w|/w_max)
 * 
 * Where:
 * - θ(t): reward prediction error signal
 * - Z(t): eligibility trace
 * - w_max: maximum allowed weight magnitude
 * 
 * **Biological Rationale**: Dopamine and other neuromodulators gate learning
 * based on behavioral relevance, implementing model-based reinforcement
 * learning in biological systems.
 */

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    /**
     * 
     * **Synaptic Identity**
     * -------------------
     * 
     * **id**: Unique identifier for the synapse within the brain
     * **sourceNeuron**: Pre-synaptic neuron identifier
     * **destinationNeuron**: Post-synaptic neuron identifier
     * 
     * These form the structural basis of the connection graph.
     */
    
    // Synapse type
    SynapseType type;
    
    /**
     * 
     * **Spike History for Long-Term Plasticity**
     * ------------------------------------------
     * 
     * **preSpikeHistory**: Timestamps of pre-synaptic spikes (for STDP)
     * **postSpikeHistory**: Timestamps of post-synaptic spikes (for STDP)
     * 
     * These vectors maintain spike timing history for STDP calculations:
     * 
     * **STDP Algorithm**
     * ------------------
     * For each pre-synaptic spike in preSpikeHistory:
     * - Calculate time difference to each post-synaptic spike
     * - Apply LTP for pre-before-post spikes (t_post > t_pre)
     * - Apply LTD for post-before-pre spikes (t_pre > t_post)
     * - Modify weight accordingly (with learning rate scaling)
     * 
     * The MAX_SPIKE_HISTORY limit prevents unbounded growth while maintaining
     * sufficient temporal resolution for STDP (~10 seconds at 1ms timestep).
     */
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    /**
     * 
     * **Short-Term Plasticity State**
     * ------------------------------
     * 
     * **shortTermDepression**: Utilization factor U (0-1, 1=maximum release)
     * **shortTermFacilitation**: Facilitation variable x (0-1, 0=no facilitation)
     * **lastPreSpikeTime**: Timestamp of last pre-synaptic spike (-1 if none)
     * **lastPostSpikeTime**: Timestamp of last post-synaptic spike (-1 if none)
     * 
     * **Tsodyks-Markram Model Variables**:
     * - x (facilitation): Increases with each spike, decays over time
     * - U (depression): Decreases with each spike, recovers over time
     * - Last spike times: Track timing for dynamics calculations
     */
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    /**
     * 
     * **Eligibility Trace for Reward-Modulated Learning**
     * ------------------------------------------------------
     * 
     * **eligibilityTrace**: Tracks recent synaptic activity for reward-modulated learning
     * 
     * **Biological Basis**:
     * - Implements the three-factor learning rule
     * - Factor 1: Pre-synaptic activity (eligibility)
     * - Factor 2: Post-synaptic activity (eligibility)
     * - Factor 3: Neuromodulatory signal (dopamine, reward prediction error)
     * 
     * **Algorithm**:
     * - Eligibility trace decays over time (learning window)
     * - Reset to zero on each spike (marking the learning opportunity)
     * - Scaled by neuromodulatory signal for weight updates
     * 
     * **Decay Rate**: 0.001 per timestep (1ms) - approximately 50ms time constant
     */
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    /**
     * 
     * **Synaptic Efficacy**
     * --------------------
     * 
     * **efficacy**: Modulates synaptic weight changes and maximum possible changes
     * 
     * **Range**: 0.0 - 2.0 (0=completely ineffective, 2=strongly potentiated)
     * 
     * **Biological Significance**:
     * - Encodes synaptic strength independent of baseline weight
     * - Modulated by neuromodulators (dopamine, acetylcholine)
     * - Affects both STDP and Hebbian learning rates
     * - Constrained to prevent runaway excitation/inhibition
     */
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    /**
     * 
     * **Weight Bounds**
     * -----------------
     * 
     * **MIN_WEIGHT**: -1.0 (minimum inhibitory weight)
     * **MAX_WEIGHT**: 1.0 (maximum excitatory weight)
     * 
     * **Biological Rationale**:
     * - Bounds maintain network stability
     * - Reflect physiological constraints on synaptic strength
     * - Prevent numerical overflow in learning algorithms
     * - Provide homeostasis through weight normalization
     */
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    /**
     * 
     * **Short-Term Plasticity Parameters**
     * ----------------------------------
     * 
     * **STP_FACILITATION_TAU**: 100.0 ms - time constant for facilitation decay
     * **STP_DEPRESSION_TAU**: 200.0 ms - time constant for depression recovery
     * **STP_U_MAX**: 1.0 - maximum utilization factor
     * 
     * **Synaptic Type Specificity**:
     * - Excitatory synapses: Typically depression dominant (U ~ 0.5)
     * - Inhibitory synapses: Often facilitation dominant (U ~ 0.2)
     * - Neuromodulatory synapses: Variable dynamics based on neurotransmitter
     */
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

/**
 * 
 * **Synapse Construction and Initialization**
 * -----------------------------------------
 * 
 * **Constructor Parameters**
 * ----------------------
 * 
 * **Synapse(SynapseId id, NeuronId source, NeuronId destination)**
 * -------------------------------------------------------------
 * 
 * Creates a new synapse with specified connectivity.
 * 
 * **Initial State**:
 * 
 * **Synaptic Type**: Excitatory (default, reflects neural network bias)
 * **Delay**: 1 simulation step (1ms at standard timestep)
 * **Eligibility Trace**: 0.0 (no recent activity)
 * **Efficacy**: 1.0 (baseline efficacy)
 * **Short-term Plasticity**: Depression=1.0 (fully recovered), Facilitation=0.0
 * 
 * **Biological Initialization**:
 * - Type reflects neurotransmitter and receptor characteristics
 * - Short-term state represents baseline synaptic condition
 * - Zero eligibility trace indicates no recent learning opportunities
 * - Baseline efficacy represents normal synaptic function
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
 * 
 * **Synapse Destruction**
 * ----------------------
 * 
 * **Destructor**: Releases implementation memory
 * 
 * The pImpl idiom ensures proper cleanup of the dynamically allocated
 * implementation structure.
 */

Synapse::~Synapse() {
    delete pImpl;
}

/**
 * 
 * **Move Semantics**
 * -----------------
 * 
 * **Move Constructor**: Efficient transfer of synapse implementation
 * - Transfers ownership of pImpl
 * - Leaves source with nullptr (ownership transferred)
 * 
 * **Move Assignment**: Assignment with cleanup
 * - Deletes current implementation
 * - Transfers ownership from source
 * - Source becomes null (ownership transferred)
 */

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
 * 
 * **Synapse Accessors**
 * --------------------
 * 
 * These methods provide read-only access to synapse state without exposing
 * implementation details (data hiding principle).
 */

SynapseId Synapse::getId() const {
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

/**
 * 
 * **Synaptic Weight Management**
 * ----------------------------
 * 
 * **getWeight()**: Returns current synaptic weight
 * 
 * **Returns**: SynapticWeight (range: -1.0 to 1.0)
 * 
 * **setWeight(SynapticWeight weight)**: Direct weight assignment
 * 
 * **Parameters**: New weight value
 * 
 * **addToWeight(SynapticWeight delta)**: Incremental weight change
 * 
 * **Parameters**: Weight change amount
 * 
 * **Clamping**: Ensures weight stays within bounds [-1.0, 1.0]
 * 
 * **Biological Rationale**:
 * - Weight represents synaptic strength/probability
 * - Clamping prevents runaway excitation/inhibition
 * - Bounds reflect physiological constraints
 */

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
 * 
 * **Synaptic Delay Management**
 * ---------------------------
 * 
 * **getDelay()**: Returns synaptic transmission delay
 * **setDelay(Delay delay)**: Set transmission delay
 * 
 * **Delay**: Number of simulation steps (typically 1-5)
 * **Biological Meaning**: Spike transmission time + synaptic processing
 * 
 * **Typical Values**:
 * - Excitatory: 1-2 steps (fast transmission)
 * - Inhibitory: 1-3 steps (slightly slower)
 * - Neuromodulatory: 2-5 steps (slower signaling)
 */

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

/**
 * 
 * **Synaptic Type Management**
 * --------------------------
 * 
 * **getType()**: Returns synapse type
 * **setType(SynapseType type)**: Set synapse type
 * **isExcitatory()**: Check if excitatory
 * **isInhibitory()**: Check if inhibitory
 * 
 * **SynapseType Values**:
 * - EXCITATORY: Depolarizing effect on post-synaptic neuron
 * - INHIBITORY: Hyperpolarizing effect on post-synaptic neuron
 * - NEUROMODULATORY: Modulatory effect on plasticity/ excitability
 * 
 * **Biological Significance**:
 * - Determines reversal potential
 * - Influences postsynaptic response
 * - Affects plasticity mechanisms
 * - Determines transmission dynamics
 */

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

/**
 * 
 * **Spike Recording for Plasticity**
 * ---------------------------------
 * 
 * **recordPreSpike(Timestamp timestamp)**: Record pre-synaptic spike
 * **recordPostSpike(Timestamp timestamp)**: Record post-synaptic spike
 * 
 * **Purpose**: Maintain spike history for STDP calculations
 * 
 * **Storage**: Circular buffer (MAX_SPIKE_HISTORY limit)
 * 
 * **Usage**: Synapse maintains independent spike histories for pre and post
 * spikes, enabling precise STDP timing calculations.
 * 
 * **History Management**:
 * - Append new spike timestamps to respective vectors
 * - Maintain maximum history length
 * - Clear older entries when capacity exceeded
 */

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

/**
 * 
 * **Spike History Accessors**
 * --------------------------
 * 
 * **getPreSpikeHistory()**: Returns vector of pre-synaptic spike timestamps
 * **getPostSpikeHistory()**: Returns vector of post-synaptic spike timestamps
 * 
 * **Returns**: const reference to prevent modification (data integrity)
 * 
 * **Use Cases**:
 * - STDP weight calculations
 * - Plasticity analysis
 * - Network statistics
 * - Debugging and monitoring
 * 
 * **Note**: Returns entire history, not just recent spikes
 */

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

/**
 * 
 * **History Management**
 * ---------------------
 * 
 * **clearHistory()**: Reset spike history
 * 
 * **Actions**:
 * - Clear pre-synaptic spike history
 * - Clear post-synaptic spike history
 * 
 * **Use Cases**:
 * - Synapse reset (learning episode boundaries)
 * - Memory consolidation
 * - Network reconfiguration
 * - Error recovery
 */

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

/**
 * 
 * **Plasticity Flag Accessors**
 * ---------------------------
 * 
 * **getPlasticityFlags()**: Returns plasticity configuration
 * 
 * **Returns**: PlasticityFlags& (mutable for synapse configuration)
 * 
 * **PlasticityFlags Structure**:
 * - hebbian: Enable/Disable Hebbian learning
 * - stdp: Enable/Disable STDP
 * - reward_modulated: Enable/Disable reward modulation
 * 
 * **enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)**:
 * Configure plasticity mechanisms for this synapse.
 * 
 * **Parameters**:
 * - hebbian: Enable/Disable Hebbian learning
 * - stdp: Enable/Disable STDP
 * - rewardModulated: Enable/Disable reward modulation
 */

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
 * 
 * **Eligibility Trace Accessors**
 * -----------------------------
 * 
 * **getEligibilityTrace()**: Returns current eligibility trace value
 * **setEligibilityTrace(float trace)**: Set eligibility trace
 * **decayEligibilityTrace(float decayRate)**: Decay eligibility trace
 * 
 * **Eligibility Trace Dynamics**:
 * - Positive when pre-synaptic spike recorded
 * - Negative when post-synaptic spike recorded  
 * - Decays exponentially over time (learning window)
 * - Scales with neuromodulatory signal for weight updates
 * 
 * **Decay Implementation**:
 * 
 * if (abs(trace) < 0.001): trace = 0.0 (reset to zero)
 * otherwise: trace *= (1.0 - decayRate)
 */

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

/**
 * 
 * **Synaptic Efficacy Accessors**
 * -----------------------------
 * 
 * **getEfficacy()**: Returns current synaptic efficacy
 * **setEfficacy(float efficacy)**: Set synaptic efficacy
 * 
 * **Efficacy Range**: 0.0 - 2.0
 * 
 * **Biological Meaning**:
 * - Encodes use-dependent modulation of synaptic strength
 * - Modulated by neuromodulators (dopamine, acetylcholine)
 * - Affects both STDP and Hebbian learning rates
 * - Constrained to prevent excessive plasticity
 * 
 * **Typical Values**:
 * - Baseline: 1.0 (normal efficacy)
 * - Potentiated: >1.0 (enhanced learning)
 * - Depressed: <1.0 (reduced learning)
 */

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

/**
 * 
 * **Synaptic Dynamics Update**
 * ---------------------------
 * 
 * **step(Timestamp currentTime)**: Update synaptic state for one timestep
 * 
 * **Update Sequence**:
 * 1. **Decay short-term facilitation** (Tsodyks-Markram model)
 * 2. **Decay short-term depression** 
 * 3. **Decay eligibility trace** (reward-modulated learning)
 * 4. **Clamp weight bounds** (stability)
 * 
 * **Short-Term Plasticity Dynamics**:
 * 
 * **Facilitation**:
 * - x(t) = x(t-Δt) × exp(-Δt/τ_facil)
 * - τ_facil = STP_FACILITATION_TAU (100ms)
 * - Recovers toward 0 after each spike
 * 
 * **Depression**:
 * - U(t) = U(t-Δt) + (1-U(t-Δt)) × (1-exp(-Δt/τ_depr))
 * - τ_depr = STP_DEPRESSION_TAU (200ms)
 * - Recovers toward 1.0 between spikes
 * 
 * **Eligibility Trace**:
 * - decayRate = 0.001 per timestep (fast decay, ~50ms time constant)
 * - Resets to 0 when spike recorded (learning opportunity)
 * 
 * **Weight Clamping**:
 * - Ensures stability (prevents runaway excitation/inhibition)
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
 * 
 * **Synaptic Reset**
 * -----------------
 * 
 * **reset()**: Reset synapse to initial state
 * 
 * **Reset Actions**:
 * - Weight: Set to 0.0 (baseline)
 * - Pre-synaptic history: Clear
 * - Post-synaptic history: Clear
 * - Eligibility trace: Set to 0.0
 * - Efficacy: Set to 1.0 (baseline)
 * 
 * **Use Cases**:
 * - Learning episode boundaries
 * - Memory consolidation
 * - Network reconfiguration
 * - Error recovery
 * 
 * **Biological Analogy**:
 * - Synaptic depression after prolonged inactivity
 * - Return to baseline state
 * - Homeostatic regulation
 */

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

/**
 * 
 * **Synaptic Initialization**
 * ---------------------------
 * 
 * **initializeRandom(RandomGenerator& rng)**: Initialize with random parameters
 * 
 * **Random Initialization Strategy**:
 * 
 * **Excitatory Synapses**:
 * - Weight: Uniform[0.1, 0.4] (small positive weights)
 * - Efficacy: Uniform[0.8, 1.0] (moderate baseline efficacy)
 * 
 * **Inhibitory Synapses**:
 * - Weight: Negative uniform[0.1, 0.4] (inhibitory effect)
 * - Efficacy: Uniform[0.8, 1.0] (moderate baseline efficacy)
 * 
 * **Other Synapse Types**:
 * - Weight: Uniform[-0.1, 0.1] (small random weights)
 * - Efficacy: Uniform[0.9, 1.0] (high baseline efficacy)
 * 
 * **Delay Initialization**:
 * - Random uniform[1, 5] simulation steps
 * - Biological realism: 1-5ms transmission delays
 * 
 * **Short-Term Plasticity Initialization**:
 * - Depression: 1.0 (fully recovered)
 * - Facilitation: 0.0 (no initial facilitation)
 * 
 * **Eligibility Trace**: 0.0 (no recent learning opportunities)
 * 
 * **Spike Times**: -1.0 (no recorded spikes)
 * 
 * **Biological Rationale**:
 * - Different synapse types have different functional constraints
 * - Excitatory synapses provide positive drive
 * - Inhibitory synapses balance network activity
 * - Random initialization creates diverse connectivity patterns
 * - Typical weights ensure stable network dynamics
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
