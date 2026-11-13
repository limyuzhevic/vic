#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace nlm {

struct Norepinephrine::Impl {
    // Current NE level (phasic component)
    float phasicLevel;
    
    // Tonic NE level (baseline)
    float tonicLevel;
    
    // Alertness state
    bool isAlert;
    float currentAlertness;
    
    // Time constants
    static constexpr float ALERT_DURATION = 500.0f;        // ms - alert duration
    static constexpr float CALM_DOWN_TIME = 1000.0f;       // ms - calm down time
    static constexpr float MAX_ALERTNESS = 3.0f;           // Maximum alertness
    static constexpr float MIN_ALERTNESS = 0.1f;           // Minimum alertness
    
    Impl() : 
        phasicLevel(0.1f),
        tonicLevel(0.05f),
        isAlert(false),
        currentAlertness(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->tonicLevel + pImpl->phasicLevel;
}

void Norepinephrine::setLevel(float level) {
    pImpl->tonicLevel = std::clamp(level * 0.6f, 0.01f, Impl::MAX_ALERTNESS * 0.6f);
    pImpl->phasicLevel = std::clamp(level * 0.4f, Impl::MIN_ALERTNESS, Impl::MAX_ALERTNESS * 0.4f);
}

float Norepinephrine::getPlasticityFactor() const {
    // PHASE 2 FIX: Real NE modulation of plasticity
    // High NE increases plasticity during alert states
    
    float plasticityFactor = 1.0f;
    
    if (pImpl->isAlert) {
        plasticityFactor *= 1.8f;
    }
    
    // Moderate alertness still enhances plasticity
    if (pImpl->currentAlertness > 0.5f) {
        plasticityFactor *= 1.2f;
    }
    
    return plasticityFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // PHASE 2 FIX: Real NE dynamics
    // Natural decay and alertness management
    
    float timeFactor = static_cast<float>(dt) / 0.001f;
    
    // Natural decay
    float decayRate = 1.0f - std::exp(-timeFactor / 100.0f);
    pImpl->phasicLevel *= (1.0f - decayRate * 0.08f);
    
    // Calm down after alert
    if (pImpl->isAlert) {
        pImpl->currentAlertness = std::max(Impl::MIN_ALERTNESS,
                                          pImpl->currentAlertness - timeFactor / ALERT_DURATION);
        if (pImpl->currentAlertness <= Impl::MIN_ALERTNESS + 0.01f) {
            calmDown();
        }
    }
}

float Norepinephrine::getExcitabilityModulator() const {
    // PHASE 2 FIX: NE modulates neural excitability and signal detection
    float modulator = 1.0f;
    
    if (pImpl->isAlert) {
        modulator *= 1.5f;
    } else if (pImpl->currentAlertness > 0.3f) {
        modulator *= 1.2f;
    }
    
    return modulator;
}

void Norepinephrine::triggerAlert(float intensity) {
    pImpl->isAlert = true;
    pImpl->currentAlertness = std::min(Impl::MAX_ALERTNESS, intensity);
}

void Norepinephrine::calmDown() {
    pImpl->isAlert = false;
    pImpl->currentAlertness = Impl::MIN_ALERTNESS;
}

float Norepinephrine::getAlertness() const {
    return pImpl->currentAlertness;
}

} // namespace nlm