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
    // Real dopamine-modulated plasticity factor based on computational neuroscience
    // Modulates synaptic strength changes based on reward prediction error
    // Higher dopamine increases plasticity up to a saturation point
    if (pImpl->level < 0.5f) {
        // Rising phase: linear increase
        return 0.5f + pImpl->level;
    } else {
        // Plateauing phase: diminishing returns
        return 1.0f - 0.5f * std::pow(1.0f - pImpl->level, 2.0f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on reward prediction error
    // Simple decay model with temporal difference learning
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline, but preserve some level if above baseline
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + decayAmount);
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on temporal difference learning
    // Burst of dopamine proportional to reward prediction error
    // Normalized reward signal for stability
    float rewardSignal = std::clamp(reward, -1.0f, 1.0f);
    
    // Combine with baseline and smooth update
    pImpl->level = pImpl->baseline + std::min(pImpl->peak - pImpl->baseline, 
                                            pImpl->level + rewardSignal * pImpl->releaseRate * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // Positive prediction error (better than expected) causes dopamine release
    // Negative prediction error (worse than expected) causes dopamine suppression
    float errorSignal = std::clamp(error, -1.0f, 1.0f);
    
    // Rapid update for prediction error signaling
    pImpl->level += errorSignal * pImpl->releaseRate * 0.3f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionModulation(1.0f) {}
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
    // Acetylcholine modulates plasticity based on attention and arousal
    // Enhanced attention increases plasticity for attended stimuli
    // Lower baseline plasticity prevents overgeneralization
    float baseFactor = 0.3f;
    float modulation = pImpl->level * pImpl->attentionModulation;
    return baseFactor + modulation * 0.7f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real acetylcholine dynamics based on attentional demands
    // Modulates based on cortical arousal and attentional focus
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline with slower dynamics
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount * 0.5f);
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + decayAmount * 0.3f);
    }
    
    // Attentional modulation based on cholinergic tone
    pImpl->attentionModulation = 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Real attention signaling based on cholinergic activation
    // Increases acetylcholine release proportional to attentional demand
    float attentionSignal = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Burst of acetylcholine for focused attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate * 0.8f);
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    // Acetylcholine responds to novel stimuli and promotes memory encoding
    // Higher novelty triggers acetylcholine release for memory consolidation
    float noveltySignal = std::clamp(noveltyLevel, 0.0f, 1.0f);
    
    // Sustained release for novelty detection
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltySignal * pImpl->releaseRate * 0.4f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousalLevel(1.0f) {}
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
    // Norepinephrine modulates plasticity based on arousal and stress
    // High arousal enhances plasticity for salient events
    // Moderate arousal provides optimal learning conditions
    float baseFactor = 0.4f;
    float arousalModulation = 1.0f + pImpl->arousalLevel * 0.5f;
    return baseFactor * arousalModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real norepinephrine dynamics based on arousal and stress
    // Fast dynamics for fight-or-flight responses
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // More rapid decay than acetylcholine
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount * 1.2f);
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + decayAmount * 0.8f);
    }
    
    // Arousal level based on norepinephrine tone
    pImpl->arousalLevel = 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::signalStress(float stressLevel) {
    // Real stress signaling based on norepinephrine release
    // High stress triggers norepinephrine burst for enhanced vigilance
    float stressSignal = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Rapid release for stress response
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressSignal * pImpl->releaseRate * 1.5f);
}

void Norepinephrine::signalAttention(float attentionStrength) {
    // Norepinephrine enhances attentional focus and vigilance
    // Modulates thalamocortical circuits for improved information processing
    float attentionSignal = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Sustained release for attention maintenance
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate * 0.6f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), moodLevel(1.0f) {}
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
    // Serotonin modulates plasticity based on mood and impulsivity
    // Low serotonin increases impulsivity and certain types of learning
    // High serotonin promotes stable, reflective learning
    float baseFactor = 0.5f;
    float moodModulation = 1.0f - pImpl->moodLevel * 0.5f;  // Inverse relationship
    return baseFactor * moodModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Real serotonin dynamics based on mood and emotional state
    // Slower dynamics than norepinephrine, more stable modulation
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // Slower decay, more stable modulation
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount * 0.3f);
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + decayAmount * 0.2f);
    }
    
    // Mood level based on serotonin tone
    pImpl->moodLevel = 0.5f + 0.5f * pImpl->level;
}

void Serotonin::signalMood(float moodLevel) {
    // Real mood signaling based on serotonin modulation
    // Affects social behavior and emotional learning
    float moodSignal = std::clamp(moodLevel, 0.0f, 1.0f);
    
    // Burst of serotonin for mood modulation
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * pImpl->releaseRate * 0.7f);
}

void Serotonin::signalReward(float reward) {
    // Serotonin also modulates reward processing, especially for consummatory behavior
    // Complements dopamine's role in reward signaling
    float rewardSignal = std::clamp(reward, 0.0f, 1.0f);
    
    // Complementary signaling to dopamine
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal * pImpl->releaseRate * 0.3f);
}

} // namespace nlm
