#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float learningRateMod;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousal(0.5f), vigilance(0.5f), learningRateMod(0.5f) {}
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
    // Norepinephrine modulates plasticity by enhancing arousal and learning rate
    // Higher NE increases learning rate and plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousalEvent() {
    // Burst of norepinephrine on arousing events
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate);
}

void Norepinephrine::signalSurpriseEvent() {
    // Norepinephrine responds to surprise/novelty
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.7f * pImpl->releaseRate);
}

void Norepinephrine::signalPredictionErrorEvent(float error) {
    // Norepinephrine responds to prediction errors
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm