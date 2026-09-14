#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalGain;
    float vigilanceGain;
    float stressModulator;
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(1.0f)
        , decayRate(0.06f)
        , arousalGain(1.0f)
        , vigilanceGain(0.8f)
        , stressModulator(0.5f)
        , arousalLevel(0.5f)
        , vigilanceLevel(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateEffects();
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity based on arousal level
    float arousal = getArousalLevel();
    if (arousal < 0.3f) {
        return 0.8f;  // Low arousal: reduced plasticity
    } else if (arousal < 0.7f) {
        return 1.2f;  // Moderate arousal: enhanced plasticity
    } else {
        return 0.9f;  // High arousal: stress reduces plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateEffects();
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressModulator;
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Norepinephrine::setPeak(float peak) {
    pImpl->peak = peak;
}

void Norepinephrine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Norepinephrine::setArousalGain(float gain) {
    pImpl->arousalGain = gain;
    updateEffects();
}

void Norepinephrine::setVigilanceGain(float gain) {
    pImpl->vigilanceGain = gain;
    updateEffects();
}

void Norepinephrine::setStressModulator(float modulator) {
    pImpl->stressModulator = modulator;
}

void Norepinephrine::updateEffects() {
    // Calculate neuromodulatory effects based on current norepinephrine level
    float normalizedLevel = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    normalizedLevel = std::clamp(normalizedLevel, 0.0f, 1.0f);
    
    // Arousal: NE increases alertness and sensory gain
    pImpl->arousalLevel = 0.5f + 0.5f * normalizedLevel * pImpl->arousalGain;
    
    // Vigilance: NE enhances focus and sustained attention
    pImpl->vigilanceLevel = 0.3f + 0.7f * normalizedLevel * pImpl->vigilanceGain;
    
    // Stress: High NE can induce stress response
    pImpl->stressModulator = normalizedLevel;
}

} // namespace nlm