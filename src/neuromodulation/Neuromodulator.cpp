#include "Neuromodulator.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
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

} // namespace nlm

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionGain(0.5f), memoryBoost(0.5f) {}
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
    // Acetylcholine modulates plasticity by enhancing working memory encoding
    // Higher ACh increases plasticity for memory-related processes
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionEvent() {
    // Burst of acetylcholine on attentional events
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate);
}

void Acetylcholine::signalNoveltyEvent() {
    // Acetylcholine responds to novel stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
}

void Acetylcholine::signalWorkingMemoryEvent() {
    // Acetylcholine enhances working memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

} // namespace nlm

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float learningRateMod;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousal(0.5f), vigilance(0.5f), learningRateMod(0.5f) {}
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
    // Norepinephrine modulates plasticity by enhancing arousal and learning rate
    // Higher NE increases learning rate and plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousalEvent() {
    // Burst of norepinephrine on arousing events
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate);
}

void Norepinephrine::signalSurpriseEvent() {
    // Norepinephrine responds to surprise/novelty
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.7f * pImpl->releaseRate);
}

void Norepinephrine::signalPredictionErrorEvent(float error) {
    // Norepinephrine responds to prediction errors
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float rewardPredictionError;
    float explorationFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), mood(0.5f), impulsivity(0.5f), rewardPredictionError(0.5f), explorationFactor(0.5f) {}
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
    // Serotonin modulates plasticity by reducing impulsivity and enhancing reward prediction
    // Higher 5-HT decreases impulsivity and affects reward-based plasticity
    return 0.5f + 0.3f * pImpl->level * pImpl->rewardPredictionError;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalRewardEvent(float reward) {
    // Serotonin responds to reward and modulates reward prediction error
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->rewardPredictionError = std::min(1.0f, pImpl->rewardPredictionError + reward * 0.5f);
}

void Serotonin::signalSocialEvent() {
    // Serotonin responds to social interactions
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.8f * pImpl->releaseRate);
}

void Serotonin::signalPredictionErrorEvent(float error) {
    // Serotonin responds to prediction errors and modulates exploration
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->explorationFactor = std::min(1.0f, pImpl->explorationFactor + std::abs(error) * 0.3f);
}

void Serotonin::reduceImpulsivity() {
    // Higher serotonin reduces impulsivity
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - 0.1f);
}

} // namespace nlm
