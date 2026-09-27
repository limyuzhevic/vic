#include "PredictionError.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct PredictionError::Impl {
    // Brain reference for neural integration
    class Brain* brain;
    
    // Prediction error state
    float error;                    // Current prediction error
    float predictedValue;           // Predicted value
    float actualValue;              // Actual observed value
    float magnitude;                // Absolute error magnitude
    
    // Temporal processing
    float temporalDifference;       // TD error for continuous learning
    float discountFactor;           // Temporal discount factor
    float predictionHorizon;        // How far ahead we predict
    
    // Learning components
    float learningRate;             // Learning rate for prediction updates
    float errorSensitivity;         // Sensitivity to prediction errors
    float confidence;               // Confidence in prediction
    
    // Neural dynamics
    float predictionPrecision;      // Precision of prediction (inverse variance)
    float baselinePrediction;       // Baseline prediction level
    float rewardModulation;         // Reward modulation of prediction
    
    // Statistics and history
    std::vector<float> history;
    std::vector<float> confidenceHistory;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    // Prediction state flags
    bool isInitialized;             // Whether system has been initialized
    bool isUpdating;                // Whether currently updating predictions
    
    Impl()
        : brain(nullptr)
        , error(0.0f)
        , predictedValue(0.0f)
        , actualValue(0.0f)
        , magnitude(0.0f)
        , temporalDifference(0.0f)
        , discountFactor(0.95f)
        , predictionHorizon(1.0f)
        , learningRate(0.1f)
        , errorSensitivity(1.0f)
        , confidence(0.5f)
        , predictionPrecision(1.0f)
        , baselinePrediction(0.0f)
        , rewardModulation(1.0f)
        , isInitialized(false)
        , isUpdating(false) {
        
        // Seed RNG with chrono for biological variability
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.9f, 1.1f);
    }
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->isInitialized = true;
    NLM_LOG_INFO("PredictionError system initialized with neural dynamics");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Real prediction error signaling: biological implementation
    
    // Store previous values
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Compute basic error
    pImpl->error = actual - predicted;
    pImpl->magnitude = std::abs(pImpl->error);
    
    // Apply temporal difference formulation
    pImpl->temporalDifference = pImpl->error;
    
    // Update prediction confidence based on error magnitude
    // High errors reduce confidence
    float confidenceUpdate = -std::abs(pImpl->error) * pImpl->errorSensitivity * 0.1f;
    pImpl->confidence = std::max(0.0f, std::min(1.0f, pImpl->confidence + confidenceUpdate));
    
    // Apply reward modulation
    float modulatedError = pImpl->error * pImpl->rewardModulation;
    pImpl->temporalDifference = modulatedError;
    
    // Store in history
    pImpl->history.push_back(pImpl->error);
    pImpl->confidenceHistory.push_back(pImpl->confidence);
    
    // Trim history
    const size_t maxHistory = 1000;
    if (pImpl->history.size() > maxHistory) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->confidenceHistory.erase(pImpl->confidenceHistory.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction with temporal discounting
    float oldPrediction = pImpl->predictedValue;
    
    // Apply temporal discounting to prediction
    float predictionUpdate = (newPrediction - oldPrediction) * pImpl->learningRate;
    pImpl->predictedValue += predictionUpdate;
    
    // Update temporal difference
    pImpl->temporalDifference = pImpl->actualValue - pImpl->predictedValue;
    
    // Adjust prediction precision based on error
    float precisionUpdate = pImpl->magnitude * 0.05f;
    pImpl->predictionPrecision = std::max(0.1f, pImpl->predictionPrecision - precisionUpdate);
    
    // Update confidence
    float confidenceUpdate = precisionUpdate * 0.5f;
    pImpl->confidence = std::min(1.0f, pImpl->confidence + confidenceUpdate);
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->confidenceHistory.clear();
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

float PredictionError::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float PredictionError::getPredictionConfidence() const {
    return pImpl->confidence;
}

void PredictionError::setPredictionHorizon(float horizon) {
    pImpl->predictionHorizon = std::max(0.1f, horizon);
}

void PredictionError::setErrorSensitivity(float sensitivity) {
    pImpl->errorSensitivity = std::clamp(sensitivity, 0.0f, 2.0f);
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.01f, 1.0f);
}

} // namespace nlm
