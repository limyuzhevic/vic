#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
{
    actionQuality_.resize(10, 0.0f);  // 10 action types
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
        pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralPlanner initialized");
    
    // Integrate with prediction system for forward planning
    if (brain_->getPredictionSystem()) {
        // Use prediction system outputs in planning
        NLM_LOG_INFO("NeuralPlanner integrated with prediction system");
        
        // Enhanced integration with prediction system for biologically plausible planning
        brain_->getPredictionSystem()->initialize(brain_);
    }
    
    // Integrate with self-model for better predictions
    // SelfModel is part of the brain's cognition systems
    // Integrate with social learning for social action planning
    // SocialLearning is part of the brain's cognition systems
    NLM_LOG_INFO("NeuralPlanner integrated with self-model and social learning");
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                     float targetReward) {
    NLM_LOG_DEBUG("NeuralPlanner planning action, targetReward: " + std::to_string(targetReward));
    
    // Create integrated state vector from brain components
    std::vector<float> integratedState = createIntegratedState(currentState);
    
    // Generate possible action sequences
    auto sequences = generateActionSequences(planningDepth_);
    
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    // Evaluate each sequence with neuromodulation effects
    float dopamineLevel = 0.0f;
    if (brain_->getDopamine()) {
        dopamineLevel = brain_->getDopamine()->getLevel();
    }
    
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, integratedState);
        
        // Apply dopamine modulation to confidence
        if (dopamineLevel > 0.0f) {
            float confidenceMod = 1.0f + dopamineLevel * 0.5f; // Higher dopamine = higher confidence
            candidate.confidence *= confidenceMod;
        }
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    // Update confidence based on how consistent evaluations are and neuromodulation
    if (!sequences.empty()) {
        // Blend with working memory state for stability
        if (brain_->getWorkingMemory()) {
            float memoryStability = std::min(1.0f, brain_->getWorkingMemory()->getActiveTraces() * 0.1f);
            planningConfidence_ = planningConfidence_ * 0.7f + best.confidence * 0.3f + memoryStability * 0.1f;
        } else {
            planningConfidence_ = best.confidence;
        }
        
        // Apply neuromodulation to final confidence
        if (dopamineLevel > 0.0f) {
            planningConfidence_ *= (1.0f + dopamineLevel * 0.3f);
            planningConfidence_ = std::min(1.0f, planningConfidence_);
        }
    }
    
    // Return first action of best sequence
    if (!best.actions.empty()) {
        // Record success/failure based on whether we achieved target
        recentPlanSuccess_.push_back(best.expectedReward >= targetReward);
        if (recentPlanSuccess_.size() > 10) {
            recentPlanSuccess_.pop_front();
        }
        
        // Use attention system to modulate action selection
        if (brain_->getAttention() && planningConfidence_ < 0.6f) {
            // Low confidence - increase attention to sensory inputs
            brain_->getAttention()->applyBottomUpSalience(
                static_cast<NeuronId>(0), // First motor neuron
                (1.0f - planningConfidence_) * 2.0f
            );
        }
        
        NLM_LOG_DEBUG("Selected action: " + std::to_string(static_cast<int>(best.actions[0])) + 
                     " with confidence: " + std::to_string(best.confidence));
        
        return best.actions[0];
    }
    
    // Default: wait if no good option
    NLM_LOG_DEBUG("No good action sequence found, defaulting to WAIT");
    return ActionType::Wait;
}

