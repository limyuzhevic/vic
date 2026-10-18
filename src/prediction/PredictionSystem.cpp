#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Configuration
    float learningRate;
    float confidenceThreshold;
    
    // Prediction state
    std::vector<ActionType> predictedActions;
    float confidence;
    float predictionError;
    
    // Learning state
    float learningProgress;
    size_t experienceCount;
    
    // History tracking
    std::vector<std::vector<float>> stateHistory;
    std::vector<ActionType> actionHistory;
    std::vector<float> rewardHistory;
    
    // Temporal patterns
    std::vector<std::vector<float>> temporalPatterns;
    float temporalCoherence;
    
    // Forward model for action predictions
    std::unordered_map<size_t, std::vector<std::vector<float>>> actionEffects;
    std::unordered_map<size_t, float> actionValues;
    
    // Random generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : learningRate(0.01f), confidenceThreshold(0.7f),
             confidence(0.5f), predictionError(0.0f),
             learningProgress(0.0f), experienceCount(0),
             temporalCoherence(0.0f), rng(std::random_device{}()),
             uniformDist(0.0f, 1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionSystem initialized");
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::configure(float learningRate, float confidenceThreshold) {
    pImpl->learningRate = learningRate;
    pImpl->confidenceThreshold = confidenceThreshold;
    NLM_LOG_INFO("PredictionSystem configured: learning_rate=" + std::to_string(learningRate) +
                 " confidence_threshold=" + std::to_string(confidenceThreshold));
}

void PredictionSystem::train(const std::vector<float>& sensoryInput, ActionType action) {
    if (sensoryInput.empty()) return;
    
    // Add to experience history
    pImpl->experienceCount++;
    
    // Learn from sensory input and action
    learnForwardModel(sensoryInput, action, {});  // Can't compute next state without actual next state
    
    // Update temporal patterns
    updateTemporalPatterns(sensoryInput);
    
    // Compute action value
    float actionValue = computeActionValue(action, sensoryInput);
    
    // Store experience
    pImpl->stateHistory.push_back(sensoryInput);
    pImpl->actionHistory.push_back(action);
    pImpl->rewardHistory.push_back(0.0f); // Will be updated when reward is received
    
    // Update learning progress
    pImpl->learningProgress = std::min(1.0f, static_cast<float>(pImpl->experienceCount) / 1000.0f);
    
    NLM_LOG_INFO("PredictionSystem trained with sensory input of size " + std::to_string(sensoryInput.size()) +
                 " and action " + std::to_string(static_cast<int>(action)));
}

void PredictionSystem::updatePrediction(const std::vector<float>& predictedState,
                                       const std::vector<float>& actualState, float error) {
    if (predictedState.empty() || actualState.empty()) return;
    
    // Update prediction error
    updatePredictionError(error);
    
    // Compute prediction accuracy
    float accuracy = 1.0f - std::abs(error);
    
    // Update confidence based on prediction accuracy
    pImpl->confidence = std::max(0.0f, pImpl->confidence * 0.9f + accuracy * 0.1f);
    
    // Store in history
    pImpl->stateHistory.push_back(predictedState);
    
    NLM_LOG_INFO("PredictionSystem updated: accuracy=" + std::to_string(accuracy) +
                 " confidence=" + std::to_string(pImpl->confidence) +
                 " error=" + std::to_string(error));
}

std::vector<ActionType> PredictionSystem::predictBestActions(const std::vector<float>& currentState,
                                                            size_t maxSteps) {
    if (currentState.empty()) {
        return {};
    }
    
    // Use Monte Carlo search to find best action sequences
    std::vector<ActionType> bestSequence = monteCarloSearch(currentState, maxSteps);
    
    // Verify sequence is valid
    if (!bestSequence.empty() && bestSequence.size() <= maxSteps) {
        pImpl->predictedActions = bestSequence;
        
        // Compute confidence in this prediction
        pImpl->confidence = computePredictionConfidence(currentState, bestSequence[0]);
        
        // Update prediction error based on how well it matches history
        if (!pImpl->stateHistory.empty()) {
            float avgError = 0.0f;
            size_t count = 0;
            for (const auto& historicalState : pImpl->stateHistory) {
                if (historicalState.size() == currentState.size()) {
                    avgError += computePredictionError(historicalState, currentState);
                    count++;
                }
            }
            if (count > 0) {
                avgError /= count;
                updatePredictionError(avgError);
            }
        }
        
        NLM_LOG_INFO("PredictionSystem predicted " + std::to_string(bestSequence.size()) + 
                     " actions with confidence " + std::to_string(pImpl->confidence));
        
        return bestSequence;
    }
    
    // Fallback to simple sequence
    std::vector<ActionType> fallbackSequence;
    if (maxSteps > 0) {
        fallbackSequence = {ActionType::MoveForward, ActionType::Wait};
        if (maxSteps > 1) {
            fallbackSequence.push_back(ActionType::MoveForward);
        }
    }
    
    pImpl->predictedActions = fallbackSequence;
    pImpl->confidence = pImpl->confidenceThreshold * 0.5f;
    
    NLM_LOG_INFO("PredictionSystem using fallback sequence with confidence " + std::to_string(pImpl->confidence));
    
    return fallbackSequence;
}

std::vector<float> PredictionSystem::predictNextState(ActionType action,
                                                      const std::vector<float>& currentState) {
    if (currentState.empty()) {
        return {};
    }
    
    // Look for similar states in history
    auto similarStates = findSimilarStates(currentState);
    
    if (similarStates.empty()) {
        // No similar states found - return current state
        return currentState;
    }
    
    // Get action effects from similar states
    auto actionIt = pImpl->actionEffects.find(static_cast<size_t>(action));
    if (actionIt == pImpl->actionEffects.end()) {
        // No action effects found - return current state
        return currentState;
    }
    
    // Interpolate between patterns
    std::vector<float> predictedState = currentState;
    for (const auto& effect : actionIt->second) {
        // Apply effect with appropriate weight
        float weight = 0.5f;  // Default weight
        for (size_t i = 0; i < std::min(predictedState.size(), effect.size()); ++i) {
            predictedState[i] = predictedState[i] * (1.0f - weight) + effect[i] * weight;
        }
    }
    
    // Add some noise for exploration
    float noise = pImpl->uniformDist(pImpl->rng);
    if (noise < 0.1f) {  // 10% chance of adding noise
        for (auto& value : predictedState) {
            value += (pImpl->uniformDist(pImpl->rng) - 0.5f) * 0.1f;
            value = std::clamp(value, 0.0f, 1.0f);
        }
    }
    
    return predictedState;
}

float PredictionSystem::computeActionValue(ActionType action, const std::vector<float>& state) const {
    // Check if we have action values for this action
    auto it = pImpl->actionValues.find(static_cast<size_t>(action));
    if (it == pImpl->actionValues.end()) {
        // Default value based on action type
        switch (action) {
            case ActionType::MoveForward: return 0.8f;
            case ActionType::TurnLeft: return 0.6f;
            case ActionType::TurnRight: return 0.6f;
            case ActionType::MoveBackward: return -0.3f;
            case ActionType::Interact: return 0.9f;
            case ActionType::Wait: return 0.1f;
            default: return 0.0f;
        }
    }
    
    // Get base value
    float value = it->second;
    
    // Adjust based on state similarity to learned patterns
    float stateSimilarity = 0.0f;
    if (!pImpl->stateHistory.empty()) {
        for (const auto& historicalState : pImpl->stateHistory) {
            if (historicalState.size() == state.size()) {
                float similarity = computePatternSimilarity(historicalState, state);
                stateSimilarity = std::max(stateSimilarity, similarity);
            }
        }
    }
    
    // Boost value if state is similar to learned states
    value *= (1.0f + stateSimilarity * 0.5f);
    
    return value;
}

std::vector<float> PredictionSystem::learnTemporalPattern(const std::vector<float>& sequence) {
    if (sequence.empty()) {
        return {};
    }
    
    // Add to temporal patterns
    pImpl->temporalPatterns.push_back(sequence);
    
    // Update temporal coherence
    float coherence = 0.0f;
    if (pImpl->temporalPatterns.size() >= 2) {
        for (size_t i = 0; i < pImpl->temporalPatterns.size() - 1; ++i) {
            float sim = computePatternSimilarity(pImpl->temporalPatterns[i], 
                                               pImpl->temporalPatterns[i + 1]);
            coherence += sim;
        }
        coherence /= (pImpl->temporalPatterns.size() - 1);
    }
    
    pImpl->temporalCoherence = coherence;
    
    NLM_LOG_INFO("PredictionSystem learned temporal pattern with coherence " + std::to_string(coherence));
    
    return sequence;
}

void PredictionSystem::clearHistory() {
    pImpl->stateHistory.clear();
    pImpl->actionHistory.clear();
    pImpl->rewardHistory.clear();
    pImpl->temporalPatterns.clear();
    pImpl->predictedActions.clear();
    pImpl->confidence = 0.5f;
    pImpl->predictionError = 0.0f;
    pImpl->learningProgress = 0.0f;
    pImpl->temporalCoherence = 0.0f;
    pImpl->actionEffects.clear();
    pImpl->actionValues.clear();
    
    NLM_LOG_INFO("PredictionSystem history cleared");
}

void PredictionSystem::reset() {
    clearHistory();
    pImpl->experienceCount = 0;
    NLM_LOG_INFO("PredictionSystem reset");
}

// Private implementation methods

void PredictionSystem::addExperience(const std::vector<float>& state, ActionType action, float reward) {
    pImpl->stateHistory.push_back(state);
    pImpl->actionHistory.push_back(action);
    pImpl->rewardHistory.push_back(reward);
    pImpl->experienceCount++;
    
    // Update learning progress
    pImpl->learningProgress = std::min(1.0f, static_cast<float>(pImpl->experienceCount) / 1000.0f);
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted,
                                              const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;  // Maximum error
    }
    
    float sumError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float error = std::abs(predicted[i] - actual[i]);
        sumError += error;
    }
    
    return sumError / predicted.size();
}

float PredictionSystem::computePatternSimilarity(const std::vector<float>& pattern1,
                                                const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 < 0.0001f || norm2 < 0.0001f) {
        return 0.0f;
    }
    
    return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

std::vector<ActionType> PredictionSystem::extractActionSequence(const std::vector<float>& state) {
    std::vector<ActionType> sequence;
    
    // Simple heuristic: map state values to actions
    float avgValue = 0.0f;
    for (float value : state) {
        avgValue += value;
    }
    avgValue /= state.size();
    
    if (avgValue > 0.8f) {
        sequence.push_back(ActionType::MoveForward);
    } else if (avgValue > 0.6f) {
        sequence.push_back(ActionType::Wait);
    } else if (avgValue > 0.4f) {
        sequence.push_back(ActionType::TurnLeft);
    } else {
        sequence.push_back(ActionType::Wait);
    }
    
    return sequence;
}

float PredictionSystem::evaluateActionSequence(const std::vector<ActionType>& sequence,
                                              const std::vector<float>& initialState) const {
    float totalValue = 0.0f;
    std::vector<float> currentState = initialState;
    
    for (ActionType action : sequence) {
        float actionValue = computeActionValue(action, currentState);
        totalValue += actionValue;
        
        // Predict next state
        currentState = predictNextState(action, currentState);
    }
    
    return totalValue / sequence.size();
}

