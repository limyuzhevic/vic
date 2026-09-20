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
    
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

// Update synaptic dynamics including Tsodyks-Markram short-term plasticity model
void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // 2. Decay short-term depression and recover toward baseline
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0 (full recovery)
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        // Cap depression at max utilization
        pImpl->shortTermDepression = std::min(pImpl->shortTermDepression, Impl::STP_U_MAX);
    }
    
    // 3. Apply synaptic depression with usage history (use-dependent modulation)
    if (!pImpl->preSpikeHistory.empty()) {
        Timestamp lastPreSpike = pImpl->preSpikeHistory.back();
        float timeSinceLastSpike = static_cast<float>(currentTime - lastPreSpike);
        
        if (timeSinceLastSpike < 10.0f) {  // Active within 10ms
            // Activity-dependent depression
            float usageFactor = std::exp(-timeSinceLastSpike / 5.0f);  // 5ms time constant
            pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - usageFactor * 0.1f);
        }
    }
    
    // 4. Implement synaptic efficacy normalization and bounds checking
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, Impl::STP_U_MAX);
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    
    // 5. Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay (1% per step)
    
    // 6. Clamp weight bounds with normalization toward baseline
    float currentWeight = pImpl->weight;
    if (std::abs(currentWeight) > Impl::MAX_WEIGHT) {
        // Normalize weight toward baseline (0.2f magnitude)
        pImpl->weight *= 0.8f;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
    
    // 7. Update synaptic transmission delay counter
    if (pImpl->lastTransmissionTime > 0) {
        pImpl->lastTransmissionTime = currentTime;
    }
}

// Apply synaptic transmission to post-synaptic neuron with delay
void Synapse::applySynapticTransmission(Neuron* postNeuron, Timestamp currentTime) {
    if (!postNeuron) return;
    
    // Implement synaptic delay mechanism
    Timestamp transmissionDelay = static_cast<Timestamp>(pImpl->delay * 0.001);  // Convert delay to time
    
    if (pImpl->lastTransmissionTime <= 0 || 
        (currentTime - pImpl->lastTransmissionTime) >= transmissionDelay) {
        
        // Synaptic transmission based on type
        if (pImpl->type == SynapseType::Excitatory) {
            // Excitatory transmission: calculate synaptic current
            float synapticCurrent = 0.0f;
            calculateSynapticCurrent(synapticCurrent, currentTime);
            
            if (synapticCurrent > 0.0f) {
                postNeuron->receiveExcitatoryInput(synapticCurrent * pImpl->efficacy);
            }
        } else if (pImpl->type == SynapseType::Inhibitory) {
            // Inhibitory transmission: apply inhibitory conductance
            float inhibitoryCurrent = 0.0f;
            calculateSynapticCurrent(inhibitoryCurrent, currentTime);
            
            if (inhibitoryCurrent > 0.0f) {
                postNeuron->receiveInhibitoryInput(inhibitoryCurrent * pImpl->efficacy);
            }
        } else if (pImpl->type == SynapseType::Modulatory) {
            // Modulatory transmission: affects plasticity
            float modulatoryLevel = calculateModulatoryLevel(currentTime);
            postNeuron->receiveModulatoryInput(modulatoryLevel * pImpl->efficacy);
        }
        
        // Update transmission time
        pImpl->lastTransmissionTime = currentTime;
    }
}

// Calculate modulatory level based on synaptic activity
float Synapse::calculateModulatoryLevel(Timestamp currentTime) {
    float level = 0.0f;
    
    // Combine pre and post spike history for modulatory effect
    if (!pImpl->preSpikeHistory.empty()) {
        float recentPreActivity = static_cast<float>(pImpl->preSpikeHistory.size()) / Impl::MAX_SPIKE_HISTORY;
        level += recentPreActivity * 0.5f;
    }
    
    if (!pImpl->postSpikeHistory.empty()) {
        float recentPostActivity = static_cast<float>(pImpl->postSpikeHistory.size()) / Impl::MAX_SPIKE_HISTORY;
        level += recentPostActivity * 0.5f;
    }
    
    // Factor in eligibility trace
    level += std::abs(pImpl->eligibilityTrace) * 0.3f;
    
    // Factor in current efficacy
    level += (pImpl->efficacy - 1.0f) * 0.2f;
    
    return std::clamp(level, 0.0f, 1.0f);
}