PlanningCandidate NeuralPlanner::evaluateSequence(const std::vector<ActionType>& actions,
                                                  const std::vector<float>& startState) {
    PlanningCandidate candidate;
    candidate.actions = actions;
    candidate.depth = actions.size();
    
    std::vector<float> currentState = startState;
    float totalReward = 0.0f;
    float confidence = 1.0f;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        ActionType action = actions[i];
        
        // Use self-model predictions if available for more accurate next state prediction
        std::vector<float> predictedNext;
        if (brain_ && planningConfidence_ > 0.5f) {
            // Try to get self-model prediction for better forward model
            if (brain_->getSelfModel()) {
                // Use SelfModel to predict sensory consequences
                for (size_t j = 0; j < integratedState.size(); ++j) {
                    ActionType action = actions[0];  // Simplified for single action evaluation
                    predictedNext = brain_->getSelfModel()->predictActionConsequence(
                        action, integratedState);
                    if (!predictedNext.empty()) {
                        break;
                    }
                }
            } else if (brain_->getPredictionSystem()) {
                // Fallback to prediction system if SelfModel not available
                predictedNext = predictNextState(action, currentState);
            } else {
                // Use prediction system if available
                predictedNext = predictNextState(action, currentState);
            }
            
            // Enhance prediction with prediction system confidence
            float predConfidence = brain_->getPredictionSystem() ? 
                brain_->getPredictionSystem()->getConfidence() : 0.0f;
            if (predConfidence > 0.0f) {
                for (auto& val : predictedNext) {
                    val *= (0.5f + predConfidence * 0.5f);
                    val = std::clamp(val, 0.0f, 1.0f);
                }
            }
        } else if (brain_ && brain_->getPredictionSystem()) {
            // Use prediction system if confidence is low
            predictedNext = predictNextState(action, currentState);
        }
        
        if (!predictedNext.empty()) {
            candidate.predictedStates.push_back(predictedNext);
            currentState = predictedNext;
        } else {
            // Fallback to simple prediction
            auto fallbackNext = predictNextState(action, currentState);
            if (!fallbackNext.empty()) {
                candidate.predictedStates.push_back(fallbackNext);
                currentState = fallbackNext;
            }
        }
        
        // Get action quality from experience
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionQuality_.size()) {
            float actionValue = actionQuality_[actionIdx] * 0.7f + evaluateAction(action, currentState) * 0.3f;
            totalReward += actionValue;
        } else {
            float actionValue = evaluateAction(action, currentState);
            totalReward += actionValue;
        }
        
        // Reduce confidence if we're uncertain, using both self-model and neuromodulators
        float uncertainty = 1.0f;
        
        // Get self-model confidence if available
        if (brain_) {
            uncertainty *= getSelfModelConfidence(action);
        }
        
        // Apply neuromodulation effects on planning confidence
        if (brain_ && brain_->getDopamine()) {
            float dopamineLevel = brain_->getDopamine()->getLevel();
            // High dopamine = lower uncertainty (more confident)
            uncertainty *= (1.0f - dopamineLevel * 0.3f);
        }
        
        confidence *= uncertainty;
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> result;
    
    // Simple enumeration of action sequences
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // For now, generate sequences of depth actions
    // This could be optimized with pruning
    
    std::function<void(std::vector<ActionType>&, size_t)> generate = 
        [&](std::vector<ActionType>& current, size_t remaining) {
            if (remaining == 0) {
                result.push_back(current);
                return;
            }
            
            for (ActionType action : baseActions) {
                current.push_back(action);
                generate(current, remaining - 1);
                current.pop_back();
            }
        };
    
    std::vector<ActionType> current;
    generate(current, depth);
    
    // Limit number of sequences
    if (result.size() > 100) {
        result.resize(100);
    }
    
    return result;
}

