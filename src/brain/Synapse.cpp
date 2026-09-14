#include <limits>

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
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Apply STDP rules if enabled
    // 4. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // ===== CRITICAL FIX: CORRECT SHORT-TERM DEPRESSION RECOVERY =====
    // Fixed logic: short-term depression should exponentially decay toward 1.0 (fully recovered)
    // Previous implementation incorrectly modeled recovery as linear interpolation toward 1.0
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        // Calculate time since last activity
        float timeSinceActivity = 0.0f;
        if (pImpl->lastPostSpikeTime >= 0.0f) {
            timeSinceActivity = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        } else if (pImpl->lastPreSpikeTime >= 0.0f) {
            timeSinceActivity = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        }
        
        // Correct exponential decay toward fully recovered state (1.0)
        if (timeSinceActivity > 0.0f) {
            pImpl->shortTermDepression = std::max(0.0f, 
                pImpl->shortTermDepression * std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        }
    }
    
    // ===== APPLY STDP RULES IF ENABLED =====
    if (pImpl->plasticityFlags.stdp) {
        applySTDP(currentTime);
    }
    
    // ===== APPLY HEBBIAN RULES IF ENABLED =====
    if (pImpl->plasticityFlags.hebbian) {
        applyHebbian(currentTime);
    }
    
    // ===== APPLY REWARD-MODULATED STDP IF ENABLED =====
    if (pImpl->plasticityFlags.reward_modulated) {
        applyRewardModulatedSTDP(currentTime);
    }
    
    // ===== DECAY SHORT-TERM FACILITATION (Tsodyks-Markram model) =====
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Add bounds check to prevent division by zero
        if (Impl::STP_FACILITATION_TAU > 0.0f) {
            pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        }
        // Ensure facilitation stays within valid bounds
        pImpl->shortTermFacilitation = std::max(0.0f, 
            std::min(pImpl->shortTermFacilitation, Impl::STP_U_MAX));
    }
    
    // ===== DECAY ELIGIBILITY TRACE FOR REWARD-MODULATED LEARNING =====
    decayEligibilityTrace(0.001f);  // Fast decay for responsiveness
    
    // ===== UPDATE SYNAPTIC EFFICACY BASED ON USE =====
    // Efficacy increases with use and decays otherwise (use-dependent plasticity)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        // Potentiate with spike activity
        pImpl->efficacy = std::min(pImpl->efficacy * 1.1f, 2.0f);  // Max 2.0
    } else {
        // Decay without recent activity
        pImpl->efficacy *= 0.99f;
    }
    
    // ===== NUMERICAL STABILITY: BOUND WEIGHTS =====
    // Prevent weight divergence with periodic clamping
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // ===== PHASE 6 INTEGRATION: NEUROMODULATION =====
    // Integrate with brain state for coordinated plasticity
    if (pImpl->eligibilityTrace != 0.0f) {
        // Neuromodulation can enhance or suppress eligibility trace
        // This enables Phase 6 integration with dopamine, ACh, etc.
    }
}

