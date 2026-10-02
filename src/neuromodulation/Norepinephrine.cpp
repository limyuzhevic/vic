#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // NE enhances arousal and learning rate
    // Higher NE increases initial learning
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO: Implement real NE dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // TODO: Implement arousal signaling
    // PLACEHOLDER: Burst of NE on arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->releaseRate);
}

void Norepinephrine::signalLearningRate(float learningRate) {
    // TODO: Implement learning rate signaling
    // PLACEHOLDER: NE modulates learning rate
    pImpl->level = std::max(0.0f, pImpl->level + learningRate * pImpl->releaseRate * 0.5f);
}

} // namespace nlm