// Implementation for SelfModel - Neural self-representation and body awareness
// NOT consciousness - sensorimotor self-awareness through experience

#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    
    // Random number generator for stochastic learning
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
    std::normal_distribution<float> normalDist;
    
    // Body effect similarity database
    std::vector<std::vector<float>> bodyEffectSimilarities;
    
    // State compression for efficient storage
    std::vector<float> compressedState;
    
    // Learning statistics
    size_t totalPredictions;
    size_t successfulPredictions;
    
    Impl() : brain(nullptr), totalPredictions(0), successfulPredictions(0) {
        // Seed random generator
        std::random_device rd;
        rng.seed(rd());
        randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
        bodyEffectSimilarities.resize(10);  // 10 action types
    }
};

SelfModel::SelfModel() : pImpl(std::make_unique<Impl>()) {
    brain_ = nullptr;
    learningRate_ = 0.05f;
    confidenceThreshold_ = 0.6f;
    maxEffectsPerAction_ = 50;
    bodyAwareness_ = 0.0f;
    selfModelAccuracy_ = 0.0f;
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized with brain reference");
}

void SelfModel::recordSelfAction(ActionType action,
                                 const std::vector<float>& beforeState,
                                 const std::vector<float>& afterState,
                                 float reward) {
    if (!brain_) {
        return;
    }
    
    // Check if we already have enough effects for this action
    if (bodyEffects_[static_cast<size_t>(action)].size() >= maxEffectsPerAction_) {
        // Remove oldest effect (simple FIFO)
        bodyEffects_[static_cast<size_t>(action)].erase(
            bodyEffects_[static_cast<size_t>(action)].begin());
    }
    
    // Calculate similarity with existing effects
    float bestSimilarity = 0.0f;
    size_t bestMatchIdx = 0;
    
    auto& effects = bodyEffects_[static_cast<size_t>(action)];
    for (size_t i = 0; i < effects.size(); ++i) {
        float similarity = calculateStateSimilarity(beforeState, effects[i].beforeState);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatchIdx = i;
        }
    }
    
    // Create new body effect
    BodyEffect newEffect(action, beforeState, afterState, bestSimilarity);
    
    // If we have a similar effect, merge or update confidence
    if (bestSimilarity > confidenceThreshold_) {
        // Update existing effect
        newEffect = effects[bestMatchIdx];
        newEffect.observationCount++;
        newEffect.avgReward = (newEffect.avgReward * (newEffect.observationCount - 1) + reward) /
                             newEffect.observationCount;
        newEffect.confidence = (newEffect.confidence + 1.0f) / 2.0f;  // Increase confidence
        effects[bestMatchIdx] = newEffect;
        
        NLM_LOG_DEBUG(std::string("Updated self-model for action ") + std::to_string(static_cast<int>(action)) +
                     std::string(" with similarity ") + std::to_string(bestSimilarity));
    } else {
        // Add new effect
        effects.push_back(newEffect);
        NLM_LOG_DEBUG(std::string("Recorded new self-model effect for action ") + std::to_string(static_cast<int>(action)));
    }
    
    // Update capability level based on success
    learnCapability(action, 1.0f);  // Assume success for now, can be adjusted
    
    pImpl->totalPredictions++;
    if (bestSimilarity > confidenceThreshold_) {
        pImpl->successfulPredictions++;
    }
    
    updateBodySchema(action, beforeState, afterState);
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) const {
    if (!brain_ || bodyEffects_[static_cast<size_t>(action)].empty()) {
        // Return current state if no model available
        return currentState;
    }
    
    // Find best matching effect
    std::vector<float> bestEffect;
    float bestSimilarity = 0.0f;
    
    const auto& effects = bodyEffects_[static_cast<size_t>(action)];
    for (const auto& effect : effects) {
        float similarity = calculateStateSimilarity(currentState, effect.beforeState);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestEffect = effect.afterState;
        }
    }
    
    if (bestEffect.empty() && !effects.empty()) {
        // Use the most confident effect
        const auto& mostConfident = effects[0];
        for (const auto& effect : effects) {
            if (effect.confidence > mostConfident.confidence) {
                bestEffect = effect.afterState;
            }
        }
    }
    
    pImpl->totalPredictions++;
    if (bestEffect.empty()) {
        // Return current state as fallback
        return currentState;
    }
    
    NLM_LOG_DEBUG(std::string("Predicted consequence for action ") + std::to_string(static_cast<int>(action)) +
                 std::string(" with similarity ") + std::to_string(bestSimilarity));
    
    return bestEffect;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    const auto& effects = bodyEffects_[static_cast<size_t>(action)];
    if (effects.empty()) {
        return 0.0f;
    }
    
    float totalConfidence = 0.0f;
    for (const auto& effect : effects) {
        totalConfidence += effect.confidence;
    }
    
    return totalConfidence / effects.size();
}

