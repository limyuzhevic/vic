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

// Serotonin implementation

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.3f), baseline(0.3f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // TODO PHASE 2: Implement real serotonin-modulated plasticity
    // PLACEHOLDER: Serotonin may decrease plasticity at high levels
    // to promote stability and reduce impulsivity
    return 0.8f - 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodDelta) {
    // TODO PHASE 2: Implement real mood signaling
    // PLACEHOLDER: Serotonin responds to mood changes
    pImpl->level = std::clamp(pImpl->level + moodDelta * pImpl->releaseRate, 0.0f, 1.0f);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // TODO PHASE 2: Implement real social behavior modulation
    // PLACEHOLDER: Serotonin affects social responsiveness
    pImpl->level = std::clamp(pImpl->level + socialStrength * 0.1f, 0.0f, 1.0f);
}

void Serotonin::signalMoodChange(float moodDelta) {
    // TODO PHASE 2: Implement real mood signaling
    // PLACEHOLDER: Serotonin responds to mood changes
    pImpl->level = std::clamp(pImpl->level + moodDelta * pImpl->releaseRate, 0.0f, 1.0f);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // TODO PHASE 2: Implement real social behavior modulation
    // PLACEHOLDER: Serotonin affects social responsiveness
    pImpl->level = std::clamp(pImpl->level + socialStrength * 0.1f, 0.0f, 1.0f);
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float attentionGain;
    
    Impl() : level(0.2f), baseline(0.2f), decayRate(0.05f), releaseRate(1.0f), attentionGain(1.5f) {}
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
    // TODO PHASE 2: Implement real norepinephrine-modulated plasticity
    // PLACEHOLDER: Norepinephrine may increase plasticity at high levels
    // for enhanced learning during arousal
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionGain;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // TODO PHASE 2: Implement real arousal signaling
    // PLACEHOLDER: Norepinephrine responds to arousal/stimuli
    pImpl->level = std::clamp(pImpl->level + arousalLevel * pImpl->releaseRate, 0.0f, 1.0f);
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // TODO PHASE 2: Implement real vigilance modulation
    // PLACEHOLDER: Norepinephrine affects alertness and focus
    pImpl->level = std::clamp(pImpl->level + vigilanceStrength * 0.15f, 0.0f, 1.0f);
}

// Serotonin destructor

Serotonin::~Serotonin() = default;

} // namespace nlm
