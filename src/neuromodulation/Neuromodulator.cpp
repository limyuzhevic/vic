#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Statistics and metrics for neuromodulation system
struct NeuromodulationStats {
    float totalSpikesReceived = 0.0f;
    float totalRewardsReceived = 0.0f;
    float averageRewardPredictionError = 0.0f;
    float averageNoveltyLevel = 0.0f;
    float totalExplorationEvents = 0.0f;
    float totalDecisionPoints = 0.0f;
    float lastUpdateTime = 0.0f;
};

// Dopamine implementation with realistic dynamics
struct Dopamine::Impl {
    // Biological parameters
    float tonicLevel;              // Baseline dopamine level (nM)
    float phasicMax;               // Maximum phasic burst amplitude (nM)
    float predictionErrorGain;     // Scaling factor for prediction error response
    float rewardGain;              // Scaling factor for reward response
    
    // Dynamics
    float currentLevel;            // Current dopamine concentration
    float predictionError;         // Current reward prediction error
    float lastRewardTime;          // Time of last reward
    float eligibilityTrace;        // Eligibility trace for reward-modulated learning
    
    // Temporal dynamics
    float decayTimeConstant;       // Time constant for decay (seconds)
    float riseTimeConstant;         // Time constant for phasic response (seconds)
    float reuptakeRate;            // Rate of dopamine reuptake (1/s)
    
    // Receptor dynamics
    ReceptorDynamics receptor;
    
    // State-dependent modulation
    float behavioralState;         // Current behavioral state (0-1)
    float stressLevel;             // Current stress level (0-1)
    float arousalLevel;            // Current arousal level (0-1)
    
    // Learning modulation
    float learningRateModulation;  // Modulation of learning rate
    float plasticityThreshold;     // Threshold for plasticity induction
    
    // Exploration-exploitation
    float uncertainty;             // Current decision uncertainty
    float explorationBias;         // Bias toward exploration
    float exploitationBias;        // Bias toward exploitation
    
    // Statistics
    NeuromodulationStats stats;
    
    // Random number generator for burst variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> burstNoise;
    std::uniform_real_distribution<float> rewardNoise;
    
    Impl() : 
        tonicLevel(20.0f),              // Normal tonic DA: ~20 nM
        phasicMax(80.0f),              // Typical phasic burst: +60-80 nM
        predictionErrorGain(0.8f),
        rewardGain(0.6f),
        currentLevel(tonicLevel),
        predictionError(0.0f),
        lastRewardTime(0.0f),
        eligibilityTrace(0.0f),
        decayTimeConstant(2.0f),       // ~2 second decay
        riseTimeConstant(0.1f),        // ~100ms rise time
        reuptakeRate(0.5f),
        behavioralState(0.5f),
        stressLevel(0.0f),
        arousalLevel(0.5f),
        learningRateModulation(1.0f),
        plasticityThreshold(0.3f),
        uncertainty(0.5f),
        explorationBias(0.3f),
        exploitationBias(0.7f),
        burstNoise(0.8f, 1.2f),        // 20% variability in burst amplitude
        rewardNoise(0.9f, 1.1f) {} {};
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized with tonic level %f nM", pImpl->tonicLevel);
}

Dopamine::~Dopamine() {
    delete pImpl;
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, tonicLevel + phasicMax);
}

float Dopamine::getPlasticityFactor() const {
    // Realistic dopamine-modulated plasticity: inverted U-shaped function
    float normalizedLevel = pImpl->currentLevel / (pImpl->tonicLevel + pImpl->phasicMax);
    float plasticity = std::exp(-std::pow(normalizedLevel - 0.5f, 2.0f) * 4.0f);
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    const float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    float targetLevel = pImpl->tonicLevel + pImpl->phasicMax * pImpl->behavioralState;
    float decay = (pImpl->currentLevel - targetLevel) / pImpl->decayTimeConstant;
    pImpl->currentLevel += decay * deltaTime;
    
    // Reuptake
    pImpl->currentLevel *= std::exp(-pImpl->reuptakeRate * deltaTime);
    
    // Update eligibility trace
    pImpl->eligibilityTrace *= std::exp(-0.1f * deltaTime); // Decay trace
    
    // Update receptor dynamics
    pImpl->receptor.updateBinding(deltaTime, pImpl->currentLevel);
    pImpl->receptor.applyDesensitization(deltaTime);
    
    // Update statistics
    pImpl->stats.lastUpdateTime = deltaTime;
}

