//include "SelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

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
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                     float targetReward) {
    // Generate possible action sequences
    auto sequences = generateActionSequences(planningDepth_);
    
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    // Evaluate each sequence
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState);
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    // Update confidence based on how consistent evaluations are
    if (!sequences.empty()) {
        planningConfidence_ = best.confidence;
    }
    
    // Return first action of best sequence
    if (!best.actions.empty()) {
        // Record success/failure based on whether we achieved target
        recentPlanSuccess_.push_back(best.expectedReward >= targetReward);
        if (recentPlanSuccess_.size() > 10) {
            recentPlanSuccess_.pop_front();
        }
        
        return best.actions[0];
    }
    
    // Default: wait if no good option
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
        
        // Evaluate this action
        float actionValue = evaluateAction(action, currentState);
        
        // Predict next state
        auto predictedNext = predictNextState(action, currentState);
        
        if (!predictedNext.empty()) {
            candidate.predictedStates.push_back(predictedNext);
            currentState = predictedNext;
        }
        
        // Get action quality from experience
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionQuality_.size()) {
            actionValue = actionQuality_[actionIdx] * 0.7f + actionValue * 0.3f;
        }
        
        totalReward += actionValue;
        
        // Reduce confidence if we're uncertain
        confidence *= getSelfModelConfidence(action);
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

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionQuality_.size()) return 0.0f;
    
    // Base value from experience
    float baseValue = actionQuality_[actionIdx];
    
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
    
    return baseValue * 0.7f + goalBonus * 0.3f;
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
    float capabilityLevel;
    float bodyAwareness;
    float agency;
    float confidence;
    
    // Action-effect associations for self-model
    std::unordered_map<uint64_t, std::vector<ActionEffect>> actionEffects;
    
    // Self-location tracking (for spatial representation)
    std::vector<NeuronId> bodyNeurons;
    std::pair<float, float> currentPosition;
    
    // Experience history for self-model updates
    std::vector<std::vector<float>> experienceBuffer;
    SimulationStep experienceBufferSize;
    
    Impl() : brain(nullptr), capabilityLevel(0.5f), bodyAwareness(0.5f), agency(0.5f), confidence(0.5f),
             experienceBufferSize(100) {
        actionEffects.reserve(10);  // 10 action types
    }
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
    , bodyAwareness_(0.5f)
    , agency_(0.5f)
    , confidence_(0.5f)
    , experienceBuffer_(100)
{
    NLM_LOG_INFO("SelfModel initialized for self-representation and experience tracking");
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with brain reference for better self-model updates
    if (brain) {
        // Connect to neuromodulation systems
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            // Integrate with dopamine for agency and motivation updates
        }
        
        // Connect to working memory for experience updates
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            // Integrate with working memory for self-model updates
        }
    }
    
    NLM_LOG_INFO("SelfModel initialized for self-representation and experience tracking");
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
    
    // Update capability level based on successful action execution
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
    
    // Update body awareness if this action involves movement
    if (isMovementAction(action)) {
        updateBodyAwareness(action, beforeState, afterState);
    }
    
    // Log self-model update
    NLM_LOG_INFO("SelfModel: Recorded action " + std::to_string(static_cast<int>(action)) +
                " (confidence: " + std::to_string(consistency) + ")");
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

void SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                            const std::vector<float>& afterState,
                                            ActionType action) const {
    // If we have a good prediction for this action, it's likely self-generated
    auto predicted = findMatchingEffect(action, beforeState);
    
    if (predicted.empty()) return;
    
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
    
    // Update overall capability based on prediction accuracy
    float predictionAccuracy = computeSimilarity(predicted, actual);
    capabilityLevel_ = capabilityLevel_ * 0.9f + predictionAccuracy * 0.1f;
}

float SelfModel::getBodyAwareness() const {
    return bodyAwareness_;
}

void SelfModel::updateBodyAwareness(ActionType action,
                                     const std::vector<float>& beforeState,
                                     const std::vector<float>& afterState) {
    // Body awareness is updated when we perform movement actions
    // and observe the consequences
    float stateChange = 0.0f;
    if (beforeState.size() == afterState.size()) {
        for (size_t i = 0; i < beforeState.size(); ++i) {
            stateChange += std::abs(afterState[i] - beforeState[i]);
        }
        stateChange /= beforeState.size();
    }
    
    bodyAwareness_ = bodyAwareness_ * 0.9f + stateChange * 0.1f;
    bodyAwareness_ = std::clamp(bodyAwareness_, 0.0f, 1.0f);
}

bool SelfModel::isMovementAction(ActionType action) const {
    switch (action) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
        case ActionType::LookLeft:
        case ActionType::LookRight:
            return true;
        default:
            return false;
    }
}

void SelfModel::recordExperience(const std::vector<float>& experience) {
    // Add experience to buffer
    pImpl->experienceBuffer.push_back(experience);
    
    // Maintain buffer size
    if (pImpl->experienceBuffer.size() > pImpl->experienceBufferSize) {
        pImpl->experienceBuffer.erase(pImpl->experienceBuffer.begin());
    }
    
    // Update capability level based on experience diversity and success
    updateCapabilityFromExperience();
}

void SelfModel::updateCapabilityFromExperience() {
    if (pImpl->experienceBuffer.empty()) return;
    
    // Compute capability level based on experience quality
    float totalCapability = 0.0f;
    size_t count = 0;
    
    for (const auto& experience : pImpl->experienceBuffer) {
        // Capability = variety of successful outcomes
        if (!experience.empty()) {
            float avgValue = 0.0f;
            for (float value : experience) {
                avgValue += value;
            }
            avgValue /= experience.size();
            
            // Successful experiences (high value) contribute more to capability
            if (avgValue > 0.5f) {
                totalCapability += avgValue;
                ++count;
            }
        }
    }
    
    if (count > 0) {
        float newCapability = totalCapability / count;
        // Smooth update
        capabilityLevel_ = capabilityLevel_ * 0.9f + newCapability * 0.1f;
    }
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

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    capabilityLevel_ = 0.5f;
    bodyAwareness_ = 0.5f;
    agency_ = 0.5f;
    confidence_ = 0.5f;
    
    // Clear experience buffer
    pImpl->experienceBuffer.clear();
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
    NLM_LOG_INFO("SocialLearning initialized for observation and imitation");
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with brain reference for better social learning integration
    if (brain) {
        // Connect to curiosity and novelty systems
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            // Integrate with curiosity for social exploration
        }
        
        // Connect to dopamine for reward prediction
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            // Integrate with dopamine for social reward learning
        }
    }
    
    NLM_LOG_INFO("SocialLearning initialized for observation and imitation");
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
    
    // Update social knowledge based on outcome
    updateSocialKnowledge(0.0f);  // Default reward since observer doesn't know it
    
    NLM_LOG_INFO("SocialLearning: Observed action " + std::to_string(static_cast<int>(observedAction)) +
                " (observations: " + std::to_string(observationCount_) + ")");
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
    
    NLM_LOG_INFO("SocialLearning: Learned communication signal (meaning: " + 
                std::to_string(signalMeaning_) + ")");
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
    
    // Update agency and social learning based on outcome
    if (observationCount_ > 0) {
        float avgReward = 0.0f;
        for (const auto& pair : observedActions_) {
            avgReward += pair.second.reward;
        }
        avgReward /= observationCount_;
        
        // High positive reward increases agency
        if (avgReward > 0.5f) {
            // Future enhancement: update agency levels
        }
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