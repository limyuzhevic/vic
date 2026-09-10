#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances attention and memory consolidation
    // Higher ACh increases plasticity for attended items
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(boost::shared_ptr<Brain> brain) {
    // TODO PHASE 2: Implement attention signaling
    // PLACEHOLDER: Burst of ACh for attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
    if (brain) {
        // Apply attention effects
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // ACh increases arousal and sensory gain
                    neuron->injectCurrent(pImpl->level * 0.3f);
                }
            }
        }
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
