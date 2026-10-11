#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionModulation;
    float memoryEnhancement;
    float decayRate;
    std::vector<float> history;
    
    Impl() : brain(nullptr), level(0.0f), attentionModulation(0.5f), memoryEnhancement(0.8f), decayRate(0.1f) {}
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
    // ACh enhances LTP (long-term potentiation) for attention and memory
    float factor = 1.0f + pImpl->attentionModulation * pImpl->level * 0.5f;
    factor += pImpl->memoryEnhancement * pImpl->level * 0.3f;
    return factor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay acetylcholine
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->history.push_back(pImpl->level);
}

void Acetylcholine::signalAttentionFocus(RegionId region) {
    // Boost attention to specific region
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
}

void Acetylcholine::enhanceMemoryConsolidation(float relevance) {
    // Enhance memory consolidation based on relevance
    pImpl->level = std::min(1.0f, pImpl->level + relevance * 0.3f);
}

void Acetylcholine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
