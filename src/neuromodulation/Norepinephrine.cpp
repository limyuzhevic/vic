#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal-related parameters
    float arousalLevel;
    
    // Stress-related parameters
    float stressIntensity;
    float stressDecay;
    bool isStressed;
    
    // Vigilance parameters
    float vigilanceGain;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.08f), releaseRate(1.2f),
             arousalLevel(1.0f), stressIntensity(0.0f), stressDecay(0.95f), isStressed(false),
             vigilanceGain(1.0f) {}
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
    // Update arousal and vigilance based on level
    pImpl->arousalLevel = 1.0f + pImpl->level * 1.5f;
    pImpl->vigilanceGain = 1.0f + pImpl->level * 1.2f;
    if (pImpl->level > 0.7f) {
        pImpl->isStressed = true;
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on stress/attention state
    float stressFactor = pImpl->isStressed ? 1.2f : 1.0f;
    return 0.5f + 0.5f * pImpl->level * stressFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay stress
    if (pImpl->isStressed) {
        pImpl->stressIntensity *= pImpl->stressDecay;
        if (pImpl->stressIntensity < 0.1f) {
            pImpl->isStressed = false;
        }
    }
}

void Norepinephrine::modulateArousal(float amount) {
    // Increase arousal level
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
    if (amount > 0.5f) {
        pImpl->stressIntensity += amount * 0.3f;
        pImpl->isStressed = true;
    }
}

void Norepinephrine::activateStressResponse(float intensity) {
    // Activate stress response
    pImpl->stressIntensity = std::min(1.0f, intensity);
    pImpl->isStressed = true;
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * 0.8f);
}

} // namespace nlm