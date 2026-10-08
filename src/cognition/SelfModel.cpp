#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    std::unordered_map<uint64_t, std::vector<ActionEffect>> actionEffects;
    std::vector<float> selfModelConfidence;
    
    Impl() : brain(nullptr) {
        selfModelConfidence.resize(10, 0.0f);  // 10 action types
    }
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.0f)
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
    // Record that taking this action caused this state change
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.confidence = 1.0f;
    effect.observationCount = 1;
    
    // Store in action effects database
    pImpl->actionEffects[static_cast<uint64_t>(action)].push_back(effect);
    
    // Update self-model confidence
    pImpl->selfModelConfidence[static_cast<size_t>(action)] = 
        (pImpl->selfModelConfidence[static_cast<size_t>(action)] * 0.9f) + 0.1f;
    
    // Update capability level based on successful actions
    if (effect.observationCount > 0) {
        capabilityLevel_ = std::min(1.0f, capabilityLevel_ + 0.05f);
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                         const std::vector<float>& currentState) {
    // Predict sensory consequences of this action
    auto it = pImpl->actionEffects.find(static_cast<uint64_t>(action));
    
    if (it != pImpl->actionEffects.end() && !it->second.empty()) {
        // Find the most similar effect to current state
        const auto& effects = it->second;
        float bestSimilarity = 0.0f;
        int bestEffectIdx = 0;
        
        for (size_t i = 0; i < effects.size(); ++i) {
            float similarity = 0.0f;
            for (size_t j = 0; j < currentState.size() && j < effects[i].beforeState.size(); ++j) {
                similarity += std::abs(currentState[j] - effects[i].beforeState[j]);
            }
            similarity = 1.0f - similarity;  // Convert to similarity
            
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestEffectIdx = i;
            }
        }
        
        if (bestSimilarity > 0.5f) {
            return effects[bestEffectIdx].afterState;
        }
    }
    
    // If no good match found, return modified current state
    std::vector<float> predicted = currentState;
    for (float& value : predicted) {
        value += 0.1f;  // Small default change
    }
    return predicted;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    return pImpl->selfModelConfidence[static_cast<size_t>(action)];
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                            const std::vector<float>& afterState,
                                            ActionType action) const {
    auto it = pImpl->actionEffects.find(static_cast<uint64_t>(action));
    
    if (it == pImpl->actionEffects.end() || it->second.empty()) {
        return 0.0f;
    }
    
    float maxLikeness = 0.0f;
    
    for (const auto& effect : it->second) {
        float likeness = 0.0f;
        for (size_t i = 0; i < beforeState.size() && i < effect.beforeState.size(); ++i) {
            likeness += std::abs(beforeState[i] - effect.beforeState[i]);
        }
        for (size_t i = 0; i < afterState.size() && i < effect.afterState.size(); ++i) {
            likeness += std::abs(afterState[i] - effect.afterState[i]);
        }
        likeness = 1.0f - likeness;  // Convert to likeness
        maxLikeness = std::max(maxLikeness, likeness);
    }
    
    return maxLikeness;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    ActionType bestAction = ActionType::Wait;
    float bestScore = -1000.0f;
    
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Get prediction for this action
        std::vector<float> predicted = predictActionConsequence(action, state);
        
        // Simple heuristic: if prediction suggests improvement, select this action
        float score = 0.0f;
        for (float value : predicted) {
            score += value;
        }
        
        if (score > bestScore) {
            bestScore = score;
            bestAction = action;
        }
    }
    
    return bestAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                              const std::vector<float>& actual,
                              ActionType action) {
    // Update stored effects with better accuracy
    auto it = pImpl->actionEffects.find(static_cast<uint64_t>(action));
    
    if (it == pImpl->actionEffects.end()) {
        return;
    }
    
    // Find best matching stored effect
    float bestMatch = 0.0f;
    size_t bestIdx = 0;
    
    for (size_t i = 0; i < it->second.size(); ++i) {
        const auto& effect = it->second[i];
        float match = 0.0f;
        for (size_t j = 0; j < predicted.size() && j < effect.beforeState.size(); ++j) {
            match += std::abs(predicted[j] - effect.beforeState[j]);
        }
        match = 1.0f - match;  // Convert to match
        
        if (match > bestMatch) {
            bestMatch = match;
            bestIdx = i;
        }
    }
    
    if (bestMatch > 0.5f) {
        // Update the effect with better accuracy
        it->second[bestIdx].confidence = (it->second[bestIdx].confidence * 0.9f) + 0.1f;
        it->second[bestIdx].observationCount++;
        
        // Average the after state with actual observation
        for (size_t i = 0; i < it->second[bestIdx].afterState.size() && i < actual.size(); ++i) {
            it->second[bestIdx].afterState[i] = 
                (it->second[bestIdx].afterState[i] * 0.9f) + (actual[i] * 0.1f);
        }
    }
}

float SelfModel::getBodyAwareness() const {
    // Body awareness is the average confidence across all actions
    float totalConfidence = 0.0f;
    for (float conf : pImpl->selfModelConfidence) {
        totalConfidence += conf;
    }
    return totalConfidence / pImpl->selfModelConfidence.size();
}

void SelfModel::clear() {
    pImpl->actionEffects.clear();
    pImpl->selfModelConfidence.assign(pImpl->selfModelConfidence.size(), 0.0f);
    capabilityLevel_ = 0.0f;
}

} // namespace nlm