#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // NE modulates arousal and stress response
    // Affects overall excitability and stress-based learning
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity, boost::shared_ptr<Brain> brain) {
    // TODO PHASE 2: Implement arousal signaling
    // PLACEHOLDER: Burst of NE for arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    if (brain) {
        // Apply arousal effects
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE increases arousal and tonic excitation
                    neuron->injectCurrent(pImpl->level * 0.2f);
                }
            }
        }
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
