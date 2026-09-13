#include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

struct SelfModel::Impl {
    Brain* brain;
    std::mt19937 rng;
    SimulationStep lastDecayStep;
    
    Impl() : brain(nullptr), rng(std::random_device()()), lastDecayStep(0) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
    , enabled_(true)
    , learningRate_(0.1f)
    , stateSize_(16)  // Default state size
    , lastCleanupStep_(0)
{
    actionEffects_.resize(static_cast<size_t>(ActionType::Count), std::vector<ActionEffect>());
    predictionRecords_.resize(static_cast<size_t>(ActionType::Count), PredictionRecord());
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Estimate state size from brain's sensory systems
    if (brain_) {
        // Get total sensory input size from agent's sensory percept
        // This provides a reasonable estimate of state representation
        stateSize_ = 256;  // Default based on typical sensory input sizes
        
        // Initialize random number generator with seed from brain config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = brain->getConfig()->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        pImpl->rng.seed(seed);
    }
    
    NLM_LOG_INFO("SelfModel initialized with state size: " + std::to_string(stateSize_));
}

void SelfModel::recordSelfAction(ActionType action,
                               const std::vector<float>& beforeState,
                               const std::vector<float>& afterState) {
    if (!enabled_ || beforeState.empty() || afterState.empty()) return;
    
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) {
        actionEffects_.resize(actionIdx + 1, std::vector<ActionEffect>());
        predictionRecords_.resize(actionIdx + 1, PredictionRecord());
    }
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.confidence = 1.0f;
    effect.observationCount = 1;
    
    // Calculate effect size for weighting
    float effectSize = 0.0f;
    for (size_t i = 0; i < beforeState.size() && i < afterState.size(); ++i) {
        effectSize += std::abs(afterState[i] - beforeState[i]);
    }
    effect.effectSize = effectSize;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Update prediction accuracy for this action
    if (predictionRecords_[actionIdx].lastUpdated > 0) {
        // We had a prediction, now we have actual outcome
        // This would update prediction accuracy in a real implementation
        predictionRecords_[actionIdx].accuracy = 0.9f;  // Simplified
        predictionRecords_[actionIdx].error = 0.05f;   // Simplified
    }
    
    // Update capability level based on learning
    updateCapabilityLevel(0.05f, 0.9f);
    
    NLM_LOG_DEBUG("SelfModel: Recorded effect for action " + std::to_string(static_cast<int>(action)));
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                      const std::vector<float>& currentState) {
    if (!enabled_ || !hasSelfModel() || currentState.empty()) {
        return std::vector<float>(stateSize_, 0.0f);
    }
    
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size() || actionEffects_[actionIdx].empty()) {
        // No effects recorded for this action
        return std::vector<float>(stateSize_, 0.0f);
    }
    
    // Find the best matching effect for the current state
    std::vector<float> predictedEffect = findBestMatchingEffect(action, currentState);
    
    // Apply the effect to current state
    std::vector<float> predictedState = currentState;
    for (size_t i = 0; i < currentState.size() && i < predictedEffect.size(); ++i) {
        predictedState[i] += predictedEffect[i];
    }
    
    // Clamp values to reasonable range
    for (size_t i = 0; i < predictedState.size(); ++i) {
        predictedState[i] = std::max(0.0f, std::min(predictedState[i], 1.0f));
    }
    
    return predictedState;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    if (actionEffects_[actionIdx].empty()) {
        return 0.0f;  // No experience with this action
    }
    
    // Calculate confidence based on number of observations and their consistency
    float confidence = 0.0f;
    size_t count = 0;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        confidence += effect.confidence;
        count++;
    }
    
    if (count > 0) {
        confidence /= count;
    }
    
    // Adjust based on prediction accuracy
    if (predictionRecords_[actionIdx].lastUpdated > 0) {
        confidence *= predictionRecords_[actionIdx].accuracy;
    }
    
    return std::min(confidence, 1.0f);
}