// Helper method to create integrated state from multiple brain systems
std::vector<float> NeuralPlanner::createIntegratedState(const std::vector<float>& baseState) {
    std::vector<float> integratedState = baseState;
    
    if (!brain_) return integratedState;
    
    // Integrate working memory
    if (brain_->getWorkingMemory()) {
        auto wmState = brain_->getWorkingMemory()->retrieve();
        if (!wmState.empty()) {
            // Blend working memory with current state
            for (size_t i = 0; i < std::min(integratedState.size(), wmState.size()); ++i) {
                integratedState[i] = integratedState[i] * 0.7f + wmState[i] * 0.3f;
            }
        }
    }
    
    // Integrate attention system
    if (brain_->getAttention()) {
        const auto& winners = brain_->getAttention()->getWinners();
        if (!winners.empty() && !integratedState.empty()) {
            // Boost attention-modulated regions
            for (size_t i = 0; i < std::min(integratedState.size(), static_cast<size_t>(winners.size())); ++i) {
                if (brain_->getAttention()->isAttended(winners[i])) {
                    integratedState[i] *= 1.2f;
                }
            }
        }
    }
    
    // Integrate prediction system confidence
    if (brain_->getPredictionSystem()) {
        float predictionConfidence = brain_->getPredictionSystem()->getConfidence();
        if (predictionConfidence > 0.0f) {
            // Scale integrated state based on prediction confidence
            float scale = 0.5f + predictionConfidence * 0.5f;
            for (auto& value : integratedState) {
                value *= scale;
                value = std::clamp(value, 0.0f, 1.0f);
            }
        }
    }
    
    return integratedState;
}

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionQuality_.size()) return 0.0f;
    
    // Base value from experience
    float baseValue = actionQuality_[actionIdx];
    
    // Integrate with prediction system for context-aware evaluation
    float predictionContext = 1.0f;
    if (brain_ && brain_->getPredictionSystem()) {
        float predConfidence = brain_->getPredictionSystem()->getConfidence();
        predictionContext = 0.5f + predConfidence * 0.5f;
    }
    
    // Evaluate action with social learning influence
    float socialInfluence = 1.0f;
    if (brain_ && brain_->getSocialLearning() && brain_->getSocialLearning()->hasSocialKnowledge()) {
        // Social learning can influence action evaluation through imitation
        // Get best imitation action for current state
        ActionType imitationAction = brain_->getSocialLearning()->getImitationAction(currentState);
        
        // Blend current action with socially preferred action
        if (imitationAction != action && imitationAction != ActionType::Wait) {
            // Calculate social similarity
            float socialSimilarity = brain_->getSocialLearning()->computeSimilarity(
                currentState, std::vector<float>(currentState.size(), 0.5f)); // Simplified
            
            // Increase action value if it matches social preference
            if (socialSimilarity > 0.7f) {
                socialInfluence = 1.2f;  // 20% boost for socially congruent actions
            }
        }
        
        // Apply neuromodulation to social influence
        if (brain_->getDopamine()) {
            float dopamineLevel = brain_->getDopamine()->getLevel();
            // High dopamine = stronger social learning influence
            socialInfluence *= (1.0f + dopamineLevel * 0.3f);
        }
    }
    
    // Apply social influence to action evaluation
    actionValue *= socialInfluence;
    
    // Bonus for approaching goal (if goal is defined)
    float goalBonus = 0.0f;
    if (!currentGoal_.empty() && state.size() == currentGoal_.size()) {
        float stateGoalSim = 0.0f, stateNorm = 0.0f, goalNorm = 0.0f;
        for (size_t i = 0; i < state.size(); ++i) {
            stateGoalSim += state[i] * currentGoal_[i];
            stateNorm += state[i] * state[i];
            goalNorm += currentGoal_[i] * currentGoal_[i];
        }
        if (stateNorm > 0.0001f && goalNorm > 0.0001f) {
            goalBonus = stateGoalSim / (std::sqrt(stateNorm) * std::sqrt(goalNorm));
        }
    }
    
    // Apply neuromodulatory modulation to evaluation
    float neuromodulationMod = 1.0f;
    if (brain_ && brain_->getDopamine()) {
        float dopamineLevel = brain_->getDopamine()->getLevel();
        // High dopamine = higher evaluation of potential reward
        neuromodulationMod = 1.0f + dopamineLevel * 0.3f;
    }
    
    return baseValue * 0.6f * predictionContext * attentionContext * neuromodulationMod +
           goalBonus * 0.4f;
}

std::vector<float> NeuralPlanner::predictNextState(ActionType action,
                                                  const std::vector<float>& currentState) {
    // Use simple action-consequence prediction
    // In a full implementation, this would use learned forward models
    
    std::vector<float> nextState = currentState;
    
    // Apply expected effect based on action
    // This is a simplified model - real implementation would use experience
    switch (action) {
        case ActionType::MoveForward:
            // Shift visual field forward
            if (nextState.size() > 10) {
                for (size_t i = 10; i < nextState.size(); ++i) {
                    nextState[i-10] = nextState[i] * 0.9f;
                }
            }
            break;
        case ActionType::MoveBackward:
            if (nextState.size() > 10) {
                for (size_t i = nextState.size() - 1; i >= 10; --i) {
                    nextState[i] = nextState[i-10] * 0.9f;
                }
            }
            break;
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Rotate pattern
            // Simplified: just add some noise
            for (auto& v : nextState) {
                v += 0.05f * (v > 0.5f ? -1.0f : 1.0f);
                v = std::clamp(v, 0.0f, 1.0f);
            }
            break;
        case ActionType::Interact:
            // Interaction changes state significantly
            for (auto& v : nextState) {
                v = v > 0.5f ? 0.8f : 0.2f;
            }
            break;
        case ActionType::Wait:
        default:
            // No change
            break;
    }
    
    return nextState;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                     const std::vector<ActionType>& actualActions,
                                     float actualReward) {
    // Update action quality based on how well plan worked
    size_t minLen = std::min(plannedActions.size(), actualActions.size());
    
    for (size_t i = 0; i < minLen; ++i) {
        size_t actionIdx = static_cast<size_t>(actualActions[i]);
        if (actionIdx < actionQuality_.size()) {
            // If action matched plan, positive update; else negative
            float delta = (actualActions[i] == plannedActions[i]) ? 0.1f : -0.05f;
            delta *= actualReward;
            
            actionQuality_[actionIdx] = std::clamp(
                actionQuality_[actionIdx] + delta, -1.0f, 1.0f);
        }
    }
}

