#include "Neuromodulator.hpp"
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

// Oxytocin Implementation

struct Oxytocin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float socialBonding;
    float trustLevel;
    float socialAnxiety;
    float socialRecognition;
    float socialReward;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.06f), socialBonding(0.5f), trustLevel(0.5f),
             socialAnxiety(0.5f), socialRecognition(0.5f), socialReward(1.0f) {}
};

Oxytocin::Oxytocin() : pImpl(new Impl) {}
Oxytocin::~Oxytocin() = default;

const char* Oxytocin::getName() const {
    return "OT";
}

float Oxytocin::getLevel() const {
    return pImpl->level;
}

void Oxytocin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Oxytocin::getPlasticityFactor() const {
    // Oxytocin modulates plasticity based on social bonding and trust
    float basePlasticity = 0.5f;
    float bondingPlasticity = pImpl->socialBonding * 0.4f;
    float trustPlasticity = pImpl->trustLevel * 0.3f;
    float socialPlasticity = pImpl->socialRecognition * 0.2f;
    float rewardPlasticity = pImpl->socialReward * 0.1f;
    
    float totalFactor = basePlasticity + bondingPlasticity + trustPlasticity + socialPlasticity + rewardPlasticity;
    return std::clamp(totalFactor, 0.4f, 2.5f);
}

void Oxytocin::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Oxytocin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Oxytocin system initialized with social bonding and trust");
}

void Oxytocin::enhanceSocialBonding(float amount) {
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + amount);
    
    // Social bonding enhances oxytocin signaling
    float bondingEffect = pImpl->socialBonding * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingEffect);
    
    NLM_LOG_DEBUG("Oxytocin social bonding enhancement: amount=" + std::to_string(amount) + ", bonding=" + std::to_string(pImpl->socialBonding));
}

void Oxytocin::promoteTrust(float amount) {
    pImpl->trustLevel = std::min(1.0f, pImpl->trustLevel + amount);
    
    // Trust enhances oxytocin signaling
    float trustEffect = pImpl->trustLevel * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + trustEffect);
    
    NLM_LOG_DEBUG("Oxytocin trust promotion: amount=" + std::to_string(amount) + ", trust=" + std::to_string(pImpl->trustLevel));
}

void Oxytocin::reduceSocialAnxiety(float amount) {
    pImpl->socialAnxiety = std::max(0.0f, pImpl->socialAnxiety - amount);
    
    // Reduced anxiety enhances oxytocin signaling
    float anxietyRelief = (1.0f - pImpl->socialAnxiety) * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + anxietyRelief);
    
    NLM_LOG_DEBUG("Oxytocin social anxiety reduction: amount=" + std::to_string(amount) + ", anxiety=" + std::to_string(pImpl->socialAnxiety));
}

void Oxytocin::enhanceSocialRecognition(float amount) {
    pImpl->socialRecognition = std::min(1.0f, pImpl->socialRecognition + amount);
    
    // Social recognition enhances oxytocin signaling
    float recognitionEffect = pImpl->socialRecognition * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + recognitionEffect);
    
    NLM_LOG_DEBUG("Oxytocin social recognition enhancement: amount=" + std::to_string(amount) + ", recognition=" + std::to_string(pImpl->socialRecognition));
}

void Oxytocin::promoteAttachment(float amount) {
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + amount * 0.7f);
    pImpl->trustLevel = std::min(1.0f, pImpl->trustLevel + amount * 0.5f);
    
    float attachmentEffect = amount * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attachmentEffect);
    
    NLM_LOG_DEBUG("Oxytocin attachment promotion: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Oxytocin::reduceAggression(float amount) {
    // Oxytocin reduces aggression directly
    pImpl->level *= (1.0f - amount * 0.3f);
    
    // Also affects social bonding
    pImpl->socialBonding = std::max(0.0f, pImpl->socialBonding - amount * 0.2f);
    
    NLM_LOG_DEBUG("Oxytocin aggression reduction: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Oxytocin::increaseProsocialBehavior(float amount) {
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + amount * 0.6f);
    pImpl->trustLevel = std::min(1.0f, pImpl->trustLevel + amount * 0.4f);
    
    float prosocialEffect = amount * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + prosocialEffect);
    
    NLM_LOG_DEBUG("Oxytocin prosocial behavior increase: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Oxytocin::modulateEmpathy(float level) {
    pImpl->socialRecognition = std::clamp(level, 0.0f, 1.0f);
    
    // Empathy affects oxytocin signaling
    float empathyEffect = pImpl->socialRecognition * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + empathyEffect);
    
    NLM_LOG_DEBUG("Oxytocin empathy modulation: level=" + std::to_string(level) + ", recognition=" + std::to_string(pImpl->socialRecognition));
}

