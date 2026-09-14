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
    
    // Synaptic dynamics parameters
    float synapticConductance;  // Current synaptic conductance (nS)
    float reversalPotential;    // Reversal potential for this synapse (mV)
    float membraneTimeConstant; // Time constant for synaptic decay (ms)
    float capacitance;          // Membrane capacitance (nF)
    
    // Activity-dependent plasticity
    float activityHistory;     // Accumulated activity for use-dependent plasticity
    float lastActivityUpdate;  // Last time activity was updated
    
    // Synaptic resource dynamics (vesicle pool)
    float availableResources;  // Number of available vesicles
    float releaseProbability; // Probability of vesicle release
    float recoveryRate;        // Rate of vesicle pool recovery
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), eligibilityTrace(0.0f), efficacy(1.0f),
             shortTermDepression(1.0f), shortTermFacilitation(0.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             synapticConductance(0.0f), reversalPotential(0.0f),
             membraneTimeConstant(5.0f), capacitance(1.0f),
             activityHistory(0.0f), lastActivityUpdate(0.0f),
             availableResources(1.0f), releaseProbability(0.5f), recoveryRate(0.1f) {}
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
    pImpl->synapticConductance = 0.0f;
    pImpl->reversalPotential = 0.0f;
    pImpl->membraneTimeConstant = 5.0f;
    pImpl->capacitance = 1.0f;
    pImpl->activityHistory = 0.0f;
    pImpl->lastActivityUpdate = 0.0f;
    pImpl->availableResources = 1.0f;
    pImpl->releaseProbability = 0.5f;
    pImpl->recoveryRate = 0.1f;
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
    
    // Update last pre-synaptic spike time
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
    
    // Update synaptic efficacy based on recent activity
    updateSynapticEfficacy(timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update last post-synaptic spike time
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
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
    // Real synaptic dynamics based on Tsodyks-Markram model
    // and spike-timing dependent plasticity principles
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Decay short-term plasticity state
    decayShortTermPlasticity(dt);
    
    // 2. Update synaptic resources
    updateSynapticResources(dt);
    
    // 3. Update synaptic conductance
    updateSynapticConductance(currentTime);
    
    // 4. Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // 5. Update activity-dependent plasticity
    updateActivityDependentPlasticity(currentTime, dt);
    
    // 6. Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // 7. Update synaptic efficacy based on use
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

void Synapse::updateSynapticConductance(Timestamp currentTime) {
    // Calculate synaptic conductance based on recent activity
    float conductance = 0.0f;
    
    // Boost conductance based on recent pre-synaptic spikes
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        float facilitationFactor = std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        conductance = pImpl->shortTermFacilitation * facilitationFactor;
        
        // Apply depression factor
        conductance *= pImpl->shortTermDepression;
        
        // Scale by efficacy
        conductance *= pImpl->efficacy;
    }
    
    pImpl->synapticConductance = conductance;
}

void Synapse::decayShortTermPlasticity(TimestepDuration dt) {
    // Decay short-term facilitation
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(dt);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Recover from depression toward 1.0
    float recovery = (1.0f - pImpl->shortTermDepression) * pImpl->recoveryRate * dt;
    pImpl->shortTermDepression = std::min(1.0f, pImpl->shortTermDepression + recovery);
}

void Synapse::updateSynapticResources(TimestepDuration dt) {
    // Update vesicle pool dynamics
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        // Release some resources based on probability
        float release = pImpl->releaseProbability * pImpl->availableResources;
        pImpl->availableResources -= release;
    }
    
    // Recover resources
    float recovery = (1.0f - pImpl->availableResources) * pImpl->recoveryRate * dt;
    pImpl->availableResources = std::min(1.0f, pImpl->availableResources + recovery);
}

void Synapse::updateSynapticEfficacy(Timestamp currentTime) {
    // Update synaptic efficacy based on recent activity
    float timeSinceLastUpdate = static_cast<float>(currentTime) - pImpl->lastActivityUpdate;
    
    if (timeSinceLastUpdate > 0.0f) {
        // Decay old activity
        pImpl->activityHistory *= std::exp(-timeSinceLastUpdate / 1000.0f);
        
        // Add current activity
        pImpl->activityHistory += 1.0f;
        pImpl->lastActivityUpdate = static_cast<float>(currentTime);
        
        // Modulate efficacy based on activity history
        float activityFactor = std::log10(pImpl->activityHistory + 1.0f) / std::log10(101.0f);
        pImpl->efficacy = std::clamp(1.0f + activityFactor, 0.5f, 2.0f);
    }
}

void Synapse::updateActivityDependentPlasticity(Timestamp currentTime, TimestepDuration dt) {
    // Implement use-dependent synaptic plasticity
    if (!pImpl->plasticityFlags.reward_modulated) return;
    
    // Activity-dependent weight change
    float activityChange = pImpl->activityHistory * 0.0001f * dt;
    
    // Stabilize weight around 0 for balanced networks
    float stabilization = 0.00005f * (0.0f - pImpl->weight) * dt;
    
    float totalDelta = activityChange + stabilization;
    if (std::abs(totalDelta) > 1e-6f) {
        pImpl->weight += totalDelta;
    }
}

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
    pImpl->synapticConductance = 0.0f;
    pImpl->activityHistory = 0.0f;
    pImpl->lastActivityUpdate = 0.0f;
    pImpl->availableResources = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type and biological constraints
    
    // Set reversal potential based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // AMPA receptors: ~0mV
        pImpl->reversalPotential = 0.0f;
        // Excitatory synapses: positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.5f);
        // Excitatory efficacy: moderate to high
        pImpl->efficacy = rng.uniformReal(0.8f, 1.5f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // GABA receptors: ~-70mV
        pImpl->reversalPotential = -70.0f;
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.5f);
        // Inhibitory efficacy: moderate
        pImpl->efficacy = rng.uniformReal(0.7f, 1.2f);
    } else {
        // Other types: small random weights
        pImpl->reversalPotential = 0.0f;
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.1f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Random membrane time constant (5-20ms typical)
    pImpl->membraneTimeConstant = rng.uniformReal(5.0f, 20.0f);
    
    // Random capacitance (0.5-2nF typical)
    pImpl->capacitance = rng.uniformReal(0.5f, 2.0f);
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = rng.uniformReal(0.0f, 0.2f);  // Some initial facilitation
    
    // Initialize activity tracking
    pImpl->activityHistory = 0.0f;
    pImpl->lastActivityUpdate = 0.0f;
    pImpl->availableResources = 1.0f;
    pImpl->releaseProbability = rng.uniformReal(0.3f, 0.7f);
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
