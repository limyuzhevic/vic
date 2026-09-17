#include "Neuromodulator.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // 5-HT modulates plasticity for mood and decision making
    return 0.5f + 0.3f * pImpl->level - 0.2f * (pImpl->level * pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // Burst of serotonin during positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate);
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    // 5-HT inhibits impulsivity, lower levels mean higher impulsivity
    pImpl->level = std::max(0.0f, pImpl->level - impulsivityLevel * 0.3f * pImpl->releaseRate);
}

void Serotonin::signalRewardPrediction(float predictionLevel) {
    // 5-HT modulates reward prediction error
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionLevel * pImpl->releaseRate * 0.5f);
}

void Serotonin::enhanceSocialLearning(float socialStrength) {
    // 5-HT enhances social learning and behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStrength * pImpl->releaseRate * 0.7f);
}

} // namespace nlm