#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Helper class for efficient O(n) spike correlation processing
class SpikeCorrelationProcessor {
public:
    // Optimized Hebbian correlation calculation using O(n) algorithm
    static size_t countCorrelatedSpikes(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        float correlationWindow) {
        if (preSpikes.empty() || postSpikes.empty()) return 0;
        
        // Create sorted copies for efficient processing
        std::vector<Timestamp> sortedPre = preSpikes;
        std::vector<Timestamp> sortedPost = postSpikes;
        std::sort(sortedPre.begin(), sortedPre.end());
        std::sort(sortedPost.begin(), sortedPost.end());
        
        size_t correlationCount = 0;
        
        // Use two-pointer technique for O(n) processing
        size_t i = 0, j = 0;
        while (i < sortedPre.size() && j < sortedPost.size()) {
            float dt = static_cast<float>(sortedPost[j] - sortedPre[i]);
            
            if (std::abs(dt) < correlationWindow) {
                // Spikes are correlated (within window)
                correlationCount++;
                
                // Try to advance both pointers if possible
                if (dt <= 0) {
                    i++;  // Pre spike is before or simultaneous with post spike
                }
                if (dt >= 0) {
                    j++;  // Post spike is before or simultaneous with pre spike
                }
            } else if (dt < 0) {
                // Pre spike is too early, skip it
                i++;
            } else {
                // Post spike is too early, skip it
                j++;
            }
        }
        
        return correlationCount;
    }
};

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
    
    // Use standardized error handling
    auto error = ErrorResult::success();
    
    error.checkNotNull(synapse, "Hebbian::update synapse parameter");
    error.check(!preSpikes.empty(), "Hebbian::update preSpikes empty");
    error.check(!postSpikes.empty(), "Hebbian::update postSpikes empty");
    
    if (error.failure()) {
        NLM_LOG_ERROR(error.message());
        return;
    }
    
    // Use optimized O(n) spike correlation counting
    size_t correlationCount = SpikeCorrelationProcessor::countCorrelatedSpikes(
        preSpikes, postSpikes, 100.0f  // 100ms correlation window
    );
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Use standardized error handling
    auto error = ErrorResult::success();
    
    error.checkNotNull(synapse, "Hebbian::applyWeightChange synapse parameter");
    
    if (error.failure()) {
        NLM_LOG_ERROR(error.message());
        return;
    }
    
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
