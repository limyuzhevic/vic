#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}
Reward::~Reward() = default;

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->history.push_back(pImpl->level);
}

float Reward::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void Reward::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Reward::signalReward(float reward) {
    pImpl->accumulatedReward += reward;
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->history.push_back(pImpl->level);
}

void Reward::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->history.push_back(pImpl->level);
}

} // namespace nlm