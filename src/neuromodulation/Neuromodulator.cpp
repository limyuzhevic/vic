#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// ========== NEUROMODULATOR IMPLEMENTATIONS ==========

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float workingMemoryModulation;
    float developmentFactor;
    float actionSelectionWeight;
    float timeSinceLastReward;
    float predictionErrorHistory[3]; // Rolling window of prediction errors
    int historyIndex;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), 
             releaseRate(1.5f), workingMemoryModulation(0.0f), 
             developmentFactor(1.0f), actionSelectionWeight(0.5f),
             timeSinceLastReward(0.0f), historyIndex(0) {
        for (int i = 0; i < 3; ++i) predictionErrorHistory[i] = 0.0f;
    }
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
    // Real dopamine modulation of STDP:
    // - High dopamine enhances LTP for salient events
    // - Low dopamine promotes LTD for unimportant events
    // - Enables reward-modulated learning
    float basePlasticity = 0.5f;
    
    // Dopamine modulates plasticity gates
    float dopamine gating = std::clamp(pImpl->level * 2.0f - 0.5f, -0.5f, 1.0f);
    gating = (gating + 0.5f) * 0.5f; // Normalize to [0, 1]
    
    // Reward prediction error component
    float predictionErrorAvg = 0.0f;
    for (int i = 0; i < 3; ++i) predictionErrorAvg += pImpl->predictionErrorHistory[i];
    predictionErrorAvg /= 3.0f;
    
    // Compute plasticity factor
    float plasticityFactor = basePlasticity + gating * 0.5f;
    
    // Add prediction error contribution
    plasticityFactor += predictionErrorAvg * 0.3f;
    
    // Add working memory component
    plasticityFactor += pImpl->workingMemoryModulation * 0.2f;
    
    // Add development component
    plasticityFactor += pImpl->developmentFactor * 0.1f;
    
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics:
    // - Phasic responses to rewards and prediction errors
    // - Tonic baseline level
    // - Habituation and adaptation
    
    timeSinceLastReward += dt;
    
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply action selection influence
    if (pImpl->level > 0.3f) {
        // Strong dopamine promotes approach behaviors
        pImpl->actionSelectionWeight = std::min(1.0f, pImpl->actionSelectionWeight + 0.01f);
    } else {
        // Low dopamine reduces action vigor
        pImpl->actionSelectionWeight = std::max(0.1f, pImpl->actionSelectionWeight - 0.005f);
    }
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst for unexpected reward
    float rewardGain = reward * 2.0f; // Amplify reward signal
    
    // Short-term habituation based on time since last reward
    float habituation = 1.0f / (1.0f + timeSinceLastReward * 0.01f);
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardGain * habituation * pImpl->releaseRate);
    
    // Update prediction error history
    float predictionError = reward - 0.5f; // Expected reward was 0.5
    pImpl->predictionErrorHistory[pImpl->historyIndex] = predictionError;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 3;
    
    timeSinceLastReward = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic dopamine response to prediction errors
    // Positive error (better than expected): DA burst
    // Negative error (worse than expected): DA dip
    
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 1.5f);
    
    // Store error in history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 3;
}

void Dopamine::setWorkingMemoryModulation(float mod) {
    pImpl->workingMemoryModulation = std::clamp(mod, 0.0f, 1.0f);
}

void Dopamine::setDevelopmentFactor(float factor) {
    pImpl->developmentFactor = std::clamp(factor, 0.1f, 2.0f);
}

void Dopamine::setActionSelectionWeight(float weight) {
    pImpl->actionSelectionWeight = std::clamp(weight, 0.0f, 1.0f);
}

float Dopamine::getWorkingMemoryModulation() const {
    return pImpl->workingMemoryModulation;
}

float Dopamine::getDevelopmentFactor() const {
    return pImpl->developmentFactor;
}

float Dopamine::getActionSelectionWeight() const {
    return pImpl->actionSelectionWeight;
}

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float workingMemoryStrength;
    float signalToNoiseRatio;
    float focusArea[5]; // Track areas of enhanced focus
    int focusIndex;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.03f), 
             releaseRate(2.0f), attentionGain(1.0f), workingMemoryStrength(0.0f),
             signalToNoiseRatio(1.0f), focusIndex(0) {
        for (int i = 0; i < 5; ++i) focusArea[i] = 1.0f;
    }
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
    
    // Update attention and memory effects based on ACh level
    updateAttentionEffects();
    updateWorkingMemoryEffects();
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity through attention gating
    float attention gating = pImpl->level;
    
    // Enhanced signal-to-noise ratio improves encoding
    float snrBenefit = pImpl->signalToNoiseRatio * 0.3f;
    
    // Working memory strength contribution
    float wmBenefit = pImpl->workingMemoryStrength * 0.4f;
    
    // Attention gain contribution
    float attentionBenefit = pImpl->attentionGain * 0.3f;
    
    return 0.5f + attentionBenefit + snrBenefit + wmBenefit;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Maintain signal-to-noise ratio through homeostatic mechanisms
    float targetSnr = 1.0f + pImpl->level * 2.0f;
    pImpl->signalToNoiseRatio = pImpl->signalToNoiseRatio + 0.1f * (targetSnr - pImpl->signalToNoiseRatio);
}

