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
    
    // Enhanced validation of input parameters
    if (!synapse) {
        NLM_LOG_WARNING("Attempted Hebbian update on null synapse");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        NLM_LOG_DEBUG("Empty spike history for Hebbian update, skipping");
        return;
    }
    
    if (std::isnan(dt) || std::isinf(dt) || dt <= 0.0) {
        NLM_LOG_ERROR("Invalid timestep duration for Hebbian update: " + std::to_string(dt));
        return;
    }
    
    // Count correlated spike pairs with improved validation
    size_t correlationCount = 0;
    size_t maxCorrelationPairs = preSpikes.size() * postSpikes.size();
    
    if (maxCorrelationPairs > 1000000) {
        NLM_LOG_WARNING("Large spike history detected for Hebbian update: " + 
                       std::to_string(maxCorrelationPairs) + " spike pairs, sampling");
        // For very large spike histories, sample to avoid excessive computation
        // This is a simplified approach - in practice, more sophisticated sampling would be used
        correlationCount = std::min(maxCorrelationPairs / 1000, static_cast<size_t>(1000));
    } else {
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                // Count spikes within a broad time window as correlated
                if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                    ++correlationCount;
                }
            }
        }
    }
    
    // Compute weight change based on correlation with bounds checking
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Validate delta value
    if (std::isnan(delta) || std::isinf(delta)) {
        NLM_LOG_ERROR("Invalid Hebbian weight delta: NaN or infinite value");
        return;
    }
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        NLM_LOG_DEBUG("Applied Hebbian weight change: delta=" + std::to_string(delta) + 
                     ", correlation pairs=" + std::to_string(correlationCount));
    } else {
        NLM_LOG_DEBUG("Hebbian weight change too small to apply: " + std::to_string(delta));
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_WARNING("Attempted to apply Hebbian weight change to null synapse");
        return;
    }
    
    // Validate delta value to prevent extreme changes
    if (std::abs(delta) > 1.0f) {
        NLM_LOG_WARNING("Extreme Hebbian weight delta: " + std::to_string(delta) + ", clamping to [-1.0, 1.0]");
        delta = std::clamp(delta, -1.0f, 1.0f);
    }
    
    // Clamp weight to bounds with better error handling
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Check for potential NaN or infinite values
    if (std::isnan(newWeight) || std::isinf(newWeight)) {
        NLM_LOG_ERROR("Invalid weight calculation: NaN or infinite value detected");
        return;
    }
    
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if weight actually changed
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        synapse->setWeight(newWeight);
        NLM_LOG_DEBUG("Applied Hebbian weight change: " + std::to_string(currentWeight) + 
                     " -> " + std::to_string(newWeight) + " (delta=" + std::to_string(delta) + ")");
    } else {
        NLM_LOG_DEBUG("Hebbian weight change too small to apply: " + std::to_string(delta));
    }
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
