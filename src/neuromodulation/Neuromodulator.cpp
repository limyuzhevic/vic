#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// ========== DOPAMINE IMPLEMENTATION ==========

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRateGain;
    float burstPhasingFactor;
    
    // RPE calculation
    float rewardPredictionError;
    float expectedValue;
    float actualValue;
    
    // Burst/pause dynamics
    float burstTimer;
    float pauseTimer;
    float phase;
    
    // History tracking
    std::vector<float> rpeHistory;
    std::vector<float> levelHistory;
    Timestamp lastRPEUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             learningRateGain(1.0f), burstPhasingFactor(0.5f),
             rewardPredictionError(0.0f), expectedValue(0.0f), actualValue(0.0f),
             burstTimer(0.0f), pauseTimer(0.0f), phase(0.0f), lastRPEUpdate(0.0f) {}
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
    pImpl->level = clampLevel(level);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity based on level and burst state
    float burstModulation = isInBurst() ? 2.0f : 1.0f;
    float pauseModulation = isInPause() ? 0.5f : 1.0f;
    float learningModulation = 1.0f + pImpl->learningRateGain * pImpl->level;
    return pImpl->level * burstModulation * pauseModulation * learningModulation;
}

void Dopamine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline with homeostatic regulation
    float targetLevel = pImpl->baseline;
    float homeostaticDrive = std::max(0.0f, pImpl->level - targetLevel) * 0.05f;
    
    // Exponential decay
    pImpl->level = targetLevel + (pImpl->level - targetLevel) * std::exp(-pImpl->decayRate * dtFloat);
    
    // Update phase (burst/pause oscillation)
    if (pImpl->level > 0.5f) {
        // Entering burst phase
        pImpl->phase = std::min(1.0f, pImpl->phase + pImpl->burstPhasingFactor * dtFloat);
        pImpl->burstTimer = std::min(1.0f, pImpl->burstTimer + dtFloat * 0.1f);
        pImpl->pauseTimer = std::max(0.0f, pImpl->pauseTimer - dtFloat * 0.2f);
    } else {
        // Entering pause phase
        pImpl->phase = std::max(0.0f, pImpl->phase - pImpl->burstPhasingFactor * dtFloat);
        pImpl->pauseTimer = std::min(1.0f, pImpl->pauseTimer + dtFloat * 0.1f);
        pImpl->burstTimer = std::max(0.0f, pImpl->burstTimer - dtFloat * 0.2f);
    }
    
    // Homeostatic regulation
    if (pImpl->level < homeostaticDrive * 0.1f) {
        pImpl->level += homeostaticDrive * 0.1f;
    }
    
    // Record history
    pImpl->levelHistory.push_back(pImpl->level);
    if (pImpl->levelHistory.size() > 100) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Burst response to reward
    pImpl->actualValue = reward;
    pImpl->phase = 1.0f; // Maximum burst phase
    pImpl->burstTimer = 1.0f;
    pImpl->pauseTimer = 0.0f;
    
    // Rapid increase with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    
    // Calculate and record RPE
    calculateAndRecordRPE();
    
    NLM_LOG_INFO("Dopamine burst: reward = " + std::to_string(reward) + ", level = " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Response to prediction error
    pImpl->rewardPredictionError = error;
    
    // Positive RPE (better than expected) -> burst
    // Negative RPE (worse than expected) -> pause
    if (error > 0.0f) {
        // Positive RPE: burst
        pImpl->phase = std::min(1.0f, pImpl->phase + error * pImpl->burstPhasingFactor);
        pImpl->burstTimer = std::min(1.0f, pImpl->burstTimer + error * 0.5f);
        pImpl->pauseTimer = std::max(0.0f, pImpl->pauseTimer - error * 0.3f);
    } else {
        // Negative RPE: pause
        pImpl->phase = std::max(0.0f, pImpl->phase - std::abs(error) * pImpl->burstPhasingFactor * 0.5f);
        pImpl->pauseTimer = std::min(1.0f, pImpl->pauseTimer + std::abs(error) * 0.5f);
        pImpl->burstTimer = std::max(0.0f, pImpl->burstTimer - error * 0.3f);
    }
    
    // Apply RPE to level
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Calculate and record RPE
    calculateAndRecordRPE();
    
    if (error > 0) {
        NLM_LOG_INFO("Dopamine burst (positive RPE): error = " + std::to_string(error) + ", level = " + std::to_string(pImpl->level));
    } else {
        NLM_LOG_INFO("Dopamine pause (negative RPE): error = " + std::to_string(error) + ", level = " + std::to_string(pImpl->level));
    }
}

