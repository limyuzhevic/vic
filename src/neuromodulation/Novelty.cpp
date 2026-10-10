#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidationRate;
    
    // Recent attention weighting
    float recentAttentionWeight;
    float attentionDecay;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , attentionGain(0.5f), memoryConsolidationRate(0.8f), recentAttentionWeight(0.0f), attentionDecay(0.95f) {}
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
    // ACh modulates attention and memory consolidation
    // Higher ACh increases attentional focus and memory encoding
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays and is modulated by attention demands
    // Natural decay rate reflects cholinergic tone
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay recent attention weighting
    pImpl->recentAttentionWeight *= pImpl->attentionDecay;
}

// Signal attentional focus based on task importance
void Acetylcholine::signalAttention(float attentionDemand, float taskImportance) {
    // ACh release increases with attentional demand and task importance
    float burst = attentionDemand * taskImportance * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update attention weighting
    pImpl->recentAttentionWeight = std::max(pImpl->recentAttentionWeight, attentionDemand * 0.5f);
}

// Enhance memory consolidation through cholinergic modulation
void Acetylcholine::enhanceMemoryConsolidation(float consolidationFactor) {
    // ACh facilitates memory consolidation
    float memoryBoost = consolidationFactor * pImpl->memoryConsolidationRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryBoost * 0.3f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float noveltySensitivity;
    
    // Locus coeruleus firing rate influences
    float firingRate;
    float adaptationRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f)
        , arousalLevel(0.5f), vigilanceLevel(0.5f), noveltySensitivity(0.8f)
        , firingRate(0.0f), adaptationRate(0.1f) {}
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
    // NE modulates arousal and learning rate
    // Higher NE increases alertness and facilitates learning
    return 0.3f + 0.7f * pImpl->level * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics involve complex locus coeruleus firing patterns
    // Responds to surprising events and arousal demands
    
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Adapt firing rate based on sustained stimulation
    pImpl->firingRate = std::max(0.0f, pImpl->firingRate - pImpl->adaptationRate * static_cast<float>(dt));
}

// Signal arousal based on environmental demands
void Norepinephrine::signalArousal(float arousalDemand, bool isNovelEvent) {
    // NE release increases with arousal demands and novelty
    float burst = arousalDemand * pImpl->releaseRate * (isNovelEvent ? 1.5f : 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update vigilance
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + arousalDemand * 0.1f);
    
    // Novel events strongly drive NE release (surprise response)
    if (isNovelEvent) {
        pImpl->level = std::min(pImpl->peak, pImpl->level * 1.2f);
    }
}

// Enhance vigilance and focus during challenging situations
void Norepinephrine::enhanceVigilance(float vigilanceBoost, bool highStress) {
    float boost = vigilanceBoost * (highStress ? 1.5f : 1.0f);
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + boost * 0.1f);
    
    // Increase NE level for sustained attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.2f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodState;
    float impulsivityLevel;
    float socialBehavior;
    float punishSensitivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f)
        , moodState(0.5f), impulsivityLevel(0.5f), socialBehavior(0.5f), punishSensitivity(0.5f) {}
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
    // Serotonin modulates mood and behavioral flexibility
    // Balanced serotonin promotes stable learning and adaptive behavior
    return 0.5f + 0.5f * (pImpl->level * 0.5f + 0.5f) * pImpl->moodState;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has complex dynamics with phasic and tonic activity
    // Responds to reward prediction errors and punishment
    
    // Natural decay and recovery towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Mood state modulates sensitivity to rewards and punishments
    if (pImpl->moodState < 0.3f) {
        // Low mood - higher sensitivity to negative events
        pImpl->impulsivityLevel = std::min(1.0f, pImpl->impulsivityLevel + 0.05f);
    } else if (pImpl->moodState > 0.7f) {
        // High mood - lower inhibition
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.05f);
    }
}

// Signal reward processing and mood regulation
void Serotonin::signalRewardProcessing(float rewardValue, bool isExpected) {
    if (isExpected) {
        // Expected rewards produce modest serotonin release
        float boost = rewardValue * pImpl->releaseRate * 0.3f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
        
        // Improve mood state
        pImpl->moodState = std::min(1.0f, pImpl->moodState + rewardValue * 0.1f);
    } else {
        // Unexpected rewards (prediction errors) produce stronger response
        float boost = rewardValue * pImpl->releaseRate * 0.6f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
        
        // Can improve or worsen mood depending on valence
        pImpl->moodState = std::min(1.0f, pImpl->moodState + rewardValue * 0.15f);
    }
}

// Signal punishment and learning from negative outcomes
void Serotonin::signalPunishment(float punishmentIntensity, bool isNovelPunishment) {
    // Punishment triggers serotonin release (inhibition and avoidance learning)
    float burst = punishmentIntensity * pImpl->releaseRate * (isNovelPunishment ? 1.2f : 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Decrease mood state
    pImpl->moodState = std::max(0.0f, pImpl->moodState - punishmentIntensity * 0.1f);
    
    // Increase punish sensitivity
    pImpl->punishSensitivity = std::min(1.0f, pImpl->punishSensitivity + punishmentIntensity * 0.05f);
}

// Enhance social behavior and reduce impulsivity
void Serotonin::enhanceSocialBehavior(float socialBoost, bool isSocialReward) {
    float boost = socialBoost * 0.2f;
    
    if (isSocialReward) {
        // Social rewards increase serotonin
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + boost);
        
        // Reduce impulsivity
        pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - boost * 0.5f);
    }
}

} // namespace nlm