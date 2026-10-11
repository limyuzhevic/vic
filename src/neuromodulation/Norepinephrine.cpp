#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.1f), releaseRate(1.0f) {}
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
    // Norepinephrine modulates arousal and vigilance
    // Influences focus and signal-to-noise ratio
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::enhanceVigilance(float strength) {
    // Increase arousal and vigilance for environmental monitoring
    if (brain_ && strength > 0.0f) {
        // In a full implementation, this would increase overall neural excitability
        // and reduce threshold for stimulus detection
    }
}

void Norepinephrine::enhanceFocus(float strength) {
    // Improve signal-to-noise ratio for relevant stimuli
    if (brain_ && strength > 0.0f) {
        // In a full implementation, this would enhance selective attention
        // and reduce distraction from irrelevant inputs
    }
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

} // namespace nlm