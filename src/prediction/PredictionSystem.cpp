#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionPatterns;
    std::vector<float> sensoryPredictions;
    std::vector<float> predictionErrors;
    std::unordered_map<size_t, float> actionPredictionWeights;
    float predictionDecayRate;
    float learningRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionDecayRate(0.95f), learningRate(0.01f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem: Brain not initialized");
        return currentState.clone();
    }
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    
    // Create prediction based on current state and neural dynamics
    std::vector<float> predictedData = currentData;
    
    // Apply prediction model using neural activity patterns
    if (!pImpl->predictionPatterns.empty() && pImpl->predictionPatterns.size() == currentData.size()) {
        for (size_t i = 0; i < currentData.size() && i < predictedData.size(); ++i) {
            // Blend current state with learned prediction patterns
            float patternWeight = pImpl->predictionPatterns[i] * 0.3f;
            float currentInfluence = currentData[i] * 0.7f;
            predictedData[i] = patternWeight + currentInfluence;
            
            // Apply non-linear transformation for realistic prediction
            predictedData[i] = 1.0f / (1.0f + std::exp(-predictedData[i]));
        }
    }
    
    // Apply motor action predictions if available
    // Predict sensory consequences of planned actions
    for (const auto& actionWeight : pImpl->actionPredictionWeights) {
        ActionType action = static_cast<ActionType>(actionWeight.first);
        float weight = actionWeight.second;
        
        // Simple forward model: actions cause predictable sensory changes
        for (size_t i = 0; i < predictedData.size(); ++i) {
            switch (action) {
                case ActionType::MoveForward:
                    if (i % 10 < 5) predictedData[i] = std::min(1.0f, predictedData[i] + weight * 0.1f);
                    break;
                case ActionType::MoveBackward:
                    if (i % 10 >= 5) predictedData[i] = std::max(0.0f, predictedData[i] - weight * 0.1f);
                    break;
                case ActionType::TurnLeft:
                case ActionType::TurnRight:
                    for (size_t j = 0; j < predictedData.size(); ++j) {
                        float angle = (j % 16) * (3.14159f / 8.0f);
                        if (action == ActionType::TurnLeft) angle = -angle;
                        predictedData[j] += weight * std::sin(angle) * 0.05f;
                    }
                    break;
                case ActionType::Interact:
                    for (size_t j = 0; j < predictedData.size(); ++j) {
                        predictedData[j] = 0.7f + weight * 0.3f * std::sin(j * 0.5f);
                    }
                    break;
                default:
                    break;
            }
        }
        
        // Clamp to valid range
        for (float& val : predictedData) {
            val = std::clamp(val, 0.0f, 1.0f);
        }
    }
    
    // Create SensoryInput with predicted data
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    pImpl->sensoryPredictions.push_back(predictedData);
    if (pImpl->sensoryPredictions.size() > 100) {
        pImpl->sensoryPredictions.erase(pImpl->sensoryPredictions.begin());
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Predicted next state with " + std::to_string(predictedData.size()) + " values");
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error
    float sumSquaredError = 0.0f;
    pImpl->predictionErrors.clear();
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        sumSquaredError += error * error;
        pImpl->predictionErrors.push_back(error);
    }
    
    pImpl->predictionError = sumSquaredError / predData.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction confidence based on error
    // Higher error means lower confidence
    float errorScale = std::min(1.0f, pImpl->predictionError * 10.0f);
    pImpl->confidence = 1.0f - errorScale * 0.8f;
    pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
    
    // Learn from prediction error (Hebbian-like learning)
    learnFromError(predData, actualData);
    
    // Decay old prediction patterns
    decayPredictionPatterns();
    
    NLM_LOG_DEBUG("PredictionSystem: Error = " + std::to_string(pImpl->predictionError) + ", Confidence = " + std::to_string(pImpl->confidence));
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
    pImpl->sensoryPredictions.clear();
    pImpl->predictionErrors.clear();
    pImpl->predictionPatterns.clear();
    pImpl->sensoryPredictions.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Learn predictive patterns from sensory input
    std::vector<float> pattern(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        // Learn temporal correlations (simple pattern matching)
        if (pImpl->sensoryPredictions.size() >= 2) {
            const auto& prevPattern = pImpl->sensoryPredictions[pImpl->sensoryPredictions.size() - 2];
            const auto& currentPattern = pImpl->sensoryPredictions.back();
            
            if (i < prevPattern.size() && i < currentPattern.size()) {
                pattern[i] = currentPattern[i] - prevPattern[i];
                pattern[i] = std::clamp(pattern[i], -1.0f, 1.0f);
            }
        }
        
        // Update prediction patterns with Hebbian learning
        pImpl->predictionPatterns[i] = pImpl->predictionPatterns[i] * 0.9f + 
                                        data[i] * pImpl->learningRate;
    }
    
    pImpl->predictionPatterns = pattern;
}

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

void PredictionSystem::decayPredictionPatterns() {
    for (float& pattern : pImpl->predictionPatterns) {
        pattern *= pImpl->predictionDecayRate;
    }
}

void PredictionSystem::learnFromError(const std::vector<float>& predicted, const std::vector<float>& actual) {
    if (!pImpl->brain) return;
    
    // Use prediction error to update action predictions
    float errorMagnitude = std::sqrt(pImpl->predictionError);
    
    // If prediction was good, reinforce successful action patterns
    if (errorMagnitude < 0.1f) {
        // Successful prediction - strengthen related action weights
        // This could be implemented with action-specific learning
        // For now, just boost overall confidence
    } else {
        // Poor prediction - reduce confidence and adjust learning
        pImpl->learningRate *= 0.95f;
    }
}

} // namespace nlm
