#include "Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float socialGain;
    float socialInterval;
    float socialTimer;
    float socialResponse;
    class RandomGenerator* rng;
    
    Impl()
        : level(0.15f)  // Baseline 5-HT level
        , baseline(0.15f)
        , decayRate(0.03f)
        , socialGain(0.3f)
        , socialInterval(3.0f)
        , socialTimer(3.0f)
        , socialResponse(0.2f)
        , rng(nullptr) {}
};

Serotonin::Serotonin() : pImpl(new Impl()) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.5f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects plasticity: low 5-HT may increase impulsivity, moderate levels enhance learning
    float normalized = std::min(1.0f, pImpl->level / 1.5f);
    return 0.6f + 0.4f * std::pow(normalized, 0.5f);  // Sublinear relationship
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: responding to social feedback and rewards
    float decayRate = pImpl->decayRate;
    float baseline = pImpl->baseline;
    
    // Decay toward baseline
    pImpl->level = std::max(baseline, pImpl->level - decayRate * static_cast<float>(dt));
    
    // Stochastic fluctuations for mood variability
    if (pImpl->rng) {
        float fluctuation = pImpl->rng->uniformReal(-0.0015f, 0.0015f) * static_cast<float>(dt);
        pImpl->level += fluctuation;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 2.5f);
    }
    
    // Social feedback responses
    pImpl->socialTimer -= static_cast<float>(dt);
    if (pImpl->socialTimer <= 0.0f) {
        pImpl->level += pImpl->socialResponse * pImpl->socialGain;
        pImpl->socialTimer = pImpl->socialInterval;
    }
}

void Serotonin::processSocialFeedback(float feedback) {
    // Positive social feedback increases serotonin
    // Negative feedback decreases it
    pImpl->level += feedback * pImpl->socialGain;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.5f);
    
    // Update social timer
    pImpl->socialTimer = pImpl->socialInterval;
}

} // namespace nlm