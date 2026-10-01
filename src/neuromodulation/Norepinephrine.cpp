#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalFactor;
    float vigilanceFactor;
    float labilityFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), arousalFactor(1.2f), vigilanceFactor(1.3f), labilityFactor(0.5f) {}
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
    // Norepinephrine affects alertness and signal detection
    // Enhances plasticity for salient events, especially during high arousal
    return 0.5f + 0.5f * pImpl->level * (pImpl->arousalFactor + pImpl->vigilanceFactor);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline but increase with stress/arousal
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Arousal signaling - NE increases during stress, novelty, or high importance
void Norepinephrine::signalArousal(float stress, float importance, float novelty) {
    // Boost level based on multiple factors
    float boost = (stress * 0.4f + importance * 0.4f + novelty * 0.2f) * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
}

// Vigilance enhancement - NE improves attention and signal-to-noise ratio
void Norepinephrine::enhanceVigilance(float signalStrength) {
    // Increase vigilance based on signal strength
    pImpl->level = std::min(pImpl->peak, pImpl->level + signalStrength * pImpl->vigilanceFactor);
}

// Emotional tagging - NE associates events with emotional significance
void Norepinephrine::tagEmotionalState(float emotionalValence) {
    // Modulate level based on emotional state
    if (emotionalValence > 0.5f) {
        // Positive emotion: moderate increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * emotionalValence);
    } else {
        // Negative emotion: stronger increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * (1.0f - emotionalValence));
    }
}

} // namespace nlm