void NeuralPlanner::clearCache() {
    // Clear any planning cache
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (recentPlanSuccess_.empty()) return true;  // No data
    
    size_t successCount = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) ++successCount;
    }
    
    return successCount > recentPlanSuccess_.size() / 2;
}

// SelfModel Implementation
struct SelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
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
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.observationCount = 1;
    
    // Compute confidence as consistency with previous observations
    float consistency = 0.5f;
    for (const auto& prev : actionEffects_[actionIdx]) {
        if (computeSimilarity(beforeState, prev.beforeState) > 0.8f) {
            consistency = std::max(consistency, 
                computeSimilarity(afterState, prev.afterState));
            effect.observationCount += prev.observationCount;
        }
    }
    effect.confidence = consistency;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Update capability level
    float totalConfidence = 0.0f;
    size_t count = 0;
    for (const auto& effects : actionEffects_) {
        for (const auto& e : effects) {
            totalConfidence += e.confidence;
            ++count;
        }
    }
    if (count > 0) {
        capabilityLevel_ = totalConfidence / count;
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) {
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

float SelfModel::computeSelfGenerated Likeness(const std::vector<float>& beforeState,
                                              const std::vector<float>& afterState,
                                              ActionType action) const {
    // If we have a good prediction for this action, it's likely self-generated
    auto predicted = findMatchingEffect(action, beforeState);
    
    if (predicted.empty()) return 0.0f;
    
    float similarity = computeSimilarity(predicted, afterState);
    return similarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    ActionType best = ActionType::Wait;
    float bestValue = -1000.0f;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        if (actionEffects_[i].empty()) continue;
        
        // Check how well this action would work in current state
        auto predicted = findMatchingEffect(static_cast<ActionType>(i), state);
        if (!predicted.empty()) {
            // Value = how much the state changes toward reward
            float value = computeSimilarity(predicted, state);
            if (value > bestValue) {
                bestValue = value;
                best = static_cast<ActionType>(i);
            }
        }
    }
    
    return best;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    // Find and update matching effect
    for (auto& effect : actionEffects_[actionIdx]) {
        if (computeSimilarity(effect.beforeState, predicted) > 0.5f) {
            // Update with prediction error
            float error = 1.0f - computeSimilarity(predicted, actual);
            
            // Adjust confidence based on error
            effect.confidence *= (1.0f - error * 0.1f);
            effect.confidence = std::max(0.1f, effect.confidence);
        }
    }
}

float SelfModel::getBodyAwareness() const {
    return capabilityLevel_;
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
    if (actionIdx >= actionEffects_.size()) return {};
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return {};
    
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& effect : effects) {
        float sim = computeSimilarity(beforeState, effect.beforeState);
        if (sim > bestSim) {
            bestSim = sim;
            bestPrediction = effect.afterState;
        }
    }
    
    return bestPrediction;
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

// SocialLearning Implementation
struct SocialLearning::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.0f)
{
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                       const std::vector<float>& observerState,
                                       const std::vector<float>& resultingState) {
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Observer doesn't know reward
    
    observedActions_.push_back({observedAction, effect});
    ++observationCount_;
    
    // Keep memory bounded
    if (observedActions_.size() > 500) {
        observedActions_.erase(observedActions_.begin());
    }
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've seen this action before and know its effect
    for (const auto& pair : observedActions_) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    ActionType best = ActionType::Wait;
    float bestSim = 0.0f;
    
    for (const auto& pair : observedActions_) {
        float sim = computeSimilarity(currentState, pair.second.state);
        if (sim > bestSim) {
            bestSim = sim;
            best = pair.first;
        }
    }
    
    return best;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                              float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (signalPattern_.empty()) return false;
    
    float similarity = computeSimilarity(neuralPattern, signalPattern_);
    return similarity > 0.7f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update value of observed actions based on whether interaction was beneficial
    for (auto& pair : observedActions_) {
        // Adjust reward estimate based on interaction outcome
        pair.second.reward = pair.second.reward * 0.9f + interactionReward * 0.1f;
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
    observationCount_ = 0;
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
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