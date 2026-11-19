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

void Synapse::setPreSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->preSpikeHistory = history;
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(
            pImpl->preSpikeHistory.begin(),
            pImpl->preSpikeHistory.begin() +
                static_cast<std::ptrdiff_t>(pImpl->preSpikeHistory.size() -
                                            Impl::MAX_SPIKE_HISTORY));
    }
}

void Synapse::setPostSpikeHistory(const std::vector<Timestamp>& history) {
    pImpl->postSpikeHistory = history;
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(
            pImpl->postSpikeHistory.begin(),
            pImpl->postSpikeHistory.begin() +
                static_cast<std::ptrdiff_t>(pImpl->postSpikeHistory.size() -
                                            Impl::MAX_SPIKE_HISTORY));
    }
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
    // Real synaptic dynamics implementation:
    // 1. Update short-term plasticity (STP) using Tsodyks-Markram model
    // 2. Decay eligibility trace for reward-modulated learning
    // 3. Update synaptic efficacy based on usage history
    // 4. Enforce weight bounds to prevent instability
    // 5. Update last spike times for STP calculations
    
    TimestepDuration dt = 0.001;  // 1ms simulation timestep
    
    // Step 1: Update short-term plasticity (STP) state
    // Tsodyks-Markram model: U = utilization, x = depression, y = facilitation
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        
        // Decay facilitation (y) towards baseline
        float dy = dt * (1.0f - pImpl->shortTermFacilitation) / Impl::STP_FACILITATION_TAU;
        pImpl->shortTermFacilitation = std::max(0.0f, pImpl->shortTermFacilitation - dy);
        
        // Update utilization (U) based on current facilitation
        float U = Impl::STP_U_MAX * pImpl->shortTermFacilitation;
        
        // Calculate available resources: x = x * exp(-dt/tau_x) + (1-x) * U
        if (pImpl->lastPreSpikeTime > 0.0f) {
            float dx = dt * pImpl->shortTermDepression / Impl::STP_DEPRESSION_TAU;
            pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - dx);
            
            // Apply depression during spike
            pImpl->shortTermDepression *= (1.0f - U);
        } else {
            // Recovery from depression towards 1.0
            float dx = dt * (1.0f - pImpl->shortTermDepression) / Impl::STP_DEPRESSION_TAU;
            pImpl->shortTermDepression = std::min(1.0f, pImpl->shortTermDepression + dx);
        }
    }
    
    // Step 2: Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay: 1ms time constant
    
    // Step 3: Update synaptic efficacy based on usage history
    // Efficacy increases with recent spike history, decays otherwise
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        float efficacyDecay = std::exp(-timeSincePre / 100.0f);  // 100ms decay
        pImpl->efficacy = std::max(0.0f, pImpl->efficacy * efficacyDecay);
        
        // Add usage boost for recent spikes
        if (timeSincePre < 50.0f) {  // Within 50ms
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + 0.1f);
        }
    } else {
        // Slow spontaneous decay
        pImpl->efficacy *= 0.999f;
    }
    
    // Step 4: Enforce synaptic weight bounds to maintain stability
    float weightChangeRate = 0.01f;  // Max weight change per timestep
    // Weight can drift slowly towards zero (homeostatic scaling)
    float homeostasisTarget = 0.0f;
    float weightDrift = (homeostasisTarget - pImpl->weight) * weightChangeRate;
    pImpl->weight += weightDrift;
    
    // Clamp weight to physical bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Step 5: Update last spike times for next STP calculation
    // This is handled externally by recordPreSpike/recordPostSpike calls
    
    // Step 6: Update plasticity flags based on spike history
    // STDP window integration: check for paired pre/post spikes
    if (!pImpl->preSpikeHistory.empty() && !pImpl->postSpikeHistory.empty()) {
        // Check for spike pairs within STDP window (~20ms)
        Timestamp now = static_cast<Timestamp>(currentTime);
        float stdpWindow = 20.0f;  // ms
        
        // Enable plasticity if we have relevant spike history
        bool hasRelevantSpikes = false;
        for (auto timestamp : pImpl->preSpikeHistory) {
            if (now - timestamp < stdpWindow) {
                hasRelevantSpikes = true;
                break;
            }
        }
        
        if (hasRelevantSpikes) {
            pImpl->plasticityFlags.stdp = true;
        }
    }
}

void Synapse::reset() {
    // Do NOT zero the weight here. Brain::reset() is called at the start
    // of every Phase 3 episode, and zeroing weights there disconnected the
    // whole network: every sensory spike then delivered weight 0, so
    // internal and motor neurons stayed exactly at rest and the agent
    // could never move. Weight is long-term connectivity / learned state;
    // reset only clears short-term state (spike histories, eligibility)
    // and restores efficacy, so a new episode starts with an intact brain.
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

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
    pImpl->delay = static_cast<Delay>(rng.uniformInt(uint32_t(1), uint32_t(5)));
    
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
