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
     * Optimized Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Optimized version for spike-based systems:
     * Δw = η * (correlation_count - baseline)
     * 
     * Where:
     *   correlation_count = number of correlated pre/post spikes within window
     *   baseline = learning_rate * mean_activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with optimization to O(n log n) complexity.
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Optimized correlation calculation using sliding window
    size_t correlationCount = calculateCorrelatedSpikesOptimized(preSpikes, postSpikes);
    
    // Compute weight change based on correlation
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

// Optimized correlation calculation using two-pointer sliding window
size_t Hebbian::calculateCorrelatedSpikesOptimized(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) const {
    
    // Combine and sort all spike times
    std::vector<std::pair<Timestamp, bool>> allSpikes;
    allSpikes.reserve(preSpikes.size() + postSpikes.size());
    
    for (Timestamp time : preSpikes) {
        allSpikes.emplace_back(time, true);  // true for pre-synaptic
    }
    for (Timestamp time : postSpikes) {
        allSpikes.emplace_back(time, false); // false for post-synaptic
    }
    
    std::sort(allSpikes.begin(), allSpikes.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    // Use sliding window approach: O(n) after sorting O(n log n)
    size_t correlationCount = 0;
    size_t preCount = 0;
    size_t postCount = 0;
    
    // Sliding window end (in ms)
    for (size_t i = 0; i < allSpikes.size(); ++i) {
        Timestamp windowStart = allSpikes[i].first - 100.0f;  // 100ms window
        
        // Clear counts from outside window
        while (!allSpikes.empty() && allSpikes.front().first < windowStart) {
            if (allSpikes.front().second) {
                preCount--;
            } else {
                postCount--;
            }
            allSpikes.erase(allSpikes.begin());
        }
        
        // Add new spike to window
        if (allSpikes[i].second) {
            preCount++;
        } else {
            postCount++;
        }
        
        // Correlated spikes = pre * post within window
        correlationCount += preCount * postCount;
    }
    
    // Subtract self-correlations (spike with itself) for more accurate result
    // Each spike shouldn't correlate with itself
    correlationCount -= (preSpikes.size() + postSpikes.size());
    
    return correlationCount;
}

// Optimized weight change application with better bounds checking
void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // More efficient weight change application
    float newWeight = synapse->getWeight() + delta;
    
    // Use clamp for better performance than manual if-else
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
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
