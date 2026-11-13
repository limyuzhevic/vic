#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace nlm {

struct Serotonin::Impl {
    // Current 5-HT level (tonal component)
    float level;
    
    // Mood state
    float mood;
    
    // Impulsivity modulation
    float impulsivity;
    
    // Time constants
    static constexpr float MOOD_DECAY_TIME = 300.0f;       // ms - mood decay
    static constexpr float IMPULSIVITY_DECAY = 200.0f;     // ms - impulsivity decay
    static constexpr float MAX_MOOD = 2.0f;                // Maximum positive mood
    static constexpr float MIN_MOOD = -1.0f;               // Minimum negative mood
    
    Impl() : 
        level(0.2f),      // Baseline serotonin
        mood(0.5f),       // Neutral baseline mood
        impulsivity(1.0f) {} // Normal impulsivity
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, Impl::MAX_MOOD * 0.8f);
}

float Serotonin::getPlasticityFactor() const {
    // PHASE 2 FIX: Real 5-HT modulation of plasticity
    // Positive mood enhances stable learning, negative mood increases exploration
    
    float plasticityFactor = 1.0f;
    
    if (pImpl->mood > 0.5f) {
        // Positive mood reduces plasticity for stable learning
        plasticityFactor *= 0.8f + pImpl->mood * 0.1f;
    } else if (pImpl->mood < -0.3f) {
        // Negative mood increases plasticity for exploration
        plasticityFactor *= 1.5f + std::abs(pImpl->mood) * 0.5f;
    }
    
    // Impulsivity affects plasticity
    if (pImpl->impulsivity > 1.2f) {
        plasticityFactor *= 1.2f;
    }
    
    return plasticityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // PHASE 2 FIX: Real 5-HT dynamics
    // Natural mood fluctuations and impulse control
    
    float timeFactor = static_cast<float>(dt) / 0.001f;
    
    // Natural decay towards baseline
    float decayRate = 1.0f - std::exp(-timeFactor / Impl::MOOD_DECAY_TIME);
    pImpl->level *= (1.0f - decayRate * 0.03f);
    
    // Mood decay
    float moodDecay = 1.0f - std::exp(-timeFactor / Impl::MOOD_DECAY_TIME);
    pImpl->mood *= (1.0f - moodDecay);
    
    // Impulsivity decay
    float impulseDecay = 1.0f - std::exp(-timeFactor / Impl::IMPULSIVITY_DECAY);
    pImpl->impulsivity *= (1.0f - impulseDecay);
}

float Serotonin::getExcitabilityModulator() const {
    // PHASE 2 FIX: 5-HT modulates neural excitability and emotional processing
    float modulator = 1.0f;
    
    // Positive mood reduces excitability (calming effect)
    if (pImpl->mood > 0.5f) {
        modulator *= 0.9f - pImpl->mood * 0.05f;
    }
    
    // Negative mood increases excitability (arousal effect)
    if (pImpl->mood < -0.5f) {
        modulator *= 1.2f + std::abs(pImpl->mood) * 0.1f;
    }
    
    return modulator;
}

void Serotonin::setMood(float mood) {
    pImpl->mood = std::clamp(mood, Impl::MIN_MOOD, Impl::MAX_MOOD);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

void Serotonin::modulateImpulsivity(float factor) {
    pImpl->impulsivity = std::clamp(factor, 0.5f, 2.0f);
}

} // namespace nlm