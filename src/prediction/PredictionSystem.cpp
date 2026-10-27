#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Predictive model weights (neural connections)
    std::vector<std::vector<float>> modelWeights;
    
    // Prediction error signals
    std::vector<float> predictionErrors;
    
    // Prediction confidence
    float confidence;
    
    // Prediction history
    std::vector<float> errorHistory;
    
    // Temporal prediction horizon
    size_t predictionHorizon;
    
    // Hebbian learning traces
    std::vector<float> eligibilityTraces;
    
    Impl() : confidence(0.5f), predictionHorizon(5), predictionError(0.0f) {
        modelWeights.reserve(100);
        predictionErrors.reserve(50);
        errorHistory.reserve(1000);
        eligibilityTraces.reserve(100);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionSystem initialized with neural predictive coding");
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    // Get current sensory pattern
    const auto& currentData = currentState.getData();
    
    // Use working memory to generate prediction
    auto* workingMem = pImpl->brain->getWorkingMemory();
    if (!workingMem) {
        return currentState.clone();
    }
    
    // Generate prediction based on learned patterns
    std::vector<float> predictedPattern;
    
    // Get current neural activity patterns from working memory
    std::vector<float> currentNeuralState = workingMem->retrieve();
    
    if (!currentNeuralState.empty()) {
        // Use neural patterns to predict next state
        predictedPattern = currentNeuralState;
        
        // Apply predictive dynamics
        for (size_t i = 0; i < predictedPattern.size() && i < currentData.size(); ++i) {
            // Predictive dynamics: current state influences next state
            float temporalDynamics = 1.0f - std::exp(-0.1f * static_cast<float>(pImpl->predictionHorizon));
            predictedPattern[i] = currentData[i] * temporalDynamics + 
                                predictedPattern[i] * (1.0f - temporalDynamics);
            
            // Add noise for exploration
            float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
            predictedPattern[i] += noise;
        }
    } else {
        // Fallback: smooth version of current state
        predictedPattern = currentData;
        for (float& val : predictedPattern) {
            val *= 0.9f;  // Damping
            val += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;  // Noise
        }
    }
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>(predictedPattern);
    
    // Update prediction error
    float error = computePredictionError(currentData, predictedPattern);
    pImpl->predictionErrors.push_back(error);
    
    // Update confidence based on prediction accuracy
    pImpl->confidence = calculateConfidence(currentData, predictedPattern);
    
    NLM_LOG_INFO("Prediction generated: error=" + std::to_string(error) + 
                 ", confidence=" + std::to_string(pImpl->confidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                         const SensoryInput& actual) {
    // Update prediction model based on actual observation
    
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.size() != actualData.size() || predictedData.empty()) {
        return;
    }
    
    // Compute prediction error
    float error = computePredictionError(actualData, predictedData);
    pImpl->predictionErrors.push_back(error);
    
    // Update model weights (Hebbian-like learning)
    updateModelWeights(predictedData, actualData, error);
    
    // Update prediction horizon based on error
    if (error > 0.5f) {
        // Large error: increase prediction horizon for more flexibility
        pImpl->predictionHorizon = std::min(pImpl->predictionHorizon + 1, 20);
    } else if (error < 0.1f) {
        // Small error: decrease prediction horizon for more stability
        pImpl->predictionHorizon = std::max(pImpl->predictionHorizon - 1, 3);
    }
    
    // Update eligibility traces for reinforcement learning
    for (auto& trace : pImpl->eligibilityTraces) {
        trace *= 0.95f;  // Decay traces
    }
    
    // Store error in history
    pImpl->errorHistory.push_back(error);
    
    // Keep history manageable
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    NLM_LOG_INFO("Predictions updated: error=" + std::to_string(error) + 
                 ", horizon=" + std::to_string(pImpl->predictionHorizon));
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->predictionErrors.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (float error : pImpl->predictionErrors) {
        sum += error;
    }
    return sum / pImpl->predictionErrors.size();
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->predictionErrors.clear();
    pImpl->errorHistory.clear();
    pImpl->confidence = 0.5f;
    NLM_LOG_INFO("Prediction system history cleared");
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    const auto& obsData = observation.getData();
    
    // Get episodic memory for training data
    auto* episodicMem = pImpl->brain->getEpisodicMemory();
    if (!episodicMem) {
        return;
    }
    
    // Get recent episodes for training
    auto recentEpisodes = episodicMem->getRecentEpisodes(10);
    
    // Train prediction model on episode sequences
    for (const auto* episode : recentEpisodes) {
        if (!episode->sensoryState.empty() && !episode->resultingSensoryState.empty()) {
            // Train on state transitions
            trainOnSequence(episode->sensoryState, episode->resultingSensoryState);
            
            // Train on action outcomes if available
            if (episode->action != ActionType::Wait) {
                std::vector<float> actionPattern(obsData.size(), 0.0f);
                actionPattern[0] = static_cast<float>(episode->action) / 10.0f;
                trainOnSequence(actionPattern, episode->resultingSensoryState);
            }
        }
    }
    
    // Update model weights with new observation
    if (!obsData.empty()) {
        // Find most similar past pattern
        size_t bestMatch = findBestPatternMatch(obsData);
        if (bestMatch < modelWeights.size()) {
            // Update weights based on new observation
            updateModelWithObservation(obsData, bestMatch);
        }
    }
    
    NLM_LOG_INFO("Prediction system trained on " + std::to_string(recentEpisodes.size()) + 
                 " episodes, current confidence=" + std::to_string(pImpl->confidence));
}

void PredictionSystem::Impl::updateModelWeights(const std::vector<float>& predicted,
                                               const std::vector<float>& actual,
                                               float error) {
    if (predicted.size() != actual.size()) return;
    
    // Ensure we have space for weights
    while (modelWeights.size() < predicted.size()) {
        modelWeights.push_back(std::vector<float>(predicted.size(), 0.0f));
    }
    
    // Hebbian-like weight update
    for (size_t i = 0; i < predicted.size() && i < modelWeights.size(); ++i) {
        // Error-driven weight adjustment
        float delta = error * (actual[i] - predicted[i]);
        
        // Update with eligibility trace
        if (i < eligibilityTraces.size()) {
            delta *= eligibilityTraces[i];
        }
        
        // Apply weight change
        modelWeights[0][i] += delta * 0.01f;
        
        // Clamp weights
        modelWeights[0][i] = std::clamp(modelWeights[0][i], -1.0f, 1.0f);
    }
}

void PredictionSystem::trainOnSequence(const std::vector<float>& fromState,
                                      const std::vector<float>& toState) {
    if (fromState.size() != toState.size()) return;
    
    // Expand model capacity if needed
    while (modelWeights.size() < fromState.size()) {
        modelWeights.push_back(std::vector<float>(fromState.size(), 0.0f));
    }
    
    // Train on state transition
    for (size_t i = 0; i < fromState.size() && i < modelWeights.size(); ++i) {
        // Weight represents connection strength from fromState[i] to toState[i]
        float weightChange = (toState[i] - fromState[i]) * fromState[i] * 0.01f;
        modelWeights[0][i] += weightChange;
        
        // Clamp weights
        modelWeights[0][i] = std::clamp(modelWeights[0][i], -1.0f, 1.0f);
    }
}

void PredictionSystem::updateModelWithObservation(const std::vector<float>& observation,
                                                 size_t patternIndex) {
    if (patternIndex >= modelWeights.size()) return;
    
    // Update existing pattern with new observation
    std::vector<float>& pattern = modelWeights[patternIndex];
    
    for (size_t i = 0; i < observation.size() && i < pattern.size(); ++i) {
        // Soft update with new observation
        pattern[i] = pattern[i] * 0.9f + observation[i] * 0.1f;
    }
}

size_t PredictionSystem::findBestPatternMatch(const std::vector<float>& query) const {
    if (modelWeights.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestMatch = 0;
    
    // Find most similar stored pattern
    for (size_t i = 0; i < modelWeights.size(); ++i) {
        float similarity = computePatternSimilarity(query, modelWeights[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = i;
        }
    }
    
    return bestMatch;
}

float PredictionSystem::computePredictionError(const std::vector<float>& actual,
                                              const std::vector<float>& predicted) const {
    if (actual.size() != predicted.size() || actual.empty()) {
        return 0.0f;
    }
    
    // Mean squared error
    float sum = 0.0f;
    for (size_t i = 0; i < actual.size(); ++i) {
        float diff = actual[i] - predicted[i];
        sum += diff * diff;
    }
    
    return std::sqrt(sum / actual.size());
}

float PredictionSystem::calculateConfidence(const std::vector<float>& actual,
                                           const std::vector<float>& predicted) const {
    float error = computePredictionError(actual, predicted);
    
    // Confidence inversely related to error
    float confidence = 1.0f - std::min(1.0f, error / 2.0f);
    confidence = std::max(0.1f, confidence); // Minimum confidence
    
    return confidence;
}

float PredictionSystem::computePatternSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
