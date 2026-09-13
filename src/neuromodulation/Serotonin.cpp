#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardBoost;
    float socialBoost;
    float inhibitionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), releaseRate(1.0f),
             rewardBoost(1.3f), socialBoost(1.2f), inhibitionLevel(0.9f) {}
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
    // 5-HT modulates mood-related plasticity
    // Reduces impulsivity, enhances social learning
    float moodFactor = 1.0f + pImpl->rewardBoost * pImpl->level * 0.3f;
    float socialFactor = 1.0f + pImpl->socialBoost * pImpl->level * 0.4f;
    return moodFactor * socialFactor * pImpl->inhibitionLevel;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSatiety(float satiety) {
    // 5-HT signals satiety and reward
    // Reduces approach behavior towards goals
    if (satiety > 0.0f) {
        setLevel(std::min(pImpl->level + satiety * 0.4f, pImpl->peak));
    }
}

void Serotonin::signalSocialReward(float reward) {
    // Social reward signals through serotonin
    // Enhances social learning and cooperation
    if (reward > 0.0f) {
        setLevel(pImpl->level + reward * 0.3f);
    }
}

void Serotonin::signalMood(float mood) {
    // Direct mood modulation
    if (mood > pImpl->level) {
        setLevel(mood);
    }
}

} // namespace nlm
