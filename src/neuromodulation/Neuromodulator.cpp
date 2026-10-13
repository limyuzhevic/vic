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

// Apply memory consolidation boost based on dopamine levels
void Dopamine::applyMemoryConsolidationBoost() {
    if (pImpl->level > 0.2f) {
        // Dopamine modulates memory consolidation - higher levels enhance it
        float consolidationBoost = 1.0f + pImpl->level * 1.5f;
        pImpl->level *= consolidationBoost;
    }
}

// Enhance learning through reward prediction error
void Dopamine::enhanceLearningFromPredictionError() {
    // Use prediction error to modulate learning rate
    if (pImpl->level > 0.1f) {
        // Higher prediction errors lead to stronger learning
        float learningBoost = 1.0f + std::abs(pImpl->level - 0.5f) * 2.0f;
        pImpl->level *= learningBoost;
    }
}

// Modulate working memory stability
void Dopamine::stabilizeWorkingMemory() {
    // Dopamine affects working memory persistence
    if (pImpl->level > 0.3f) {
        // Higher dopamine stabilizes working memory traces
        float stabilityBoost = 1.0f + (pImpl->level - 0.3f) * 0.5f;
        pImpl->level *= stabilityBoost;
    }
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodEnhancement;
    float impulsivityReduction;
    float socialBehaviorModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             moodEnhancement(1.3f), impulsivityReduction(1.2f), socialBehaviorModulation(1.1f) {}
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
    // 5-HT modulates mood and impulsivity, affecting learning stability
    return 1.0f + pImpl->level * pImpl->moodEnhancement;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::enhanceMood() {
    // Improve mood and motivation
    if (pImpl->level > 0.3f) {
        float moodStrength = 1.0f + pImpl->level * pImpl->moodEnhancement;
        // Improved mood enhances learning and memory consolidation
        pImpl->level *= moodStrength;
    }
}

void Serotonin::reduceImpulsivity() {
    // Reduce impulsive actions and promote deliberation
    if (pImpl->level > 0.2f) {
        float impulseControl = 1.0f - pImpl->level * (1.0f - pImpl->impulsivityReduction);
        // This would affect action selection in integrated systems
        pImpl->level *= impulseControl;
    }
}

void Serotonin::modulateSocialBehavior(float socialInput) {
    // Modulate social behavior based on social context
    if (socialInput > 0.0f) {  // Positive social context
        float socialBoost = 1.0f + socialInput * pImpl->socialBehaviorModulation;
        pImpl->level = std::min(pImpl->peak, pImpl->level * socialBoost);
    } else {  // Negative social context
        pImpl->level = std::max(pImpl->baseline, pImpl->level - std::abs(socialInput) * 0.3f);
    }
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    float vigilanceIncrease;
    float anxietyModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             arousalBoost(1.5f), vigilanceIncrease(1.8f), anxietyModulation(0.8f) {}
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
    // NE modulates arousal and vigilance, affecting learning
    return 1.0f + pImpl->level * pImpl->vigilanceIncrease;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal() {
    // Increase arousal for heightened attention
    if (pImpl->level > 0.2f) {
        float arousalStrength = 1.0f + pImpl->level * pImpl->arousalBoost;
        pImpl->level *= arousalStrength;
    }
}

void Norepinephrine::increaseVigilance() {
    // Increase vigilance for threat detection
    if (pImpl->level > 0.3f) {
        float vigilanceStrength = 1.0f + pImpl->level * 2.0f;
        // Vigilance affects processing of novel stimuli
        pImpl->level *= vigilanceStrength;
    }
}

void Norepinephrine::modulateAnxiety(float emotionalValence) {
    // NE modulates anxiety level based on emotional valence
    if (emotionalValence < 0.0f) {  // Negative valence
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(emotionalValence) * pImpl->anxietyModulation);
    } else if (emotionalValence > 0.5f) {  // High positive valence
        pImpl->level = std::max(pImpl->baseline, pImpl->level - emotionalValence * 0.5f);
    }
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float consolidationBoost;
    float attentionModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             consolidationBoost(2.0f), attentionModulation(1.5f) {}
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
    // ACh enhances both memory consolidation and attention
    return 1.0f + pImpl->level * pImpl->attentionModulation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Boost memory consolidation when ACh is high
    if (pImpl->level > 0.3f) {
        // Consolidation enhancement is exponential with ACh level
        float consolidationStrength = 1.0f + (pImpl->level - 0.3f) * pImpl->consolidationBoost;
        pImpl->level *= consolidationStrength;
    }
}

void Acetylcholine::modulateAttentionSelection() {
    // ACh modulates attention - higher levels increase selective attention
    if (pImpl->level > 0.2f) {
        // Attention modulation increases with ACh
        float attentionStrength = 1.0f + pImpl->level * pImpl->attentionModulation;
        // This would modulate attention system when integrated
        pImpl->level *= attentionStrength;
    }
}

void Acetylcholine::signalRewardPredictionError(float error) {
    // ACh responds to reward prediction error
    // Facilitates learning when prediction errors occur
    pImpl->level = std::max(pImpl->level, std::min(pImpl->peak, pImpl->level + error * 1.5f));
}
