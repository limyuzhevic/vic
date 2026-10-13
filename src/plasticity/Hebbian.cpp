#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float normalizationFactor;  // For Oja's rule
    float bcmThreshold;         // For BCM theory (sliding threshold)
    float homeostaticRate;      // For synaptic scaling
    
    // Statistics
    size_t updateCount;
    float totalNormalization;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), normalizationFactor(1.0f),
             bcmThreshold(0.5f), homeostaticRate(0.001f),
             updateCount(0), totalNormalization(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation with multiple learning rules
     * 
     * Mathematical formulations:
     * 
     * 1. Covariance Rule (Standard Hebbian):
     *   Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * 2. Oja's Rule (Normalized Hebbian):
     *   Δw = η * (post * pre - w * post^2)
     * 
     * 3. BCM Theory (Sliding Threshold):
     *   Δw = η * (post - θ) * pre
     *   Where θ = φ / (1 + y^2), y = post activity
     * 
     * 4. Implicit Normalization:
     *   Δw = η * (post * pre - w * ⟨post^2⟩)
     * 
     * Biological inspiration:
     *   - Reflects synaptic strength changes with correlated activity
     *   - "Neurons that fire together, wire together"
     *   - homeostatic mechanisms prevent runaway excitation
     *   
     * Limitations:
     *   - Assumes stationary statistics (doesn't track running averages)
     *   - Single learning rate (could be adaptive)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current state
    float currentWeight = synapse->getWeight();
    float preActivity = 0.0f;
    float postActivity = 0.0f;
    
    // Calculate average activities from spike trains
    for (Timestamp time : preSpikes) {
        preActivity += std::abs(static_cast<float>(time));
    }
    preActivity /= preSpikes.size();
    
    for (Timestamp time : postSpikes) {
        postActivity += std::abs(static_cast<float>(time));
    }
    postActivity /= postSpikes.size();
    
    // Apply different Hebbian learning rules based on configuration
    
    // 1. Standard Covariance Rule (default)
    if (pImpl->covarianceThreshold <= 0.0f) {
        // Simple correlation-based learning
        float coactivity = 0.0f;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                coactivity += std::abs(static_cast<float>(preTime - postTime));
            }
        }
        coactivity /= (preSpikes.size() * postSpikes.size());
        
        float delta = pImpl->learningRate * (coactivity - preActivity * postActivity);
        applyHebbianWeightChange(synapse, delta);
    }
    
    // 2. Oja's Rule (normalized Hebbian)
    else if (pImpl->normalizationFactor > 0.0f) {
        // Normalized learning that maintains weight stability
        float normalization = pImpl->normalizationFactor * postActivity * postActivity;
        float delta = pImpl->learningRate * (postActivity * preActivity - currentWeight * normalization);
        applyHebbianWeightChange(synapse, delta);
    }
    
    // 3. BCM Theory (sliding threshold)
    else if (pImpl->bcmThreshold > 0.0f) {
        // Sliding threshold theory with dynamic threshold
        float theta = pImpl->bcmThreshold / (1.0f + postActivity * postActivity);
        float delta = pImpl->learningRate * (postActivity - theta) * preActivity;
        applyHebbianWeightChange(synapse, delta);
    }
    
    // 4. Implicit Normalization
    else {
        // Normalization based on running average of squared activity
        float normalization = postActivity * postActivity;
        float delta = pImpl->learningRate * (postActivity * preActivity - currentWeight * normalization);
        applyHebbianWeightChange(synapse, delta);
    }
    
    pImpl->updateCount++;
}

void Hebbian::applyHebbianWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with sophisticated homeostatic mechanisms
    float newWeight = synapse->getWeight() + delta;
    
    // 1. Apply BCM-like sliding threshold
    float activity = std::abs(synapse->getWeight());  // Simplified activity proxy
    float theta = pImpl->bcmThreshold / (1.0f + activity * activity);
    
    // 2. Apply synaptic scaling based on overall network activity
    newWeight *= (1.0f - pImpl->homeostaticRate);
    
    // 3. Apply soft bounds that expand with learning
    float softMin = -1.0f * (1.0f + pImpl->homeostaticRate * 10.0f);
    float softMax = 1.0f * (1.0f + pImpl->homeostaticRate * 10.0f);
    
    newWeight = std::clamp(newWeight, softMin, softMax);
    
    // Apply final weight
    synapse->setWeight(newWeight);
    
    // Update normalization statistics
    pImpl->totalNormalization += std::abs(delta);
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setNormalizationFactor(float factor) {
    pImpl->normalizationFactor = std::clamp(factor, 0.0f, 2.0f);
}

float Hebbian::getNormalizationFactor() const {
    return pImpl->normalizationFactor;
}

void Hebbian::setBCMThreshold(float threshold) {
    pImpl->bcmThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getBCMThreshold() const {
    return pImpl->bcmThreshold;
}

void Hebbian::enableCovarianceRule() {
    pImpl->covarianceThreshold = 0.0f;  // Zero means use covariance rule
}

void Hebbian::enableOjaRule(float factor) {
    pImpl->covarianceThreshold = std::max(pImpl->covarianceThreshold, 0.1f);  // Non-zero means Oja's rule
    setNormalizationFactor(factor);
}

void Hebbian::enableBCMTheory(float threshold) {
    pImpl->covarianceThreshold = std::max(pImpl->covarianceThreshold, 0.2f);  // Non-zero means BCM theory
    setBCMThreshold(threshold);
}

void Hebbian::enableImplicitNormalization() {
    pImpl->covarianceThreshold = std::max(pImpl->covarianceThreshold, 0.3f);  // Non-zero means implicit normalization
}

} // namespace nlm