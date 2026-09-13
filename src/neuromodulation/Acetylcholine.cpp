#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBias;
    float memoryConsolidation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.2f)
        , releaseRate(1.5f)
        , attentionBias(0.5f)
        , memoryConsolidation(0.5f) {}
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
    // ACh enhances plasticity for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::modulateAttention(AttentionalSelection* attention, float bias) {
    if (!attention) return;
    
    // ACh enhances signal-to-noise ratio in attention
    float attentionModulation = pImpl->level * bias;
    attention->setInhibitionStrength(0.5f - attentionModulation * 0.3f);
    attention->setExcitationStrength(1.5f + attentionModulation * 0.5f);
}

void Acetylcholine::enhanceMemoryConsolidation(NeuralEpisodicMemory* episodicMemory, float strength) {
    if (!episodicMemory) return;
    
    // ACh enhances memory consolidation
    float consolidation = pImpl->level * strength;
    episodicMemory->setConsolidationRate(std::min(1.0f, episodicMemory->getConsolidationRate() + consolidation * 0.2f));
}

void Acetylcholine::enhanceWorkingMemory(NeuralWorkingMemory* workingMemory, float factor) {
    if (!workingMemory) return;
    
    // ACh enhances working memory maintenance
    float memoryEnhance = pImpl->level * factor;
    workingMemory->setDecayRate(std::max(0.0f, workingMemory->getDecayRate() - memoryEnhance * 0.1f));
}

void Acetylcholine::signalAttentionEvent() {
    // Burst of acetylcholine for attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryEvent() {
    // Burst of acetylcholine for memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f * pImpl->releaseRate);
}

void Acetylcholine::integrateWithDopamine(float dopamineLevel) {
    // ACh and dopamine have synergistic effects on attention
    if (dopamineLevel > 0.5f) {
        pImpl->attentionBias = std::min(1.0f, pImpl->attentionBias + dopamineLevel * 0.1f);
    }
}

void Acetylcholine::integrateWithPredictionError(float predictionError) {
    // ACh responds to prediction errors for learning
    if (predictionError > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * 0.2f);
    }
}

} // namespace nlm
