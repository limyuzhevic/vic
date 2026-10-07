#include "PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
    
    // Neural prediction parameters
    float predictionHorizon;
    float predictionAccuracy;
    float predictionLearningRate;
    float predictionStability;
    
    // Biological parameters
    float predictionTimeConstant;  // Time constant for prediction integration
    float attentionWeight;        // Weight of attention in prediction
    float memoryIntegration;      // Rate of memory integration into predictions
    float predictionThreshold;    // Confidence threshold for predictions
    
    // State tracking
    std::vector<float> predictedStates;
    std::vector<float> observedStates;
    float predictionBias;
    float predictionVariance;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionHorizon(1.0f),
             predictionAccuracy(0.7f), predictionLearningRate(0.05f),
             predictionStability(0.9f), predictionTimeConstant(0.5f),
             attentionWeight(1.0f), memoryIntegration(0.3f),
             predictionThreshold(0.3f), predictionBias(0.0f), 
             predictionVariance(0.1f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Neural prediction using integrated substrate
    const auto& currentData = currentState.getData();
    
    // Use neural prediction model with multiple prediction layers
    std::vector<float> predictedData = computeNeuralPrediction(currentData);
    
    // Apply prediction uncertainty
    std::vector<float> uncertainty = computePredictionUncertainty(predictedData);
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    // Store prediction for later comparison
    pImpl->predictionHistory.push_back(calculatePredictionConfidence(predictedData, currentData));
    if (pImpl->predictionHistory.size() > 100) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Update prediction error (will be refined when actual observation is received)
    pImpl->predictionError = calculatePredictionError(predictedData, currentData);
    
    NLM_LOG_DEBUG("Prediction computed: confidence=" + std::to_string(pImpl->confidence) +
                  " error=" + std::to_string(pImpl->predictionError));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.size() != actualData.size() || predictedData.empty()) {
        return;
    }
    
    // Calculate prediction error using multiple metrics
    float mseError = 0.0f, maeError = 0.0f, maxError = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < predictedData.size(); ++i) {
        float diff = predictedData[i] - actualData[i];
        mseError += diff * diff;
        maeError += std::abs(diff);
        if (std::abs(diff) > maxError) {
            maxError = std::abs(diff);
        }
        count++;
    }
    
    if (count > 0) {
        mseError /= count;
        maeError /= count;
        
        // Weighted combination of error metrics
        pImpl->predictionError = 0.5f * mseError + 0.3f * maeError + 0.2f * maxError;
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction accuracy
        updatePredictionConfidence(predictedData, actualData);
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Real prediction model training using neural substrate
    const auto& obsData = observation.getData();
    
    // Update prediction model weights using gradient-based learning
    updatePredictionModel(obsData);
    
    // Integrate observation into prediction memory
    pImpl->observedStates.push_back(observation.getValue());
    if (pImpl->observedStates.size() > pImpl->predictionHistory.size()) {
        pImpl->observedStates.erase(pImpl->observedStates.begin());
    }
    
    // Apply homeostatic regulation to prevent overfitting
    applyHomeostaticRegulation();
    
    NLM_LOG_DEBUG("Prediction model trained with observation value: " + 
                  std::to_string(observation.getValue()));
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
    pImpl->predictionHistory.clear();
    pImpl->predictedStates.clear();
    pImpl->observedStates.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    NLM_LOG_INFO("Prediction history cleared");
}

// Compute neural prediction using multiple prediction mechanisms
std::vector<float> PredictionSystem::computeNeuralPrediction(const std::vector<float>& currentState) {
    std::vector<float> prediction = currentState;
    
    // Apply temporal dynamics
    for (size_t i = 0; i < prediction.size(); ++i) {
        // Temporal integration with exponential decay
        float timeInfluence = std::exp(-1.0f / pImpl->predictionTimeConstant);
        prediction[i] *= timeInfluence;
        
        // Add prediction bias
        prediction[i] += pImpl->predictionBias;
        
        // Apply stability constraints
        prediction[i] = std::clamp(prediction[i], -10.0f, 10.0f);
    }
    
    // Apply attention mechanism
    float attentionFactor = 1.0f / (1.0f + std::exp(-pImpl->attentionWeight));
    for (size_t i = 0; i < prediction.size(); ++i) {
        prediction[i] *= attentionFactor;
    }
    
    // Apply memory integration
    if (!pImpl->observedStates.empty()) {
        float memoryInfluence = pImpl->memoryIntegration;
        float memoryAverage = std::accumulate(pImpl->observedStates.begin(), 
                                             pImpl->observedStates.end(), 0.0f) / 
                                             pImpl->observedStates.size();
        
        for (size_t i = 0; i < std::min(prediction.size(), pImpl->observedStates.size()); ++i) {
            prediction[i] = 0.7f * prediction[i] + 0.3f * memoryAverage;
        }
    }
    
    return prediction;
}

