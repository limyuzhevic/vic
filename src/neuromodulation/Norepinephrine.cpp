#include "Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float surpriseGain;
    float noveltySurge;
    float noveltyInterval;
    float noveltyTimer;
    class RandomGenerator* rng;
    
    Impl()
        : level(0.1f)  // Baseline NE level
        , baseline(0.1f)
        , decayRate(0.02f)
        , surpriseGain(0.5f)
        , noveltySurge(0.3f)
        , noveltyInterval(5.0f)
        , noveltyTimer(5.0f)
        , rng(nullptr) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl()) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 3.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE affects plasticity: moderate NE enhances LTP, high NE may promote LTD
    if (pImpl->level < 0.5f) {
        return 0.8f + 0.2f * pImpl->level;  // Enhancement
    } else {
        return 1.2f - 0.3f * (pImpl->level - 0.5f);  // Suppression
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: responding to stress, novelty, and arousal
    float decayRate = pImpl->decayRate;
    float baseline = pImpl->baseline;
    
    // Decay toward baseline
    pImpl->level = std::max(baseline, pImpl->level - decayRate * static_cast<float>(dt));
    
    // Random fluctuations for vigilance
    if (pImpl->rng) {
        float fluctuation = pImpl->rng->uniformReal(-0.002f, 0.002f) * static_cast<float>(dt);
        pImpl->level += fluctuation;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 3.0f);
    }
    
    // Brief surges for novelty detection
    pImpl->noveltyTimer -= static_cast<float>(dt);
    if (pImpl->noveltyTimer <= 0.0f) {
        pImpl->level += pImpl->noveltySurge;
        pImpl->noveltyTimer = pImpl->noveltyInterval;
    }
}

void Norepinephrine::signalSurprise(float surpriseLevel) {
    pImpl->level += surpriseLevel * pImpl->surpriseGain;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 3.0f);
    
    // Update novelty timer
    pImpl->noveltyTimer = pImpl->noveltyInterval;
}

} // namespace nlm