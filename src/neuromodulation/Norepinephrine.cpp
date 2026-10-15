#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceGain;
    float noradrenergicBurst;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.07f)
        , arousalLevel(0.5f)
        , vigilanceGain(1.0f)
        , noradrenergicBurst(1.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal-dependent learning
    float arousalModulation = pImpl->level * pImpl->arousalLevel;
    float vigilanceModulation = pImpl->vigilanceGain;
    return 1.0f + arousalModulation * 0.3f * vigilanceModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Spike-based signaling
    if (pImpl->noradrenergicBurst > 1.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level * pImpl->noradrenergicBurst);
        pImpl->noradrenergicBurst = 1.0f;
    }
}

void Norepinephrine::signalSpike() {
    pImpl->noradrenergicBurst = 1.5f;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->noradrenergicBurst = 1.0f;
}

void Norepinephrine::setArousalLevel(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::setVigilanceGain(float gain) {
    pImpl->vigilanceGain = gain;
}

} // namespace nlm
