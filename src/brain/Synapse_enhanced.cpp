#include <stdexcept>
#include <limits>
#include <cmath>

// Enhanced Synapse implementation with improved validation and error handling
namespace nlm {

class SynapseValidationError : public std::runtime_error {
public:
    explicit SynapseValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    if (id.value == 0) {
        throw SynapseValidationError("SynapseId cannot be zero");
    }
    if (source.value == 0) {
        throw SynapseValidationError("Source NeuronId cannot be zero");
    }
    if (destination.value == 0) {
        throw SynapseValidationError("Destination NeuronId cannot be zero");
    }
    
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
    if (std::isnan(weight)) {
        throw SynapseValidationError("Cannot set synapse weight to NaN");
    }
    if (std::isinf(weight)) {
        throw SynapseValidationError("Cannot set synapse weight to infinity");
    }
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (std::isnan(delta) || std::isinf(delta)) {
        throw SynapseValidationError("Cannot add NaN or infinite delta to synapse weight");
    }
    
    float newWeight = pImpl->weight + delta;
    if (newWeight < Impl::MIN_WEIGHT || newWeight > Impl::MAX_WEIGHT) {
        throw SynapseValidationError("New weight would be out of bounds [" + 
                                   std::to_string(Impl::MIN_WEIGHT) + ", " +
                                   std::to_string(Impl::MAX_WEIGHT) + "]");
    }
    
    pImpl->weight = newWeight;
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    if (delay < 1) {
        throw SynapseValidationError("Synapse delay must be at least 1 step");
    }
    if (delay > 1000) {
        throw SynapseValidationError("Synapse delay too large (max 1000 steps)");
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
        throw SynapseValidationError("Spike timestamp cannot be negative");
    }
    if (timestamp > 1e6f) {
        throw SynapseValidationError("Spike timestamp unreasonably large");
    }
    
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update last spike time
    pImpl->lastPreSpikeTime = timestamp;
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw SynapseValidationError("Spike timestamp cannot be negative");
    }
    if (timestamp > 1e6f) {
        throw SynapseValidationError("Spike timestamp unreasonably large");
    }
    
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update last spike time
    pImpl->lastPostSpikeTime = timestamp;
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
    for (Timestamp ts : history) {
        if (ts < 0.0f) {
            throw SynapseValidationError("Spike timestamp cannot be negative");
        }
    }
    
    pImpl->preSpikeHistory = history;
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(
            pImpl->preSpikeHistory.begin(),
            pImpl->preSpikeHistory.begin() +
                static_cast<std::ptrdiff_t>(pImpl->preSpikeHistory.size() -
                                            Impl::MAX_SPIKE_HISTORY));
    }
    
    // Update last spike time
    if (!pImpl->preSpikeHistory.empty()) {
        pImpl->lastPreSpikeTime = pImpl->preSpikeHistory.back();
    }
}

void Synapse::setPostSpikeHistory(const std::vector<Timestamp>& history) {
    for (Timestamp ts : history) {
        if (ts < 0.0f) {
            throw SynapseValidationError("Spike timestamp cannot be negative");
        }
    }
    
    pImpl->postSpikeHistory = history;
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(
            pImpl->postSpikeHistory.begin(),
            pImpl->postSpikeHistory.begin() +
                static_cast<std::ptrdiff_t>(pImpl->postSpikeHistory.size() -
                                            Impl::MAX_SPIKE_HISTORY));
    }
    
    // Update last spike time
    if (!pImpl->postSpikeHistory.empty()) {
        pImpl->lastPostSpikeTime = pImpl->postSpikeHistory.back();
    }
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    if (std::isnan(trace) || std::isinf(trace)) {
        throw SynapseValidationError("Eligibility trace cannot be NaN or infinity");
    }
    if (trace < -10.0f || trace > 10.0f) {
        throw SynapseValidationError("Eligibility trace out of reasonable range (-10 to 10)");
    }
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (decayRate < 0.0f || decayRate > 1.0f) {
        throw SynapseValidationError("Decay rate must be between 0 and 1");
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
    if (std::isnan(efficacy) || std::isinf(efficacy)) {
        throw SynapseValidationError("Efficacy cannot be NaN or infinity");
    }
    if (efficacy < 0.0f || efficacy > 2.0f) {
        throw SynapseValidationError("Efficacy out of reasonable range (0-2)");
    }
    pImpl->efficacy = efficacy;
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Validate current time
    if (currentTime < 0.0f) {
        throw SynapseValidationError("Current time cannot be negative");
    }
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        if (timeSincePre < 0.0f) {
            throw SynapseValidationError("Time since pre-spike cannot be negative");
        }
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
    
    // Clamp values to reasonable ranges
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
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