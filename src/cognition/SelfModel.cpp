#include "AdvancedCognitive.hpp"

namespace nlm {

struct SelfModel::Impl {
    // Learning rate for self-model updates
    float learningRate_;
    
    // Confidence in predictions
    float predictionConfidence_;
    
    // Body awareness
    float bodyAwareness_;
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.0f)
{
    pImpl->learningRate_ = 0.1f;
    pImpl->predictionConfidence_ = 0.5f;
    pImpl->bodyAwareness_ = 0.3f;
    NLM_LOG_INFO("SelfModel created");
}

SelfModel::~SelfModel() {
    NLM_LOG_INFO("SelfModel destroyed");
}

void SelfModel::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized");
}

void SelfModel::recordSelfAction(ActionType action,
                                const std::vector<float>& beforeState,
                                const std::vector<float>& afterState) {
    if (!brain_) return;
    
    // Find or create action effects for this action
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex >= actionEffects_.size()) {
        actionEffects_.resize(actionIndex + 1);
    }
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.confidence = 0.5f;  // Initial confidence
    effect.observationCount = 0;
    
    actionEffects_[actionIndex].push_back(effect);
    
    // Update capability level
    capabilityLevel_ = std::min(1.0f, capabilityLevel_ + pImpl->learningRate_ * 0.1f);
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                    const std::vector<float>& currentState) {
    if (!brain_) return std::vector<float>();
    
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex >= actionEffects_.size() || actionEffects_[actionIndex].empty()) {
        // No model exists - return current state as prediction
        return currentState;
    }
    
    // Find best matching effect
    const auto& effects = actionEffects_[actionIndex];
    float bestConfidence = 0.0f;
    const ActionEffect* bestEffect = nullptr;
    
    for (const auto& effect : effects) {
        if (effect.confidence > bestConfidence) {
            bestConfidence = effect.confidence;
            bestEffect = &effect;
        }
    }
    
    if (!bestEffect) return currentState;
    
    // Compute prediction with some noise
    std::vector<float> predicted = currentState;
    for (size_t i = 0; i < currentState.size() && i < bestEffect->afterState.size(); ++i) {
        predicted[i] = bestEffect->afterState[i] * (bestEffect->confidence * 0.5f + 0.5f);
    }
    
    return predicted;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex >= actionEffects_.size() || actionEffects_[actionIndex].empty()) {
        return 0.0f;
    }
    
    const auto& effects = actionEffects_[actionIndex];
    float maxConfidence = 0.0f;
    for (const auto& effect : effects) {
        maxConfidence = std::max(maxConfidence, effect.confidence);
    }
    
    return maxConfidence;
}

float SelfModel::computeSelfGeneratedLikelihood(const std::vector<float>& beforeState,
                                             const std::vector<float>& afterState,
                                             ActionType action) const {
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex >= actionEffects_.size() || actionEffects_[actionIndex].empty()) {
        return 0.0f;
    }
    
    float similarity = 0.0f;
    const auto& effects = actionEffects_[actionIndex];
    
    for (const auto& effect : effects) {
        if (effect.beforeState.size() == beforeState.size() && 
            effect.afterState.size() == afterState.size()) {
            
            float sim = 0.0f;
            for (size_t i = 0; i < beforeState.size(); ++i) {
                sim += std::abs(effect.beforeState[i] - beforeState[i]);
                sim += std::abs(effect.afterState[i] - afterState[i]);
            }
            
            // Normalize to 0-1, where 1 means identical
            float maxSim = beforeState.size() * 2;
            if (maxSim > 0.0f) {
                similarity = 1.0f - (sim / maxSim);
            }
        }
    }
    
    return similarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    if (actionEffects_.empty()) return ActionType::Wait;
    
    ActionType bestAction = ActionType::Wait;
    float bestConfidence = 0.0f;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        const auto& effects = actionEffects_[i];
        if (!effects.empty() && effects[0].confidence > bestConfidence) {
            bestConfidence = effects[0].confidence;
            bestAction = static_cast<ActionType>(i);
        }
    }
    
    return bestAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                             const std::vector<float>& actual,
                             ActionType action) {
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex >= actionEffects_.size() || actionEffects_[actionIndex].empty()) return;
    
    // Find matching effect
    auto& effects = actionEffects_[actionIndex];
    bool found = false;
    
    for (auto& effect : effects) {
        // Check if this effect matches the current state
        if (effect.beforeState.size() == predicted.size() &&
            effect.afterState.size() == actual.size()) {
            
            // Compute prediction error
            float error = 0.0f;
            for (size_t i = 0; i < predicted.size() && i < actual.size(); ++i) {
                error += std::abs(predicted[i] - actual[i]);
            }
            
            float errorScore = 1.0f - std::min(1.0f, error / 10.0f);
            
            // Update confidence based on prediction error
            effect.confidence = (effect.confidence * 0.8f) + (errorScore * 0.2f);
            effect.observationCount++;
            
            // Update body awareness
            pImpl->bodyAwareness_ = std::min(1.0f, pImpl->bodyAwareness_ + errorScore * 0.05f);
            
            found = true;
            break;
        }
    }
    
    // If no match found, create new effect
    if (!found) {
        ActionEffect newEffect;
        newEffect.beforeState = predicted;
        newEffect.afterState = actual;
        newEffect.confidence = 0.3f;  // Start with low confidence
        newEffect.observationCount = 1;
        
        effects.push_back(newEffect);
    }
}

void SelfModel::clear() {
    actionEffects_.clear();
    capabilityLevel_ = 0.0f;
    pImpl->bodyAwareness_ = 0.3f;
    NLM_LOG_INFO("SelfModel cleared");
}

void SelfModel::update(const class NeuralAssociativeMemory& associative) {
    // Update self-model based on associations from neural learning
    // In a real implementation, this would integrate learned relationships
    // between actions and their consequences from the associative memory
    
    // For now, this is a placeholder for Phase 6 integration
    // Future implementation would:
    // 1. Retrieve associations involving action-related patterns
    // 2. Update action effects based on learned associations
    // 3. Refine body schema based on successful predictions
}

float SelfModel::getBodyAwareness() const {
    return pImpl->bodyAwareness_;
}

} // namespace nlm
