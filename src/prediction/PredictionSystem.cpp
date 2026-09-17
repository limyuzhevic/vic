#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    std::shared_ptr<Brain> brain;
    
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // For temporal prediction
    std::unique_ptr<SensoryInput> lastObservation;
    float temporalDecay;
    
    Impl() : predictionError(0.0f), confidence(0.5f), temporalDecay(0.9f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionSystem with null brain pointer");
        return;
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain integration");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase 6: Use brain's prediction capabilities for realistic forward modeling
    if (!pImpl->brain) {
        // Fallback to simple prediction
        return currentState.clone();
    }
    
    // Use the brain's neural prediction system if available
    // For now, use simple temporal forward model
    auto prediction = std::make_unique<SensoryInput>(currentState);
    
    // Add small temporal noise to simulate prediction uncertainty
    auto& vision = prediction->getVision();
    for (float& v : vision) {
        if (pImpl->brain->getRandomGenerator()) {
            v += pImpl->brain->getRandomGenerator()->uniformReal(-0.1f, 0.1f);
            v = std::clamp(v, 0.0f, 1.0f);
        }
    }
    
    // Store for later comparison
    pImpl->lastObservation = prediction->clone();
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error using brain's prediction system
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = std::sqrt(sumError / predData.size());
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Trim error history
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    } else {
        NLM_LOG_WARNING("Prediction update failed: incompatible data sizes or empty predictions");
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Phase 6: Use brain's learning capabilities for prediction model training
    if (!pImpl->brain) return;
    
    // Use brain's neural plasticity to learn prediction patterns
    // The prediction system integrates with the brain's plasticity rules
    // For now, this is a placeholder that could be enhanced with real learning
}

void PredictionSystem::update(float dt) {
    // Temporal decay of prediction confidence
    pImpl->confidence *= std::pow(pImpl->temporalDecay, dt);
}

} // namespace nlm