#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodWeight;
    float impulsivityWeight;
    float rewardSensitivity;
    float satiationThreshold;
    
    Impl() : 
        brain(nullptr),
        level(0.0f),
        baseline(0.0f),
        peak(1.0f),
        decayRate(0.08f),
        releaseRate(1.0f),
        moodWeight(0.6f),
        impulsivityWeight(0.4f),
        rewardSensitivity(0.8f),
        satiationThreshold(0.7f) {}
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
    // Serotonin affects mood, impulse control, and learning modulation
    // Moderate levels optimize learning, high levels can reduce impulsivity
    float learningModulation = 1.0f - std::abs(0.5f - pImpl->level) * 2.0f; // Inverted U-shape
    return std::max(0.1f, learningModulation);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: maintains tone level, modulates with environmental feedback
    if (pImpl->level > 0.0f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
}

void Serotonin::signalSatiation(float rewardValue) {
    // Serotonin responds to reward prediction errors and satiation
    if (rewardValue < 0.0f) {  // Negative prediction error
        setLevel(std::min(1.0f, getLevel() + (-rewardValue) * pImpl->rewardSensitivity * 0.5f));
    }
}

void Serotonin::reduceImpulsivity(float duration) {
    // Serotonin reduces impulsive actions, promotes persistence
    setLevel(std::min(1.0f, getLevel() + duration * 0.03f));
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity * pImpl->level;
}

float Serotonin::getImpulsivity() const {
    return std::max(0.0f, 1.0f - pImpl->level);  // Inverse relationship
}

} // namespace nlm