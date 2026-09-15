#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionLevel;
    float memoryStability;
    float baseline;
    float decayRate;
    float attentionBoostRate;
    float memoryConsolidationRate;
    float workingMemoryStrength;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , attentionLevel(0.5f)
        , memoryStability(0.5f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , attentionBoostRate(0.5f)
        , memoryConsolidationRate(0.3f)
        , workingMemoryStrength(0.5f) {}
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
    // ACh modulates working memory plasticity
    // Higher ACh increases synaptic plasticity for memory-related circuits
    return 0.3f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth attention and memory stability based on level
    pImpl->attentionLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->memoryStability = 0.2f + 0.8f * pImpl->level;
}

void Acetylcholine::enhanceAttention(float focusWeight) {
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + focusWeight * pImpl->attentionBoostRate);
}

void Acetylcholine::modulateWorkingMemory(float memoryStrength) {
    pImpl->workingMemoryStrength = std::clamp(memoryStrength, 0.0f, 1.0f);
    // Boost current level based on memory strength
    pImpl->level = std::min(1.0f, pImpl->level + memoryStrength * 0.2f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::consolidateMemory(float memoryStability) {
    pImpl->memoryStability = std::clamp(memoryStability, 0.0f, 1.0f);
    // Enhance current level for memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + memoryStability * 0.3f);
}

void Acetylcholine::enhanceLongTermMemory(float consolidationFactor) {
    pImpl->level = std::min(1.0f, pImpl->level + consolidationFactor * pImpl->memoryConsolidationRate);
}

float Acetylcholine::getMemoryStability() const {
    return pImpl->memoryStability;
}

} // namespace nlm