void Dopamine::configure(float baselineLevel, float peakLevel, float decayRate, float releaseRate,
                         float learningRateGain, float burstPhasingFactor) {
    pImpl->baseline = std::clamp(baselineLevel, 0.0f, peakLevel);
    pImpl->peak = peakLevel;
    pImpl->decayRate = decayRate;
    pImpl->releaseRate = releaseRate;
    pImpl->learningRateGain = learningRateGain;
    pImpl->burstPhasingFactor = std::clamp(burstPhasingFactor, 0.1f, 2.0f);
    
    reset();
}

bool Dopamine::isInBurst() const {
    return pImpl->burstTimer > 0.5f;
}

bool Dopamine::isInPause() const {
    return pImpl->pauseTimer > 0.5f;
}

float Dopamine::getBurstPhase() const {
    return pImpl->phase;
}

float Dopamine::getRPE() const {
    return pImpl->rewardPredictionError;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->burstTimer = 0.0f;
    pImpl->pauseTimer = 0.0f;
    pImpl->phase = 0.0f;
    pImpl->rpeHistory.clear();
    pImpl->levelHistory.clear();
}

void Dopamine::calculateAndRecordRPE() {
    pImpl->rewardPredictionError = pImpl->actualValue - pImpl->expectedValue;
    pImpl->rpeHistory.push_back(pImpl->rewardPredictionError);
    if (pImpl->rpeHistory.size() > 50) {
        pImpl->rpeHistory.erase(pImpl->rpeHistory.begin());
    }
    pImpl->lastRPEUpdate = 0; // TODO: Replace with actual timestamp
}

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float filteringStrength;
    float attentionGain;
    float memoryGain;
    float filteringTimeConstant;
    
    // Attention and filtering dynamics
    float attentionFilter;
    float noveltyBuffer;
    float memoryTrace;
    
    // History tracking
    std::vector<float> stimulusHistory;
    std::vector<float> attentionHistory;
    
    Impl() : level(0.0f), baseline(0.0f), filteringStrength(0.5f), 
             attentionGain(1.0f), memoryGain(1.0f), filteringTimeConstant(10.0f),
             attentionFilter(0.0f), noveltyBuffer(0.0f), memoryTrace(0.0f) {}
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
    pImpl->level = clampLevel(level);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity through attention modulation
    float filteringMod = std::pow(pImpl->filteringStrength, 2.0f);
    float memoryMod = 1.0f + pImpl->memoryGain * pImpl->memoryTrace;
    return 1.0f + pImpl->attentionGain * pImpl->attentionFilter * filteringMod * memoryMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Exponential decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-dtFloat / pImpl->filteringTimeConstant);
    
    // Update attention filter with smoothing
    pImpl->attentionFilter = 0.9f * pImpl->attentionFilter + 0.1f * pImpl->level;
    
    // Update memory trace
    pImpl->memoryTrace = std::min(1.0f, pImpl->memoryTrace + dtFloat * 0.01f);
    
    // Novelty decay
    pImpl->noveltyBuffer = std::max(0.0f, pImpl->noveltyBuffer - dtFloat * 0.05f);
    
    // Record history
    pImpl->attentionHistory.push_back(pImpl->attentionFilter);
    if (pImpl->attentionHistory.size() > 100) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
}

