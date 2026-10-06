// Enhanced Neuromodulator.cpp Implementation

#include "EnhancedNeuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

// Dopamine Implementation

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float noveltyModulation;
    float plasticityMultiplier;
    
    // History for integration
    std::vector<float> rewardHistory;
    std::vector<float> errorHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), predictionError(0.0f),
             noveltyModulation(0.0f), plasticityMultiplier(1.0f) {}
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
    // Enhanced plasticity modulation based on dopamine level
    float basePlasticity = 0.5f;
    float dopamineEffect = pImpl->level * 0.6f;
    float noveltyEffect = pImpl->noveltyModulation * 0.3f;
    float predictionErrorEffect = std::abs(pImpl->predictionError) * 0.2f;
    
    float totalFactor = basePlasticity + dopamineEffect + noveltyEffect + predictionErrorEffect;
    return std::clamp(totalFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Smooth prediction error effect
    pImpl->predictionError *= 0.95f;
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized with enhanced functionality");
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics
    float burstAmount = reward * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    pImpl->rewardHistory.push_back(reward);
    
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    NLM_LOG_DEBUG("Dopamine burst: reward=" + std::to_string(reward) + ", level=" + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    float errorAmount = std::min(1.0f, std::abs(error)) * pImpl->releaseRate * 0.6f;
    
    if (error > 0.0f) {  // Positive prediction error (better than expected)
        pImpl->level += errorAmount;
    } else {  // Negative prediction error (worse than expected)
        pImpl->level = std::max(0.0f, pImpl->level - errorAmount);
    }
    
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    NLM_LOG_DEBUG("Dopamine RPE: error=" + std::to_string(error) + ", level=" + std::to_string(pImpl->level));
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void Dopamine::modulateByNovelty(float novelty) {
    // Modulate dopamine based on novelty detection
    pImpl->noveltyModulation = novelty * 0.5f;
    
    // Novelty can enhance dopamine signaling
    if (novelty > 0.5f && pImpl->level < 0.7f) {
        float noveltyBoost = (novelty - 0.5f) * 0.4f;
        pImpl->level += noveltyBoost;
    }
    
    NLM_LOG_DEBUG("Dopamine novelty modulation: novelty=" + std::to_string(novelty) + ", level=" + std::to_string(pImpl->level));
}

float Dopamine::getExplorationDrive() const {
    // Exploration drive based on novelty and prediction error
    float noveltyDrive = pImpl->noveltyModulation * 0.6f;
    float errorDrive = std::min(1.0f, std::abs(pImpl->predictionError)) * 0.4f;
    float baselineDrive = 0.2f;
    
    return baselineDrive + noveltyDrive + errorDrive;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->noveltyModulation = 0.0f;
    pImpl->rewardHistory.clear();
    pImpl->errorHistory.clear();
    NLM_LOG_INFO("Dopamine system reset");
}

// Acetylcholine Implementation

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    float memoryEnhancement;
    float sensoryGain;
    float workingMemoryBoost;
    
    // History tracking
    std::vector<float> attentionHistory;
    std::vector<float> stimulusHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.05f), attentionGain(1.0f), memoryEnhancement(1.0f),
             sensoryGain(1.0f), workingMemoryBoost(1.0f) {}
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
    // Acetylcholine enhances plasticity for attention-related learning
    float basePlasticity = 0.5f;
    float attentionPlasticity = pImpl->level * 0.8f;
    float memoryPlasticity = pImpl->memoryEnhancement * 0.3f;
    
    float totalFactor = basePlasticity + attentionPlasticity + memoryPlasticity;
    return std::clamp(totalFactor, 0.2f, 2.5f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized with working memory enhancement");
}

void Acetylcholine::enhanceAttention(float stimulusStrength) {
    // Enhance attention based on stimulus strength
    float attentionBoost = stimulusStrength * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionBoost);
    pImpl->attentionGain = 1.0f + attentionBoost;
    
    pImpl->attentionHistory.push_back(pImpl->attentionGain);
    if (pImpl->attentionHistory.size() > 50) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
    
    NLM_LOG_DEBUG("Acetylcholine attention enhancement: strength=" + std::to_string(stimulusStrength) + ", gain=" + std::to_string(pImpl->attentionGain));
}

void Acetylcholine::modulateAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.5f, 2.0f);
    
    // Modulate cholinergic signaling to achieve desired gain
    float targetLevel = (pImpl->attentionGain - 1.0f) * 0.4f;
    pImpl->level = std::min(pImpl->peak, std::max(pImpl->baseline, pImpl->level + targetLevel));
    
    NLM_LOG_DEBUG("Acetylcholine attention gain modulation: gain=" + std::to_string(gain) + ", level=" + std::to_string(pImpl->level));
}