float SelfModel::computeSelfGeneratedLikelihood(const std::vector<float>& beforeState,
                                                const std::vector<float>& afterState,
                                                ActionType action) const {
    if (!brain_ || bodyEffects_[static_cast<size_t>(action)].empty()) {
        return 0.0f;  // Assume external if no model
    }
    
    // Find best matching effect
    float bestSimilarity = 0.0f;
    const auto& effects = bodyEffects_[static_cast<size_t>(action)];
    
    for (const auto& effect : effects) {
        float similarity = calculateStateSimilarity(beforeState, effect.beforeState);
        float afterSimilarity = calculateStateSimilarity(afterState, effect.afterState);
        float combinedSimilarity = (similarity + afterSimilarity) / 2.0f;
        
        if (combinedSimilarity > bestSimilarity) {
            bestSimilarity = combinedSimilarity;
        }
    }
    
    // Likelihood is high if we have a good match
    return bestSimilarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) const {
    ActionType preferredAction = ActionType::Wait;
    float bestScore = 0.0f;
    
    for (size_t i = 0; i < bodyEffects_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        const auto& effects = bodyEffects_[i];
        
        if (effects.empty()) {
            continue;
        }
        
        // Calculate average confidence for this action
        float totalConfidence = 0.0f;
        for (const auto& effect : effects) {
            totalConfidence += effect.confidence;
        }
        float avgConfidence = totalConfidence / effects.size();
        
        // Get capability level for this action
        float capability = getCapabilityLevel(action);
        
        // Combine confidence and capability for score
        float score = (avgConfidence + capability) / 2.0f;
        
        if (score > bestScore) {
            bestScore = score;
            preferredAction = action;
        }
    }
    
    return preferredAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    float predictionError = calculateStateSimilarity(predicted, actual);
    
    if (predictionError < 0.5f) {
        // Prediction was good, increase confidence
        updateConfidence(action, predictionError);
        
        // Improve future predictions
        NLM_LOG_DEBUG(std::string("Self-model improved - prediction error ") + std::to_string(predictionError));
    } else {
        // Prediction was poor, decrease confidence
        updateConfidence(action, -predictionError);
        
        NLM_LOG_DEBUG(std::string("Self-model updated - prediction error ") + std::to_string(predictionError));
    }
    
    // Update body awareness based on prediction accuracy
    updateBodyAwareness();
}

void SelfModel::learnCapability(ActionType action, float successRate) {
    auto& capabilities = capabilityLevels_;
    size_t actionIdx = static_cast<size_t>(action);
    
    if (actionIdx >= capabilities.size()) {
        capabilities.resize(actionIdx + 1);
    }
    
    auto& capability = capabilities[actionIdx];
    
    // Update capability level
    capability.action = action;
    capability.level = (capability.level * 0.8f) + (successRate * 0.2f);  // Weighted update
    capability.attempts++;
    
    // Update stability based on consistency
    if (capability.attempts > 1) {
        capability.stability = std::min(1.0f, capability.stability + 0.05f);
    } else {
        capability.stability = successRate;
    }
    
    NLM_LOG_DEBUG(std::string("Learned capability ") + std::to_string(static_cast<int>(action)) +
                 std::string(" level ") + std::to_string(capability.level));
}

