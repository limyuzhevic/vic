#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action preferences for exploration vs. exploitation
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Reward prediction and action evaluation
    float lastRewardPrediction = 0.0f;
    float rewardHistory[50]; // Rolling window of last 50 rewards
    int rewardHistoryIndex = 0;
    float averageReward = 0.0f;
    
    // Neural activity tracking
    std::vector<float> recentMotorActivity;
    float explorationTemperature = 1.0f; // For softmax action selection
    
    // Random number generator for exploration
    std::mt19937 rng;
    
    Impl() {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::LookUp,
            ActionType::LookDown,
            ActionType::Interact,
            ActionType::Eat,
            ActionType::Drink,
            ActionType::Rest,
            ActionType::Wait,
            ActionType::Custom
        };
        
        // Initialize action preferences
        resetPreferences();
        
        // Seed the random number generator
        std::random_device rd;
        rng.seed(rd());
        
        // Initialize reward history
        for (int i = 0; i < 50; ++i) {
            rewardHistory[i] = 0.0f;
        }
        
        // Initialize neural activity tracking
        recentMotorActivity.resize(15, 0.0f); // Last 15 time steps
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion, 
                                                  const Neuromodulator* rewardSignal) {
    // Get current neural activity from motor neuron populations
    std::vector<float> motorActivity;
    
    // Calculate activity for each motor neuron population
    for (const auto& population : motorRegion.getPopulations()) {
        // Get average firing rate or activity level
        float activity = population->getAverageFiringRate();
        motorActivity.push_back(activity);
    }
    
    // If no motor populations found, use default heuristic
    if (motorActivity.empty()) {
        // Fallback: use neural region statistics as proxy
        float avgFiring = motorRegion.getAverageFiringRate();
        float activeFraction = static_cast<float>(motorRegion.getFiringNeuronCount()) / 
                               motorRegion.getTotalNeuronCount();
        motorActivity = { avgFiring * 0.7f + activeFraction * 0.3f * 100.0f,  // Scale to match typical firing rates
                        0.5f,  // Default preference for movement
                        0.3f }; // Default tendency to explore
    }
    
    // Update neural activity tracking
    recentMotorActivity.erase(recentMotorActivity.begin());
    recentMotorActivity.push_back(calculateMotorActivityScore(motorActivity));
    
    // Calculate action scores based on neural activity, preferences, and reward
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (ActionType action : availableActions) {
        // Neural contribution
        float neuralScore = 0.0f;
        size_t actionIndex = static_cast<size_t>(action) % motorActivity.size();
        neuralScore = motorActivity[actionIndex];
        
        // Action preference contribution (exploration bias)
        float prefScore = pImpl->actionPreferences.count(action) ? 
            pImpl->actionPreferences[action] : 1.0f;
        
        // Reward prediction contribution
        float rewardScore = 1.0f;
        if (rewardSignal) {
            // Positive reward signal encourages actions that led to reward
            // Negative reward signal discourages actions that led to punishment
            rewardScore = std::max(0.1f, 0.5f + rewardSignal->getLevel() * 0.5f);
        }
        
        // Historical performance contribution
        float historicalScore = getHistoricalPerformance(action);
        
        // Combine scores with weighted factors
        float totalScore = neuralScore * 0.4f + 
                          prefScore * 0.2f + 
                          rewardScore * 0.3f + 
                          historicalScore * 0.1f;
        
        actionScores.push_back({action, totalScore});
    }
    
    // Apply softmax selection with exploration temperature
    std::vector<std::pair<ActionType, float>> sortedActions = actionScores;
    std::sort(sortedActions.begin(), sortedActions.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });n    
    // Calculate softmax probabilities
    float maxScore = sortedActions[0].second;
    float sumExp = 0.0f;
    std::vector<float> probabilities;
    
    for (const auto& actionScore : sortedActions) {
        float expScore = std::exp(actionScore.second / pImpl->explorationTemperature);
        probabilities.push_back(expScore);
        sumExp += expScore;
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= sumExp;
    }
    
    // Select action based on probabilities
    float randVal = std::uniform_real_distribution<float>(0.0f, 1.0f)(pImpl->rng);
    float cumulative = 0.0f;
    ActionType selectedAction = sortedActions[0].first;
    
    for (size_t i = 0; i < probabilities.size(); ++i) {
        cumulative += probabilities[i];
        if (randVal <= cumulative) {
            selectedAction = sortedActions[i].first;
            break;
        }
    }
    
    // Create and return the action
    auto action = std::make_unique<Action>(selectedAction);
    
    // Add to action history
    actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences[action] = std::clamp(preference, 0.1f, 10.0f);
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

void MotorSystem::update(float timestep) {
    // Update exploration temperature based on reward prediction stability
    // Lower temperature when rewards are predictable (exploit more)
    // Higher temperature when rewards are unpredictable (explore more)
    
    if (pImpl->rewardHistory[0] != 0.0f) { // If we have reward history
        float rewardVariance = 0.0f;
        float meanReward = pImpl->averageReward;
        
        // Calculate variance of recent rewards
        for (int i = 0; i < 50; ++i) {
            float diff = pImpl->rewardHistory[i] - meanReward;
            rewardVariance += diff * diff;
        }
        rewardVariance /= 50.0f;
        
        // Adjust exploration temperature
        pImpl->explorationTemperature = 1.0f + std::sqrt(rewardVariance) * 5.0f;
        pImpl->explorationTemperature = std::clamp(pImpl->explorationTemperature, 0.5f, 5.0f);
    }
    
    // Decay old action preferences over time
    for (auto& pref : pImpl->actionPreferences) {
        pref.second *= 0.99f; // Slow decay
        // Ensure minimum preference
        pref.second = std::max(pref.second, 1.0f);
    }
}

void MotorSystem::resetPreferences() {
    // Reset action preferences to neutral values
    for (ActionType action : availableActions) {
        setActionPreference(action, 1.0f);
    }
}

// Helper methods
float MotorSystem::Impl::calculateMotorActivityScore(const std::vector<float>& activities) const {
    if (activities.empty()) return 0.0f;
    
    // Calculate weighted score based on activity levels
    // Earlier activities (forward-related) get higher weight
    float score = 0.0f;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < activities.size(); ++i) {
        float weight = 1.0f / (1.0f + static_cast<float>(i)); // Decreasing weight with index
        score += activities[i] * weight;
        totalWeight += weight;
    }
    
    return score / totalWeight;
}

float MotorSystem::Impl::getHistoricalPerformance(ActionType action) const {
    // Calculate how well this action has performed historically
    // This is a simplified implementation - real version would track
    // reward per action and maintain success rates
    
    // For now, return a neutral score
    return 1.0f;
}

} // namespace nlm
