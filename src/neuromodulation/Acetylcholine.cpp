#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionBoost;
    float memoryConsolidation;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionBoost(1.0f)
        , memoryConsolidation(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances attention and learning
    // Higher ACh improves signal-to-noise ratio
    float attentionFactor = 1.0f + pImpl->level * pImpl->attentionBoost;
    float memoryFactor = 1.0f + pImpl->level * pImpl->memoryConsolidation;
    return (attentionFactor * memoryFactor) / 2.0f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost(float strength) {
    // Signal attention-boosting event (novel stimulus)
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate);
    NLM_LOG_INFO("Acetylcholine attention boost: " + std::to_string(strength));
}

void Acetylcholine::signalMemoryConsolidation(float efficacy) {
    // Signal memory consolidation event (during rest/sleep)
    pImpl->level = std::min(1.0f, pImpl->level + efficacy * 0.5f * pImpl->releaseRate);
    NLM_LOG_INFO("Acetylcholine memory consolidation: " + std::to_string(efficacy));
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm