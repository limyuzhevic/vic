#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float vigilanceWeight;
    float alertnessDuration;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f), 
             vigilanceWeight(1.0f), alertnessDuration(0.0f) {}
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
    // NE modulates arousal and vigilance
    // Higher NE increases alertness and plasticity during novel situations
    if (pImpl->level < 0.3f) {
        return 1.0f;  // Baseline vigilance
    } else {
        return 0.5f + pImpl->level;  // Increased plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: Slower decay, can have spontaneous increases
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Spontaneous alertness increases during quiet periods
    if (pImpl->level < 0.2f) {
        float spontaneousIncrease = dt * 0.001f;  // Slow spontaneous increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + spontaneousIncrease);
    }
}

void Norepinephrine::signalNovelty(float novelty) {
    // NE spikes strongly in response to novel or surprising stimuli
    float spike = std::min(1.0f, novelty * pImpl->releaseRate);
    pImpl->level = std::min(pImpl->peak, pImpl->level + spike);
    pImpl->alertnessDuration = 100.0f;  // Set alertness duration
}

void Norepinephrine::enhanceSensoryProcessing(float gain) {
    // NE enhances sensory processing and signal-to-noise ratio
    float processingGain = pImpl->level * gain * pImpl->vigilanceWeight;
    // In a full implementation, this would be passed to sensory systems
    if (processingGain > 0.01f) {
        NLM_LOG_DEBUG("Norepinephrine enhanced sensory processing by " + std::to_string(processingGain));
    }
}

void Norepinephrine::getAlertnessState() const {
    if (pImpl->alertnessDuration > 0.0f) {
        NLM_LOG_DEBUG("Norepinephrine: High alertness state for " + 
                     std::to_string(pImpl->alertnessDuration) + " steps");
    }
}

} // namespace nlm