void Acetylcholine::enhanceAttention(float area) {
    // Focus attention on specific neural areas
    pImpl->focusArea[pImpl->focusIndex] = std::clamp(area, 0.5f, 2.0f);
    pImpl->attentionGain = pImpl->level * 0.5f + pImpl->level * pImpl->focusArea[pImpl->focusIndex];
    
    pImpl->focusIndex = (pImpl->focusIndex + 1) % 5;
}

void Acetylcholine::strengthenWorkingMemoryTrace(float traceId, float strength) {
    // Strengthen specific working memory traces
    pImpl->workingMemoryStrength = std::min(1.0f, pImpl->workingMemoryStrength + 
                                           strength * pImpl->level * 0.5f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getWorkingMemoryStrength() const {
    return pImpl->workingMemoryStrength;
}

float Acetylcholine::getSignalToNoiseRatio() const {
    return pImpl->signalToNoiseRatio;
}

void Acetylcholine::updateAttentionEffects() {
    // Update attention-related parameters based on current ACh level
    pImpl->attentionGain = 1.0f + pImpl->level;
}

void Acetylcholine::updateWorkingMemoryEffects() {
    // Update working memory-related parameters based on current ACh level
    pImpl->workingMemoryStrength = pImpl->level * 0.8f;
}

// ========== NOREPINEPHRINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilance;
    float plasticityBoost;
    float responseSpeed;
    float noveltyDetection;
    bool responseInhibition;
    float adaptationTime;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.04f),
             releaseRate(1.8f), arousalLevel(1.0f), vigilance(1.0f),
             plasticityBoost(0.0f), responseSpeed(1.0f), noveltyDetection(0.0f),
             responseInhibition(false), adaptationTime(0.0f) {}
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
    
    // Update arousal and vigilance effects based on NE level
    updateArousalEffects();
    updateVigilanceEffects();
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity during novel events and stress
    float noveltyBenefit = pImpl->noveltyDetection * 0.5f;
    float arousalBenefit = std::min(1.0f, pImpl->arousalLevel * 0.3f);
    float vigilanceBenefit = pImpl->vigilance * 0.2f;
    
    // Response inhibition reduces plastic changes for impulsive responses
    float inhibitionPenalty = pImpl->responseInhibition ? 0.3f : 0.0f;
    
    return 1.0f + noveltyBenefit + arousalBenefit + vigilanceBenefit - inhibitionPenalty;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Homeostatic adaptation of response systems
    pImpl->adaptationTime += dt;
    if (pImpl->adaptationTime > 10.0f) {
        pImpl->responseInhibition = false;
        pImpl->adaptationTime = 0.0f;
    }
    
    // Maintain arousal and vigilance levels
    float targetArousal = 1.0f + pImpl->level * 1.5f;
    pImpl->arousalLevel = pImpl->arousalLevel + 0.1f * (targetArousal - pImpl->arousalLevel);
    
    float targetVigilance = 1.0f + pImpl->level;
    pImpl->vigilance = pImpl->vigilance + 0.1f * (targetVigilance - pImpl->vigilance);
}

void Norepinephrine::enhanceArousal(float intensity) {
    // Quick boost to arousal level
    pImpl->arousalLevel = std::min(2.0f, pImpl->arousalLevel + intensity * pImpl->level);
    setLevel(std::min(1.0f, pImpl->level + intensity * 0.3f));
}

void Norepinephrine::increaseVigilance(float amount) {
    // Increase vigilance for detecting novel/important events
    pImpl->vigilance = std::min(2.0f, pImpl->vigilance + amount * pImpl->level);
}

void Norepinephrine::boostPlasticityDuringNovelty(float novelty) {
    // Enhance plasticity when novel events occur
    pImpl->noveltyDetection = std::min(1.0f, novelty * pImpl->level);
    
    if (novelty > 0.7f) {
        // Strong novelty triggers plasticity boost
        pImpl->plasticityBoost = 1.5f;
        setLevel(std::min(1.0f, pImpl->level + 0.3f));
    }
}