void Acetylcholine::strengthenMemoryTrace(float memoryId, float strength) {
    // Enhance memory trace strength based on cholinergic input
    float memoryBoost = strength * 0.3f;
    pImpl->memoryEnhancement += memoryBoost;
    
    // Clamp enhancement factor
    pImpl->memoryEnhancement = std::min(2.0f, pImpl->memoryEnhancement);
    
    // This would integrate with working memory systems
    if (pImpl->brain) {
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        if (workingMemory) {
            // In a real implementation, this would strengthen specific memory traces
            // workingMemory->enhanceTrace(memoryId, memoryBoost);
        }
    }
    
    NLM_LOG_DEBUG("Acetylcholine memory enhancement: memoryId=" + std::to_string(static_cast<int>(memoryId)) + ", strength=" + std::to_string(memoryBoost));
}

void Acetylcholine::modulateMemoryConsolidation(bool enable) {
    pImpl->memoryEnhancement = enable ? 2.0f : 1.0f;
    NLM_LOG_DEBUG("Acetylcholine memory consolidation: " + std::string(enable ? "enabled" : "disabled"));
}

void Acetylcholine::modulateSensoryProcessing(float gain) {
    pImpl->sensoryGain = std::clamp(gain, 0.5f, 2.0f);
    
    // Enhance cholinergic signaling to achieve desired sensory gain
    float targetLevel = (pImpl->sensoryGain - 1.0f) * 0.3f;
    pImpl->level = std::min(pImpl->peak, std::max(pImpl->baseline, pImpl->level + targetLevel));
    
    NLM_LOG_DEBUG("Acetylcholine sensory processing modulation: gain=" + std::to_string(gain) + ", level=" + std::to_string(pImpl->level));
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionGain = 1.0f;
    pImpl->memoryEnhancement = 1.0f;
    pImpl->sensoryGain = 1.0f;
    pImpl->workingMemoryBoost = 1.0f;
    pImpl->attentionHistory.clear();
    pImpl->stimulusHistory.clear();
    NLM_LOG_INFO("Acetylcholine system reset");
}

// Norepinephrine Implementation

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    float attentionLevel;
    float signalToNoiseRatio;
    float locusCoeruleusGain;
    
    // History tracking
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.03f), arousalLevel(0.5f), vigilanceLevel(0.5f),
             attentionLevel(0.5f), signalToNoiseRatio(1.0f), locusCoeruleusGain(1.0f) {}
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
    // Norepinephrine modulates plasticity based on arousal and attention
    float basePlasticity = 0.5f;
    float arousalPlasticity = pImpl->arousalLevel * 0.4f;
    float vigilancePlasticity = pImpl->vigilanceLevel * 0.3f;
    float attentionPlasticity = pImpl->attentionLevel * 0.2f;
    
    float snrEffect = (pImpl->signalToNoiseRatio - 1.0f) * 0.15f;
    
    float totalFactor = basePlasticity + arousalPlasticity + vigilancePlasticity + attentionPlasticity + snrEffect;
    return std::clamp(totalFactor, 0.3f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized with arousal and vigilance");
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel + amount, 0.0f, 1.0f);
    
    // Arousal affects locus coeruleus firing
    float targetLevel = pImpl->arousalLevel * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + targetLevel);
    
    pImpl->arousalHistory.push_back(pImpl->arousalLevel);
    if (pImpl->arousalHistory.size() > 50) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
    }
    
    NLM_LOG_DEBUG("Norepinephrine arousal increase: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Norepinephrine::decreaseArousal(float amount) {
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - amount);
    
    float targetLevel = pImpl->arousalLevel * 0.6f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - (pImpl->level - targetLevel) * 0.5f);
    
    NLM_LOG_DEBUG("Norepinephrine arousal decrease: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Norepinephrine::modulateArousal(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
    
    float targetLevel = pImpl->arousalLevel * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (targetLevel - pImpl->level) * 0.3f);
    
    NLM_LOG_DEBUG("Norepinephrine arousal modulation: level=" + std::to_string(level) + ", current=" + std::to_string(pImpl->level));
}

void Norepinephrine::enhanceVigilance(float targetFocus) {
    pImpl->vigilanceLevel = std::clamp(targetFocus, 0.0f, 1.0f);
    
    // Vigilance improves signal-to-noise ratio
    pImpl->signalToNoiseRatio = 1.0f + pImpl->vigilanceLevel * 0.8f;
    
    float targetLevel = pImpl->vigilanceLevel * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + targetLevel);
    
    pImpl->vigilanceHistory.push_back(pImpl->vigilanceLevel);
    if (pImpl->vigilanceHistory.size() > 50) {
        pImpl->vigilanceHistory.erase(pImpl->vigilanceHistory.begin());
    }
    
    NLM_LOG_DEBUG("Norepinephrine vigilance enhancement: target=" + std::to_string(targetFocus) + ", SNR=" + std::to_string(pImpl->signalToNoiseRatio));
}

