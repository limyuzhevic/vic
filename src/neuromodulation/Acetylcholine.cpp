#include "../Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionModulator;
    float memoryConsolidationRate;
    float decayRate;
    
    Impl() : level(0.0f), attentionModulator(1.0f), memoryConsolidationRate(0.0f), decayRate(0.1f) {}
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
    // ACh directly modulates attention - higher levels = better focus
    pImpl->attentionModulator = 0.5f + 0.5f * pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-driven learning
    float basePlasticity = 1.0f;
    return basePlasticity + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Lower ACh enhances memory consolidation during rest
    if (pImpl->level < 0.1f) {
        pImpl->memoryConsolidationRate = 0.01f;
    } else {
        pImpl->memoryConsolidationRate = 0.001f;
    }
}

void Acetylcholine::signalAttention(float novelty, float predictionError) {
    // Enhance ACh in response to novel information
    float attentionSignal = novelty * 0.7f + std::abs(predictionError) * 0.3f;
    pImpl->level = std::min(1.0f, pImpl->level + attentionSignal * 0.5f);
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Active ACh consolidation during rest periods
    if (pImpl->level > 0.3f) {
        pImpl->memoryConsolidationRate += 0.005f;
    }
}

float Acetylcholine::getAttentionModulator() const {
    return pImpl->attentionModulator;
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

} // namespace nlm