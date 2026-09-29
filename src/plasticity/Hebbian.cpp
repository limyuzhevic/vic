#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Advanced learning parameters
    float ojaLearningRate;      // For Oja's rule
    float bcmThreshold;         // BCM theory threshold
    float bcmLearningRate;      // BCM theory learning rate
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), ojaLearningRate(0.01f),
             bcmThreshold(0.0f), bcmLearningRate(0.01f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Advanced Hebbian learning implementation
     * 
     * Mathematical formulations:
     * 
     * 1. Classical Hebbian (Correlated firing):
     * Δw = η * (coactivity - baseline)
     * 
     * 2. Oja's rule (weight normalization):
     * Δw = η * (y * x - w * y²)
     * 
     * 3. BCM theory (Bilateral Connectivity Model):
     * Δw = η * y * (y - θ(y)) where θ(y) = y² / (y² + κ²)
     * 
     * 4. Covariance rule:
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike statistics
    float preRate = static_cast<float>(preSpikes.size()) / 1000.0f;  // spikes per second
    float postRate = static_cast<float>(postSpikes.size()) / 1000.0f;
    
    // 1. Classical Hebbian learning (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // 2. Oja's rule (additional normalization term)
    float preActivity = preRate;
    float postActivity = postRate;
    float normalizationTerm = pImpl->ojaLearningRate * (postActivity * preActivity - synapse->getWeight() * postActivity * postActivity);
    
    // 3. BCM theory (threshold-based learning)
    float bcmThreshold = pImpl->bcmThreshold > 0.0f ? pImpl->bcmThreshold : 0.5f;
    float bcmTheta = postActivity * postActivity / (postActivity * postActivity + bcmThreshold * bcmThreshold);
    float bcmDelta = pImpl->bcmLearningRate * postActivity * (postActivity - bcmTheta);
    
    // 4. Covariance rule (full covariance calculation)
    float preMean = preRate;
    float postMean = postRate;
    float prePostCovariance = 0.0f;
    
    if (preSpikes.size() > 1 && postSpikes.size() > 1) {
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float preVal = static_cast<float>(preTime) / 1000.0f;
                float postVal = static_cast<float>(postTime) / 1000.0f;
                prePostCovariance += (preVal - preMean) * (postVal - postMean);
            }
        }
        prePostCovariance /= (preSpikes.size() * postSpikes.size());
    }
    
    float covarianceDelta = pImpl->learningRate * prePostCovariance;
    
    // Combine different learning rules with adaptive weighting
    float totalDelta = delta * 0.3f + normalizationTerm * 0.3f + bcmDelta * 0.2f + covarianceDelta * 0.2f;
    
    // Apply with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
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
