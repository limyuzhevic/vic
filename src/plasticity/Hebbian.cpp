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
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZATION: Sort spike times for efficient processing
    // Create sorted copies to enable sliding window optimization
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use sliding window to find spike pairs within correlation window
    // Hebbian correlation typically lasts 100-200ms
    float correlationWindow = 100.0f;  // 100ms correlation window
    
    // Two-pointer approach to find spike pairs efficiently
    size_t preIdx = 0, postIdx = 0;
    size_t correlationCount = 0;
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        float dt = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes[preIdx]);
        
        if (std::abs(dt) <= correlationWindow) {
            // Within correlation window
            ++correlationCount;
            
            // Move both pointers to next spike
            ++preIdx;
            ++postIdx;
        } else if (dt < -correlationWindow) {
            // Post spike too old, move post pointer
            ++postIdx;
        } else {
            // Pre spike too old, move pre pointer
            ++preIdx;
        }
    }
    
    // Compute weight change based on correlation
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
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
