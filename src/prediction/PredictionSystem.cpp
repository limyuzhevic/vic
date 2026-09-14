#include "PredictionSystem.hpp"
#include "../core/Random/Random.hpp"
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& obsData = observation.getData();
    if (obsData.empty()) return;
    
    float currentError = 0.1f;
    
    pImpl->errorHistory.push_back(currentError);
    
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    std::vector<float> predictedData = currentData;
    float smoothingFactor = 0.9f;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.5f, 0.5f);
    
    for (size_t i = 0; i < predictedData.size(); ++i) {
        float noise = dis(gen) * 0.1f;
        predictedData[i] = currentData[i] * smoothingFactor + noise;
    }
    
    auto predictedInput = std::make_unique<Vision>(1, static_cast<size_t>(predictedData.size()), 1);
    predictedInput->setData(predictedData);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumSquaredError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumSquaredError += error * error;
        }
        pImpl->predictionError = sumSquaredError / predData.size();
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

} // namespace nlm
