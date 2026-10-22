// Norepinephrine: Arousal and vigilance neuromodulator
// Biological role: Increases alertness, enhances signal-to-noise ratio, modulates attention

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float alertness;
    float signalEnhance;
    float noiseReduce;
    float decayRate;
    float peak;
    
    // Arousal state
    float currentArousal;
    float arousalTarget;
    float vigilance;
    
    // Signal processing state
    float signalStrength;
    float noiseLevel;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , alertness(0.7f)
        , signalEnhance(0.9f)
        , noiseReduce(0.8f)
        , decayRate(0.15f)
        , peak(1.0f)
        , currentArousal(0.5f)
        , arousalTarget(0.5f)
        , vigilance(0.5f)
        , signalStrength(1.0f)
        , noiseLevel(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(class Brain* brain) {
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
    // NE modulates plasticity based on arousal state
    float arousalModulation = pImpl->alertness * 0.5f;
    return 0.5f + 0.5f * pImpl->level * arousalModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Update arousal level
    pImpl->arousalTarget = 0.5f + 0.5f * pImpl->level;
    pImpl->currentArousal += (pImpl->arousalTarget - pImpl->currentArousal) * 0.05f;
    
    // Update vigilance (ability to maintain attention)
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level * pImpl->alertness;
    
    // Update signal enhancement and noise reduction
    pImpl->signalStrength = 1.0f + pImpl->level * pImpl->signalEnhance;
    pImpl->noiseLevel = 0.5f * (1.0f - pImpl->level * pImpl->noiseReduce);
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::enhanceAlertness() {
    // Boost arousal and vigilance
    if (pImpl->brain) {
        pImpl->currentArousal = std::min(1.0f, pImpl->currentArousal + 0.1f);
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.1f);
        
        // Store in brain for integration
        if (auto* config = pImpl->brain->getConfig()) {
            config->set("arousal_level", pImpl->currentArousal);
            config->set("vigilance_level", pImpl->vigilance);
        }
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->currentArousal;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilance;
}

float Norepinephrine::getSignalToNoiseRatio() const {
    return pImpl->signalStrength / pImpl->noiseLevel;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->currentArousal = 0.5f;
    pImpl->arousalTarget = 0.5f;
    pImpl->vigilance = 0.5f;
    pImpl->signalStrength = 1.0f;
    pImpl->noiseLevel = 0.5f;
}

} // namespace nlm