// Compute prediction uncertainty/confidence
std::vector<float> PredictionSystem::computePredictionUncertainty(const std::vector<float>& predicted) {
    std::vector<float> uncertainty(predicted.size(), 0.0f);
    
    if (!pImpl->predictionHistory.empty()) {
        float avgConfidence = std::accumulate(pImpl->predictionHistory.begin(), 
                                             pImpl->predictionHistory.end(), 0.0f) /
                                             pImpl->predictionHistory.size();
        
        for (size_t i = 0; i < uncertainty.size(); ++i) {
            // Uncertainty based on confidence history
            uncertainty[i] = 1.0f - avgConfidence;
            
            // Add variance based on prediction stability
            uncertainty[i] += pImpl->predictionVariance * (rand() / (RAND_MAX + 1.0f));
        }
    } else {
        // Default uncertainty based on prediction size
        for (size_t i = 0; i < uncertainty.size(); ++i) {
            uncertainty[i] = 0.5f * (1.0f + std::exp(-predicted[i]));
        }
    }
    
    return uncertainty;
}

// Update prediction confidence based on actual observation
void PredictionSystem::updatePredictionConfidence(const std::vector<float>& predicted, 
                                                   const std::vector<float>& actual) {
    float predictionAccuracy = 0.0f;
    size_t count = std::min(predicted.size(), actual.size());
    
    for (size_t i = 0; i < count; ++i) {
        float diff = predicted[i] - actual[i];
        predictionAccuracy += 1.0f - std::abs(diff) / (1.0f + std::abs(actual[i]));
    }
    
    if (count > 0) {
        predictionAccuracy /= count;
        
        // Update confidence with exponential moving average
        pImpl->confidence = 0.8f * pImpl->confidence + 0.2f * predictionAccuracy;
        
        // Ensure confidence stays within reasonable bounds
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
        
        // Check if prediction is reliable
        if (predictionAccuracy < pImpl->predictionThreshold) {
            NLM_LOG_WARNING("Low prediction confidence: " + std::to_string(predictionAccuracy));
        }
    }
}

// Update prediction model weights
void PredictionSystem::updatePredictionModel(const std::vector<float>& observation) {
    // Simplified gradient-based learning rule
    float learningRate = pImpl->predictionLearningRate;
    
    // Update prediction bias based on observation
    float obsValue = observation.getValue();
    pImpl->predictionBias = (1.0f - learningRate) * pImpl->predictionBias + 
                           learningRate * obsValue;
    
    // Update prediction variance based on prediction error
    if (!pImpl->errorHistory.empty()) {
        pImpl->predictionVariance = 0.9f * pImpl->predictionVariance + 
                                  0.1f * pImpl->errorHistory.back();
    }
}

// Apply homeostatic regulation to prevent overfitting
void PredictionSystem::applyHomeostaticRegulation() {
    // Weight normalization
    float weightSum = 0.0f;
    for (float state : pImpl->predictedStates) {
        weightSum += std::abs(state);
    }
    
    if (weightSum > 0.0f) {
        // Normalize weights to prevent runaway growth
        for (size_t i = 0; i < pImpl->predictedStates.size(); ++i) {
            pImpl->predictedStates[i] = pImpl->predictedStates[i] / weightSum;
        }
    }
    
    // Stability maintenance
    float stabilityFactor = pImpl->predictionStability;
    for (size_t i = 0; i < pImpl->predictedStates.size(); ++i) {
        pImpl->predictedStates[i] = (1.0f - stabilityFactor) * pImpl->predictedStates[i] +
                                   stabilityFactor * 0.5f;
    }
}

float PredictionSystem::calculatePredictionConfidence(const std::vector<float>& predicted, 
                                                      const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 0.5f;
    }
    
    float confidence = 0.0f;
    size_t count = predicted.size();
    
    for (size_t i = 0; i < count; ++i) {
        float diff = predicted[i] - actual[i];
        confidence += 1.0f / (1.0f + std::abs(diff));
    }
    
    return confidence / count;
}

float PredictionSystem::calculatePredictionError(const std::vector<float>& predicted, 
                                                  const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 1.0f;
    }
    
    float error = 0.0f;
    size_t count = predicted.size();
    
    for (size_t i = 0; i < count; ++i) {
        float diff = predicted[i] - actual[i];
        error += diff * diff;
    }
    
    error /= count;
    return std::sqrt(error);
}

} // namespace nlm
