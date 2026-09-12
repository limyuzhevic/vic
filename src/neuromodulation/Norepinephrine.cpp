#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalHistory;
    float vigilance;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.08f), releaseRate(3.0f),
             arousalHistory(0.0f), vigilance(0.5f), stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for salient events and arousal
    float vigilanceFactor = pImpl->vigilance * 0.5f;
    float stressFactor = std::tanh(pImpl->stressLevel * 0.3f) * 0.3f;
    return 0.5f + 0.5f * pImpl->level + vigilanceFactor + stressFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline with more rapid decay than ACh
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay arousal and vigilance
    pImpl->arousalHistory *= 0.9f;
    pImpl->vigilance *= 0.95f;
    pImpl->stressLevel *= 0.9f;
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // Arousal signals boost NE level rapidly
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->releaseRate);
    pImpl->arousalHistory += arousalSignal * 0.7f;
    
    // Increase vigilance for novel/salient stimuli
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + arousalSignal * 0.4f);
}

} // namespace nlm
