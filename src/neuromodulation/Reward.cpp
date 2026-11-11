#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float accumulatedReward;
    float energy;
    float novelty;
    float resultingReward;
    std::vector<float> history;
    
    Impl() : accumulatedReward(0.0f), energy(0.0f), novelty(0.0f), resultingReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->accumulatedReward;
}

void Reward::setValue(float value) {
    pImpl->accumulatedReward = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->accumulatedReward = 0.0f;
    pImpl->currentValue = 0.0f;
    pImpl->energy = 0.0f;
    pImpl->novelty = 0.0f;
    pImpl->resultingReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on observation
    // Implements reinforcement learning principles: reward as a function of
    // observation (state) and learned value function
    
    // Extract features from observation
    const std::vector<float>& observationData = observation.getData();
    if (observationData.empty()) {
        return 0.0f;
    }
    
    // Calculate reward based on observation features
    // This is a simplified model - real implementation would use
    // learned value functions or neural networks
    
    float reward = 0.0f;
    
    // 1. Energy-based reward (higher energy = negative reward)
    float energy = observation.getEnergy();
    if (energy > 0.0f) {
        reward -= energy * 0.1f;  // Cost of energy expenditure
    }
    
    // 2. Novelty-based reward (unexpected states get positive reward)
    float novelty = observation.getNovelty();
    if (novelty > 0.0f) {
        reward += novelty * 0.5f;
    }
    
    //  intravascular optics
    // 3. Resulting reward from action outcomes
    float resulting = observation.getResultingReward();
    if (resulting > 0.0f) {
        reward += resulting * 0.3f;
    }
    
    // 4. Position-based reward (if goal state is defined)
    auto position = observation.getPosition();
    if (position.has_value()) {
        const auto& pos = position.value();
        // If we have a goal position (e.g., center of environment), reward proximity
        float centerReward = 0.0f;
        if (pos.x > 0.0f && pos.x < 10.0f && pos.y > 0.0f && pos.y < 10.0f) {
            // Within bounds, reward proximity to center
            float distToCenter = std::sqrt((pos.x - 5.0f) * (pos.x - 5.0f) + 
                                          (pos.y - 5.0f) * (pos.y - 5.0f));
            centerReward = std::max(0.0f, 1.0f - distToCenter / 5.0f);
        }
        reward += centerReward * 0.2f;
    }
    
    // 5. Reward prediction error (difference from expected reward)
    // This would require a learned value function, simplified here
    float predictionError = calculateRewardPredictionError(observation);
    reward += predictionError * 0.1f;
    
    // Store in history
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Log reward computation
    if (std::abs(reward) > 0.01f) {
        NLM_LOG_INFO("Reward computed: " + std::to_string(reward) + 
                    " (energy=" + std::to_string(energy) + 
                    ", novelty=" + std::to_string(novelty) + ")");
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Helper method to calculate reward prediction error
float Reward::calculateRewardPredictionError(const Observation& observation) const {
    // Simplified reward prediction error calculation
    // In a real implementation, this would use a learned value function
    
    float currentReward = computeReward(observation);
    
    // Get average reward from history
    float averageReward = 0.0f;
    if (!pImpl->history.empty()) {
        float sum = 0.0f;
        for (float r : pImpl->history) {
            sum += r;
        }
        averageReward = sum / pImpl->history.size();
    }
    
    // Prediction error = actual - expected
    return currentReward - averageReward;
}

} // namespace nlm