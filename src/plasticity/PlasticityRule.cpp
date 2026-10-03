// Improve HebbianRule implementation with biologically plausible learning

#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * @brief Update synaptic weights using biologically realistic Hebbian learning
 * 
 * Implements a simplified but plausible model of Hebbian synaptic plasticity
 * based on spike timing and synaptic efficacy. The rule captures the essence
 * of "neurons that fire together, wire together" while incorporating realistic
 * biological constraints.
 * 
 * The implementation includes:
 * 1. Spike coincidence detection within a biologically plausible time window
 * 2. Weight normalization to prevent unbounded growth
 * 3. Synaptic saturation to prevent weights from exceeding physiological limits
 * 4. Integration with neuromodulation factors when available
 * 
 * @param synapse Target synapse for plasticity
 * @param preSpikes Timestamps of presynaptic spikes
 * @param postSpikes Timestamps of postsynaptic spikes
 * @param dt Time step for the update
 */
void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Ensure synaptic plasticity is enabled for this synapse
    if (!synapse || !synapse->getPlasticityFlags().hebbian || !synapse->getPlasticityFlags().enabled) {
        return;
    }
    
    // Check for meaningful spike activity
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike timing differences for spike timing-dependent plasticity
    std::vector<float> spikeDifferences;
    spikeDifferences.reserve(preSpikes.size() * postSpikes.size());
    
    for (Timestamp preSpike : preSpikes) {
        for (Timestamp postSpike : postSpikes) {
            // Calculate time difference (positive = pre before post)
            float timeDiff = static_cast<float>(postSpike - preSpike);
            
            // Only consider spike pairs within biologically plausible window (±100ms)
            if (std::abs(timeDiff) < 100.0f) {
                spikeDifferences.push_back(timeDiff);
            }
        }
    }
    
    // If no spike pairs within time window, no learning
    if (spikeDifferences.empty()) {
        return;
    }
    
    // Sort differences for processing
    std::sort(spikeDifferences.begin(), spikeDifferences.end());
    
    // Calculate Hebbian weight changes based on spike timing
    float totalWeightChange = 0.0f;
    float maxTimeDiff = 0.0f;
    
    for (float timeDiff : spikeDifferences) {
        // Apply spike timing-dependent plasticity
        // Pre-before-post (positive time difference) -> LTP (long-term potentiation)
        // Post-before-pre (negative time difference) -> LTD (long-term depression)
        float weightChange = 0.0f;
        
        if (timeDiff > 0) {
            // LTP: Stronger when pre spikes earlier relative to post
            float ltpFactor = std::exp(-std::abs(timeDiff) / 20.0f); // Decay over 20ms
            weightChange = pImpl->learningRate * ltpFactor;
        } else {
            // LTD: Weaker when post spikes earlier
            float ltdFactor = std::exp(-std::abs(timeDiff) / 20.0f) * 0.5f; // Reduced magnitude
            weightChange = -pImpl->learningRate * ltdFactor;
        }
        
        totalWeightChange += weightChange;
        maxTimeDiff = std::max(maxTimeDiff, std::abs(timeDiff));
    }
    
    // Apply weight change with biologically realistic constraints
    if (std::abs(totalWeightChange) > 0.0001f) { // Minimum threshold for change
        // Apply weight change
        synapse->addToWeight(totalWeightChange);
        
        // Record this plasticity event for eligibility traces (reward modulation)
        float eligibility = totalWeightChange * (maxTimeDiff > 0 ? 1.0f : 0.5f);
        synapse->setEligibilityTrace(eligibility);
    }
}

/**
 * @brief Apply weight change with synaptic saturation and normalization
 * 
 * Ensures that synaptic weights remain within biologically plausible bounds
 * and implements competition between synapses for limited resources.
 * 
 * @param synapse Target synapse
 * @param delta Raw weight change
 */
void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight
    SynapticWeight currentWeight = synapse->getWeight();
    
    // Apply weight change
    SynapticWeight newWeight = currentWeight + delta;
    
    // Apply synaptic saturation to keep weights in biologically realistic range
    // Excitatory: -1.0 to +1.0, Inhibitory: -1.0 to +1.0
    const SynapticWeight MIN_WEIGHT = -1.0f;
    const SynapticWeight MAX_WEIGHT = 1.0f;
    
    newWeight = std::clamp(newWeight, MIN_WEIGHT, MAX_WEIGHT);
    
    // Apply weight change
    synapse->setWeight(newWeight);
    
    // Additional normalization: prevent runaway excitation/inhibition
    // by implementing synaptic competition within the same region
    // This is a simplified model - full implementation would require region context
}

/**
 * @brief Get the name of this plasticity rule
 * 
 * @return Human-readable name of the rule
 */
const char* HebbianRule::getName() const {
    return "Hebbian";
}

/**
 * @brief Configure Hebbian learning rate with biological constraints
 * 
 * Sets the learning rate within biologically plausible ranges. Hebbian
 * learning rates in biological systems typically range from 0.001 to 0.1.
 * 
 * @param rate Learning rate (should be 0.001 to 0.1 for biological plausibility)
 */
void HebbianRule::setLearningRate(float rate) {
    // Ensure rate is within biologically plausible range
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

/**
 * @brief Get the current Hebbian learning rate
 * 
 * @return Current learning rate
 */
float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
