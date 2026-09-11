// BrainPredictionIntegration.cpp - Prediction system integration implementation
// Contains prediction system integration functionality

#include "BrainPredictionIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Implementation of BrainPredictionIntegration

BrainPredictionIntegration::BrainPredictionIntegration(BrainCore* core) : pImpl(new Impl(core)) {}

BrainPredictionIntegration::~BrainPredictionIntegration() = default;

BrainPredictionIntegration::BrainPredictionIntegration(BrainPredictionIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainPredictionIntegration& BrainPredictionIntegration::operator=(BrainPredictionIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainPredictionIntegration::initialize() {
    NLM_LOG_INFO("Initializing Brain Prediction Integration...");
    
    if (!pImpl->brainCore) {
        NLM_LOG_ERROR("Cannot initialize prediction integration - no brain core available");
        return false;
    }
    
    pImpl->predictionSystem = pImpl->brainCore->getPredictionSystem();
    
    NLM_LOG_INFO("Brain Prediction Integration initialized successfully");
    return true;
}

void BrainPredictionIntegration::update(SimulationStep currentStep, Timestamp currentTime) {
    updatePredictionSystem(currentStep, currentTime);
}

void BrainPredictionIntegration::processPredictionError(const std::vector<float>& currentActivity,
                                                       const std::vector<float>& previousActivity) {
    calculatePredictionError(currentActivity, previousActivity);
}

void BrainPredictionIntegration::storeCurrentSensoryActivity(const std::vector<float>& activity) {
    pImpl->previousSensoryActivity = pImpl->currentSensoryActivity;
    pImpl->currentSensoryActivity = activity;
    
    // Limit storage size
    if (pImpl->currentSensoryActivity.size() > 100) {
        pImpl->currentSensoryActivity.erase(pImpl->currentSensoryActivity.begin());
    }
    if (pImpl->previousSensoryActivity.size() > 100) {
        pImpl->previousSensoryActivity.erase(pImpl->previousSensoryActivity.begin());
    }
}

PredictionSystem* BrainPredictionIntegration::getPredictionSystem() {
    return pImpl->predictionSystem;
}

float BrainPredictionIntegration::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

void BrainPredictionIntegration::clear() {
    pImpl->currentSensoryActivity.clear();
    pImpl->previousSensoryActivity.clear();
    pImpl->predictionErrorsProcessed = 0;
    pImpl->predictionConfidence = 0.0f;
}

size_t BrainPredictionIntegration::getPredictionErrorsProcessed() const {
    return pImpl->predictionErrorsProcessed;
}

void BrainPredictionIntegration::Impl::Impl(BrainCore* core) : brainCore(core), predictionSystem(nullptr), 
    predictionErrorsProcessed(0), predictionConfidence(0.0f) {}

void BrainPredictionIntegration::updatePredictionSystem(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->brainCore || !pImpl->predictionSystem) return;
    
    // Process prediction error if we have both current and previous activity
    if (!pImpl->previousSensoryActivity.empty() && !pImpl->currentSensoryActivity.empty()) {
        calculatePredictionError(pImpl->currentSensoryActivity, pImpl->previousSensoryActivity);
    }
    
    // Update prediction system confidence based on stability
    if (!pImpl->previousSensoryActivity.empty()) {
        float predictionError = 0.0f;
        for (size_t i = 0; i < std::min(pImpl->previousSensoryActivity.size(), pImpl->currentSensoryActivity.size()); ++i) {
            predictionError += std::abs(pImpl->previousSensoryActivity[i] - pImpl->currentSensoryActivity[i]);
        }
        if (pImpl->previousSensoryActivity.size() > 0) {
            predictionError /= pImpl->previousSensoryActivity.size();
        }
        float stability = 1.0f / (1.0f + predictionError);
        updatePredictionConfidence(stability);
    }
}

void BrainPredictionIntegration::calculatePredictionError(const std::vector<float>& currentActivity,
                                                         const std::vector<float>& previousActivity) {
    if (currentActivity.empty() || previousActivity.empty() || !pImpl->predictionSystem) return;
    
    float predictionError = 0.0f;
    for (size_t i = 0; i < std::min(currentActivity.size(), previousActivity.size()); ++i) {
        predictionError += std::abs(currentActivity[i] - previousActivity[i]);
    }
    
    if (!previousActivity.empty()) {
        predictionError /= previousActivity.size();
    }
    
    pImpl->predictionSystem->update(predictionError);
    pImpl->predictionErrorsProcessed++;
}

void BrainPredictionIntegration::updatePredictionConfidence(float stability) {
    pImpl->predictionConfidence = stability;
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->updateConfidence(stability);
    }
}

void BrainPredictionIntegration::calculatePredictionStats() const {}

} // namespace nlm

