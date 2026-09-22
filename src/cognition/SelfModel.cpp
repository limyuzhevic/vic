#pragma once

#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    float bodyAwareness;
    float capabilityLevel;
    float predictionAccuracy;
    std::unordered_map<ActionType, float> actionEffectiveness;
    
    Impl() : brain(nullptr), bodyAwareness(0.5f), capabilityLevel(0.5f), predictionAccuracy(0.7f) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    actionEffects_.resize(10);  // 10 action types
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized");
}

void SelfModel::recordSelfAction(ActionType action,
                               const std::vector<float>& beforeState,
                               const std::vector<float>& afterState) {
    // Find matching effect
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    
    float similarity = 1.0f;
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < effects.size(); ++i) {
        float sim = computeSelfGeneratedLikeness(beforeState, effects[i].beforeState, action);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = i;
        }
    }
    
    if (bestSimilarity > 0.8f) {
        // Update existing effect
        auto& effect = effects[bestMatch];
        effect.observationCount++;
        
        // Blend with new observation (moving average)
        float weight = 1.0f / effect.observationCount;
        for (size_t i = 0; i < beforeState.size() && i < effect.beforeState.size(); ++i) {
            effect.beforeState[i] = effect.beforeState[i] * (1.0f - weight) + beforeState[i] * weight;
        }
        for (size_t i = 0; i < afterState.size() && i < effect.afterState.size(); ++i) {
            effect.afterState[i] = effect.afterState[i] * (1.0f - weight) + afterState[i] * weight;
        }
        
        effect.confidence = std::min(1.0f, effect.confidence + 0.1f);
    } else {
        // Create new effect record
        ActionEffect newEffect;
        newEffect.beforeState = beforeState;
        newEffect.afterState = afterState;
        newEffect.confidence = 0.5f;
        newEffect.observationCount = 1;
        
        effects.push_back(newEffect);
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                      const std::vector<float>& currentState) {
    // Find best matching effect
    auto& effects = actionEffects_[static_cast<size_t>(action)];
    
    std::vector<float> predictedNext;
    float bestConfidence = 0.0f;
    
    for (const auto& effect : effects) {
        float similarity = computeSelfGeneratedLikeness(currentState, effect.beforeState, action);
        
        if (similarity > bestConfidence) {
            bestConfidence = similarity;
            predictedNext = effect.afterState;
        }
    }
    
    if (predictedNext.empty()) {
        // No effect found, predict no change
        predictedNext = currentState;
    }
    
    // Confidence affects prediction accuracy
    float accuracy = bestConfidence * pImpl->predictionAccuracy;
    
    // Add some noise based on uncertainty
    for (auto& value : predictedNext) {
        value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * (1.0f - accuracy) * 0.1f;
    }
    
    return predictedNext;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    auto it = pImpl->actionEffectiveness.find(action);
    if (it != pImpl->actionEffectiveness.end()) {
        return it->second;
    }
    return 0.0f;
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                           const std::vector<float>& afterState,
                                           ActionType action) const {
    // Simple self-likelihood computation: lower change = more likely self-generated
    if (beforeState.size() != afterState.size() || beforeState.empty()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(beforeState.size(), afterState.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(beforeState[i] - afterState[i]);
        totalDiff += diff;
    }
    
    // Normalize: low difference = high self-likelihood
    float avgDiff = totalDiff / compareLen;
    return std::max(0.0f, 1.0f - avgDiff);
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    // Find action with highest effectiveness for this state
    ActionType preferredAction = ActionType::Wait;
    float highestEffectiveness = 0.0f;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        auto& effects = actionEffects_[i];
        if (!effects.empty()) {
            // Calculate effectiveness based on average confidence
            float totalConfidence = 0.0f;
            for (const auto& effect : effects) {
                totalConfidence += effect.confidence;
            }
            float avgConfidence = totalConfidence / effects.size();
            
            if (avgConfidence > highestEffectiveness) {
                highestEffectiveness = avgConfidence;
                preferredAction = static_cast<ActionType>(i);
            }
        }
    }
    
    return preferredAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                              const std::vector<float>& actual,
                              ActionType action) {
    // Update self-model based on prediction error
    std::vector<float> error(predicted.size());
    for (size_t i = 0; i < predicted.size(); ++i) {
        error[i] = std::abs(predicted[i] - actual[i]);
    }
    
    // Update action effectiveness
    float predictionError = std::accumulate(error.begin(), error.end(), 0.0f) / error.size();
    
    auto it = pImpl->actionEffectiveness.find(action);
    if (it != pImpl->actionEffectiveness.end()) {
        // Reduce effectiveness based on error
        it->second = std::max(0.0f, it->second - predictionError * 0.2f);
    } else {
        pImpl->actionEffectiveness[action] = 1.0f - predictionError;
    }
}

float SelfModel::getBodyAwareness() const {
    return pImpl->bodyAwareness;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    pImpl->actionEffectiveness.clear();
    pImpl->bodyAwareness = 0.5f;
    pImpl->capabilityLevel = 0.5f;
    pImpl->predictionAccuracy = 0.7f;
    NLM_LOG_INFO("SelfModel cleared");
}

} // namespace nlm