void Dopamine::signalReward(float reward) {
    // Phasic burst response to reward
    float burstAmplitude = pImpl->phasicMax * pImpl->rewardGain * reward;
    burstAmplitude *= rewardNoise(pImpl->rng);
    
    // Add prediction error component
    float predictionErrorResponse = pImpl->predictionErrorGain * pImpl->predictionError;
    
    // Update eligibility trace for reward prediction error learning
    pImpl->eligibilityTrace = std::clamp(
        pImpl->eligibilityTrace + predictionErrorResponse * 0.5f, 0.0f, 1.0f);
    
    // Phasic increase in dopamine
    pImpl->currentLevel += burstAmplitude;
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, 0.0f, 
                                   pImpl->tonicLevel + pImpl->phasicMax);
    
    // Update reward statistics
    pImpl->stats.totalRewardsReceived += reward;
    pImpl->lastRewardTime = 0.0f; // Reset reward timer
    
    NLM_LOG_INFO("Dopamine phasic burst: %.2f nM (reward: %.2f)", burstAmplitude, reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (Rescorla-Wagner style)
    pImpl->predictionError = error;
    
    // Phasic response scaled by prediction error
    float errorResponse = pImpl->predictionErrorGain * error;
    pImpl->currentLevel += errorResponse;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = std::clamp(
        pImpl->eligibilityTrace + errorResponse * 0.3f, 0.0f, 1.0f);
    
    // Update statistics
    pImpl->stats.averageRewardPredictionError = 
        (pImpl->stats.averageRewardPredictionError * 0.9f) + (std::abs(error) * 0.1f);
    
    NLM_LOG_INFO("Dopamine prediction error: %.2f (response: %.2f nM)", error, errorResponse);
}

float Dopamine::getTonicLevel() const {
    return pImpl->tonicLevel;
}

float Dopamine::getPhasicBurst() const {
    return pImpl->currentLevel - pImpl->tonicLevel;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->tonicLevel = baseline;
}

void Dopamine::setPeakLevel(float peak) {
    pImpl->phasicMax = peak;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayTimeConstant = rate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->reuptakeRate = rate;
}

void Dopamine::setReceptorParameters(float affinity, float density) {
    pImpl->receptor.affinity = affinity;
    pImpl->receptor.density = density;
}

const ReceptorDynamics& Dopamine::getReceptorState() const {
    return pImpl->receptor;
}

float Dopamine::getRewardModulatedPlasticity() const {
    // Reward-modulated plasticity factor based on eligibility trace
    return pImpl->eligibilityTrace * getPlasticityFactor();
}

float Dopamine::getLearningRateEnhancement() const {
    // Learning rate enhancement based on dopamine level
    return 1.0f + (pImpl->currentLevel / pImpl->tonicLevel) * 0.5f;
}

// Serotonin implementation
struct Serotonin::Impl {
    // Biological parameters
    float tonicLevel;              // Baseline serotonin level (nM)
    float phasicMax;               // Maximum phasic burst amplitude (nM)
    
    // Learning rate modulation
    float baseLearningRate;        // Base learning rate
    float learningRateVariance;    // Variance in learning rate
    
    // Novelty sensitivity
    float noveltySensitivity;       // Sensitivity to novel stimuli
    float noveltyThreshold;        // Threshold for novelty detection
    
    // Mood state
    float moodState;               // Current mood (0.0 = negative, 1.0 = positive)
    float impulsivity;              // Impulsivity level (0.0 = low, 1.0 = high)
    
    // Temporal dynamics
    float decayTimeConstant;       // Time constant for decay (seconds)
    float recoveryTimeConstant;    // Recovery from phasic dips
    