void Oxytocin::reduceStressResponse(float amount) {
    // Oxytocin reduces stress response
    pImpl->level *= (1.0f - amount * 0.2f);
    pImpl->socialAnxiety = std::max(0.0f, pImpl->socialAnxiety - amount * 0.3f);
    
    NLM_LOG_DEBUG("Oxytocin stress response reduction: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Oxytocin::promoteCalmState(float amount) {
    pImpl->socialAnxiety = std::max(0.0f, pImpl->socialAnxiety - amount * 0.8f);
    
    float calmEffect = (1.0f - pImpl->socialAnxiety) * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + calmEffect);
    
    NLM_LOG_DEBUG("Oxytocin calm state promotion: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Oxytocin::enhanceSocialReward(float amount) {
    pImpl->socialReward = std::min(2.0f, pImpl->socialReward + amount);
    
    // Social reward enhances oxytocin signaling
    float rewardEffect = (pImpl->socialReward - 1.0f) * 0.2f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardEffect);
    
    NLM_LOG_DEBUG("Oxytocin social reward enhancement: amount=" + std::to_string(amount) + ", reward=" + std::to_string(pImpl->socialReward));
}

void Oxytocin::promoteSocialSharing(float amount) {
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + amount * 0.5f);
    pImpl->trustLevel = std::min(1.0f, pImpl->trustLevel + amount * 0.3f);
    
    float sharingEffect = amount * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + sharingEffect);
    
    NLM_LOG_DEBUG("Oxytocin social sharing promotion: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Oxytocin::reset() {
    pImpl->level = 0.0f;
    pImpl->socialBonding = 0.5f;
    pImpl->trustLevel = 0.5f;
    pImpl->socialAnxiety = 0.5f;
    pImpl->socialRecognition = 0.5f;
    pImpl->socialReward = 1.0f;
    NLM_LOG_INFO("Oxytocin system reset");
}

// Vasopressin Implementation

struct Vasopressin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float socialRecognition;
    float pairBonding;
    float mateGuarding;
    float aggression;
    float socialHierarchy;
    float stressResponse;
    
    // History tracking
    std::vector<float> hierarchyHistory;
    std::vector<float> aggressionHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.07f), socialRecognition(0.5f), pairBonding(0.5f),
             mateGuarding(0.5f), aggression(0.5f), socialHierarchy(0.5f),
             stressResponse(0.5f) {}
};

Vasopressin::Vasopressin() : pImpl(new Impl) {}
Vasopressin::~Vasopressin() = default;

const char* Vasopressin::getName() const {
    return "AVP";
}

float Vasopressin::getLevel() const {
    return pImpl->level;
}

void Vasopressin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Vasopressin::getPlasticityFactor() const {
    // Vasopressin modulates plasticity based on social recognition and hierarchy
    float basePlasticity = 0.5f;
    float recognitionPlasticity = pImpl->socialRecognition * 0.3f;
    float bondingPlasticity = pImpl->pairBonding * 0.25f;
    float guardingPlasticity = pImpl->mateGuarding * 0.2f;
    float aggressionPlasticity = pImpl->aggression * 0.15f;
    float hierarchyPlasticity = pImpl->socialHierarchy * 0.1f;
    
    float stressPlasticity = (1.0f - pImpl->stressResponse) * 0.1f;
    
    float totalFactor = basePlasticity + recognitionPlasticity + bondingPlasticity + guardingPlasticity +
                       aggressionPlasticity + hierarchyPlasticity + stressPlasticity;
    return std::clamp(totalFactor, 0.4f, 3.0f);
}

void Vasopressin::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Vasopressin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Vasopressin system initialized with social recognition and pair bonding");
}

void Vasopressin::enhanceSocialRecognition(float amount) {
    pImpl->socialRecognition = std::min(1.0f, pImpl->socialRecognition + amount);
    
    // Social recognition enhances vasopressin signaling
    float recognitionEffect = pImpl->socialRecognition * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + recognitionEffect);
    
    NLM_LOG_DEBUG("Vasopressin social recognition enhancement: amount=" + std::to_string(amount) + ", recognition=" + std::to_string(pImpl->socialRecognition));
}

void Vasopressin::promotePairBonding(float amount) {
    pImpl->pairBonding = std::min(1.0f, pImpl->pairBonding + amount);
    
    // Pair bonding enhances vasopressin signaling
    float bondingEffect = pImpl->pairBonding * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingEffect);
    
    NLM_LOG_DEBUG("Vasopressin pair bonding promotion: amount=" + std::to_string(amount) + ", bonding=" + std::to_string(pImpl->pairBonding));
}

