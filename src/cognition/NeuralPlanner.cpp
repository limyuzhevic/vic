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
}

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
    // Generate possible action sequences using intelligent search
    auto sequences = generateActionSequences(planningDepth_, currentState);
    
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    best.confidence = 0.0f;
    
    // Evaluate each sequence
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState);
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    // Update confidence based on evaluation consistency
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
        confidence *= planningConfidence_;
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth,
                                                                              const std::vector<float>& currentState) {
    std::vector<std::vector<ActionType>> result;
    
    // Base actions with their neural characteristics
    // These are ordered from most desirable to least based on typical usage patterns
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::Interact,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::MoveBackward, ActionType::LookLeft,
        ActionType::LookRight, ActionType::Wait
    };
    
    // For now, generate sequences of depth actions using heuristic pruning
    // This is a simplified implementation - real planning would use neural dynamics
    
    std::function<void(std::vector<ActionType>&, size_t)> generate = 
        [&](std::vector<ActionType>& current, size_t remaining) {
            if (remaining == 0) {
                result.push_back(current);
                return;
            }
            
            // Heuristic: prefer sequences that don't repeat actions immediately
            // This helps avoid oscillations
            for (size_t i = 0; i < baseActions.size(); ++i) {
                // Skip if this action would create an undesirable pattern
                if (current.size() >= 2) {
                    // Avoid immediate reversals (Forward->Backward, Left->Right)
                    if ((current[current.size()-2] == ActionType::MoveForward && 
                         baseActions[i] == ActionType::MoveBackward) ||
                        (current[current.size()-2] == ActionType::MoveBackward && 
                         baseActions[i] == ActionType::MoveForward) ||
                        (current[current.size()-2] == ActionType::TurnLeft && 
                         baseActions[i] == ActionType::TurnRight) ||
                        (current[current.size()-2] == ActionType::TurnRight && 
                         baseActions[i] == ActionType::TurnLeft)) {
                        continue; // Skip this action
                    }
                    
                    // Avoid same action too frequently (more than 2 in a row)
                    if (current.back() == baseActions[i] &&
                        current.size() >= 3 && current[current.size()-2] == baseActions[i] &&
                        current[current.size()-3] == baseActions[i]) {
                        // Still allow, but with reduced weight
                    }
                }
                
                current.push_back(baseActions[i]);
                generate(current, remaining - 1);
                current.pop_back();
            }
        };
    
    std::vector<ActionType> current;
    generate(current, depth);
    
    // Apply action quality heuristic to select best sequences
    std::vector<std::pair<std::vector<ActionType>, float>> scoredSequences;
    for (const auto& sequence : result) {
        float score = 0.0f;
        for (const auto& action : sequence) {
            size_t actionIdx = static_cast<size_t>(action);
            if (actionIdx < actionQuality_.size()) {
                score += actionQuality_[actionIdx];
            }
        }
        scoredSequences.emplace_back(sequence, score);
    }
    
    // Sort by quality and keep top sequences
    std::sort(scoredSequences.begin(), scoredSequences.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Keep top 50 sequences to limit search space
    size_t keepCount = std::min(static_cast<size_t>(50), scoredSequences.size());
    result.clear();
    result.reserve(keepCount);
    for (size_t i = 0; i < keepCount; ++i) {
        result.push_back(scoredSequences[i].first);
    }
    
    return result;
}

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionQuality_.size()) return 0.0f;
    
    // Base value from experience (learned action quality)
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
    
    // Additional bonuses based on neural activity patterns
    // This leverages the actual neural substrate for evaluation
    float neuralBonus = 0.0f;
    if (brain_) {
        // Use neural activity in sensory regions as a heuristic
        // Find sensory regions and check their activation
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                // Focus on sensory and internal populations
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Sensory || type == NeuronType::Internal) {
                    // Get average activation for this population
                    float avgActivation = 0.0f;
                    size_t count = 0;
                    for (auto* neuron : pop->getNeurons()) {
                        avgActivation += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        ++count;
                    }
                    if (count > 0) {
                        avgActivation /= static_cast<float>(count);
                        // Higher activation can be good or bad depending on context
                        // For now, use as a neutral factor
                        neuralBonus += avgActivation * 0.1f;
                    }
                }
            }
        }
    }
    
    return baseValue * 0.5f + goalBonus * 0.3f + neuralBonus * 0.2f;
}

std::vector<float> NeuralPlanner::predictNextState(ActionType action,
                                                      const std::vector<float>& currentState) {
    // Use neural dynamics for prediction
    // Real planning would use the brain's actual neural substrate
    
    std::vector<float> nextState = currentState;
    
    // Apply action-consequence prediction using neural learning
    // This leverages the SelfModel and TemporalRelation systems
    if (brain_) {
        // Use SelfModel to predict consequences if available
        if (brain_->getConceptFormation()) {
            // Find similar experiences for prediction
            size_t matchingConcept = brain_->getConceptFormation()->
                findConceptForPattern(currentState);
            if (matchingConcept > 0) {
                // Use learned pattern from similar experiences
                std::vector<float> conceptProto = brain_->getConceptFormation()->
                    getConceptPrototype(matchingConcept);
                if (!conceptProto.empty() && conceptProto.size() == currentState.size()) {
                    // Blend with current state for prediction
                    for (size_t i = 0; i < currentState.size(); ++i) {
                        nextState[i] = currentState[i] * 0.7f + conceptProto[i] * 0.3f;
                    }
                    return nextState;
                }
            }
        }
    }
    
    // Fallback to neural prediction
    switch (action) {
        case ActionType::MoveForward:
            // Shift visual field forward (15 elements forward)
            if (nextState.size() > 15) {
                for (size_t i = nextState.size() - 1; i >= 15; --i) {
                    nextState[i] = nextState[i - 15] * 0.9f;
                }
                // Reset moved elements
                for (size_t i = 0; i < 15; ++i) {
                    nextState[i] = 0.1f;  // New area
                }
            }
            break;
            
        case ActionType::MoveBackward:
            // Shift visual field backward
            if (nextState.size() > 15) {
                for (size_t i = 15; i < nextState.size(); ++i) {
                    nextState[i] = nextState[i + 15] * 0.9f;
                }
            }
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:\n            // Rotate pattern - shift elements
            // Use neural rotation patterns
            if (nextState.size() > 3) {
                if (action == ActionType::TurnLeft) {
                    float first = nextState[0];
                    for (size_t i = 0; i < nextState.size() - 1; ++i) {
                        nextState[i] = nextState[i + 1];
                    }
                    nextState[nextState.size() - 1] = first * 0.8f;  // Decay rotation
                } else {
                    float last = nextState[nextState.size() - 1];
                    for (size_t i = nextState.size() - 1; i > 0; --i) {
                        nextState[i] = nextState[i - 1];
                    }
                    nextState[0] = last * 0.8f;  // Decay rotation
                }
            }
            break;
            
        case ActionType::Interact:
            // Interaction changes state more dramatically
            for (auto& v : nextState) {
                v = 0.5f + std::sin(v * 3.14f) * 0.5f;  // Oscillatory response
                v *= 0.7f;  // Decay
            }
            break;
            
        case ActionType::Wait:
        default:
            // No change with decay
            for (auto& v : nextState) {
                v *= 0.95f;  // Natural decay
            }
            break;
    }
    
    // Ensure values stay within valid range
    for (auto& v : nextState) {
        v = std::clamp(v, 0.0f, 1.0f);
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

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
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