#include "MotorSystem.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <random>
#include <unordered_map>
#include <cmath>
#include <chrono>

namespace nlm {

struct ActionSelectionInfo {
    ActionType action;
    double score;  // Combined preference, neural activity, reward, etc.
    ActionType type; // For backward compatibility
    ActionSelectionInfo(ActionType a, double s) : action(a), score(s), type(a) {}
};

struct ActionStats {
    uint32_t successCount = 0;
    uint32_t failureCount = 0;
    double totalReward = 0.0;
    double totalCost = 0.0;
    Timestamp lastSuccessTime = 0.0;
    Timestamp lastFailureTime = 0.0;
    Timestamp lastExecutedTime = 0.0;
    double averageValue = 0.0;
    uint32_t visitCount = 0;
    
    double getSuccessRate() const {
        uint32_t total = successCount + failureCount;
        return total > 0 ? static_cast<double>(successCount) / total : 0.0;
    }
    
    double getNetValue() const {
        double net = totalReward - totalCost;
        return visitCount > 0 ? net / visitCount : 0.0;
    }
};

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<std::unique_ptr<Action>> actionHistory;  // Store ownership
    std::unordered_map<ActionType, float> actionPreferences;
    std::unordered_map<ActionType, ActionStats> actionStats;
    std::unordered_map<ActionType, double> valueEstimates;  // RL value estimates
    std::unordered_map<ActionType, uint32_t> actionCounts;  // For Thompson sampling
    RandomGenerator rng;
    
    // Action selection parameters
    double explorationTemperature = 1.0;  // For softmax
    double epsilonGreedyRate = 0.1;      // For epsilon-greedy
    bool useThompsonSampling = true;
    bool useSoftmax = true;
    
    // Neural activity integration
    double neuralActivityWeight = 0.3;
    double preferenceWeight = 0.4;
    double historyWeight = 0.2;
    double rewardWeight = 0.1;
    
    Impl() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::Interact,
            ActionType::Wait
        };
        
        // Initialize preferences to 1.0 for all actions (uniform)
        for (const auto& action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
        
        // Initialize value estimates
        for (const auto& action : availableActions) {
            valueEstimates[action] = 0.0;
        }
    }
    
    // Cleanup function for action history
    void cleanupHistory() {
        // Remove old actions to prevent memory bloat
        // Keep only the last N actions (e.g., 100)
        const size_t maxHistorySize = 100;
        if (actionHistory.size() > maxHistorySize) {
            auto keepStart = actionHistory.begin() + (actionHistory.size() - maxHistorySize);
            std::vector<std::unique_ptr<Action>> newHistory;
            newHistory.reserve(maxHistorySize);
            newHistory.insert(newHistory.end(), keepStart, actionHistory.end());
            actionHistory = std::move(newHistory);
        }
    }
};

