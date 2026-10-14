#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float signalDetectionBoost;
    float noveltyEncodingBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(0.0f), signalDetectionBoost(0.0f), noveltyEncodingBoost(0.0f) {}
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
    
    void Norepinephrine::setBaseline(float baseline) {
        pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    }
    
    float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal and novelty processing
    return 0.5f + 0.5f * pImpl->level * 1.2f;  // Moderate modulation
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Decay modulation factors
    pImpl->arousalLevel *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
    pImpl->signalDetectionBoost *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
    pImpl->noveltyEncodingBoost *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::enhanceSignalDetection(float snrBoost) {
    pImpl->signalDetectionBoost = std::min(1.0f, snrBoost * pImpl->level);
}

void Norepinephrine::enhanceNoveltyEncoding(float noveltyFactor) {
    pImpl->noveltyEncodingBoost = std::min(1.0f, noveltyFactor * pImpl->level);
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    pImpl->arousalLevel = std::min(1.0f, arousalLevel * pImpl->level);
}

void Norepinephrine::enhanceWorkingMemoryEncoding(float encodingStrength) {
    // NE enhances working memory encoding during novel or salient events
    if (encodingStrength > 0.0f) {
        // Boost encoding processes
    }
}

void Norepinephrine::enhanceMemoryEncodingDuringNovelty(float factor) {
    // NE specifically enhances memory encoding during novel events
    if (factor > 0.0f) {
        // Link novelty detection to memory encoding
    }
}

} // namespace nlm