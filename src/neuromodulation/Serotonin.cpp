#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float mood;
    float impulsivity;
    float socialMotiv;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             mood(0.5f), impulsivity(0.5f), socialMotiv(0.5f) {}
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
    // Serotonin modulates mood-dependent plasticity
    // High serotonin promotes stable, long-term changes; low serotonin increases plasticity but reduces stability
    if (pImpl->level > 0.6f) {
        return 0.9f;  // Stable, long-term learning
    } else if (pImpl->level < 0.3f) {
        return 1.5f;  // High plasticity, less stable
    } else {
        return 1.2f;  // Balanced plasticity
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has slow dynamics, modulates mood and behavior
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Improve mood and reduce impulsivity
void Serotonin::improveMood(float amount) {
    pImpl->mood = std::min(1.0f, pImpl->mood + amount * 0.1f);
    // Reduce impulsivity as mood improves
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - amount * 0.05f);
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - amount);
}

// Get mood-impulsivity ratio (higher = more stable, less impulsive)
float Serotonin::getMoodStability() const {
    return pImpl->mood / (pImpl->impulsivity + 0.1f);
}

} // namespace nlm
