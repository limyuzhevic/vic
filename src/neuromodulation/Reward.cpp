#include "BasePimpl.hpp"
#include "Reward.hpp"

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
    return pImpl->get()->currentValue;
}

void Reward::setValue(float value) {
    pImpl->get()->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->get()->accumulatedReward += delta;
    pImpl->get()->currentValue += delta;
}

void Reward::reset() {
    pImpl->get()->currentValue = 0.0f;
    pImpl->get()->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->get()->history;
}

void Reward::clearHistory() {
    pImpl->get()->history.clear();
}

} // namespace nlm
