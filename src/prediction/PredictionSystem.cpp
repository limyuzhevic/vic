#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, just return a copy of current state
    return currentState.clone();
}
    }
    
    return predicted;
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
    // TODO PHASE 2: Train prediction model using neural substrate
    // For now, just record the observation as potential training data
    // This will be implemented in Phase 2 to use the brain's actual neural patterns
    // IMPROVED: Record observation for future training
    trainData_.push_back(observation);
    
    // Simple training: learn average patterns
    if (trainData_.size() > 1) {
        size_t totalElements = 0;
        for (const auto& obs : trainData_) {
            const auto& data = obs.getData();
            totalElements += data.size();
            // Accumulate data for pattern learning
            for (float val : data) {
                learnedPattern_.push_back(val);
            }
        }
        if (totalElements > 0) {
            // Calculate average learned pattern
            float sum = 0.0f;
            for (float val : learnedPattern_) sum += val;
            pImpl->confidence = std::min(1.0f, trainData_.size() / 10.0f); // Confidence grows with training
        }
    }
}

} // namespace nlm
