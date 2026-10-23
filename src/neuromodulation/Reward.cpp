#include "Reward.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward components
    float sensoryReward;
    float memoryReward;
    float predictionReward;
    float goalReward;
    
    // Reward learning parameters
    float rewardAlpha;      // Learning rate for reward updates
    float rewardGamma;      // Discount factor for future rewards
    float explorationBonus; // Bonus for novel experiences
    
    // Reward history for temporal difference learning
    std::vector<float> tdErrorHistory;
    float lastPredictionError;
    
    // Reward shaping parameters
    float noveltyThreshold;     // Threshold for considering experience novel
    float stabilityWeight;      // Weight for stable experiences
    float noveltyWeight;        // Weight for novel experiences
    float predictionWeight;    // Weight for prediction errors
    
    Impl() : 
        currentValue(0.0f), accumulatedReward(0.0f),
        sensoryReward(0.0f), memoryReward(0.0f), predictionReward(0.0f), goalReward(0.0f),
        rewardAlpha(0.1f), rewardGamma(0.95f), explorationBonus(0.1f),
        lastPredictionError(0.0f),
        noveltyThreshold(0.7f), stabilityWeight(0.6f), noveltyWeight(0.3f), predictionWeight(0.5f) {}
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
    pImpl->history.push_back(delta);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->tdErrorHistory.clear();
    pImpl->sensoryReward = 0.0f;
    pImpl->memoryReward = 0.0f;
    pImpl->predictionReward = 0.0f;
    pImpl->goalReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    float totalReward = 0.0f;
    
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const auto& sensoryData = sensoryInput->getData();
    if (sensoryData.empty()) {
        return 0.0f;
    }
    
    // Compute different reward components
    float sensoryComp = computeSensoryReward(sensoryData);
    float memoryComp = computeMemoryReward(sensoryData);
    float predictionComp = computePredictionReward(sensoryData);
    float goalComp = computeGoalReward(sensoryData);
    
    // Weighted sum of reward components
    totalReward = sensoryComp * pImpl->noveltyWeight +
                  memoryComp * pImpl->stabilityWeight +
                  predictionComp * pImpl->predictionWeight +
                  goalComp * 0.2f; // Lower weight for goal reward
    
    // Add exploration bonus for novel patterns
    float novelty = computeNovelty(sensoryData);
    if (novelty > pImpl->noveltyThreshold) {
        totalReward += pImpl->explorationBonus * novelty;
    }
    
    // Temporal difference learning
    float tdError = totalReward - pImpl->lastPredictionError;
    pImpl->lastPredictionError = totalReward;
    pImpl->tdErrorHistory.push_back(tdError);
    
    // Keep TD error history bounded
    if (pImpl->tdErrorHistory.size() > 100) {
        pImpl->tdErrorHistory.erase(pImpl->tdErrorHistory.begin());
    }
    
    // Update accumulated reward
    pImpl->sensoryReward = sensoryComp;
    pImpl->memoryReward = memoryComp;
    pImpl->predictionReward = predictionComp;
    pImpl->goalReward = goalComp;
    
    return totalReward;
}

float Reward::computeSensoryReward(const std::vector<float>& sensoryData) const {
    // Compute reward based on sensory features
    if (sensoryData.empty()) return 0.0f;
    
    float maxValue = *std::max_element(sensoryData.begin(), sensoryData.end());
    float minValue = *std::min_element(sensoryData.begin(), sensoryData.end());
    
    // Reward for strong sensory responses
    float intensityReward = (maxValue - minValue) / 2.0f;
    
    // Reward for balanced sensory input (avoid saturation)
    float balanceReward = 1.0f - std::abs(maxValue - 0.5f) * 2.0f;
    
    // Combine rewards
    float reward = intensityReward * 0.7f + balanceReward * 0.3f;
    
    // Apply sigmoid to keep reward in reasonable range
    reward = 1.0f / (1.0f + std::exp(-5.0f * (reward - 0.5f)));
    
    return reward * 2.0f - 1.0f; // Center around 0, range [-1, 1]
}

float Reward::computeMemoryReward(const std::vector<float>& sensoryData) const {
    // This would ideally integrate with working memory and episodic memory
    // For now, provide a placeholder implementation based on pattern coherence
    
    if (sensoryData.size() < 2) return 0.0f;
    
    // Compute pattern coherence (how similar adjacent values are)
    float coherence = 0.0f;
    size_t pairs = std::min(size_t(10), sensoryData.size() - 1);
    
    for (size_t i = 0; i < pairs; ++i) {
        float diff = std::abs(sensoryData[i] - sensoryData[i + 1]);
        coherence += (1.0f - diff);
    }
    
    coherence /= pairs;
    
    // Reward for coherent patterns (indicating predictable environment)
    float reward = coherence * 0.5f + 0.5f;
    
    return reward * 2.0f - 1.0f; // Range [-1, 1]
}

float Reward::computePredictionReward(const std::vector<float>& sensoryData) const {
    // This would ideally integrate with prediction system
    // For now, return prediction error-based reward
    
    float predictionError = pImpl->lastPredictionError;
    
    // Negative reward for large prediction errors (surprise)
    // But small reward for moderate errors (learning signal)
    float reward = -std::abs(predictionError) * 0.5f;
    
    // Add positive component for errors within learning range
    if (std::abs(predictionError) < 0.3f) {
        reward += (0.3f - std::abs(predictionError)) * 0.5f;
    }
    
    return reward;
}

float Reward::computeGoalReward(const std::vector<float>& sensoryData) const {
    // This would ideally integrate with goal system and planning
    // For now, provide a placeholder based on target patterns
    
    if (sensoryData.empty()) return 0.0f;
    
    // Look for reward-relevant patterns (e.g., values approaching 1.0)
    float maxValue = *std::max_element(sensoryData.begin(), sensoryData.end());
    
    // Reward for approaching goal state
    float goalReward = std::min(maxValue / 0.8f, 1.0f);
    
    // Small bonus for exceeding goal
    if (maxValue > 1.2f) {
        goalReward += (maxValue - 1.2f) * 0.5f;
    }
    
    return goalReward * 2.0f - 1.0f; // Range [-1, 1]
}

float Reward::computeNovelty(const std::vector<float>& pattern) const {
    // Simple novelty detection based on pattern uniqueness
    // In a full implementation, this would use episodic memory
    
    if (pattern.empty()) return 0.0f;
    
    // Compute pattern complexity (variance)
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    // Normalize variance to [0, 1]
    float normalizedVariance = std::min(variance * 10.0f, 1.0f);
    
    return normalizedVariance;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