void Norepinephrine::inhibitImpulsiveResponses() {
    // Inhibit reactive/impulsive responses
    pImpl->responseInhibition = true;
    pImpl->adaptationTime = 0.0f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getPlasticityBoost() const {
    return pImpl->plasticityBoost;
}

float Norepinephrine::getResponseSpeed() const {
    return pImpl->responseSpeed;
}

bool Norepinephrine::getResponseInhibition() const {
    return pImpl->responseInhibition;
}

void Norepinephrine::updateArousalEffects() {
    // Update arousal-related parameters based on current NE level
    pImpl->arousalLevel = 1.0f + pImpl->level * 1.5f;
}

void Norepinephrine::updateVigilanceEffects() {
    // Update vigilance-related parameters based on current NE level
    pImpl->vigilance = 1.0f + pImpl->level;
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float patience;
    float impulsivity;
    float delayedGratisfaction;
    float mood;
    float inhibitionStrength;
    float reactiveResponseLevel;
    float rewardProcessing;
    
    Impl() : level(0.0f), baseline(0.03f), peak(1.0f), decayRate(0.06f),
             releaseRate(1.2f), patience(0.5f), impulsivity(0.5f),
             delayedGratisfaction(0.5f), mood(0.5f), inhibitionStrength(0.0f),
             reactiveResponseLevel(1.0f), rewardProcessing(1.0f) {}
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
    
    // Update mood and impulsivity effects based on 5-HT level
    updateMoodEffects();
    updateImpulsivityEffects();
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for stable learning and mood regulation
    float moodBenefit = pImpl->mood * 0.3f;
    float patienceBenefit = pImpl->patience * 0.3f;
    float delayedGratisfactionBenefit = pImpl->delayedGratisfaction * 0.2f;
    float rewardProcessingBenefit = pImpl->rewardProcessing * 0.2f;
    
    // Inhibition strength reduces impulsive plastic changes
    float inhibitionBenefit = pImpl->inhibitionStrength * 0.5f;
    
    // Reactive response reduction stabilizes plasticity
    float reactivityReduction = (1.0f - pImpl->reactiveResponseLevel) * 0.3f;
    
    return 1.0f + moodBenefit + patienceBenefit + 
           delayedGratisfactionBenefit + rewardProcessingBenefit + 
           inhibitionBenefit + reactivityReduction;
}

void Serotonin::update(TimestepDuration dt) {
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Mood and reward processing homeostasis
    float targetMood = 0.5f + pImpl->level * 0.5f;
    pImpl->mood = pImpl->mood + 0.05f * (targetMood - pImpl->mood);
    
    float targetRewardProcessing = 1.0f - pImpl->level * 0.3f;
    pImpl->rewardProcessing = pImpl->rewardProcessing + 0.05f * (targetRewardProcessing - pImpl->rewardProcessing);
    
    // Maintain patience and delayed gratification
    float targetPatience = 0.5f + pImpl->level * 0.6f;
    pImpl->patience = pImpl->patience + 0.05f * (targetPatience - pImpl->patience);
    
    float targetDelayedGratisfaction = 0.5f + pImpl->level * 0.5f;
    pImpl->delayedGratisfaction = pImpl->delayedGratisfaction + 0.05f * (targetDelayedGratisfaction - pImpl->delayedGratisfaction);
}

void Serotonin::enhancePatience(float amount) {
    // Increase patience and tolerance for delay
    pImpl->patience = std::min(1.0f, pImpl->patience + amount * pImpl->level * 0.5f);
    setLevel(std::min(1.0f, pImpl->level + amount * 0.2f));
}

void Serotonin::decreaseImpulsivity(float amount) {
    // Decrease impulsive responses
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - amount * pImpl->level * 0.5f);
}

void Serotonin::enhanceDelayedGratisfaction(float amount) {
    // Enhance ability to delay gratification
    pImpl->delayedGratisfaction = std::min(1.0f, pImpl->delayedGratisfaction + amount * pImpl->level * 0.3f);
}

void Serotonin::improveMood(float amount) {
    // Improve mood state
    pImpl->mood = std::clamp(pImpl->mood + amount * pImpl->level * 0.4f, 0.0f, 1.0f);
}

void Serotonin::inhibitReactiveResponses() {
    // Inhibit reactive and impulsive responses
    pImpl->reactiveResponseLevel = std::max(0.0f, pImpl->reactiveResponseLevel - 0.5f * pImpl->level);
    pImpl->inhibitionStrength = std::min(1.0f, pImpl->inhibitionStrength + 0.3f * pImpl->level);
}

float Serotonin::getPatience() const {
    return pImpl->patience;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getDelayedGratisfaction() const {
    return pImpl->delayedGratisfaction;
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getInhibitionStrength() const {
    return pImpl->inhibitionStrength;
}

float Serotonin::getReactiveResponseLevel() const {
    return pImpl->reactiveResponseLevel;
}

float Serotonin::getRewardProcessing() const {
    return pImpl->rewardProcessing;
}

void Serotonin::updateMoodEffects() {
    // Update mood-related parameters based on current 5-HT level
    pImpl->mood = 0.5f + pImpl->level * 0.5f;
}

void Serotonin::updateImpulsivityEffects() {
    // Update impulsivity-related parameters based on current 5-HT level
    pImpl->impulsivity = 1.0f - pImpl->level * 0.8f;
}

} // namespace nlm