#include "Norepinephrine.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;           // Current norepinephrine level
    float arousal;        // Arousal state (0.0 to 1.0)
    float vigilance;       // Vigilance/wakefulness (0.0 to 1.0)
    
    Impl() : level(0.0f), arousal(0.0f), vigilance(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    // Clamp level to reasonable range
    pImpl->level = std::clamp(level, 0.0f, 10.0f);
    
    // Update arousal and vigilance based on level
    pImpl->arousal = std::min(pImpl->level / 10.0f, 1.0f);
    pImpl->vigilance = std::min(pImpl->level / 8.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates attention and learning
    // Optimal arousal improves learning, but too much can impair it
    float optimalArousal = 0.5f;
    float arousalDiff = std::abs(pImpl->arousal - optimalArousal);
    
    // Inverted U-shaped relationship: moderate arousal improves learning
    float modulation = 1.0f - arousalDiff * arousalDiff * 2.0f;
    modulation = std::clamp(modulation, 0.1f, 1.5f);
    
    return modulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Gradually decay toward baseline
    pImpl->level += (0.1f - pImpl->level) * dt * 0.01f;
    pImpl->arousal = std::min(pImpl->level / 10.0f, 1.0f);
    pImpl->vigilance = std::min(pImpl->level / 8.0f, 1.0f);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

} // namespace nlm