float SelfModel::computeSelfGeneratedLikelihood(const std::vector<float>& beforeState,
                                               const std::vector<float>& afterState,
                                               ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    float maxLikelihood = 0.0f;
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        // Compute similarity between observed effect and current effect
        float similarity = 0.0f;
        size_t comparisons = 0;
        
        for (size_t i = 0; i < beforeState.size() && i < effect.beforeState.size(); ++i) {
            if (std::abs(afterState[i] - effect.beforeState[i]) < 0.1f) {
                similarity += 1.0f;
            }
            comparisons++;
        }
        
        if (comparisons > 0) {
            similarity /= comparisons;
            maxLikelihood = std::max(maxLikelihood, similarity);
        }
    }
    
    return maxLikelihood;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    if (!enabled_ || !hasSelfModel()) return ActionType::Wait;
    
    // Convert state to string key for body schema lookup
    std::string stateKey;
    for (float val : state) {
        stateKey += std::to_string(val) + ",";
    }
    
    // Check body schema
    auto it = bodySchema_.find(stateKey);
    if (it != bodySchema_.end()) {
        return it->second;
    }
    
    // If not found, determine preferred action based on most effective action in this state
    std::unordered_map<ActionType, float> actionScores;
    
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
        ActionType action = static_cast<ActionType>(i);
        float score = 0.0f;
        
        for (const auto& effect : actionEffects_[i]) {
            // Compute similarity to current state
            float similarity = 0.0f;
            for (size_t j = 0; j < state.size() && j < effect.beforeState.size(); ++j) {
                similarity += std::abs(state[j] - effect.beforeState[j]);
            }
            if (state.size() > 0) similarity /= state.size();
            similarity = 1.0f - std::min(similarity, 1.0f);
            
            // Score based on confidence and effect size
            score += similarity * effect.confidence * effect.effectSize;
        }
        
        if (score > 0.0f) {
            actionScores[action] = score;
        }
    }
    
    if (actionScores.empty()) {
        return ActionType::Wait;
    }
    
    // Return action with highest score
    ActionType bestAction = ActionType::Wait;
    float bestScore = 0.0f;
    
    for (const auto& pair : actionScores) {
        if (pair.second > bestScore) {
            bestScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    // Store in body schema for future use
    bodySchema_[stateKey] = bestAction;
    
    return bestAction;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    if (!enabled_) return;
    
    size_t actionIdx = static_cast<size_t>(action);
    
    // Calculate prediction error
    float error = calculatePredictionError(predicted, actual);
    
    // Update prediction record
    if (predictionRecords_[actionIdx].lastUpdated > 0) {
        predictionRecords_[actionIdx].error = error;
        predictionRecords_[actionIdx].accuracy = std::max(0.0f, 1.0f - error);
    }
    
    predictionRecords_[actionIdx].lastUpdated = pImpl->brain ? pImpl->brain->getStepCount() : 0;
    
    // If error is high, adjust the action effect
    if (error > 0.3f) {
        // Reduce confidence in this effect
        for (auto& effect : actionEffects_[actionIdx]) {
            effect.confidence *= (1.0f - learningRate_ * error);
        }
    }
    
    // Update capability level based on performance
    updateCapabilityLevel(error, 1.0f - error);
}

float SelfModel::calculatePredictionError(const std::vector<float>& predicted,
                                         const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;  // Maximum error for invalid data
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    float mse = sumSquaredError / predicted.size();
    
    // Convert to error in [0, 1] range
    return std::min(1.0f, std::sqrt(mse) * 10.0f);
}

void SelfModel::updateCapabilityLevel(float error, float confidence) {
    // Update capability level based on learning performance
    float newCapability = capabilityLevel_ + learningRate_ * (confidence - error) * (1.0f - capabilityLevel_);
    capabilityLevel_ = std::max(0.1f, std::min(0.99f, newCapability));
}

std::vector<float> SelfModel::findBestMatchingEffect(ActionType action,
                                                     const std::vector<float>& currentState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size() || actionEffects_[actionIdx].empty()) {
        return std::vector<float>(stateSize_, 0.0f);
    }
    
    float bestScore = 0.0f;
    std::vector<float> bestEffect(stateSize_, 0.0f);
    
    for (const auto& effect : actionEffects_[actionIdx]) {
        float score = 0.0f;
        
        for (size_t i = 0; i < currentState.size() && i < effect.beforeState.size(); ++i) {
            // Higher similarity = higher score
            float diff = std::abs(currentState[i] - effect.beforeState[i]);
            float similarity = 1.0f - std::min(diff, 1.0f);
            score += similarity;
        }
        
        if (currentState.size() > 0) {
            score /= currentState.size();
        }
        
        score *= effect.confidence * effect.effectSize;
        
        if (score > bestScore) {
            bestScore = score;
            bestEffect = effect.afterState;
            if (bestScore > 0.8f) break;  // Good enough match
        }
    }
    
    return bestEffect;
}

