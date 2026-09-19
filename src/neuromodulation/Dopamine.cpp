#include "Dopamine.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardPredictionError;
    float learningSignal;
    float rewardHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , rewardPredictionError(0.0f)
        , learningSignal(0.0f)
        , rewardHistory(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity based on reward prediction error
    // Positive prediction errors increase plasticity for learned behaviors
    // Negative prediction errors decrease plasticity (stabilization)
    return 0.5f + pImpl->rewardPredictionError * 0.5f;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update reward prediction error signal
    pImpl->rewardPredictionError = pImpl->learningSignal;
}

void Dopamine::signalReward(float reward) {
    // Direct reward signaling (phased)
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->rewardHistory = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (key dopamine function)
    pImpl->learningSignal = error;
    pImpl->rewardPredictionError = error;
    
    // Update dopamine level based on prediction error
    if (error > 0.5f) {
        // Positive prediction error (better than expected) -> dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.0f);
    } else if (error > 0.0f) {
        // Small positive prediction error -> moderate increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate);
    } else if (error < -0.5f) {
        // Large negative prediction error -> dopamine dip
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->learningSignal = 0.0f;
    pImpl->rewardHistory = 0.0f;
}

} // namespace nlm