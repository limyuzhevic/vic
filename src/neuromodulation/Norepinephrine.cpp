#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), 
             arousalLevel(0.0f), stressLevel(0.0f) {}
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
    // NE modulates arousal and learning
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate arousal and stress
    pImpl->arousalLevel = pImpl->level * 0.6f;
    pImpl->stressLevel = pImpl->level * 0.4f;
}

void Norepinephrine::signalArousal(float arousalStrength) {
    setLevel(std::min(pImpl->peak, getLevel() + arousalStrength * 0.5f));
    arousalLevel = pImpl->level * 0.6f;
}

void Norepinephrine::signalStress(float stressStrength) {
    setLevel(std::min(pImpl->peak, getLevel() + stressStrength * 0.8f));
    stressLevel = pImpl->level * 0.4f;
}

void Norepinephrine::signalRecovery(float recoveryStrength) {
    setLevel(std::max(pImpl->baseline, getLevel() - recoveryStrength * 0.7f));
    arousalLevel = std::max(0.0f, arousalLevel - recoveryStrength * 0.5f);
    stressLevel = std::max(0.0f, stressLevel - recoveryStrength * 0.3f);
}

float Norepinephrine::getLearningRateModulation() const {
    return 1.0f + pImpl->level * 0.5f;
}

float Norepinephrine::getInhibitionLevel() const {
    return pImpl->level * 0.7f;
}

} // namespace nlm