void Acetylcholine::signalStimulus(float intensity) {
    // Update novelty buffer
    pImpl->noveltyBuffer = std::max(pImpl->noveltyBuffer, intensity);
    
    // Update attention with stimulus-driven gain
    pImpl->attentionFilter = std::min(1.0f, pImpl->attentionFilter + intensity * pImpl->filteringStrength);
    
    // Increase ACh level with filtering
    float stimulusResponse = intensity * pImpl->filteringStrength * pImpl->attentionGain;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stimulusResponse);
    
    // Boost memory trace for attended stimuli
    pImpl->memoryTrace = std::min(1.0f, pImpl->memoryTrace + intensity * pImpl->memoryGain * 0.1f);
    
    NLM_LOG_INFO("Acetylcholine: stimulus intensity = " + std::to_string(intensity) + ", attention = " + std::to_string(pImpl->attentionFilter));
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    // Update novelty buffer
    pImpl->noveltyBuffer = std::max(pImpl->noveltyBuffer, noveltyLevel);
    
    // Increase ACh level with novelty-driven gain
    float noveltyResponse = noveltyLevel * pImpl->filteringStrength * pImpl->attentionGain * 2.0f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyResponse);
    
    NLM_LOG_INFO("Acetylcholine: novelty level = " + std::to_string(noveltyLevel) + ", level = " + std::to_string(pImpl->level));
}

bool Acetylcholine::isAttentive() const {
    return pImpl->attentionFilter > 0.3f;
}

float Acetylcholine::getFilteringOutput() const {
    return pImpl->attentionFilter;
}

void Acetylcholine::configure(float baselineLevel, float filteringStrength, float attentionGain,
                              float memoryGain, float filteringTimeConstant) {
    pImpl->baseline = baselineLevel;
    pImpl->filteringStrength = std::clamp(filteringStrength, 0.1f, 2.0f);
    pImpl->attentionGain = attentionGain;
    pImpl->memoryGain = memoryGain;
    pImpl->filteringTimeConstant = filteringTimeConstant;
    
    reset();
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

float Acetylcholine::getFilteringStrength() const {
    return pImpl->filteringStrength;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryGain() const {
    return pImpl->memoryGain;
}

float Acetylcholine::getFilteringTimeConstant() const {
    return pImpl->filteringTimeConstant;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->noveltyBuffer = 0.0f;
    pImpl->memoryTrace = 0.0f;
    pImpl->attentionFilter = 0.0f;
    pImpl->stimulusHistory.clear();
    pImpl->attentionHistory.clear();
}

// ========== NORADRENALINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalGain;
    float vigilanceGain;
    float stressSensitivity;
    float recoveryRate;
    
    // Arousal and vigilance dynamics
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    
    // History tracking
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    
    Impl() : level(0.0f), baseline(0.0f), arousalGain(1.0f), vigilanceGain(1.0f),
             stressSensitivity(1.0f), recoveryRate(0.1f),
             arousalLevel(0.0f), vigilanceLevel(0.0f), stressLevel(0.0f) {}
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
    pImpl->level = clampLevel(level);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and vigilance
    float arousalMod = 1.0f + pImpl->arousalGain * pImpl->arousalLevel;
    float vigilanceMod = 1.0f + pImpl->vigilanceGain * pImpl->vigilanceLevel;
    float stressMod = 1.0f - pImpl->stressSensitivity * pImpl->stressLevel;
    return arousalMod * vigilanceMod * stressMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Exponential decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->recoveryRate * dtFloat);
    
    // Update arousal with slow dynamics
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + dtFloat * 0.01f);
    
    // Update vigilance with fast dynamics
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + dtFloat * 0.05f);
    
    // Stress recovery
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - dtFloat * pImpl->recoveryRate * 0.5f);
    
    // Record history
    pImpl->arousalHistory.push_back(pImpl->arousalLevel);
    pImpl->vigilanceHistory.push_back(pImpl->vigilanceLevel);
    if (pImpl->arousalHistory.size() > 100) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
        pImpl->vigilanceHistory.erase(pImpl->vigilanceHistory.begin());
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    // Update stress level with saturation
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressLevel * pImpl->stressSensitivity);
    
    // Increase NE level with stress response
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 0.5f);
    
    // Suppress arousal and vigilance during stress
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - stressLevel * 0.3f);
    pImpl->vigilanceLevel = std::max(0.0f, pImpl->vigilanceLevel - stressLevel * 0.2f);
    
    NLM_LOG_INFO("Norepinephrine: stress level = " + std::to_string(stressLevel) + ", NE level = " + std::to_string(pImpl->level));
}

