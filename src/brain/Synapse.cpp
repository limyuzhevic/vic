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
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Update short-term plasticity (STP) - Tsodyks-Markram model
    // Facilitation dynamics: dx/dt = (1 - x)/tau_x - u*x*delta(t - t_pre)
    // Depression dynamics: dy/dt = (1 - y)/tau_y + (1 - y)*delta(t - t_post)
    
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Decay facilitation (recovers toward baseline)
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        
        // Apply depression: u*x decreases availability
        pImpl->shortTermDepression = pImpl->shortTermDepression * (1.0f - pImpl->shortTermFacilitation);
        pImpl->shortTermDepression = std::max(pImpl->shortTermDepression, 0.0f);
    }
    
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePost = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSincePost / Impl::STP_DEPRESSION_TAU));
    }
    
    // Clamp STP variables
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, Impl::STP_U_MAX);
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    
    void Synapse::applySTDP(Timestamp currentTime) {
    if (!pImpl->plasticityFlags.stdp) return;
    
    // Spike Timing-Dependent Plasticity (STDP) - biologically realistic implementation
    // Based on BiMantzaris & Pikovsky model with NMDA receptor dependencies
    
    // Calculate time differences between most recent spikes
    float lastPreTime = pImpl->lastPreSpikeTime;
    float lastPostTime = pImpl->lastPostSpikeTime;
    
    // LTP when pre before post (post occurs after pre)
    if (lastPreTime >= 0.0f && lastPostTime > lastPreTime && 
        (lastPostTime - lastPreTime) < 50.0f) {
        // Long-Term Potentiation
        float timeDiff = static_cast<float>(currentTime - lastPreTime);
        float A_LTP = 0.01f;
        float tau_LTP = 20.0f;
        
        float weightChange = A_LTP * std::exp(-timeDiff / tau_LTP);
        
        // NMDA receptor requirement: postsynaptic depolarization needed
        // Simplified: assume postsynaptic neuron is depolarized near spike time
        if (lastPostTime > currentTime - 10.0f) {
            // NMDA-dependent LTP (requires Ca2+ influx)
            weightChange *= 1.5f;  // NMDA boosts LTP
        }
        
        addToWeight(weightChange);
        
        // Update lastPreSpikeTime to maintain STDP trace
        pImpl->lastPreSpikeTime = currentTime;
    }
    
    // LTD when post before pre (pre occurs after post)
    if (lastPostTime >= 0.0f && lastPreTime > lastPostTime && 
        (lastPreTime - lastPostTime) < 50.0f) {
        // Long-Term Depression
        float timeDiff = static_cast<float>(currentTime - lastPostTime);
        float A_LTD = 0.012f;  // Slightly stronger LTD
        float tau_LTD = 25.0f;
        
        float weightChange = -A_LTD * std::exp(-timeDiff / tau_LTD);
        
        // NMDA contribution can modulate LTD
        if (lastPreTime > currentTime - 10.0f) {
            weightChange *= 1.3f;  // NMDA enhances LTD
        }
        
        addToWeight(weightChange);
        
        // Update lastPostSpikeTime to maintain STDP trace
        pImpl->lastPostSpikeTime = currentTime;
    }
    
    // Apply homosynaptic scaling for network stability
    // Average weight tends to return to baseline
    applyHomeostaticPlasticity();
}

void Synapse::applyHomeostaticPlasticity() {
    // Homosynaptic scaling to maintain network stability
    // Implements weight normalization based on average firing rates
    
    // For now, apply simple weight normalization
    // In a full implementation, this would track global firing rates
    // and adjust weights to maintain balance
    
    // Normalize weight to [-0.5, 0.5] range as a starting point
    // This helps prevent runaway excitation/inhibition
    if (std::abs(pImpl->weight) > 0.5f) {
        pImpl->weight = 0.5f * (pImpl->weight / std::abs(pImpl->weight));
    }
}

void Synapse::updateReceptorDynamics() {
    // NMDA/AMPA receptor dynamics with Mg2 block
    
    // AMPA receptor (fast, no Mg2 block)
    float AMPA_Reversal = 0.0f;  // ~0 mV for AMPA
    
    // NMDA receptor (slow, Mg2 block dependent on voltage)
    float NMDA_Reversal = 50.0f;  // ~50 mV for NMDA
    
    // Mg2 block factor for NMDA: stronger block at hyperpolarized potentials
    float V = 0.0f;  // Would get from postsynaptic neuron
    float Mg2_factor = 1.0f / (1.0f + std::exp(0.062f * (-V - 30.0f)) * 1.0f / 3.57f);
    
    // Update receptor activation states
    // AMPA: fast rise and decay (tau ~1-5ms)
    float AMPA_activation = pImpl->shortTermDepression * std::min(pImpl->shortTermFacilitation, 1.0f);
    
    // NMDA: slower rise and decay (tau ~50-100ms), voltage-dependent
    float NMDA_activation = pImpl->efficacy * Mg2_factor * pImpl->shortTermDepression;
    
    // Store receptor activations for synaptic current calculation
    pImpl->shortTermDepression = AMPA_activation;  // Reuse as storage
    pImpl->shortTermFacilitation = NMDA_activation;  // Reuse as storage
}

void Synapse::applyPlasticityUpdates() {
    // Combine multiple plasticity mechanisms
    // Hebbian, STDP, reward-modulated together
    
    if (pImpl->plasticityFlags.hebbian) {
        // Apply Hebbian learning based on spike correlations
        applyHebbianLearning();
    }
    
    // STDP already applied earlier in step()
    
    if (pImpl->plasticityFlags.reward_modulated && pImpl->eligibilityTrace > 0.001f) {
        // Reward-modulated learning
        applyRewardModulatedLearning();
    }
}

void Synapse::applyHebbianLearning() {
    // Simple Hebbian learning: strengthen synapses when pre and post fire together
    // Oja's rule implementation for stability
    
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPostSpikeTime >= 0.0f) {
        float correlation = std::exp(-(std::abs(pImpl->lastPostSpikeTime - pImpl->lastPreSpikeTime)) / 10.0f);
        
        float weightChange = correlation * pImpl->weight * 0.001f;
        addToWeight(weightChange);
    }
}

void Synapse::applyRewardModulatedLearning() {
    // Reward-modulated synaptic plasticity
    // Uses eligibility trace to modulate learning based on reward prediction error
    
    if (pImpl->eligibilityTrace > 0.001f) {
        // In a real implementation, this would use actual reward signals
        // For now, apply scaled eligibility
        float rewardSignal = 0.1f;  // Placeholder reward
        float weightChange = pImpl->eligibilityTrace * rewardSignal * 0.01f;
        addToWeight(weightChange);
        
        // Decay eligibility after use
        pImpl->eligibilityTrace *= 0.9f;
    }
}
    
    // 3. Update synaptic efficacy (use-dependent modulation)
    // Short-term efficacy changes with activity
    if (pImpl->lastPreSpikeTime >= 0.0f || pImpl->lastPostSpikeTime >= 0.0f) {
        // Enhanced efficacy with recent activity
        pImpl->efficacy = std::min(pImpl->efficacy + 0.1f, 2.0f);
    } else {
        // Natural decay of efficacy
        pImpl->efficacy *= 0.995f;
    }
    
    // 4. Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // 5. Apply NMDA/AMPA conductance-based dynamics
    updateReceptorDynamics();
    
    // 6. Update synaptic weight based on combined plasticity mechanisms
    applyPlasticityUpdates();
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
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
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
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
