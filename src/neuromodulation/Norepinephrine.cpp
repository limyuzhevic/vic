#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float modulationFactor;
    float arousalLevel;
    float stressLevel;
    
    Impl() : level(0.1f), baseline(0.05f), peak(1.0f), decayRate(0.03f), releaseRate(1.2f),
             modulationFactor(0.8f), arousalLevel(0.5f), stressLevel(0.3f) {}
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
    // Norepinephrine modulates arousal and attention
    // Moderate NE improves focus, high NE can cause stress and impair learning
    float arousal = pImpl->level;
    float optimalArousal = 0.5f;
    
    // Inverted U-shaped relationship with plasticity
    float plasticity = std::exp(-std::pow((arousal - optimalArousal) / 0.3f, 2.0f));
    
    // Apply stress penalty if too high
    if (pImpl->stressLevel > 0.7f) {
        plasticity *= (1.0f - (pImpl->stressLevel - 0.7f) * 2.0f);
    }
    
    return plasticity * pImpl->modulationFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add noise for environmental sensitivity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.05f, 0.05f);
    pImpl->level += dist(gen);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
    
    // Boost NE for arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * 0.4f);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Stress increases NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 0.3f);
}

} // namespace nlm