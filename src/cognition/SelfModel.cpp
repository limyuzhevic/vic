// SelfModel implementation
#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SelfModel::SelfModel()
    : pImpl(std::make_unique<Impl>())
    , bodyAwarenessLevel_(0.5f) {
    actionEffects_.resize(10); // 10 action types
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("SelfModel initialized");
}

void SelfModel::recordSelfAction(ActionType action,
                                const std::vector<float>& beforeState,
                                const std::vector<float>& afterState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.observationCount = 1;
    
    // Compute confidence as consistency with previous observations
    float consistency = 0.5f;
    for (const auto& prev : actionEffects_[actionIdx]) {
        if (MathUtilities::cosineSimilarity(beforeState, prev.beforeState) > 0.8f) {
            consistency = std::max(consistency, 
                MathUtilities::cosineSimilarity(afterState, prev.afterState));
            effect.observationCount += prev.observationCount;
        }
    }
    effect.confidence = consistency;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Update body awareness based on consistency
    float totalConfidence = 0.0f;
    size_t count = 0;
    for (const auto& effects : actionEffects_) {
        for (const auto& e : effects) {
            totalConfidence += e.confidence;
            ++count;
        }
    }
    if (count > 0) {
        bodyAwarenessLevel_ = totalConfidence / count;
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return currentState;
    
    return findMatchingEffect(action, currentState);
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return 0.0f;
    
    float totalConf = 0.0f;
    for (const auto& e : effects) {
        totalConf += e.confidence;
    }
    return totalConf / effects.size();
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    // Find and update matching effect
    for (auto& effect : actionEffects_[actionIdx]) {
        if (MathUtilities::cosineSimilarity(effect.beforeState, predicted) > 0.5f) {
            // Update with prediction error
            float error = 1.0f - MathUtilities::cosineSimilarity(predicted, actual);
            
            // Adjust confidence based on error
            effect.confidence *= (1.0f - error * 0.1f);
            effect.confidence = std::max(0.1f, effect.confidence);
        }
    }
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) const {
    ActionType best = ActionType::Wait;
    float bestValue = -1000.0f;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        if (actionEffects_[i].empty()) continue;
        
        // Check how well this action would work in current state
        auto predicted = findMatchingEffect(static_cast<ActionType>(i), state);
        if (!predicted.empty()) {
            // Value = how much the state changes toward reward
            float value = MathUtilities::cosineSimilarity(predicted, state);
            if (value > bestValue) {
                bestValue = value;
                best = static_cast<ActionType>(i);
            }
        }
    }
    
    return best;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    bodyAwarenessLevel_ = 0.5f;
}

float SelfModel::getPredictionConfidence(ActionType action) const {
    return getSelfModelConfidence(action);
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                 const std::vector<float>& beforeState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return {};
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return {};
    
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& effect : effects) {
        float sim = MathUtilities::cosineSimilarity(beforeState, effect.beforeState);
        if (sim > bestSim) {
            bestSim = sim;
            bestPrediction = effect.afterState;
        }
    }
    
    return bestPrediction;
}

} // namespace nlm
