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
    // Simple predictive model: predict continuation of current pattern
    // In a real implementation, this would use neural dynamics to predict future states
    auto predicted = currentState.clone();
    
    // Apply small forward change to simulate prediction
    const auto& data = predicted->getData();
    auto mutableData = predicted->getData();
    for (size_t i = 0; i < mutableData.size(); ++i) {
        // Simple linear extrapolation with decay
        mutableData[i] *= 0.95f + 0.05f * static_cast<float>(i % 5) / 5.0f;
    }
    predicted->setData(mutableData);
    
    pImpl->confidence = 0.7f; // Moderate confidence for simple prediction
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error using mean squared error
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
    // Train prediction model using neural patterns
    // In a full implementation, this would update internal neural weights
    // to improve prediction accuracy based on observed states
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
}

} // namespace nlm