// Apply Hebbian learning to synaptic weights based on correlated activity
void Synapse::applyHebbianLearning(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   float learningRate) {
    if (!pImpl->plasticityFlags.hebbian) return;
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    // Count correlated spike pairs (within 10ms window)
    size_t correlations = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(static_cast<float>(pre - post)) < 10.0f) {
                correlations++;
            }
        }
    }
    
    // Apply Hebbian weight change
    if (correlations > 0) {
        float delta = learningRate * static_cast<float>(correlations);
        addToWeight(delta);
        
        // Update eligibility trace
        setEligibilityTrace(getEligibilityTrace() + delta * 0.5f);
        
        // Update efficacy
        setEfficacy(getEfficacy() + delta * 0.1f);
    }
}

// Apply STDP (Spike-Timing Dependent Plasticity) to synaptic weights
void Synapse::updateSTDP(const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        float neuromodulationFactor,
                        Timestamp currentTime) {
    if (!pImpl->plasticityFlags.stdp) return;
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    // Implement standard STDP rule based on spike timing differences
    float totalDelta = 0.0f;
    float timeConstant = 20.0f;  // STDP time constant (20ms)
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: Long-term potentiation (LTP)
                float ltp = 0.01f * std::exp(-dt / timeConstant);
                totalDelta += ltp * (1.0f + neuromodulationFactor);
            } else if (dt < 0) {
                // Post before pre: Long-term depression (LTD)
                float ltd = 0.012f * std::exp(dt / timeConstant);
                totalDelta -= ltd * (1.0f + neuromodulationFactor);
            }
        }
    }
    
    // Apply weight change with bounds and efficacy scaling
    if (std::abs(totalDelta) > 1e-6f) {
        totalDelta *= pImpl->efficacy;
        addToWeight(totalDelta);
        
        // Update eligibility trace
        setEligibilityTrace(getEligibilityTrace() + totalDelta * 0.5f);
    }
}

// Apply neuromodulation effects to synaptic dynamics
void Synapse::applyNeuromodulation(float dopamine, float acetylcholine, float serotonin) {
    // Dopamine: modulates STDP and eligibility traces
    if (dopamine > 0.0f) {
        // Enhance reward-modulated learning
        pImpl->eligibilityTrace *= (1.0f + dopamine * 0.5f);
        
        // Enhance efficacy for positive prediction errors
        if (dopamine > 0.5f) {
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + dopamine * 0.05f);
        }
    } else if (dopamine < 0.0f) {
        // Suppress learning for negative prediction errors
        pImpl->eligibilityTrace *= std::max(0.0f, 1.0f + dopamine * 0.5f);
        pImpl->efficacy = std::max(0.0f, pImpl->efficacy + dopamine * 0.02f);
    }
    
    // Acetylcholine: modulates synaptic tagging and capture
    if (acetylcholine > 0.0f) {
        // Enhance synaptic tagging
        pImpl->eligibilityTrace += acetylcholine * 0.3f;
        
        // Increase efficacy
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy + acetylcholine * 0.03f);
    }
    
    // Serotonin: modulates global plasticity state
    if (serotonin > 0.0f) {
        // Enhance structural plasticity
        pImpl->plasticityFlags.structural = true;
        
        // Increase excitability
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy + serotonin * 0.02f);
    }
    
    // Apply bounds to neuromodulation effects
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

// Apply synaptic current to post-synaptic neuron
void Synapse::applyConductionBasedCurrent(Neuron& postNeuron,
                                        float synapticConductance,
                                        Timestamp currentTime) {
    if (pImpl->type == SynapseType::Excitatory) {
        float current = synapticConductance * pImpl->efficacy;
        postNeuron.receiveExcitatoryInput(current);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        float current = synapticConductance * pImpl->efficacy;
        postNeuron.receiveInhibitoryInput(current);
    }
}

