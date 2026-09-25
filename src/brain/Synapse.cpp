#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Constants.hpp"
#include "../core/Utils.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> pre_spike_history;
    std::vector<Timestamp> post_spike_history;
    
    // Plasticity state
    PlasticityFlags plasticity_flags;
    
    // Short-term plasticity state
    float short_term_depression;  // Utilization factor (0-1)
    float short_term_facilitation;  // Facilitation factor
    float last_pre_spike_time;  // For short-term dynamics
    float last_post_spike_time;
    
    // Eligibility trace for reward-modulated learning
    float eligibility_trace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds (using Constants)
    static constexpr float MIN_WEIGHT = Constants::MIN_WEIGHT;
    static constexpr float MAX_WEIGHT = Constants::MAX_WEIGHT;
    
    // Short-term plasticity parameters (using Constants)
    static constexpr float STP_FACILITATION_TAU = Constants::STP_FACILITATION_TAU_MS;
    static constexpr float STP_DEPRESSION_TAU = Constants::STP_DEPRESSION_TAU_MS;
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = Constants::MAX_SPIKE_HISTORY;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->source_neuron = source;
    pImpl->destination_neuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibility_trace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->short_term_depression = 1.0f;
    pImpl->short_term_facilitation = 0.0f;
    pImpl->last_pre_spike_time = -1.0f;
    pImpl->last_post_spike_time = -1.0f;
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
    return pImpl->source_neuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destination_neuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds using common utility
    pImpl->weight = Utils::clampWeight(pImpl->weight);
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
    pImpl->pre_spike_history.push_back(timestamp);
    Utils::manageSpikeHistory(pImpl->pre_spike_history, timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->post_spike_history.push_back(timestamp);
    Utils::manageSpikeHistory(pImpl->post_spike_history, timestamp);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->pre_spike_history;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->post_spike_history;
}

void Synapse::clearHistory() {
    pImpl->pre_spike_history.clear();
    pImpl->post_spike_history.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticity_flags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool reward_modulated) {
    pImpl->plasticity_flags.hebbian = hebbian;
    pImpl->plasticity_flags.stdp = stdp;
    pImpl->plasticity_flags.reward_modulated = reward_modulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticity_flags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibility_trace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibility_trace = trace;
}

void Synapse::decayEligibilityTrace(float decay_rate) {
    pImpl->eligibility_trace *= (1.0f - decay_rate);
    if (std::abs(pImpl->eligibility_trace) < Constants::EPSILON_FOR_NORMALS) {
        pImpl->eligibility_trace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = Utils::clampEfficacy(efficacy);
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = Constants::DEFAULT_TIMESTEP_MS;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->last_pre_spike_time >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->last_pre_spike_time);
        pImpl->short_term_facilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (pImpl->last_post_spike_time >= 0.0f || pImpl->last_pre_spike_time >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->last_post_spike_time >= 0.0f ? static_cast<float>(currentTime - pImpl->last_post_spike_time) : 0.0f,
            pImpl->last_pre_spike_time >= 0.0f ? static_cast<float>(currentTime - pImpl->last_pre_spike_time) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->short_term_depression += (1.0f - pImpl->short_term_depression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(Constants::DEFAULT_NOVELTY_DECAY);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = Utils::clampWeight(pImpl->weight);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->pre_spike_history.clear();
    pImpl->post_spike_history.clear();
    pImpl->eligibility_trace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(Constants::NEURON_WEIGHT_INITIAL_MIN, Constants::NEURON_WEIGHT_INITIAL_MAX);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(Constants::INHIBITORY_WEIGHT_INITIAL_MIN, Constants::INHIBITORY_WEIGHT_INITIAL_MAX);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(Constants::OTHER_WEIGHT_INITIAL_MIN, Constants::OTHER_WEIGHT_INITIAL_MAX);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->short_term_depression = 1.0f;  // Fully recovered
    pImpl->short_term_facilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibility_trace = 0.0f;
    
    // Initialize last spike times
    pImpl->last_pre_spike_time = -1.0f;
    pImpl->last_post_spike_time = -1.0f;
}

} // namespace nlm
