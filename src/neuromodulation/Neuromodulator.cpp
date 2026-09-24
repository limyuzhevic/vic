#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// ========== NEUROMODULATION SYSTEMS ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionWeight(1.0f) {}
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
    // ACh modulates attention and working memory consolidation
    // Higher ACh increases attention and memory strength
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: Decay towards baseline, peaks on attention-relevant stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // ACh peaks in response to attention-grabbing stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::enhanceWorkingMemory(float memoryStrength) {
    // ACh enhances working memory storage and retrieval
    // Effect is proportional to current ACh level
    float enhancement = pImpl->level * memoryStrength * pImpl->attentionWeight;
    // In a full implementation, this would be passed to working memory systems
    // For now, we'll just log the enhancement
    if (enhancement > 0.01f) {
        NLM_LOG_DEBUG("Acetylcholine enhanced working memory by " + std::to_string(enhancement));
    }
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float vigilanceWeight;
    float alertnessDuration;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f), 
             vigilanceWeight(1.0f), alertnessDuration(0.0f) {}
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
    // NE modulates arousal and vigilance
    // Higher NE increases alertness and plasticity during novel situations
    if (pImpl->level < 0.3f) {
        return 1.0f;  // Baseline vigilance
    } else {
        return 0.5f + pImpl->level;  // Increased plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: Slower decay, can have spontaneous increases
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Spontaneous alertness increases during quiet periods
    if (pImpl->level < 0.2f) {
        float spontaneousIncrease = dt * 0.001f;  // Slow spontaneous increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + spontaneousIncrease);
    }
}

void Norepinephrine::signalNovelty(float novelty) {
    // NE spikes strongly in response to novel or surprising stimuli
    float spike = std::min(1.0f, novelty * pImpl->releaseRate);
    pImpl->level = std::min(pImpl->peak, pImpl->level + spike);
    pImpl->alertnessDuration = 100.0f;  // Set alertness duration
}

void Norepinephrine::enhanceSensoryProcessing(float gain) {
    // NE enhances sensory processing and signal-to-noise ratio
    float processingGain = pImpl->level * gain * pImpl->vigilanceWeight;
    // In a full implementation, this would be passed to sensory systems
    if (processingGain > 0.01f) {
        NLM_LOG_DEBUG("Norepinephrine enhanced sensory processing by " + std::to_string(processingGain));
    }
}

void Norepinephrine::getAlertnessState() const {
    if (pImpl->alertnessDuration > 0.0f) {
        NLM_LOG_DEBUG("Norepinephrine: High alertness state for " + 
                     std::to_string(pImpl->alertnessDuration) + " steps");
    }
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodWeight;
    float rewardResponse;
    float inhibitionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodWeight(1.0f), rewardResponse(0.0f), inhibitionLevel(0.0f) {}
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
    // Serotonin modulates mood and reward processing
    // Higher serotonin promotes stability and reduces impulsivity
    if (pImpl->level < 0.3f) {
        return 0.8f - pImpl->level * 0.2f;  // Lower plasticity when low
    } else {
        return 0.5f + pImpl->level * 0.1f;  // Moderate plasticity
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: Slower decay, responds to reward prediction errors
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Natural decline after reward responses
    if (pImpl->rewardResponse > 0.0f) {
        pImpl->rewardResponse -= dt * 0.01f;
    }
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward prediction errors (inverse dopamine)
    // Low reward or unexpected lack of reward increases serotonin
    float response = -reward * pImpl->releaseRate;  // Negative response to reward
    pImpl->level = std::max(pImpl->level + response, pImpl->baseline);
    pImpl->rewardResponse = std::abs(response);
}

void Serotonin::signalPunishment(float punishment) {
    // Strong response to punishment
    float response = punishment * pImpl->releaseRate * 2.0f;  // Stronger response
    pImpl->level = std::min(pImpl->peak, pImpl->level + response);
}

void Serotonin::modulateMood(float mood) {
    // Serotonin modulates mood and emotional state
    float moodEffect = mood * pImpl->moodWeight;
    pImpl->level = std::max(pImpl->baseline, pImpl->level + moodEffect * 0.1f);
}

void Serotonin::reduceImpulsivity() {
    // Serotonin reduces impulsive behavior
    float inhibition = pImpl->level * 0.2f;
    if (inhibition > 0.05f) {
        // In a full implementation, this would be passed to action selection
        NLM_LOG_DEBUG("Serotonin reduced impulsivity by " + std::to_string(inhibition));
    }
}

} // namespace nlm
