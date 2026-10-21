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
    actionEffects_.resize(10, std::vector<ActionEffect>());  // 10 action types
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
    if (!brain_) return;
    
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) {
        actionEffects_.resize(actionIdx + 1, std::vector<ActionEffect>());
    }
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.confidence = 1.0f;
    effect.observationCount = 1;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Update capability level based on success
    float reward = computeSelfGeneratedLikeness(beforeState, afterState, action);
    capabilityLevel_ = std::min(1.0f, capabilityLevel_ + 0.01f * reward);
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                        const std::vector<float>& currentState) {
    // Find the best matching effect for this action and state
    std::vector<float> bestResult;
    float bestConfidence = 0.0f;
    
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return bestResult;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float similarity = computeSimilarity(currentState, effect.beforeState);
        
        if (similarity > bestConfidence && similarity > 0.5f) {
            bestConfidence = similarity;
            bestResult = effect.afterState;
        }
    }
    
    return bestResult;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size() || actionEffects_[actionIdx].empty()) {
        return 0.0f;
    }
    
    float totalConfidence = 0.0f;
    for (const auto& effect : actionEffects_[actionIdx]) {
        totalConfidence += effect.confidence;
    }
    
    return totalConfidence / actionEffects_[actionIdx].size();
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                           const std::vector<float>& afterState,
                                           ActionType action) const {
    // Find the best matching effect
    std::vector<float> bestResult;
    float bestConfidence = 0.0f;
    
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float similarity = computeSimilarity(beforeState, effect.beforeState);
        
        if (similarity > bestConfidence) {
            bestConfidence = similarity;
            bestResult = effect.afterState;
        }
    }
    
    // Compare predicted result with actual result
    float resultSimilarity = computeSimilarity(bestResult, afterState);
    
    return resultSimilarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    std::vector<std::pair<ActionType, float>> scores;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Find the effect that best matches current state
        float bestSimilarity = 0.0f;
        for (const auto& effect : actionEffects_[i]) {
            float similarity = computeSimilarity(state, effect.beforeState);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
            }
        }
        
        scores.push_back({action, bestSimilarity});
    }
    
    // Find action with highest similarity
    ActionType bestAction = ActionType::Wait;
    float bestScore = 0.0f;
    
    for (const auto& score : scores) {
        if (score.second > bestScore) {
            bestScore = score.second;
            bestAction = score.first;
        }
    }
    
    return bestAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    float confidence = computeSimilarity(predicted, actual);
    
    // Find matching effect or create new one
    ActionEffect* targetEffect = nullptr;
    for (auto& effect : actionEffects_[actionIdx]) {
        if (computeSimilarity(predicted, effect.beforeState) > 0.8f) {
            targetEffect = &effect;
            break;
        }
    }
    
    if (targetEffect) {
        // Update existing effect
        targetEffect->confidence = (targetEffect->confidence + confidence) * 0.5f;
        targetEffect->observationCount++;
    } else {
        // Create new effect
        ActionEffect effect;
        effect.beforeState = predicted;
        effect.afterState = actual;
        effect.confidence = confidence;
        effect.observationCount = 1;
        
        actionEffects_[actionIdx].push_back(effect);
    }
}

void SelfModel::clear() {
    actionEffects_.clear();
    actionEffects_.resize(10, std::vector<ActionEffect>());
    capabilityLevel_ = 0.5f;
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                               const std::vector<float>& beforeState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return std::vector<float>();
    
    std::vector<float> bestMatch;
    float bestSimilarity = 0.0f;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float similarity = computeSimilarity(beforeState, effect.beforeState);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = effect.beforeState;
        }
    }
    
    return bestMatch;
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
