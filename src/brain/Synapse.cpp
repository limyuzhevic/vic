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
    static constexpr float STP_U = 0.5f;  // Utilization rate
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

void Synapse::setShortTermDeprression(float depression) {
    pImpl->shortTermDepression = std::clamp(depression, 0.0f, 1.0f);
}

void Synapse::setShortTermFacilitation(float facilitation) {
    pImpl->shortTermFacilitation = std::clamp(facilitation, 0.0f, 2.0f);
}

float Synapse::getShortTermDeprression() const {
    return pImpl->shortTermDepression;
}

float Synapse::getShortTermFacilitation() const {
    return pImpl->shortTermFacilitation;
}

float Synapse::getLastPreSpikeTime() const {
    return pImpl->lastPreSpikeTime;
}

float Synapse::getLastPostSpikeTime() const {
    return pImpl->lastPostSpikeTime;
}

float Synapse::getFacilitationTimeConstant() const {
    return Impl::STP_FACILITATION_TAU;
}

float Synapse::getDepressionTimeConstant() const {
    return Impl::STP_DEPRESSION_TAU;
}

void Synapse::setFacilitationTimeConstant(float tau) {
    // Note: In real implementation, this would update the parameter
    // For now, we keep the constant values
}

void Synapse::setDepressionTimeConstant(float tau) {
    // Note: In real implementation, this would update the parameter
    // For now, we keep the constant values
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    // Update last pre spike time
    pImpl->lastPreSpikeTime = timestamp;
    
    // Update utilization based on Tsodyks-Markram model
    if (pImpl->shortTermDepression > 0.0f) {
        // Depression decreases with activity
        float depressionChange = -pImpl->shortTermDepression * Impl::STP_U;
        pImpl->shortTermDepression += depressionChange;
        pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression);
    }
    
    // Increase facilitation
    if (pImpl->shortTermFacilitation < 1.0f) {
        float facilitationChange = Impl::STP_U * (1.0f - pImpl->shortTermFacilitation);
        pImpl->shortTermFacilitation += facilitationChange;
        pImpl->shortTermFacilitation = std::min(1.0f, pImpl->shortTermFacilitation);
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    // Update last post spike time
    pImpl->lastPostSpikeTime = timestamp;
    
    // For inhibitory synapses, depression may be more effective
    if (!isExcitatory()) {
        if (pImpl->shortTermDepression > 0.0f) {
            float depressionChange = -pImpl->shortTermDepression * (Impl::STP_U * 0.8f);
            pImpl->shortTermDepression += depressionChange;
            pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression);
        }
    }
}

void Synapse::updatePlasticity(float deltaW, float preSpikeTime, float postSpikeTime) {
    // STDP implementation: spike-timing-dependent plasticity
    float dt = postSpikeTime - preSpikeTime;
    
    if (dt > 0.0f) {
        // Pre before post: Long-term potentiation (LTP)
        float weightChange = 0.01f * std::exp(-dt / 20.0f);  // Typical STDP parameters
        addToWeight(weightChange);
    } else if (dt < 0.0f) {
        // Post before pre: Long-term depression (LTD)
        float weightChange = -0.012f * std::exp(dt / 20.0f);  // Typical STDP parameters
        addToWeight(weightChange);
    }
    
    // Update eligibility trace for reward-modulated plasticity
    if (pImpl->plasticityFlags.reward_modulated) {
        float eligibility = std::max(std::abs(dt), 1.0f) * 0.01f;
        pImpl->eligibilityTrace += eligibility;
    }
}

void Synapse::applyRewardModulation(float reward, float predictionError) {
    if (!pImpl->plasticityFlags.reward_modulated) {
        return;
    }
    
    // Reward-modulated plasticity (dopamine-based)
    float learningRate = 0.001f;
    float modulatedLearning = learningRate * (1.0f + 2.0f * predictionError);  // Scale with prediction error
    
    // Apply weight changes based on eligibility trace
    if (pImpl->eligibilityTrace > 0.001f) {
        float deltaW = modulatedLearning * pImpl->eligibilityTrace;
        addToWeight(deltaW);
        
        // Decay eligibility trace
        decayEligibilityTrace(0.1f);
    }
}

void Synapse::step(Timestamp currentTime) {
    // Comprehensive synaptic dynamics implementation
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Update short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = currentTime - pImpl->lastPreSpikeTime;
        float decayFactor = std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        pImpl->shortTermFacilitation = pImpl->shortTermFacilitation * decayFactor;
        
        // Facilitation effect on synaptic efficacy
        if (pImpl->shortTermFacilitation > 0.01f) {
            float efficacyBoost = (pImpl->shortTermFacilitation - 0.01f) * 0.5f;
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + efficacyBoost * 0.1f);
        }
    }
    
    // 2. Update short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePost = currentTime - pImpl->lastPostSpikeTime;
        float recoveryFactor = 1.0f - std::exp(-timeSincePost / Impl::STP_DEPRESSION_TAU);
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * recoveryFactor;
    }
    
    // 3. Apply activity-dependent depression for recurrent activity
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->lastPostSpikeTime >= 0.0f) {
        float activityLevel = std::min(1.0f, (currentTime - pImpl->lastPostSpikeTime) / 10.0f);
        float activityDepression = activityLevel * Impl::STP_U * 0.01f;
        pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - activityDepression);
    }
    
    // 4. Decay eligibility trace for reward-modulated learning
    if (pImpl->eligibilityTrace > 0.001f) {
        float decayRate = 0.05f;  // 5% decay per timestep
        pImpl->eligibilityTrace *= (1.0f - decayRate);
    }
    
    // 5. Apply homeostatic plasticity to prevent runaway excitation/inhibition
    float targetWeight = isExcitatory() ? 0.2f : -0.2f;
    float weightDeviation = std::abs(pImpl->weight - targetWeight);
    
    if (weightDeviation > 0.3f) {
        // Apply homeostatic weight change
        float homeostaticRate = 0.001f;
        float homeostaticDelta = homeostaticRate * (targetWeight - pImpl->weight);
        addToWeight(homeostaticDelta);
    }
    
    // 6. Apply synaptic scaling based on overall network activity
    // Simplified version: scale efficacy based on short-term state
    if (pImpl->shortTermDepression < 0.5f && pImpl->shortTermFacilitation > 0.5f) {
        // High activity state: increase efficacy
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy * 1.02f);
    } else {
        // Low activity state: decrease efficacy
        pImpl->efficacy = std::max(0.5f, pImpl->efficacy * 0.98f);
    }
    
    // 7. Clamp weight bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 2.0f);
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
    // Proper random initialization based on synapse type with biological constraints
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: typical range 0.1-0.8, with higher probability for moderate values
        pImpl->weight = rng.uniformReal(0.1f, 0.6f);
        // Apply Hebbian-like initialization bias
        if (rng.uniformReal(0.0f, 1.0f) < 0.3f) {
            pImpl->weight = std::min(pImpl->weight + 0.1f, Impl::MAX_WEIGHT);
        }
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.7f, 1.2f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights, typical range -0.8 to -0.1
        pImpl->weight = -rng.uniformReal(0.1f, 0.6f);
        pImpl->efficacy = rng.uniformReal(0.6f, 1.1f);
    } else {
        // Other types (modulatory, electrical): small random weights
        pImpl->weight = rng.uniformReal(-0.2f, 0.2f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.2f);
    }
    
    // Random delay: biologically plausible range 1-10 steps (1-10ms)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 10));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f - rng.uniformReal(0.0f, 0.2f);  // Slight depression bias
    pImpl->shortTermFacilitation = rng.uniformReal(0.0f, 0.5f);  // Random facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm