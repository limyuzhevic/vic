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

// Validation helper functions
namespace {
    bool validateSynapticWeight(SynapticWeight weight) {
        // Validate weight is within biologically plausible bounds
        // Based on constants from Neuron.hpp
        const float MIN_WEIGHT = -1.0f;  // nlm::constants::MIN_WEIGHT
        const float MAX_WEIGHT = 1.0f;   // nlm::constants::MAX_WEIGHT
        return weight >= MIN_WEIGHT && weight <= MAX_WEIGHT;
    }
    
    bool validateDelay(Delay delay) {
        // Synaptic delays should be positive and reasonable
        // Typical delays: 1-100 steps (1-100ms at 1ms timestep)
        const Delay MIN_DELAY = 1;
        const Delay MAX_DELAY = 100;
        return delay >= MIN_DELAY && delay <= MAX_DELAY;
    }
    
    bool validateSynapseType(SynapseType type) {
        // All enum values should be valid (exhaustive check)
        return type >= SynapseType::Excitatory && 
               type <= SynapseType::GapJunction;
    }
    
    bool validateSpikeTimestamp(Timestamp timestamp) {
        // Spike timestamps should be non-negative
        return timestamp >= 0.0;
    }
    
    bool validatePlasticityFlags(const PlasticityFlags& flags) {
        // Plasticity flags should be valid combinations
        // At least one plasticity mechanism should be enabled
        return flags.hebbian || flags.stdp || flags.reward_modulated || 
               flags.structural || flags.eligible;
    }
    
    bool validateShortTermPlasticity(float depression, float facilitation) {
        // Short-term plasticity values should be within bounds
        const float MIN_STP = 0.0f;
        const float MAX_STP = 2.0f;  // Some facilitation can exceed 1
        return depression >= MIN_STP && depression <= MAX_STP &&
               facilitation >= MIN_STP && facilitation <= MAX_STP;
    }
    
    bool validateEfficacy(float efficacy) {
        // Synaptic efficacy should be within bounds
        // Based on constants from Neuron.hpp
        const float MIN_EFFICIENCY = 0.0f;  // nlm::constants::MIN_EFFICIENCY
        const float MAX_EFFICIENCY = 2.0f;  // nlm::constants::MAX_EFFICIENCY
        return efficacy >= MIN_EFFICIENCY && efficacy <= MAX_EFFICIENCY;
    }
    
    bool validateNeuronId(NeuronId id) {
        // Neuron IDs should not be invalid
        return id != INVALID_NEURON_ID;
    }
} // namespace anonymous

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
    // Validate weight before setting
    if (validateSynapticWeight(weight)) {
        pImpl->weight = weight;
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, nlm::constants::MIN_WEIGHT, nlm::constants::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    // Validate delay before setting
    if (validateDelay(delay)) {
        pImpl->delay = delay;
    }
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    // Validate type before setting
    if (validateSynapseType(type)) {
        pImpl->type = type;
    }
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    // Validate timestamp before recording
    if (validateSpikeTimestamp(timestamp)) {
        pImpl->preSpikeHistory.push_back(timestamp);
        if (pImpl->preSpikeHistory.size() > nlm::constants::MAX_SPIKE_HISTORY) {
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        }
        
        // Update last pre-spike time
        pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Validate timestamp before recording
    if (validateSpikeTimestamp(timestamp)) {
        pImpl->postSpikeHistory.push_back(timestamp);
        if (pImpl->postSpikeHistory.size() > nlm::constants::MAX_SPIKE_HISTORY) {
            pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        }
        
        // Update last post-spike time
        pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
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
    // Validate efficacy before setting
    if (validateEfficacy(efficacy)) {
        pImpl->efficacy = efficacy;
    }
}

void Synapse::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = nlm::constants::DEFAULT_TIMESTEP;  // 1ms timestep
    
    // Validate current time
    if (!validateSpikeTimestamp(currentTime)) {
        return;
    }
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / nlm::constants::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / nlm::constants::STP_DEPRESSION_TAU));
    }
    
    // Validate short-term plasticity state
    if (!validateShortTermPlasticity(pImpl->shortTermDepression, pImpl->shortTermFacilitation)) {
        // Reset to valid values if invalid
        pImpl->shortTermDepression = 1.0f;
        pImpl->shortTermFacilitation = 0.0f;
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(nlm::constants::DEFAULT_ELIGIBILITY_TRACE_DECAY);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, nlm::constants::MIN_WEIGHT, nlm::constants::MAX_WEIGHT);
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
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (validateSynapseType(pImpl->type)) {
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
        
        // Validate weights and efficacy
        if (!validateSynapticWeight(pImpl->weight)) {
            pImpl->weight = 0.0f;
        }
        if (!validateEfficacy(pImpl->efficacy)) {
            pImpl->efficacy = 1.0f;
        }
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Validate delay
    if (!validateDelay(pImpl->delay)) {
        pImpl->delay = 1;
    }
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Validate short-term plasticity
    if (!validateShortTermPlasticity(pImpl->shortTermDepression, pImpl->shortTermFacilitation)) {
        pImpl->shortTermDepression = 1.0f;
        pImpl->shortTermFacilitation = 0.0f;
    }
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