// Apply synaptic tagging and capture for reward-modulated learning
void Synapse::applySynapticTagging(float neuromodulatorLevel, Timestamp currentTime) {
    if (neuromodulatorLevel > 0.1f) {
        // Tag the synapse for later plasticity events
        pImpl->eligibilityTrace += neuromodulatorLevel * 0.5f;
        
        // Also update efficacy based on neuromodulator
        if (neuromodulatorLevel > 0.5f) {
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + neuromodulatorLevel * 0.05f);
        }
    }
}

// Apply synaptic capture with reward prediction error
void Synapse::applySynapticCapture(float rewardPredictionError, float neuromodulatorLevel) {
    if (pImpl->plasticityFlags.reward_modulated) {
        // Calculate weight change based on reward prediction error
        float rewardInfluence = std::min(1.0f, std::abs(rewardPredictionError) * 2.0f);
        
        if (rewardPredictionError > 0) {
            // Positive RPE: strengthen synapse
            pImpl->eligibilityTrace += rewardInfluence * 0.5f;
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + rewardInfluence * 0.02f);
        } else {
            // Negative RPE: weaken synapse
            pImpl->eligibilityTrace -= rewardInfluence * 0.5f;
            pImpl->efficacy = std::max(0.0f, pImpl->efficacy - rewardInfluence * 0.02f);
        }
        
        // Cap eligibility trace
        if (std::abs(pImpl->eligibilityTrace) > 1.0f) {
            pImpl->eligibilityTrace = std::copysign(1.0f, pImpl->eligibilityTrace);
        }
    }
}

// Reset last spike times for synaptic dynamics
void Synapse::resetLastSpikeTimes() {
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Add synaptic depression with usage history
void Synapse::addSynapticDepression(float usageFactor) {
    // Implement activity-dependent synaptic depression
    pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - usageFactor * 0.2f);
}

// Implement activity-dependent efficacy changes
void Synapse::updateEfficacyWithActivity(Timestamp currentTime) {
    // Implement activity-dependent efficacy changes
    if (!pImpl->preSpikeHistory.empty()) {
        Timestamp lastPreSpike = pImpl->preSpikeHistory.back();
        float timeSinceSpike = static_cast<float>(currentTime - lastPreSpike);
        
        if (timeSinceSpike < 50.0f) {  // Within 50ms
            float activityFactor = std::exp(-timeSinceSpike / 20.0f);
            pImpl->efficacy += activityFactor * 0.02f;
        }
    }
    
    // Decay efficacy over time
    pImpl->efficacy *= 0.99f;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

// Tsodyks-Markram model for synaptic transmission
void Synapse::calculateSynapticCurrent(float& totalCurrent, Timestamp currentTime) {
    // Tsodyks-Markram model for synaptic transmission
    // Calculates the synaptic current based on utilization, depression, and facilitation
    
    // Reset current
    totalCurrent = 0.0f;
    
    // Check if pre-synaptic neuron has spiked within the last step
    if (!pImpl->preSpikeHistory.empty()) {
        Timestamp lastPreSpike = pImpl->preSpikeHistory.back();
        float timeSinceSpike = static_cast<float>(currentTime - lastPreSpike);
        
        if (timeSinceSpike < Impl::STP_DEPRESSION_TAU) {
            // Calculate utilization U (between 0 and 1)
            float utilization = std::min(Impl::STP_U_MAX, pImpl->shortTermDepression);
            
            // Update depression based on usage (Tsodyks-Markram model)
            pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - 
                                              utilization * std::exp(-timeSinceSpike / Impl::STP_DEPRESSION_TAU));
            
            // Calculate synaptic efficacy with facilitation
            float effectiveEfficacy = pImpl->efficacy * (1.0f + pImpl->shortTermFacilitation);
            
            // Calculate synaptic current
            totalCurrent = static_cast<float>(pImpl->weight) * 
                          utilization * effectiveEfficacy;
            
            // Apply synaptic response to post-synaptic neuron (placeholder for actual transmission)
            // In a real implementation, this would trigger post-synaptic conductances
            // For now, just set a flag that synapse has been active
            if (timeSinceSpike < 10.0f) {  // Active within 10ms
                pImpl->efficacy = std::min(2.0f, pImpl->efficacy * 1.1f);  // Facilitation
            }
        }
    }
}

