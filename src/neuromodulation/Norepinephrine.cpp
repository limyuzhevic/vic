#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalStrength;
    float vigilanceLevel;
    float alertness;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             arousalStrength(1.0f), vigilanceLevel(0.5f), alertness(0.5f) {}
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
    // Norepinephrine enhances alertness and working memory encoding
    return 0.4f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine dynamics: promote arousal for salient stimuli, steady baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->alertness = std::min(1.0f, pImpl->alertness + (pImpl->level - pImpl->alertness) * 0.1f);
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // Salient stimuli trigger norepinephrine release for increased arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->arousalStrength);
}

void Norepinephrine::modulateVigilance(float vigilanceLevel) {
    // Modulate vigilance/alertness level
    pImpl->vigilanceLevel = std::clamp(vigilanceLevel, 0.0f, 1.0f);
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

} // namespace nlm