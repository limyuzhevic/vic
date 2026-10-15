#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulsivityFactor;
    float serotonergicBurst;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.06f)
        , moodLevel(0.5f)
        , impulsivityFactor(1.0f)
        , serotonergicBurst(1.5f) {}
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
    // 5-HT modulates plasticity for mood-dependent learning and impulse control
    float moodModulation = pImpl->level * pImpl->moodLevel;
    float impulsivityModulation = 1.0f / (1.0f + pImpl->impulsivityFactor * 2.0f);
    return 0.8f + moodModulation * 0.4f * impulsivityModulation;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Spike-based signaling
    if (pImpl->serotonergicBurst > 1.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level * pImpl->serotonergicBurst);
        pImpl->serotonergicBurst = 1.0f;
    }
}

void Serotonin::signalSpike() {
    pImpl->serotonergicBurst = 1.5f;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->serotonergicBurst = 1.0f;
}

void Serotonin::setMoodLevel(float level) {
    pImpl->moodLevel = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::setImpulsivityFactor(float factor) {
    pImpl->impulsivityFactor = factor;
}

} // namespace nlm
