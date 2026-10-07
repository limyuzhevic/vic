#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float socialModulator;
    float inhibitionModulator;
    
    Impl() : level(0.0f), baseline(0.2f), peak(0.6f), decayRate(0.02f), releaseRate(1.0f),
             socialModulator(2.0f), inhibitionModulator(1.5f) {}
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
    // Serotonin has complex effects: high levels promote LTD, moderate levels enhance certain forms of learning
    if (pImpl->level > 0.7f) {
        return 0.3f;  // Strong LTD for emotional memory updating
    } else if (pImpl->level > 0.3f) {
        return 0.8f + 0.4f * (pImpl->level - 0.3f);  // Enhanced learning for positive states
    } else {
        return 0.6f * pImpl->level;  // Baseline modulation
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has slow dynamics and circadian modulation
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::signalSocialReward(float socialValue) {
    // Serotonin increases with social reward and positive interactions
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialValue * pImpl->socialModulator);
}

void Serotonin::signalNegativeAffect(float negativeInput) {
    // Negative experiences trigger serotonin changes
    pImpl->level = std::max(0.0f, pImpl->level - negativeInput * pImpl->inhibitionModulator);
}

float Serotonin::getSocialModulator() const {
    return pImpl->socialModulator;
}

} // namespace nlm
