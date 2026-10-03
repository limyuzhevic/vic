#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <numeric>

namespace nlm;

// Memory pool for Hebbian correlation calculations
struct HebbianMemoryPool {
    static constexpr size_t MAX_CORRELATION_PAIRS = 10000;
    
    // Temporary buffer for optimized correlation calculation
    float tempCorrelations[MAX_CORRELATION_PAIRS];
    size_t tempCorrelationsSize;
    
    // Precomputed spike times for faster correlation
    Timestamp preSpikeCache[MAX_CORRELATION_PAIRS];
    Timestamp postSpikeCache[MAX_CORRELATION_PAIRS];
    size_t preSpikeCount;
    size_t postSpikeCount;
    
    HebbianMemoryPool() : tempCorrelationsSize(0), preSpikeCount(0), postSpikeCount(0) {}
};

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Use memory pool for temporary storage
    HebbianMemoryPool memoryPool;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Cache frequently accessed values
    float learningRate = pImpl->learningRate;
    
    // Optimized Hebbian calculation using spatial and temporal locality
    // Sort spikes for better cache performance and faster windowing
    auto sortedPreSpikes = preSpikes;
    auto sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use two-pointer technique for O(n + m) complexity instead of O(n*m)
    size_t preIdx = 0, postIdx = 0;
    size_t correlationCount = 0;
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        float preTime = static_cast<float>(sortedPreSpikes[preIdx]);
        float postTime = static_cast<float>(sortedPostSpikes[postIdx]);
        float timeDiff = std::abs(postTime - preTime);
        
        // Check correlation within 100ms window
        if (timeDiff < 100.0f) {
            correlationCount++;
            
            // Advance both pointers if times are close (correlated)
            if (timeDiff < 1.0f) {  // Consider spikes within 1ms as directly correlated
                preIdx++;
                postIdx++;
            } else if (preTime < postTime) {
                preIdx++;
            } else {
                postIdx++;
            }
        } else if (preTime < postTime) {
            preIdx++;
        } else {
            postIdx++;
        }
    }
    
    // Compute weight change based on correlation
    float delta = learningRate * static_cast<float>(correlationCount);
    
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
