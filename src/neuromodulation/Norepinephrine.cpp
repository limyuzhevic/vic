#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalGain;
    float vigilanceGain;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalGain(2.0f), vigilanceGain(1.8f), stressResponse(1.2f) {}
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
    // High NE increases plasticity for salient/novel events
    return 1.0f + pImpl->level * pImpl->arousalGain * pImpl->vigilanceGain;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: increases with salient events, decays naturally
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Arousal (salient/novel events) increases NE level
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress response increases NE for fight-or-flight
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->stressResponse * pImpl->releaseRate);
}

} // namespace nlm
