// Reward implementation
#include "Reward.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float value;
    float accumulated;
    std::vector<float> history;
    
    Impl() : value(0.0f), accumulated(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->value;
}

void Reward::setValue(float value) {
    pImpl->value = value;
    pImpl->accumulated += value;
}

void Reward::add(float delta) {
    pImpl->value += delta;
    pImpl->accumulated += delta;
    pImpl->history.push_back(delta);
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->value = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
