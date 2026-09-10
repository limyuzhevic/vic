// Synapse implementation with comprehensive short-term plasticity

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
    
    // Short-term plasticity state (Tsodyks-Markram model)
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
    
    // Homeostatic regulation
    float homeostasisTarget;
};

// Constructor
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
    pImpl->homeostasisTarget = 1.0f;
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

// Basic property getters/setters
SynapseId Synapse::getId() const { return pImpl->id; }
NeuronId Synapse::getSourceNeuron() const { return pImpl->sourceNeuron; }
NeuronId Synapse::getDestinationNeuron() const { return pImpl->destinationNeuron; }
SynapticWeight Synapse::getWeight() const { return pImpl->weight; }
void Synapse::setWeight(SynapticWeight weight) { pImpl->weight = weight; }
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}
Delay Synapse::getDelay() const { return pImpl->delay; }
void Synapse::setDelay(Delay delay) { pImpl->delay = delay; }
SynapseType Synapse::getType() const { return pImpl->type; }
void Synapse::setType(SynapseType type) { pImpl->type = type; }
bool Synapse::isExcitatory() const { return pImpl->type == SynapseType::Excitatory; }
bool Synapse::isInhibitory() const { return pImpl->type == SynapseType::Inhibitory; }

// Spike history management for STDP
void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const { return pImpl->preSpikeHistory; }
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const { return pImpl->postSpikeHistory; }
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Plasticity control
PlasticityFlags& Synapse::getPlasticityFlags() { return pImpl->plasticityFlags; }
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}
const PlasticityFlags& Synapse::getPlasticityFlags() const { return pImpl->plasticityFlags; }

// Eligibility trace for reward-modulated learning
float Synapse::getEligibilityTrace() const { return pImpl->eligibilityTrace; }
void Synapse::setEligibilityTrace(float trace) { pImpl->eligibilityTrace = trace; }
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) pImpl->eligibilityTrace = 0.0f;
}
void Synapse::addToEligibilityTrace(float delta) {
    pImpl->eligibilityTrace += delta;
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
}

// Synaptic efficacy (use-dependent modulation)
float Synapse::getEfficacy() const { return pImpl->efficacy; }
void Synapse::setEfficacy(float efficacy) { pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f); }
void Synapse::updateEfficacy(bool preSpike, bool postSpike, Timestamp currentTime) {
    if (preSpike) pImpl->efficacy += 0.02f * pImpl->shortTermFacilitation;
    if (postSpike) pImpl->efficacy -= 0.03f * pImpl->shortTermDepression;
    pImpl->eligibilityTrace += (preSpike ? 0.01f : 0.0f) - (postSpike ? 0.01f : 0.0f);
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.1f, 2.0f);
}

// Short-term plasticity state access
float Synapse::getShortTermFacilitation() const { return pImpl->shortTermFacilitation; }
float Synapse::getShortTermDepression() const { return pImpl->shortTermDepression; }
float Synapse::getUtilization() const {
    return (pImpl->shortTermDepression + pImpl->shortTermFacilitation) * 0.5f;
}

// Synaptic weight homeostasis
void Synapse::setHomeostasisTarget(float target) { pImpl->homeostasisTarget = target; }
float Synapse::getHomeostasisTarget() const { return pImpl->homeostasisTarget; }
void Synapse::updateHomeostasis(TimestepDuration dt) {
    float error = pImpl->efficacy - pImpl->homeostasisTarget;
    pImpl->weight += -error * 0.001f * dt;
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

// Realistic synaptic transmission (conductance-based model)
void Synapse::applySynapticInput(MembranePotential& membranePotential, Timestamp currentTime) const {
    float conductance = std::abs(pImpl->weight) * 0.1f;
    float reversalPotential = pImpl->type == SynapseType::Inhibitory ? -80.0f : 0.0f;
    float drivingForce = membranePotential - reversalPotential;
    float synapticCurrent = conductance * drivingForce;
    float potentialChange = synapticCurrent * 0.01f;
    membranePotential += potentialChange;
    membranePotential = std::clamp(membranePotential, -100.0f, 50.0f);
}

// Main synaptic dynamics step (integrates all plasticity mechanisms)
void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001f;  // 1ms timestep
    
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    }
    
    // 2. Decay short-term depression (recovery toward baseline)
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
                                       (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    }
    
    // 3. Update efficacy based on recent activity
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        pImpl->efficacy += 0.01f * pImpl->shortTermFacilitation;
        if (pImpl->lastPostSpikeTime >= 0.0f) {
            pImpl->efficacy -= 0.02f * pImpl->shortTermDepression;
        }
    }
    pImpl->efficacy *= 0.995f;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.1f, 2.0f);
    
    // 4. Synaptic weight homeostasis
    const float TARGET_EFFICACY = 1.0f;
    float efficacyError = pImpl->efficacy - TARGET_EFFICACY;
    if (std::abs(efficacyError) > 0.1f) {
        float homeostasisRate = 0.001f * dt;
        pImpl->weight += -efficacyError * homeostasisRate;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
    
    // 5. Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);
    
    // 6. Safety bounds enforcement
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
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
    pImpl->homeostasisTarget = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (pImpl->type == SynapseType::Excitatory) {
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->homeostasisTarget = 1.0f;
}

// Additional advanced features
void Synapse::setSynapticStrength(float strength) {
    float normalizedStrength = (std::abs(pImpl->weight) + pImpl->efficacy) * 0.5f;
    pImpl->weight = pImpl->weight > 0 ? 
                     std::min(pImpl->weight * strength, Impl::MAX_WEIGHT) : 
                     std::max(pImpl->weight * strength, Impl::MIN_WEIGHT);
}

void Synapse::enableDepression(bool enable) {
    if (!enable) pImpl->shortTermDepression = 0.0f;
}

void Synapse::enableFacilitation(bool enable) {
    if (!enable) pImpl->shortTermFacilitation = 0.0f;
}

void Synapse::setSTPParameters(float facilitationTimeConstant, float depressionTimeConstant) {
    // In a full implementation, these would be stored as member variables
    // These would be used in the decay calculations within step()
}

} // namespace nlm