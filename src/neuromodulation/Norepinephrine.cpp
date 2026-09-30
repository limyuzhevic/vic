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
    
    // Arousal and vigilance
    float arousalLevel;
    float vigilanceLevel;
    float attentionFocus;
    
    // Stress response
    bool stressActive;
    float stressLevel;
    float fightOrFlightResponse;
    
    // REM sleep
    bool inREMState;
    float remDuration;
    
    // Cognitive control
    float cognitiveControlLevel;
    float workingMemoryEnhancement;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.08f), releaseRate(2.0f)
        , arousalLevel(0.0f), vigilanceLevel(0.0f), attentionFocus(0.0f)
        , stressActive(false), stressLevel(0.0f), fightOrFlightResponse(1.0f)
        , inREMState(false), remDuration(0.0f)
        , cognitiveControlLevel(1.0f), workingMemoryEnhancement(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // Update arousal and vigilance
    pImpl->arousalLevel = 0.1f + 0.8f * std::min(pImpl->level, 1.0f);
    pImpl->vigilanceLevel = 0.2f + 0.6f * std::min(pImpl->level, 1.5f);
    
    // Enhance working memory with higher NE
    pImpl->workingMemoryEnhancement = 1.0f + 0.4f * std::min(pImpl->level, 1.0f);
    
    // Boost cognitive control
    pImpl->cognitiveControlLevel = 1.0f + 0.3f * std::min(pImpl->level, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for important, arousal-relevant experiences
    // but decreases stability of unimportant memories
    return 1.0f + 0.8f * std::min(pImpl->level, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Gradual decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Stress response decays slower
    if (pImpl->stressActive) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->decayRate * 0.5f * static_cast<float>(dt));
        if (pImpl->stressLevel < 0.1f) {
            pImpl->stressActive = false;
        }
    }
    
    // REM state duration
    if (pImpl->inREMState) {
        pImpl->remDuration += static_cast<float>(dt);
        // Decrease NE during REM
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * 2.0f * static_cast<float>(dt));
    }
}

void Norepinephrine::signalArousal(float arousalSignal) {
    pImpl->arousalLevel = std::clamp(arousalSignal, 0.0f, 1.0f);
    // Boost NE for arousal responses
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->releaseRate * 0.7f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::increaseVigilance(float vigilanceIncrement) {
    pImpl->vigilanceLevel = std::clamp(vigilanceIncrement, 0.0f, 1.0f);
    // Increase attention focus
    pImpl->attentionFocus = std::clamp(vigilanceIncrement, 0.0f, 1.0f);
    
    // Boost NE for vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceIncrement * pImpl->releaseRate * 0.5f);
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressActive = true;
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Significant NE increase for stress response
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 1.5f);
    
    // Fight or flight response
    pImpl->fightOrFlightResponse = stressLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::enterREMState() {
    pImpl->inREMState = true;
    pImpl->remDuration = 0.0f;
    
    // Decrease NE significantly for REM sleep
    pImpl->level = pImpl->baseline * 0.5f;
}

void Norepinephrine::exitREMState() {
    pImpl->inREMState = false;
    
    // Allow NE to recover
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
}

bool Norepinephrine::isInREMState() const {
    return pImpl->inREMState;
}

void Norepinephrine::enhanceCognitiveControl(float controlStrength) {
    pImpl->cognitiveControlLevel = std::clamp(controlStrength, 0.0f, 1.0f);
    
    // Boost NE for enhanced cognitive control
    pImpl->level = std::min(pImpl->peak, pImpl->level + controlStrength * pImpl->releaseRate * 0.3f);
}

float Norepinephrine::getCognitiveControlLevel() const {
    return pImpl->cognitiveControlLevel;
}

} // namespace nlm
