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

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination) {
    validateSynapseId(id);
    validateNeuronIds(source, destination);
    initializeImpl(id, source, destination);
}

Synapse::~Synapse() noexcept = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Synapse::validateSynapseId(SynapseId id) {
    if (id == INVALID_SYNAPSE_ID) {
        NLM_THROW_NEURAL("Synapse ID cannot be invalid");
    }
}

void Synapse::validateNeuronIds(NeuronId source, NeuronId destination) {
    if (source == INVALID_NEURON_ID || destination == INVALID_NEURON_ID) {
        NLM_THROW_NEURAL("Neuron IDs cannot be invalid");
    }
    if (source == destination) {
        NLM_THROW_NEURAL("Synapse cannot connect a neuron to itself");
    }
}

void Synapse::initializeImpl(SynapseId id, NeuronId source, NeuronId destination) {
    pImpl = std::make_unique<Impl>();
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

void Synapse::setWeight(SynapticWeight weight) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    validateWeight(weight);
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    SynapticWeight newWeight = pImpl->weight + delta;
    validateWeight(newWeight);
    pImpl->weight = newWeight;
}

void Synapse::setDelay(Delay delay) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    if (delay == 0) {
        throw std::invalid_argument("Synapse delay must be > 0");
    }
    pImpl->delay = delay;
}

void Synapse::setType(SynapseType type) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->type = type;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
}

void Synapse::clearHistory() {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Synapse::setEligibilityTrace(float trace) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    if (decayRate < 0.0f || decayRate >= 1.0f) {
        throw std::invalid_argument("Decay rate must be in range [0, 1)");
    }
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

void Synapse::setEfficacy(float efficacy) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::validateWeight(SynapticWeight weight) const noexcept {
    if (weight < Impl::MIN_WEIGHT || weight > Impl::MAX_WEIGHT) {
        throw std::invalid_argument("Synapse weight out of valid range");
    }
}

SynapseId Synapse::getId() const noexcept {
    return pImpl ? pImpl->id : INVALID_SYNAPSE_ID;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->weight;
}

Delay Synapse::getDelay() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->delay;
}

SynapseType Synapse::getType() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->type;
}

bool Synapse::isExcitatory() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->type == SynapseType::Inhibitory;
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->postSpikeHistory;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->eligibilityTrace;
}

float Synapse::getEfficacy() const {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
    return pImpl->efficacy;
}

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
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
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
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

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        NLM_THROW_NEURAL("Synapse not properly initialized");
    }
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
