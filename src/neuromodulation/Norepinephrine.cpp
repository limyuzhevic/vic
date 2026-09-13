#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.15f)
        , releaseRate(2.0f)
        , arousal(0.5f)
        , vigilance(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

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
    // NE enhances plasticity for arousal and learning
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::modulateArousal(float arousalLevel) {
    // NE modulates arousal state
    pImpl->arousal = std::clamp(arousalLevel, 0.0f, 1.0f);
    if (pImpl->arousal > 0.7f) {
        // High arousal increases NE level
        setLevel(std::min(pImpl->peak, getLevel() + 0.2f * pImpl->releaseRate));
    }
}

void Norepinephrine::enhanceVigilance(AttentionalSelection* attention, float vigilanceBoost) {
    if (!attention) return;
    
    // NE enhances vigilance and alertness
    float vigilanceEffect = pImpl->level * vigilanceBoost;
    attention->setInhibitionStrength(std::max(0.0f, attention->getInhibitionStrength() - vigilanceEffect * 0.3f));
    attention->setExcitationStrength(std::min(2.0f, attention->getExcitationStrength() + vigilanceEffect * 0.3f));
}

void Norepinephrine::integrateWithDopamine(float dopamineLevel) {
    // NE and dopamine have synergistic effects on attention and arousal
    if (dopamineLevel > 0.5f && pImpl->level > 0.5f) {
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + dopamineLevel * 0.1f);
    }
}

void Norepinephrine::integrateWithAcetylcholine(float achLevel) {
    // NE and ACh have complementary effects - NE for arousal, ACh for attention focus
    if (achLevel > 0.5f && pImpl->level > 0.5f) {
        // High NE + high ACh = optimal cognitive state
        setLevel(1.0f);
    }
}

} // namespace nlm
