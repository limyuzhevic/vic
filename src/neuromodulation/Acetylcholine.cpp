#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float impactOnAttention;
    float impactOnMemory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , impactOnAttention(0.5f)
        , impactOnMemory(0.8f) {}
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
    // ACh enhances plasticity for memory consolidation and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionShift(float strength) {
    // ACh signals shifts in attention and focus
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    // ACh promotes memory consolidation during awake periods
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::setAttentionImpact(float impact) {
    pImpl->impactOnAttention = impact;
}

void Acetylcholine::setMemoryImpact(float impact) {
    pImpl->impactOnMemory = impact;
}

float Acetylcholine::getAttentionImpact() const {
    return pImpl->impactOnAttention;
}

float Acetylcholine::getMemoryImpact() const {
    return pImpl->impactOnMemory;
}

} // namespace nlm