// Update synaptic efficacy based on spike pairing (STDP-like)
void Synapse::updateEfficacyWithSTDP(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes) {
    // Update synaptic efficacy based on spike pairing (STDP-like)
    // This represents synaptic modification rules independent of eligibility traces
    
    if (preSpikes.empty() && postSpikes.empty()) return;
    
    float deltaEfficacy = 0.0f;
    
    // Analyze pre-post spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0 && dt < 50.0f) {
                // Pre before post: potentiation
                deltaEfficacy += 0.01f * std::exp(-dt / 20.0f);
            } else if (dt < 0 && std::abs(dt) < 50.0f) {
                // Post before pre: depression
                deltaEfficacy -= 0.012f * std::exp(dt / 20.0f);
            }
        }
    }
    
    // Update efficacy with bounds
    pImpl->efficacy += deltaEfficacy;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

// Apply synaptic tagging based on neuromodulator levels
void Synapse::applySynapticTagging(float neuromodulatorLevel, Timestamp currentTime) {
    // Apply synaptic tagging based on neuromodulator levels
    // This represents processes like acetylcholine that tag synapses for potential strengthening
    
    if (neuromodulatorLevel > 0.1f) {
        // Tag the synapse for later plasticity events
        pImpl->eligibilityTrace += neuromodulatorLevel * 0.5f;
        
        // Also update efficacy based on neuromodulator
        if (neuromodulatorLevel > 0.5f) {
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + neuromodulatorLevel * 0.05f);
        }
    }
}

// Apply synaptic capture with reward prediction error
void Synapse::applySynapticCapture(float rewardPredictionError, float neuromodulatorLevel) {
    // Apply synaptic capture with reward prediction error
    // This implements reward-modulated learning
    
    if (pImpl->plasticityFlags.reward_modulated) {
        // Calculate weight change based on reward prediction error
        float rewardInfluence = std::min(1.0f, std::abs(rewardPredictionError) * 2.0f);
        
        if (rewardPredictionError > 0) {
            // Positive RPE: strengthen synapse
            pImpl->eligibilityTrace += rewardInfluence * 0.5f;
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + rewardInfluence * 0.02f);
        } else {
            // Negative RPE: weaken synapse
            pImpl->eligibilityTrace -= rewardInfluence * 0.5f;
            pImpl->efficacy = std::max(0.0f, pImpl->efficacy - rewardInfluence * 0.02f);
        }
        
        // Cap eligibility trace
        if (std::abs(pImpl->eligibilityTrace) > 1.0f) {
            pImpl->eligibilityTrace = std::copysign(1.0f, pImpl->eligibilityTrace);
        }
    }
}

// Add activity-dependent efficacy changes
void Synapse::updateEfficacyWithActivity(Timestamp currentTime) {
    // Implement activity-dependent efficacy changes
    if (!pImpl->preSpikeHistory.empty()) {
        Timestamp lastPreSpike = pImpl->preSpikeHistory.back();
        float timeSinceSpike = static_cast<float>(currentTime - lastPreSpike);
        
        if (timeSinceSpike < 50.0f) {  // Within 50ms
            float activityFactor = std::exp(-timeSinceSpike / 20.0f);
            pImpl->efficacy += activityFactor * 0.02f;
        }
    }
    
    // Decay efficacy over time
    pImpl->efficacy *= 0.99f;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

// Add synaptic depression with usage history
void Synapse::addSynapticDepression(float usageFactor) {
    // Implement activity-dependent synaptic depression
    pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - usageFactor * 0.2f);
}

// Calculate modulatory level based on synaptic activity
float Synapse::calculateModulatoryLevel(Timestamp currentTime) {
    float level = 0.0f;
    
    // Combine pre and post spike history for modulatory effect
    if (!pImpl->preSpikeHistory.empty()) {
        float recentPreActivity = static_cast<float>(pImpl->preSpikeHistory.size()) / Impl::MAX_SPIKE_HISTORY;
        level += recentPreActivity * 0.5f;
    }
    
    if (!pImpl->postSpikeHistory.empty()) {
        float recentPostActivity = static_cast<float>(pImpl->postSpikeHistory.size()) / Impl::MAX_SPIKE_HISTORY;
        level += recentPostActivity * 0.5f;
    }
    
    // Factor in eligibility trace
    level += std::abs(pImpl->eligibilityTrace) * 0.3f;
    
    // Factor in current efficacy
    level += (pImpl->efficacy - 1.0f) * 0.2f;
    
    return std::clamp(level, 0.0f, 1.0f);
}

