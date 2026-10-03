#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Reward::Impl {
    class Brain* brain;
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , currentValue(0.0f)
        , accumulatedReward(0.0f) {}
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

void Reward::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
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

// Override Neuromodulator interface
const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->currentValue;
}

void Reward::setLevel(float level) {
    pImpl->currentValue = level;
}

float Reward::getPlasticityFactor() const {
    // Reward modulates plasticity based on accumulated reward
    // Higher cumulative reward increases plasticity
    float cumulativeReward = pImpl->accumulatedReward;
    return 0.5f + 0.5f * std::min(1.0f, cumulativeReward / 10.0f);
}

void Reward::update(TimestepDuration dt) {
    // Decay reward slowly over time
    pImpl->currentValue = std::max(0.0f, pImpl->currentValue - 0.01f * static_cast<float>(dt));
}

} // namespace nlm
