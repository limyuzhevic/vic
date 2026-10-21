#include "OjaRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct OjaRule::Impl {
    // Oja's rule parameters
    float learningRate;     // Learning rate (η)
    float normalizationFactor;  // Normalization factor (λ)
    
    // Weight bounds
    float maxWeight;         // Maximum weight bound
    float minWeight;         // Minimum weight bound
    
    // Running statistics for normalization
    float runningVariance;   // Running variance of weights
    float sumSquares;        // Sum of squared weights
    float updateCount;       // Number of updates performed
    
    Impl() : learningRate(0.01f), normalizationFactor(1.0f), maxWeight(1.0f),
             minWeight(-1.0f), runningVariance(0.0f), sumSquares(0.0f), updateCount(0) {}
};

OjaRule::OjaRule() : pImpl(new Impl) {}

OjaRule::~OjaRule() = default;

void OjaRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Simple Oja's rule implementation for spike-based systems
    // Δw = η * x * y - η * λ * w * y²
    // Where x is pre-synaptic activity, y is post-synaptic activity, w is weight
    
    // Use average spike counts as proxy for pre and post activity
    size_t preActivity = preSpikes.size();
    size_t postActivity = postSpikes.size();
    
    if (preActivity == 0 || postActivity == 0) {
        return;
    }
    
    float preSignal = static_cast<float>(preActivity);
    float postSignal = static_cast<float>(postActivity);
    float weight = synapse->getWeight();
    
    // Calculate Oja's rule weight change
    float delta = pImpl->learningRate * preSignal * postSignal -
                  pImpl->learningRate * pImpl->normalizationFactor * weight * postSignal * postSignal;
    
    // Update running statistics for normalization
    updateRunningVariance(delta);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void OjaRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* OjaRule::getName() const {
    return "OjaRule";
}

void OjaRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float OjaRule::getLearningRate() const {
    return pImpl->learningRate;
}

void OjaRule::setNormalizationFactor(float factor) {
    pImpl->normalizationFactor = std::clamp(factor, 0.1f, 10.0f);
}

float OjaRule::getNormalizationFactor() const {
    return pImpl->normalizationFactor;
}

void OjaRule::setMaxWeight(float max) {
    pImpl->maxWeight = std::clamp(max, 0.0f, 10.0f);
}

float OjaRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void OjaRule::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
}

float OjaRule::getMinWeight() const {
    return pImpl->minWeight;
}

void OjaRule::updateRunningVariance(SynapticWeight weightDelta) {
    // Update running variance for normalization
    // This is a simplified version for demonstration
    pImpl->updateCount++;
    
    // Update running statistics
    float oldVariance = pImpl->runningVariance;
    float newVariance = oldVariance + (weightDelta * weightDelta / pImpl->updateCount) -
                       ((pImpl->updateCount - 1.0f) / pImpl->updateCount) * (weightDelta * weightDelta);
    
    pImpl->runningVariance = newVariance;
    pImpl->sumSquares = pImpl->runningVariance * pImpl->updateCount;
}

float OjaRule::getRunningVariance() const {
    return pImpl->runningVariance;
}

float OjaRule::calculateNormalizedDelta(SynapticWeight weight, SynapticWeight preSignal, SynapticWeight postSignal) const {
    // Calculate normalized weight change using Oja's rule
    float delta = pImpl->learningRate * preSignal * postSignal -
                  pImpl->learningRate * pImpl->normalizationFactor * weight * postSignal * postSignal;
    return delta;
}

} // namespace nlm