void PredictionSystem::updateTemporalPatterns(const std::vector<float>& currentState) {
    if (currentState.empty()) return;
    
    // Remove oldest patterns if we have too many
    if (pImpl->temporalPatterns.size() > 100) {
        pImpl->temporalPatterns.erase(pImpl->temporalPatterns.begin());
    }
    
    // Add current state as a new pattern
    pImpl->temporalPatterns.push_back(currentState);
    
    // Update coherence
    float coherence = 0.0f;
    if (pImpl->temporalPatterns.size() >= 2) {
        for (size_t i = 0; i < pImpl->temporalPatterns.size() - 1; ++i) {
            float sim = computePatternSimilarity(pImpl->temporalPatterns[i], 
                                               pImpl->temporalPatterns[i + 1]);
            coherence += sim;
        }
        coherence /= (pImpl->temporalPatterns.size() - 1);
    }
    
    pImpl->temporalCoherence = coherence;
}

std::vector<float> PredictionSystem::interpolatePattern(const std::vector<float>& pattern1,
                                                        const std::vector<float>& pattern2,
                                                        float weight) const {
    std::vector<float> result;
    
    size_t size = std::min(pattern1.size(), pattern2.size());
    result.reserve(size);
    
    for (size_t i = 0; i < size; ++i) {
        float value = pattern1[i] * (1.0f - weight) + pattern2[i] * weight;
        result.push_back(value);
    }
    
    return result;
}

