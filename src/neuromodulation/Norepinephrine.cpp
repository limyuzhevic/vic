// Implementation of Norepinephrine neuromodulator
#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                     // Current NE concentration
    float baseline;                  // Baseline level
    float peak;                      // Peak level
    float decayRate;                 // Rate of decay
    float releaseRate;               // Rate of release
    float arousalStrength;          // Effect on arousal/vigilance
    float learningRate;              // Effect on learning speed
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             arousalStrength(1.0f), learningRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

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
    // NE affects arousal and vigilance
    // Moderate NE improves signal-to-noise ratio
    float arousalFactor = 0.8f + 0.4f * std::sin(pImpl->level * M_PI);
    float learningFactor = 1.0f + 0.5f * pImpl->level;
    return arousalFactor * learningFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decays but responds to novel/stimulating inputs
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalNovelStimulus(float intensity) {
    // Novel or intense stimuli increase NE
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Norepinephrine::signalAlert(float urgency) {
    // Urgent alerts trigger NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + urgency * pImpl->learningRate * 0.8f);
}

} // namespace nlm