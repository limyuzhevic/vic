#include "MotorSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct MotorSystem::Impl {
    Brain* brain;
    
    // Action preference learned through experience
    std::unordered_map<ActionType, float> actionPreferences_;
    
    // Action execution statistics
    std::unordered_map<ActionType, size_t> executionCounts_;
    std::unordered_map<ActionType, float> successRates_;
    std::unordered_map<ActionType, float> cumulativeRewards_;
    std::unordered_map<ActionType, size_t> lastExecutionSteps_;
    
    // Action learning memory
    struct Experience {
        SimulationStep step;
        ActionType action;
        std::vector<float> state;
        float reward;
        float confidence;
    };
    std::vector<Experience> experienceMemory_;
    
    // Recent action history for diversity and novelty
    std::deque<ActionType> recentActions_;
    
    // Exploration settings
    float explorationRate_;
    bool explorationEnabled_;
    
    // Action type enabled/disabled
    std::unordered_map<ActionType, bool> actionTypesEnabled_;
    
    // Reward modulation for specific actions
    std::unordered_map<ActionType, float> actionRewardMods_;
    
    // Action learning enabled
    bool actionLearningEnabled_;
    
    // Random number generator for exploration
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
    std::bernoulli_distribution bernoulliDist_;
    
    // Neuromodulation influence
    float neuromodulationInfluence_;
    
    // Developmental stage influence on action selection
    float developmentalActionBias_;
    
    Impl() : brain(nullptr)
        , explorationRate_(0.1f)
        , explorationEnabled_(true)
        , actionLearningEnabled_(true)
        , neuromodulationInfluence_(1.0f)
        , developmentalActionBias_(1.0f) {
        
        // Initialize random number generator with seed
        std::random_device rd;
        rng_ = std::mt19937(rd());
        uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
        bernoulliDist_ = std::bernoulli_distribution(0.5f);
        
        // Initialize action preferences
        for (int i = 0; i < 8; ++i) {
            ActionType action = static_cast<ActionType>(i);
            actionPreferences_[action] = 1.0f; // Equal preference initially
            executionCounts_[action] = 0;
            successRates_[action] = 0.5f;
            cumulativeRewards_[action] = 0.0f;
            lastExecutionSteps_[action] = 0;
            actionTypesEnabled_[action] = true;
            actionRewardMods_[action] = 1.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    NLM_LOG_INFO("MotorSystem initialized with " + 
                 std::to_string(pImpl->actionTypesEnabled_.size()) + " action types");
}

ActionType MotorSystem::selectAction(const std::vector<float>& currentState,
                                    SimulationStep currentStep) {
    if (!pImpl->brain) {
        return ActionType::Wait;
    }
    
    // Get available actions based on brain motor neurons
    auto enabledActions = getEnabledActions();
    if (enabledActions.empty()) {
        return ActionType::Wait;
    }
    
    // Calculate action values based on preferences, statistics, and state
    std::vector<std::pair<ActionType, float>> actionValues;
    
    for (ActionType action : enabledActions) {
        // Base value from preference
        float value = pImpl->actionPreferences_[action];
        
        // Add statistics (success rate)
        float successRate = pImpl->successRates_[action];
        value += successRate * 2.0f; // Weight success rate heavily
        
        // Add cumulative reward
        float totalReward = pImpl->cumulativeRewards_[action];
        if (pImpl->executionCounts_[action] > 0) {
            value += (totalReward / pImpl->executionCounts_[action]) * 1.5f;
        }
        
        // Add state-dependent value (simplified - could use neural patterns)
        // For now, use random component based on state hash
        float stateComponent = std::abs(std::hash<std::vector<float>>{}(currentState) % 100) / 100.0f;
        value += stateComponent * 0.5f;
        
        // Add action-specific reward modulation
        value *= pImpl->actionRewardMods_[action];
        
        // Add developmental bias (earlier stages favor exploration)
        if (currentStep < 100) { // Early steps (development)
            value *= 1.2f; // Favor exploration during development
        } else if (currentStep > 1000) { // Adult stage
            value *= 0.9f; // Favor exploitation in adult stage
        }
        
        // Add neuromodulation influence
        if (pImpl->neuromodulationInfluence_ != 1.0f) {
            value *= pImpl->neuromodulationInfluence_;
        }
        
        // Apply curiosity-based exploration (external factor)
        if (pImpl->explorationEnabled_) {
            // Random exploration factor
            float explorationBonus = uniformDist_(pImpl->rng_) * pImpl->explorationRate_;
            value += explorationBonus;
        }
        
        actionValues.push_back({action, value});
    }
    
    // Find action with highest value
    ActionType bestAction = ActionType::Wait;
    float bestValue = -999999.0f;
    
    for (const auto& pair : actionValues) {
        if (pair.second > bestValue) {
            bestValue = pair.second;
            bestAction = pair.first;
        }
    }
    
    // Record action execution
    pImpl->executionCounts_[bestAction]++;
    pImpl->lastExecutionSteps_[bestAction] = currentStep;
    
    // Add to recent actions for diversity calculation
    pImpl->recentActions_.push_back(bestAction);
    if (pImpl->recentActions_.size() > 100) {
        pImpl->recentActions_.pop_front();
    }
    
    NLM_LOG_DEBUG("MotorSystem selected action: " + std::to_string(static_cast<int>(bestAction)) + 
                 " (value: " + std::to_string(bestValue) + ")");
    
    return bestAction;
}

float MotorSystem::getActionConfidence(ActionType action,
                                       const std::vector<float>& state) const {
    // Confidence based on execution count and success rate
    size_t count = pImpl->executionCounts_.at(action);
    if (count == 0) return 0.0f;
    
    float successRate = pImpl->successRates_.at(action);
    
    // Combine execution count and success rate for confidence
    float confidence = successRate * 0.6f + std::min(1.0f, static_cast<float>(count) / 50.0f) * 0.4f;
    
    // Add state matching component (simplified)
    float stateMatch = std::abs(std::hash<std::vector<float>>{}(state) % 100) / 100.0f;
    confidence = (confidence + stateMatch) * 0.5f;
    
    return confidence;
}

void MotorSystem::learnFromExperience(ActionType action,
                                      const std::vector<float>& beforeState,
                                      const std::vector<float>& afterState,
                                      float reward) {
    if (!pImpl->actionLearningEnabled_) {
        return;
    }
    
    // Update execution statistics
    pImpl->executionCounts_[action]++;
    
    // Update success rate (reward > 0.5 is success)
    bool wasSuccessful = (reward > 0.5f);
    float currentSuccessRate = pImpl->successRates_[action];
    float newSuccessRate = currentSuccessRate * 0.9f + (wasSuccessful ? 1.0f : 0.0f) * 0.1f;
    pImpl->successRates_[action] = newSuccessRate;
    
    // Update cumulative reward
    pImpl->cumulativeRewards_[action] += reward;
    
    // Record experience
    auto& rng = pImpl->rng_;
    auto& uniformDist = pImpl->uniformDist_;
    Experience exp {
        /*step*/ 0, // Would need to track actual step
        action,
        beforeState,
        reward,
        uniformDist(rng) // Random confidence for now
    };
    pImpl->experienceMemory_.push_back(exp);
    
    // Limit memory size
    if (pImpl->experienceMemory_.size() > 1000) {
        pImpl->experienceMemory_.erase(pImpl->experienceMemory_.begin());
    }
    
    // Update action preference based on reward
    pImpl->actionPreferences_[action] *= (1.0f + reward * 0.1f);
    
    // Apply reward modulation to other actions
    for (auto& pair : pImpl->actionRewardMods_) {
        if (pair.first != action) {
            // Vicarious learning - similar actions get small reward modulations
            pair.second *= (1.0f - reward * 0.01f);
        }
    }
    
    // Apply action-specific reward modulation
    pImpl->actionRewardMods_[action] *= (1.0f + reward * 0.2f);
    
    // Clamp values
    pImpl->actionRewardMods_[action] = std::clamp(pImpl->actionRewardMods_[action], 0.1f, 5.0f);
    
    NLM_LOG_DEBUG("MotorSystem learned from action " + std::to_string(static_cast<int>(action)) + 
                 " with reward " + std::to_string(reward));
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences_[action] = std::clamp(preference, 0.0f, 10.0f);
}

float MotorSystem::getActionPreference(ActionType action) const {
    auto it = pImpl->actionPreferences_.find(action);
    return (it != pImpl->actionPreferences_.end()) ? it->second : 1.0f;
}

float MotorSystem::getActionCount(ActionType action) const {
    auto it = pImpl->executionCounts_.find(action);
    return (it != pImpl->executionCounts_.end()) ? it->second : 0;
}

float MotorSystem::getActionSuccessRate(ActionType action) const {
    auto it = pImpl->successRates_.find(action);
    return (it != pImpl->successRates_.end()) ? it->second : 0.5f;
}

void MotorSystem::enableActionType(ActionType action, bool enable) {
    pImpl->actionTypesEnabled_[action] = enable;
}

bool MotorSystem::isActionTypeEnabled(ActionType action) const {
    auto it = pImpl->actionTypesEnabled_.find(action);
    return (it != pImpl->actionTypesEnabled_.end()) ? it->second : true;
}

std::vector<ActionType> MotorSystem::getEnabledActions() const {
    std::vector<ActionType> enabled;
    for (const auto& pair : pImpl->actionTypesEnabled_) {
        if (pair.second) {
            enabled.push_back(pair.first);
        }
    }
    return enabled;
}

void MotorSystem::clearLearningHistory() {
    pImpl->experienceMemory_.clear();
    for (auto& pair : pImpl->actionPreferences_) {
        pair.second = 1.0f; // Reset preferences
    }
    for (auto& pair : pImpl->executionCounts_) {
        pair.second = 0; // Reset counts
    }
    for (auto& pair : pImpl->successRates_) {
        pair.second = 0.5f; // Reset success rates
    }
    for (auto& pair : pImpl->cumulativeRewards_) {
        pair.second = 0.0f; // Reset rewards
    }
    
    NLM_LOG_INFO("MotorSystem learning history cleared");
}

void MotorSystem::setExplorationMode(float explorationRate) {
    pImpl->explorationRate_ = std::clamp(explorationRate, 0.0f, 1.0f);
}

float MotorSystem::getExplorationMode() const {
    return pImpl->explorationRate_;
}

float MotorSystem::getActionDiversity() const {
    if (pImpl->recentActions_.size() < 2) return 0.0f;
    
    // Count unique actions in recent history
    std::unordered_set<ActionType> uniqueActions(pImpl->recentActions_.begin(),
                                                 pImpl->recentActions_.end());
    return static_cast<float>(uniqueActions.size()) / 8.0f; // Normalize to 0-1
}

MotorCommand MotorSystem::generateMotorCommand(ActionType action, float confidence) const {
    // Convert action to motor command (simplified mapping)
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Wait: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

float MotorSystem::getActionPredictionError(ActionType action,
                                           const std::vector<float>& state) const {
    // Calculate prediction error based on expected vs actual outcomes
    // This would normally use a predictive model, but for now use simplified heuristic
    
    size_t count = pImpl->executionCounts_.at(action);
    if (count == 0) return 1.0f; // High uncertainty for unused actions
    
    float expectedReward = pImpl->cumulativeRewards_.at(action) / count;
    float variance = 0.0f;
    
    // Estimate variance based on recent outcomes
    // For now, use a simplified approach
    variance = 0.2f * (1.0f - pImpl->successRates_.at(action));
    
    // Convert to error measure (lower variance = lower error)
    float error = variance / (0.5f + expectedReward * 0.5f);
    
    return std::clamp(error, 0.0f, 1.0f);
}

bool MotorSystem::shouldExplore(ActionType currentAction, float curiosityLevel,
                                SimulationStep currentStep) const {
    // Multiple criteria for exploration decision
    
    // 1. High curiosity suggests exploration
    if (curiosityLevel > 0.7f) {
        return true;
    }
    
    // 2. Random exploration based on exploration rate
    if (uniformDist_(pImpl->rng_) < pImpl->explorationRate_) {
        return true;
    }
    
    // 3. Novelty seeking (different from recent actions)
    if (!pImpl->recentActions_.empty()) {
        ActionType lastAction = pImpl->recentActions_.back();
        if (currentAction != lastAction) {
            // Check if current action is different enough
            return true;
        }
    }
    
    // 4. During developmental stages (early simulation steps)
    if (currentStep < 100) {
        return true;
    }
    
    // 5. If current action has high prediction error
    float predictionError = getActionPredictionError(currentAction, std::vector<float>());
    if (predictionError > 0.5f) {
        return true;
    }
    
    return false;
}

std::vector<ActionType> MotorSystem::predictActionSequence(
    const std::vector<float>& currentState,
    size_t sequenceLength) const {
    
    std::vector<ActionType> sequence;
    
    // Simple heuristic-based sequence prediction
    // In a real implementation, this would use learned patterns
    
    std::vector<ActionType> enabledActions = getEnabledActions();
    if (enabledActions.empty()) {
        return sequence;
    }
    
    // Start with current best action (simplified)
    ActionType currentAction = enabledActions[0];
    sequence.push_back(currentAction);
    
    // Generate sequence using simple rules
    for (size_t i = 1; i < sequenceLength; ++i) {
        // Choose next action based on sequence patterns
        ActionType nextAction;
        
        if (i == 1) {
            // First step: try something different
            do {
                nextAction = enabledActions[uniformDist_(pImpl->rng_) % enabledActions.size()];
            } while (nextAction == currentAction && enabledActions.size() > 1);
        } else {
            // Subsequent steps: use learned patterns
            if (sequence.size() >= 3) {
                // Simple pattern: if we moved forward last, alternate with turn
                if (sequence[i-2] == ActionType::MoveForward) {
                    nextAction = ActionType::TurnLeft;
                } else {
                    nextAction = ActionType::MoveForward;
                }
            } else {
                nextAction = currentAction; // Repeat for short sequences
            }
        }
        
        sequence.push_back(nextAction);
        currentAction = nextAction;
    }
    
    return sequence;
}

void MotorSystem::setActionRewardMod(ActionType action, float modValue) {
    pImpl->actionRewardMods_[action] = std::clamp(modValue, 0.1f, 5.0f);
}

float MotorSystem::getActionRewardMod(ActionType action) const {
    auto it = pImpl->actionRewardMods_.find(action);
    return (it != pImpl->actionRewardMods_.end()) ? it->second : 1.0f;
}

void MotorSystem::enableActionLearning(bool enable) {
    pImpl->actionLearningEnabled_ = enable;
}

bool MotorSystem::isActionLearningEnabled() const {
    return pImpl->actionLearningEnabled_;
}

float MotorSystem::calculateActionValue(ActionType action,
                                       const std::vector<float>& state) const {
    // Comprehensive action value calculation
    float value = 0.0f;
    
    // Preference component
    value += getActionPreference(action) * 0.3f;
    
    // Success rate component
    value += getActionSuccessRate(action) * 0.3f;
    
    // Experience count component
    float count = getActionCount(action);
    float countComponent = std::min(1.0f, count / 50.0f);
    value += countComponent * 0.2f;
    
    // Prediction error component (lower error = higher value)
    float error = getActionPredictionError(action, state);
    value += (1.0f - error) * 0.2f;
    
    return value;
}

size_t MotorSystem::getActionExecutionCount(ActionType action) const {
    auto it = pImpl->executionCounts_.find(action);
    return (it != pImpl->executionCounts_.end()) ? it->second : 0;
}

bool MotorSystem::wasActionRecentlySuccessful(ActionType action,
                                              SimulationStep lookbackSteps) const {
    // Check if action was successful in recent steps
    // This would normally track actual outcomes
    
    size_t count = getActionExecutionCount(action);
    if (count == 0) return false;
    
    float successRate = getActionSuccessRate(action);
    return successRate > 0.7f; // Consider recently successful if high success rate
}

std::vector<MotorSystem::ActionStats> MotorSystem::getActionStatistics() const {
    std::vector<ActionStats> stats;
    
    for (const auto& pair : pImpl->executionCounts_) {
        ActionType action = pair.first;
        ActionStats stat;
        stat.action = action;
        stat.executionCount = pair.second;
        stat.successRate = pImpl->successRates_.at(action);
        stat.averageReward = (stat.executionCount > 0) ? 
            pImpl->cumulativeRewards_.at(action) / stat.executionCount : 0.0f;
        stat.confidence = getActionConfidence(action, std::vector<float>());
        stat.preference = pImpl->actionPreferences_.at(action);
        stat.lastExecutionStep = pImpl->lastExecutionSteps_.at(action);
        
        stats.push_back(stat);
    }
    
    return stats;
}
