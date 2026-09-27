#include "PredictiveCoding.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

struct PredictiveCoding::Impl {
    class Brain* brain;
    float predictionGain;
    float learningRate;
    float errorThreshold;
    size_t hierarchyLevels;
    
    // Current prediction and error
    std::vector<float> currentPredictions;
    float currentError;
    
    // Error statistics
    float mse;
    float maxError;
    float predictionAccuracy;
    
    // Hierarchical processing
    size_t activeHierarchyLevel;
    std::vector<bool> levelEnabled;
    std::vector<float> levelErrors;
    std::vector<float> levelPredictions;
    
    // Learning and history
    bool enabled;
    size_t trainingSamplesProcessed;
    size_t convergenceCount;
    
    // History tracking
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionHistory;
    
    // Forward and backward models
    struct LevelModel {
        std::vector<float> weights;           // Model weights
        std::vector<float> biases;           // Model biases
        std::vector<float> predictionBuffer; // Recent predictions
        float learningRate;                  // Level-specific learning rate
        
        LevelModel() : learningRate(0.01f) {}
    };
    
    std::vector<LevelModel> forwardModels;
    std::vector<LevelModel> backwardModels;
    
    // Temporal dynamics
    std::vector<float> predictionErrorBuffer;
    std::vector<float> stateBuffer;
    
    Impl() : brain(nullptr), predictionGain(0.1f), learningRate(0.01f),
             errorThreshold(0.001f), hierarchyLevels(3), currentError(0.0f),
             mse(0.0f), maxError(0.0f), predictionAccuracy(0.0f),
             activeHierarchyLevel(0), enabled(true), trainingSamplesProcessed(0),
             convergenceCount(0) {
        levelEnabled.resize(hierarchyLevels, true);
        levelErrors.resize(hierarchyLevels, 0.0f);
        levelPredictions.resize(hierarchyLevels, 0.0f);
        predictionErrorBuffer.resize(hierarchyLevels, 0.0f);
        stateBuffer.resize(hierarchyLevels, 0.0f);
    }
};

PredictiveCoding::PredictiveCoding() : pImpl(std::make_unique<Impl>()) {}

PredictiveCoding::~PredictiveCoding() = default;

void PredictiveCoding::initialize(class Brain* brain, float predictionGain, 
                                 size_t hierarchyLevels, float learningRate) {
    pImpl->brain = brain;
    pImpl->predictionGain = predictionGain;
    pImpl->hierarchyLevels = hierarchyLevels;
    pImpl->learningRate = learningRate;
    pImpl->errorThreshold = 0.001f;
    
    // Initialize forward and backward models for each hierarchy level
    pImpl->forwardModels.resize(hierarchyLevels);
    pImpl->backwardModels.resize(hierarchyLevels);
    
    for (size_t i = 0; i < hierarchyLevels; ++i) {
        pImpl->forwardModels[i].learningRate = learningRate / (i + 1);
        pImpl->backwardModels[i].learningRate = learningRate * (i + 1);
    }
    
    // Initialize history buffers
    pImpl->errorHistory.reserve(1000);
    pImpl->predictionHistory.reserve(1000);
    
    NLM_LOG_INFO("Predictive coding system initialized");
    NLM_LOG_INFO("Hierarchy levels: " + std::to_string(hierarchyLevels));
    NLM_LOG_INFO("Prediction gain: " + std::to_string(predictionGain));
}

void PredictiveCoding::setSensoryInput(const std::vector<float>& input) {
    pImpl->currentPredictions = input;
    
    // Initialize prediction buffers
    for (size_t i = 0; i < pImpl->hierarchyLevels; ++i) {
        if (i < input.size()) {
            pImpl->levelPredictions[i] = input[i];
        } else {
            pImpl->levelPredictions[i] = 0.0f;
        }
    }
}

float PredictiveCoding::getPredictionError() const {
    return pImpl->currentError;
}

std::vector<float> PredictiveCoding::getCurrentPredictions() const {
    return pImpl->currentPredictions;
}

