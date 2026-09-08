#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <numeric>
#include <random>

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
    // Validate synapse ID
    if (id == INVALID_SYNAPSE_ID) {
        NLM_LOG_ERROR("Synapse: Cannot create synapse with invalid ID");
        throw std::invalid_argument("Synapse ID is invalid");
    }
    
    // Validate neuron IDs
    if (source == INVALID_NEURON_ID || destination == INVALID_NEURON_ID) {
        NLM_LOG_ERROR("Synapse: Invalid source or destination neuron ID");
        throw std::invalid_argument("Synapse requires valid source and destination neuron IDs");
    }
    
    if (source == destination) {
        NLM_LOG_WARNING("Synapse: Self-connection detected - neurons cannot connect to themselves");
    }
    
    pImpl.reset(new Impl());
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
    
    NLM_LOG_DEBUG("Synapse: Created synapse " + std::to_string(id.index()) + 
                 " (source: " + std::to_string(source.index()) + 
                 ", destination: " + std::to_string(destination.index()) + ")");
}

Synapse::~Synapse() {
    pImpl.reset();
}

Synapse::Synapse(Synapse&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr;
    NLM_LOG_DEBUG("Synapse: Move constructor completed");
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = std::move(other.pImpl);
        other.pImpl = nullptr;
        NLM_LOG_DEBUG("Synapse: Move assignment completed");
    }
    return *this;
}

SynapseId Synapse::getId() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get ID - pImpl is null");
        return INVALID_SYNAPSE_ID;
    }
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get source neuron - pImpl is null");
        return INVALID_NEURON_ID;
    }
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get destination neuron - pImpl is null");
        return INVALID_NEURON_ID;
    }
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get weight - pImpl is null");
        return 0.0f;
    }
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    // Validate input
    if (std::isnan(weight) || std::isinf(weight)) {
        NLM_LOG_ERROR("Synapse: Invalid weight value");
        return;
    }
    
    // Clamp to reasonable bounds
    if (pImpl) {
        pImpl->weight = std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        NLM_LOG_DEBUG("Synapse: Set weight to " + std::to_string(pImpl->weight) + 
                     " (clamped to [" + std::to_string(Impl::MIN_WEIGHT) + ", " + 
                     std::to_string(Impl::MAX_WEIGHT) + "])");
    } else {
        NLM_LOG_ERROR("Synapse: Cannot set weight - pImpl is null");
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    // Validate input
    if (std::isnan(delta) || std::isinf(delta)) {
        NLM_LOG_ERROR("Synapse: Invalid weight delta");
        return;
    }
    
    if (pImpl) {
        float newWeight = pImpl->weight + delta;
        pImpl->weight = std::clamp(newWeight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        NLM_LOG_DEBUG("Synapse: Adjusted weight by " + std::to_string(delta) + 
                     " to " + std::to_string(pImpl->weight));
    } else {
        NLM_LOG_ERROR("Synapse: Cannot adjust weight - pImpl is null");
    }
}

Delay Synapse::getDelay() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get delay - pImpl is null");
        return 0;
    }
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    // Validate input
    if (delay < 0) {
        NLM_LOG_ERROR("Synapse: Invalid delay value (must be non-negative)");
        return;
    }
    
    if (delay > 1000) {  // Reasonable upper bound
        NLM_LOG_WARNING("Synapse: Unusually high delay: " + std::to_string(delay) + " steps");
    }
    
    if (pImpl) {
        pImpl->delay = delay;
        NLM_LOG_DEBUG("Synapse: Set delay to " + std::to_string(delay) + " steps");
    } else {
        NLM_LOG_ERROR("Synapse: Cannot set delay - pImpl is null");
    }
}

SynapseType Synapse::getType() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get type - pImpl is null");
        return SynapseType::Excitatory;
    }
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    if (pImpl) {
        pImpl->type = type;
        NLM_LOG_DEBUG("Synapse: Set type to " + std::to_string(static_cast<int>(type)));
    } else {
        NLM_LOG_ERROR("Synapse: Cannot set type - pImpl is null");
    }
}

