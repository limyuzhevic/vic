#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalModulator;     // How much NE affects arousal
    float salienceModulator;    // How much NE affects salience detection
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), 
             arousalModulator(1.0f), salienceModulator(1.0f) {}
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
    // NE modulates arousal and salience detection
    // Higher NE increases arousal, making stimuli more salient
    // This affects both attention and memory formation
    return 1.0f + pImpl->arousalModulator * pImpl->level * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // NE signals increased arousal
    // Makes stimuli more salient and increases behavioral activation
    if (arousalLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate);
    }
}

void Norepinephrine::signalSalience(float salienceStrength) {
    // NE signals stimulus salience
    // Enhances processing of novel, unexpected, or important events
    pImpl->salienceModulator = std::min(2.0f, 1.0f + salienceStrength * 2.0f);
}

void Norepinephrine::signalLocusCoeruleusBurst(float burstIntensity) {
    // Strong NE burst from locus coeruleus
    // Markov reset and strong arousal for learning events
    pImpl->level = 1.0f;  // Max level for strong signal
    pImpl->arousalModulator = 2.0f;  // Double arousal modulation
}

} // namespace nlm