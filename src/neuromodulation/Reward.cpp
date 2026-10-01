#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    std::weak_ptr<class Brain> brain;
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : brain(nullptr), currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(std::make_unique<Impl>()) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    // Check if the brain is still valid
    if (!brain) {
        return;
    }
    
    // Convert to shared_ptr for weak_ptr
    pImpl->brain = brain;
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
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
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
