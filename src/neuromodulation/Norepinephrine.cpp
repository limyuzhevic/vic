#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float arousalGain;
    float focusModulation;
    float baseline;
    float peak;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , decayRate(0.05f)
        , arousalGain(1.3f)
        , focusModulation(1.2f)
        , baseline(0.0f)
        , peak(1.0f) {}
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
    // NE increases signal-to-noise ratio and modulates arousal
    return 1.0f + pImpl->level * 0.4f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Increase NE level based on arousal (novelty, stress, etc.)
    setLevel(std::min(1.0f, pImpl->level + arousalLevel * 0.3f));
}

void Norepinephrine::enhanceFocus(const std::vector<NeuronId>& neurons, float intensity) {
    // Increase firing rates of attended neurons (focus effect)
    if (pImpl->brain) {
        for (const auto& neuronId : neurons) {
            pImpl->brain->injectCurrent(neuronId, intensity * pImpl->focusModulation);
        }
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm