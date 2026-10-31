#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodBoost;
    float impulsivityModulator;
    float socialBehavior;
    float patienceLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.03f), moodBoost(0.4f), 
             impulsivityModulator(0.8f), socialBehavior(0.6f), patienceLevel(0.5f) {}
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
    // Serotonin modulates mood-related plasticity
    // Moderate serotonin levels can enhance certain types of learning
    if (pImpl->level < 0.5f) {
        return 0.7f + 0.6f * pImpl->level;  // Low serotonin: moderate plasticity
    } else {
        return 1.0f;  // High serotonin: normal plasticity
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: decays very slowly, accumulates with positive experiences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalPositiveExperience(float reward, float satisfaction) {
    // Serotonin increases with positive experiences and satisfaction
    float boost = std::clamp((reward + satisfaction) * 0.5f, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
}

void Serotonin::signalSocialSuccess(float socialSuccess, float connectionQuality) {
    // Serotonin increases with social success and strong connections
    float socialBoost = std::clamp((socialSuccess + connectionQuality) * 0.7f, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialBoost);
}

} // namespace nlm