void Vasopressin::enhanceMateGuarding(float amount) {
    pImpl->mateGuarding = std::min(1.0f, pImpl->mateGuarding + amount);
    
    // Mate guarding enhances vasopressin signaling
    float guardingEffect = pImpl->mateGuarding * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + guardingEffect);
    
    NLM_LOG_DEBUG("Vasopressin mate guarding enhancement: amount=" + std::to_string(amount) + ", guarding=" + std::to_string(pImpl->mateGuarding));
}

void Vasopressin::modulateTerritorialBehavior(float level) {
    pImpl->aggression = std::clamp(level, 0.0f, 1.0f);
    
    // Territorial behavior affects vasopressin signaling
    float territorialEffect = pImpl->aggression * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + territorialEffect);
    
    NLM_LOG_DEBUG("Vasopressin territorial behavior modulation: level=" + std::to_string(level) + ", aggression=" + std::to_string(pImpl->aggression));
}

void Vasopressin::enhanceResourceDefense(float amount) {
    pImpl->aggression = std::min(1.0f, pImpl->aggression + amount * 0.7f);
    
    float defenseEffect = amount * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + defenseEffect);
    
    NLM_LOG_DEBUG("Vasopressin resource defense enhancement: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Vasopressin::increaseAggression(float amount) {
    pImpl->aggression = std::min(1.0f, pImpl->aggression + amount);
    
    // Aggression enhances vasopressin signaling
    float aggressionEffect = pImpl->aggression * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + aggressionEffect);
    
    pImpl->aggressionHistory.push_back(pImpl->aggression);
    if (pImpl->aggressionHistory.size() > 50) {
        pImpl->aggressionHistory.erase(pImpl->aggressionHistory.begin());
    }
    
    NLM_LOG_DEBUG("Vasopressin aggression increase: amount=" + std::to_string(amount) + ", aggression=" + std::to_string(pImpl->aggression));
}

void Vasopressin::decreaseAggression(float amount) {
    pImpl->aggression = std::max(0.0f, pImpl->aggression - amount);
    
    pImpl->level *= 0.8f;
    NLM_LOG_DEBUG("Vasopressin aggression decrease: amount=" + std::to_string(amount) + ", aggression=" + std::to_string(pImpl->aggression));
}

void Vasopressin::modulateAggressionLevel(float level) {
    pImpl->aggression = std::clamp(level, 0.0f, 1.0f);
    
    float targetLevel = pImpl->aggression * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (targetLevel - pImpl->level) * 0.3f);
    
    NLM_LOG_DEBUG("Vasopressin aggression level: level=" + std::to_string(level) + ", aggression=" + std::to_string(pImpl->aggression));
}

void Vasopressin::enhanceDominanceBehavior(float amount) {
    pImpl->socialHierarchy = std::min(1.0f, pImpl->socialHierarchy + amount);
    
    // Dominance behavior enhances vasopressin signaling
    float dominanceEffect = pImpl->socialHierarchy * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + dominanceEffect);
    
    pImpl->hierarchyHistory.push_back(pImpl->socialHierarchy);
    if (pImpl->hierarchyHistory.size() > 50) {
        pImpl->hierarchyHistory.erase(pImpl->hierarchyHistory.begin());
    }
    
    NLM_LOG_DEBUG("Vasopressin dominance behavior enhancement: amount=" + std::to_string(amount) + ", hierarchy=" + std::to_string(pImpl->socialHierarchy));
}

void Vasopressin::modulateSocialRanking(float level) {
    pImpl->socialHierarchy = std::clamp(level, 0.0f, 1.0f);
    
    float targetLevel = pImpl->socialHierarchy * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + (targetLevel - pImpl->level) * 0.3f);
    
    NLM_LOG_DEBUG("Vasopressin social ranking: level=" + std::to_string(level) + ", hierarchy=" + std::to_string(pImpl->socialHierarchy));
}

void Vasopressin::modulateStressResponse(float level) {
    pImpl->stressResponse = std::clamp(level, 0.0f, 1.0f);
    
    // Stress response affects vasopressin signaling
    float stressEffect = pImpl->stressResponse * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressEffect);
    
    NLM_LOG_DEBUG("Vasopressin stress response modulation: level=" + std::to_string(level) + ", stress=" + std::to_string(pImpl->stressResponse));
}

