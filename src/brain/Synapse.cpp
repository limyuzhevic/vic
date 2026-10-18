#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Synapse::Impl {
    // ========== SYNAPSE IDENTITY ==========
    SynapseId id;                           // Unique identifier for this synapse
    NeuronId sourceNeuron;                 // Source neuron identifier
    NeuronId destinationNeuron;            // Destination neuron identifier
    SynapticWeight weight;                 // Synaptic weight (-1.0 to 1.0)
    Delay delay;                          // Synaptic delay in simulation steps
    
    // ========== SYNAPSE TYPE ==========
    SynapseType type;                      // Functional type (Excitatory, Inhibitory, etc.)
    
    // ========== SPIKE HISTORY FOR PLASTICITY ==========
    std::vector<Timestamp> preSpikeHistory;  // Timestamps of pre-synaptic spikes (STDP)
    std::vector<Timestamp> postSpikeHistory; // Timestamps of post-synaptic spikes (STDP)
    
    // ========== PLASTICITY FLAGS ==========
    PlasticityFlags plasticityFlags;        // Enables STDP, Hebbian, and reward-modulated learning
    
    // ========== SHORT-TERM PLASTICITY STATE ==========
    float shortTermDepression;            // Utilization factor (0-1) for depression
    float shortTermFacilitation;          // Facilitation factor (0-1) for short-term potentiation
    float lastPreSpikeTime;               // Timestamp of last pre-synaptic spike (-1 if none)
    float lastPostSpikeTime;              // Timestamp of last post-synaptic spike (-1 if none)
    
    // ========== ELIGIBILITY TRACE ==========
    float eligibilityTrace;                // For reward-modulated learning (Hebbian with reward)
    
    // ========== SYNAPTIC EFFICACY ==========
    float efficacy;                        // Use-dependent modulation of synaptic strength
    
    // ========== WEIGHT BOUNDS ==========
    static constexpr float MIN_WEIGHT = -1.0f;  // Minimum allowed weight (inhibitory limit)
    static constexpr float MAX_WEIGHT = 1.0f;   // Maximum allowed weight (excitatory limit)
    
    // ========== SHORT-TERM PLASTICITY PARAMETERS ==========
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // Time constant for facilitation (ms)
    static constexpr float STP_DEPRESSION_TAU = 200.0f;   // Time constant for depression (ms)
    static constexpr float STP_U_MAX = 1.0f;              // Maximum utilization factor
    
    // ========== HISTORY LIMITS ==========
    static constexpr size_t MAX_SPIKE_HISTORY = 100;     // Maximum spike history entries
    
    // ========== INITIALIZATION ==========
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), eligibilityTrace(0.0f), efficacy(1.0f),
             shortTermDepression(1.0f), shortTermFacilitation(0.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f) {}
};

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

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Biological synaptic initialization with type-specific constraints
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights (0.1-0.4)
        // Excitatory synaptic transmission increases likelihood of postsynaptic firing
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy (0.8-1.0)
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights (-0.1 to -0.4)
        // Inhibitory synaptic transmission decreases likelihood of postsynaptic firing
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types (Modulatory, Electrical, GapJunction): small random weights (-0.1 to 0.1)
        // Modulatory synapses affect plasticity without direct excitation/inhibition
        // Electrical and gap junction synapses have different biophysical properties
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    // Synaptic delay reflects biological transmission time
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    // Short-term depression recovers toward 1.0 (full recovery)
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    // Short-term facilitation starts at 0.0 (no initial facilitation)
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace for reward-modulated learning
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
