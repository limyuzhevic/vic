#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float memoryBoost;
    float attentionModulation;
    float baseline;
    float peak;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , decayRate(0.05f)
        , memoryBoost(1.2f)
        , attentionModulation(1.5f)
        , baseline(0.0f)
        , peak(1.0f) {}
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
    // ACh enhances plasticity for learning new information but
    // reduces interference with old memories
    return 1.0f + pImpl->level * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // Increase ACh level based on attentional demands
    setLevel(std::min(pImpl->level + attentionLevel * 0.3f, 1.0f));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Boost memory consolidation during high ACh states
    if (pImpl->brain && pImpl->brain->getEpisodicMemory()) {
        // Increase storage rate
        // This would normally trigger consolidation of working memory to episodic
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm