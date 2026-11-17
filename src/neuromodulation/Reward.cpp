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
}

float Reward::computeReward(const Observation& observation) const {
        // TODO PHASE 2: Implement real reward computation from observation
        // PLACEHOLDER: Returns 0
        return 0.0f;
    }

    const char* Reward::getName() const {
        return "R";
    }

    float Reward::getLevel() const {
        return getValue();
    }

    void Reward::setLevel(float level) {
        setValue(level);
    }

    float Reward::getPlasticityFactor() const {
        // TODO PHASE 2: Implement real reward-modulated plasticity factor
        // PLACEHOLDER: Higher reward increases plasticity
        return 0.5f + 0.5f * getValue();
    }

    void Reward::update(TimestepDuration dt) {
        // TODO PHASE 2: Implement real reward dynamics
        // PLACEHOLDER: Decay towards zero
        if (pImpl->currentValue > 0.0f) {
            pImpl->currentValue = std::max(0.0f, pImpl->currentValue - 0.1f * static_cast<float>(dt));
        }
    }

    void Reward::signalReward(float reward) {
        // TODO PHASE 2: Implement real reward signaling
        // PLACEHOLDER: Accumulate reward
        pImpl->currentValue = std::min(1.0f, pImpl->currentValue + reward);
        pImpl->accumulatedReward += reward;
        pImpl->history.push_back(reward);
        if (pImpl->history.size() > 100) {
            pImpl->history.erase(pImpl->history.begin());
        }
    }
