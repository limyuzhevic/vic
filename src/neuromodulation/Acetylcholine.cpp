#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionStrength;
    float memoryConsolidationRate;
    float wmEncodingEfficiency;
    float currentFocusLevel;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), 
          decayRate(0.05f), attentionStrength(1.0f),
          memoryConsolidationRate(0.5f), wmEncodingEfficiency(0.8f),
          currentFocusLevel(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine system initialized");
}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
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
    // ACh affects working memory encoding and attention
    // Higher ACh improves working memory capacity and focus
    return 1.0f + pImpl->wmEncodingEfficiency * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: slower decay, modulates attention
    // Time constant ~500ms (decayRate = 0.05 per ms)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->currentFocusLevel = pImpl->level; // ACh directly modulates focus
}

void Acetylcholine::signalEvent(float magnitude) {
    // Event signaling affects attention and memory
    if (magnitude > 0.0f) {
        // Novel events or attention-grabbing stimuli increase ACh
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * 1.0f);
        pImpl->wmEncodingEfficiency = std::min(1.0f, pImpl->wmEncodingEfficiency + magnitude * 0.1f);
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->wmEncodingEfficiency = 0.8f;
    pImpl->currentFocusLevel = 0.0f;
}

float Acetylcholine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Acetylcholine::getPeakLevel() const {
    return pImpl->peak;
}

void Acetylcholine::signalAttention(float focusLevel) {
    // Increases attention to specific stimuli
    pImpl->currentFocusLevel = focusLevel;
    // ACh release enhances cortical excitability for attended items
    if (focusLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + focusLevel * 0.5f);
    }
}

void Acetylcholine::signalTaskSwitching(float switchCost) {
    // High switch cost leads to increased ACh release
    if (switchCost > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
        pImpl->wmEncodingEfficiency = std::max(0.5f, pImpl->wmEncodingEfficiency - 0.2f);
    }
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh release during rest promotes memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
    pImpl->memoryConsolidationRate = std::min(1.0f, pImpl->memoryConsolidationRate + 0.1f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->currentFocusLevel;
}

float Acetylcholine::getWMEncodingEfficiency() const {
    return pImpl->wmEncodingEfficiency;
}

} // namespace nlm
