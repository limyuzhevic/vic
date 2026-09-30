#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    float capabilityLevel;
    
    Impl() : brain(nullptr), capabilityLevel(0.0f) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.0f)
{
    NLM_LOG_INFO("SelfModel created");
}

SelfModel::~SelfModel() {
    NLM_LOG_INFO("SelfModel destroyed");
}

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("SelfModel initialized");
}

void SelfModel::recordSelfAction(ActionType action,
                                const std::vector<float>& beforeState,
                                const std::vector<float>& afterState) {
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.confidence = 1.0f;
    effect.observationCount = 1;
    
    actionEffects_[static_cast<size_t>(action)].push_back(effect);
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) {
    std::vector<float> bestPrediction;
    float bestConfidence = 0.0f;
    
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    for (const auto& effect : effects) {
        if (effect.confidence > bestConfidence) {
            bestConfidence = effect.confidence;
            bestPrediction = effect.afterState;
        }
    }
    
    if (!bestPrediction.empty()) {
        // Apply noise to prediction
        for (float& val : bestPrediction) {
            val += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
        }
    }
    
    return bestPrediction;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    float totalConfidence = 0.0f;
    size_t count = 0;
    
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    for (const auto& effect : effects) {
        totalConfidence += effect.confidence;
        count++;
    }
    
    if (count == 0) return 0.0f;
    return totalConfidence / static_cast<float>(count);
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                            const std::vector<float>& afterState,
                                            ActionType action) const {
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    
    float bestLikelihood = 0.0f;
    for (const auto& effect : effects) {
        float similarity = 0.0f;
        for (size_t i = 0; i < beforeState.size() && i < afterState.size(); ++i) {
            similarity += std::abs(effect.afterState[i] - afterState[i]);
        }
        similarity = 1.0f - std::min(1.0f, similarity);
        
        if (similarity > bestLikelihood) {
            bestLikelihood = similarity;
        }
    }
    
    return bestLikelihood;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    std::vector<float> bestPredictions;
    std::vector<float> bestConfidences;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        std::vector<float> prediction = predictActionConsequence(action, state);
        float confidence = getSelfModelConfidence(action);
        
        if (!prediction.empty()) {
            bestPredictions.push_back(prediction[0]);  // Use first dimension
            bestConfidences.push_back(confidence);
        }
    }
    
    if (bestPredictions.empty()) {
        return ActionType::Wait;
    }
    
    // Find prediction closest to current state's first dimension
    float currentValue = state.empty() ? 0.0f : state[0];
    size_t bestIndex = 0;
    float bestDistance = std::abs(bestPredictions[0] - currentValue);
    
    for (size_t i = 1; i < bestPredictions.size(); ++i) {
        float distance = std::abs(bestPredictions[i] - currentValue);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = i;
        }
    }
    
    return static_cast<ActionType>(bestIndex);
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                              const std::vector<float>& actual,
                              ActionType action) {
    float predictionError = 0.0f;
    for (size_t i = 0; i < predicted.size() && i < actual.size(); ++i) {
        predictionError += std::abs(predicted[i] - actual[i]);
    }
    predictionError = 1.0f - std::min(1.0f, predictionError);
    
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    for (auto& effect : effects) {
        effect.confidence *= (0.8f + 0.2f * predictionError);
        if (predictionError > 0.5f) {
            effect.observationCount++;
            // Update effect if this is better
            if (predictionError > 0.8f) {
                effect.afterState = actual;
            }
        }
    }
    
    // Update capability level based on prediction accuracy
    capabilityLevel_ = (capabilityLevel_ * 0.9f) + (predictionError * 0.1f);
}

void SelfModel::clear() {
    for (auto& actionEffects : actionEffects_) {
        actionEffects.clear();
    }
}

float SelfModel::getBodyAwareness() const {
    float totalAwareness = 0.0f;
    size_t count = 0;
    
    for (const auto& actionEffects : actionEffects_) {
        for (const auto& effect : actionEffects) {
            totalAwareness += effect.confidence;
            count++;
        }
    }
    
    if (count == 0) return 0.0f;
    return totalAwareness / static_cast<float>(count);
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                const std::vector<float>& beforeState) const {
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    
    std::vector<float> bestMatch;
    float bestScore = 0.0f;
    
    for (const auto& effect : effects) {
        float score = 0.0f;
        for (size_t i = 0; i < beforeState.size() && i < effect.beforeState.size(); ++i) {
            score += std::abs(beforeState[i] - effect.beforeState[i]);
        }
        score = 1.0f - std::min(1.0f, score);
        
        if (score > bestScore) {
            bestScore = score;
            bestMatch = effect.afterState;
        }
    }
    
    return bestMatch;
}

} // namespace nlm