#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../world/Observation.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// Reward implementation
struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float predictedValue;
    float learningRate;
    float discountFactor;
    float errorThreshold;
    
    RewardMode rewardMode;
    
    std::vector<float> history;
    std::vector<float> predictionHistory;
    
    std::unique_ptr<RandomGenerator> rng;
    
    Impl()
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , predictedValue(0.0f)
        , learningRate(RewardConstants::TD_LEARNING_RATE)
        , discountFactor(RewardConstants::TD_GAMMA)
        , errorThreshold(PredictionErrorConstants::PREDICTION_ERROR_THRESHOLD)
        , rewardMode(RewardMode::Goals)
        , rng(nullptr) {
        rng = std::make_unique<RandomGenerator>(42);
        history.reserve(RewardConstants::REWARD_HISTORY_SIZE);
        predictionHistory.reserve(RewardConstants::REWARD_HISTORY_SIZE);
    }
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return std::clamp(currentValue / RewardConstants::REWARD_MAX, 0.0f, 1.0f);
}

void Reward::setLevel(float level) {
    currentValue = level * RewardConstants::REWARD_MAX;
}

float Reward::getPlasticityFactor() const {
    // Reward-based plasticity depends on prediction error magnitude
    // Small errors can enhance learning, large errors can disrupt it
    return 1.0f + std::abs(currentValue - predictedValue) * 0.1f;
}

void Reward::update(TimestepDuration dt) {
    // Smooth current value with exponential moving average
    float target = accumulatedReward;
    currentValue += (target - currentValue) * RewardConstants::REWARD_SMOOTHING_ALPHA;
}

void Reward::reset() {
    currentValue = 0.0f;
    accumulatedReward = 0.0f;
    predictedValue = 0.0f;
    history.clear();
    predictionHistory.clear();
}

bool Reward::isActive() const {
    return std::abs(currentValue) > 0.1f;
}

float Reward::computeReward(const Observation& observation) const {
    return computeReward(observation.getData());
}

float Reward::computeReward(const std::vector<float>& sensoryPattern) const {
    float reward = 0.0f;
    
    switch (rewardMode) {
        case RewardMode::Collisions:
            // Check for collisions in sensory data
            // For now, use a simple heuristic: negative reward if any sensor reading is too high
            for (float value : sensoryPattern) {
                if (value > 0.8f) {
                    reward -= 10.0f;
                }
            }
            break;
            
        case RewardMode::Goals:
            // Check for goal achievement
            // Look for specific patterns indicating goal reached
            // For now, use maximum sensor reading as proxy for proximity
            if (!sensoryPattern.empty()) {
                float maxValue = *std::max_element(sensoryPattern.begin(), sensoryPattern.end());
                if (maxValue > 0.9f) {
                    reward += 100.0f;
                } else if (maxValue > 0.7f) {
                    reward += 10.0f;
                }
            }
            break;
            
        case RewardMode::Proximity:
            // Based on proximity to targets
            if (!sensoryPattern.empty()) {
                float avgValue = std::accumulate(sensoryPattern.begin(), sensoryPattern.end(), 0.0f) / 
                               static_cast<float>(sensoryPattern.size());
                reward = avgValue * 50.0f;  // Convert to reward
            }
            break;
            
        case RewardMode::Custom:
            // Default to proximity calculation
            if (!sensoryPattern.empty()) {
                float avgValue = std::accumulate(sensoryPattern.begin(), sensoryPattern.end(), 0.0f) / 
                               static_cast<float>(sensoryPattern.size());
                reward = avgValue * 50.0f;
            }
            break;
    }
    
    return reward;
}

void Reward::add(float delta) {
    accumulatedReward += delta;
    history.push_back(delta);
    if (history.size() > RewardConstants::REWARD_HISTORY_SIZE) {
        history.erase(history.begin());
    }
    
    // Update prediction error for next step
    updatePrediction(computePredictionError(delta));
}

float Reward::getAverageReward() const {
    if (history.empty()) return 0.0f;
    float sum = std::accumulate(history.begin(), history.end(), 0.0f);
    return sum / static_cast<float>(history.size());
}

float Reward::getTotalReward() const {
    return accumulatedReward;
}

size_t Reward::getRewardCount() const {
    return history.size();
}

const std::vector<float>& Reward::getHistory() const {
    return history;
}

void Reward::clearHistory() {
    history.clear();
    predictionHistory.clear();
}

void Reward::updatePrediction(float value) {
    // TD learning: update predicted value towards actual
    float predictionError = value - predictedValue;
    predictedValue += predictionError * learningRate;
    
    predictionHistory.push_back(predictionError);
    if (predictionHistory.size() > RewardConstants::REWARD_HISTORY_SIZE) {
        predictionHistory.erase(predictionHistory.begin());
    }
}

float Reward::getPredictedValue() const {
    return predictedValue;
}

float Reward::computePredictionError(float actual) const {
    return actual - predictedValue;
}

float Reward::getLearningRateModulation() const {
    return learningRate;
}

void Reward::setRewardMode(RewardMode mode) {
    rewardMode = mode;
}

} // namespace nlm
