// Enhanced Norepinephrine implementation
// Comprehensive norepinephrine neuromodulator for arousal and vigilance

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalLevel;
    float vigilanceLevel;
    float locusCoeruleusGain;
    
    // Enhanced norepinephrine state tracking
    float signalDetection;
    float attentionalFocus;
    float stressResponse;
    float fightOrFlight;
    float memoryEnhancement;
    
    Impl() : 
        level(0.0f), baseline(0.03f), arousalLevel(0.5f),
        vigilanceLevel(0.8f), locusCoeruleusGain(1.3f),
        signalDetection(1.0f), attentionalFocus(1.0f),
        stressResponse(1.0f), fightOrFlight(1.0f),
        memoryEnhancement(1.0f) {}
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
    // Norepinephrine enhances plasticity for arousal-related learning
    float baselinePlasticity = 0.3f;
    
    // Signal detection enhances plasticity in noisy environments
    float signalEffect = pImpl->signalDetection * 0.3f;
    
    // Memory enhancement under stress
    float memoryEffect = pImpl->memoryEnhancement * 0.4f;
    
    // Fight-or-flight response enhances threat-related learning
    float fightEffect = pImpl->fightOrFlight * 0.3f;
    
    return baselinePlasticity + signalEffect + memoryEffect + fightEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Enhanced norepinephrine dynamics with multiple components
    float dtFloat = static_cast<float>(dt);
    
    // Maintain arousal level with moderate decay
    pImpl->arousalLevel = std::max(pImpl->baseline,
        pImpl->arousalLevel - 0.05f * dtFloat);
    
    // Vigilance decays slower than arousal
    pImpl->vigilanceLevel = std::max(pImpl->baseline,
        pImpl->vigilanceLevel - 0.03f * dtFloat);
    
    // Overall level is weighted sum of arousal and vigilance
    pImpl->level = pImpl->arousalLevel * 0.6f + pImpl->vigilanceLevel * 0.4f;
}

// Enhanced norepinephrine methods implementation
float Norepinephrine::getArousalModulation() const {
    // Norepinephrine modulates arousal and alertness
    return pImpl->arousalLevel * getLevel() * 1.5f;
}

float Norepinephrine::getSignalDetectionFactor() const {
    // Norepinephrine enhances signal detection in noisy environments
    return pImpl->signalDetection * (1.0f + pImpl->arousalLevel * 0.5f);
}

float Norepinephrine::getLocusCoeruleusModulation() const {
    // Locus coeruleus firing rate modulation
    return pImpl->locusCoeruleusGain;
}

float Norepinephrine::getAttentionalFocusModulation() const {
    // Norepinephrine enhances attentional focus and selectivity
    return pImpl->attentionalFocus * (1.0f + pImpl->vigilanceLevel * 0.7f);
}

float Norepinephrine::getStressMemoryModulation() const {
    // Norepinephrine enhances memory encoding under stress
    return 1.0f + pImpl->stressResponse * 0.5f;
}

float Norepinephrine::getFightOrFlightModulation() const {
    // Fight-or-flight response modulation
    return pImpl->fightOrFlight;
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Norepinephrine::setArousalLevel(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
}

void Norepinephrine::setVigilanceLevel(float vigilance) {
    pImpl->vigilanceLevel = std::clamp(vigilance, 0.0f, 1.0f);
}

void Norepinephrine::setLocusCoeruleusGain(float gain) {
    pImpl->locusCoeruleusGain = std::clamp(gain, 0.5f, 2.0f);
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getLocusCoeruleusGain() const {
    return pImpl->locusCoeruleusGain;
}

} // namespace nlm