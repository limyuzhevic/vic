#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Use brain reference for neural prediction
    Brain* brain;
    
    // Neural prediction model that uses the brain's substrate
    std::unique_ptr<NeuralPredictionModel> neuralModel;
    
    // Prediction error tracking
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Training data
    std::vector<SensoryInput> trainingData;
    
    // For generating temporal patterns
    std::mt19937 rng;
    
    Impl(Brain* brain) : brain(brain), predictionError(0.0f), confidence(0.5f) {
        neuralModel = std::make_unique<NeuralPredictionModel>(brain);
        rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl(nullptr)) {}

PredictionSystem::PredictionSystem(Brain* brain) : pImpl(new Impl(brain)) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use neural prediction model if brain is available
    if (pImpl->brain) {
        return pImpl->neuralModel->predictNextState(currentState);
    }
    
    // Fallback to enhanced version of current state
    auto result = currentState.clone();
    auto& data = result->getData();
    
    // Apply predictive dynamics
    if (!data.empty()) {
        // Add temporal trend prediction
        for (size_t i = 1; i < data.size(); ++i) {
            float trend = data[i] - data[i-1];
            if (std::abs(trend) > 0.01f) {
                // Predict continued trend
                float predictedNext = data[i] + trend * 0.5f;
                predictedNext = std::max(0.0f, std::min(1.0f, predictedNext));
                data[i] = predictedNext;
            }
        }
        
        // Add noise for exploration
        std::uniform_real_distribution<float> noise(-0.05f, 0.05f);
        for (float& val : data) {
            val += noise(pImpl->rng);
            val = std::max(0.0f, std::min(1.0f, val));
        }
    }
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error
    float error = 0.0f;
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumSquaredError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
        }
        error = std::sqrt(sumSquaredError / predData.size());
    } else {
        error = 1.0f; // Maximum error if shapes don't match
    }
    
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Keep error history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update neural prediction model based on error
    if (pImpl->brain && pImpl->neuralModel) {
        pImpl->neuralModel->updatePredictionModel(predicted, actual, error);
    }
    
    // Update confidence based on error history
    updateConfidence();
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
    // Store observation for training
    pImpl->trainingData.push_back(observation);
    
    // Keep training data bounded
    if (pImpl->trainingData.size() > 10000) {
        pImpl->trainingData.erase(pImpl->trainingData.begin());
    }
    
    // Train neural prediction model
    if (pImpl->brain && pImpl->neuralModel) {
        pImpl->neuralModel->trainWithObservation(observation);
    }
}

void PredictionSystem::updateConfidence() {
    if (pImpl->errorHistory.empty()) {
        pImpl->confidence = 0.5f;
        return;
    }
    
    float avgError = 0.0f;
    for (float error : pImpl->errorHistory) {
        avgError += error;
    }
    avgError /= pImpl->errorHistory.size();
    
    // Confidence is inversely related to error
    if (avgError < 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
    } else if (avgError > 0.5f) {
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.1f);
    } else {
        // Moderate error - maintain baseline confidence
        pImpl->confidence = 0.5f;
    }
}

// Update the prediction system in Brain.cpp to actually use the prediction system
// This will be done by modifying the Brain::step() method

} // namespace nlm
