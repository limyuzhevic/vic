#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalWeight;
    float vigilanceWeight;
    float th alertnessThreshold;
    
    Impl() : 
        brain(nullptr),
        level(0.0f),
        baseline(0.0f),
        peak(1.0f),
        decayRate(0.05f),
        releaseRate(1.0f),
        arousalWeight(0.7f),
        vigilanceWeight(0.6f),
        alertnessThreshold(0.5f) {}
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
    // NE modulates arousal and signal detection - affects vigilance and learning rate
    float vigilanceFactor = (pImpl->level > pImpl->alertnessThreshold) ? 1.0f : 0.3f;
    return 0.3f + 0.7f * vigilanceFactor * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: burst-pause pattern with adaptive response to novel stimuli
    if (pImpl->level > 0.1f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
}

void Norepinephrine::signalNovelty(float novelty) {
    // NE increases with novel or unexpected stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * pImpl->arousalWeight);
}

void Norepinephrine::enhanceVigilance(float duration) {
    // NE boosts vigilance and signal detection during exploration
    setLevel(std::min(1.0f, getLevel() + duration * 0.05f));
}

float Norepinephrine::getAlertness() const {
    return pImpl->level > pImpl->alertnessThreshold ? 1.0f : pImpl->level / pImpl->alertnessThreshold;
}

} // namespace nlm