    // Receptor dynamics
    ReceptorDynamics receptor;
    
    // Statistics
    NeuromodulationStats stats;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> noveltyNoise;
    std::uniform_real_distribution<float> moodNoise;
    
    Impl() : 
        tonicLevel(40.0f),              // Normal tonic 5-HT: ~40 nM
        phasicMax(60.0f),              // Typical phasic bursts
        baseLearningRate(0.1f),
        learningRateVariance(0.05f),
        noveltySensitivity(0.7f),
        noveltyThreshold(0.3f),
        moodState(0.5f),
        impulsivity(0.5f),
        decayTimeConstant(3.0f),
        recoveryTimeConstant(1.0f),
        noveltyNoise(0.9f, 1.1f),
        moodNoise(0.9f, 1.1f) {} {};
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system initialized with tonic level %f nM", pImpl->tonicLevel);
}

Serotonin::~Serotonin() {
    delete pImpl;
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->currentLevel;
}

void Serotonin::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, pImpl->tonicLevel + pImpl->phasicMax);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: high levels suppress, low levels enhance
    float normalizedLevel = pImpl->currentLevel / (pImpl->tonicLevel + pImpl->phasicMax);
    return 1.0f - 0.5f * normalizedLevel; // Inverted relationship
}

void Serotonin::update(TimestepDuration dt) {
    const float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline with mood-dependent recovery
    float moodFactor = 1.0f - pImpl->moodState; // Better mood = faster recovery
    float recoveryRate = moodFactor / pImpl->recoveryTimeConstant;
    
    float targetLevel = pImpl->tonicLevel + pImpl->phasicMax * pImpl->moodState;
    float decay = (pImpl->currentLevel - targetLevel) / pImpl->decayTimeConstant;
    pImpl->currentLevel += decay * deltaTime;
    
    // Add stochastic dips (modeling natural fluctuations)
    if (deltaTime > 0.1f && (rand() % 100) < 5) { // 5% chance per timestep
        pImpl->currentLevel *= (0.7f + 0.3f * pImpl->moodNoise(pImpl->rng));
    }
    
    // Update receptor dynamics
    pImpl->receptor.updateBinding(deltaTime, pImpl->currentLevel);
    pImpl->receptor.applyDesensitization(deltaTime);
    
    // Update novelty detection
    pImpl->noveltyThreshold = 0.3f * (1.0f - pImpl->moodState);
}

void Serotonin::signalNovelty(float novelty) {
    // Phasic increase in response to novelty
    float noveltyResponse = novelty * pImpl->noveltySensitivity;
    noveltyResponse *= pImpl->noveltyNoise(pImpl->rng);
    
    pImpl->currentLevel += noveltyResponse;
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, 0.0f, 
                                   pImpl->tonicLevel + pImpl->phasicMax);
    
    // Update statistics
    pImpl->stats.averageNoveltyLevel = 
        (pImpl->stats.averageNoveltyLevel * 0.9f) + (novelty * 0.1f);
    
    NLM_LOG_INFO("Serotonin novelty response: %.2f (novelty: %.2f)", noveltyResponse, novelty);
}

void Serotonin::signalMoodState(float mood) {
    // Update mood state with noise
    pImpl->moodState = std::clamp(mood, 0.0f, 1.0f);
    
    // Mood affects serotonin dynamics
    if (pImpl->moodState > 0.7f) {
        // Positive mood -> higher baseline
        pImpl->tonicLevel *= 1.1f;
    } else if (pImpl->moodState < 0.3f) {
        // Negative mood -> lower baseline
        pImpl->tonicLevel *= 0.9f;
    }
    
    NLM_LOG_INFO("Serotonin mood state updated: %.2f", mood);
}

void Serotonin::signalLearningRateAdjustment(float adjustment) {
    // Adjust learning rate based on adjustment signal
    pImpl->learningRateVariance = std::abs(adjustment) * 0.1f;
    pImpl->learningRateVariance = std::clamp(pImpl->learningRateVariance, 0.0f, 0.2f);
    
    NLM_LOG_INFO("Serotonin learning rate adjusted: %.2f", adjustment);
}

float Serotonin::getLearningRateModulation() const {
    // Learning rate modulation based on serotonin level
    float normalizedLevel = pImpl->currentLevel / (pImpl->tonicLevel + pImpl->phasicMax);
    float varianceFactor = 1.0f + (pImpl->learningRateVariance * 2.0f);
    
    // Low serotonin enhances learning (approximately)
    return pImpl->baseLearningRate * (1.0f + (1.0f - normalizedLevel)) * varianceFactor;
}

float Serotonin::getTonicSerotonin() const {
    return pImpl->tonicLevel;
}

float Serotonin::getPhasicSerotonin() const {
    return pImpl->currentLevel - pImpl->tonicLevel;
}

void Serotonin::setLearningRateBase(float baseRate) {
    pImpl->baseLearningRate = std::clamp(baseRate, 0.01f, 0.5f);
}

void Serotonin::setLearningRateVariance(float variance) {
    pImpl->learningRateVariance = std::clamp(variance, 0.0f, 0.5f);
}

void Serotonin::setNoveltySensitivity(float sensitivity) {
    pImpl->noveltySensitivity = std::clamp(sensitivity, 0.0f, 2.0f);
}

float Serotonin::getNoveltySensitivity() const {
    return pImpl->noveltySensitivity;
}

const ReceptorDynamics& Serotonin::getReceptorState() const {
    return pImpl->receptor;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    // Biological parameters
    float tonicLevel;              // Baseline ACh level (nM)
    float phasicMax;               // Maximum phasic burst amplitude (nM)
    
    // Attentional parameters
    float attentionFocus;          // Current attentional focus (0.0 = diffuse, 1.0 = focused)
    float detectionThreshold;      // Sensory detection threshold (lower = better detection)
    float workingMemoryLoad;       // Working memory load (0.0 = empty, 1.0 = full)
    
    // Temporal dynamics
    float decayTimeConstant;       // Time constant for decay (seconds)
    float recoveryTimeConstant;    // Recovery rate
    
    // Receptor dynamics
    ReceptorDynamics receptor;
    
    // Memory consolidation
    float consolidationRate;       // Rate of memory consolidation
    float stabilizationTime;       // Time for memory stabilization
    
    // Statistics
    NeuromodulationStats stats;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> focusNoise;
    std::uniform_real_distribution<float> loadNoise;
    
    Impl() : 
        tonicLevel(15.0f),              // Normal tonic ACh: ~15 nM
        phasicMax(45.0f),              // Typical phasic bursts
        attentionFocus(0.5f),
        detectionThreshold(0.3f),
        workingMemoryLoad(0.0f),
        decayTimeConstant(1.5f),
        recoveryTimeConstant(0.5f),
        consolidationRate(0.2f),
        stabilizationTime(5.0f),
        focusNoise(0.9f, 1.1f),
        loadNoise(0.9f, 1.1f) {} {};
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine system initialized with tonic level %f nM", pImpl->tonicLevel);
}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->currentLevel;
}

void Acetylcholine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, pImpl->tonicLevel + pImpl->phasicMax);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for attended stimuli
    float attentionFactor = pImpl->attentionFocus;
    float workingMemoryFactor = 1.0f - pImpl->workingMemoryLoad; // Less load = more plasticity
    
    return 0.8f * attentionFactor * workingMemoryFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    const float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    float targetLevel = pImpl->tonicLevel + pImpl->phasicMax * pImpl->attentionFocus;
    float decay = (pImpl->currentLevel - targetLevel) / pImpl->decayTimeConstant;
    pImpl->currentLevel += decay * deltaTime;
    
    // Update attentional focus based on novelty and task demands
    pImpl->attentionFocus = std::clamp(pImpl->attentionFocus, 0.0f, 1.0f);
    
    // Update working memory load
    pImpl->workingMemoryLoad = std::min(pImpl->workingMemoryLoad + 0.01f * deltaTime, 1.0f);
    
    // Update receptor dynamics
    pImpl->receptor.updateBinding(deltaTime, pImpl->currentLevel);
    pImpl->receptor.applyDesensitization(deltaTime);
}