void Vasopressin::promoteStability(float amount) {
    pImpl->socialHierarchy = std::min(1.0f, pImpl->socialHierarchy + amount * 0.5f);
    pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - amount * 0.3f);
    
    float stabilityEffect = amount * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stabilityEffect);
    
    NLM_LOG_DEBUG("Vasopressin stability promotion: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Vasopressin::enhanceSocialMemory(float amount) {
    pImpl->socialRecognition = std::min(1.0f, pImpl->socialRecognition + amount * 0.6f);
    pImpl->pairBonding = std::min(1.0f, pImpl->pairBonding + amount * 0.4f);
    
    float memoryEffect = amount * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryEffect);
    
    NLM_LOG_DEBUG("Vasopressin social memory enhancement: amount=" + std::to_string(amount) + ", level=" + std::to_string(pImpl->level));
}

void Vasopressin::modulatePairBondMemory(bool enhance) {
    pImpl->pairBonding = enhance ? 1.0f : 0.0f;
    
    float bondEffect = enhance ? 0.6f : -0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondEffect);
    
    NLM_LOG_DEBUG("Vasopressin pair bond memory: " + std::string(enhance ? "enhanced" : "standard"));
}

void Vasopressin::reset() {
    pImpl->level = 0.0f;
    pImpl->socialRecognition = 0.5f;
    pImpl->pairBonding = 0.5f;
    pImpl->mateGuarding = 0.5f;
    pImpl->aggression = 0.5f;
    pImpl->socialHierarchy = 0.5f;
    pImpl->stressResponse = 0.5f;
    pImpl->hierarchyHistory.clear();
    pImpl->aggressionHistory.clear();
    NLM_LOG_INFO("Vasopressin system reset");
}

// Novelty Implementation

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float noveltyDetectionThreshold;
    float changeSensitivity;
    float memoryComparison;
    
    // History tracking
    std::vector<float> noveltyHistory;
    std::vector<float> changeHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.04f), noveltyDetectionThreshold(0.3f),
             changeSensitivity(1.0f), memoryComparison(0.5f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}
Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "NOVELTY";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty modulates plasticity based on change detection
    float basePlasticity = 0.5f;
    float noveltyPlasticity = pImpl->level * 0.7f;
    float changePlasticity = pImpl->changeSensitivity * 0.2f;
    float memoryPlasticity = pImpl->memoryComparison * 0.1f;
    
    float totalFactor = basePlasticity + noveltyPlasticity + changePlasticity + memoryPlasticity;
    return std::clamp(totalFactor, 0.3f, 2.5f);
}

void Novelty::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty system initialized with change detection");
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    // Detect novelty by comparing current and previous patterns
    if (currentPattern.empty() || previousPattern.empty()) {
        return;
    }
    
    float totalChange = 0.0f;
    size_t minSize = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float change = std::abs(currentPattern[i] - previousPattern[i]);
        totalChange += change;
    }
    
    float averageChange = totalChange / static_cast<float>(minSize);
    
    // Normalize to 0-1 range
    float noveltyLevel = std::min(1.0f, averageChange * pImpl->changeSensitivity);
    
    // Check against detection threshold
    if (noveltyLevel > pImpl->noveltyDetectionThreshold) {
        pImpl->level = noveltyLevel;
        
        pImpl->noveltyHistory.push_back(noveltyLevel);
        pImpl->changeHistory.push_back(averageChange);
        
        if (pImpl->noveltyHistory.size() > 100) {
            pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
            pImpl->changeHistory.erase(pImpl->changeHistory.begin());
        }
        
        NLM_LOG_DEBUG("Novelty detected: change=" + std::to_string(averageChange) + ", level=" + std::to_string(noveltyLevel));
    }
}

void Novelty::setNoveltyLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Novelty level set: level=" + std::to_string(level));
}

void Novelty::decayOverTime(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Novelty::clearHistory() {
    pImpl->noveltyHistory.clear();
    pImpl->changeHistory.clear();
    NLM_LOG_DEBUG("Novelty history cleared");
}

// Curiosity Implementation

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float noveltyWeight;
    float predictionErrorWeight;
    float explorationDrive;
    float behavioralOutput;
    
    // History tracking
    std::vector<float> curiosityHistory;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.05f), noveltyWeight(0.5f), predictionErrorWeight(0.5f),
             explorationDrive(0.5f), behavioralOutput(0.5f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}
Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "CURIOSITY";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity modulates plasticity based on exploration drive
    float basePlasticity = 0.5f;
    float curiosityPlasticity = pImpl->level * 0.6f;
    float explorationPlasticity = pImpl->explorationDrive * 0.3f;
    float behavioralPlasticity = pImpl->behavioralOutput * 0.1f;
    
    float totalFactor = basePlasticity + curiosityPlasticity + explorationPlasticity + behavioralPlasticity;
    return std::clamp(totalFactor, 0.4f, 2.5f);
}