void Norepinephrine::signalAlertness(float alertnessLevel) {
    // Increase alertness
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + alertnessLevel * 0.3f);
    
    // Increase NE level with alertness response
    pImpl->level = std::min(pImpl->peak, pImpl->level + alertnessLevel * 0.3f);
    
    // Recovery from stress
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - alertnessLevel * 0.5f);
    
    NLM_LOG_INFO("Norepinephrine: alertness level = " + std::to_string(alertnessLevel) + ", NE level = " + std::to_string(pImpl->level));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

bool Norepinephrine::isAlert() const {
    return pImpl->vigilanceLevel > 0.5f && pImpl->stressLevel < 0.5f;
}

void Norepinephrine::configure(float baselineLevel, float arousalGain, float vigilanceGain,
                               float stressSensitivity, float recoveryRate) {
    pImpl->baseline = baselineLevel;
    pImpl->arousalGain = arousalGain;
    pImpl->vigilanceGain = vigilanceGain;
    pImpl->stressSensitivity = stressSensitivity;
    pImpl->recoveryRate = recoveryRate;
    
    reset();
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

float Norepinephrine::getArousalGain() const {
    return pImpl->arousalGain;
}

float Norepinephrine::getVigilanceGain() const {
    return pImpl->vigilanceGain;
}

float Norepinephrine::getStressSensitivity() const {
    return pImpl->stressSensitivity;
}

float Norepinephrine::getRecoveryRate() const {
    return pImpl->recoveryRate;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.0f;
    pImpl->vigilanceLevel = 0.0f;
    pImpl->stressLevel = 0.0f;
    pImpl->arousalHistory.clear();
    pImpl->vigilanceHistory.clear();
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodGain;
    float impulsivityGain;
    float socialSensitivity;
    float recoveryTimeConstant;
    
    // Mood and impulsivity dynamics
    float moodLevel;
    float impulsivityLevel;
    float socialCohesion;
    
    // History tracking
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    
    Impl() : level(0.0f), baseline(0.0f), moodGain(1.0f), impulsivityGain(1.0f),
             socialSensitivity(1.0f), recoveryTimeConstant(20.0f),
             moodLevel(0.5f), impulsivityLevel(0.5f), socialCohesion(0.5f) {}
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
    pImpl->level = clampLevel(level);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on mood and impulsivity
    float moodMod = 1.0f + pImpl->moodGain * (pImpl->moodLevel - 0.5f) * 2.0f;
    float impulsivityMod = 1.0f - pImpl->impulsivityGain * pImpl->impulsivityLevel;
    float socialMod = 1.0f + pImpl->socialSensitivity * pImpl->socialCohesion;
    return moodMod * impulsivityMod * socialMod;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Exponential decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-dtFloat / pImpl->recoveryTimeConstant);
    
    // Update mood with social influence
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + dtFloat * 0.005f * pImpl->socialCohesion);
    
    // Update impulsivity with homeostatic regulation
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - dtFloat * 0.002f);
    
    // Update social cohesion
    pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + dtFloat * 0.001f);
    
    // Record history
    pImpl->moodHistory.push_back(pImpl->moodLevel);
    pImpl->socialHistory.push_back(pImpl->socialCohesion);
    if (pImpl->moodHistory.size() > 100) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
}

