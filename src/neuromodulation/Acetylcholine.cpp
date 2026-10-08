#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionGain;
    float memoryConsolidationRate;
    float decayRate;
    float baseline;
    float peak;
    float lastLearningEventTime;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionGain(1.0f)
        , memoryConsolidationRate(0.5f)
        , decayRate(0.2f)
        , baseline(0.1f)
        , peak(2.0f)
        , lastLearningEventTime(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
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
    // Acetylcholine modulates learning and attention
    // Higher levels enhance plasticity for attended inputs
    return 0.3f + 0.7f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayFactor = std::pow(1.0f - pImpl->decayRate, static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Apply memory consolidation effects
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Enhance working memory consolidation
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->strengthenMemory(1.0f + 0.5f * (pImpl->level - 0.5f));
        }
    }
}

void Acetylcholine::enhanceAttention(float boostFactor, float duration) {
    // Boost attention for a period of time
    pImpl->level = std::min(pImpl->peak, pImpl->level + boostFactor);
    pImpl->attentionGain = std::min(2.0f, pImpl->attentionGain + 0.5f * boostFactor);
    pImpl->lastLearningEventTime = duration;
}

void Acetylcholine::resetAttentionGain() {
    // Gradually return attention gain to baseline
    pImpl->attentionGain = std::max(1.0f, pImpl->attentionGain - 0.1f);
}

} // namespace nlm