void PredictionSystem::learnForwardModel(const std::vector<float>& state, ActionType action,
                                        const std::vector<float>& nextState) {
    size_t actionKey = static_cast<size_t>(action);
    
    if (!nextState.empty()) {
        // Store action-effect mapping
        pImpl->actionEffects[actionKey].push_back(nextState);
        
        // Update action value
        float currentValue = pImpl->actionValues[actionKey];
        if (!nextState.empty()) {
            float effectSize = 0.0f;
            for (float value : nextState) {
                effectSize += value;
            }
            effectSize /= nextState.size();
            pImpl->actionValues[actionKey] = currentValue * 0.9f + effectSize * 0.1f;
        }
    }
}

std::vector<ActionType> PredictionSystem::monteCarloSearch(const std::vector<float>& currentState,
                                                          size_t searchDepth) const {
    std::vector<ActionType> bestSequence;
    float bestValue = -std::numeric_limits<float>::max();
    
    if (currentState.empty()) {
        return bestSequence;
    }
    
    // Generate possible action sequences
    std::vector<ActionType> allActions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // Simple Monte Carlo: evaluate random sequences
    std::vector<ActionType> currentSeq;
    std::function<void(size_t)> search = [&](size_t depth) {
        if (depth >= searchDepth) {
            // Evaluate sequence
            float value = evaluateActionSequence(currentSeq, currentState);
            if (value > bestValue) {
                bestValue = value;
                bestSequence = currentSeq;
            }
            return;
        }
        
        for (ActionType action : allActions) {
            currentSeq.push_back(action);
            search(depth + 1);
            currentSeq.pop_back();
        }
    };
    
    search(0);
    
    return bestSequence;
}

float PredictionSystem::estimateActionValue(ActionType action, const std::vector<float>& state) const {
    // Get learned value if available
    auto it = pImpl->actionValues.find(static_cast<size_t>(action));
    if (it != pImpl->actionValues.end()) {
        return it->second;
    }
    
    // Compute value based on state similarity
    float maxSimilarity = 0.0f;
    float maxValue = 0.0f;
    
    for (const auto& historicalState : pImpl->stateHistory) {
        if (historicalState.size() == state.size()) {
            float similarity = computePatternSimilarity(historicalState, state);
            if (similarity > maxSimilarity) {
                maxSimilarity = similarity;
                
                // Find corresponding action value
                size_t actionIdx = static_cast<size_t>(action);
                if (actionIdx < pImpl->actionHistory.size()) {
                    ActionType histAction = pImpl->actionHistory[actionIdx];
                    auto valIt = pImpl->actionValues.find(static_cast<size_t>(histAction));
                    if (valIt != pImpl->actionValues.end()) {
                        maxValue = valIt->second;
                    }
                }
            }
        }
    }
    
    // Adjust value based on similarity
    return maxValue * (1.0f + maxSimilarity * 0.5f);
}

float PredictionSystem::computePredictionConfidence(const std::vector<float>& state,
                                                   ActionType action) const {
    if (state.empty()) {
        return 0.5f;
    }
    
    // Find similar states in history
    float maxSimilarity = 0.0f;
    for (const auto& historicalState : pImpl->stateHistory) {
        if (historicalState.size() == state.size()) {
            float similarity = computePatternSimilarity(historicalState, state);
            maxSimilarity = std::max(maxSimilarity, similarity);
        }
    }
    
    // Confidence based on similarity and action history
    float confidence = maxSimilarity;
    
    // Check if action is common in similar states
    if (maxSimilarity > 0.5f) {
        // Look at actions taken in similar states
        std::vector<ActionType> similarActions;
        for (size_t i = 0; i < pImpl->stateHistory.size(); ++i) {
            if (i < pImpl->actionHistory.size()) {
                if (computePatternSimilarity(pImpl->stateHistory[i], state) > 0.7f) {
                    similarActions.push_back(pImpl->actionHistory[i]);
                }
            }
        }
        
        if (!similarActions.empty()) {
            // Count occurrences of this action
            size_t actionCount = std::count(similarActions.begin(), 
                                          similarActions.end(), action);
            confidence *= (1.0f + static_cast<float>(actionCount) * 0.1f);
        }
    }
    
    // Clamp confidence to reasonable range
    confidence = std::clamp(confidence, 0.1f, 1.0f);
    
    return confidence;
}

void PredictionSystem::updatePredictionError(float newError) {
    // Blend with existing error
    pImpl->predictionError = pImpl->predictionError * 0.7f + newError * 0.3f;
    
    // Update confidence based on error magnitude
    pImpl->confidence = std::max(0.1f, 1.0f - std::abs(pImpl->predictionError));
}

std::vector<std::vector<float>> PredictionSystem::findSimilarStates(const std::vector<float>& queryState,
                                                                    float threshold) const {
    std::vector<std::vector<float>> similarStates;
    
    for (const auto& historicalState : pImpl->stateHistory) {
        if (historicalState.size() == queryState.size()) {
            float similarity = computePatternSimilarity(historicalState, queryState);
            if (similarity >= threshold) {
                similarStates.push_back(historicalState);
            }
        }
    }
    
    return similarStates;
}

std::vector<ActionType> PredictionSystem::transferActions(const std::vector<float>& sourceState,
                                                         const std::vector<float>& targetState) const {
    std::vector<ActionType> actions;
    
    // Find similar source states
    auto similarStates = findSimilarStates(sourceState);
    
    if (similarStates.empty()) {
        return actions;
    }
    
    // Get actions taken from similar states
    std::vector<ActionType> possibleActions;
    for (const auto& state : similarStates) {
        // Find index of this state in history
        auto it = std::find(pImpl->stateHistory.begin(), 
                           pImpl->stateHistory.end(), state);
        if (it != pImpl->stateHistory.end()) {
            size_t index = std::distance(pImpl->stateHistory.begin(), it);
            if (index < pImpl->actionHistory.size()) {
                possibleActions.push_back(pImpl->actionHistory[index]);
            }
        }
    }
    
    if (possibleActions.empty()) {
        return actions;
    }
    
    // Most common action is good to transfer
    std::unordered_map<ActionType, size_t> actionCounts;
    for (ActionType action : possibleActions) {
        actionCounts[action]++;
    }
    
    ActionType mostCommon = possibleActions[0];
    size_t maxCount = 0;
    for (const auto& pair : actionCounts) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostCommon = pair.first;
        }
    }
    
    // Transfer this action
    actions.push_back(mostCommon);
    
    return actions;
}

} // namespace nlm
