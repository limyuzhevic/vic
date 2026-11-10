// #include "Reward.hpp"
//
// namespace nlm {
//
// struct Reward::Impl {
//     float currentValue;
//     float accumulatedReward;
//     std::vector<float> history;
//     
//     Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
// };
//
// Reward::Reward() : pImpl(new Impl) {}
//
// Reward::~Reward() = default;
//
// float Reward::getValue() const {
//     return pImpl->currentValue;
// }
//
// void Reward::setValue(float value) {
//     pImpl->currentValue = value;
// }
//
// void Reward::add(float delta) {
//     pImpl->accumulatedReward += delta;
//     pImpl->currentValue += delta;
// }
//
// void Reward::reset() {
//     pImpl->currentValue = 0.0f;
//     pImpl->accumulatedReward = 0.0f;
// }
//
// float Reward::computeReward(const Observation& observation) const {
//     // TODO PHASE 2: Implement real reward computation from observation
//     // PLACEHOLDER: Returns 0
//     return 0.0f;
// }
//
// const std::vector<float>& Reward::getHistory() const {
//     return pImpl->history;
// }
//
// void Reward::clearHistory() {
//     pImpl->history.clear();
// }
//
// } // namespace nlm

#include "Reward.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation,
                           const Brain& brain,
                           const Action& action,
                           float predictionError) const {
    // TODO: Implement real reward computation from observation
    // For now, return a placeholder reward based on action outcome
    float reward = 0.0f;
    
    // Simple placeholder: reward based on action type and prediction error
    if (action.getType() == ActionType::Interact) {
        reward += 10.0f;  // Positive reward for interaction
    } else if (action.getType() == ActionType::MoveForward) {
        reward += 5.0f;   // Moderate reward for movement
    }
    
    // Add prediction error component
    reward += predictionError * 2.0f;
    
    // Add observation-based reward (placeholder)
    // In a real implementation, this would analyze the sensory observation
    const auto& sensoryData = observation.getSensoryData();
    if (!sensoryData.empty()) {
        // Simple heuristic: sum of positive sensory values
        float sensoryReward = 0.0f;
        for (float value : sensoryData) {
            if (value > 0.5f) {
                sensoryReward += value * 2.0f;
            }
        }
        reward += sensoryReward;
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    return sum / pImpl->history.size();
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::max_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::min_element(pImpl->history.begin(), pImpl->history.end());
}

void Reward::applyRewardToPlasticity(float reward, Neuromodulator& neuromodulator) const {
    // Apply reward to plasticity via neuromodulator
    neuromodulator.setLevel(reward);
}

} // namespace nlm