void Norepinephrine::reduceInattention(bool state) {
    if (state) {
        // Inattention reduces NE signaling
        pImpl->level *= 0.7f;
        pImpl->signalToNoiseRatio *= 0.8f;
        NLM_LOG_DEBUG("Norepinephrine inattention state: NE level reduced");
    }
}

void Norepinephrine::improveSignalToNoiseRatio(float improvement) {
    pImpl->signalToNoiseRatio = std::min(3.0f, pImpl->signalToNoiseRatio + improvement);
    
    // Better SNR enhances NE signaling
    float snrEffect = (pImpl->signalToNoiseRatio - 1.0f) * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + snrEffect * 0.1f);
    
    NLM_LOG_DEBUG("Norepinephrine SNR improvement: improvement=" + std::to_string(improvement) + ", SNR=" + std::to_string(pImpl->signalToNoiseRatio));
}

void Norepinephrine::suppressNoise(float reduction) {
    pImpl->signalToNoiseRatio = std::max(0.5f, pImpl->signalToNoiseRatio - reduction);
    NLM_LOG_DEBUG("Norepinephrine noise suppression: reduction=" + std::to_string(reduction) + ", SNR=" + std::to_string(pImpl->signalToNoiseRatio));
}

void Norepinephrine::increaseAlertness(float amount) {
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + amount * 0.3f);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + amount * 0.2f);
    
    float targetLevel = (pImpl->vigilanceLevel + pImpl->arousalLevel) * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + targetLevel);
    
    NLM_LOG_DEBUG("Norepinephrine alertness increase: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Norepinephrine::decreaseAlertness(float amount) {
    pImpl->vigilanceLevel = std::max(0.0f, pImpl->vigilanceLevel - amount * 0.3f);
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - amount * 0.2f);
    
    pImpl->level *= 0.8f;
    NLM_LOG_DEBUG("Norepinephrine alertness decrease: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->arousalLevel = 0.5f;
    pImpl->vigilanceLevel = 0.5f;
    pImpl->attentionLevel = 0.5f;
    pImpl->signalToNoiseRatio = 1.0f;
    pImpl->locusCoeruleusGain = 1.0f;
    pImpl->arousalHistory.clear();
    pImpl->vigilanceHistory.clear();
    NLM_LOG_INFO("Norepinephrine system reset");
}

// Serotonin Implementation

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulsivityControl;
    float socialBehavior;
    float stressLevel;
    float memoryRetrieval;
    
    // History tracking
    std::vector<float> moodHistory;
    std::vector<float> stressHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.08f), moodLevel(0.5f), impulsivityControl(0.5f),
             socialBehavior(0.5f), stressLevel(0.5f), memoryRetrieval(1.0f) {}
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
    // Serotonin modulates plasticity based on mood and stress
    float basePlasticity = 0.5f;
    float moodPlasticity = pImpl->moodLevel * 0.3f;
    float stressPlasticity = (1.0f - pImpl->stressLevel) * 0.2f;
    float socialPlasticity = pImpl->socialBehavior * 0.2f;
    float memoryPlasticity = pImpl->memoryRetrieval * 0.1f;
    
    float totalFactor = basePlasticity + moodPlasticity + stressPlasticity + socialPlasticity + memoryPlasticity;
    return std::clamp(totalFactor, 0.3f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized with mood and impulse regulation");
}

void Serotonin::elevateMood(float amount) {
    pImpl->moodLevel = std::clamp(pImpl->moodLevel + amount, 0.0f, 1.0f);
    
    // Mood affects serotonin signaling
    float targetLevel = pImpl->moodLevel * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + targetLevel);
    
    pImpl->moodHistory.push_back(pImpl->moodLevel);
    if (pImpl->moodHistory.size() > 50) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    
    NLM_LOG_DEBUG("Serotonin mood elevation: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Serotonin::reduceMood(float amount) {
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - amount);
    
    pImpl->level *= 0.8f;
    NLM_LOG_DEBUG("Serotonin mood reduction: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Serotonin::modulateMood(float level) {
    pImpl->moodLevel = std::clamp(level, 0.0f, 1.0f);
    
    float targetLevel = pImpl->moodLevel * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (targetLevel - pImpl->level) * 0.3f);
    
    NLM_LOG_DEBUG("Serotonin mood modulation: level=" + std::to_string(level) + ", current=" + std::to_string(pImpl->level));
}