// Synaptic transmission and plasticity methods
    void calculateSynapticCurrent(float& totalCurrent, Timestamp currentTime);
    void updateEfficacyWithSTDP(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes);
    void applySynapticTagging(float neuromodulatorLevel, Timestamp currentTime);
    void applySynapticCapture(float rewardPredictionError, float neuromodulatorLevel);
    void resetLastSpikeTimes();
    void updateSTDP(const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   float neuromodulationFactor,
                   Timestamp currentTime);
    void applyHebbianLearning(const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             float learningRate);
    void applyNeuromodulation(float dopamine, float acetylcholine,
                             float serotonin);
    void applyConductionBasedCurrent(Neuron& postNeuron,
                                     float synapticConductance,
                                     Timestamp currentTime);
    void applySynapticTransmission(Neuron* postNeuron, Timestamp currentTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Update synaptic dynamics including Tsodyks-Markram short-term plasticity model
void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // 2. Decay short-term depression and recover toward baseline
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0 (full recovery)
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        // Cap depression at max utilization
        pImpl->shortTermDepression = std::min(pImpl->shortTermDepression, Impl::STP_U_MAX);
    }
    
    // 3. Apply synaptic depression with usage history (use-dependent modulation)
    if (!pImpl->preSpikeHistory.empty()) {
        Timestamp lastPreSpike = pImpl->preSpikeHistory.back();
        float timeSinceLastSpike = static_cast<float>(currentTime - lastPreSpike);
        
        if (timeSinceLastSpike < 10.0f) {  // Active within 10ms
            // Activity-dependent depression
            float usageFactor = std::exp(-timeSinceLastSpike / 5.0f);  // 5ms time constant
            pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - usageFactor * 0.1f);
        }
    }
    
    // 4. Implement synaptic efficacy normalization and bounds checking
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
    pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, Impl::STP_U_MAX);
    pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, 1.0f);
    
    // 5. Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay (1% per step)
    
    // 6. Clamp weight bounds with normalization toward baseline
    float currentWeight = pImpl->weight;
    if (std::abs(currentWeight) > Impl::MAX_WEIGHT) {
        // Normalize weight toward baseline (0.2f magnitude)
        pImpl->weight *= 0.8f;
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
    
    // 7. Update synaptic transmission delay counter
    if (pImpl->lastTransmissionTime > 0) {
        pImpl->lastTransmissionTime = currentTime;
    }
}

// Apply synaptic transmission to post-synaptic neuron with delay
void Synapse::applySynapticTransmission(Neuron* postNeuron, Timestamp currentTime) {
    if (!postNeuron) return;
    
    // Implement synaptic delay mechanism
    Timestamp transmissionDelay = static_cast<Timestamp>(pImpl->delay * 0.001);  // Convert delay to time
    
    if (pImpl->lastTransmissionTime <= 0 || 
        (currentTime - pImpl->lastTransmissionTime) >= transmissionDelay) {
        
        // Synaptic transmission based on type
        if (pImpl->type == SynapseType::Excitatory) {
            // Excitatory transmission: calculate synaptic current
            float synapticCurrent = 0.0f;
            calculateSynapticCurrent(synapticCurrent, currentTime);
            
            if (synapticCurrent > 0.0f) {
                postNeuron->receiveExcitatoryInput(synapticCurrent * pImpl->efficacy);
            }
        } else if (pImpl->type == SynapseType::Inhibitory) {
            // Inhibitory transmission: apply inhibitory conductance
            float inhibitoryCurrent = 0.0f;
            calculateSynapticCurrent(inhibitoryCurrent, currentTime);
            
            if (inhibitoryCurrent > 0.0f) {
                postNeuron->receiveInhibitoryInput(inhibitoryCurrent * pImpl->efficacy);
            }
        } else if (pImpl->type == SynapseType::Modulatory) {
            // Modulatory transmission: affects plasticity
            float modulatoryLevel = calculateModulatoryLevel(currentTime);
            postNeuron->receiveModulatoryInput(modulatoryLevel * pImpl->efficacy);
        }
        
        // Update transmission time
        pImpl->lastTransmissionTime = currentTime;
    }
}