void Curiosity::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with exploration motivation");
}

void Curiosity::updateCuriosity(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Calculate exploration drive
    pImpl->explorationDrive = target * 0.8f + 0.1f;
    
    // Behavioral output based on curiosity
    pImpl->behavioralOutput = std::min(1.0f, pImpl->level * 1.2f);
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->curiosityHistory.push_back(pImpl->level);
    if (pImpl->curiosityHistory.size() > 100) {
        pImpl->curiosityHistory.erase(pImpl->curiosityHistory.begin());
    }
    
    NLM_LOG_DEBUG("Curiosity updated: novelty=" + std::to_string(novelty) + ", error=" + std::to_string(predictionError) + ", level=" + std::to_string(pImpl->level));
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Curiosity novelty weight: weight=" + std::to_string(weight));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Curiosity prediction error weight: weight=" + std::to_string(weight));
}

void Curiosity::driveExploration(float amount) {
    pImpl->explorationDrive = std::min(1.0f, pImpl->explorationDrive + amount);
    
    // Exploration affects curiosity level
    float explorationBoost = amount * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + explorationBoost);
    
    NLM_LOG_DEBUG("Curiosity exploration drive: amount=" + std::to_string(amount) + ", drive=" + std::to_string(pImpl->explorationDrive));
}

void Curiosity::reduceExploration(float amount) {
    pImpl->explorationDrive = std::max(0.0f, pImpl->explorationDrive - amount);
    
    pImpl->level *= 0.9f;
    NLM_LOG_DEBUG("Curiosity exploration reduction: amount=" + std::to_string(amount) + ", drive=" + std::to_string(pImpl->explorationDrive));
}

// Prediction Error Implementation

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    // Integrated modulation signals
    float dopamineModulation;
    float serotoninModulation;
    float noveltyModulation;
    float curiosityModulation;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             dopamineModulation(0.0f), serotoninModulation(0.0f), 
             noveltyModulation(0.0f), curiosityModulation(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}
PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PREDICTION_ERROR";
}

float PredictionError::getLevel() const {
    return std::abs(pImpl->error);
}

void PredictionError::setLevel(float level) {
    pImpl->error = level;
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error modulates plasticity based on error magnitude
    float basePlasticity = 0.5f;
    float errorPlasticity = std::min(1.0f, std::abs(pImpl->error)) * 0.8f;
    float integratedPlasticity = pImpl->dopamineModulation * 0.3f + 
                                 pImpl->serotoninModulation * 0.2f +
                                 pImpl->noveltyModulation * 0.2f +
                                 pImpl->curiosityModulation * 0.1f;
    
    float totalFactor = basePlasticity + errorPlasticity + integratedPlasticity;
    return std::clamp(totalFactor, 0.5f, 3.0f);
}

void PredictionError::update(TimestepDuration dt) {
    // Smooth error decay over time
    pImpl->error *= 0.95f;
    
    // Update integrated modulations
    pImpl->dopamineModulation = std::min(1.0f, std::abs(pImpl->error) * 0.6f);
    pImpl->serotoninModulation = std::min(1.0f, std::abs(pImpl->error) * 0.4f);
    pImpl->noveltyModulation = std::min(1.0f, std::abs(pImpl->error) * 0.5f);
    pImpl->curiosityModulation = std::min(1.0f, std::abs(pImpl->error) * 0.3f);
}

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized with neuromodulator integration");
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
    
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    NLM_LOG_DEBUG("PredictionError computed: predicted=" + std::to_string(predicted) + ", actual=" + std::to_string(actual) + ", error=" + std::to_string(pImpl->error));
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

void PredictionError::applyToDopamine(Dopamine* dopamine) {
    if (dopamine) {
        dopamine->setPredictionError(pImpl->error);
        dopamine->modulateByNovelty(pImpl->noveltyModulation);
    }
}

void PredictionError::applyToSerotonin(Serotonin* serotonin) {
    if (serotonin) {
        // Serotonin can be modulated by prediction error for mood regulation
        float errorEffect = std::min(1.0f, std::abs(pImpl->error) * 0.4f);
        serotonin->setLevel(serotonin->getLevel() + errorEffect * 0.2f);
    }
}

void PredictionError::applyToNovelty(Novelty* novelty) {
    if (novelty) {
        // Novelty can be enhanced by prediction error
        float errorEffect = std::min(1.0f, std::abs(pImpl->error) * 0.5f);
        novelty->setLevel(novelty->getLevel() + errorEffect * 0.3f);
    }
}

float PredictionError::getError() const {
    return pImpl->error;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm