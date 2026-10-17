#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.2f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , attentionBoost(0.3f)
        , memoryBoost(0.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attentional control");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionPlasticityFactor() const {
    // ACh enhances attention-related plasticity
    // Higher ACh = stronger attentional gating
    return 0.5f + 0.5f * pImpl->level;
}

float Acetylcholine::getMemoryPlasticityFactor() const {
    // ACh enhances memory encoding and consolidation
    // Important for working memory and episodic memory
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float intensity) {
    // Signal attentional focus on specific neural populations
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    NLM_LOG_DEBUG("Acetylcholine signals attentional focus: " + std::to_string(intensity));
}

void Acetylcholine::signalMemoryEncoding() {
    // Boost memory encoding when attention is high
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->attentionBoost * 0.5f);
    NLM_LOG_DEBUG("Acetylcholine enhances memory encoding");
}

void Acetylcholine::signalMemoryConsolidation() {
    // Boost memory consolidation (during rest/sleep)
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->memoryBoost * 0.3f);
    NLM_LOG_DEBUG("Acetylcholine enhances memory consolidation");
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
