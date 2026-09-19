#include "Reward.hpp"
#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    class Brain* brain;
    
    // Enhanced reward computation parameters
    float rewardPredictionError;
    float predictionWeight;
    float noveltyContribution;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), brain(nullptr),
             rewardPredictionError(0.0f), predictionWeight(0.7f), noveltyContribution(0.3f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
}

void Reward::integratePredictionError(float predictionError) {
    // Integrate prediction error with reward computation
    // Positive prediction error increases reward value
    pImpl->rewardPredictionError = predictionError;
    
    // Apply prediction error modulation to reward
    // Better predictions lead to smaller rewards (prediction error)
    // Unexpected outcomes produce larger rewards
    float modulatedReward = std::max(0.0f, predictionError * 0.5f);
    
    // Add modulated reward
    pImpl->currentValue += modulatedReward;
    pImpl->accumulatedReward += modulatedReward;
    
    // History tracks both reward and prediction error
    if (!pImpl->history.empty()) {
        pImpl->history.push_back(predictionError);
    }
}

void Reward::integrateNovelty(float novelty, TimestepDuration dt) {
    // Novelty contributes to reward as exploration bonus
    float noveltyBonus = novelty * pImpl->noveltyContribution;
    
    // Decay novelty contribution over time
    float noveltyEffect = noveltyBonus * (1.0f - pImpl->decayRate * static_cast<float>(dt));
    
    // Add novelty-based exploration reward
    pImpl->currentValue += noveltyEffect;
    pImpl->accumulatedReward += noveltyEffect;
    
    pImpl->history.push_back(novelty);
}

void Reward::integratePrediction(float predictedValue, float actualValue) {
    // Real reward prediction error (RPE) computation
    // Based on temporal difference learning: RPE = actual - predicted
    float predictionError = actualValue - predictedValue;
    
    // Normalize prediction error to reasonable range
    // Can be positive (better than expected) or negative (worse than expected)
    predictionError = std::clamp(predictionError, -1.0f, 1.0f);
    
    // Apply prediction error to reward system
    integratePredictionError(predictionError);
    
    // Store prediction data for learning
    pImpl->rewardPredictionError = predictionError;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation from observation
    // Extract relevant features from observation
    const auto& data = observation.getData();
    
    if (data.empty()) {
        return 0.0f;
    }
    
    // Simple reward heuristic based on observation values
    // Positive observations (good states) produce positive rewards
    // Negative observations (bad states) produce negative rewards
    float reward = 0.0f;
    
    for (float value : data) {
        // Normalize input to reward range [-1, 1]
        float normalized = std::clamp(value / 10.0f, -1.0f, 1.0f);
        
        // Apply reward function: exponential decay for negative rewards
        if (normalized >= 0) {
            reward += normalized * 0.5f;  // Positive rewards linear
        } else {
            reward += normalized * 0.2f;  // Negative rewards weaker
        }
    }
    
    // Average over number of features
    reward = reward / data.size();
    
    return reward;
}

void Reward::applyExplorationBonus(float novelty, TimestepDuration dt) {
    // Apply exploration bonus for novel experiences
    float explorationBonus = novelty * 0.5f * (1.0f - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->currentValue += explorationBonus;
    pImpl->accumulatedReward += explorationBonus;
    
    pImpl->history.push_back(explorationBonus);
}

void Reward::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::resetDecay() {
    pImpl->decayRate = 0.05f;
}

void Reward::increaseDecay() {
    pImpl->decayRate = 0.1f;
}

} // namespace nlm