void Serotonin::increaseImpulsivityControl(float amount) {
    pImpl->impulsivityControl = std::min(1.0f, pImpl->impulsivityControl + amount);
    
    // Impulse control enhances serotonin signaling
    float impulseEffect = pImpl->impulsivityControl * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + impulseEffect);
    
    NLM_LOG_DEBUG("Serotonin impulsivity control increase: amount=" + std::to_string(amount) + ", control=" + std::to_string(pImpl->impulsivityControl));
}

void Serotonin::decreaseImpulsivityControl(float amount) {
    pImpl->impulsivityControl = std::max(0.0f, pImpl->impulsivityControl - amount);
    
    pImpl->level *= 0.9f;
    NLM_LOG_DEBUG("Serotonin impulsivity control decrease: amount=" + std::to_string(amount) + ", control=" + std::to_string(pImpl->impulsivityControl));
}

void Serotonin::setImpulsivityLevel(float level) {
    pImpl->impulsivityControl = std::clamp(level, 0.0f, 1.0f);
    
    float targetLevel = pImpl->impulsivityControl * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (targetLevel - pImpl->level) * 0.3f);
    
    NLM_LOG_DEBUG("Serotonin impulsivity level: level=" + std::to_string(level) + ", control=" + std::to_string(pImpl->impulsivityControl));
}

void Serotonin::enhanceSocialBehavior(float amount) {
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + amount);
    
    // Social behavior enhances serotonin signaling
    float socialEffect = pImpl->socialBehavior * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialEffect);
    
    NLM_LOG_DEBUG("Serotonin social behavior enhancement: amount=" + std::to_string(amount) + ", behavior=" + std::to_string(pImpl->socialBehavior));
}

void Serotonin::reduceSocialBehavior(float amount) {
    pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - amount);
    
    pImpl->level *= 0.85f;
    NLM_LOG_DEBUG("Serotonin social behavior reduction: amount=" + std::to_string(amount) + ", behavior=" + std::to_string(pImpl->socialBehavior));
}

void Serotonin::modulateSocialDrive(float level) {
    pImpl->socialBehavior = std::clamp(level, 0.0f, 1.0f);
    
    float targetLevel = pImpl->socialBehavior * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (targetLevel - pImpl->level) * 0.3f);
    
    NLM_LOG_DEBUG("Serotonin social drive modulation: level=" + std::to_string(level) + ", behavior=" + std::to_string(pImpl->socialBehavior));
}

void Serotonin::reduceStress(float amount) {
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - amount);
    
    // Stress reduction enhances serotonin signaling
    float stressRelief = (1.0f - pImpl->stressLevel) * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressRelief);
    
    pImpl->stressHistory.push_back(pImpl->stressLevel);
    if (pImpl->stressHistory.size() > 50) {
        pImpl->stressHistory.erase(pImpl->stressHistory.begin());
    }
    
    NLM_LOG_DEBUG("Serotonin stress reduction: amount=" + std::to_string(amount) + ", stress=" + std::to_string(pImpl->stressLevel));
}

void Serotonin::increaseStress(float amount) {
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + amount);
    
    pImpl->level *= 0.8f;
    NLM_LOG_DEBUG("Serotonin stress increase: amount=" + std::to_string(amount) + ", stress=" + std::to_string(pImpl->stressLevel));
}

void Serotonin::setStressLevel(float level) {
    pImpl->stressLevel = std::clamp(level, 0.0f, 1.0f);
    
    float stressEffect = (1.0f - pImpl->stressLevel) * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (stressEffect - pImpl->level) * 0.2f);
    
    NLM_LOG_DEBUG("Serotonin stress level: level=" + std::to_string(level) + ", stress=" + std::to_string(pImpl->stressLevel));
}

void Serotonin::enhanceMemoryRetrieval(float amount) {
    pImpl->memoryRetrieval = std::min(2.0f, pImpl->memoryRetrieval + amount);
    
    // Enhanced memory retrieval improves serotonin signaling
    float memoryEffect = (pImpl->memoryRetrieval - 1.0f) * 0.2f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryEffect);
    
    NLM_LOG_DEBUG("Serotonin memory retrieval enhancement: amount=" + std::to_string(amount) + ", retrieval=" + std::to_string(pImpl->memoryRetrieval));
}

void Serotonin::modulateMemoryEncoding(bool enhance) {
    pImpl->memoryRetrieval = enhance ? 2.0f : 1.0f;
    NLM_LOG_DEBUG("Serotonin memory encoding: " + std::string(enhance ? "enhanced" : "standard"));
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityControl = 0.5f;
    pImpl->socialBehavior = 0.5f;
    pImpl->stressLevel = 0.5f;
    pImpl->memoryRetrieval = 1.0f;
    pImpl->moodHistory.clear();
    pImpl->stressHistory.clear();
    NLM_LOG_INFO("Serotonin system reset");
}

} // namespace nlm