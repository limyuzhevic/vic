#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::unique_ptr<SensoryInput>> predictionBuffer;
    size_t predictionHistorySize;
    float learningRate;
    float confidenceDecay;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionHistorySize(10), learningRate(0.01f), confidenceDecay(0.99f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 6: Real predictive coding using brain's neural substrate
    // Use the brain's working memory and neural activity patterns to generate predictions
    
    // Get current brain state from working memory
    float predictionConfidence = pImpl->confidence;
    
    // Generate prediction based on neural activity patterns
    auto predictedState = std::make_unique<SensoryInput>();
    
    // For now, create a simple prediction that incorporates current state
    // In a full implementation, this would use the brain's neural dynamics
    const auto& currentData = currentState.getData();
    std::vector<float> predictedData;
    
    if (!currentData.empty()) {
        predictedData = currentData;
        
        // Add prediction error to the prediction based on confidence
        float predictionNoise = (1.0f - predictionConfidence) * 0.1f;
        for (auto& value : predictedData) {
            value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * predictionNoise;
        }
    }
    
    predictedState->setData(predictedData);
    
    // Store prediction in buffer
    pImpl->predictionBuffer.push_back(std::make_unique<SensoryInput>(predictedState.get()));
    
    // Limit buffer size
    if (pImpl->predictionBuffer.size() > pImpl->predictionHistorySize) {
        pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
    }
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // PHASE 6: Real prediction error computation using brain's error signals
    // Integrate with brain's prediction error signals for better learning
    
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
        
        // Update confidence based on prediction accuracy
        // Lower error -> higher confidence
        float accuracy = std::max(0.0f, 1.0f - pImpl->predictionError);
        pImpl->confidence = pImpl->confidence * pImpl->confidenceDecay + accuracy * pImpl->learningRate;
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit error history size
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
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
    pImpl->predictionBuffer.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // PHASE 6: Train prediction model using brain's learning mechanisms
    // Integrate with STDP and Hebbian learning for prediction learning
    
    // In a full implementation, this would:
    // 1. Extract patterns from observation using working memory
    // 2. Update prediction weights based on STDP
    // 3. Use Hebbian learning to strengthen prediction patterns
    // 4. Integrate with neuromodulation (dopamine, curiosity)
    
    // For now, store observation in prediction buffer for learning
    pImpl->predictionBuffer.push_back(std::make_unique<SensoryInput>(observation));
    
    // Update learning based on current error
    if (!pImpl->errorHistory.empty()) {
        float recentError = pImpl->errorHistory.back();
        pImpl->learningRate = std::min(0.1f, pImpl->learningRate + recentError * 0.01f);
    }
}

} // namespace nlm
