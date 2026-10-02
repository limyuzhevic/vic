#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressResponse;
    float vigilanceFactor;
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.2f), releaseRate(1.0f)
        , arousalLevel(0.5f), stressResponse(0.0f), vigilanceFactor(1.0f) {}
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
    // Norepinephrine modulates plasticity for arousal and attention
    // Moderate NE enhances plasticity, high NE impairs (fight or flight)
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Inverted U-shaped response for optimal arousal
    float optimalLevel = 0.3f;  // Lower optimum for NE
    float sigma = 0.25f;
    
    float plasticityFactor = std::exp(-std::pow(normalizedLevel - optimalLevel, 2) / (2 * sigma * sigma));
    
    // Apply stress response modulation
    if (pImpl->stressResponse > 0.0f) {
        plasticityFactor *= (1.0f - pImpl->stressResponse * 0.3f);
    }
    
    // Apply vigilance enhancement
    plasticityFactor *= pImpl->vigilanceFactor;
    
    return std::clamp(plasticityFactor, 0.2f, 1.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * deltaTime);
}

void Norepinephrine::signalArousal(float arousal) {
    // Real NE response to arousal stimuli
    // Phasic increases for novel or salient stimuli
    
    float burstAmount = arousal * pImpl->releaseRate;
    
    // Modulate with current arousal level
    if (arousal > 0.5f) {
        // Strong arousal: larger, faster response
        burstAmount *= 1.5f;
        // Add stress component
        pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 0.2f * arousal);
    } else {
        // Moderate arousal: proportional response
        burstAmount *= 0.8f;
    }
    
    // Update NE level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    
    // Update vigilance based on arousal
    pImpl->vigilanceFactor = std::min(2.0f, 1.0f + 0.5f * arousal);
}

} // namespace nlm