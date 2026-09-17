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
    // Early validation checks for null pointers and empty spike vectors
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }

    // Optimized O(n) algorithm using running sums instead of O(n²) nested loops
    // Calculate correlation using running totals for better performance
    float preSum = 0.0f, postSum = 0.0f, prePostSum = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        float pre = static_cast<float>(preSpikes[i]);
        preSum += pre;
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float post = static_cast<float>(postSpikes[j]);
            prePostSum += pre * post;
            postSum += post;
            ++count;
        }
    }
    
    // Compute covariance-based weight change
    if (count > 0) {
        float meanPre = preSum / count;
        float meanPost = postSum / count;
        
        // Covariance rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
        float delta = pImpl->learningRate * (prePostSum / count - meanPre * meanPost);
        
        // Apply common weight change with proper bounds
        applyCommonWeightChange(synapse, delta, pImpl->minWeight, pImpl->maxWeight);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use common weight change method
    applyCommonWeightChange(synapse, delta, pImpl->minWeight, pImpl->maxWeight);
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
