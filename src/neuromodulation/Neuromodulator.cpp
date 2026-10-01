#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

}

// ============================================================================
// Acetylcholine Implementation
// ============================================================================

struct Acetylcholine::Impl {
    float level;
    float attentionBoost;
    float memoryBoost;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), attentionBoost(1.5f), memoryBoost(2.0f), 
             baseline(0.0f), decayRate(0.05f), releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances attention and memory consolidation
    // Higher ACh increases the effectiveness of attention-based learning
    return 1.0f + pImpl->level * (pImpl->attentionBoost - 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float strength) {
    // Boost attention when receiving attentional signals
    pImpl->level = std::min(pImpl->level + strength * pImpl->releaseRate, 1.0f);
}

void Acetylcholine::enhanceMemory(float memoryStrength) {
    // Enhance memory consolidation
    pImpl->level = std::min(pImpl->level + memoryStrength * pImpl->memoryBoost * pImpl->releaseRate, 1.0f);
}

// ============================================================================
// Norepinephrine Implementation
// ============================================================================

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilance;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), arousalLevel(1.2f), vigilance(1.8f), 
             baseline(0.0f), decayRate(0.08f), releaseRate(1.0f) {}
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
    // Norepinephrine increases arousal and vigilance
    // Higher NE enhances focus and alertness
    return 1.0f + pImpl->level * (pImpl->arousalLevel - 1.0f) +
           0.2f * pImpl->level * (pImpl->vigilance - 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float strength) {
    // Boost arousal when experiencing stimulating events
    pImpl->level = std::min(pImpl->level + strength * pImpl->releaseRate, 1.0f);
}

void Norepinephrine::increaseVigilance(float increase) {
    // Increase vigilance/focus
    pImpl->level = std::min(pImpl->level + increase * pImpl->vigilance * pImpl->releaseRate, 1.0f);
}

// ============================================================================
// Serotonin Implementation
// ============================================================================

struct Serotonin::Impl {
    float level;
    float moodEffect;
    float impulseControl;
    float socialBehavior;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), moodEffect(1.3f), impulseControl(0.8f), 
             socialBehavior(1.4f), baseline(0.0f), decayRate(0.06f), releaseRate(1.0f) {}
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

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and impulse control
    // Higher 5-HT can stabilize mood but reduce impulsivity
    return 1.0f - 0.2f * pImpl->level * (1.0f - pImpl->impulseControl);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Affect mood when experiencing events
    pImpl->level = std::min(pImpl->level + mood * pImpl->moodEffect * pImpl->releaseRate, 1.0f);
}

void Serotonin::modifyImpulsivity(float impulseChange) {
    // Change impulsivity levels
    pImpl->level = std::min(pImpl->level + impulseChange * (1.0f - pImpl->impulseControl), 1.0f);
}

void Serotonin::enhanceSocialBehavior(float socialBoost) {
    // Boost social behavior
    pImpl->level = std::min(pImpl->level + socialBoost * pImpl->socialBehavior * pImpl->releaseRate, 1.0f);
}

} // namespace nlm
