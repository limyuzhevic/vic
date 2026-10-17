#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionBoost;
    float memoryConsolidation;
    float decayRate;
    float acetylcholineHistory;
    
    Impl() : level(0.0f), attentionBoost(0.0f), memoryConsolidation(0.0f), 
             decayRate(0.05f), acetylcholineHistory(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine system initialized");
}

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
    // Acetylcholine modulates plasticity for memory formation
    // Higher ACh enhances memory-related plasticity
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply attention boost effect to neural excitability
    // ACh increases gain in cortical circuits
    if (pImpl->level > 0.0f) {
        NLM_LOG_DEBUG("Acetylcholine modulating attention: level=" + std::to_string(pImpl->level));
    }
}

void Acetylcholine::enhanceAttention() {
    // Rapid increase in acetylcholine for attentional focus
    pImpl->level = std::min(1.0f, pImpl->level + 0.3f);
    pImpl->attentionBoost = 0.2f * pImpl->level;
    pImpl->acetylcholineHistory = pImpl->level;
    
    NLM_LOG_DEBUG("Acetylcholine attention boost: level=" + std::to_string(pImpl->level));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Acetylcholine promotes memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
    pImpl->memoryConsolidation = 0.3f * pImpl->level;
    pImpl->acetylcholineHistory = pImpl->level;
    
    NLM_LOG_DEBUG("Acetylcholine memory consolidation: level=" + std::to_string(pImpl->level));
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

float Acetylcholine::getMemoryConsolidationBoost() const {
    return pImpl->memoryConsolidation;
}

float Acetylcholine::getTotalSignal() const {
    return pImpl->level + pImpl->acetylcholineHistory * 0.5f;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionBoost = 0.0f;
    pImpl->memoryConsolidation = 0.0f;
    pImpl->acetylcholineHistory = 0.0f;
}

} // namespace nlm