void Acetylcholine::signalAttentionalFocus(float focusLevel) {
    // Update attentional focus
    pImpl->attentionFocus = std::clamp(focusLevel, 0.0f, 1.0f);
    
    // Phasic increase in response to focused attention
    float focusResponse = pImpl->phasicMax * pImpl->attentionFocus * 0.5f;
    pImpl->currentLevel += focusResponse;
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, 0.0f, 
                                   pImpl->tonicLevel + pImpl->phasicMax);
    
    NLM_LOG_INFO("Acetylcholine attentional focus: %.2f (response: %.2f nM)", focusLevel, focusResponse);
}

void Acetylcholine::signalWorkingMemoryLoad(float load) {
    // Update working memory load
    pImpl->workingMemoryLoad = std::clamp(load, 0.0f, 1.0f);
    
    // Working memory load affects attentional focus
    pImpl->attentionFocus *= (1.0f - pImpl->workingMemoryLoad * 0.5f);
    
    NLM_LOG_INFO("Acetylcholine working memory load: %.2f", load);
}

void Acetylcholine::signalSensoryGain(float gain) {
    // Modulate sensory processing gain
    pImpl->detectionThreshold *= (1.0f - gain * 0.3f); // Higher gain = lower threshold
    pImpl->detectionThreshold = std::max(0.1f, std::min(1.0f, pImpl->detectionThreshold));
    
    NLM_LOG_INFO("Acetylcholine sensory gain: %.2f (threshold: %.2f)", gain, pImpl->detectionThreshold);
}

float Acetylcholine::getAttentionalFocus() const {
    return pImpl->attentionFocus;
}

float Acetylcholine::getSensoryDetectionThreshold() const {
    return pImpl->detectionThreshold;
}

void Acetylcholine::setAttentionThreshold(float threshold) {
    pImpl->detectionThreshold = std::clamp(threshold, 0.1f, 1.0f);
}

float Acetylcholine::getMemoryConsolidationBoost() const {
    // Memory consolidation boost based on ACh level
    float normalizedLevel = pImpl->currentLevel / (pImpl->tonicLevel + pImpl->phasicMax);
    return pImpl->consolidationRate * normalizedLevel;
}

void Acetylcholine::setMemoryConsolidationRate(float rate) {
    pImpl->consolidationRate = std::clamp(rate, 0.0f, 1.0f);
}

float Acetylcholine::getTonicACh() const {
    return pImpl->tonicLevel;
}

float Acetylcholine::getPhasicACh() const {
    return pImpl->currentLevel - pImpl->tonicLevel;
}

const ReceptorDynamics& Acetylcholine::getReceptorState() const {
    return pImpl->receptor;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    // Biological parameters
    float tonicLevel;              // Baseline NE level (nM)
    float phasicMax;               // Maximum phasic burst amplitude (nM)
    
    // Arousal parameters
    float arousalLevel;            // Current arousal (0.0 = calm, 1.0 = high arousal)
    float vigilance;               // Vigilance level (0.0 = low, 1.0 = high)
    
    // Stress response
    float stressLevel;             // Current stress (0.0 = none, 1.0 = high)
    float fightOrFlightActivation; // Fight-or-flight activation (0.0 = calm, 1.0 = activated)
    
    // Temporal dynamics
    float decayTimeConstant;       // Time constant for decay (seconds)
    float recoveryTimeConstant;    // Recovery rate
    
    // Cognitive parameters
    float reactionTimeFactor;      // Modulation of reaction time
    float attentionBroadening;    // Broadening of attentional scope
    
    // Receptor dynamics
    ReceptorDynamics receptor;
    
    // Statistics
    NeuromodulationStats stats;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> arousalNoise;
    std::uniform_real_distribution<float> stressNoise;
    
    Impl() : 
        tonicLevel(10.0f),              // Normal tonic NE: ~10 nM
        phasicMax(50.0f),              // Typical phasic bursts
        arousalLevel(0.5f),
        vigilance(0.5f),
        stressLevel(0.0f),
        fightOrFlightActivation(0.0f),
        decayTimeConstant(1.0f),
        recoveryTimeConstant(0.3f),
        reactionTimeFactor(1.0f),
        attentionBroadening(0.5f),
        arousalNoise(0.9f, 1.1f),
        stressNoise(0.9f, 1.1f) {} {};
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system initialized with tonic level %f nM", pImpl->tonicLevel);
}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->currentLevel;
}

