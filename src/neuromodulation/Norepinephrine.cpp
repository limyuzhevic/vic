#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
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
    float arousalBoost;
    float signalDetectionBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(1.0f)
        , arousalBoost(0.4f)
        , signalDetectionBoost(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getArousalLevel() const {
    // NE modulates arousal (waking up, alertness)
    return pImpl->level;
}

float Norepinephrine::getSignalDetection() const {
    // NE enhances signal detection and discrimination
    // Higher NE = better ability to detect weak signals
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // Signal arousal increase (waking up, increased alertness)
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    NLM_LOG_DEBUG("Norepinephrine signals arousal: " + std::to_string(intensity));
}

void Norepinephrine::signalNovelStimulus(float intensity) {
    // Signal detection of novel stimulus
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->signalDetectionBoost);
    NLM_LOG_DEBUG("Norepinephrine signals novel stimulus: " + std::to_string(intensity));
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
