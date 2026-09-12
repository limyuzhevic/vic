#include "MetaPlasticity.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct MetaPlasticity::Impl {
    float targetLearningRate;   // η_target (0.0 < rate < 1.0)
    float metaLearningRate;     // η_meta (meta-learning rate)
    float learningRate;         // η_current (current learning rate)
    
    Impl() : targetLearningRate(0.05f), metaLearningRate(0.001f), learningRate(0.01f) {}
};

MetaPlasticity::MetaPlasticity() : pImpl(new Impl) {}

MetaPlasticity::~MetaPlasticity() = default;

void MetaPlasticity::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    // Validate input parameters
    if (!synapse || dt <= 0.0) {
        return;
    }
    
    // Estimate current learning rate from recent plasticity effectiveness
    float currentLearningRate = estimateLearningRate(synapse, dt);
    
    // Calculate deviation from target learning rate
    float learningRateError = pImpl->targetLearningRate - currentLearningRate;
    
    // Apply meta-plastic weight change to adjust learning rate
    float weightChange = pImpl->metaLearningRate * learningRateError * synapse->getWeight();
    
    // Only apply significant changes
    if (std::abs(weightChange) > 1e-8f) {
        applyWeightChange(synapse, weightChange);
    }
}

void MetaPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;
    }
    
    // Apply meta-plastic weight change with bounds checking
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Ensure weight stays within reasonable bounds (-1.0 to 1.0)
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    // Only update if weight actually changed significantly
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        synapse->setWeight(newWeight);
    }
}

const char* MetaPlasticity::getName() const {
    return "MetaPlasticity";
}

void MetaPlasticity::setTargetLearningRate(float targetLearningRate) {
    pImpl->targetLearningRate = std::clamp(targetLearningRate, 0.001f, 0.5f);
}

float MetaPlasticity::getTargetLearningRate() const {
    return pImpl->targetLearningRate;
}

void MetaPlasticity::setMetaLearningRate(float rate) {
    pImpl->metaLearningRate = std::clamp(rate, 0.00001f, 0.01f);
}

float MetaPlasticity::getMetaLearningRate() const {
    return pImpl->metaLearningRate;
}

void MetaPlasticity::configure(float targetLearningRate, float rate) {
    setTargetLearningRate(targetLearningRate);
    setMetaLearningRate(rate);
}

float MetaPlasticity::estimateLearningRate(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0) {
        return 0.001f;  // Default fallback
    }
    
    // Simple estimate based on recent weight changes
    // This is a simplified version - a real implementation would track
    // recent learning history more accurately
    float currentWeight = synapse->getWeight();
    
    // Avoid division by zero and ensure reasonable bounds
    float estimatedRate = std::min(std::abs(currentWeight) * 0.01f, 0.1f);
    
    return estimatedRate;
}

} // namespace nlm