bool SelfModel::isReadyForPrediction() const {
    if (!hasSelfModel()) return false;
    
    // Check if we have enough observations for each action type
    size_t totalObservations = 0;
    for (const auto& effects : actionEffects_) {
        totalObservations += effects.size();
    }
    
    // Need at least some observations per action type
    bool ready = totalObservations >= 20 && capabilityLevel_ > 0.3f;
    
    NLM_LOG_DEBUG("SelfModel readiness check: " + std::to_string(totalObservations) +
                 " observations, capability: " + std::to_string(capabilityLevel_) +
                 " -> " + std::string(ready ? "READY" : "NOT READY"));
    
    return ready;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    
    for (auto& record : predictionRecords_) {
        record.accuracy = 0.0f;
        record.error = 0.0f;
        record.lastUpdated = 0;
    }
    
    bodySchema_.clear();
    capabilityLevel_ = 0.5f;
    
    NLM_LOG_INFO("SelfModel cleared");
}

float SelfModel::getBodyAwareness() const {
    if (!hasSelfModel()) return 0.0f;
    
    // Calculate body awareness based on prediction accuracy and consistency
    float totalAccuracy = 0.0f;
    float totalError = 0.0f;
    size_t validRecords = 0;
    
    for (const auto& record : predictionRecords_) {
        if (record.lastUpdated > 0) {
            totalAccuracy += record.accuracy;
            totalError += record.error;
            validRecords++;
        }
    }
    
    if (validRecords == 0) return 0.0f;
    
    float avgAccuracy = totalAccuracy / validRecords;
    float avgError = totalError / validRecords;
    
    // Body awareness is high when predictions are accurate and errors are small
    float awareness = (avgAccuracy + (1.0f - avgError)) * 0.5f;
    
    return std::min(1.0f, awareness);
}

std::string SelfModel::exportSelfModel() const {
    std::stringstream ss;
    
    ss << "SelfModel Export\n";
    ss << "================\n\n";
    
    ss << "State Size: " << stateSize_ << "\n";
    ss << "Capability Level: " << capabilityLevel_ << "\n";
    ss << "Enabled: " << (enabled_ ? "true" : "false") << "\n";
    ss << "Learning Rate: " << learningRate_ << "\n\n";
    
    ss << "Action Effects:\n";
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
        if (actionEffects_[i].empty()) continue;
        
        ss << "  Action " << static_cast<int>(static_cast<ActionType>(i)) << ": "
           << actionEffects_[i].size() << " effects\n";
        
        for (const auto& effect : actionEffects_[i]) {
            ss << "    - Confidence: " << effect.confidence << ", Effect Size: " 
               << effect.effectSize << ", Observations: " << effect.observationCount << "\n";
        }
    }
    
    ss << "\nPrediction Records:\n";
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
        if (predictionRecords_[i].lastUpdated > 0) {
            ss << "  Action " << static_cast<int>(static_cast<ActionType>(i)) << ": "
               << "Accuracy: " << predictionRecords_[i].accuracy << ", Error: " 
               << predictionRecords_[i].error << "\n";
        }
    }
    
    ss << "\nBody Schema Entries: " << bodySchema_.size() << "\n";
    
    return ss.str();
}

bool SelfModel::importSelfModel(const std::string& data) {
    // For now, this is a placeholder - could parse JSON or binary format
    // In a real implementation, this would load saved self-model state
    
    NLM_LOG_WARNING("SelfModel import not yet implemented - placeholder implementation");
    
    // For now, just reset to clean state
    clear();
    
    return true;
}

std::vector<SelfModel::ActionEffect> SelfModel::getRecentEffects(ActionType action, size_t limit) const {
    std::vector<ActionEffect> recentEffects;
    
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return recentEffects;
    
    size_t count = std::min(limit, actionEffects_[actionIdx].size());
    recentEffects.reserve(count);
    
    // Get most recent effects (assuming effects are added to end)
    size_t startIdx = actionEffects_[actionIdx].size() - count;
    for (size_t i = startIdx; i < actionEffects_[actionIdx].size(); ++i) {
        recentEffects.push_back(actionEffects_[actionIdx][i]);
    }
    
    return recentEffects;
}

float SelfModel::getAveragePredictionAccuracy() const {
    float totalAccuracy = 0.0f;
    size_t validRecords = 0;
    
    for (const auto& record : predictionRecords_) {
        if (record.lastUpdated > 0) {
            totalAccuracy += record.accuracy;
            validRecords++;
        }
    }
    
    return validRecords > 0 ? totalAccuracy / validRecords : 0.0f;
}

float SelfModel::getTypicalPredictionError() const {
    float totalError = 0.0f;
    size_t validRecords = 0;
    
    for (const auto& record : predictionRecords_) {
        if (record.lastUpdated > 0) {
            totalError += record.error;
            validRecords++;
        }
    }
    
    return validRecords > 0 ? totalError / validRecords : 0.0f;
}

size_t SelfModel::getTotalObservations() const {
    size_t total = 0;
    for (const auto& effects : actionEffects_) {
        total += effects.size();
    }
    return total;
}

} // namespace nlm
