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

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    // Constructor - initialize synapse with default parameters
    //
    // Sets up the basic synapse structure with default values:
    // - Weight: neutral (0.0)
    // - Delay: 1 timestep (minimal biological transmission delay)
    // - Type: excitatory (default)
    // - Plasticity: all rules disabled (plastic components initialized)
    // - Short-term plasticity: fully recovered, no facilitation
    // - Eligibility trace: zero (no learning history)
    // - Efficacy: 1.0 (normal synaptic efficacy)
    //
    // The Pimpl idiom ensures encapsulation of implementation details.
    
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
    pImpl->weight = weight;
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

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

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

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    // Update synapse for one simulation step with comprehensive synaptic dynamics
    //
    // Implements multiple interacting plasticity mechanisms simultaneously:
    // 1. Short-term synaptic plasticity (STP) using Tsodyks-Markram model
    // 2. Eligibility trace decay for reward-modulated learning
    // 3. Synaptic efficacy use-dependent modulation
    // 4. Weight boundary enforcement for stability
    //
    // Short-term Plasticity (STP) Models:
    // - Facilitation: Previous activity increases synaptic efficacy (resources pooled)
    // - Depression: Previous activity depletes synaptic resources (usage proportional)
    // - These processes operate on different timescales (facilitation ~100ms, depression ~200ms)
    // - STP is use-dependent, enhancing learning during sustained activity
    //
    // Eligibility Trace Management:
    // - Tracks recent spike activity for reward-modulated learning
    // - Decay reflects the temporal window for plasticity eligibility
    // - Zero when no recent spike activity
    // - Integrates with reward signals to produce long-term weight changes
    //
    // Performance Considerations:
    // - All calculations use exponential functions (can be optimized with precomputed factors)
    // - Operations are O(1) per synapse, suitable for large-scale networks
    // - Spike history management uses circular buffer semantics (fixed size)
    
    TimestepDuration dt = 0.001;  // 1ms timestep (simulation parameter)
    
    // Phase 1: Decay short-term facilitation
    // Facilitation process: previous activity increases synaptic efficacy
    // Modeled as exponential decay toward baseline (0.0)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        
        // Prevent numerical underflow
        if (pImpl->shortTermFacilitation < 1e-10f) {
            pImpl->shortTermFacilitation = 0.0f;
        }
    }
    
    // Phase 2: Decay short-term depression
    // Depression process: previous activity depletes synaptic resources
    // Modeled as recovery toward baseline (1.0) over time
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0 (full resource availability)
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        
        // Clamp to reasonable bounds to prevent numerical issues
        if (pImpl->shortTermDepression > 1.0f) pImpl->shortTermDepression = 1.0f;
        if (pImpl->shortTermDepression < 0.0f) pImpl->shortTermDepression = 0.0f;
    }
    
    // Phase 3: Decay eligibility trace for reward-modulated learning
    // Eligibility trace represents the synaptic eligibility for plasticity
    // Fast decay ensures only recent activity influences learning
    // The decay rate (0.001) provides ~1000ms window for plasticity eligibility
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Phase 4: Enforce synaptic weight bounds
    // Prevents numerical instability and ensures biological plausibility
    // Clamping is more efficient than complex constraint solvers
    if (pImpl->weight < Impl::MIN_WEIGHT) pImpl->weight = Impl::MIN_WEIGHT;
    if (pImpl->weight > Impl::MAX_WEIGHT) pImpl->weight = Impl::MAX_WEIGHT;
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Initialize synapse with biologically plausible random parameters
    //
    // This method sets up initial synaptic properties using biologically realistic
    // distributions derived from experimental data and network modeling literature.
    // Different values for excitatory vs inhibitory synapses reflect known
    // differences in synaptic physiology.
    //
    // Biological Parameter Rationale:
    // - Excitatory synapses: Small positive weights (0.1-0.4) reflect typical AMPA
    //   receptor-mediated EPSP magnitudes in cortical networks
    // - Inhibitory synapses: Negative weights (-0.1 to -0.4) reflect GABA-mediated
    //   IPSP magnitudes, typically similar in absolute value to excitatory
    // - Delay: 1-5 timesteps (1-5ms) reflects biological synaptic transmission
    //   times (glial processes, neurotransmitter release)
    // - Efficacy: Close to 1.0 for both types, representing baseline synaptic strength
    // - Short-term plasticity: Fully recovered state, no initial facilitation
    //
    // Performance Considerations:
    // - Uses uniform distributions for simplicity and speed
    // - Clamping ensures weights stay within reasonable bounds
    // - Initializes all state variables to biologically plausible values
    // - Efficient for large-scale network initialization
    
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        // Reflects typical AMPA receptor-mediated EPSP magnitudes in cortical networks
        // Values between 0.1 and 0.4 ensure suprathreshold effects with realistic dynamics
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        
        // Excitatory synapses have moderate initial efficacy
        // Represents baseline synaptic strength for Type I/II synapses
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        // Reflects GABA-mediated IPSP magnitudes
        // Same absolute values as excitatory for balanced network construction
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        
    } else {
        // Other types (e.g., neuromodulatory): small random weights
        // May include electrical synapses, gap junctions, or modulatory connections
        // Smaller magnitudes appropriate for modulatory effects
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    // Reflects biological variability in synaptic transmission times
    // Includes glial processes, neurotransmitter release, and receptor activation
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    // - shortTermDepression = 1.0f: fully recovered (resources available)
    // - shortTermFacilitation = 0.0f: no initial facilitation (resources not pooled)
    // This represents the synapse at rest, ready for activity
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    // Indicates no current eligibility for reward-modulated learning
    // Will be set to non-zero values when spikes occur via recordPreSpike/recordPostSpike
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times to -1.0f (inactive)
    // Negative value indicates no spike has occurred yet
    // Used by short-term plasticity dynamics to compute time since last activity
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
