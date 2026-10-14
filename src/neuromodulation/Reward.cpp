#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Reward::Impl {
    class Brain* brain;
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : brain(nullptr), currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

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
    NLM_LOG_INFO("Reward added: " + std::to_string(delta) + ", total: " + std::to_string(pImpl->currentValue));
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // Simple placeholder: analyze observation data for rewards
    if (!pImpl->brain) return 0.0f;
    
    const std::vector<float>& data = observation.getData();
    
    // Simple reward function based on observation values
    float reward = 0.0f;
    for (size_t i = 0; i < data.size(); ++i) {
        // Positive values give reward, negative values give penalty
        if (data[i] > 0.5f) {
            reward += data[i] * 0.1f;
        } else if (data[i] < -0.5f) {
            reward -= std::abs(data[i]) * 0.1f;
        }
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
