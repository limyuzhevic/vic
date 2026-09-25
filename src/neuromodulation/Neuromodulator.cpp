#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryEnhancing;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , attentionGain(0.0f), memoryEnhancing(0.0f), arousal(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() { delete pImpl; }

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
    // ACh modulates attention and memory, not plasticity directly
    return 0.8f + 0.2f * pImpl->memoryEnhancing;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: promotes attention and memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update effects
    pImpl->attentionGain = pImpl->level * 0.7f;  // ACh enhances attentional focus
    pImpl->memoryEnhancing = pImpl->level * 0.5f; // ACh improves memory encoding/consolidation
    pImpl->arousal = pImpl->level * 0.3f;        // ACh increases arousal
}

void Acetylcholine::signalNovelty(float novelty) {
    // ACh responds to novel stimuli
    if (novelty > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 1.5f);
        pImpl->attentionGain = std::min(1.0f, pImpl->attentionGain + novelty * 0.1f);
        pImpl->memoryEnhancing = std::min(1.0f, pImpl->memoryEnhancing + novelty * 0.1f);
    }
}

void Acetylcholine::signalUnpredicted(float unpredicted) {
    // ACh responds to unpredicted sensory input
    if (unpredicted > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + unpredicted * pImpl->releaseRate * 1.2f);
        pImpl->arousal = std::min(1.0f, pImpl->arousal + unpredicted * 0.05f);
    }
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float vigilance;
    float alertness;
    float stress;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , vigilance(0.0f), alertness(0.0f), stress(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() { delete pImpl; }

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
    // NE modulates vigilance and learning
    return 0.6f + 0.4f * pImpl->vigilance;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: promotes alertness and vigilance
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update effects
    pImpl->vigilance = pImpl->level * 0.6f;    // NE enhances vigilance to environment
    pImpl->alertness = pImpl->level * 0.5f;   // NE increases alertness
    pImpl->stress = pImpl->level * 0.3f;      // NE responds to stressors
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE responds to stress/arousal
    if (stressLevel > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 2.0f);
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + stressLevel * 0.15f);
        pImpl->alertness = std::min(1.0f, pImpl->alertness + stressLevel * 0.1f);
    }
}

void Norepinephrine::signalRewardPredictionError(float error) {
    // NE responds to unexpected outcomes with high arousal
    if (std::abs(error) > 0.5f) {  // Strong response to large errors
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(error) * pImpl->releaseRate * 1.8f);
        pImpl->alertness = std::min(1.0f, pImpl->alertness + std::abs(error) * 0.1f);
    }
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float socialEngagement;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , mood(0.0f), impulsivity(0.0f), socialEngagement(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() { delete pImpl; }

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
    // 5-HT modulates mood and social behavior
    return 0.7f + 0.3f * pImpl->socialEngagement;
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: promotes mood stability and social behavior
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update effects
    pImpl->mood = pImpl->level * 0.5f;              // 5-HT improves mood
    pImpl->impulsivity = std::max(0.0f, pImpl->level - 0.5f) * 0.4f; // High 5-HT reduces impulsivity
    pImpl->socialEngagement = pImpl->level * 0.4f;  // 5-HT enhances social engagement
}

void Serotonin::signalSocialReward(float socialValue) {
    // 5-HT responds to social rewards
    if (socialValue > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialValue * pImpl->releaseRate * 1.3f);
        pImpl->socialEngagement = std::min(1.0f, pImpl->socialEngagement + socialValue * 0.15f);
        pImpl->mood = std::min(1.0f, pImpl->mood + socialValue * 0.1f);
    }
}

void Serotonin::signalPunishment(float punishment) {
    // 5-HT responds to punishments/sadness
    if (punishment > 0) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + punishment * pImpl->releaseRate * 0.8f);
        pImpl->mood = std::max(0.0f, pImpl->mood - punishment * 0.2f);
    }
}

} // namespace nlm
