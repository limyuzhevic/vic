#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include "../core/Utils.hpp"

namespace nlm {

struct Reward::Impl {
    float current_value;
    float accumulated_reward;
    std::vector<float> history;
    
    Impl() : current_value(0.0f), accumulated_reward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->current_value;
}

void Reward::setValue(float value) {
    pImpl->current_value = value;
}

void Reward::add(float delta) {
    pImpl->accumulated_reward += delta;
    pImpl->current_value += delta;
}

void Reward::reset() {
    pImpl->current_value = 0.0f;
    pImpl->accumulated_reward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
