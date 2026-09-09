#include "Norepinephrine.hpp"
#include <algorithm>
#include <random>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                    // Current NE level (0.0-2.0)
    float baseline;                  // Baseline tonic firing
    float arousal;                  // Arousal state (0.0-1.0)
    float vigilance;                 // Vigilance level (0.0-1.0)
    float locusCoeruleusGain;        // LC neuron gain
    float phasicBurst;               // Phasic burst amplitude
    float tonicFiring;               // Tonic firing rate
    float peakLevel;                 // Peak phasic level
    float decayRate;                 // Recovery to baseline
    float releaseRate;               // Phasic release rate
    float noiseLevel;                // Baseline noise floor
    bool alertState;                 // Current alert state
    
    Impl() : level(0.0f), baseline(0.05f), arousal(0.1f), vigilance(0.1f),
             locusCoeruleusGain(1.0f), phasicBurst(0.0f), tonicFiring(0.05f),
             peakLevel(3.0f), decayRate(0.02f), releaseRate(2.0f),
             noiseLevel(0.01f), alertState(false) {}
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
    pImpl->level = std::clamp(level, 0.0f, 3.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for salient events
    return 1.0f + 0.5f * pImpl->arousal;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay and recovery
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal based on current level
    pImpl->arousal = std::min(1.0f, pImpl->level * 0.3f);
    
    // Update vigilance based on state
    if (pImpl->alertState) {
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.01f * static_cast<float>(dt));
    } else {
        pImpl->vigilance = std::max(0.0f, pImpl->vigilance - 0.005f * static_cast<float>(dt));
    }
    
    // Phasic burst decays
    pImpl->phasicBurst = std::max(0.0f, pImpl->phasicBurst - 0.01f * static_cast<float>(dt));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousal;
}

void Norepinephrine::signalArousal(float intensity, bool novelEvent) {
    // Phasic response to novel or salient events
    float burst = intensity;
    if (novelEvent) {
        burst *= 1.5f;  // Novel events trigger stronger bursts
    }
    
    pImpl->level = std::min(pImpl->peakLevel, pImpl->level + burst * pImpl->releaseRate);
    pImpl->phasicBurst = burst;
    pImpl->alertState = true;
}

float Norepinephrine::getVigilanceModulator() const {
    return pImpl->vigilance;
}

void Norepinephrine::maintainVigilance(bool isAlertState) {
    pImpl->alertState = isAlertState;
    if (isAlertState && pImpl->vigilance < 0.8f) {
        pImpl->vigilance = 0.8f;
    }
}

float Norepinephrine::getLocusCoeruleusGain() const {
    return pImpl->locusCoeruleusGain;
}

void Norepinephrine::increaseAlertness() const {
    // Increase global alertness and signal-to-noise ratio
    // Implementation would modulate neuromodulatory inputs across brain regions
}

void Norepinephrine::enhanceSignalToNoiseRatio() const {
    // Enhance detection of relevant signals while suppressing noise
    // Implementation would adjust synaptic gains and thresholds
}

void Norepinephrine::prepareForAction() const {
    // Prepare motor systems for action execution
    // Implementation would enhance motor neuron excitability
}

} // namespace nlm