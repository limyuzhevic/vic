#include "Hebbian.hpp"
#include "PlasticityErrorHandler.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    // Validate input parameters
    nlm::ValidationUtils::validatePointerNotNull(synapse, "Hebbian::update: synapse pointer");
    nlm::ValidationUtils::validateNotEmpty(preSpikes, "Hebbian::update: preSpikes vector");
    nlm::ValidationUtils::validateNotEmpty(postSpikes, "Hebbian::update: postSpikes vector");
    nlm::ValidationUtils::validateRange(dt, 0.0, 1000.0, "Hebbian::update: timestep duration");
    
    // Check if plasticity is enabled
    nlm::ValidationUtils::validatePlasticityEnabled(true, "Hebbian::update");
    
    try {
        // Count correlated spike pairs (simplified covariance)
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
        
        // Compute weight change based on correlation
        float delta = pImpl->learningRate * static_cast<float>(correlationCount);
        
        // Apply with bounds
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
        
    } catch (const std::exception& e) {
        nlm::PlasticityErrorHandler::handleHebbianError("Hebbian::update", e.what());
    }
}

void Hebbian::applyWeightChange(SynapticWeight delta) {
    // Validate input parameters
    ValidationUtils::validateRange(delta, pImpl->minWeight, pImpl->maxWeight,
                                  "Hebbian::applyWeightChange: delta validation");
    
    try {
        // Apply weight change
        float newWeight = pImpl->learningRate + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        pImpl->learningRate = newWeight;
        
    } catch (const std::exception& e) {
        nlm::PlasticityErrorHandler::handleHebbianError("Hebbian::applyWeightChange", e.what());
    }
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    // Validate input
    ValidationUtils::validateRange(rate, 0.0f, 1.0f,
                                  "Hebbian::setLearningRate: rate validation");
    
    pImpl->learningRate = rate;
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    // Validate input
    ValidationUtils::validateRange(maxWeight, 0.0f, 10.0f,
                                  "Hebbian::setMaxWeight: maxWeight validation");
    
    pImpl->maxWeight = maxWeight;
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
