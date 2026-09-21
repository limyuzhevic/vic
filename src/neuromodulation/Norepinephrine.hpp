#include "Norepinephrine.hpp"
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Real norepinephrine-modulated plasticity factor implementation
    // NE enhances arousal, vigilance, and learning under stress
    // Modulated by attention and emotional state
    
    if (pImpl->level < 0.05f) {
        return 1.0f;  // Baseline plasticity
    }
    
    // Moderate arousal enhances plasticity (inverted U-curve)
    float arousalMod = 1.0f + 0.8f * pImpl->level;  // Up to 1.8x at moderate levels
    
    // High arousal can impair learning (negative feedback)
    if (pImpl->level > 0.7f) {
        arousalMod = 1.5f + 0.5f * (1.0f - pImpl->level);  // Decline after 0.7
    }
    
    return std::clamp(arousalMod, 1.0f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real norepinephrine dynamics implementation
    // Based on locus coeruleus firing patterns in the brain
    // Regulates arousal, attention, and stress response
    
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline exponentially
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-timeDecay);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal increased arousal and vigilance
    // NE peaks with stress, novelty, and challenge
    // Used for focused attention and alertness
    
    if (arousal > 0.0f) {
        // Arousal burst proportional to stress level
        float burst = std::min(pImpl->peak, arousal * pImpl->releaseRate * 1.5f);
        
        // Increase level (not replace)
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Maintain arousal baseline for alertness
        if (pImpl->baseline == 0.0f) {
            pImpl->baseline = pImpl->level * 0.1f;  // 10% of peak as baseline
        }
    }
}

void Norepinephrine::signalStress(float stress) {
    // Signal stress response
    // NE response to stress activates fight-or-flight
    // Modulates memory consolidation for emotional events
    
    if (stress > 0.0f) {
        // Stress triggers NE release
        float response = std::min(pImpl->peak, stress * pImpl->releaseRate * 2.0f);
        
        // Add to current level
        pImpl->level = std::min(pImpl->peak, pImpl->level + response);
        
        // Stress can enhance memory (or impair if too high)
        if (pImpl->baseline == 0.0f) {
            pImpl->baseline = pImpl->level * 0.05f;  // Lower baseline for stress state
        }
    }
}

} // namespace nlm