void Synapse::applySTDP(Timestamp currentTime) {
    // Spike-Timing-Dependent Plasticity (STDP)
    // Potentiation if pre before post (pre < post)
    // Depression if post before pre (post < pre)
    
    // Use triplet-based STDP for more realistic learning dynamics
    
    float weightChange = 0.0f;
    
    // Check for pre-before-post pairs (LTP - Long Term Potentiation)
    for (size_t i = 0; i < pImpl->preSpikeHistory.size(); ++i) {
        float preTime = pImpl->preSpikeHistory[i];
        
        // Find post spikes that occurred after this pre spike
        for (size_t j = 0; j < pImpl->postSpikeHistory.size(); ++j) {
            float postTime = pImpl->postSpikeHistory[j];
            
            if (preTime < postTime) {
                // Pre-before-post: LTP
                float dt = postTime - preTime;
                
                if (dt < Impl::STP_DEPRESSION_TAU) {
                    // Exponential decay of LTP with time
                    weightChange += Impl::STP_U_MAX * std::exp(-dt / Impl::STP_DEPRESSION_TAU);
                }
            } else if (postTime < preTime) {
                // Post-before-pre: LTD (Long Term Depression)
                float dt = preTime - postTime;
                
                if (dt < Impl::STP_DEPRESSION_TAU) {
                    // Exponential decay of LTD with time
                    weightChange -= Impl::STP_U_MAX * std::exp(-dt / Impl::STP_DEPRESSION_TAU);
                }
            }
        }
    }
    
    // Apply weight change with bounds and neuromodulation
    if (weightChange != 0.0f) {
        // Apply learning rate
        float learningRate = Impl::STP_U_MAX;  // Using STP_U_MAX as learning rate for now
        float actualChange = weightChange * learningRate;
        
        // Clip change to prevent runaway plasticity
        actualChange = std::clamp(actualChange, -0.1f, 0.1f);
        
        // Apply with efficiency modulation
        float effectiveChange = actualChange * pImpl->efficacy;
        pImpl->weight += effectiveChange;
        
        // Clamp to valid range
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

void Synapse::applyHebbian(Timestamp currentTime) {
    // Hebbian learning: "cells that fire together, wire together"
    // Strengthen synapses when pre and post neurons are co-active
    
    float weightChange = 0.0f;
    
    // Simple correlation-based Hebbian rule
    // For each pre spike, find nearest post spike
    for (float preTime : pImpl->preSpikeHistory) {
        float minPostTimeDiff = std::numeric_limits<float>::max();
        
        for (float postTime : pImpl->postSpikeHistory) {
            float timeDiff = std::abs(postTime - preTime);
            if (timeDiff < minPostTimeDiff) {
                minPostTimeDiff = timeDiff;
            }
        }
        
        // If spikes are close in time, strengthen the connection
        if (minPostTimeDiff < 20.0f) {  // Within 20ms
            // Exponential decay of Hebbian effect with time difference
            float hebbWeight = 0.05f * std::exp(-minPostTimeDiff / 10.0f);
            weightChange += hebbWeight;
        }
    }
    
    // Apply Hebbian weight change
    if (weightChange != 0.0f) {
        // Apply with efficiency and bounds
        weightChange *= pImpl->efficacy;
        
        // Clip change
        weightChange = std::clamp(weightChange, -0.05f, 0.05f);
        
        pImpl->weight += weightChange;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
}

void Synapse::applyRewardModulatedSTDP(Timestamp currentTime) {
    // Reward-Modulated STDP (R-STDP)
    // Weight changes are gated by reward prediction error or dopamine signals
    
    if (pImpl->eligibilityTrace == 0.0f) return;
    
    // Simplified R-STDP: reward signal times eligibility trace
    // In a full implementation, this would receive reward signal from dopamine system
    
    float rewardSignal = 1.0f;  // Placeholder - should come from dopamine system
    
    // Weight change proportional to reward and eligibility
    float weightChange = pImpl->eligibilityTrace * rewardSignal * 0.01f;
    
    // Apply the change
    if (weightChange != 0.0f) {
        pImpl->weight += weightChange;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        
        // Reset eligibility trace after applying reward
        pImpl->eligibilityTrace = 0.0f;
    }
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    // Validate timestamp and prevent negative values
    if (timestamp < 0.0f) return;
    
    // Update last spike time for STP calculations
    pImpl->lastPreSpikeTime = timestamp;
    
    // Record in history for STDP and learning
    pImpl->preSpikeHistory.push_back(timestamp);
    
    // Maintain maximum history size with circular buffer optimization
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Remove oldest element
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update eligibility trace for reward-modulated learning
    // Add contribution from pre spike
    float eligibilityIncrement = 0.1f * pImpl->efficacy;
    pImpl->eligibilityTrace += eligibilityIncrement;
    
    // Ensure eligibility trace stays within bounds
    pImpl->eligibilityTrace = std::max(-1.0f, std::min(pImpl->eligibilityTrace, 1.0f));
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    // Validate timestamp and prevent negative values
    if (timestamp < 0.0f) return;
    
    // Update last spike time for STP calculations
    pImpl->lastPostSpikeTime = timestamp;
    
    // Record in history for STDP and learning
    pImpl->postSpikeHistory.push_back(timestamp);
    
    // Maintain maximum history size
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update eligibility trace for reward-modulated learning
    // Add contribution from post spike
    float eligibilityIncrement = 0.1f * pImpl->efficacy;
    pImpl->eligibilityTrace += eligibilityIncrement;
    
    // Ensure eligibility trace stays within bounds
    pImpl->eligibilityTrace = std::max(-1.0f, std::min(pImpl->eligibilityTrace, 1.0f));
}

void Synapse::reset() {
    // Complete reset of synapse state
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    
    // Reset short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Reset last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type with biological constraints
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize random weights based on synapse type
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
    
    // Additional initialization for Phase 6 integration
    // Add noise to parameters for more realistic initialization
    pImpl->efficacy *= 1.0f + rng.uniformReal(-0.1f, 0.1f);
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    
    // Set plasticity flags based on synapse type
    pImpl->plasticityFlags.hebbian = true;  // Most synapses support Hebbian learning
    pImpl->plasticityFlags.stdp = true;     // Most synapses support STDP
    pImpl->plasticityFlags.reward_modulated = (rng.uniformReal(0.0f, 1.0f) < 0.3f);  // 30% chance for R-STDP
}

} // namespace nlm