// Calculate modulatory level based on synaptic activity
float Synapse::calculateModulatoryLevel(Timestamp currentTime) {
    float level = 0.0f;
    
    // Combine pre and post spike history for modulatory effect
    if (!pImpl->preSpikeHistory.empty()) {
        float recentPreActivity = static_cast<float>(pImpl->preSpikeHistory.size()) / Impl::MAX_SPIKE_HISTORY;
        level += recentPreActivity * 0.5f;
    }
    
    if (!pImpl->postSpikeHistory.empty()) {
        float recentPostActivity = static_cast<float>(pImpl->postSpikeHistory.size()) / Impl::MAX_SPIKE_HISTORY;
        level += recentPostActivity * 0.5f;
    }
    
    // Factor in eligibility trace
    level += std::abs(pImpl->eligibilityTrace) * 0.3f;
    
    // Factor in current efficacy
    level += (pImpl->efficacy - 1.0f) * 0.2f;
    
    return std::clamp(level, 0.0f, 1.0f);
}

// Apply Hebbian learning to synaptic weights based on correlated activity
void Synapse::applyHebbianLearning(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   float learningRate) {
    if (!pImpl->plasticityFlags.hebbian) return;
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    // Count correlated spike pairs (within 10ms window)
    size_t correlations = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(static_cast<float>(pre - post)) < 10.0f) {
                correlations++;
            }
        }
    }
    
    // Apply Hebbian weight change
    if (correlations > 0) {
        float delta = learningRate * static_cast<float>(correlations);
        addToWeight(delta);
        
        // Update eligibility trace
        setEligibilityTrace(getEligibilityTrace() + delta * 0.5f);
        
        // Update efficacy
        setEfficacy(getEfficacy() + delta * 0.1f);
    }
}

// Apply STDP (Spike-Timing Dependent Plasticity) to synaptic weights
void Synapse::updateSTDP(const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        float neuromodulationFactor,
                        Timestamp currentTime) {
    if (!pImpl->plasticityFlags.stdp) return;
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    // Implement standard STDP rule based on spike timing differences
    float totalDelta = 0.0f;
    float timeConstant = 20.0f;  // STDP time constant (20ms)
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: Long-term potentiation (LTP)
                float ltp = 0.01f * std::exp(-dt / timeConstant);
                totalDelta += ltp * (1.0f + neuromodulationFactor);
            } else if (dt < 0) {
                // Post before pre: Long-term depression (LTD)
                float ltd = 0.012f * std::exp(dt / timeConstant);
                totalDelta -= ltd * (1.0f + neuromodulationFactor);
            }
        }
    }
    
    // Apply weight change with bounds and efficacy scaling
    if (std::abs(totalDelta) > 1e-6f) {
        totalDelta *= pImpl->efficacy;
        addToWeight(totalDelta);
        
        // Update eligibility trace
        setEligibilityTrace(getEligibilityTrace() + totalDelta * 0.5f);
    }
}

// Apply neuromodulation effects to synaptic dynamics
void Synapse::applyNeuromodulation(float dopamine, float acetylcholine, float serotonin) {
    // Dopamine: modulates STDP and eligibility traces
    if (dopamine > 0.0f) {
        // Enhance reward-modulated learning
        pImpl->eligibilityTrace *= (1.0f + dopamine * 0.5f);
        
        // Enhance efficacy for positive prediction errors
        if (dopamine > 0.5f) {
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + dopamine * 0.05f);
        }
    } else if (dopamine < 0.0f) {
        // Suppress learning for negative prediction errors
        pImpl->eligibilityTrace *= std::max(0.0f, 1.0f + dopamine * 0.5f);
        pImpl->efficacy = std::max(0.0f, pImpl->efficacy + dopamine * 0.02f);
    }
    
    // Acetylcholine: modulates synaptic tagging and capture
    if (acetylcholine > 0.0f) {
        // Enhance synaptic tagging
        pImpl->eligibilityTrace += acetylcholine * 0.3f;
        
        // Increase efficacy
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy + acetylcholine * 0.03f);
    }
    
    // Serotonin: modulates global plasticity state
    if (serotonin > 0.0f) {
        // Enhance structural plasticity
        pImpl->plasticityFlags.structural = true;
        
        // Increase excitability
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy + serotonin * 0.02f);
    }
    
    // Apply bounds to neuromodulation effects
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

