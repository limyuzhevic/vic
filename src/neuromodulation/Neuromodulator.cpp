#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== Dopamine Implementation ==========

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulator;  // How dopamine affects attention
    float workingMemoryModulator;  // How dopamine affects working memory
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f), releaseRate(2.0f)
        , attentionModulator(1.0f), workingMemoryModulator(1.0f) {}
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
    // PHASE 6: Implement real dopamine-modulated plasticity factor
    // Dopamine modulates multiple plasticity processes:
    // - Increases LTP probability
    // - Decreases LTD probability
    // - Enhances synaptic tagging and capture
    
    // Stronger baseline when dopamine is high
    float plasticity = pImpl->baseline + pImpl->attentionModulator * 0.4f;
    
    // Scale by current dopamine level
    // When dopamine is low (0), plasticity is baseline
    // When dopamine is high (1), plasticity is significantly enhanced
    plasticity += (1.0f - pImpl->baseline) * pImpl->level * 0.6f;
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 6: Implement real dopamine dynamics
    // More biologically realistic dopamine dynamics
    
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Adapt modulators based on activity level
    // High dopamine activity increases attention and working memory capacity
    pImpl->attentionModulator = std::clamp(1.0f + pImpl->level * 0.5f, 0.5f, 2.0f);
    pImpl->workingMemoryModulator = std::clamp(1.0f + pImpl->level * 0.3f, 0.7f, 1.5f);
}

void Dopamine::signalReward(float reward) {
    // PHASE 6: Implement real reward signaling
    // More realistic reward dynamics based on reward prediction error
    
    // Exponential burst response to reward
    float burstSize = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    pImpl->level = burstSize;
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 6: Implement reward prediction error signaling
    // Dopaminergic neurons encode reward prediction error
    // Positive error -> increase dopamine
    // Negative error -> decrease dopamine
    
    // More realistic RPE computation
    float rpe = std::clamp(error, -1.0f, 1.0f) * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + rpe);
    
    // Reset baseline if we get a strong negative error
    if (error < -0.5f) {
        pImpl->baseline = std::max(0.0f, pImpl->baseline - 0.05f);
    } else if (error > 0.5f) {
        // Slightly increase baseline on strong positive error
        pImpl->baseline = std::min(0.5f, pImpl->baseline + 0.02f);
    }
}

// ========== Acetylcholine Implementation ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionBoost;  // How ACh affects attention
    float memoryConsolidation;  // How ACh affects memory consolidation
    float cholinergicReverberation;  // ACh persistence effects
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f)
        , attentionBoost(1.0f), memoryConsolidation(1.0f), cholinergicReverberation(0.8f) {}
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
    // PHASE 6: Implement real acetylcholine-modulated plasticity
    // Acetylcholine has complex effects on plasticity:
    // - Enhances encoding of new memories
    // - Promotes working memory stability
    // - Reduces interference
    
    float plasticity = 1.0f;
    
    // Attention modulation: stronger ACh = better attention = more focused plasticity
    plasticity *= pImpl->attentionBoost;
    
    // Memory consolidation: stronger ACh = more memory consolidation
    plasticity *= pImpl->memoryConsolidation;
    
    // Reverb effects: ACh has persistent effects on synaptic strength
    plasticity *= (1.0f + pImpl->cholinergicReverberation * 0.1f);
    
    return std::clamp(plasticity, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // PHASE 6: Implement real acetylcholine dynamics
    // ACh has different dynamics from dopamine:
    // - Slower decay
    // - Strong persistence (reverberation)
    // - Modulates attention gates
    
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * 0.5f);
    
    // Maintain reverberation
    pImpl->cholinergicReverberation = std::max(0.3f, pImpl->cholinergicReverberation - 0.01f * static_cast<float>(dt));
    
    // ACh modulates attention gates
    pImpl->attentionBoost = 1.0f + pImpl->level * 0.8f;
    
    // ACh promotes memory consolidation
    pImpl->memoryConsolidation = 1.0f + pImpl->level * 0.5f;
}

// ========== Norepinephrine Implementation ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousal;  // How NE affects arousal and vigilance
    float stressResponse;  // How NE responds to stress
    float signalToLocusCoeruleus;  // NE signaling
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.15f)
        , arousal(1.0f), stressResponse(0.0f), signalToLocusCoeruleus(1.0f) {}
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
    // PHASE 6: Implement real norepinephrine-modulated plasticity
    // Norepinephrine has arousal-related effects on plasticity:
    // - Enhances memory formation during emotionally salient events
    // - Reduces irrelevant synaptic connections
    // - Improves signal-to-noise ratio
    
    float plasticity = 1.0f;
    
    // Arousal modulation: stronger NE = more arousal = enhanced plasticity for salient events
    plasticity *= pImpl->arousal;
    
    // Stress response: stress can enhance or impair plasticity depending on intensity
    plasticity *= (1.0f + pImpl->stressResponse * 0.2f);
    
    return std::clamp(plasticity, 0.3f, 2.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // PHASE 6: Implement real norepinephrine dynamics
    // NE has unique characteristics:
    // - Phasic bursts for salient events
    // - Tonic background activity
    // - Strong locus coeruleus connections
    
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update arousal and stress responses
    pImpl->arousal = 1.0f + pImpl->level * 0.7f;
    
    // Stress response builds over time
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 0.005f * static_cast<float>(dt));
    
    // NE signals to locus coeruleus for widespread effects
    pImpl->signalToLocusCoeruleus = 1.0f + pImpl->level * 0.5f;
}

// ========== Serotonin Implementation ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float mood;  // How 5-HT affects mood
    float impulsivity;  // How 5-HT affects impulsivity
    float socialBehavior;  // How 5-HT affects social behavior
    float hippocampalModulation;  // 5-HT effects on hippocampal plasticity
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f)
        , mood(1.0f), impulsivity(1.0f), socialBehavior(1.0f), hippocampalModulation(1.0f) {}
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
    // PHASE 6: Implement real serotonin-modulated plasticity
    // Serotonin has complex effects:
    // - Modulates mood and emotional state
    // - Affects impulsivity and decision making
    // - Influences social behavior
    // - Has complex effects on hippocampal plasticity (bidirectional)
    
    float plasticity = 1.0f;
    
    // Mood modulation: affects learning rate and emotional memory
    plasticity *= pImpl->mood;
    
    // Impulsivity: low 5-HT typically increases impulsivity
    plasticity /= pImpl->impulsivity;
    
    // Social behavior: affects social learning and cooperation
    plasticity *= pImpl->socialBehavior;
    
    // Hippocampal modulation: complex, sometimes inverted
    plasticity *= pImpl->hippocampalModulation;
    
    return std::clamp(plasticity, 0.2f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // PHASE 6: Implement real serotonin dynamics
    // 5-HT has complex temporal dynamics:
    // - Slower turnover than dopamine
    // - Strong effects on mood and affect
    // - Complex interactions with other systems
    
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * 0.8f);
    
    // Mood modulation effects
    pImpl->mood = 1.0f + pImpl->level * 0.5f;
    
    // Impulsivity regulation: higher 5-HT reduces impulsivity
    pImpl->impulsivity = 1.0f / (1.0f + pImpl->level * 0.8f);
    
    // Social behavior modulation
    pImpl->socialBehavior = 1.0f + pImpl->level * 0.3f;
    
    // Hippocampal modulation (complex, can be enhancing or suppressing)
    pImpl->hippocampalModulation = 1.0f + pImpl->level * 0.2f - pImpl->level * pImpl->level * 0.1f;
}

} // namespace nlm
