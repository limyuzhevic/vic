#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 0.0f, 1.0f);
}

void Dopamine::setDecayRate(float decayRate) {
    pImpl->decayRate = std::max(0.0f, decayRate);
}

void Dopamine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = std::max(0.0f, releaseRate);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

const char* Dopamine::getDynamicsDescription() const {
    return "Placeholder: Linear decay to baseline, reward-dependent increase with bounded release rate";
}

std::string Dopamine::getStateDescription() const {
    std::ostringstream oss;
    oss << "DA Level: " << pImpl->level << ", Baseline: " << pImpl->baseline 
        << ", Peak: " << pImpl->peak << ", Decay Rate: " << pImpl->decayRate 
        << ", Release Rate: " << pImpl->releaseRate;
    return oss.str();
}

} // namespace nlm
