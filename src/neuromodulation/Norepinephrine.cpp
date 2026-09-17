#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;  // Separate arousal signal
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f), arousal(0.5f) {}
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
    // NE modulates arousal and working memory encoding
    float arousalMod = pImpl->arousal * 0.5f;
    return 1.0f + arousalMod * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: very slow decay, responds to salient events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // NE responds to arousing events (threat, reward, novelty)
    pImpl->arousal = std::clamp(arousalLevel, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE responds to stress
    float effectiveStress = stressLevel * (1.0f + pImpl->level);
    pImpl->level = std::min(pImpl->peak, pImpl->level + effectiveStress * pImpl->releaseRate * 0.7f);
}

} // namespace nlm