void SelfModel::updateBodySchema(ActionType action,
                                const std::vector<float>& beforeState,
                                const std::vector<float>& afterState) {
    // Find existing body schema entry
    bool found = false;
    for (auto& effect : bodyEffects_[static_cast<size_t>(action)]) {
        if (calculateStateSimilarity(beforeState, effect.beforeState) > 0.9f) {
            // Update existing entry
            effect.afterState = afterState;
            effect.confidence = std::min(1.0f, effect.confidence + 0.1f);
            found = true;
            break;
        }
    }
    
    if (!found) {
        // Create new body schema entry
        BodyEffect newEffect(action, beforeState, afterState, 1.0f);
        bodyEffects_[static_cast<size_t>(action)].push_back(newEffect);
    }
    
    updateBodyAwareness();
}

void SelfModel::clear() {
    for (auto& effects : bodyEffects_) {
        effects.clear();
    }
    
    for (auto& capability : capabilityLevels_) {
        capability.level = 0.0f;
        capability.stability = 0.0f;
    }
    
    bodyAwareness_ = 0.0f;
    selfModelAccuracy_ = 0.0f;
    
    NLM_LOG_INFO("SelfModel cleared");
}

void SelfModel::updateBodyAwareness() {
    if (pImpl->totalPredictions == 0) {
        bodyAwareness_ = 0.0f;
        return;
    }
    
    // Calculate body awareness as successful predictions rate
    bodyAwareness_ = static_cast<float>(pImpl->successfulPredictions) / pImpl->totalPredictions;
    
    // Update overall self-model accuracy
    selfModelAccuracy_ = bodyAwareness_;
    
    NLM_LOG_DEBUG(std::string("Body awareness updated: ") + std::to_string(bodyAwareness_));
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                 const std::vector<float>& beforeState) const {
    const auto& effects = bodyEffects_[static_cast<size_t>(action)];
    
    if (effects.empty()) {
        return std::vector<float>();
    }
    
    // Find most similar effect
    float bestSimilarity = 0.0f;
    std::vector<float> bestAfterState;
    
    for (const auto& effect : effects) {
        float similarity = calculateStateSimilarity(beforeState, effect.beforeState);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestAfterState = effect.afterState;
        }
    }
    
    return bestAfterState;
}

float SelfModel::calculateStateSimilarity(const std::vector<float>& state1,
                                         const std::vector<float>& state2) const {
    if (state1.size() != state2.size()) {
        return 0.0f;
    }
    
    float sumSquaredDiff = 0.0f;
    for (size_t i = 0; i < state1.size(); ++i) {
        float diff = state1[i] - state2[i];
        sumSquaredDiff += diff * diff;
    }
    
    float similarity = 1.0f / (1.0f + sqrt(sumSquaredDiff));
    return similarity;
}

void SelfModel::updateConfidence(ActionType action, float predictionError) {
    auto& effects = bodyEffects_[static_cast<size_t>(action)];
    
    if (effects.empty()) {
        return;
    }
    
    // Update confidence based on prediction error
    for (auto& effect : effects) {
        effect.confidence = std::max(0.0f, effect.confidence + learningRate_ * (1.0f - predictionError));
        
        // Cap confidence
        if (effect.confidence > 1.0f) {
            effect.confidence = 1.0f;
        }
    }
}

float SelfModel::computeAverageReward(ActionType action) const {
    const auto& effects = bodyEffects_[static_cast<size_t>(action)];
    
    if (effects.empty()) {
        return 0.0f;
    }
    
    float totalReward = 0.0f;
    for (const auto& effect : effects) {
        totalReward += effect.avgReward;
    }
    
    return totalReward / effects.size();
}

float SelfModel::getCapabilityLevel(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    
    if (actionIdx >= capabilityLevels_.size()) {
        return 0.0f;
    }
    
    return capabilityLevels_[actionIdx].level;
}

float SelfModel::getBodyAwareness() const {
    return bodyAwareness_;
}

float SelfModel::getSelfModelAccuracy() const {
    return selfModelAccuracy_;
}

} // namespace nlm