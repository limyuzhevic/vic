#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // 5-HT modulates mood and impulsivity
    // Affects decision-making and social behavior
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodChange, boost::shared_ptr<Brain> brain) {
    // TODO PHASE 2: Implement mood signaling
    // PLACEHOLDER: Burst of 5-HT for mood change
    pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(moodChange) * pImpl->releaseRate);
    if (brain) {
        // Apply mood effects
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 5-HT modulates impulsivity and decision-making
                    neuron->injectCurrent(pImpl->level * 0.15f);
                }
            }
        }
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
