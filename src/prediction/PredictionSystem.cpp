#include "src/prediction/PredictionSystem.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    std::vector<float> recentSensoryState;
    std::vector<float> predictedNextState;
    float predictionError;
    float confidence;
    size_t historySize;
    
    // Simple forward model: recent state → predicted next state
    std::vector<float> forwardWeights;
    std::vector<float> bias;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), 
             historySize(10), forwardWeights(50, 0.0f), bias(50, 0.0f) {
        // Initialize weights randomly
        for (size_t i = 0; i < forwardWeights.size(); ++i) {
            forwardWeights[i] = (static_cast<float>(rand()) / RAND_MAX) * 0.1f - 0.05f;
        }
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->recentSensoryState.clear();
    pImpl->predictedNextState.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    NLM_LOG_INFO("PredictionSystem initialized");
}

bool PredictionSystem::train(const std::vector<float>& currentState, 
                            const std::vector<float>& nextState) {
    if (!pImpl->brain || currentState.empty() || nextState.empty()) {
        return false;
    }
    
    // Simple forward model training
    // Predict next state based on current state
    pImpl->predictedNextState.resize(nextState.size(), 0.0f);
    
    for (size_t i = 0; i < nextState.size() && i < pImpl->forwardWeights.size(); ++i) {
        float prediction = 0.0f;
        for (size_t j = 0; j < currentState.size() && j < pImpl->forwardWeights.size(); ++j) {
            prediction += currentState[j] * pImpl->forwardWeights[i * currentState.size() + j];
        }
        prediction += pImpl->bias[i];
        pImpl->predictedNextState[i] = prediction;
    }
    
    // Calculate prediction error
    pImpl->predictionError = 0.0f;
    for (size_t i = 0; i < nextState.size() && i < pImpl->predictedNextState.size(); ++i) {
        float diff = nextState[i] - pImpl->predictedNextState[i];
        pImpl->predictionError += diff * diff;
    }
    pImpl->predictionError = std::sqrt(pImpl->predictionError / std::max(1.0f, (float)nextState.size()));
    
    // Update forward model weights (simple delta rule)
    float learningRate = 0.01f;
    for (size_t i = 0; i < nextState.size() && i < pImpl->forwardWeights.size(); ++i) {
        float error = nextState[i] - pImpl->predictedNextState[i];
        for (size_t j = 0; j < currentState.size() && j < pImpl->forwardWeights.size(); ++j) {
            pImpl->forwardWeights[i * currentState.size() + j] += learningRate * error * currentState[j];
        }
        pImpl->bias[i] += learningRate * error;
    }
    
    // Update confidence based on prediction error
    pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError * 5.0f);
    
    // Store current state for next prediction
    pImpl->recentSensoryState = currentState;
    
    NLM_LOG_INFO("PredictionSystem trained: error=" + std::to_string(pImpl->predictionError) +
                " confidence=" + std::to_string(pImpl->confidence));
    
    return true;
}

std::vector<float> PredictionSystem::predictNextState(const std::vector<float>& currentState) {
    if (currentState.empty()) {
        return std::vector<float>();
    }
    
    // Simple forward model prediction
    std::vector<float> prediction;
    prediction.resize(pImpl->bias.size(), 0.0f);
    
    for (size_t i = 0; i < pImpl->bias.size(); ++i) {
        float pred = 0.0f;
        for (size_t j = 0; j < currentState.size() && j < pImpl->forwardWeights.size(); ++j) {
            if (i < pImpl->forwardWeights.size() / currentState.size()) {
                pred += currentState[j] * pImpl->forwardWeights[i * currentState.size() + j];
            }
        }
        pred += pImpl->bias[i];
        prediction[i] = pred;
    }
    
    return prediction;
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

void PredictionSystem::reset() {
    pImpl->recentSensoryState.clear();
    pImpl->predictedNextState.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    NLM_LOG_INFO("PredictionSystem reset");
}

} // namespace nlm