void PredictiveCoding::computePredictions(const std::vector<float>& currentState) {
    if (!pImpl->enabled) {
        pImpl->currentError = 0.0f;
        return;
    }
    
    // Reset error calculations
    float totalMSE = 0.0f;
    float totalMaxError = 0.0f;
    pImpl->predictionAccuracy = 0.0f;
    
    // Compute predictions at each hierarchy level
    pImpl->currentPredictions.clear();
    pImpl->levelErrors.assign(pImpl->hierarchyLevels, 0.0f);
    
    for (size_t level = 0; level < pImpl->hierarchyLevels; ++level) {
        if (!pImpl->levelEnabled[level]) {
            continue;
        }
        
        // Get prediction from forward model
        std::vector<float> prediction = predictInput(currentState);
        
        // Store prediction for this level
        pImpl->levelPredictions[level] = prediction.empty() ? 0.0f : prediction[0];
        
        // Compute error for this level
        float levelError = computeLevelError(prediction, currentState);
        pImpl->levelErrors[level] = levelError;
        
        // Accumulate error statistics
        totalMSE += levelError * levelError;
        totalMaxError = std::max(totalMaxError, std::abs(levelError));
        
        // Update current predictions with top-level prediction
        if (level == pImpl->hierarchyLevels - 1) {
            pImpl->currentPredictions = prediction;
        }
    }
    
    // Calculate final error metrics
    pImpl->mse = totalMSE / pImpl->hierarchyLevels;
    pImpl->maxError = totalMaxError;
    
    // Calculate prediction accuracy (inverse of normalized error)
    float maxPossibleError = 1.0f;
    pImpl->predictionAccuracy = std::max(0.0f, 1.0f - (pImpl->mse / maxPossibleError));
    
    // Store overall prediction error
    pImpl->currentError = pImpl->mse;
    
    // Add to history
    pImpl->errorHistory.push_back(pImpl->currentError);
    if (pImpl->predictionHistory.size() < 1000) {
        pImpl->predictionHistory.push_back(pImpl->currentPredictions);
    }
    
    // Update training statistics
    pImpl->trainingSamplesProcessed++;
}

void PredictiveCoding::updateState(const std::vector<float>& predictionError) {
    if (!pImpl->enabled || predictionError.empty()) {
        return;
    }
    
    // Update state at each hierarchy level based on prediction error
    for (size_t level = 0; level < pImpl->hierarchyLevels; ++level) {
        if (!pImpl->levelEnabled[level]) {
            continue;
        }
        
        // Get current state
        std::vector<float> currentState = getCurrentStateAtLevel(level);
        
        // Update state using backward model
        std::vector<float> updatedState = updateState(predictionError, currentState);
        
        // Apply learning to backward model
        trainBackwardModel(predictionError, updatedState);
    }
}

float PredictiveCoding::getMeanSquaredError() const {
    return pImpl->mse;
}

float PredictiveCoding::getMaxError() const {
    return pImpl->maxError;
}

float PredictiveCoding::getPredictionAccuracy() const {
    return pImpl->predictionAccuracy;
}

size_t PredictiveCoding::getActiveHierarchyLevel() const {
    return pImpl->activeHierarchyLevel;
}

std::vector<float> PredictiveCoding::getLevelErrors() const {
    return pImpl->levelErrors;
}

std::vector<float> PredictiveCoding::getLevelPredictions() const {
    return pImpl->levelPredictions;
}

const std::vector<float>& PredictiveCoding::getErrorHistory() const {
    return pImpl->errorHistory;
}

const std::vector<std::vector<float>>& PredictiveCoding::getPredictionHistory() const {
    return pImpl->predictionHistory;
}

void PredictiveCoding::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->trainingSamplesProcessed = 0;
    pImpl->convergenceCount = 0;
}

void PredictiveCoding::enable(bool enable) {
    pImpl->enabled = enable;
}

bool PredictiveCoding::isEnabled() const {
    return pImpl->enabled;
}

void PredictiveCoding::setPredictionGain(float gain) {
    pImpl->predictionGain = gain;
}

void PredictiveCoding::setLearningRate(float rate) {
    pImpl->learningRate = rate;
    
    for (auto& model : pImpl->forwardModels) {
        model.learningRate = rate;
    }
    for (auto& model : pImpl->backwardModels) {
        model.learningRate = rate;
    }
}

void PredictiveCoding::setHierarchyLevels(size_t levels) {
    pImpl->hierarchyLevels = levels;
    
    // Resize model vectors
    pImpl->forwardModels.resize(levels);
    pImpl->backwardModels.resize(levels);
    pImpl->levelEnabled.resize(levels, true);
    pImpl->levelErrors.resize(levels, 0.0f);
    pImpl->levelPredictions.resize(levels, 0.0f);
    pImpl->predictionErrorBuffer.resize(levels, 0.0f);
    pImpl->stateBuffer.resize(levels, 0.0f);
}

void PredictiveCoding::setErrorThreshold(float threshold) {
    pImpl->errorThreshold = threshold;
}

void PredictiveCoding::setActiveLevel(size_t level) {
    if (level < pImpl->hierarchyLevels) {
        pImpl->activeHierarchyLevel = level;
    }
}

void PredictiveCoding::enableLevel(size_t level, bool enable) {
    if (level < pImpl->hierarchyLevels) {
        pImpl->levelEnabled[level] = enable;
    }
}

