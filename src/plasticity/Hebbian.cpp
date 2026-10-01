#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float correlationWindow;    // 100ms window for correlation counting
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), correlationWindow(100.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Optimized O(n+m) Hebbian learning implementation using two-pointer technique
     * 
     * Original O(n²) implementation compared every pre-spike with every post-spike
     * New O(n+m) implementation efficiently counts spike pairs within correlation window
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Create sorted copies of spike times (assuming they're not already sorted)
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Two-pointer technique for O(n+m) correlation counting
    size_t preIdx = 0;
    size_t postIdx = 0;
    size_t correlationCount = 0;
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        Timestamp preTime = sortedPreSpikes[preIdx];
        Timestamp postTime = sortedPostSpikes[postIdx];
        
        float dt = static_cast<float>(postTime - preTime);
        
        if (std::abs(dt) < pImpl->correlationWindow) {
            // Spike pair is within correlation window
            correlationCount++;
            
            // Move both pointers since both spikes have been processed
            // (this spike pair is accounted for)
            preIdx++;
            postIdx++;
        } else if (dt < 0) {
            // Post spike is before pre spike by more than correlation window
            // Move to next pre spike to catch up
            preIdx++;
        } else {
            // dt > 0: pre spike is before post spike by more than correlation window
            // Move to next post spike to catch up
            postIdx++;
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
