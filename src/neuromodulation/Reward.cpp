#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() {
    delete pImpl;
}

void Reward::initialize(class Brain* brain) {
    // Reward doesn't use brain, but we can store it for future use if needed
    if (!pImpl) return;
    // Store brain reference for future implementation
    (void)brain; // Avoid unused parameter warning
}

float Reward::getValue() const {
    if (!pImpl) return 0.0f;
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    if (!pImpl) return;
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    if (!pImpl) return;
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    if (!pImpl) return;
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    if (!pImpl) static const std::vector<float> empty{};
    return pImpl->history;
}

void Reward::clearHistory() {
    if (!pImpl) return;
    pImpl->history.clear();
}

} // namespace nlm
