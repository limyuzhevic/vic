#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodModifier;
    float learningRateModulator;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), 
             decayRate(0.05f), releaseRate(2.0f),
             moodModifier(1.0f), learningRateModulator(1.0f) {}
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
    // Serotonin modulates learning rate and impulsivity
    // Higher 5-HT generally reduces learning rate but increases stability
    // and mood-based modulation of plasticity
    float learningMod = 1.0f - pImpl->learningRateModulator * pImpl->level * 0.3f;
    float moodMod = pImpl->moodModifier * (1.0f - pImpl->level * 0.2f);
    return std::clamp(learningMod * moodMod, 0.1f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update associated modulators
    pImpl->learningRateModulator = std::clamp(1.0f - pImpl->level * 0.5f, 0.5f, 1.5f);
    pImpl->moodModifier = std::clamp(0.8f + pImpl->level * 0.6f, 0.8f, 2.0f);
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Serotonin responds to social interactions and reward prediction errors
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodDelta * pImpl->releaseRate);
}

void Serotonin::modulateLearning(float learningRate) {
    // Serotonergic modulation of learning based on current level
    float modulation = 1.0f - pImpl->level * 0.4f;
    // Ensure learning rate doesn't drop below reasonable minimum
    // but can be enhanced at low levels for exploration
}

} // namespace nlm