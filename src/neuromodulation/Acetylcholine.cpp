#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float attentionBoost;
    float memoryBoost;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , attentionBoost(1.0f)
        , memoryBoost(1.0f)
        , decayRate(0.1f) {}
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
    // Acetylcholine enhances both attention and memory consolidation
    // Higher ACh increases learning rate and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Signal attentional focus
    pImpl->level = std::min(1.0f, pImpl->level + attention * 0.5f);
}

void Acetylcholine::signalMemory(float memoryStrength) {
    // Signal memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + memoryStrength * 0.3f);
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

float Acetylcholine::getMemoryBoost() const {
    return pImpl->memoryBoost;
}

void Acetylcholine::setAttentionBoost(float boost) {
    pImpl->attentionBoost = boost;
}

void Acetylcholine::setMemoryBoost(float boost) {
    pImpl->memoryBoost = boost;
}

} // namespace nlm