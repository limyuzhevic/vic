#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float rewardPredictionError;
    float explorationFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), mood(0.5f), impulsivity(0.5f), rewardPredictionError(0.5f), explorationFactor(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity by reducing impulsivity and enhancing reward prediction
    // Higher 5-HT decreases impulsivity and affects reward-based plasticity
    return 0.5f + 0.3f * pImpl->level * pImpl->rewardPredictionError;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalRewardEvent(float reward) {
    // Serotonin responds to reward and modulates reward prediction error
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->rewardPredictionError = std::min(1.0f, pImpl->rewardPredictionError + reward * 0.5f);
}

void Serotonin::signalSocialEvent() {
    // Serotonin responds to social interactions
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.8f * pImpl->releaseRate);
}

void Serotonin::signalPredictionErrorEvent(float error) {
    // Serotonin responds to prediction errors and modulates exploration
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->explorationFactor = std::min(1.0f, pImpl->explorationFactor + std::abs(error) * 0.3f);
}

void Serotonin::reduceImpulsivity() {
    // Higher serotonin reduces impulsivity
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - 0.1f);
}

} // namespace nlm