bool Synapse::isExcitatory() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot check type - pImpl is null");
        return false;
    }
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot check type - pImpl is null");
        return false;
    }
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    // Validate input
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        NLM_LOG_ERROR("Synapse: Invalid spike timestamp");
        return;
    }
    
    if (pImpl) {
        pImpl->preSpikeHistory.push_back(timestamp);
        if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        }
        
        // Update last spike time for short-term plasticity
        pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
        
        NLM_LOG_DEBUG("Synapse: Recorded pre-synaptic spike at timestamp " + std::to_string(timestamp));
    } else {
        NLM_LOG_ERROR("Synapse: Cannot record spike - pImpl is null");
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Validate input
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        NLM_LOG_ERROR("Synapse: Invalid spike timestamp");
        return;
    }
    
    if (pImpl) {
        pImpl->postSpikeHistory.push_back(timestamp);
        if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        }
        
        // Update last spike time for short-term plasticity
        pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
        
        NLM_LOG_DEBUG("Synapse: Recorded post-synaptic spike at timestamp " + std::to_string(timestamp));
    } else {
        NLM_LOG_ERROR("Synapse: Cannot record spike - pImpl is null");
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get pre-spike history - pImpl is null");
        static const std::vector<Timestamp> empty;
        return empty;
    }
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get post-spike history - pImpl is null");
        static const std::vector<Timestamp> empty;
        return empty;
    }
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    if (pImpl) {
        size_t preSize = pImpl->preSpikeHistory.size();
        size_t postSize = pImpl->postSpikeHistory.size();
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->eligibilityTrace = 0.0f;
        
        NLM_LOG_DEBUG("Synapse: Cleared spike history (" + std::to_string(preSize) + " pre, " + 
                     std::to_string(postSize) + " post spikes)");
    } else {
        NLM_LOG_ERROR("Synapse: Cannot clear history - pImpl is null");
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get plasticity flags - pImpl is null");
        static PlasticityFlags flags{};
        return flags;
    }
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
        
        NLM_LOG_DEBUG("Synapse: Enabled plasticity (Hebbian: " + std::to_string(hebbian) + 
                     ", STDP: " + std::to_string(stdp) + ", reward-modulated: " + 
                     std::to_string(rewardModulated) + ")");
    } else {
        NLM_LOG_ERROR("Synapse: Cannot enable plasticity - pImpl is null");
    }
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get plasticity flags - pImpl is null");
        static const PlasticityFlags flags{};
        return flags;
    }
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get eligibility trace - pImpl is null");
        return 0.0f;
    }
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    // Validate input
    if (std::isnan(trace) || std::isinf(trace)) {
        NLM_LOG_ERROR("Synapse: Invalid eligibility trace value");
        return;
    }
    
    if (pImpl) {
        pImpl->eligibilityTrace = trace;
        NLM_LOG_DEBUG("Synapse: Set eligibility trace to " + std::to_string(trace));
    } else {
        NLM_LOG_ERROR("Synapse: Cannot set eligibility trace - pImpl is null");
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    // Validate input
    if (std::isnan(decayRate) || std::isinf(decayRate) || decayRate < 0.0f || decayRate > 1.0f) {
        NLM_LOG_ERROR("Synapse: Invalid eligibility trace decay rate");
        return;
    }
    
    if (pImpl) {
        pImpl->eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
        
        NLM_LOG_DEBUG("Synapse: Decayed eligibility trace (rate: " + std::to_string(decayRate) + 
                     ", value: " + std::to_string(pImpl->eligibilityTrace) + ")");
    } else {
        NLM_LOG_ERROR("Synapse: Cannot decay eligibility trace - pImpl is null");
    }
}

float Synapse::getEfficacy() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot get efficacy - pImpl is null");
        return 1.0f;
    }
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    // Validate input
    if (std::isnan(efficacy) || std::isinf(efficacy) || efficacy < 0.0f || efficacy > 2.0f) {
        NLM_LOG_ERROR("Synapse: Invalid efficacy value");
        return;
    }
    
    if (pImpl) {
        pImpl->efficacy = efficacy;
        NLM_LOG_DEBUG("Synapse: Set efficacy to " + std::to_string(efficacy));
    } else {
        NLM_LOG_ERROR("Synapse: Cannot set efficacy - pImpl is null");
    }
}

void Synapse::step(Timestamp currentTime) {
    // Validate current time and pImpl
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot step - pImpl is null");
        return;
    }
    
    if (std::isnan(currentTime) || std::isinf(currentTime)) {
        NLM_LOG_ERROR("Synapse: Invalid current time for step");
        return;
    }
    
    NLM_LOG_DEBUG("Synapse: Stepping at time " + std::to_string(currentTime) + 
                 " (weight: " + std::to_string(pImpl->weight) + ", efficacy: " + std::to_string(pImpl->efficacy) + ")");
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        
        // Validate facilitation stays in valid range
        if (pImpl->shortTermFacilitation < 0.0f) pImpl->shortTermFacilitation = 0.0f;
        if (pImpl->shortTermFacilitation > 1.0f) pImpl->shortTermFacilitation = 1.0f;
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        
        // Validate depression stays in valid range
        if (pImpl->shortTermDepression < 0.0f) pImpl->shortTermDepression = 0.0f;
        if (pImpl->shortTermDepression > 1.0f) pImpl->shortTermDepression = 1.0f;
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Log short-term plasticity state
    NLM_LOG_DEBUG("Synapse: STP state (facilitation: " + std::to_string(pImpl->shortTermFacilitation) + 
                 ", depression: " + std::to_string(pImpl->shortTermDepression) + ")");
}

void Synapse::reset() {
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot reset - pImpl is null");
        return;
    }
    
    NLM_LOG_DEBUG("Synapse: Resetting synapse");
    
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
    // Validate random generator and pImpl
    if (!rng.isValid()) {
        NLM_LOG_ERROR("Synapse: Invalid random generator for initialization");
        return;
    }
    
    if (!pImpl) {
        NLM_LOG_ERROR("Synapse: Cannot initialize - pImpl is null");
        return;
    }
    
    NLM_LOG_DEBUG("Synapse: Initializing with random parameters");
    
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        NLM_LOG_DEBUG("Synapse: Initialized excitatory synapse (weight: " + std::to_string(pImpl->weight) + ")");
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        NLM_LOG_DEBUG("Synapse: Initialized inhibitory synapse (weight: " + std::to_string(pImpl->weight) + ")");
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
        NLM_LOG_DEBUG("Synapse: Initialized other synapse (weight: " + std::to_string(pImpl->weight) + ")");
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    NLM_LOG_DEBUG("Synapse: Set random delay to " + std::to_string(pImpl->delay) + " steps");
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    NLM_LOG_DEBUG("Synapse: Random initialization complete (weight: " + std::to_string(pImpl->weight) + 
                 ", delay: " + std::to_string(pImpl->delay) + ")");
}

} // namespace nlm
