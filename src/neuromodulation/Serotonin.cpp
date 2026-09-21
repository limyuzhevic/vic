#include "Serotonin.hpp"
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Real serotonin-modulated plasticity factor implementation
    // Serotonin regulates mood, impulsivity, and social behavior
    // Modulated by social context and environmental factors
    
    if (pImpl->level < 0.05f) {
        return 1.0f;  // Baseline plasticity
    }
    
    // Moderate serotonin enhances plasticity (promotes learning)
    float moodMod = 1.0f + 0.6f * pImpl->level;  // Up to 1.6x at moderate levels
    
    // High serotonin can reduce impulsivity and improve sustained attention
    float impulsivityMod = 1.0f + 0.3f * pImpl->level;  // Up to 1.3x
    
    // Social context enhances learning when serotonin is elevated
    float socialMod = 1.0f + 0.4f * pImpl->level;  // Up to 1.4x
    
    return std::clamp(moodMod * impulsivityMod * socialMod, 1.0f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Real serotonin dynamics implementation
    // Based on serotonergic system in the brain
    // Regulates mood, sleep, appetite, and social behavior
    
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline exponentially
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-timeDecay);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalMoodChange(float mood) {
    // Signal mood regulation
    // Serotonin modulates mood states and emotional learning
    // Promotes sustained attention and reduces negative affect
    
    if (mood > 0.0f) {
        // Positive mood boost proportional to emotional state
        float boost = std::min(pImpl->peak, mood * pImpl->releaseRate * 1.5f);
        
        // Increase level (not replace)
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
        
        // Maintain mood baseline
        if (pImpl->baseline == 0.0f) {
            pImpl->baseline = pImpl->level * 0.08f;  // 8% of peak as baseline
        }
    }
}

void Serotonin::signalSocialContext(float context) {
    // Signal social context processing
    // Serotonin enhances processing of social information
    // Reduces impulsivity in social situations
    
    if (context > 0.0f) {
        // Social context signal proportional to social engagement
        float socialBoost = std::min(0.5f, context * pImpl->releaseRate * 0.8f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialBoost);
    }
}

} // namespace nlm