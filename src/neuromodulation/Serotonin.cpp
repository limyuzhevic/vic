#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulseControl;
    float socialBehavior;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , mood(0.5f)
        , impulseControl(1.0f)
        , socialBehavior(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (5-HT) system initialized - Mood, impulsivity, and social behavior");
}

void Serotonin::signalSocialReward(float reward, bool isSocialInteraction) {
    // 5-HT increases with social rewards and positive outcomes
    float socialSignal = reward * (isSocialInteraction ? 2.0f : 1.0f); // Stronger for social interactions
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialSignal * pImpl->releaseRate);
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialSignal * 0.1f);
}

void Serotonin::signalAversive(float punishment) {
    // 5-HT decreases with aversive/punishing stimuli
    float aversiveSignal = punishment * 0.5f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - aversiveSignal * pImpl->releaseRate);
    pImpl->impulseControl = std::max(0.1f, pImpl->impulseControl - aversiveSignal * 0.2f);
}

void Serotonin::signalImpulseControl(float selfControlLevel) {
    // 5-HT strengthens with impulse control
    pImpl->level = std::min(pImpl->peak, pImpl->level + selfControlLevel * pImpl->releaseRate * 0.5f);
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + selfControlLevel * 0.2f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates mood-related plasticity
    // Higher serotonin promotes stable, mood-based learning
    float moodMod = 1.0f + pImpl->mood * 0.2f;  // Up to 1.2x normal plasticity
    float impulseMod = 1.0f + pImpl->impulseControl * 0.1f;  // Better impulse control = better learning
    
    // 5-HT particularly enhances plasticity for emotionally salient experiences
    return moodMod * impulseMod;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Integrate with brain's emotional regulation systems
    if (pImpl->brain) {
        // 5-HT modulates emotional regulation and decision making
        // For now, we can log that serotonin levels are being updated
        if (pImpl->level > 0.5f) {
            NLM_LOG_DEBUG("Serotonin levels high - promoting positive mood and stable behavior");
        } else {
            NLM_LOG_DEBUG("Serotonin levels low - may affect mood and impulse control");
        }
    }
}

} // namespace nlm
