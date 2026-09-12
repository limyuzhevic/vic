#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

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
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , arousalGain(1.0f), vigilanceGain(1.0f), stressResponse(0.0f) {}
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
    // Norepinephrine enhances attention and arousal
    // Moderate NE increases signal-to-noise ratio and learning
    // High NE can cause hypervigilance and impaired learning
    float normalized = pImpl->level / 1.0f;  // Assuming peak is 1.0
    return 0.5f + 0.3f * std::exp(-std::pow(normalized - 0.5f, 2.0f) / 0.05f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float stimulus) {
    // Increase arousal for novel or intense stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + stimulus * pImpl->releaseRate * pImpl->arousalGain);
}

void Norepinephrine::signalVigilance(float alertness) {
    // Enhance vigilance and focus
    pImpl->stressResponse += alertness * 0.1f;
    pImpl->stressResponse = std::clamp(pImpl->stressResponse, 0.0f, 1.0f);
}

float Norepinephrine::getStressResponse() const {
    return pImpl->stressResponse;
}

} // namespace nlm
