#include "MotorSystem.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../memory/AdvancedWorkingMemory.hpp"
#include "../memory/AdvancedEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <chrono>

namespace nlm {

struct MotorSystem::Impl {
    // Action preference system
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Action history and statistics
    std::deque<ActionHistoryEntry> actionHistory;
    std::unordered_map<ActionType, ActionStatistics> actionStats;
    
    // Neural integration
    Brain* brain;
    
    // Selection weighting
    float neuralWeight;
    float memoryWeight;
    float predictionWeight;
    float plannerWeight;
    
    // Adaptation parameters
    float preferenceAdaptationRate;
    float reinforcementStrength;
    float noveltyThreshold;
    float predictionErrorThreshold;
    
    // Optimization
    float temperature;  // For exploration-exploitation tradeoff
    size_t historySize;
    
    // Random number generator for exploration
    std::mt19937 rng;
    
    Impl() : brain(nullptr), neuralWeight(0.4f), memoryWeight(0.2f), 
             predictionWeight(0.2f), plannerWeight(0.2f),
             preferenceAdaptationRate(0.1f), reinforcementStrength(0.3f),
             noveltyThreshold(0.5f), predictionErrorThreshold(0.3f),
             temperature(1.0f), historySize(100) {
        
        // Initialize random number generator
        std::random_device rd;
        rng.seed(rd());
        
        // Initialize action preferences
        std::vector<ActionType> actions = {
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::MoveLeft, ActionType::MoveRight,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Look, ActionType::LookUp, ActionType::LookDown,
            ActionType::Interact, ActionType::Eat, ActionType::Drink,
            ActionType::Rest, ActionType::Wait, ActionType::Custom
        };
        
        for (const auto& action : actions) {
            actionPreferences[action] = 0.5f;  // Neutral preference
        }
        
        // Initialize action statistics
        for (const auto& action : actions) {
            actionStats[action] = ActionStatistics();
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_WARNING("MotorSystem::initialize: Brain pointer is null");
        return;
    }
    
    pImpl->brain = brain;
    NLM_LOG_INFO("MotorSystem initialized with brain integration");
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Check if all systems are initialized
    if (!pImpl->brain) {
        NLM_LOG_ERROR("MotorSystem::selectAction: Brain not initialized");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get current state from brain systems
    std::vector<float> currentNeuralActivity;
    std::vector<float> currentSensoryState;
    float predictionError = 0.0f;
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    
    // Get prediction error system
    auto predictionErrorSystem = pImpl->brain->getPredictionErrorSignal();
    
    // Extract neural activity from motor region
    for (const auto& population : motorRegion.getAllPopulations()) {
        float populationActivity = population->getActivityLevel();
        for (size_t i = 0; i < population->getSize(); ++i) {
            currentNeuralActivity.push_back(populationActivity);
        }
    }
    
    // Get sensory state
    currentSensoryState = pImpl->brain->getCurrentSensoryState();
    
    // Get neuromodulation signals
    auto rewardSystem = pImpl->brain->getReward();
    if (rewardSystem) {
        predictionError = predictionErrorSystem ? predictionErrorSystem->getModulationSignal() : 0.0f;
    }
    
    auto dopamineSystem = pImpl->brain->getDopamine();
    if (dopamineSystem) {
        dopamineLevel = dopamineSystem->getLevel();
    }
    
    auto curiositySystem = pImpl->brain->getCuriosity();
    if (curiositySystem) {
        curiosityLevel = curiositySystem->getLevel();
    }
    
    // Get action probabilities from each system
    std::vector<std::pair<ActionType, float>> neuralActions;
    std::vector<std::pair<ActionType, float>> memoryActions;
    std::vector<std::pair<ActionType, float>> predictionActions;
    std::vector<std::pair<ActionType, float>> plannerActions;
    
    // Neural-based selection
    if (!currentNeuralActivity.empty()) {
        neuralActions = selectActionFromNeuralActivity(currentNeuralActivity);
    }
    
    // Memory-based selection
    memoryActions = selectActionBasedOnMemory();
    
    // Prediction-based selection
    predictionActions = selectActionBasedOnPrediction();
    
    // Planner-based selection
    plannerActions = selectActionBasedOnPlanner();
    
    // Combine all sources with weighted averaging
    std::unordered_map<ActionType, float> combinedScores;
    
    // Normalize and combine neural scores
    float neuralMax = 0.0f;
    for (const auto& pair : neuralActions) {
        neuralMax = std::max(neuralMax, pair.second);
    }
    if (neuralMax > 0.0f) {
        for (const auto& pair : neuralActions) {
            float normalizedScore = pair.second / neuralMax;
            combinedScores[pair.first] += normalizedScore * pImpl->neuralWeight;
        }
    }
    
    // Normalize and combine memory scores
    float memoryMax = 0.0f;
    for (const auto& pair : memoryActions) {
        memoryMax = std::max(memoryMax, pair.second);
    }
    if (memoryMax > 0.0f) {
        for (const auto& pair : memoryActions) {
            float normalizedScore = pair.second / memoryMax;
            combinedScores[pair.first] += normalizedScore * pImpl->memoryWeight;
        }
    }
    
    // Normalize and combine prediction scores
    float predictionMax = 0.0f;
    for (const auto& pair : predictionActions) {
        predictionMax = std::max(predictionMax, pair.second);
    }
    if (predictionMax > 0.0f) {
        for (const auto& pair : predictionActions) {
            float normalizedScore = pair.second / predictionMax;
            combinedScores[pair.first] += normalizedScore * pImpl->predictionWeight;
        }
    }
    
    // Normalize and combine planner scores
    float plannerMax = 0.0f;
    for (const auto& pair : plannerActions) {
        plannerMax = std::max(plannerMax, pair.second);
    }
    if (plannerMax > 0.0f) {
        for (const auto& pair : plannerActions) {
            float normalizedScore = pair.second / plannerMax;
            combinedScores[pair.first] += normalizedScore * pImpl->plannerWeight;
        }
    }
    
    // Apply action preferences
    for (auto& pair : combinedScores) {
        pair.second *= pImpl->actionPreferences[pair.first];
    }
    
    // Apply exploration based on temperature and neuromodulation
    float explorationBonus = 1.0f + dopamineLevel * 0.5f + curiosityLevel * 0.3f;
    float totalScore = 0.0f;
    std::vector<ActionType> candidateActions;
    std::vector<float> candidateScores;
    
    for (const auto& pair : combinedScores) {
        candidateActions.push_back(pair.first);
        candidateScores.push_back(pair.second * explorationBonus);
        totalScore += pair.second * explorationBonus;
    }
    
    // Normalize scores
    if (totalScore > 0.0f) {
        for (float& score : candidateScores) {
            score /= totalScore;
        }
    }
    
    // Apply temperature parameter for exploration-exploitation
    std::vector<float> temperedScores;
    for (float score : candidateScores) {
        float tempered = std::pow(score, 1.0f / pImpl->temperature);
        temperedScores.push_back(tempered);
    }
    
    // Convert to probabilities using softmax
    float maxTempered = *std::max_element(temperedScores.begin(), temperedScores.end());
    std::vector<float> probabilities;
    float sumProbs = 0.0f;
    
    for (float score : temperedScores) {
        float prob = std::exp(score - maxTempered);
        probabilities.push_back(prob);
        sumProbs += prob;
    }
    
    if (sumProbs > 0.0f) {
        for (float& prob : probabilities) {
            prob /= sumProbs;
        }
    }
    
    // Sample action based on probabilities
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float sample = dist(pImpl->rng);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probabilities.size(); ++i) {
        cumulative += probabilities[i];
        if (sample <= cumulative) {
            ActionType selectedAction = candidateActions[i];
            
            // Evaluate the selected action
            float evaluation = evaluateAction(selectedAction);
            
            // Create action with parameters
            auto action = std::make_unique<Action>(selectedAction);
            
            // Store neural activation for this action
            if (!neuralActions.empty()) {
                action->setNeuralActivation(neuralActions[i].second);
            }
            
            // Record action execution
            recordAction(selectedAction, *action);
            
            return action;
        }
    }
    
    // Fallback: select action with highest combined score
    if (!candidateActions.empty()) {
        size_t bestIdx = std::distance(candidateScores.begin(), 
                                      std::max_element(candidateScores.begin(), candidateScores.end()));
        ActionType fallbackAction = candidateActions[bestIdx];
        
        auto action = std::make_unique<Action>(fallbackAction);
        recordAction(fallbackAction, *action);
        return action;
    }
    
    // Ultimate fallback
    return std::make_unique<Action>(ActionType::Wait);
}

std::unique_ptr<Action> MotorSystem::selectActionBasedOnMemory() {
    if (!pImpl->brain) return std::make_unique<Action>(ActionType::Wait);
    
    auto workingMemory = pImpl->brain->getAdvancedWorkingMemory();
    auto episodicMemory = pImpl->brain->getAdvancedEpisodicMemory();
    
    if (!workingMemory || !episodicMemory) return std::make_unique<Action>(ActionType::Wait);
    
    std::vector<std::pair<ActionType, float>> actionScores;
    
    // Get current sensory state for similarity matching
    const std::vector<float>& currentState = pImpl->brain->getCurrentSensoryState();
    
    // Retrieve similar episodes from episodic memory
    auto similarEpisodes = episodicMemory->retrieveSimilar(currentState, 5);
    
    for (const auto* episode : similarEpisodes) {
        if (episode && episode->action != ActionType::Wait) {
            actionScores.push_back({episode->action, episode->reward});
        }
    }
    
    // Get working memory items that might indicate action preferences
    for (const auto& action : getAvailableActions()) {
        bool hasWorkingMemory = workingMemory->contains(static_cast<NeuronId>(action));
        if (hasWorkingMemory) {
            float workingMemoryValue = workingMemory->retrieve(static_cast<NeuronId>(action));
            actionScores.push_back({action, workingMemoryValue * 0.5f});  // Weight less than episodic
        }
    }
    
    // Aggregate scores for each action
    std::unordered_map<ActionType, float> actionScoresMap;
    for (const auto& pair : actionScores) {
        actionScoresMap[pair.first] += pair.second;
    }
    
    // Normalize and return best action
    if (actionScoresMap.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    ActionType bestAction = ActionType::Wait;
    float bestScore = -1000.0f;
    
    for (const auto& pair : actionScoresMap) {
        if (pair.second > bestScore) {
            bestScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    auto action = std::make_unique<Action>(bestAction);
    action->setMemoryBased(true);
    recordAction(bestAction, *action);
    
    return action;
}

std::unique_ptr<Action> MotorSystem::selectActionBasedOnPrediction() {
    if (!pImpl->brain) return std::make_unique<Action>(ActionType::Wait);
    
    auto predictionSystem = pImpl->brain->getPredictionSystem();
    auto rewardSystem = pImpl->brain->getReward();
    auto predictionErrorSystem = pImpl->brain->getPredictionErrorSignal();
    
    if (!predictionSystem || !rewardSystem) return std::make_unique<Action>(ActionType::Wait);
    
    const std::vector<float>& currentState = pImpl->brain->getCurrentSensoryState();
    
    // Get prediction error for action selection
    float predictionError = predictionErrorSystem ? predictionErrorSystem->getModulationSignal() : 0.0f;
    
    // Get current reward value
    float currentReward = rewardSystem->getValue();
    
    std::vector<std::pair<ActionType, float>> actionScores;
    
    // Evaluate each action based on predicted outcomes
    for (const auto& action : getAvailableActions()) {
        // Get predicted consequence of this action
        auto predictedState = predictionSystem->predictActionConsequence(action, currentState);
        
        if (predictedState.empty()) {
            actionScores.push_back({action, 0.0f});
            continue;
        }
        
        // Compute predicted reward
        float predictedReward = 0.0f;
        if (!currentState.empty()) {
            float similarity = computeSensorySimilarity(predictedState, currentState);
            predictedReward = similarity * (1.0f + currentReward * 0.5f);
        }
        
        // Adjust for prediction error (high error = higher exploration bonus)
        float errorAdjustment = 1.0f + predictionError * 0.5f;
        
        // Modulate by action preferences
        float preferenceFactor = pImpl->actionPreferences[action];
        
        // Final predicted score
        float finalScore = predictedReward * errorAdjustment * preferenceFactor;
        
        actionScores.push_back({action, finalScore});
    }
    
    // Normalize and select best action
    if (actionScores.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    ActionType bestAction = ActionType::Wait;
    float bestScore = -1000.0f;
    
    for (const auto& pair : actionScores) {
        if (pair.second > bestScore) {
            bestScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    auto action = std::make_unique<Action>(bestAction);
    action->setPredictionBased(true);
    recordAction(bestAction, *action);
    
    return action;
}

std::unique_ptr<Action> MotorSystem::selectActionBasedOnPlanner() {
    if (!pImpl->brain) return std::make_unique<Action>(ActionType::Wait);
    
    auto neuralPlanner = pImpl->brain->getPlanner();
    if (!neuralPlanner) return std::make_unique<Action>(ActionType::Wait);
    
    const std::vector<float>& currentState = pImpl->brain->getCurrentSensoryState();
    float currentReward = 0.0f;
    
    auto rewardSystem = pImpl->brain->getReward();
    if (rewardSystem) {
        currentReward = rewardSystem->getValue();
    }
    
    // Let the neural planner decide
    ActionType plannedAction = neuralPlanner->planAction(currentState, currentReward);
    
    auto action = std::make_unique<Action>(plannedAction);
    action->setPlannerBased(true);
    recordAction(plannedAction, *action);
    
    return action;
}

float MotorSystem::evaluateAction(ActionType action) {
    auto rewardSystem = pImpl->brain ? pImpl->brain->getReward() : nullptr;
    auto predictionErrorSystem = pImpl->brain ? pImpl->brain->getPredictionErrorSignal() : nullptr;
    auto curiositySystem = pImpl->brain ? pImpl->brain->getCuriosity() : nullptr;
    auto noveltySystem = pImpl->brain ? pImpl->brain->getNovelty() : nullptr;
    
    if (!rewardSystem) return 0.0f;
    
    float evaluation = 0.0f;
    
    // Get action statistics
    auto statsIt = pImpl->actionStats.find(action);
    if (statsIt != pImpl->actionStats.end()) {
        const auto& stats = statsIt->second;
        
        // Success rate contribution
        float successRate = stats.getSuccessRate();
        evaluation += successRate * 0.4f;
        
        // Average reward contribution
        evaluation += std::min(1.0f, stats.averageReward) * 0.3f;
        
        // Prediction error contribution (lower error = higher evaluation)
        float errorContribution = std::max(0.0f, 1.0f - stats.averagePredictionError);
        evaluation += errorContribution * 0.2f;
        
        // Novelty contribution
        evaluation += std::min(1.0f, stats.noveltyScore) * 0.1f;
    }
    
    // Prediction error bonus (encourages exploration of uncertain actions)
    float predictionError = predictionErrorSystem ? predictionErrorSystem->getModulationSignal() : 0.0f;
    evaluation += predictionError * 0.2f * (1.0f - pImpl->actionPreferences[action]);
    
    // Curiosity bonus
    float curiosity = curiositySystem ? curiositySystem->getLevel() : 0.0f;
    evaluation += curiosity * 0.1f * (1.0f - pImpl->actionPreferences[action]);
    
    // Novelty bonus
    float novelty = noveltySystem ? noveltySystem->getLevel() : 0.0f;
    evaluation += novelty * 0.1f * (1.0f - pImpl->actionPreferences[action]);
    
    return evaluation;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 1.0f);
}

const std::unordered_map<ActionType, float>& MotorSystem::getActionPreferences() const {
    return pImpl->actionPreferences;
}

void MotorSystem::adaptActionPreferences() {
    if (pImpl->actionHistory.size() < 10) return;  // Need enough data
    
    // Analyze recent action outcomes
    size_t recentHistorySize = std::min(pImpl->historySize, pImpl->actionHistory.size());
    std::deque<ActionHistoryEntry> recentHistory(
        pImpl->actionHistory.end() - recentHistorySize, 
        pImpl->actionHistory.end()
    );
    
    // Count successful executions per action
    std::unordered_map<ActionType, int> successCounts;
    std::unordered_map<ActionType, int> totalCounts;
    
    for (const auto& entry : recentHistory) {
        totalCounts[entry.action]++;
        if (entry.success) {
            successCounts[entry.action]++;
        }
    }
    
    // Adapt preferences based on success rates
    for (auto& pair : pImpl->actionPreferences) {
        ActionType action = pair.first;
        int total = totalCounts[action];
        int success = successCounts[action];
        
        if (total > 5) {  // Need minimum samples
            float currentSuccessRate = static_cast<float>(success) / total;
            
            // Increase preference for successful actions
            if (currentSuccessRate > 0.6f) {
                pair.second = std::min(1.0f, pair.second + pImpl->preferenceAdaptationRate);
            }
            // Decrease preference for unsuccessful actions
            else if (currentSuccessRate < 0.4f) {
                pair.second = std::max(0.1f, pair.second - pImpl->preferenceAdaptationRate);
            }
        }
    }
}

void MotorSystem::reinforceAction(ActionType action, float strength) {
    if (strength <= 0.0f) return;
    
    // Increase action preference
    pImpl->actionPreferences[action] = std::min(1.0f, 
        pImpl->actionPreferences[action] + strength * pImpl->reinforcementStrength);
    
    // Update action statistics
    auto& stats = pImpl->actionStats[action];
    stats.executionCount++;
    if (strength > 0.5f) {  // Consider strong reinforcement as success
        stats.successCount++;
    }
    
    // Record reinforcement in history
    ActionHistoryEntry entry;
    entry.timestamp = SimulationStep(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    entry.action = action;
    entry.reward = strength;
    entry.success = (strength > 0.5f);
    
    pImpl->actionHistory.push_back(entry);
    if (pImpl->actionHistory.size() > pImpl->historySize) {
        pImpl->actionHistory.pop_front();
    }
}

void MotorSystem::recordAction(ActionType action, const Action& actionData) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ActionHistoryEntry entry;
    entry.timestamp = SimulationStep(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    entry.action = action;
    entry.actionData = std::make_unique<Action>(actionData);
    
    // Record neural activation (simplified - would come from actual neural activity)
    entry.neuralActivation = pImpl->actionPreferences[action];
    
    // For now, use placeholder values for predicted/actual outcomes
    entry.predictedOutcome = 0.5f;
    entry.actualOutcome = 0.5f;
    entry.reward = 0.0f;
    entry.predictionError = 0.0f;
    entry.success = false;
    
    pImpl->actionHistory.push_back(entry);
    if (pImpl->actionHistory.size() > pImpl->historySize) {
        pImpl->actionHistory.pop_front();
    }
    
    // Update action statistics
    auto& stats = pImpl->actionStats[action];
    stats.executionCount++;
    if (entry.success) {
        stats.successCount++;
    }
    
    // Record completion time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    stats.averageExecutionTime = (stats.averageExecutionTime * (stats.executionCount - 1) + 
                                  duration.count() / 1000.0f) / stats.executionCount;
}

const std::deque<ActionHistoryEntry>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

const std::unordered_map<ActionType, ActionStatistics>& MotorSystem::getActionStatistics() const {
    return pImpl->actionStats;
}

float MotorSystem::getSuccessRate(ActionType action) const {
    auto statsIt = pImpl->actionStats.find(action);
    if (statsIt == pImpl->actionStats.end()) return 0.0f;
    return statsIt->second.getSuccessRate();
}

std::deque<ActionType> MotorSystem::getActionSequence(size_t maxLength) const {
    std::deque<ActionType> sequence;
    size_t count = std::min(maxLength, pImpl->actionHistory.size());
    
    for (auto it = pImpl->actionHistory.rbegin(); it != pImpl->actionHistory.rend() && sequence.size() < count; ++it) {
        sequence.push_front(it->action);
    }
    
    return sequence;
}

void MotorSystem::optimizeActionSelection() {
    // Adaptive temperature based on exploration-exploitation balance
    float explorationRate = 1.0f - getAverageSuccessRate();
    
    // Adjust temperature to maintain good exploration
    if (explorationRate > 0.7f) {
        // Too much exploration, reduce temperature
        pImpl->temperature = std::max(0.5f, pImpl->temperature * 0.9f);
    }
    else if (explorationRate < 0.3f) {
        // Too much exploitation, increase temperature
        pImpl->temperature = std::min(5.0f, pImpl->temperature * 1.1f);
    }
    
    // Adapt action preferences based on recent performance
    adaptActionPreferences();
    
    // Reset if performance is poor
    if (getAverageSuccessRate() < 0.2f) {
        resetActionHistory();
    }
}

void MotorSystem::handleActionFailure(ActionType action) {
    // Decrease action preference for failed action
    pImpl->actionPreferences[action] *= 0.7f;
    
    // Update statistics to mark failure
    auto& stats = pImpl->actionStats[action];
    stats.executionCount++;
    // No increment of successCount (failure)
    
    // Record failure in history
    ActionHistoryEntry entry;
    entry.timestamp = SimulationStep(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    entry.action = action;
    entry.success = false;
    
    pImpl->actionHistory.push_back(entry);
    if (pImpl->actionHistory.size() > pImpl->historySize) {
        pImpl->actionHistory.pop_front();
    }
    
    NLM_LOG_WARNING(std::string("MotorSystem::handleActionFailure: Action ") + 
                   std::to_string(static_cast<int>(action)) + " failed and preferences reduced");
}

void MotorSystem::resetActionHistory() {
    pImpl->actionHistory.clear();
    
    // Reset action statistics to neutral values
    for (auto& pair : pImpl->actionStats) {
        pair.second = ActionStatistics();
    }
    
    // Reset action preferences to neutral
    for (auto& pair : pImpl->actionPreferences) {
        pair.second = 0.5f;
    }
    
    NLM_LOG_INFO("MotorSystem::resetActionHistory: Action history and preferences reset");
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    // Return default available actions
    static const std::vector<ActionType> actions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::MoveLeft, ActionType::MoveRight,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    return actions;
}

// Helper functions

std::vector<std::pair<ActionType, float>> MotorSystem::selectActionFromNeuralActivity(
    const std::vector<float>& neuralActivity) {
    std::vector<std::pair<ActionType, float>> actionScores;
    
    if (neuralActivity.empty()) {
        return actionScores;
    }
    
    // Compute neural activation patterns for each action type
    for (const auto& action : getAvailableActions()) {
        // For now, use a simplified model based on action type index
        size_t actionIndex = static_cast<size_t>(action);
        if (actionIndex < neuralActivity.size()) {
            actionScores.push_back({action, neuralActivity[actionIndex]});
        } else {
            actionScores.push_back({action, 0.5f});  // Default neutral activation
        }
    }
    
    return actionScores;
}

float MotorSystem::computeSensorySimilarity(const std::vector<float>& a, 
                                          const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

float MotorSystem::getAverageSuccessRate() const {
    if (pImpl->actionStats.empty()) return 0.0f;
    
    float totalSuccessRate = 0.0f;
    size_t count = 0;
    
    for (const auto& pair : pImpl->actionStats) {
        totalSuccessRate += pair.second.getSuccessRate();
        count++;
    }
    
    return totalSuccessRate / count;
}

} // namespace nlm
