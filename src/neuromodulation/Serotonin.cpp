#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // For social behavior effects
    float socialWeight;
    float impulsivityLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             socialWeight(1.0f), impulsivityLevel(0.0f) {}
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
    // 5-HT modulates mood and social behavior
    // Higher serotonin stabilizes plasticity and reduces impulsive changes
    return 1.0f - 0.5f * pImpl->level;  // Inhibitory effect on plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Social weight decays slowly
    pImpl->socialWeight = std::max(0.1f, pImpl->socialWeight - 0.005f * static_cast<float>(dt));
}

void Serotonin::signalSocial(float socialSignal) {
    // Social behavior signal from raphe nuclei
    setLevel(socialSignal);
    pImpl->socialWeight = std::min(1.0f, pImpl->socialWeight + socialSignal * 0.1f);
}

} // namespace nlm