bool PredictiveCoding::isLevelEnabled(size_t level) const {
    if (level < pImpl->hierarchyLevels) {
        return pImpl->levelEnabled[level];
    }
    return false;
}

void PredictiveCoding::trainForwardModel(const std::vector<float>& state, 
                                       const std::vector<float>& predictedInput) {
    // Simplified forward model training
    // In a real implementation, this would involve neural network training
    
    size_t level = pImpl->activeHierarchyLevel;
    if (level >= pImpl->forwardModels.size()) {
        return;
    }
    
    // Update model weights based on prediction error
    float error = computePredictionError(state, predictedInput);
    
    // Simple weight update rule (placeholder for real neural network training)
    for (size_t i = 0; i < pImpl->forwardModels[level].weights.size() && i < state.size(); ++i) {
        float gradient = error * state[i];
        pImpl->forwardModels[level].weights[i] -= pImpl->learningRate * gradient;
    }
}

std::vector<float> PredictiveCoding::predictInput(const std::vector<float>& state) const {
    // Simplified prediction using dot product with weights
    size_t level = pImpl->activeHierarchyLevel;
    if (level >= pImpl->forwardModels.size()) {
        return std::vector<float>();
    }
    
    const auto& weights = pImpl->forwardModels[level].weights;
    
    // Compute prediction using weighted sum
    std::vector<float> prediction;
    float sum = 0.0f;
    
    for (size_t i = 0; i < weights.size() && i < state.size(); ++i) {
        sum += weights[i] * state[i];
    }
    
    // Add bias if available
    float bias = pImpl->forwardModels[level].biases.empty() ? 0.0f : 
                 pImpl->forwardModels[level].biases[0];
    
    prediction.push_back(sum + bias);
    
    return prediction;
}

void PredictiveCoding::trainBackwardModel(const std::vector<float>& predictionError,
                                         const std::vector<float>& state) {
    // Simplified backward model training
    // In a real implementation, this would involve error-based learning
    
    size_t level = pImpl->activeHierarchyLevel;
    if (level >= pImpl->backwardModels.size()) {
        return;
    }
    
    // Update backward model weights based on prediction error
    for (size_t i = 0; i < pImpl->backwardModels[level].weights.size() && 
         i < predictionError.size(); ++i) {
        
        float gradient = predictionError[i] * state[i];
        pImpl->backwardModels[level].weights[i] += pImpl->learningRate * gradient;
    }
}

std::vector<float> PredictiveCoding::updateState(const std::vector<float>& predictionError,
                                                 const std::vector<float>& previousState) const {
    // Simplified state update using prediction error
    std::vector<float> updatedState = previousState;
    
    // Update state based on prediction error
    for (size_t i = 0; i < updatedState.size() && i < predictionError.size(); ++i) {
        // Integrate prediction error into state
        updatedState[i] += pImpl->predictionGain * predictionError[i];
    }
    
    return updatedState;
}

float PredictiveCoding::getLearningProgress() const {
    if (pImpl->errorHistory.size() < 2) {
        return 0.0f;
    }
    
    // Compute improvement over time
    float firstError = pImpl->errorHistory.front();
    float lastError = pImpl->errorHistory.back();
    
    return std::max(0.0f, 1.0f - (lastError / (firstError + 0.001f)));
}

size_t PredictiveCoding::getTrainingSamplesProcessed() const {
    return pImpl->trainingSamplesProcessed;
}

float PredictiveCoding::computeLevelError(const std::vector<float>& prediction, 
                                         const std::vector<float>& target) const {
    if (prediction.empty() || target.empty()) {
        return 0.0f;
    }
    
    float totalError = 0.0f;
    size_t size = std::min(prediction.size(), target.size());
    
    for (size_t i = 0; i < size; ++i) {
        float diff = prediction[i] - target[i];
        totalError += diff * diff;
    }
    
    // Return root mean square error for this level
    return std::sqrt(totalError / static_cast<float>(size));
}

float PredictiveCoding::computePredictionError(const std::vector<float>& state,
                                               const std::vector<float>& prediction) const {
    if (state.empty() || prediction.empty()) {
        return 0.0f;
    }
    
    float totalError = 0.0f;
    size_t size = std::min(state.size(), prediction.size());
    
    for (size_t i = 0; i < size; ++i) {
        float diff = state[i] - prediction[i];
        totalError += diff * diff;
    }
    
    return std::sqrt(totalError / static_cast<float>(size));
}

std::vector<float> PredictiveCoding::getCurrentStateAtLevel(size_t level) const {
    // Return current state for a specific hierarchy level
    // This would typically come from brain internal state
    if (level >= pImpl->stateBuffer.size()) {
        return std::vector<float>();
    }
    
    return pImpl->stateBuffer;
}

} // namespace nlm