// Update action history with memory management
    void addToActionHistory(std::unique_ptr<Action> action) {
        // Add new action to history
        actionHistory.push_back(std::move(action));
        Timestamp now = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        
        ActionType actionType = actionHistory.back()->getType();
        if (actionStats.find(actionType) != actionStats.end()) {
            actionStats[actionType].lastExecutedTime = now;
            actionStats[actionType].visitCount++;
        }
        
        cleanupHistory();
    }
    
    // Update action statistics based on reward/cost
    void updateActionStats(ActionType action, double reward, double cost, bool success) {
        if (actionStats.find(action) != actionStats.end()) {
            auto& stats = actionStats[action];
            if (success) {
                stats.successCount++;
                stats.lastSuccessTime = std::chrono::duration_cast<std::chrono::duration<double>>(
                    std::chrono::steady_clock::now().time_since_epoch()).count();
            } else {
                stats.failureCount++;
                stats.lastFailureTime = std::chrono::duration_cast<std::chrono::duration<double>>(
                    std::chrono::steady_clock::now().time_since_epoch()).count();
            }
            
            stats.totalReward += reward;
            stats.totalCost += cost;
            stats.averageValue = (stats.averageValue * (stats.visitCount - 1) + (reward - cost)) / stats.visitCount;
            
            // Update value estimate (simple moving average)
            valueEstimates[action] = stats.averageValue;
            actionCounts[action]++;
        }
    }
    
    // Calculate combined score for an action
    double calculateActionScore(ActionType action, const NeuralRegion& motorRegion) const {
        double score = 0.0;
        
        // 1. Neural activity contribution
        double neuralActivityScore = 0.0;
        double totalActivity = 0.0;
        size_t neuronCount = 0;
        
        for (const auto& pop : motorRegion.getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                if (neuron && neuron->isFiring()) {
                    double firingRate = neuron->getFiringRate();
                    totalActivity += firingRate;
                    neuronCount++;
                }
            }
        }
        
        if (neuronCount > 0) {
            // Simple heuristic: higher average activity promotes movement actions
            double avgFiringRate = totalActivity / neuronCount;
            if (avgFiringRate > 30.0f) {  // High activity
                // Prefer movement actions during high activity
                if (action == ActionType::MoveForward || action == ActionType::MoveLeft || 
                    action == ActionType::MoveRight) {
                    neuralActivityScore += 1.0;
                } else if (action == ActionType::Look || action == ActionType::Interact) {
                    neuralActivityScore += 0.5;
                }
            }
        }
        
        score += neuralActivityScore * neuralActivityWeight;
        
        // 2. Action preference contribution
        double prefScore = 1.0;  // Default
        if (actionPreferences.find(action) != actionPreferences.end()) {
            prefScore = actionPreferences[action];
        }
        score += prefScore * preferenceWeight;
        
        // 3. Historical success contribution
        double historyScore = 0.0;
        if (actionStats.find(action) != actionStats.end()) {
            const auto& stats = actionStats[action];
            historyScore = stats.getSuccessRate() * stats.getNetValue() * historyWeight;
        }
        score += historyScore;
        
        // 4. Reward contribution (RL value)
        double rewardScore = 0.0;
        if (valueEstimates.find(action) != valueEstimates.end()) {
            rewardScore = valueEstimates[action] * rewardWeight;
        }
        score += rewardScore;
        
        return score;
    }
    
    // Action selection algorithms
    ActionType selectActionGreedy(const std::vector<ActionType>& candidates) const {
        ActionType bestAction = candidates[0];
        double bestScore = -std::numeric_limits<double>::max();
        
        for (const auto& action : candidates) {
            double score = calculateActionScore(action, motorRegion);
            if (score > bestScore) {
                bestScore = score;
                bestAction = action;
            }
        }
        
        return bestAction;
    }
    
    ActionType selectActionSoftmax(const std::vector<ActionType>& candidates) const {
        // Calculate scores for all candidates
        std::vector<std::pair<ActionType, double>> actionScores;
        actionScores.reserve(candidates.size());
        
        double maxScore = -std::numeric_limits<double>::max();
        for (const auto& action : candidates) {
            double score = calculateActionScore(action, motorRegion);
            actionScores.emplace_back(action, score);
            if (score > maxScore) maxScore = score;
        }
        
        // Compute softmax probabilities
        std::vector<double> probabilities;
        probabilities.reserve(actionScores.size());
        
        double sumExp = 0.0;
        for (const auto& pair : actionScores) {
            double expScore = std::exp((pair.second - maxScore) / explorationTemperature);
            probabilities.push_back(expScore);
            sumExp += expScore;
        }
        
        if (sumExp == 0.0) {
            // Fallback to uniform distribution
            return rng.pickRandom(candidates);
        }
        
        // Normalize to get probabilities
        for (auto& prob : probabilities) {
            prob /= sumExp;
        }
        
        // Sample from distribution
        double threshold = rng.uniformReal(0.0, 1.0);
        double cumulative = 0.0;
        for (size_t i = 0; i < actionScores.size(); ++i) {
            cumulative += probabilities[i];
            if (threshold <= cumulative) {
                return actionScores[i].first;
            }
        }
        
        return actionScores.back().first; // Should not reach here
    }
    
    ActionType selectActionThompsonSampling(const std::vector<ActionType>& candidates) const {
        // Use action counts as prior for Thompson sampling
        std::vector<double> probabilities;
        probabilities.reserve(candidates.size());
        
        double totalCount = 0.0;
        for (const auto& action : candidates) {
            totalCount += static_cast<double>(actionCounts.find(action) != actionCounts.end() ? 
                                              actionCounts.at(action) : 1);
        }
        
        // Compute empirical success probability for each action
        for (const auto& action : candidates) {
            double prob = 1.0;
            if (actionStats.find(action) != actionStats.end()) {
                const auto& stats = actionStats[action];
                prob = stats.getSuccessRate();
            }
            // Add beta distribution parameters (success+1, failure+1)
            if (actionStats.find(action) != actionStats.end()) {
                const auto& stats = actionStats[action];
                double alpha = stats.successCount + 1.0;
                double beta = stats.failureCount + 1.0;
                
                // Sample from Beta distribution using inverse transform
                double u = rng.uniformReal(0.0, 1.0);
                double x = std::pow(u, 1.0 / alpha) / (std::pow(u, 1.0 / alpha) + std::pow(1.0 - u, 1.0 / beta));
                probabilities.push_back(x);
            }
        }
        
        if (probabilities.empty()) {
            return rng.pickRandom(candidates);
        }
        
        // Normalize probabilities
        double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
        for (auto& prob : probabilities) {
            prob /= sum;
        }
        
        // Sample from distribution
        double threshold = rng.uniformReal(0.0, 1.0);
        double cumulative = 0.0;
        size_t idx = 0;
        for (const auto& prob : probabilities) {
            cumulative += prob;
            if (threshold <= cumulative) {
                return candidates[idx];
            }
            idx++;
        }
        
        return candidates.back(); // Should not reach here
    }
    
    ActionType selectAction(const NeuralRegion& motorRegion) {
        if (availableActions.empty()) {
            return ActionType::Wait;
        }
        
        // Store motorRegion for use in score calculations
        this->motorRegion = &motorRegion;
        
        std::vector<ActionType> candidates;
        if (useSoftmax) {
            // Softmax selection with exploration
            candidates = availableActions;
        } else if (useThompsonSampling) {
            // Thompson sampling
            candidates = availableActions;
        } else {
            // Epsilon-greedy or deterministic
            candidates = availableActions;
            
            if (rng.bernoulli(epsilonGreedyRate)) {
                // Explore randomly
                return rng.pickRandom(candidates);
            }
        }
        
        ActionType selectedAction;
        if (useSoftmax) {
            selectedAction = selectActionSoftmax(candidates);
        } else if (useThompsonSampling) {
            selectedAction = selectActionThompsonSampling(candidates);
        } else {
            selectedAction = selectActionGreedy(candidates);
        }
        
        return selectedAction;
    }
