#pragma once

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float arousalModulator;
    float vigilanceModulator;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , arousalModulator(1.0f)
        , vigilanceModulator(0.8f)
        , decayRate(0.2f)
        , releaseRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance
    float arousal = pImpl->level * pImpl->arousalModulator;
    float vigilance = pImpl->level * pImpl->vigilanceModulator;
    float totalMod = arousal + vigilance;
    return 0.3f + 0.7f * std::clamp(totalMod, 0.0f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE signals decay with higher time constant
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Increase NE level for increased alertness
    setLevel(std::min(pImpl->peak, getLevel() + arousal * pImpl->releaseRate * 0.8f));
}

void Norepinephrine::signalVigilance(float vigilance) {
    // Boost NE for improved focus and attention
    if (vigilance > 0.5f) {
        setLevel(std::min(pImpl->peak, getLevel() + vigilance * pImpl->releaseRate * 0.6f));
    }
}

} // namespace nlm
