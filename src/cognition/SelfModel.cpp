#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
{
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
    // Convert action to index
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) {
        actionEffects_.resize(actionIdx + 1);
    }
    
    // Find matching before state in existing effects
    bool found = false;
    for (auto& effect : actionEffects_[actionIdx]) {
        if (computeSimilarity(beforeState, effect.beforeState) > 0.8f) {
            // Update existing effect
            effect.afterState = afterState;
            // Blend confidence with previous
            effect.confidence = (effect.confidence + 1.0f) / 2.0f;
            effect.observationCount++;
            found = true;
            break;
        }
    }
    
    if (!found) {
        // Create new effect
        ActionEffect newEffect;
        newEffect.beforeState = beforeState;
        newEffect.afterState = afterState;
        newEffect.confidence = 1.0f;
        newEffect.observationCount = 1;
        actionEffects_[actionIdx].push_back(newEffect);
    }
    
    // Update capability based on successful predictions
    capabilityLevel_ = std::min(1.0f, capabilityLevel_ + 0.01f);
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                      const std::vector<float>& currentState) {
    // Find best matching effect for this action and current state
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size() || actionEffects_[actionIdx].empty()) {
        // No knowledge about this action, return unchanged state
        return currentState;
    }
    
    // Find most similar before state
    const ActionEffect* bestEffect = nullptr;
    float bestSimilarity = 0.0f;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float sim = computeSimilarity(currentState, effect.beforeState);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestEffect = &effect;
        }
    }
    
    if (bestEffect && bestSimilarity > 0.5f) {
        // Return predicted after state
        return bestEffect->afterState;
    }
    
    // No good prediction, return current state
    return currentState;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    float totalConfidence = 0.0f;
    size_t count = 0;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        totalConfidence += effect.confidence;
        count++;
    }
    
    return count > 0 ? totalConfidence / count : 0.0f;
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                            const std::vector<float>& afterState,
                                            ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    // Find best matching effect
    const ActionEffect* bestEffect = nullptr;
    float bestSimilarity = 0.0f;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float sim = computeSimilarity(beforeState, effect.beforeState);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestEffect = &effect;
        }
    }
    
    if (!bestEffect) return 0.0f;
    
    // Compute similarity between predicted and actual after state
    float afterSim = computeSimilarity(afterState, bestEffect->afterState);
    return afterSim;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    // Find action that gives most reliable predictions for current state
    ActionType preferredAction = ActionType::Wait;
    float highestConfidence = 0.0f;
    
    for (size_t actionIdx = 0; actionIdx < ActionTypeCount; ++actionIdx) {
        ActionType action = static_cast<ActionType>(actionIdx);
        float confidence = getSelfModelConfidence(action);
        
        // Also consider similarity of state to known before states
        float stateSimilarity = 0.0f;
        for (const auto& effect : actionEffects_[actionIdx]) {
            float sim = computeSimilarity(state, effect.beforeState);
            stateSimilarity = std::max(stateSimilarity, sim);
        }
        
        float combinedScore = confidence * stateSimilarity;
        if (combinedScore > highestConfidence) {
            highestConfidence = combinedScore;
            preferredAction = action;
        }
    }
    
    return preferredAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                              const std::vector<float>& actual,
                              ActionType action) {
    // Update confidence of most similar effect
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    // Find effect with most similar before state (but we don't have access to before state here)
    // For simplicity, update all effects for this action
    for (auto& effect : actionEffects_[actionIdx]) {
        // Update confidence based on prediction error
        float error = computeSimilarity(predicted, actual);
        effect.confidence = (effect.confidence + error) / 2.0f;
    }
}

float SelfModel::getBodyAwareness() const {
    // Compute average confidence across all actions
    float totalConfidence = 0.0f;
    size_t totalEffects = 0;
    
    for (const auto& effects : actionEffects_) {
        for (const auto& effect : effects) {
            totalConfidence += effect.confidence;
            totalEffects++;
        }
    }
    
    if (totalEffects == 0) return 0.0f;
    return totalConfidence / totalEffects;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    capabilityLevel_ = 0.5f;
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                const std::vector<float>& beforeState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return std::vector<float>();
    
    // Find most similar effect
    const ActionEffect* bestEffect = nullptr;
    float bestSimilarity = 0.0f;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float sim = computeSimilarity(beforeState, effect.beforeState);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestEffect = &effect;
        }
    }
    
    if (bestEffect) {
        return bestEffect->afterState;
    }
    
    return std::vector<float>();
}

float SelfModel::computeSimilarity(const std::vector<float>& a,
                                   const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
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
