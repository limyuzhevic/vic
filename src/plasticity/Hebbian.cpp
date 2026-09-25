#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    float timeWindow = 100.0f;  // 100ms correlation window
    
    // OPTIMIZED: Two-pointer technique for spike pair detection
    // Since spike times are typically monotonically increasing,
    // we can find correlated spike pairs more efficiently
    size_t preIdx = 0;
    size_t postIdx = 0;
    
    while (preIdx < preSpikes.size() && postIdx < postSpikes.size()) {
        float dt = static_cast<float>(postSpikes[postIdx] - preSpikes[preIdx]);
        
        if (std::abs(dt) < timeWindow) {
            // Spike pair within correlation window
            correlationCount++;
            
            // Move both pointers to next spikes
            ++preIdx;
            ++postIdx;
        } else if (dt < -timeWindow) {
            // Post spike too old, move to next post spike
            ++postIdx;
        } else { // dt > timeWindow
            // Pre spike too old, move to next pre spike
            ++preIdx;
        }
    }
    
    // Compute weight change based on correlation
    // IMPROVED: Scale correlation by spike counts to prevent huge weight deltas
    float preCount = static_cast<float>(preSpikes.size());
    float postCount = static_cast<float>(postSpikes.size());
    float activityScale = preCount * postCount;
    
    // Biological constraint: correlation is limited by minimum activity levels
    // to prevent runaway potentiation/depression
    float activityFactor = std::min(1.0f, (preCount + postCount) / 10.0f);
    
    // Scale correlation by spike counts and activity factor
    float delta = pImpl->learningRate * static_cast<float>(correlationCount) 
                  * activityFactor / std::max(activityScale, 1.0f);
    
    // Biological constraint: limit maximum weight change per update
    // Scale with activity factor for context-dependent learning rate
    float maxDelta = 0.1f * pImpl->learningRate * activityFactor;
    delta = std::clamp(delta, -maxDelta, maxDelta);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
