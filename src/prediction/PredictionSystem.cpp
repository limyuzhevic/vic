#include "PredictionSystem.hpp"

namespace nlm {

// Constants for prediction system parameters
namespace {
    constexpr float DEFAULT_LEARNING_RATE = 0.01f;        // Learning rate for prediction updates
    constexpr float DEFAULT_PREDICTION_HORIZON = 1.0f;   // Time horizon for predictions (seconds)
    constexpr float DEFAULT_PRECISION_WEIGHT = 1.0f;     // Weight for prediction precision
    constexpr float DEFAULT_CONFIDENCE = 0.5f;           // Default prediction confidence
    constexpr float MIN_PREDICTION_HORIZON = 0.1f;       // Minimum reasonable horizon
    constexpr float MAX_PREDICTION_HORIZON = 10.0f;      // Maximum reasonable horizon
    constexpr float MIN_LEARNING_RATE = 0.001f;          // Minimum learning rate
    constexpr float MAX_LEARNING_RATE = 1.0f;            // Maximum learning rate
    constexpr float MIN_PRECISION_WEIGHT = 0.1f;         // Minimum precision weight
    constexpr float MAX_PRECISION_WEIGHT = 10.0f;         // Maximum precision weight
    constexpr float DEFAULT_HISTORY_LIMIT = 1000;        // Maximum error history entries
}

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(DEFAULT_CONFIDENCE) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), learningRate_(DEFAULT_LEARNING_RATE),
                                       predictionHorizon_(DEFAULT_PREDICTION_HORIZON),
                                       precisionWeight_(DEFAULT_PRECISION_WEIGHT) {}

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
        
        // Keep history size manageable
        if (pImpl->errorHistory.size() > DEFAULT_HISTORY_LIMIT) {
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

// Configuration methods implementation
void PredictionSystem::setLearningRate(float rate) {
    learningRate_ = (rate < MIN_LEARNING_RATE) ? MIN_LEARNING_RATE : 
                   (rate > MAX_LEARNING_RATE) ? MAX_LEARNING_RATE : rate;
}

void PredictionSystem::setPredictionHorizon(float horizon) {
    predictionHorizon_ = (horizon < MIN_PREDICTION_HORIZON) ? MIN_PREDICTION_HORIZON :
                        (horizon > MAX_PREDICTION_HORIZON) ? MAX_PREDICTION_HORIZON : horizon;
}

void PredictionSystem::setPrecisionWeight(float weight) {
    precisionWeight_ = (weight < MIN_PRECISION_WEIGHT) ? MIN_PRECISION_WEIGHT :
                      (weight > MAX_PRECISION_WEIGHT) ? MAX_PRECISION_WEIGHT : weight;
}

// Statistics methods implementation
float PredictionSystem::getMeanPredictionError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    float sum = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f);
    return sum / pImpl->errorHistory.size();
}

float PredictionSystem::getMaxPredictionError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    return *std::max_element(pImpl->errorHistory.begin(), pImpl->errorHistory.end());
}

float PredictionSystem::getMinPredictionError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    return *std::min_element(pImpl->errorHistory.begin(), pImpl->errorHistory.end());
}

// Check if prediction error is significant
bool PredictionSystem::isSignificantPredictionError() const {
    return pImpl->predictionError > 0.5f; // Threshold for significant error
}

// Reset prediction system
void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = DEFAULT_CONFIDENCE;
    pImpl->errorHistory.clear();
    // Reset parameters to defaults
    learningRate_ = DEFAULT_LEARNING_RATE;
    predictionHorizon_ = DEFAULT_PREDICTION_HORIZON;
    precisionWeight_ = DEFAULT_PRECISION_WEIGHT;
}

} // namespace nlm
