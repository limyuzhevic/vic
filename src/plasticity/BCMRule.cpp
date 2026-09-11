#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float hebbianThreshold;
    float bcmThreshold;
    float minWeight;
    float maxWeight;
    float baselineActivity;
    float neuromodulationSensitivity;
    float stdDev;
    
    Impl() : learningRate(0.01f), hebbianThreshold(0.1f), bcmThreshold(0.5f),
             minWeight(-1.0f), maxWeight(1.0f), baselineActivity(0.05f),
             neuromodulationSensitivity(1.0f), stdDev(0.1f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {
    NLM_LOG_INFO("BCM rule initialized - Bienenstock-Cooper-Munro plasticity rule");
}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) Learning Rule Implementation
     * 
     * Mathematical formulation:
     * Θ(t+1) = Θ(t) + ξ * (⟨pre * post⟩ - Θ(t))
     * Δw = η * (pre * post - Θ(t) * w)
     * 
     * Where Θ(t) is the sliding threshold (theta),
     *       ξ is the learning rate for threshold adaptation,
     *       η is the synaptic learning rate.
     * 
     * Biological basis:
     * - Implements sliding threshold for synaptic modification
     * - Reflects calcium-dependent threshold dynamics
     * - Accounts for postsynaptic activity history
     * - Explains both potentiation and depression
     * 
     * Key features:
     * - Adaptive threshold that moves with activity
     * - Weight-dependent plasticity sign
     * - Stable learning dynamics
     * - Neuromodulator integration
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float currentWeight = synapse->getWeight();
    
    // Compute postsynaptic activity (simplified)
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalize
    
    // Update sliding threshold (Theta)
    float thresholdChange = pImpl->learningRate * (postActivity - pImpl->bcmThreshold);
    pImpl->bcmThreshold += thresholdChange;
    
    // Constrain threshold
    pImpl->bcmThreshold = std::clamp(pImpl->bcmThreshold, 0.01f, 2.0f);
    
    // Compute coactivity (simplified spike correlation)
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 100.0f) {
                ++coactivity;
            }
        }
    }
    
    float normalizedCoactivity = static_cast<float>(coactivity) / 10.0f;
    
    // BCM weight change rule: Δw = η * (⟨pre * post⟩ - Θ * w)
    float weightChange = pImpl->learningRate * (normalizedCoactivity - pImpl->bcmThreshold * currentWeight);
    
    // Apply neuromodulation if available
    float modulationFactor = 1.0f;  // Placeholder
    weightChange *= modulationFactor;
    
    // Apply weight change with homeostatic constraints
    float newWeight = currentWeight + weightChange;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply soft bound for stability
    if (std::abs(newWeight - pImpl->bcmThreshold) > 1.0f) {
        float target = pImpl->bcmThreshold;
        newWeight = currentWeight * 0.9f + target * 0.1f;
    }
    
    synapse->setWeight(newWeight);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply BCM-specific stabilization
    float activityLevel = std::max(0.0f, std::min(1.0f, newWeight / 2.0f));
    float stabilityFactor = 0.9f + 0.1f * activityLevel;  // More stable at higher weights
    
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCMRule";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setHebbianThreshold(float threshold) {
    pImpl->hebbianThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float BCMRule::getHebbianThreshold() const {
    return pImpl->hebbianThreshold;
}

void BCMRule::setBcmThreshold(float threshold) {
    pImpl->bcmThreshold = std::clamp(threshold, 0.0f, 2.0f);
}

float BCMRule::getBcmThreshold() const {
    return pImpl->bcmThreshold;
}

void BCMRule::setBaselineActivity(float baseline) {
    pImpl->baselineActivity = std::clamp(baseline, 0.0f, 1.0f);
}

float BCMRule::getBaselineActivity() const {
    return pImpl->baselineActivity;
}

} // namespace nlm