#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

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
    // Input validation
    if (synapse == nullptr) {
        NLM_LOG_WARN("Hebbian::update: synapse is null");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        // No spikes to process - this is normal, not an error
        return;
    }
    
    if (dt <= 0.0f) {
        NLM_LOG_WARN("Hebbian::update: invalid timestep duration: " + std::to_string(dt));
        return;
    }
    
    try {
        // Validate parameters
        if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) {
            NLM_LOG_WARN("Hebbian::update: invalid learning rate: " + std::to_string(pImpl->learningRate));
            pImpl->learningRate = std::clamp(pImpl->learningRate, 0.0f, 1.0f);
        }
        
        if (pImpl->maxWeight < pImpl->minWeight) {
            NLM_LOG_WARN("Hebbian::update: invalid weight bounds: max=" + std::to_string(pImpl->maxWeight) +
                        ", min=" + std::to_string(pImpl->minWeight));
            std::swap(pImpl->maxWeight, pImpl->minWeight);
        }
        
        // Count correlated spike pairs (simplified covariance)
        size_t correlationCount = 0;
        
        for (size_t i = 0; i < preSpikes.size(); ++i) {
            Timestamp preTime = preSpikes[i];
            
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                Timestamp postTime = postSpikes[j];
                float dt = static_cast<float>(postTime - preTime);
                
                // Count spikes within a broad time window as correlated
                if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                    ++correlationCount;
                }
            }
        }
        
        // Compute weight change based on correlation
        // More sophisticated: use actual spike counts and firing rates
        float delta = pImpl->learningRate * static_cast<float>(correlationCount);
        
        // Apply threshold: only learn if correlation exceeds threshold
        if (std::abs(delta) > pImpl->covarianceThreshold) {
            // Apply with bounds
            if (std::abs(delta) > 1e-6f) {
                try {
                    applyWeightChange(synapse, delta);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Hebbian::update: error applying weight change: " + std::string(e.what()));
                }
            }
        }
        
        // Log activity for debugging
        if (correlationCount > 0) {
            NLM_LOG_DEBUG("Hebbian::update: correlation count=" + std::to_string(correlationCount) +
                         ", delta=" + std::to_string(delta) +
                         ", final_weight=" + std::to_string(synapse->getWeight()));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Hebbian::update: error processing spike pairs: " + std::string(e.what()));
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse == nullptr) {
        NLM_LOG_WARN("Hebbian::applyWeightChange: synapse is null");
        return;
    }
    
    if (std::abs(delta) < 1e-6f) {
        // Very small weight change - skip to avoid numerical issues
        return;
    }
    
    try {
        float newWeight = synapse->getWeight() + delta;
        
        // Check bounds
        if (newWeight < pImpl->minWeight) {
            NLM_LOG_DEBUG("Hebbian::applyWeightChange: weight " + std::to_string(newWeight) +
                         " below minimum " + std::to_string(pImpl->minWeight) +
                         ", clamping to minimum");
            newWeight = pImpl->minWeight;
        } else if (newWeight > pImpl->maxWeight) {
            NLM_LOG_DEBUG("Hebbian::applyWeightChange: weight " + std::to_string(newWeight) +
                         " above maximum " + std::to_string(pImpl->maxWeight) +
                         ", clamping to maximum");
            newWeight = pImpl->maxWeight;
        }
        
        // Check if weight actually changed
        if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
            synapse->setWeight(newWeight);
            NLM_LOG_DEBUG("Hebbian::applyWeightChange: weight changed from " +
                         std::to_string(synapse->getWeight() - delta) + " to " + std::to_string(newWeight));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Hebbian::applyWeightChange: error applying weight change: " + std::string(e.what()));
    }
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    if (rate < 0.0f) {
        NLM_LOG_WARN("Hebbian::setLearningRate: negative learning rate will be clamped: " + std::to_string(rate));
    } else if (rate > 1.0f) {
        NLM_LOG_WARN("Hebbian::setLearningRate: learning rate > 1.0 may cause runaway potentiation: " + std::to_string(rate));
    }
    
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    if (maxWeight < pImpl->minWeight) {
        NLM_LOG_WARN("Hebbian::setMaxWeight: max weight " + std::to_string(maxWeight) +
                    " < min weight " + std::to_string(pImpl->minWeight) + 
                    ", swapping values");
        std::swap(maxWeight, pImpl->minWeight);
    }
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setMinWeight(float minWeight) {
    if (minWeight > pImpl->maxWeight) {
        NLM_LOG_WARN("Hebbian::setMinWeight: min weight " + std::to_string(minWeight) +
                    " > max weight " + std::to_string(pImpl->maxWeight) + 
                    ", swapping values");
        std::swap(minWeight, pImpl->maxWeight);
    }
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setCovarianceThreshold(float threshold) {
    if (threshold < 0.0f) {
        NLM_LOG_WARN("Hebbian::setCovarianceThreshold: negative threshold will be made positive: " + std::to_string(threshold));
    }
    pImpl->covarianceThreshold = std::max(0.0f, threshold);
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::configure(float learningRate, float maxWeight, float minWeight, 
                       float covarianceThreshold) {
    if (std::abs(learningRate - pImpl->learningRate) < 1e-6f && 
        std::abs(maxWeight - pImpl->maxWeight) < 1e-6f &&
        std::abs(minWeight - pImpl->minWeight) < 1e-6f &&
        std::abs(covarianceThreshold - pImpl->covarianceThreshold) < 1e-6f) {
        return; // No change needed
    }
    
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
    setMinWeight(minWeight);
    setCovarianceThreshold(covarianceThreshold);
    
    NLM_LOG_INFO("Hebbian configured: learningRate=" + std::to_string(pImpl->learningRate) +
                ", maxWeight=" + std::to_string(pImpl->maxWeight) +
                ", minWeight=" + std::to_string(pImpl->minWeight) +
                ", covarianceThreshold=" + std::to_string(pImpl->covarianceThreshold));
}

} // namespace nlm