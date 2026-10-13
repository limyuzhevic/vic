#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

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
    
    // Enhanced short-term plasticity state (Tsodyks-Markram model)
    float utilization;        // Utilization factor (0-1)
    float recovery;           // Recovery variable
    float facilitation;      // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Enhanced weight bounds
    static constexpr float MIN_WEIGHT = -2.0f;
    static constexpr float MAX_WEIGHT = 2.0f;
    static constexpr float MIN_EFFECTIVE_WEIGHT = -1.0f;
    static constexpr float MAX_EFFECTIVE_WEIGHT = 1.0f;
    
    // Enhanced short-term plasticity parameters
    static constexpr float DEFAULT_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float DEFAULT_DEPRESSION_TAU = 200.0f;    // ms
    static constexpr float MAX_FACILITATION_TAU = 1000.0f;
    static constexpr float MAX_DEPRESSION_TAU = 2000.0f;
    static constexpr float MAX_U = 1.0f;  // Max utilization
    static constexpr float MIN_U = 0.0f;  // Min utilization
    
    // Eligibility trace parameters
    static constexpr float DEFAULT_ELIGIBILITY_DECAY = 0.001f;
    static constexpr float MAX_ELIGIBILITY_DECAY = 0.1f;
    static constexpr float MIN_ELIGIBILITY_DECAY = 0.0001f;
    static constexpr float MAX_ELIGIBILITY_TRACE = 1.0f;
    static constexpr float MIN_ELIGIBILITY_TRACE = -1.0f;
    
    // Enhanced efficacy parameters
    static constexpr float MAX_EFFICACY = 2.0f;
    static constexpr float MIN_EFFICACY = 0.1f;
    static constexpr float EFFICACY_DECAY_RATE = 0.98f;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;  // Increased for better analysis
    
    // Default constructor
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), utilization(0.0f), recovery(1.0f),
             facilitation(0.0f), lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             eligibilityTrace(0.0f), efficacy(1.0f) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->utilization = 0.0f;
    pImpl->recovery = 1.0f;
    pImpl->facilitation = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Validate synapse IDs
    if (static_cast<uint64_t>(source) == 0 || static_cast<uint64_t>(destination) == 0) {
        throw std::invalid_argument("Synapse neuron IDs cannot be zero");
    }
    if (source == destination) {
        throw std::invalid_argument("Synapse cannot connect a neuron to itself");
    }
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
    // Validate weight range
    if (weight < Impl::MIN_WEIGHT || weight > Impl::MAX_WEIGHT) {
        throw std::out_of_range("Synaptic weight out of valid range");
    }
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
    if (delay <= 0) {
        throw std::invalid_argument("Synapse delay must be positive");
    }
    if (delay > 1000) {
        throw std::out_of_range("Synapse delay too large");
    }
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
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Spike timestamp cannot be negative");
    }
    pImpl->preSpikeHistory.push_back(timestamp);
    pImpl->lastPreSpikeTime = timestamp;
    
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Spike timestamp cannot be negative");
    }
    pImpl->postSpikeHistory.push_back(timestamp);
    pImpl->lastPostSpikeTime = timestamp;
    
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
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
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
    if (trace < Impl::MIN_ELIGIBILITY_TRACE || trace > Impl::MAX_ELIGIBILITY_TRACE) {
        throw std::out_of_range("Eligibility trace out of valid range");
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    // Validate decay rate
    if (decayRate < Impl::MIN_ELIGIBILITY_DECAY || decayRate > Impl::MAX_ELIGIBILITY_DECAY) {
        throw std::out_of_range("Eligibility decay rate out of valid range");
    }
    
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    if (efficacy < Impl::MIN_EFFICACY || efficacy > Impl::MAX_EFFICACY) {
        throw std::out_of_range("Synaptic efficacy out of valid range");
    }
    pImpl->efficacy = efficacy;
}

