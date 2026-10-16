#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Config/Config.hpp"
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
    static constexpr float MIN_WEIGHT;
    static constexpr float MAX_WEIGHT;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU;
    static constexpr float STP_DEPRESSION_TAU;
    static constexpr float STP_U_MAX;
    
    static constexpr size_t MAX_SPIKE_HISTORY;
};

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
    // Return the unique identifier of this synapse
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    // Return the source neuron ID that sends spikes to this synapse
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    // Return the destination neuron ID that receives spikes from this synapse
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    // Return the current synaptic weight
    // Weight represents connection strength between source and destination neurons
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    // Set the synaptic weight to a new value
    // Weight represents connection strength (negative for inhibitory, positive for excitatory)
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    // Adjust synaptic weight by adding the delta (positive for potentiation, negative for depression)
    // Commonly used in plasticity rules like STDP and Hebbian learning
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    // Return the synaptic delay in simulation steps
    // Delay represents the time it takes for a spike to propagate through this synapse
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    // Set the synaptic delay in simulation steps
    // Larger delays create temporal effects in network dynamics
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    // Return the synapse type (Excitatory or Inhibitory)
    // Type determines whether the synapse increases or decreases postsynaptic potential
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    // Set the synapse type (Excitatory or Inhibitory)
    // Type determines the sign of the postsynaptic effect
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    // Check if this synapse is excitatory
    // Returns true for Excitatory synapses, false otherwise
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    // Check if this synapse is inhibitory
    // Returns true for Inhibitory synapses, false otherwise
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    // Record a spike from the presynaptic neuron at the given timestamp
    // Maintains spike history for STDP and other timing-dependent plasticity
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Record a spike from the postsynaptic neuron at the given timestamp
    // Maintains spike history for STDP and other timing-dependent plasticity
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    // Return the history of presynaptic neuron spikes
    // Used by STDP to calculate timing-dependent plasticity
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    // Return the history of postsynaptic neuron spikes
    // Used by STDP to calculate timing-dependent plasticity
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    // Clear both pre-synaptic and post-synaptic spike histories
    // Called when synapse is removed or reset
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    // Return the plasticity flags for this synapse
    // Indicates which plasticity mechanisms are enabled (Hebbian, STDP, reward-modulated)
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    // Return the plasticity flags for this synapse (const version)
    // Indicates which plasticity mechanisms are enabled (Hebbian, STDP, reward-modulated)
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    // Enable/disable specific plasticity rules for this synapse
    // hebbian: Enable Hebbian (correlation-based) learning
    // stdp: Enable Spike-Timing-Dependent Plasticity
    // rewardModulated: Enable reward-modulated plasticity
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    // Return the eligibility trace value for reward-modulated learning
    // Represents recent synaptic activity that can be modified by future rewards
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    // Set the eligibility trace value for reward-modulated learning
    // Higher values indicate more recent activity eligible for plasticity
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    // Decay the eligibility trace by the given rate
    // Eligibility traces decay over time if no reward signal arrives
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    // Return the synaptic efficacy (use-dependent modulation factor)
    // Efficacy modulates the strength of synaptic transmission based on recent use
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    // Set the synaptic efficacy factor (use-dependent modulation)
    // Efficacy ranges from 0.0 to 2.0, affecting synaptic transmission strength
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}


void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Read configuration values with defaults for backward compatibility
    static const float minWeight = -1.0f;
    static const float maxWeight = 1.0f;
    static const float facilitationTau = 100.0f;  // ms
    static const float depressionTau = 200.0f;  // ms
    static const float uMax = 1.0f;  // Max utilization
    
    // TODO: Replace with actual config loading when Config is available
    // float minWeight = config.getOr("synapse.weight_min", -1.0f);
    // float maxWeight = config.getOr("synapse.weight_max", 1.0f);
    // float facilitationTau = config.getOr("synapse.stp_facilitation_tau", 100.0f);
    // float depressionTau = config.getOr("synapse.stp_depression_tau", 200.0f);
    // float uMax = config.getOr("synapse.stp_u_max", 1.0f);
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / facilitationTau);
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / depressionTau));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, minWeight, maxWeight);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    validateSynapseParameters();
    
    setInitialWeight(pImpl->type, rng);
    setInitialEfficacy(rng);
    setInitialDelay(rng);
    setInitialPlasticityState();
    setInitialEligibilityTrace();
    setInitialSpikeTimes();
}

} // namespace nlm
