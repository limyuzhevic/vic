// Norepinephrine.cpp - Arousal and alertness implementation

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalModulator;
    float attentionModulator;
    float vigilanceModulator;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalModulator(0.7f)
        , attentionModulator(0.6f)
        , vigilanceModulator(0.8f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f) {}
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
    // NE enhances alertness and modulates plasticity
    return 0.4f + pImpl->level * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Increase arousal (wake/attention)
void Norepinephrine::increaseArousal(Brain* brain, float arousalDelta) {
    if (!brain) return;
    
    // Increase NE level
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalDelta);
    
    // Enhance attention through neuromodulation
    if (auto* attention = brain->getAttention()) {
        // NE increases excitation for focused attention
        float currentStrength = attention->getExcitationStrength();
        attention->setExcitationStrength(currentStrength * (1.0f + pImpl->attentionModulator * pImpl->level));
    }
    
    // Enhance working memory
    if (auto* workingMem = brain->getWorkingMemory()) {
        // NE improves working memory maintenance
        workingMem->strengthenMemory(1.0f + pImpl->arousalModulator * pImpl->level);
    }
}

// Enhance vigilance (monitoring for salient events)
void Norepinephrine::enhanceVigilance(Brain* brain, float vigilanceDelta) {
    if (!brain) return;
    
    // Increase vigilance level
    float currentVigilance = pImpl->vigilanceModulator * pImpl->level;
    
    // Could enhance prediction error detection
    // Higher vigilance leads to more frequent prediction error signaling
    NLM_LOG_DEBUG("Norepinephrine::enhanceVigilance: Vigilance increased to " << currentVigilance);
}

// Signal stress or challenge
void Norepinephrine::signalStress(float stressLevel) {
    // Stress increases NE levels
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 0.8f);
}

} // namespace nlm
