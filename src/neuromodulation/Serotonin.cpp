#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float socialThreshold;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), socialThreshold(0.4f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

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
    // Serotonin has complex effects:
    // - Low serotonin: high impulsivity, increased risk-taking
    // - High serotonin: reduced impulsivity, increased patience
    // - Modulates mood and social behavior
    float s = pImpl->level;
    if (s < 0.3f) {
        // Low serotonin: increased exploration, reduced behavioral inhibition
        return 0.7f + 0.3f * s;
    } else if (s < 0.7f) {
        // Moderate serotonin: balanced behavior
        return 0.8f + 0.2f * (s - 0.3f);
    } else {
        // High serotonin: increased behavioral inhibition, reduced exploration
        return 0.9f - (s - 0.7f) * 0.4f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: influenced by social interactions and environmental context
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialConnection(float socialStrength) {
    // Serotonin release during social bonding and positive interactions
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStrength * pImpl->releaseRate);
}

void Serotonin::signalReward(float reward) {
    // Serotonin response to predictable, expected rewards (vs dopamine for prediction error)
    if (reward > 0.3f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.5f);
    }
}

void Serotonin::signalThreat(float threatLevel) {
    // Serotonin reduces under threat (prepare for fight/flight)
    pImpl->level = std::max(0.0f, pImpl->level - threatLevel * 0.5f);
}

} // namespace nlm
