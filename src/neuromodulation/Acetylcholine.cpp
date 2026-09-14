#pragma once

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float attentionModulator;
    float memoryModulator;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , attentionModulator(1.0f)
        , memoryModulator(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f) {}
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
    float attention = pImpl->level * pImpl->attentionModulator;
    float memory = pImpl->level * pImpl->memoryModulator;
    float totalMod = attention + memory;
    return 0.5f + 0.5f * std::clamp(totalMod, 0.0f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh signals get weaker over time (decay)
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Increase ACh level proportionally to attention demand
    setLevel(getLevel() + attention * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::signalMemory(uint64_t neuronId, float memoryStrength) {
    // Boost ACh for memory encoding
    if (memoryStrength > 0.1f) {
        setLevel(std::min(pImpl->peak, getLevel() + memoryStrength * pImpl->releaseRate * 0.3f));
    }
}

} // namespace nlm
