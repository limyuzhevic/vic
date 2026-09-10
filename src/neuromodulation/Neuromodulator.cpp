#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates attention and memory encoding
    // Higher ACh increases attention and memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decays but can be released by novel stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost(float novelty) {
    // ACh release increases with novelty, enhancing attention and memory
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.08f), releaseRate(0.6f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and working memory
    // Higher NE increases arousal and working memory capacity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: decays but can be released by stressors or important events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float importance) {
    // NE release increases with event importance, enhancing arousal and vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + importance * pImpl->releaseRate);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.06f), releaseRate(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and impulsivity
    // Higher serotonin stabilizes mood and reduces impulsivity
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: decays but can be released by positive experiences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialReward(float reward) {
    // Serotonin release increases with social reward, enhancing mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

} // namespace nlm
