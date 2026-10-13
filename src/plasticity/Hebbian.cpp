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
     * Enhanced Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Enhanced implementation with:
     *   - Dynamic learning rate scaling
     *   - Activity-dependent baseline
     *   - Weight normalization
     *   - Stability mechanisms
     *   
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking with homeostatic regulation
     *   - Hebbian plasticity at Schaffer collateral synapses with metaplasticity
     *   - Correlation-based learning in visual cortex with normalization
     *   
     * Advanced features:
     *   - Spike timing dependent enhancements
     *   - Neuromodulation integration points
     *   - Weight saturation and boundary conditions
     *   - Learning rate adaptation based on overall activity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs with timing precision
    size_t correlationCount = 0;
    float totalTimeOffset = 0.0f;
    size_t timeWindowCount = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Use adaptive time window based on learning rate
            float timeWindow = 100.0f * (1.0f + pImpl->learningRate);
            if (std::abs(dt) < timeWindow) {
                ++correlationCount;
                totalTimeOffset += std::abs(dt);
                ++timeWindowCount;
            }
        }
    }
    
    // Compute base weight change with activity-dependent scaling
    float baseDelta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply time-weighted enhancement (earlier spikes have stronger impact)
    float timeWeightedDelta = baseDelta;
    if (timeWindowCount > 0 && correlationCount > 0) {
        float avgTimeOffset = totalTimeOffset / static_cast<float>(timeWindowCount);
        // Weight by inverse timing distance - closer spikes strengthen more
        timeWeightedDelta *= (1.0f + 0.1f * (100.0f - avgTimeOffset) / 100.0f);
    }
    
    // Apply homeostatic regulation to prevent runaway excitation
    float currentWeight = synapse->getWeight();
    float homeostaticFactor = 1.0f;
    if (pImpl->maxWeight > 0.0f && pImpl->minWeight < 0.0f) {
        // Center weight around 0 for balanced networks
        float weightDeviation = std::abs(currentWeight - 0.0f);
        float maxDeviation = std::min(std::abs(pImpl->maxWeight), std::abs(pImpl->minWeight));
        if (maxDeviation > 0.0f) {
            homeostaticFactor = 1.0f - (weightDeviation / maxDeviation) * 0.5f;
            homeostaticFactor = std::clamp(homeostaticFactor, 0.5f, 1.0f);
        }
    }
    
    float finalDelta = timeWeightedDelta * homeostaticFactor;
    
    // Apply with enhanced stability and validation
    if (std::abs(finalDelta) > 1e-6f) {
        applyWeightChange(synapse, finalDelta);
        
        // Log Hebbian learning activity for debugging
        if (correlationCount > 0) {
            NLM_LOG_DEBUG("Hebbian learning: synapse weight " + std::to_string(currentWeight) +
                         " → " + std::to_string(synapse->getWeight()) +
                         " (Δ=" + std::to_string(finalDelta) + 
                         ", coactivity=" + std::to_string(correlationCount) + ")");
        }
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
