#include "PredictionSystem.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> lastConsensusPattern;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

void PredictionSystem::updateWithAssociations(const std::vector<std::vector<float>>& associatedPatterns) {
    // Use associative memory patterns to refine predictions
    // More associated patterns = higher prediction confidence
    float associationCount = static_cast<float>(associatedPatterns.size());
    pImpl->confidence = std::min(1.0f, pImpl->confidence + associationCount * 0.1f);
    
    // Average patterns to create consensus prediction
    if (!associatedPatterns.empty()) {
        size_t patternSize = associatedPatterns[0].size();
        std::vector<float> consensusPattern(patternSize, 0.0f);
        
        for (const auto& pattern : associatedPatterns) {
            for (size_t i = 0; i < patternSize; ++i) {
                consensusPattern[i] += pattern[i];
            }
        }
        
        for (size_t i = 0; i < patternSize; ++i) {
            consensusPattern[i] /= associatedPatterns.size();
        }
        
        // Store for potential use by concept formation
        pImpl->lastConsensusPattern = consensusPattern;
    }
}

const std::vector<float>& PredictionSystem::getLastConsensusPattern() const {
    return pImpl->lastConsensusPattern;
}

} // namespace nlm
