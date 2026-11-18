#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float impactOnArousal;
    float impactOnMemory;
    float impactOnAttention;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , impactOnArousal(1.0f)
        , impactOnMemory(0.3f)
        , impactOnAttention(0.4f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance, affecting plasticity globally
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float strength) {
    // NE signals increased arousal and alertness
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float strength) {
    // NE enhances vigilance and selective attention
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::setArousalImpact(float impact) {
    pImpl->impactOnArousal = impact;
}

void Norepinephrine::setMemoryImpact(float impact) {
    pImpl->impactOnMemory = impact;
}

void Norepinephrine::setAttentionImpact(float impact) {
    pImpl->impactOnAttention = impact;
}

float Norepinephrine::getArousalImpact() const {
    return pImpl->impactOnArousal;
}

float Norepinephrine::getMemoryImpact() const {
    return pImpl->impactOnMemory;
}

float Norepinephrine::getAttentionImpact() const {
    return pImpl->impactOnAttention;
}

} // namespace nlm
