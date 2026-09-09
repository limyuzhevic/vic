#include "Norepinephrine.hpp"
#include "../brain/Brain.hpp"
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
    float arousalLevel;
    float vigilance;
    float signalToNoiseRatio;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)  // Baseline NE level ~2%
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(2.5f)
        , arousalLevel(0.5f)
        , vigilance(0.5f)
        , signalToNoiseRatio(1.0f)
        {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - modulating arousal and vigilance");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived state
    updateDerivedState();
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance based on NE level
    pImpl->arousalLevel = 0.5f + pImpl->level * 0.8f;  // Range [0.5, 1.3]
    pImpl->vigilance = 0.5f + pImpl->level * 0.8f;
    
    // Update signal-to-noise ratio - NE enhances signal processing
    pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 1.5f;  // Range [1.0, 2.5]
    
    // Log level changes
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Norepinephrine::updateDerivedState() {
    // Update vigilance based on NE level
    pImpl->vigilance = 0.5f + pImpl->level * 0.8f;
    
    // Modulate arousal in the brain
    if (pImpl->brain) {
        pImpl->arousalLevel = 0.5f + pImpl->level * 0.8f;  // Range [0.5, 1.3]
        pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 1.5f;  // Range [1.0, 2.5]
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates signal-to-noise ratio, affecting learning efficiency
    // Higher NE improves signal detection but can reduce plasticity for noisy inputs
    return 0.8f + pImpl->level * 0.4f;  // Range [0.8, 1.2]
}

void Norepinephrine::signalStartle() {
    // Burst of NE on startling events
    setLevel(std::min(pImpl->peak, pImpl->level + 1.2f * pImpl->releaseRate));
}

void Norepinephrine::signalSustainedAttention() {
    // Sustained increase for maintaining vigilance
    setLevel(std::min(pImpl->peak, pImpl->level + 0.8f * pImpl->releaseRate));
}

void Norepinephrine::setArousalLevel(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 2.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getSignalToNoiseRatio() const {
    return pImpl->signalToNoiseRatio;
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
