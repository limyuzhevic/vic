#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedReward;
    float novelty;
    float explorationTarget;
    float learningRate;
    float temporalHorizon;
    float eligibilityTraceDecay;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , predictionError(0.0f), expectedReward(0.0f), novelty(0.0f), explorationTarget(0.5f)
            , learningRate(0.05f), temporalHorizon(5.0f), eligibilityTraceDecay(0.1f) {}
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
    // Implement real dopamine-modulated plasticity factor
    // Biological inverted-U: optimal plasticity at moderate dopamine levels
    // Too little dopamine = low learning, too much = excessive plasticity, not enough for selective learning
    
    float optimalDopamine = 0.4f;  // Peak of inverted-U
    float width = 0.3f;           // Width of inverted-U
    
    // Compute inverted-U curve
    float deviation = std::abs(pImpl->level - optimalDopamine);
    float plasticity = 1.0f - (deviation / width) * (deviation / width);
    
    // Ensure plasticity stays within reasonable bounds
    plasticity = std::clamp(plasticity, 0.1f, 2.0f);
    
    // Modulate by novelty (novelty enhances plasticity for learning new things)
    plasticity *= (1.0f + pImpl->novelty * 0.5f);
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics with temporal integration
    float dtFloat = static_cast<float>(dt);
    
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Integrate prediction error with temporal decay (short-term memory)
    pImpl->predictionError *= std::exp(-dtFloat * pImpl->temporalHorizon);
    pImpl->expectedReward *= std::exp(-dtFloat * pImpl->temporalHorizon);
    
    // Novelty adaptation (slow decay)
    pImpl->novelty *= std::exp(-dtFloat * 0.5f);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling with burst dynamics
    // Phasic dopamine burst proportional to reward magnitude
    float burst = reward * pImpl->releaseRate;
    
    // Add temporal eligibility trace for credit assignment
    pImpl->predictionError = burst;
    pImpl->expectedReward = reward;
    
    // Combine with tonic level for sustained response
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst * 0.5f);
    
    // Also update novelty (surprise)
    pImpl->novelty = std::max(pImpl->novelty, std::abs(reward - pImpl->expectedReward));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // R-PE can be positive (better than expected) or negative (worse)
    pImpl->predictionError = error;
    
    // Immediate phasic response
    if (error > 0) {
        // Positive prediction error: unexpected reward -> dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
    } else if (error < 0) {
        // Negative prediction error: missing reward -> dopamine dip
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
    
    // Learning via temporal difference: update expected reward
    pImpl->expectedReward += pImpl->learningRate * error;
    pImpl->expectedReward = std::clamp(pImpl->expectedReward, 0.0f, 1.0f);
}

// Novelty detection and integration
float Dopamine::getNovelty() const {
    return pImpl->novelty;
}

// Prediction error access
float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

// Expected reward access
float Dopamine::getExpectedReward() const {
    return pImpl->expectedReward;
}

// Exploration drive based on novelty and prediction error
float Dopamine::getExplorationDrive() const {
    float drive = pImpl->novelty * 0.7f + std::abs(pImpl->predictionError) * 0.3f;
    return std::clamp(drive, 0.0f, 1.0f);
}

// Integrate novelty signal
void Dopamine::integrateNovelty(float novelty) {
    pImpl->novelty = std::max(pImpl->novelty, novelty);
}

// Set novelty level (for Brain integration)
void Dopamine::setNovelty(float novelty) {
    pImpl->novelty = novelty;
}
};

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , attentionGain(1.0f), memoryConsolidation(1.0f) {}
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
    // Acetylcholine enhances plasticity for memory consolidation
    // Optimal memory consolidation at moderate ACh levels
    float optimalACh = 0.5f;
    float width = 0.4f;
    
    float deviation = std::abs(pImpl->level - optimalACh);
    float memoryEffect = 1.0f - (deviation / width) * (deviation / width);
    
    // More ACh = stronger memory consolidation
    memoryEffect *= (1.0f + pImpl->level * 0.5f);
    
    return std::clamp(memoryEffect, 0.1f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh modulates attention and memory processes
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Attention gain modulation
    pImpl->attentionGain *= std::exp(-dtFloat * 0.5f);
    
    // Memory consolidation adaptation
    pImpl->memoryConsolidation *= std::exp(-dtFloat * 0.3f);
}

void Acetylcholine::consolidateMemory(float memoryStrength) {
    pImpl->memoryConsolidation = memoryStrength;
}

void Acetylcholine::modulateAttention(float attentionGain) {
    pImpl->attentionGain = attentionGain;
}

// ========== NOREPINEPHRINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressLevel;
    float alertness;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , arousalLevel(1.0f), stressLevel(0.0f), alertness(1.0f) {}
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
    // Norepinephrine enhances arousal and focused attention
    // Optimal vigilance at moderate NE levels
    float optimalNE = 0.3f;
    float width = 0.25f;
    
    float deviation = std::abs(pImpl->level - optimalNE);
    float vigilance = 1.0f - (deviation / width) * (deviation / width);
    
    // More NE = stronger arousal
    vigilance *= (1.0f + pImpl->level * 0.7f);
    
    return std::clamp(vigilance, 0.1f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE modulates arousal and stress responses
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Alertness adaptation
    pImpl->alertness *= std::exp(-dtFloat * 0.4f);
    
    // Stress level decay
    pImpl->stressLevel *= std::exp(-dtFloat * 0.2f);
}

void Norepinephrine::enhanceAlertness(float arousalLevel) {
    pImpl->arousalLevel = arousalLevel;
}

void Norepinephrine::triggerStressResponse(float stressLevel) {
    pImpl->stressLevel = stressLevel;
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float socialCohesion;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , moodLevel(0.5f), socialCohesion(1.0f), impulsivity(1.0f) {}
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
    // Serotonin modulates mood and social behavior
    // Optimal mood regulation at moderate 5-HT levels
    float optimal5HT = 0.6f;
    float width = 0.35f;
    
    float deviation = std::abs(pImpl->level - optimal5HT);
    float moodEffect = 1.0f - (deviation / width) * (deviation / width);
    
    // More 5-HT = stronger mood regulation
    moodEffect *= (1.0f + pImpl->level * 0.4f);
    
    return std::clamp(moodEffect, 0.1f, 3.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT modulates mood and social processes
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Mood adaptation
    pImpl->moodLevel *= std::exp(-dtFloat * 0.3f);
    
    // Social cohesion adaptation
    pImpl->socialCohesion *= std::exp(-dtFloat * 0.25f);
    
    // Impulsivity adaptation
    pImpl->impulsivity *= std::exp(-dtFloat * 0.35f);
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->moodLevel = moodLevel;
}

void Serotonin::modulateSocialBehavior(float socialCohesion) {
    pImpl->socialCohesion = socialCohesion;
}

void Serotonin::controlImpulsivity(float inhibitionLevel) {
    pImpl->impulsivity = inhibitionLevel;
}

} // namespace nlm