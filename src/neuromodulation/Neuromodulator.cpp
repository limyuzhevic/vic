#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

struct Neuromodulator::Impl {
    float currentLevel;
    float targetLevel;
    float modulationStrength;
    float timeConstant;
    float decayRate;
    float adaptationTimeConstant;
    float currentTime;
    float previousLevel;
    bool isActive;
    
    Impl() : currentLevel(0.0f), targetLevel(0.0f), modulationStrength(1.0f),
             timeConstant(1.0f), decayRate(0.01f), adaptationTimeConstant(1.0f),
             currentTime(0.0f), previousLevel(0.0f), isActive(false) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl()) {}

void Neuromodulator::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, -1.0f, 1.0f);
    pImpl->targetLevel = pImpl->currentLevel;
    pImpl->previousLevel = pImpl->currentLevel;
    pImpl->isActive = std::abs(pImpl->currentLevel) > 0.001f;
}

struct Dopamine::Impl {
    float tonicLevel;
    float phasicLevel;
    float predictionError;
    float expectedReward;
    float modulationStrength;
    float integrationRate;
    float adaptationTimeConstant;
    float phi;
    
    // Reward history
    std::vector<float> rewardHistory;
    size_t historySize;
    
    // Neural components
    float releaseProbability;
    float reuptakeRate;
    float receptorDensity;
    
    // Random components
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : tonicLevel(0.2f), phasicLevel(0.0f), predictionError(0.0f),
             expectedReward(0.0f), modulationStrength(1.0f), integrationRate(0.05f),
             adaptationTimeConstant(2.0f), phi(0.5f), historySize(100),
             releaseProbability(0.1f), reuptakeRate(0.05f), receptorDensity(1.0f) {
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
    }
};

Dopamine::Dopamine() : pImpl(new Impl()) {}
Dopamine::~Dopamine() = default;

float Dopamine::getLevel() const {
    // Calculate level as weighted sum of tonic and phasic components
    float phasicComponent = pImpl->phasicLevel * pImpl->phi;
    float tonicComponent = pImpl->tonicLevel * (1.0f - pImpl->phi);
    float level = tonicComponent + phasicComponent;
    
    // Apply modulation strength
    level *= pImpl->modulationStrength;
    
    return std::clamp(level, -1.0f, 1.0f);
}

void Dopamine::setLevel(float level) {
    Neuromodulator::setLevel(level);
    if (pImpl) {
        pImpl->targetLevel = level;
        pImpl->phasicLevel = level - pImpl->tonicLevel;
    }
}

float Dopamine::getPlasticityFactor() const {
    // Calculate plasticity factor based on dopamine level
    float level = getLevel();
    
    // Positive dopamine enhances LTP, negative enhances LTD
    float baseFactor = 1.0f;
    float modulation = std::abs(level);
    float sign = level >= 0.0f ? 1.0f : -1.0f;
    
    // Dopamine modulates plasticity strength
    float plasticityFactor = baseFactor + sign * modulation * 0.5f;
    
    // Clamp to reasonable range
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    if (!pImpl) return;
    
    // Update prediction error
    float currentPredictionError = getLevel() - pImpl->expectedReward;
    pImpl->predictionError = currentPredictionError;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward = pImpl->expectedReward * 0.95f + getLevel() * 0.05f;
    
    // Calculate phasic component
    pImpl->phasicLevel = currentPredictionError * pImpl->integrationRate;
    
    // Apply decay to phasic component
    pImpl->phasicLevel *= (1.0f - pImpl->decayRate * dt);
    
    // Gradually return to tonic level
    pImpl->tonicLevel = std::lerp(pImpl->tonicLevel, pImpl->targetLevel, pImpl->adaptationTimeConstant * dt);
    
    // Add to history
    pImpl->rewardHistory.push_back(getLevel());
    if (pImpl->rewardHistory.size() > pImpl->historySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

void Dopamine::signalReward(float reward) {
    if (!pImpl) return;
    
    // Add reward to history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > pImpl->historySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Increase phasic component based on unexpected reward
    pImpl->phasicLevel += reward * (1.0f + std::abs(pImpl->predictionError)) * 0.5f;
}

void Dopamine::signalRewardPredictionError(float error) {
    if (!pImpl) return;
    
    pImpl->predictionError = error;
    
    // Phasic response based on prediction error
    pImpl->phasicLevel = error * pImpl->integrationRate * (1.0f + std::abs(error));
}

struct Acetylcholine::Impl {
    float attentionLevel;
    float noveltyLevel;
    float memoryDemand;
    float learningModulation;
    float memoryModulation;
    
    // Neural correlates
    float frontalLobeActivity;
    float prefrontalActivation;
    float workingMemoryLoad;
    
    // Biological parameters
    float synthesisRate;
    float reuptakeRate;
    float receptorAffinity;
    float tonicBaseline;
    
    // State tracking
    float currentTime;
    float targetLevel;
    float adaptiveCapacity;
    
    Impl() : attentionLevel(0.0f), noveltyLevel(0.0f), memoryDemand(0.0f),
             learningModulation(1.0f), memoryModulation(1.0f),
             frontalLobeActivity(0.0f), prefrontalActivation(0.0f),
             workingMemoryLoad(0.0f), synthesisRate(0.1f), reuptakeRate(0.05f),
             receptorAffinity(1.0f), tonicBaseline(0.1f),
             currentTime(0.0f), targetLevel(0.0f), adaptiveCapacity(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl()) {}
Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    if (!pImpl) return 0.0f;
    
    // Calculate level based on attentional state
    float level = pImpl->attentionLevel * 0.4f +
                  pImpl->noveltyLevel * 0.3f +
                  pImpl->memoryDemand * 0.3f;
    
    // Apply modulation strength
    level *= pImpl->modulationStrength;
    
    // Add tonic baseline
    level += pImpl->tonicBaseline;
    
    return std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::setLevel(float level) {
    Neuromodulator::setLevel(level);
    if (pImpl) {
        pImpl->targetLevel = level;
        pImpl->attentionLevel = level * 0.7f;
        pImpl->noveltyLevel = (level - 0.5f) * 0.5f;  // Positive for novelty
    }
}

float Acetylcholine::getPlasticityFactor() const {
    if (!pImpl) return 1.0f;
    
    // ACh enhances working memory and attention
    float attentionEffect = pImpl->attentionLevel * 0.5f;
    float memoryEffect = pImpl->memoryDemand * 0.3f;
    
    // Learning modulation
    float learningMod = pImpl->learningModulation * (1.0f + attentionEffect);
    
    // Memory consolidation modulation
    float memoryMod = pImpl->memoryModulation * (1.0f + memoryEffect);
    
    // Combined effect
    float plasticityFactor = learningMod * memoryMod;
    
    return std::clamp(plasticityFactor, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    if (!pImpl) return;
    
    // Update attentional state based on external inputs
    pImpl->attentionLevel = std::lerp(pImpl->attentionLevel, pImpl->targetLevel, 0.1f * dt);
    pImpl->noveltyLevel = std::lerp(pImpl->noveltyLevel, 0.0f, pImpl->reuptakeRate * dt);
    pImpl->memoryDemand = std::lerp(pImpl->memoryDemand, 0.0f, pImpl->reuptakeRate * dt);
    
    // Update neural correlates
    pImpl->frontalLobeActivity = std::lerp(pImpl->frontalLobeActivity, pImpl->attentionLevel * 2.0f, 0.2f * dt);
    pImpl->prefrontalActivation = std::lerp(pImpl->prefrontalActivation, pImpl->attentionLevel * 1.5f, 0.2f * dt);
    pImpl->workingMemoryLoad = std::lerp(pImpl->workingMemoryLoad, pImpl->memoryDemand, 0.3f * dt);
    
    pImpl->currentTime += dt;
}

void Acetylcholine::signalNovelty(float novelty) {
    if (!pImpl) return;
    pImpl->noveltyLevel = std::max(pImpl->noveltyLevel, novelty);
}

void Acetylcholine::signalAttention(float attentionSignal) {
    if (!pImpl) return;
    pImpl->attentionLevel = std::max(pImpl->attentionLevel, attentionSignal);
    pImpl->targetLevel = attentionSignal;
}

void Acetylcholine::signalMemoryDemand(float demand) {
    if (!pImpl) return;
    pImpl->memoryDemand = std::max(pImpl->memoryDemand, demand);
}

struct Norepinephrine::Impl {
    float arousalLevel;
    float stressLevel;
    float cognitiveModulation;
    float sensoryGating;
    float workingMemoryUpdate;
    
    // Brain region activities
    float locusCoeruleusActivity;
    float pontineActivity;
    float thalamicActivity;
    
    // Neurochemical dynamics
    float baselineArousal;
    float responseGain;
    float saturationPoint;
    float tonicRelease;
    float phasicRelease;
    
    // State tracking
    float currentTime;
    float targetLevel;
    float adaptationRate;
    float recoveryRate;
    
    Impl() : arousalLevel(0.0f), stressLevel(0.0f), cognitiveModulation(1.0f),
             sensoryGating(1.0f), workingMemoryUpdate(1.0f),
             locusCoeruleusActivity(0.0f), pontineActivity(0.0f),
             thalamicActivity(0.0f), baselineArousal(0.1f),
             responseGain(1.0f), saturationPoint(2.0f),
             tonicRelease(0.1f), phasicRelease(0.5f),
             currentTime(0.0f), targetLevel(0.0f),
             adaptationRate(0.2f), recoveryRate(0.05f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl()) {}
Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    if (!pImpl) return 0.0f;
    
    // Calculate level based on arousal state
    float level = pImpl->arousalLevel * 0.5f +
                  pImpl->stressLevel * 0.3f;
    
    // Apply modulation strength
    level *= pImpl->modulationStrength;
    
    // Clamp to reasonable range
    return std::clamp(level, 0.0f, 2.0f);  // NE can be > 1.0
}

void Norepinephrine::setLevel(float level) {
    Neuromodulator::setLevel(level);
    if (pImpl) {
        pImpl->targetLevel = level;
        pImpl->stressLevel = level * 0.6f;
    }
}

float Norepinephrine::getPlasticityFactor() const {
    if (!pImpl) return 1.0f;
    
    // NE has complex effects on plasticity
    float arousalEffect = pImpl->arousalLevel * 0.3f;
    float stressEffect = pImpl->stressLevel * 0.4f;
    
    // High arousal improves LTP, moderate stress enhances LTD
    float plasticityFactor = 1.0f + arousalEffect - stressEffect * 0.2f;
    
    // Apply cognitive modulation
    plasticityFactor *= pImpl->cognitiveModulation;
    
    return std::clamp(plasticityFactor, 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    if (!pImpl) return;
    
    // Update arousal based on stress and baseline
    float stressInput = pImpl->stressLevel * pImpl->responseGain;
    pImpl->arousalLevel = std::lerp(pImpl->arousalLevel, stressInput, pImpl->adaptationRate * dt);
    
    // Recovery from stress
    pImpl->stressLevel = std::lerp(pImpl->stressLevel, 0.0f, pImpl->recoveryRate * dt);
    
    // Neural correlates
    pImpl->locusCoeruleusActivity = std::lerp(pImpl->locusCoeruleusActivity, pImpl->arousalLevel, 0.5f * dt);
    pImpl->pontineActivity = std::lerp(pImpl->pontineActivity, pImpl->arousalLevel * 0.8f, 0.3f * dt);
    pImpl->thalamicActivity = std::lerp(pImpl->thalamicActivity, pImpl->arousalLevel * 0.6f, 0.4f * dt);
    
    // Gating mechanisms
    pImpl->sensoryGating = 1.0f / (1.0f + std::exp(-5.0f * (pImpl->arousalLevel - 0.5f)));
    pImpl->workingMemoryUpdate = 1.0f + pImpl->arousalLevel * 0.5f;
    
    pImpl->currentTime += dt;
}

void Norepinephrine::signalStress(float stress) {
    if (!pImpl) return;
    pImpl->stressLevel = std::max(pImpl->stressLevel, stress);
    pImpl->targetLevel = stress;
}

void Norepinephrine::signalExcitement(float excitement) {
    if (!pImpl) return;
    pImpl->arousalLevel = std::max(pImpl->arousalLevel, excitement);
}

void Norepinephrine::signalAlertness(float alertness) {
    if (!pImpl) return;
    pImpl->arousalLevel = alertness;
    pImpl->cognitiveModulation = alertness;
}

struct Serotonin::Impl {
    float moodLevel;
    float impulsivityLevel;
    float anhedoniaLevel;
    float learningRateMod;
    float extinctionMod;
    
    // Raphe nucleus activity
    float dorsalRapheActivity;
    float medianRapheActivity;
    float ponsCoeruleusActivity;
    
    // Neurochemical dynamics
    float tonicRelease;
    float phasicRelease;
    float synthesisRate;
    float reuptakeRate;
    float receptorSensitivity;
    
    // Behavioral correlates
    float socialCohesion;
    float dominanceLevel;
    float anxietyLevel;
    float rewardSensitivity;
    
    // State tracking
    float currentTime;
    float targetLevel;
    float adaptationTimeConstant;
    float feedbackDelay;
    float homeostaticRegulation;
    
    Impl() : moodLevel(0.5f), impulsivityLevel(0.5f), anhedoniaLevel(0.0f),
             learningRateMod(1.0f), extinctionMod(1.0f),
             dorsalRapheActivity(0.0f), medianRapheActivity(0.0f),
             ponsCoeruleusActivity(0.0f), tonicRelease(0.2f),
             phasicRelease(0.3f), synthesisRate(0.1f), reuptakeRate(0.05f),
             receptorSensitivity(1.0f), socialCohesion(0.5f),
             dominanceLevel(0.5f), anxietyLevel(0.2f), rewardSensitivity(1.0f),
             currentTime(0.0f), targetLevel(0.5f),
             adaptationTimeConstant(1.0f), feedbackDelay(0.1f),
             homeostaticRegulation(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl()) {}
Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    if (!pImpl) return 0.0f;
    
    // Calculate level based on mood and behavioral state
    float level = pImpl->moodLevel * 0.4f +
                  pImpl->anhedoniaLevel * 0.2f +
                  pImpl->impulsivityLevel * 0.2f;
    
    // Apply modulation strength
    level *= pImpl->modulationStrength;
    
    // Add baseline
    level += 0.1f;
    
    return std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::setLevel(float level) {
    Neuromodulator::setLevel(level);
    if (pImpl) {
        pImpl->targetLevel = level;
        pImpl->moodLevel = level * 0.8f;
        pImpl->impulsivityLevel = (level < 0.5f) ? (0.5f - level) * 2.0f : 0.0f;
        pImpl->anhedoniaLevel = (0.5f - level) * 0.5f;  // Higher level = less anhedonia
    }
}

float Serotonin::getPlasticityFactor() const {
    if (!pImpl) return 1.0f;
    
    // Serotonin has complex effects on plasticity
    float moodEffect = pImpl->moodLevel * 0.3f;
    float impulsivityEffect = pImpl->impulsivityLevel * 0.2f;
    float anhedoniaEffect = pImpl->anhedoniaLevel * 0.4f;
    
    // Positive mood promotes LTP, high impulsivity promotes LTD
    float plasticityFactor = 1.0f + moodEffect - impulsivityEffect * 0.2f - anhedoniaEffect * 0.5f;
    
    // Apply learning rate modulation
    plasticityFactor *= pImpl->learningRateMod;
    
    // Apply extinction modulation
    plasticityFactor *= pImpl->extinctionMod;
    
    return std::clamp(plasticityFactor, 0.5f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    if (!pImpl) return;
    
    // Update mood based on reward outcomes
    float moodUpdate = (pImpl->rewardSensitivity - pImpl->anhedoniaLevel) * 0.1f;
    pImpl->moodLevel = std::lerp(pImpl->moodLevel, pImpl->targetLevel, moodUpdate * dt);
    
    // Update social behavior
    pImpl->socialCohesion = std::lerp(pImpl->socialCohesion, pImpl->moodLevel, 0.05f * dt);
    pImpl->dominanceLevel = std::lerp(pImpl->dominanceLevel, 0.5f, 0.05f * dt);
    pImpl->anxietyLevel = std::lerp(pImpl->anxietyLevel, pImpl->moodLevel, 0.02f * dt);
    
    // Raphe nucleus activity
    pImpl->dorsalRapheActivity = std::lerp(pImpl->dorsalRapheActivity, pImpl->moodLevel * 1.5f, 0.3f * dt);
    pImpl->medianRapheActivity = std::lerp(pImpl->medianRapheActivity, pImpl->moodLevel * 2.0f, 0.4f * dt);
    pImpl->ponsCoeruleusActivity = std::lerp(pImpl->ponsCoeruleusActivity, pImpl->moodLevel * 1.2f, 0.3f * dt);
    
    pImpl->currentTime += dt;
}

void Serotonin::signalRewardOutcome(float outcome) {
    if (!pImpl) return;
    
    // Update reward sensitivity based on outcome
    pImpl->rewardSensitivity = std::lerp(pImpl->rewardSensitivity, outcome, 0.1f);
    
    // Update mood
    if (outcome > 0.7f) {
        pImpl->moodLevel = std::min(pImpl->moodLevel + 0.1f, 1.0f);
    } else if (outcome < 0.3f) {
        pImpl->moodLevel = std::max(pImpl->moodLevel - 0.1f, 0.0f);
    }
}

void Serotonin::signalSocialBonding(float bonding) {
    if (!pImpl) return;
    pImpl->socialCohesion = std::max(pImpl->socialCohesion, bonding);
}

void Serotonin::signalPunishment(float punishment) {
    if (!pImpl) return;
    
    // Punishment increases anxiety and reduces mood
    pImpl->anxietyLevel = std::min(pImpl->anxietyLevel + punishment, 1.0f);
    pImpl->moodLevel = std::max(pImpl->moodLevel - punishment * 0.5f, 0.0f);
    
    // Increases impulsivity
    pImpl->impulsivityLevel = std::min(pImpl->impulsivityLevel + punishment * 0.3f, 1.0f);
}

} // namespace nlm