void Serotonin::signalMoodEvent(float moodValue) {
    // Update mood level
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + moodValue * pImpl->moodGain);
    
    // Increase 5-HT level with mood response
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodValue * 0.3f);
    
    NLM_LOG_INFO("Serotonin: mood value = " + std::to_string(moodValue) + ", 5-HT level = " + std::to_string(pImpl->level));
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    // Update impulsivity level
    pImpl->impulsivityLevel = std::min(1.0f, pImpl->impulsivityLevel + impulsivityLevel * pImpl->impulsivityGain);
    
    // Decrease 5-HT level with impulsivity response (high impulsivity = low 5-HT)
    pImpl->level = std::max(0.0f, pImpl->level - impulsivityLevel * 0.5f);
    
    NLM_LOG_INFO("Serotonin: impulsivity level = " + std::to_string(impulsivityLevel) + ", 5-HT level = " + std::to_string(pImpl->level));
}

void Serotonin::signalSocialEvent(float socialCohesion) {
    // Update social cohesion
    pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + socialCohesion * pImpl->socialSensitivity);
    
    // Increase 5-HT level with social response
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * 0.3f);
    
    NLM_LOG_INFO("Serotonin: social cohesion = " + std::to_string(socialCohesion) + ", 5-HT level = " + std::to_string(pImpl->level));
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

bool Serotonin::isSociallyCooperative() const {
    return pImpl->socialCohesion > 0.6f && pImpl->impulsivityLevel < 0.4f;
}

void Serotonin::configure(float baselineLevel, float moodGain, float impulsivityGain,
                           float socialSensitivity, float recoveryTimeConstant) {
    pImpl->baseline = baselineLevel;
    pImpl->moodGain = moodGain;
    pImpl->impulsivityGain = impulsivityGain;
    pImpl->socialSensitivity = socialSensitivity;
    pImpl->recoveryTimeConstant = recoveryTimeConstant;
    
    reset();
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

float Serotonin::getMoodGain() const {
    return pImpl->moodGain;
}

float Serotonin::getImpulsivityGain() const {
    return pImpl->impulsivityGain;
}

float Serotonin::getSocialSensitivity() const {
    return pImpl->socialSensitivity;
}

float Serotonin::getRecoveryTimeConstant() const {
    return pImpl->recoveryTimeConstant;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialCohesion = 0.5f;
    pImpl->moodHistory.clear();
    pImpl->socialHistory.clear();
}

// ========== NEUROMODULATOR MANAGER IMPLEMENTATION ==========

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    
    // Individual neuromodulators
    Dopamine* dopamine;
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Serotonin* serotonin;
    
    // Combined effects
    float combinedArousal;
    float combinedAttention;
    float combinedReward;
    float combinedMood;
    float combinedSocial;
    
    // Cross-talk interactions
    float daAChInteraction;
    float neAChInteraction;
    float daSerotoninInteraction;
    
    // History tracking
    std::vector<float> effectHistory;
    
    Impl() : brain(nullptr), combinedArousal(0.0f), combinedAttention(0.0f),
             combinedReward(0.0f), combinedMood(0.0f), combinedSocial(0.0f),
             daAChInteraction(1.0f), neAChInteraction(1.0f), daSerotoninInteraction(1.0f) {}
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Create neuromodulators
    pImpl->dopamine = new Dopamine();
    pImpl->acetylcholine = new Acetylcholine();
    pImpl->norepinephrine = new Norepinephrine();
    pImpl->serotonin = new Serotonin();
    
    // Configure with biologically realistic parameters
    pImpl->dopamine->configure(0.1f, 1.0f, 0.2f, 1.5f, 1.0f, 0.8f);
    pImpl->acetylcholine->configure(0.2f, 0.8f, 1.2f, 1.5f, 10.0f);
    pImpl->norepinephrine->configure(0.15f, 1.0f, 1.2f, 1.0f, 0.05f);
    pImpl->serotonin->configure(0.1f, 1.0f, 1.0f, 1.5f, 20.0f);
    
    NLM_LOG_INFO("NeuromodulatorManager initialized with real biological implementations");
}

void NeuromodulatorManager::updateAll(TimestepDuration dt) {
    // Update all neuromodulators
    pImpl->dopamine->update(dt);
    pImpl->acetylcholine->update(dt);
    pImpl->norepinephrine->update(dt);
    pImpl->serotonin->update(dt);
}

