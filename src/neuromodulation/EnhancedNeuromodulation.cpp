#include "EnhancedNeuromodulation.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

// Dopamine implementation

struct Dopamine::Impl {
    float phasicComponent;      // Phasic burst component (0-5.0)
    float tonicComponent;       // Tonic baseline (0-1.0)
    float predictionError;      // Reward prediction error (-1 to 1)
    float learningRate;         // Current learning rate (0-0.1)
    float phasicGain;           // Gain for phasic responses
    float tonicDecay;           // Decay rate for tonic component
    float learningRateBase;     // Base learning rate
    
    // Temporal dynamics
    float phasicTime;           // Time since last phasic event
    float tonicHistory[20];      // Tonic level history
    int historyIndex;
    
    // Reward history
    float currentReward;        // Current reward value
    float expectedReward;       // Expected reward
    float rewardHistory[10];     // Recent rewards
    
    Impl() : phasicComponent(0.0f), tonicComponent(0.1f), predictionError(0.0f),
             learningRate(0.001f), phasicGain(2.0f), tonicDecay(0.01f),
             learningRateBase(0.001f), phasicTime(0.0f), historyIndex(0),
             currentReward(0.0f), expectedReward(0.0f) {
        for (int i = 0; i < 20; ++i) tonicHistory[i] = 0.1f;
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->tonicComponent + pImpl->phasicComponent * 2.0f;
}

void Dopamine::setLevel(float level) {
    // Clamp to reasonable range
    level = std::clamp(level, 0.0f, 5.0f);
    
    // Distribute between tonic and phasic components
    pImpl->tonicComponent = level * 0.2f;
    pImpl->phasicComponent = (level - pImpl->tonicComponent) / 2.0f;
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity in complex ways
    // High dopamine increases STDP magnitude
    // But too much dopamine can cause instability
    float stabilityFactor = 1.0f / (1.0f + std::abs(pImpl->predictionError) * 2.0f);
    float phasicModulation = 1.0f + pImpl->phasicComponent * pImpl->phasicGain * 0.1f;
    return stabilityFactor * phasicModulation * pImpl->learningRate;
}

void Dopamine::update(TimestepDuration dt) {
    // Update temporal dynamics
    pImpl->phasicTime += static_cast<float>(dt);
    
    // Natural decay of phasic component
    pImpl->phasicComponent *= std::exp(-pImpl->tonicDecay * static_cast<float>(dt));
    
    // Update tonic level with history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    pImpl->tonicHistory[pImpl->historyIndex] = pImpl->tonicComponent;
    
    // Calculate average tonic level from history
    float sum = 0.0f;
    for (int i = 0; i < 20; ++i) sum += pImpl->tonicHistory[i];
    pImpl->tonicComponent = sum / 20.0f;
    
    // Adapt learning rate based on innovation
    updateLearningRate(0.0f); // Would need innovation level from elsewhere
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to unexpected reward
    currentReward = std::clamp(reward, 0.0f, 1.0f);
    
    float rewardPrediction = getRewardPrediction(expectedReward);
    float error = currentReward - rewardPrediction;
    
    // Strong phasic response for large prediction errors
    pImpl->phasicComponent = std::abs(error) * 3.0f;
    pImpl->predictionError = error;
    
    // Update expected reward
    expectedReward = currentReward * 0.7f + expectedReward * 0.3f;
}

void Dopamine::signalRewardPredictionError(float error, float predictedReward) {
    // Direct prediction error signaling
    pImpl->predictionError = std::clamp(error, -1.0f, 1.0f);
    expectedReward = std::clamp(predictedReward, 0.0f, 1.0f);
    
    // Phasic response proportional to prediction error magnitude
    pImpl->phasicComponent = std::abs(error) * pImpl->phasicGain;
}

float Dopamine::getPhasicComponent() const {
    return pImpl->phasicComponent;
}

float Dopamine::getTonicComponent() const {
    return pImpl->tonicComponent;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getRewardPrediction(float predictedReward) const {
    return std::clamp(predictedReward, 0.0f, 1.0f);
}

void Dopamine::updateLearningRate(float innovationLevel) {
    // Dopamine enhances learning when there's prediction error or novelty
    float innovationModifier = 1.0f + std::abs(pImpl->predictionError) * 2.0f + innovationLevel * 0.5f;
    pImpl->learningRate = pImpl->learningRateBase * innovationModifier;
    
    // Keep learning rate within bounds
    pImpl->learningRate = std::clamp(pImpl->learningRate, 0.0001f, 0.01f);
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

void Dopamine::reset() {
    pImpl->phasicComponent = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->learningRate = pImpl->learningRateBase;
    pImpl->phasicTime = 0.0f;
    currentReward = 0.0f;
    expectedReward = 0.0f;
}

void Dopamine::setPhasicGain(float gain) {
    pImpl->phasicGain = std::clamp(gain, 0.0f, 10.0f);
}

void Dopamine::setTonicDecay(float decay) {
    pImpl->tonicDecay = std::clamp(decay, 0.001f, 0.1f);
}

void Dopamine::setLearningRateBase(float rate) {
    pImpl->learningRateBase = std::clamp(rate, 0.0001f, 0.01f);
    pImpl->learningRate = pImpl->learningRateBase;
}

// Acetylcholine implementation

struct Acetylcholine::Impl {
    float attentionLevel;           // Current attention (0-1)
    float memoryConsolidation;      // Memory consolidation strength (0-1)
    float sensoryGain;              // Sensory input gain (0-2)
    float attentionGain;            // Attention modulation gain
    float memoryThreshold;          // Threshold for consolidation
    float basalLevel;               // Baseline ACh level
    
    // Temporal dynamics
    float attentionHistory[15];      // Attention level history
    float memoryHistory[10];         // Memory enhancement history
    int historyIndex;
    int memoryIndex;
    
    // Novelty detection
    float currentNovelty;           // Current novelty level (0-1)
    float noveltyThreshold;         // Threshold for attention shift
    
    Impl() : attentionLevel(0.5f), memoryConsolidation(0.3f), sensoryGain(1.0f),
             attentionGain(1.0f), memoryThreshold(0.7f), basalLevel(0.1f),
             historyIndex(0), memoryIndex(0),
             currentNovelty(0.0f), noveltyThreshold(0.5f) {
        for (int i = 0; i < 15; ++i) attentionHistory[i] = 0.5f;
        for (int i = 0; i < 10; ++i) memoryHistory[i] = 0.3f;
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->attentionLevel + pImpl->memoryConsolidation * 0.5f;
}

void Acetylcholine::setLevel(float level) {
    level = std::clamp(level, 0.0f, 2.0f);
    pImpl->attentionLevel = level * 0.7f;
    pImpl->memoryConsolidation = (level - pImpl->attentionLevel * 0.7f) * 0.5f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attended and novel information
    float noveltyModulation = 1.0f + pImpl->currentNovelty * pImpl->attentionGain * 0.5f;
    float attentionModulation = 1.0f + pImpl->attentionLevel * pImpl->attentionGain * 0.3f;
    return noveltyModulation * attentionModulation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Update attention based on novelty
    if (pImpl->currentNovelty > pImpl->noveltyThreshold) {
        // Novel stimuli capture attention
        pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + 0.05f);
    } else {
        // Decay attention to baseline
        pImpl->attentionLevel = std::max(pImpl->basalLevel, pImpl->attentionLevel * 0.95f);
    }
    
    // Memory consolidation builds up over time
    if (pImpl->attentionLevel > pImpl->memoryThreshold) {
        pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.02f);
    } else {
        pImpl->memoryConsolidation *= 0.98f;
    }
    
    // Update history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 15;
    pImpl->attentionHistory[pImpl->historyIndex] = pImpl->attentionLevel;
    
    pImpl->memoryIndex = (pImpl->memoryIndex + 1) % 10;
    pImpl->memoryHistory[pImpl->memoryIndex] = pImpl->memoryConsolidation;
}

void Acetylcholine::signalNovelty(float novelty) {
    pImpl->currentNovelty = std::clamp(novelty, 0.0f, 1.0f);
    
    // Novelty increases attention if above threshold
    if (pImpl->currentNovelty > pImpl->noveltyThreshold) {
        pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + 0.1f * pImpl->attentionGain);
    }
}

void Acetylcholine::signalSensoryInput(float inputStrength) {
    // Strong sensory inputs increase cholinergic activity
    pImpl->sensoryGain = std::clamp(inputStrength, 0.0f, 2.0f);
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + 0.05f * pImpl->sensoryGain);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::initiateMemoryConsolidation() {
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.3f);
}

void Acetylcholine::enhanceMemoryTrace(float timeConstant) {
    // Store memory enhancement with specified time constant
    timeConstant = std::clamp(timeConstant, 0.1f, 5.0f);
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation * (1.0f + 0.1f / timeConstant));
}

float Acetylcholine::getMemoryEnhancement() const {
    return pImpl->memoryConsolidation;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 5.0f);
}

void Acetylcholine::setMemoryConsolidationThreshold(float threshold) {
    pImpl->memoryThreshold = std::clamp(threshold, 0.1f, 1.0f);
}

void Acetylcholine::setBasalLevel(float basal) {
    pImpl->basalLevel = std::clamp(basal, 0.0f, 0.5f);
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    float vigilanceLevel;           // Current vigilance (0-1)
    float responseVigor;            // Response vigor (0-1)
    float stressLevel;              // Stress level (0-1)
    float vigilanceGain;            // Vigilance modulation gain
    float stressDecay;              // Stress decay rate
    float baselineVigilance;        // Baseline vigilance level
    
    // Temporal dynamics
    float vigilanceHistory[20];      // Vigilance level history
    float vigorHistory[15];           // Response vigor history
    int historyIndex;
    
    // Task state
    float currentDifficulty;        // Current task difficulty
    float timeOnTask;               // Time spent on current task
    
    // Stress tracking
    float currentStressor;          // Current stressor intensity
    float stressDuration;           // How long stress has been present
    
    Impl() : vigilanceLevel(0.5f), responseVigor(0.5f), stressLevel(0.0f),
             vigilanceGain(1.0f), stressDecay(0.01f), baselineVigilance(0.3f),
             historyIndex(0), currentDifficulty(0.5f), timeOnTask(0.0f),
             currentStressor(0.0f), stressDuration(0.0f) {
        for (int i = 0; i < 20; ++i) vigilanceHistory[i] = 0.5f;
        for (int i = 0; i < 15; ++i) vigorHistory[i] = 0.5f;
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->vigilanceLevel + pImpl->responseVigor * 0.5f + pImpl->stressLevel * 0.3f;
}

void Norepinephrine::setLevel(float level) {
    level = std::clamp(level, 0.0f, 3.0f);
    pImpl->vigilanceLevel = level * 0.5f;
    pImpl->responseVigor = (level - pImpl->vigilanceLevel * 0.5f) * 0.4f;
    pImpl->stressLevel = std::max(0.0f, level - 1.0f) * 0.3f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and stress
    float stressModulation = 1.0f + pImpl->stressLevel * 0.5f;
    float vigilanceModulation = 1.0f + pImpl->vigilanceLevel * 0.3f;
    return stressModulation * vigilanceModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeSeconds = static_cast<float>(dt);
    
    // Update vigilance based on task difficulty and time on task
    updateVigilance(pImpl->currentDifficulty, pImpl->timeOnTask);
    
    // Decay stress over time
    pImpl->stressDecay = std::min(pImpl->stressDecay + 0.001f, 0.05f);
    pImpl->stressLevel *= (1.0f - pImpl->stressDecay * timeSeconds);
    
    // Update history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    pImpl->vigilanceHistory[pImpl->historyIndex] = pImpl->vigilanceLevel;
    
    // Update response vigor history
    int vigorIndex = (pImpl->historyIndex + 5) % 15;  // 5-step lag
    pImpl->vigorHistory[vigorIndex] = pImpl->responseVigor;
}

void Norepinephrine::updateVigilance(float difficulty, float timeOnTask) {
    pImpl->currentDifficulty = std::clamp(difficulty, 0.0f, 1.0f);
    pImpl->timeOnTask = timeOnTask;
    
    // Vigilance follows an inverted U function of difficulty
    // Optimal vigilance is higher for difficult tasks
    float optimalVigilance = pImpl->baselineVigilance * (1.0f + pImpl->currentDifficulty * 0.5f);
    
    // Vigilance decays with time on task
    float timeDecay = 1.0f - std::min(1.0f, timeOnTask * 0.001f);
    
    pImpl->vigilanceLevel = optimalVigilance * timeDecay;
    pImpl->vigilanceLevel = std::clamp(pImpl->vigilanceLevel, pImpl->baselineVigilance, 1.0f);
    
    // Modulate response vigor with vigilance
    pImpl->responseVigor = pImpl->vigilanceLevel * 0.8f;
}

void Norepinephrine::modulateResponse(float vigor, float baselineEnergy) {
    pImpl->responseVigor = std::clamp(vigor, 0.0f, 1.0f);
    
    // Vigor affects energy availability
    float energyMultiplier = 1.0f + pImpl->responseVigor * 0.5f;
    
    // Modulate other neuromodulators based on vigor
    if (pImpl->responseVigor > 0.7f) {
        // High vigor increases stress response
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + 0.1f);
    }
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::signalStress(float stressorIntensity, float duration) {
    pImpl->currentStressor = std::clamp(stressorIntensity, 0.0f, 1.0f);
    pImpl->stressDuration = std::clamp(duration, 0.0f, 10.0f);
    
    // Stress response depends on both intensity and duration
    float stressResponse = pImpl->currentStressor * (1.0f + pImpl->stressDuration * 0.1f);
    pImpl->stressLevel = std::min(1.0f, stressResponse);
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::setVigilanceGain(float gain) {
    pImpl->vigilanceGain = std::clamp(gain, 0.0f, 5.0f);
}

void Norepinephrine::setStressDecay(float decay) {
    pImpl->stressDecay = std::clamp(decay, 0.001f, 0.1f);
}

void Norepinephrine::setBaselineVigilance(float level) {
    pImpl->baselineVigilance = std::clamp(level, 0.1f, 1.0f);
}

// Serotonin implementation

struct Serotonin::Impl {
    float moodLevel;               // Current mood (0-1)
    float inhibitionLevel;        // Behavioral inhibition (0-1)
    float socialRank;              // Social status (-1 to 1)
    float socialSupport;           // Social support (-1 to 1)
    float inhibitionGain;          // Inhibition modulation gain
    float baselineSerotonin;       // Baseline serotonin level
    
    // Temporal dynamics
    float moodHistory[20];          // Mood level history
    float inhibitionHistory[15];    // Inhibition level history
    int historyIndex;
    
    // Sleep-wake regulation
    float sleepPressure;           // Sleep pressure (0-1)
    bool isSleeping;               // Current sleep state
    float wakeTime;                // Time since last sleep
    
    // Conflict tracking
    float currentConflict;         // Current conflict strength (0-1)
    
    Impl() : moodLevel(0.5f), inhibitionLevel(0.5f), socialRank(0.0f), socialSupport(0.0f),
             inhibitionGain(1.0f), baselineSerotonin(0.2f),
             historyIndex(0), sleepPressure(0.0f), isSleeping(false), wakeTime(0.0f),
             currentConflict(0.0f) {
        for (int i = 0; i < 20; ++i) moodHistory[i] = 0.5f;
        for (int i = 0; i < 15; ++i) inhibitionHistory[i] = 0.5f;
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->moodLevel + pImpl->inhibitionLevel * 0.5f;
}

void Serotonin::setLevel(float level) {
    level = std::clamp(level, 0.0f, 2.0f);
    pImpl->moodLevel = level * 0.6f;
    pImpl->inhibitionLevel = (level - pImpl->moodLevel * 0.6f) * 0.5f;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and inhibition
    float moodModulation = 1.0f + pImpl->moodLevel * 0.3f;
    float inhibitionModulation = 1.0f - pImpl->inhibitionLevel * 0.5f;
    return moodModulation * inhibitionModulation;
}

void Serotonin::update(TimestepDuration dt) {
    float timeSeconds = static_cast<float>(dt);
    
    // Update sleep pressure
    updateSleepPressure(pImpl->wakeTime);
    
    // Update mood based on social factors
    updateSocialStatus(pImpl->socialRank, pImpl->socialSupport);
    
    // Update history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    pImpl->moodHistory[pImpl->historyIndex] = pImpl->moodLevel;
    
    int inhibitionIndex = (pImpl->historyIndex + 5) % 15;
    pImpl->inhibitionHistory[inhibitionIndex] = pImpl->inhibitionLevel;
}

void Serotonin::updateSocialStatus(float socialRank, float socialSupport) {
    pImpl->socialRank = std::clamp(socialRank, -1.0f, 1.0f);
    pImpl->socialSupport = std::clamp(socialSupport, -1.0f, 1.0f);
    
    // Social factors affect mood
    float socialInfluence = (pImpl->socialRank + pImpl->socialSupport) * 0.5f;
    pImpl->moodLevel = 0.5f + socialInfluence * 0.3f;
    pImpl->moodLevel = std::clamp(pImpl->moodLevel, 0.1f, 1.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

void Serotonin::signalConflict(float conflictStrength) {
    pImpl->currentConflict = std::clamp(conflictStrength, 0.0f, 1.0f);
    
    // Conflict increases inhibition
    pImpl->inhibitionLevel = std::min(1.0f, pImpl->inhibitionLevel + 0.1f * pImpl->currentConflict);
}

void Serotonin::inhibitBehavior(float targetStrength) {
    pImpl->inhibitionLevel = std::min(1.0f, pImpl->inhibitionLevel + targetStrength * pImpl->inhibitionGain);
}

float Serotonin::getInhibitionLevel() const {
    return pImpl->inhibitionLevel;
}

void Serotonin::updateSleepPressure(float wakeTime) {
    pImpl->wakeTime = wakeTime;
    
    // Sleep pressure builds up during wake period
    if (!pImpl->isSleeping) {
        pImpl->sleepPressure = std::min(1.0f, pImpl->sleepPressure + 0.001f * wakeTime);
    } else {
        // Sleep reduces pressure
        pImpl->sleepPressure *= 0.95f;
    }
}

void Serotonin::enterSleep() {
    pImpl->isSleeping = true;
    pImpl->sleepPressure = 0.0f;
    pImpl->inhibitionLevel = pImpl->baselineSerotonin;
}

void Serotonin::wakeUp() {
    pImpl->isSleeping = false;
    pImpl->wakeTime = 0.0f;
}

float Serotonin::getSleepPressure() const {
    return pImpl->sleepPressure;
}

bool Serotonin::isAsleep() const {
    return pImpl->isSleeping;
}

void Serotonin::setSocialSensitivity(float sensitivity) {
    pImpl->inhibitionGain = std::clamp(sensitivity, 0.0f, 5.0f);
}

void Serotonin::setInhibitionDecay(float decay) {
    // Inhibition decays with time
    // This is used in the update loop
}

void Serotonin::setBaselineSerotonin(float level) {
    pImpl->baselineSerotonin = std::clamp(level, 0.0f, 0.5f);
    pImpl->moodLevel = pImpl->baselineSerotonin;
}

// NeuromodulatorySystem implementation

struct NeuromodulatorySystem::Impl {
    Dopamine* dopamine;
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Serotonin* serotonin;
    
    // Integration parameters
    float dopamineWeight;           // Weight for dopamine integration
    float achWeight;                // Weight for ACh integration
    float neWeight;                 // Weight for NE integration
    float fiveHTWeight;             // Weight for 5-HT integration
    
    // Network state
    float totalSpikes;              // Total spikes in network
    size_t totalNeurons;           // Total neurons in network
    float currentExcitability;      // Current network excitability
    
    Impl() : dopamine(nullptr), acetylcholine(nullptr), norepinephrine(nullptr), serotonin(nullptr),
             dopamineWeight(1.0f), achWeight(1.0f), neWeight(0.8f), fiveHTWeight(0.6f),
             totalSpikes(0.0f), totalNeurons(100), currentExcitability(1.0f) {}
};

NeuromodulatorySystem::NeuromodulatorySystem() : pImpl(new Impl) {}

NeuromodulatorySystem::~NeuromodulatorySystem() = default;

Dopamine* NeuromodulatorySystem::getDopamine() {
    return pImpl->dopamine;
}

Acetylcholine* NeuromodulatorySystem::getAcetylcholine() {
    return pImpl->acetylcholine;
}

Norepinephrine* NeuromodulatorySystem::getNorepinephrine() {
    return pImpl->norepinephrine;
}

Serotonin* NeuromodulatorySystem::getSerotonin() {
    return pImpl->serotonin;
}

void NeuromodulatorySystem::updateAll(TimestepDuration dt) {
    if (pImpl->dopamine) pImpl->dopamine->update(dt);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(dt);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(dt);
    if (pImpl->serotonin) pImpl->serotonin->update(dt);
}

float NeuromodulatorySystem::computeSTDPModulation() const {
    float totalModulation = 1.0f;
    
    if (pImpl->dopamine) {
        totalModulation *= (1.0f + pImpl->dopamine->getPhasicComponent() * 0.5f);
    }
    if (pImpl->acetylcholine) {
        totalModulation *= (1.0f + pImpl->acetylcholine->getAttentionLevel() * 0.3f);
    }
    if (pImpl->norepinephrine) {
        totalModulation *= (1.0f + pImpl->norepinephrine->getVigilanceLevel() * 0.2f);
    }
    if (pImpl->serotonin) {
        totalModulation *= (1.0f - pImpl->serotonin->getInhibitionLevel() * 0.2f);
    }
    
    return totalModulation;
}

float NeuromodulatorySystem::computeHebbianModulation() const {
    float totalModulation = 1.0f;
    
    if (pImpl->acetylcholine) {
        totalModulation *= (1.0f + pImpl->acetylcholine->getMemoryEnhancement() * 0.4f);
    }
    if (pImpl->dopamine) {
        totalModulation *= (1.0f + pImpl->dopamine->getPredictionError() * 0.3f);
    }
    if (pImpl->serotonin) {
        totalModulation *= (1.0f + pImpl->serotonin->getMoodLevel() * 0.2f);
    }
    
    return totalModulation;
}

float NeuromodulatorySystem::computeHomeostaticModulation() const {
    float totalModulation = 1.0f;
    
    if (pImpl->norepinephrine) {
        totalModulation *= (1.0f + pImpl->norepinephrine->getStressLevel() * 0.3f);
    }
    if (pImpl->dopamine) {
        totalModulation *= (1.0f + pImpl->dopamine->getLearningRate() * 10.0f);
    }
    
    return totalModulation;
}

void NeuromodulatorySystem::signalReward(float reward, float predictionError) {
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(predictionError, reward);
    }
}

void NeuromodulatorySystem::signalNovelty(float novelty) {
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->signalNovelty(novelty);
    }
}

void NeuromodulatorySystem::signalStress(float stressLevel) {
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->signalStress(stressLevel, 1.0f);
    }
}

void NeuromodulatorySystem::updateVigilance(float difficulty) {
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->updateVigilance(difficulty, 0.0f);
    }
}

void NeuromodulatorySystem::configureAll(float baselineLevel, float modulationStrength) {
    // Set baseline levels for all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->setLevel(baselineLevel * 0.5f);
    if (pImpl->acetylcholine) pImpl->acetylcholine->setLevel(baselineLevel * 0.7f);
    if (pImpl->norepinephrine) pImpl->norepinephrine->setLevel(baselineLevel * 0.6f);
    if (pImpl->serotonin) pImpl->serotonin->setLevel(baselineLevel * 0.4f);
    
    // Set modulation strength parameters
    pImpl->dopamineWeight = baselineLevel * modulationStrength;
    pImpl->achWeight = baselineLevel * modulationStrength * 1.2f;
    pImpl->neWeight = baselineLevel * modulationStrength * 0.9f;
    pImpl->fiveHTWeight = baselineLevel * modulationStrength * 0.7f;
}

void NeuromodulatorySystem::resetAll() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    // Reset other neuromodulators similarly
}

float NeuromodulatorySystem::getNetworkExcitability() const {
    return pImpl->currentExcitability;
}

float NeuromodulatorySystem::getNetworkStability() const {
    float stability = 1.0f / (1.0f + pImpl->currentExcitability * 0.5f);
    return stability;
}

float NeuromodulatorySystem::getLearningReadiness() const {
    float readiness = 1.0f;
    if (pImpl->dopamine) readiness *= pImpl->dopamine->getLearningRate();
    if (pImpl->acetylcholine) readiness *= pImpl->acetylcholine->getAttentionLevel();
    if (pImpl->serotonin) readiness *= pImpl->serotonin->getMoodLevel();
    return readiness;
}

} // namespace nlm

