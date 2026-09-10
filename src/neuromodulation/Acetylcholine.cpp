#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionModulation;
    float memoryConsolidationStrength;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , attentionModulation(1.5f)
        , memoryConsolidationStrength(2.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (ACh) system initialized - attention and memory modulator");
}

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
    // ACh enhances plasticity for attention-based learning
    return 0.8f + 0.4f * pImpl->level;  // Range: [0.8, 1.2]
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // ACh bursts when attention is focused
    float burst = attentionStrength * pImpl->attentionModulation;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    NLM_LOG_INFO("Acetylcholine signal: attention strength = " + std::to_string(attentionStrength) + 
                 ", ACh level = " + std::to_string(pImpl->level));
}

void Acetylcholine::enhanceMemoryConsolidation(float memoryStrength) {
    // ACh strengthens memory consolidation
    float consolidation = memoryStrength * pImpl->memoryConsolidationStrength;
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidation);
    
    NLM_LOG_INFO("Acetylcholine enhancing memory consolidation: " + std::to_string(memoryStrength) + 
                 ", ACh level = " + std::to_string(pImpl->level));
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation * (1.0f + pImpl->level);
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidationStrength * (1.0f + pImpl->level);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm