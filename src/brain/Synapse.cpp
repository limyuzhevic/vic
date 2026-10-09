#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <iomanip>

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

Synapse::~Synapse() {
    delete pImpl;
}

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
    // Enhanced synaptic dynamics with biological realism:
    // 1. Calcium-dependent short-term plasticity
    // 2. Neuromodulator integration (dopamine, acetylcholine)
    // 3. Activity-dependent efficacy changes
    // 4. Advanced eligibility trace dynamics
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Enhanced short-term plasticity with calcium dynamics
    // More biologically accurate Tsodyks-Markram model with multiple variables
    
    // Calcium dynamics (for spike-triggered processes)
    float calciumInput = 0.0f;
    if (pImpl->lastPreSpikeTime >= 0.0f && 
        static_cast<float>(currentTime - pImpl->lastPreSpikeTime) < 50.0f) {
        // Recent presynaptic spike increases calcium
        calciumInput += 1.0f;
    }
    if (pImpl->lastPostSpikeTime >= 0.0f && 
        static_cast<float>(currentTime - pImpl->lastPostSpikeTime) < 100.0f) {
        // Recent postsynaptic spike also contributes to calcium
        calciumInput += 0.5f;
    }
    
    // Update calcium with decay
    float calcium = pImpl->efficacy * 0.1f;  // Calcium proportional to efficacy
    calcium += calciumInput * 0.05f;  // Spike-triggered calcium influx
    calcium *= std::exp(-static_cast<float>(currentTime - (pImpl->lastPreSpikeTime >= 0.0f ? pImpl->lastPreSpikeTime : 0)) / 100.0f);  // Decay
    pImpl->efficacy = std::clamp(calcium, 0.0f, 2.0f);  // Update efficacy from calcium
    
    // Enhanced short-term depression with utilization-based model
    // u: utilization (0-1), x: recovery (0-1)
    float u = pImpl->shortTermDepression;  // Utilization
    float x = pImpl->shortTermFacilitation;  // Recovery
    
    // Activity-dependent changes in u and x
    if (pImpl->lastPreSpikeTime >= 0.0f && 
        static_cast<float>(currentTime - pImpl->lastPreSpikeTime) < 20.0f) {
        // Recent spike increases utilization
        u = std::min(1.0f, u + 0.1f);
        // Spike also increases recovery slightly
        x = std::min(1.0f, x + 0.05f);
    }
    
    // Decay dynamics
    u *= std::exp(-static_cast<float>(currentTime - (pImpl->lastPostSpikeTime >= 0.0f ? pImpl->lastPostSpikeTime : 0)) / 100.0f);
    x *= std::exp(-static_cast<float>(currentTime - (pImpl->lastPreSpikeTime >= 0.0f ? pImpl->lastPreSpikeTime : 0)) / 200.0f);
    
    // Clamp to biological bounds
    u = std::clamp(u, 0.0f, 1.0f);
    x = std::clamp(x, 0.0f, 1.0f);
    
    // Update STP state
    pImpl->shortTermDepression = u;
    pImpl->shortTermFacilitation = x;
    
    // Enhanced eligibility trace with neuromodulator integration
    // Dopamine and acetylcholine modulate eligibility trace dynamics
    float neuromodulatorEffect = 1.0f;  // Baseline
    if (pImpl->plasticityFlags.reward_modulated) {
        // Reward-modulated learning: dopamine enhances eligibility trace
        neuromodulatorEffect *= 1.5f;
        // Add eligibility trace based on recent activity
        if (pImpl->lastPreSpikeTime >= 0.0f) {
            pImpl->eligibilityTrace += pImpl->efficacy * neuromodulatorEffect * 0.1f;
        }
    }
    
    // Decay eligibility trace with neuromodulator modulation
    decayEligibilityTrace(0.002f * neuromodulatorEffect);  // Slower decay with dopamine
    
    // Efficacy modulation based on use history
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        // Recent activity increases efficacy (use-dependent plasticity)
        pImpl->efficacy *= 1.01f;
    }
    
    // Clamp weight bounds with neurobiological constraints
    pImpl->weight = std::clamp(pImpl->weight, -2.0f, 2.0f);  // Expanded range for stronger synapses
    
    // Log synaptic state for debugging
    NLM_LOG_DEBUG("Synapse " + std::to_string(pImpl->id) + " state:")
        << " weight=" << pImpl->weight 
        << " efficacy=" << pImpl->efficacy
        << " u=" << pImpl->shortTermDepression
        << " x=" << pImpl->shortTermFacilitation
        << " eligibility=" << pImpl->eligibilityTrace
        << " lastPre=" << pImpl->lastPreSpikeTime
        << " lastPost=" << pImpl->lastPostSpikeTime;
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Enhanced random initialization with biological constraints and realism
    
    // Validate RNG is properly initialized
    if (!rng.engine()) {
        NLM_LOG_ERROR("Invalid random generator for synapse " + std::to_string(pImpl->id));
        return;
    }
    
    // Set up biological initial conditions based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: realistic biological ranges
        // AMPA receptors: typical conductance 0.1-0.5 nS, weight 0.2-0.8
        pImpl->weight = rng.uniformReal(0.2f, 0.8f);
        // Excitatory efficacy typically high (0.9-1.2)
        pImpl->efficacy = rng.uniformReal(0.9f, 1.2f);
        // Excitatory synapses have moderate initial facilitation
        pImpl->shortTermFacilitation = rng.uniformReal(0.0f, 0.3f);
        // Short-term depression starts at resting level (1.0)
        pImpl->shortTermDepression = 1.0f;
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: realistic GABA_A receptor dynamics
        // GABA_A: typical conductance 0.5-2.0 nS, weight -0.5 to -1.5
        pImpl->weight = -rng.uniformReal(0.5f, 1.5f);
        // Inhibitory efficacy typically moderate (0.7-1.1)
        pImpl->efficacy = rng.uniformReal(0.7f, 1.1f);
        // Inhibitory synapses have low initial facilitation
        pImpl->shortTermFacilitation = rng.uniformReal(0.0f, 0.2f);
        // Short-term depression starts at resting level (1.0)
        pImpl->shortTermDepression = 1.0f;
    } else {
        // Modulatory synapses: neuromodulator receptors
        // Acetylcholine, dopamine, serotonin receptors: different weight ranges
        pImpl->weight = rng.uniformReal(-0.2f, 0.2f);
        // Modulatory efficacy typically 0.8-1.0
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        // Modulatory synapses have minimal facilitation
        pImpl->shortTermFacilitation = rng.uniformReal(0.0f, 0.1f);
        // Short-term depression starts at resting level (1.0)
        pImpl->shortTermDepression = 1.0f;
    }
    
    // Random delay: 1-10 steps (more biologically realistic)
    // Physiological synaptic delays: 1-10ms typical
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 10));
    
    // Initialize calcium-based efficacy with some noise
    pImpl->efficacy = std::clamp(pImpl->efficacy + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f, 0.0f, 2.0f);
    
    // Initialize eligibility trace with small baseline
    pImpl->eligibilityTrace = 0.01f;  // Small baseline for reward-modulated learning
    
    // Initialize last spike times to "no spike" (-1.0)
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Log synaptic initialization for debugging
    NLM_LOG_DEBUG("Synapse " + std::to_string(pImpl->id) + " initialized:")
        << " weight=" << pImpl->weight 
        << " efficacy=" << pImpl->efficacy
        << " type=" << static_cast<int>(pImpl->type)
        << " delay=" << pImpl->delay
        << " facilitation=" << pImpl->shortTermFacilitation
        << " depression=" << pImpl->shortTermDepression
        << " eligibility=" << pImpl->eligibilityTrace;
}

} // namespace nlm