void Synapse::step(Timestamp currentTime) {
    // Enhanced synaptic dynamics:
    // 1. Decay short-term plasticity state with numerical stability
    // 2. Decay eligibility trace with validated decay rate
    // 3. Update synaptic efficacy with decay
    // 4. Maintain weight bounds
    
    // Use configurable timestep
    TimestepDuration dt = 0.001f;  // 1ms timestep
    
    // Enhanced Tsodyks-Markram short-term plasticity model
    // Facilitation dynamics
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Numerically stable exponential decay
        float decayFactor = std::exp(-timeSincePre / Impl::DEFAULT_FACILITATION_TAU);
        pImpl->facilitation *= decayFactor;
        
        // Clamp facilitation to prevent numerical issues
        if (pImpl->facilitation < 0.0f) pImpl->facilitation = 0.0f;
        if (pImpl->facilitation > Impl::MAX_U) pImpl->facilitation = Impl::MAX_U;
    }
    
    // Depression dynamics
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        
        // Numerically stable recovery calculation
        float recoveryFactor = 1.0f - std::exp(-timeSinceActivity / Impl::DEFAULT_DEPRESSION_TAU);
        pImpl->recovery += (1.0f - pImpl->recovery) * recoveryFactor;
        
        // Clamp recovery to prevent numerical issues
        if (pImpl->recovery < 0.0f) pImpl->recovery = 0.0f;
        if (pImpl->recovery > 1.0f) pImpl->recovery = 1.0f;
    }
    
    // Decay eligibility trace with safe decay rate
    decayEligibilityTrace(Impl::DEFAULT_ELIGIBILITY_DECAY);
    
    // Update utilization based on recent activity
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->utilization = std::max(0.0f, pImpl->utilization - timeSincePre / Impl::DEFAULT_DEPRESSION_TAU);
        if (pImpl->utilization < 0.0f) pImpl->utilization = 0.0f;
    }
    
    // Decay synaptic efficacy
    pImpl->efficacy *= Impl::EFFICACY_DECAY_RATE;
    if (pImpl->efficacy < Impl::MIN_EFFICACY) pImpl->efficacy = Impl::MIN_EFFICACY;
    if (pImpl->efficacy > Impl::MAX_EFFICACY) pImpl->efficacy = Impl::MAX_EFFICACY;
    
    // Effective weight calculation considering STP
    float effectiveWeight = pImpl->weight * pImpl->utilization * pImpl->efficacy;
    
    // Clamp effective weight to prevent numerical issues
    if (effectiveWeight < Impl::MIN_EFFECTIVE_WEIGHT) {
        effectiveWeight = Impl::MIN_EFFECTIVE_WEIGHT;
    } else if (effectiveWeight > Impl::MAX_EFFECTIVE_WEIGHT) {
        effectiveWeight = Impl::MAX_EFFECTIVE_WEIGHT;
    }
    
    // Clamp main weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->utilization = 0.0f;
    pImpl->recovery = 1.0f;
    pImpl->facilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Enhanced random initialization with biological constraints and validation
    try {
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
        
        // Validate weight range
        if (pImpl->weight < Impl::MIN_WEIGHT || pImpl->weight > Impl::MAX_WEIGHT) {
            pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        }
        
        // Validate efficacy range
        if (pImpl->efficacy < Impl::MIN_EFFICACY || pImpl->efficacy > Impl::MAX_EFFICACY) {
            pImpl->efficacy = std::clamp(pImpl->efficacy, Impl::MIN_EFFICACY, Impl::MAX_EFFICACY);
        }
        
        // Random delay: 1-100 steps (more biologically realistic)
        pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 100));
        
        // Initialize short-term plasticity state with realistic values
        pImpl->utilization = 0.0f;  // Initially no utilization
        pImpl->recovery = 1.0f;     // Fully recovered
        pImpl->facilitation = 0.0f;  // No initial facilitation
        
        // Initialize eligibility trace to 0
        pImpl->eligibilityTrace = 0.0f;
        
        // Initialize last spike times
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
        
    } catch (const std::exception& e) {
        // If initialization fails, set safe defaults
        reset();
    }
}

} // namespace nlm
