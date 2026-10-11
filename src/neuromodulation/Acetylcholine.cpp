#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation boost
    float memoryConsolidationBoost;
    
    // Attention modulation
    float attentionModulation;
    
    // Arousal level
    float arousalLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , memoryConsolidationBoost(0.0f)
        , attentionModulation(0.0f)
        , arousalLevel(0.0f) {}
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
    // ACh enhances memory consolidation and attention
    // Higher ACh increases effective learning rate
    float attentionEffect = pImpl->attentionModulation * 0.5f;
    float memoryEffect = pImpl->memoryConsolidationBoost * 0.3f;
    float arousalEffect = pImpl->arousalLevel * 0.2f;
    return 0.5f + attentionEffect + memoryEffect + arousalEffect;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decay towards baseline, respond to stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->arousalLevel = pImpl->level;
}

float Acetylcholine::getMemoryConsolidationBoost() const {
    return pImpl->memoryConsolidationBoost;
}

void Acetylcholine::setMemoryConsolidationBoost(float boost) {
    pImpl->memoryConsolidationBoost = std::clamp(boost, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

void Acetylcholine::setAttentionModulation(float modulation) {
    pImpl->attentionModulation = std::clamp(modulation, 0.0f, 1.0f);
}

float Acetylcholine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Acetylcholine::setArousalLevel(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
}

} // namespace nlm