// Apply synaptic current to post-synaptic neuron
void Synapse::applyConductionBasedCurrent(Neuron& postNeuron,
                                        float synapticConductance,
                                        Timestamp currentTime) {
    if (pImpl->type == SynapseType::Excitatory) {
        float current = synapticConductance * pImpl->efficacy;
        postNeuron.receiveExcitatoryInput(current);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        float current = synapticConductance * pImpl->efficacy;
        postNeuron.receiveInhibitoryInput(current);
    }
}

// Apply synaptic tagging and capture for reward-modulated learning
void Synapse::applySynapticTagging(float neuromodulatorLevel, Timestamp currentTime) {
    if (neuromodulatorLevel > 0.1f) {
        // Tag the synapse for later plasticity events
        pImpl->eligibilityTrace += neuromodulatorLevel * 0.5f;
        
        // Also update efficacy based on neuromodulator
        if (neuromodulatorLevel > 0.5f) {
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + neuromodulatorLevel * 0.05f);
        }
    }
}

// Apply synaptic capture with reward prediction error
void Synapse::applySynapticCapture(float rewardPredictionError, float neuromodulatorLevel) {
    if (pImpl->plasticityFlags.reward_modulated) {
        // Calculate weight change based on reward prediction error
        float rewardInfluence = std::min(1.0f, std::abs(rewardPredictionError) * 2.0f);
        
        if (rewardPredictionError > 0) {
            // Positive RPE: strengthen synapse
            pImpl->eligibilityTrace += rewardInfluence * 0.5f;
            pImpl->efficacy = std::min(2.0f, pImpl->efficacy + rewardInfluence * 0.02f);
        } else {
            // Negative RPE: weaken synapse
            pImpl->eligibilityTrace -= rewardInfluence * 0.5f;
            pImpl->efficacy = std::max(0.0f, pImpl->efficacy - rewardInfluence * 0.02f);
        }
        
        // Cap eligibility trace
        if (std::abs(pImpl->eligibilityTrace) > 1.0f) {
            pImpl->eligibilityTrace = std::copysign(1.0f, pImpl->eligibilityTrace);
        }
    }
}

// Reset last spike times for synaptic dynamics
void Synapse::resetLastSpikeTimes() {
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// Add synaptic depression with usage history
void Synapse::addSynapticDepression(float usageFactor) {
    // Implement activity-dependent synaptic depression
    pImpl->shortTermDepression = std::max(0.0f, pImpl->shortTermDepression - usageFactor * 0.2f);
}

// Implement activity-dependent efficacy changes
void Synapse::updateEfficacyWithActivity(Timestamp currentTime) {
    // Implement activity-dependent efficacy changes
    if (!pImpl->preSpikeHistory.empty()) {
        Timestamp lastPreSpike = pImpl->preSpikeHistory.back();
        float timeSinceSpike = static_cast<float>(currentTime - lastPreSpike);
        
        if (timeSinceSpike < 50.0f) {  // Within 50ms
            float activityFactor = std::exp(-timeSinceSpike / 20.0f);
            pImpl->efficacy += activityFactor * 0.02f;
        }
    }
    
    // Decay efficacy over time
    pImpl->efficacy *= 0.99f;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

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
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
    
    // STDP update with pre-synaptic spike
    if (pImpl->plasticityFlags.stdp) {
        updateSTDP(pImpl->preSpikeHistory, pImpl->postSpikeHistory, 
                   pImpl->plasticityFlags.reward_modulated ? 1.0f : 0.0f, 
                   timestamp);
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
    
    // STDP update with post-synaptic spike
    if (pImpl->plasticityFlags.stdp) {
        updateSTDP(pImpl->preSpikeHistory, pImpl->postSpikeHistory, 
                   pImpl->plasticityFlags.reward_modulated ? 1.0f : 0.0f, 
                   timestamp);
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
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
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
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
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
    pImpl->delay = static_cast<Delay>(rng.template uniformInt<uint32_t>(1, 5));
    
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
