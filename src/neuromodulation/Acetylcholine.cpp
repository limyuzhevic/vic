#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionGain;
    float consolidationRate;
    float attentionDecay;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.1f), attentionGain(2.0f), 
             consolidationRate(0.8f), attentionDecay(0.05f), memoryBoost(1.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl), attentionEnabled_(true), consolidationRate_(0.5f) {}

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
    // ACh modulates plasticity for learning and memory
    float attentionMod = attentionEnabled_ ? pImpl->attentionGain : 1.0f;
    return 1.0f + (pImpl->level - 1.0f) * attentionMod * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay back to baseline
    pImpl->level = std::max(pImpl->baseline, 
        pImpl->level - pImpl->attentionDecay * static_cast<float>(dt));
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    // ACh enhances memory consolidation
    return pImpl->memoryBoost * pImpl->level;
}

float Acetylcholine::getAttentionFocusFactor() const {
    // ACh improves attention focus
    return 0.5f + pImpl->level * 0.5f;
}

void Acetylcholine::signalLearningEvent() {
    // Sharp increase in ACh for learning
    pImpl->level = std::min(1.0f, pImpl->level + 0.3f);
    NLM_LOG_INFO("Acetylcholine signaling learning event, level: " + std::to_string(pImpl->level));
}

} // namespace nlm