void Norepinephrine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, pImpl->tonicLevel + pImpl->phasicMax);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity: high arousal enhances, extreme stress suppresses
    float arousalFactor = pImpl->arousalLevel;
    float stressFactor = 1.0f - pImpl->stressLevel;
    
    return 0.8f * arousalFactor * stressFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    const float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    float targetLevel = pImpl->tonicLevel + pImpl->phasicMax * pImpl->arousalLevel;
    float decay = (pImpl->currentLevel - targetLevel) / pImpl->decayTimeConstant;
    pImpl->currentLevel += decay * deltaTime;
    
    // Update arousal and vigilance
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel, 0.0f, 1.0f);
    pImpl->vigilance = std::clamp(pImpl->vigilance, 0.0f, 1.0f);
    
    // Update receptor dynamics
    pImpl->receptor.updateBinding(deltaTime, pImpl->currentLevel);
    pImpl->receptor.applyDesensitization(deltaTime);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Update arousal level
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
    
    // Phasic increase in response to high arousal
    float arousalResponse = pImpl->phasicMax * pImpl->arousalLevel * 0.3f;
    pImpl->currentLevel += arousalResponse;
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, 0.0f, 
                                   pImpl->tonicLevel + pImpl->phasicMax);
    
    // Arousal affects vigilance
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.1f * pImpl->arousalLevel);
    
    NLM_LOG_INFO("Norepinephrine arousal: %.2f (response: %.2f nM)", arousalLevel, arousalResponse);
}

void Norepinephrine::signalStressResponse(float stressLevel) {
    // Update stress level
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // High stress triggers fight-or-flight response
    if (pImpl->stressLevel > 0.7f) {
        pImpl->fightOrFlightActivation = std::min(1.0f, pImpl->fightOrFlightActivation + 0.2f);
    }
    
    // Stress affects arousal and vigilance
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + 0.1f * pImpl->stressLevel);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.15f * pImpl->stressLevel);
    
    NLM_LOG_INFO("Norepinephrine stress response: %.2f", stressLevel);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // Update vigilance level
    pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f);
    
    // Vigilance affects arousal
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + 0.05f * pImpl->vigilance);
    
    NLM_LOG_INFO("Norepinephrine vigilance: %.2f", vigilance);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getReactionTimeModulation() const {
    // High NE arousal reduces reaction time (improves speed)
    return 1.0f / (1.0f + pImpl->arousalLevel * 0.5f);
}

void Norepinephrine::setReactionTimeFactor(float factor) {
    pImpl->reactionTimeFactor = std::clamp(factor, 0.5f, 2.0f);
}

float Norepinephrine::getStressResponseLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getFightOrFlightActivation() const {
    return pImpl->fightOrFlightActivation;
}

void Norepinephrine::setStressSensitivity(float sensitivity) {
    // Sensitivity affects how easily stress is triggered
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + sensitivity * 0.1f);
}

float Norepinephrine::getTonicNE() const {
    return pImpl->tonicLevel;
}

float Norepinephrine::getPhasicNE() const {
    return pImpl->currentLevel - pImpl->tonicLevel;
}

const ReceptorDynamics& Norepinephrine::getReceptorState() const {
    return pImpl->receptor;
}

// NeuromodulatorEnsemble implementation
struct NeuromodulatorEnsemble::Impl {
    Brain* brain;                   // Reference to brain for integration
    
    // Individual neuromodulators
    Dopamine dopamine;
    Serotonin serotonin;
    Acetylcholine acetylcholine;
    Norepinephrine norepinephrine;
    
    // Combined signals
    float combinedPlasticityFactor; // Combined effect on plasticity
    float combinedExplorationProbability; // Combined exploration bias
    float combinedDecisionUncertainty;   // Combined uncertainty
    float combinedRewardPredictionError; // Combined prediction error
    float combinedNoveltySignal;    // Combined novelty
    float combinedAttentionFocus;   // Combined attention
    float combinedArousal;         // Combined arousal
    
    // State-dependent modulation
    float behavioralState;         // Current behavioral state
    float taskDifficulty;          // Current task difficulty
    float timeOfDay;               // Time of day effect (circadian)
    
    // Statistics
    NeuromodulationStats stats;
    
    Impl() : 
        brain(nullptr),
        combinedPlasticityFactor(1.0f),
        combinedExplorationProbability(0.3f),
        combinedDecisionUncertainty(0.5f),
        combinedRewardPredictionError(0.0f),
        combinedNoveltySignal(0.0f),
        combinedAttentionFocus(0.5f),
        combinedArousal(0.5f),
        behavioralState(0.5f),
        taskDifficulty(0.5f),
        timeOfDay(12.0f) {} {};
};

NeuromodulatorEnsemble::NeuromodulatorEnsemble() : pImpl(new Impl) {
    NLM_LOG_INFO("Neuromodulator ensemble initialized");
}

NeuromodulatorEnsemble::~NeuromodulatorEnsemble() {
    delete pImpl;
}

void NeuromodulatorEnsemble::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Neuromodulator ensemble integrated with brain");
}

void NeuromodulatorEnsemble::update(TimestepDuration dt) {
    // Update individual neuromodulators
    dopamine.update(dt);
    serotonin.update(dt);
    acetylcholine.update(dt);
    norepinephrine.update(dt);
    
    // Update state-dependent modulation
    updateStateDependent(pImpl->brain);
    
    // Compute combined signals
    combinedPlasticityFactor = 
        0.3f * dopamine.getPlasticityFactor() +
        0.3f * serotonin.getPlasticityFactor() +
        0.2f * acetylcholine.getPlasticityFactor() +
        0.2f * norepinephrine.getPlasticityFactor();
    
    combinedExplorationProbability = 
        0.4f * dopamine.getExplorationBias() +
        0.3f * serotonin.getLevel() * 0.2f +  // High 5-HT = more exploration
        0.3f * acetylcholine.getAttentionalFocus(); // Focused attention = less exploration
    
    combinedDecisionUncertainty = 
        0.5f * (1.0f - dopamine.getLevel()) +  // Low DA = more uncertainty
        0.3f * serotonin.getLevel() +         // High 5-HT = more uncertainty
        0.2f * norepinephrine.getArousal();    // High NE = less uncertainty
    
    combinedRewardPredictionError = 
        dopamine.getRewardPredictionError();
    
    combinedNoveltySignal = 
        dopamine.getLevel() * 0.5f +        // High DA responds to novelty
        serotonin.getNoveltySignal() * 0.3f +
        acetylcholine.getAttentionalFocus() * 0.2f;
    
    combinedAttentionFocus = 
        acetylcholine.getAttentionalFocus() * 0.5f +
        norepinephrine.getArousal() * 0.3f +
        dopamine.getLevel() * 0.2f;
    
    combinedArousal = 
        norepinephrine.getArousal() * 0.6f +
        dopamine.getLevel() * 0.4f;
    
    // Update statistics
    pImpl->stats.lastUpdateTime = dt;
}

void NeuromodulatorEnsemble::signalEvent(EventType type, float value) {
    switch (type) {
        case EventType::Reward:
            dopamine.signalReward(value);
            break;
        case EventType::Punishment:
            dopamine.signalRewardPredictionError(-value);
            break;
        case EventType::Novelty:
            serotonin.signalNovelty(value);
            dopamine.signalRewardPredictionError(value * 0.5f);
            break;
        case EventType::Curiosity:
            // Curiosity is computed externally but affects neuromodulation
            break;
        case EventType::Attention:
            acetylcholine.signalAttentionalFocus(value);
            break;
        case EventType::Stress:
            norepinephrine.signalStressResponse(value);
            break;
        case EventType::Arousal:
            norepinephrine.signalArousal(value);
            break;
        case EventType::PredictionError:
            dopamine.signalRewardPredictionError(value);
            break;
        default:
            break;
    }
}

void NeuromodulatorEnsemble::applyToSynapse(Synapse* synapse) {
    if (!synapse || !pImpl->brain) return;
    
    // Compute combined neuromodulatory effects on synapse
    float plasticityModulation = combinedPlasticityFactor;
    float learningRateMod = 
        dopamine.getLearningRateEnhancement() * 
        serotonin.getLearningRateModulation();
    
    // Apply to synapse
    synapse->setPlasticityFlags(
        0.5f * plasticityModulation,  // Hebbian
        0.3f * plasticityModulation,  // STDP
        learningRateMod,              // Reward-modulated
        0.2f * plasticityModulation    // Structural
    );
    
    // Update eligibility trace
    float eligibilityTrace = 
        0.4f * dopamine.getRewardModulatedPlasticity() +
        0.3f * acetylcholine.getLevel() +
        0.3f * norepinephrine.getArousal();
    
    synapse->setEligibilityTrace(eligibilityTrace);
}

float NeuromodulatorEnsemble::getCombinedPlasticityFactor() const {
    return pImpl->combinedPlasticityFactor;
}

float NeuromodulatorEnsemble::getExplorationProbability() const {
    return pImpl->combinedExplorationProbability;
}

float NeuromodulatorEnsemble::getDecisionUncertainty() const {
    return pImpl->combinedDecisionUncertainty;
}

float NeuromodulatorEnsemble::getDopamineLevel() const {
    return pImpl->dopamine.getLevel();
}

float NeuromodulatorEnsemble::getSerotoninLevel() const {
    return pImpl->serotonin.getLevel();
}

float NeuromodulatorEnsemble::getAChLevel() const {
    return pImpl->acetylcholine.getLevel();
}

float NeuromodulatorEnsemble::getNELevel() const {
    return pImpl->norepinephrine.getLevel();
}

float NeuromodulatorEnsemble::getRewardPredictionError() const {
    return pImpl->combinedRewardPredictionError;
}

float NeuromodulatorEnsemble::getNoveltySignal() const {
    return pImpl->combinedNoveltySignal;
}

float NeuromodulatorEnsemble::getAttentionFocus() const {
    return pImpl->combinedAttentionFocus;
}

float NeuromodulatorEnsemble::getArousalLevel() const {
    return pImpl->combinedArousal;
}

void NeuromodulatorEnsemble::updateStateDependent(Brain* brain) {
    if (!brain) return;
    
    // State-dependent modulation based on behavioral state
    float timeFactor = std::sin(pImpl->timeOfDay / 12.0f * M_PI); // Circadian rhythm
    pImpl->behavioralState = (pImpl->behavioralState + timeFactor * 0.1f) * 0.95f + 0.05f;
    
    // Task difficulty affects neuromodulator release
    pImpl->taskDifficulty = std::clamp(pImpl->taskDifficulty, 0.0f, 1.0f);
    
    // Update neuromodulators based on state
    if (pImpl->behavioralState > 0.8f) {
        // High behavioral state -> more exploratory
        pImpl->dopamine.setLevel(pImpl->dopamine.getLevel() * 1.1f);
        pImpl->serotonin.setLevel(pImpl->serotonin.getLevel() * 0.9f);
    } else if (pImpl->behavioralState < 0.3f) {
        // Low behavioral state -> more conservative
        pImpl->dopamine.setLevel(pImpl->dopamine.getLevel() * 0.9f);
        pImpl->serotonin.setLevel(pImpl->serotonin.getLevel() * 1.1f);
    }
    
    // Update brain state-dependent signals
    if (pImpl->brain) {
        pImpl->dopamine.updateStateDependent(brain);
        pImpl->serotonin.updateStateDependent(brain);
        pImpl->acetylcholine.updateStateDependent(brain);
        pImpl->norepinephrine.updateStateDependent(brain);
    }
}

} // namespace nlm