void NeuromodulatorManager::processIntegration(TimestepDuration dt) {
    // Process cross-talk interactions
    float daLevel = pImpl->dopamine->getLevel();
    float acLevel = pImpl->acetylcholine->getLevel();
    float neLevel = pImpl->norepinephrine->getLevel();
    float serLevel = pImpl->serotonin->getLevel();
    
    // DA-ACh interaction: Dopamine modulates ACh filtering
    pImpl->daAChInteraction = 1.0f + daLevel * 0.5f * acLevel * 0.2f;
    
    // NE-ACh interaction: Norepinephrine modulates ACh attention
    pImpl->neAChInteraction = 1.0f + neLevel * 0.3f * acLevel * 0.3f;
    
    // DA-Serotonin interaction: Balanced through opponent processes
    pImpl->daSerotoninInteraction = 1.0f - daLevel * serLevel * 0.3f;
    
    // Calculate combined effects
    pImpl->combinedArousal = neLevel * pImpl->brain->getNorepinephrine()->getArousalLevel();
    pImpl->combinedAttention = acLevel * pImpl->brain->getAcetylcholine()->getFilteringOutput();
    pImpl->combinedReward = daLevel;
    pImpl->combinedMood = serLevel * pImpl->brain->getSerotonin()->getMoodLevel();
    pImpl->combinedSocial = serLevel * pImpl->brain->getSerotonin()->getSocialCohesion();
    
    // Apply interactions to neuromodulators
    pImpl->acetylcholine->configure(
        pImpl->acetylcholine->getBaseline(),
        pImpl->acetylcholine->getFilteringStrength(),
        pImpl->acetylcholine->getAttentionGain(),
        pImpl->acetylcholine->getMemoryGain(),
        pImpl->acetylcholine->getFilteringTimeConstant()
    );
}

void NeuromodulatorManager::applyAll(class Brain* brain) {
    // Apply all neuromodulators to the brain
    brain->applyNeuromodulation(*pImpl->dopamine);
    brain->applyNeuromodulation(*pImpl->acetylcholine);
    brain->applyNeuromodulation(*pImpl->norepinephrine);
    brain->applyNeuromodulation(*pImpl->serotonin);
}

float NeuromodulatorManager::getCombinedEffect() const {
    return pImpl->combinedReward * 0.4f + pImpl->combinedAttention * 0.3f + 
           pImpl->combinedArousal * 0.2f + pImpl->combinedMood * 0.1f;
}

void NeuromodulatorManager::signalEvents(float reward, float predictionError, float novelty, 
                                          float stress, float mood, float socialCohesion) {
    // Signal events through all relevant neuromodulators
    if (std::abs(reward) > 0.01f) {
        pImpl->dopamine->signalReward(reward);
    }
    if (std::abs(predictionError) > 0.01f) {
        pImpl->dopamine->signalRewardPredictionError(predictionError);
    }
    if (novelty > 0.01f) {
        pImpl->acetylcholine->signalNovelty(novelty);
    }
    if (stress > 0.01f) {
        pImpl->norepinephrine->signalStress(stress);
    }
    if (std::abs(mood) > 0.01f) {
        pImpl->serotonin->signalMoodEvent(mood);
    }
    if (std::abs(socialCohesion) > 0.01f) {
        pImpl->serotonin->signalSocialEvent(socialCohesion);
    }
}

Dopamine* NeuromodulatorManager::getDopamine() const {
    return pImpl->dopamine;
}

Acetylcholine* NeuromodulatorManager::getAcetylcholine() const {
    return pImpl->acetylcholine;
}

Norepinephrine* NeuromodulatorManager::getNorepinephrine() const {
    return pImpl->norepinephrine;
}

Serotonin* NeuromodulatorManager::getSerotonin() const {
    return pImpl->serotonin;
}

void NeuromodulatorManager::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->acetylcholine) pImpl->acetylcholine->reset();
    if (pImpl->norepinephrine) pImpl->norepinephrine->reset();
    if (pImpl->serotonin) pImpl->serotonin->reset();
    
    pImpl->effectHistory.clear();
}

} // namespace nlm