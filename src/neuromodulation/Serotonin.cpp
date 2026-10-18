#include "Serotonin.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float releaseRate;
    float recoveryRate;
    float homeostasis;
    
    // Mood regulation state
    float moodState;
    float positiveArousal;
    float negativeArousal;
    float emotionalResonance;
    
    // Behavioral inhibition state
    float inhibitionStrength;
    float impulsivityLevel;
    float responseSuppression;
    std::vector<float> inhibitionHistory;
    
    // Social behavior state
    float socialCohesion;
    float socialMotivation;
    float statusConflict;
    std::vector<float> socialInteractionHistory;
    
    // Sleep-wake regulation state
    float circadianPhase;
    float sleepDrive;
    float arousalLevel;
    float circadianRhythm;
    
    // Reward sensitivity state
    float rewardSensitivity;
    float predictionErrorSensitivity;
    float rewardResponse;
    
    // Cognitive flexibility state
    float cognitiveFlexibility;
    float strategySwitching;
    float noveltyDrivenExploration;
    
    // Pain processing state
    float painSensitivity;
    float painInhibition;
    float anxietyLevel;
    
    // Phasic response dynamics
    float phasicStartTime;
    float phasicDuration;
    float phasicPeak;
    float phasicDecayRate;
    bool isPhasicActive;
    
    // Integration state
    float rewardMotivationBalance;
    float stressAnxietyBalance;
    float attentionMoodBalance;
    float explorationInhibitionBalance;
    
    // Homeostatic regulation
    float homeostaticTarget;
    float homeostaticError;
    float adaptationRate;
    std::vector<float> serotoninHistory;
    
    // History tracking
    std::vector<float> moodHistory;
    std::vector<float> inhibitionHistory;
    std::vector<float> socialHistory;
    std::vector<float> sleepHistory;
    std::vector<float> rewardHistory;
    std::vector<float> cognitiveHistory;
    std::vector<float> painHistory;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Statistics
    size_t totalPhasicEvents;
    size_t successfulInhibitions;
    size_t socialModulations;
    size_t sleepRegulations;
    size_t rewardModulations;
    size_t cognitiveEnhancements;
    size_t painModulations;
    
    Impl()
        : brain(nullptr)
        , level(0.02f)              // Baseline 5-HT level (lower than NE/ACh)
        , baselineLevel(0.02f)
        , peakLevel(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , recoveryRate(0.1f)
        , homeostasis(1.0f)
        
        // Mood regulation
        , moodState(0.5f)
        , positiveArousal(0.5f)
        , negativeArousal(0.5f)
        , emotionalResonance(0.5f)
        
        // Behavioral inhibition
        , inhibitionStrength(0.5f)
        , impulsivityLevel(0.5f)
        , responseSuppression(0.5f)
        
        // Social behavior
        , socialCohesion(0.5f)
        , socialMotivation(0.5f)
        , statusConflict(0.5f)
        
        // Sleep-wake
        , circadianPhase(0.0f)
        , sleepDrive(0.5f)
        , arousalLevel(0.5f)
        , circadianRhythm(0.5f)
        
        // Reward sensitivity
        , rewardSensitivity(0.5f)
        , predictionErrorSensitivity(0.5f)
        , rewardResponse(0.5f)
        
        // Cognitive flexibility
        , cognitiveFlexibility(0.5f)
        , strategySwitching(0.5f)
        , noveltyDrivenExploration(0.5f)
        
        // Pain processing
        , painSensitivity(0.5f)
        , painInhibition(0.5f)
        , anxietyLevel(0.5f)
        
        // Phasic response
        , phasicStartTime(0.0f)
        , phasicDuration(60.0f)     // Longer duration than NE (slower dynamics)
        , phasicPeak(0.0f)
        , phasicDecayRate(0.2f)
        , isPhasicActive(false)
        
        // Integration balances
        , rewardMotivationBalance(0.5f)
        , stressAnxietyBalance(0.5f)
        , attentionMoodBalance(0.5f)
        , explorationInhibitionBalance(0.5f)
        
        // Homeostatic
        , homeostaticTarget(0.5f)
        , homeostaticError(0.0f)
        , adaptationRate(0.05f)
        , totalPhasicEvents(0)
        , successfulInhibitions(0)
        , socialModulations(0)
        , sleepRegulations(0)
        , rewardModulations(0)
        , cognitiveEnhancements(0)
        , painModulations(0)
    {
        // Initialize random generator with time-based seed
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 1.0f);
        
        // Initialize history vectors with baseline values
        moodHistory.reserve(1000);
        inhibitionHistory.reserve(1000);
        socialHistory.reserve(1000);
        sleepHistory.reserve(1000);
        rewardHistory.reserve(1000);
        cognitiveHistory.reserve(1000);
        painHistory.reserve(1000);
        serotoninHistory.reserve(1000);
        
        moodHistory.push_back(moodState);
        inhibitionHistory.push_back(inhibitionStrength);
        socialHistory.push_back(socialCohesion);
        sleepHistory.push_back(sleepDrive);
        rewardHistory.push_back(rewardSensitivity);
        cognitiveHistory.push_back(cognitiveFlexibility);
        painHistory.push_back(painSensitivity);
        serotoninHistory.push_back(level);
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin neuromodulation system initialized");
    NLM_LOG_INFO("Baseline 5-HT level: 0.02, Peak: 1.0, Decay rate: 0.05");
    NLM_LOG_INFO("Serotonin system ready for mood, inhibition, and social behavior modulation");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update history
    pImpl->serotoninHistory.push_back(pImpl->level);
    if (pImpl->serotoninHistory.size() > 1000) {
        pImpl->serotoninHistory.erase(pImpl->serotoninHistory.begin());
    }
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for mood-dependent learning and long-term memory
    // Base plasticity factor of 1.0, enhanced by 5-HT for emotional learning
    float basePlasticity = 1.0f;
    float serotoninPlasticity = 0.3f * pImpl->level; // Moderate serotonin influence on plasticity
    return basePlasticity + serotoninPlasticity;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Update phasic responses if active
    if (pImpl->isPhasicActive) {
        float timeSincePhasicStart = pImpl->phasicStartTime;
        if (timeSincePhasicStart < pImpl->phasicDuration) {
            // Phasic response is active - compute current level
            float timeProgress = timeSincePhasicStart / pImpl->phasicDuration;
            float phasicAmplitude = pImpl->phasicPeak * (1.0f - timeProgress);
            
            // Decay during phasic phase
            pImpl->phasicLevel = phasicAmplitude * std::exp(-pImpl->phasicDecayRate * timeSincePhasicStart);
            
            // Current level is phasic plus tonic
            setLevel(pImpl->phasicLevel + pImpl->baselineLevel);
        } else {
            // Phasic response ended - return to tonic level
            pImpl->isPhasicActive = false;
            pImpl->phasicLevel = 0.0f;
            applyTonicLevel(pImpl->baselineLevel);
        }
    } else {
        // No phasic activity - apply decay towards baseline
        applyDecay(pImpl->decayRate, dtFloat);
    }
    
    // Update homeostatic regulation
    homeostaticRegulation(pImpl->serotoninHistory, pImpl->homeostaticTarget);
    
    // Update history for analysis
    pImpl->moodHistory.push_back(pImpl->moodState);
    pImpl->inhibitionHistory.push_back(pImpl->inhibitionStrength);
    pImpl->socialHistory.push_back(pImpl->socialCohesion);
    pImpl->sleepHistory.push_back(pImpl->sleepDrive);
    pImpl->rewardHistory.push_back(pImpl->rewardSensitivity);
    pImpl->cognitiveHistory.push_back(pImpl->cognitiveFlexibility);
    pImpl->painHistory.push_back(pImpl->painSensitivity);
    
    // Trim history if too long
    auto trimHistory = [](auto& hist) {
        if (hist.size() > 1000) {
            hist.erase(hist.begin(), hist.begin() + 100);
        }
    };
    
    trimHistory(pImpl->moodHistory);
    trimHistory(pImpl->inhibitionHistory);
    trimHistory(pImpl->socialHistory);
    trimHistory(pImpl->sleepHistory);
    trimHistory(pImpl->rewardHistory);
    trimHistory(pImpl->cognitiveHistory);
    trimHistory(pImpl->painHistory);
    
    NLM_LOG_DEBUG("Serotonin update: level=" + std::to_string(pImpl->level) + 
                  ", mood=" + std::to_string(pImpl->moodState) + 
                  ", inhibition=" + std::to_string(pImpl->inhibitionStrength) + ", 
                  "social_cohesion=" + std::to_string(pImpl->socialCohesion));
}

void Serotonin::computeMoodState(float rewardInput, float threatInput) {
    // Compute mood state based on reward and threat inputs
    // Integration of positive and negative affective components
    
    // Reward component: positive mood induction
    float rewardComponent = std::tanh(rewardInput * 2.0f - 0.5f); // Normalize to [-1,1]
    
    // Threat component: negative mood induction  
    float threatComponent = std::tanh(-threatInput * 2.0f + 0.5f); // Normalize to [-1,1]
    
    // Integrate components with serotonergic modulation
    float integratedMood = 0.7f * rewardComponent + 0.3f * threatComponent;
    
    // Apply serotonergic modulation - 5-HT promotes positive mood state
    float serotoninModulation = 1.0f + pImpl->level * 0.5f; // Enhanced positive mood at high 5-HT
    
    // Compute final mood state
    pImpl->moodState = (0.5f + 0.5f * integratedMood) * serotoninModulation;
    pImpl->moodState = std::clamp(pImpl->moodState, 0.0f, 1.0f);
    
    // Update emotional arousal components
    pImpl->positiveArousal = pImpl->level * 0.7f + 0.3f * rewardInput;
    pImpl->negativeArousal = pImpl->level * 0.3f + 0.7f * threatInput;
    pImpl->emotionalResonance = 1.0f - std::abs(pImpl->positiveArousal - pImpl->negativeArousal);
    
    NLM_LOG_DEBUG("Serotonin mood computation: reward=" + std::to_string(rewardInput) + 
                  ", threat=" + std::to_string(threatInput) + 
                  ", mood_state=" + std::to_string(pImpl->moodState));
}

void Serotonin::generateBehavioralInhibition(float currentBehaviorStrength, 
                                               std::vector<float>& inhibitionSignals) {
    // Generate behavioral inhibition signal based on current behavior strength
    // Implements response suppression through serotonergic pathways
    
    inhibitionSignals.resize(1); // Single inhibition signal for now
    
    // Compute inhibition based on current behavior and 5-HT level
    // High 5-HT increases behavioral inhibition
    float baseInhibition = 1.0f - currentBehaviorStrength; // More inhibition for stronger behavior
    float serotoninInhibition = pImpl->level * 0.8f; // Strong serotonergic inhibition
    
    // Combine with homeostatic regulation
    float homeostaticInhibition = 1.0f - pImpl->homeostaticError;
    
    // Compute final inhibition signal
    float inhibition = baseInhibition * serotoninInhibition * homeostaticInhibition;
    inhibition = std::clamp(inhibition, 0.0f, 1.0f);
    
    // Update inhibition state
    pImpl->inhibitionStrength = inhibition;
    pImpl->responseSuppression = inhibition;
    pImpl->impulsivityLevel = 1.0f - inhibition; // Inverse relationship
    
    // Record successful inhibition for statistics
    if (inhibition > 0.7f) {
        pImpl->successfulInhibitions++;
        NLM_LOG_DEBUG("Serotonin: Successful behavioral inhibition (strength=" + 
                     std::to_string(inhibition) + ")");
    }
    
    inhibitionSignals[0] = inhibition;
}

void Serotonin::modulateSocialBehavior(float socialCohesion, float statusConflict) {
    // Modulate social behavior based on social interaction history
    // Influences social bonding and cooperative behaviors
    
    // Update social state based on inputs
    pImpl->socialCohesion = (pImpl->socialCohesion * 0.7f) + (socialCohesion * 0.3f);
    pImpl->statusConflict = (pImpl->statusConflict * 0.7f) + (statusConflict * 0.3f);
    
    // Apply serotonergic modulation - high 5-HT promotes social bonding
    float socialModulation = 1.0f + pImpl->level * 0.6f; // Enhanced social motivation at high 5-HT
    
    // Update social motivation
    pImpl->socialMotivation = pImpl->socialCohesion * socialModulation;
    pImpl->socialMotivation = std::clamp(pImpl->socialMotivation, 0.0f, 1.0f);
    
    // Record social modulation for statistics
    pImpl->socialModulations++;
    
    NLM_LOG_DEBUG("Serotonin social modulation: cohesion=" + std::to_string(pImpl->socialCohesion) + 
                  ", conflict=" + std::to_string(pImpl->statusConflict) + 
                  ", motivation=" + std::to_string(pImpl->socialMotivation));
}

void Serotonin::regulateSleepWakeCycle(float timeOfDay, float arousalLevel) {
    // Regulate sleep-wake cycle through circadian rhythm integration
    // Serotonergic system modulates sleep patterns and circadian rhythms
    
    // Update circadian phase based on time of day
    pImpl->circadianPhase = std::fmod(timeOfDay / 24.0f, 1.0f);
    
    // Compute sleep drive based on circadian phase and arousal
    // Serotonergic neurons are most active during wakefulness
    float circadianSleepModulation = 0.5f * (1.0f - std::abs(pImpl->circadianPhase - 0.5f)) * 2.0f;
    
    // Update arousal level input
    pImpl->arousalLevel = arousalLevel;
    
    // Compute sleep drive
    pImpl->sleepDrive = 1.0f - pImpl->arousalLevel * 0.8f - circadianSleepModulation * 0.3f;
    pImpl->sleepDrive = std::clamp(pImpl->sleepDrive, 0.0f, 1.0f);
    
    // Update circadian rhythm with homeostatic regulation
    pImpl->circadianRhythm = (pImpl->circadianRhythm * 0.9f) + (pImpl->sleepDrive * 0.1f);
    
    // Record sleep regulation for statistics
    pImpl->sleepRegulations++;
    
    NLM_LOG_DEBUG("Serotonin sleep regulation: time=" + std::to_string(timeOfDay) + 
                  ", phase=" + std::to_string(pImpl->circadianPhase) + 
                  ", sleep_drive=" + std::to_string(pImpl->sleepDrive));
}

void Serotonin::modulateRewardSensitivity(float predictionError, float predictionConfidence) {
    // Modulate reward sensitivity based on prediction error and confidence
    // Serotonergic system adjusts reward responsiveness based on learning signals
    
    // Compute reward sensitivity based on prediction error
    // High positive prediction error (better than expected) increases sensitivity
    // High negative prediction error (worse than expected) decreases sensitivity
    float predictionErrorModulation = 1.0f + predictionError * 0.5f;
    
    // Confidence modulates sensitivity - high confidence sharpens sensitivity
    float confidenceModulation = 1.0f + predictionConfidence * 0.3f;
    
    // Apply serotonergic baseline modulation
    float serotoninModulation = 1.0f + pImpl->level * 0.4f;
    
    // Compute final reward sensitivity
    pImpl->rewardSensitivity = predictionErrorModulation * confidenceModulation * serotoninModulation;
    pImpl->rewardResponse = std::min(1.0f, pImpl->rewardSensitivity);
    pImpl->predictionErrorSensitivity = predictionErrorModulation;
    
    // Record reward modulation for statistics
    pImpl->rewardModulations++;
    
    NLM_LOG_DEBUG("Serotonin reward modulation: error=" + std::to_string(predictionError) + 
                  ", confidence=" + std::to_string(predictionConfidence) + 
                  ", sensitivity=" + std::to_string(pImpl->rewardSensitivity));
}

void Serotonin::enhanceCognitiveFlexibility(const std::vector<float>& currentStrategy, 
                                           std::vector<float>& strategyAlternatives) {
    // Enhance cognitive flexibility through serotonergic modulation
    // Modulates switching between cognitive strategies
    
    strategyAlternatives.resize(currentStrategy.size());
    
    // Compute strategy switching propensity based on 5-HT level
    // High 5-HT enhances cognitive flexibility and strategy switching
    float switchingPropensity = pImpl->level * 0.7f + 0.3f;
    
    // Generate alternative strategies with serotonergic modulation
    for (size_t i = 0; i < currentStrategy.size(); ++i) {
        // Base alternative: random exploration weighted by switching propensity
        float alternative = currentStrategy[i] * (1.0f - switchingPropensity) +
                           uniformDist(rng) * switchingPropensity;
        
        // Apply cognitive flexibility enhancement
        float enhancedAlternative = alternative * (1.0f + pImpl->cognitiveFlexibility * 0.3f);
        
        strategyAlternatives[i] = std::clamp(enhancedAlternative, 0.0f, 1.0f);
    }
    
    // Update cognitive flexibility state
    pImpl->cognitiveFlexibility = switchingPropensity;
    pImpl->strategySwitching = switchingPropensity;
    pImpl->noveltyDrivenExploration = pImpl->level * 0.5f + 0.2f;
    
    // Record cognitive enhancement for statistics
    pImpl->cognitiveEnhancements++;
    
    NLM_LOG_DEBUG("Serotonin cognitive flexibility: current_strategies=" + 
                  std::to_string(currentStrategy.size()) + 
                  ", alternatives=" + std::to_string(strategyAlternatives.size()) + 
                  ", switching=" + std::to_string(pImpl->strategySwitching));
}

void Serotonin::modulatePainSensitivity(float nociceptiveInput, float anxietyLevel) {
    // Modulate pain sensitivity through serotonergic pathways
    // Influences pain perception and sensitivity
    
    // Update anxiety level input
    pImpl->anxietyLevel = anxietyLevel;
    
    // Compute pain sensitivity modulation
    // Serotonin generally inhibits pain transmission (analgesic effect)
    float serotonergicInhibition = pImpl->level * 0.6f; // Pain inhibition through 5-HT
    
    // Anxiety enhances pain sensitivity
    float anxietyAmplification = 1.0f + anxietyLevel * 0.5f;
    
    // Compute final pain sensitivity
    float baseSensitivity = nociceptiveInput * (1.0f - serotonergicInhibition);
    pImpl->painSensitivity = baseSensitivity * anxietyAmplification;
    pImpl->painSensitivity = std::clamp(pImpl->painSensitivity, 0.0f, 1.0f);
    
    // Compute pain inhibition
    pImpl->painInhibition = serotonergicInhibition * (1.0f - anxietyLevel * 0.3f);
    
    // Record pain modulation for statistics
    pImpl->painModulations++;
    
    NLM_LOG_DEBUG("Serotonin pain modulation: input=" + std::to_string(nociceptiveInput) + 
                  ", anxiety=" + std::to_string(anxietyLevel) + 
                  ", sensitivity=" + std::to_string(pImpl->painSensitivity) + 
                  ", inhibition=" + std::to_string(pImpl->painInhibition));
}

void Serotonin::generatePhasicResponse(const Reward& reward,
                                       const Novelty& novelty,
                                       const PredictionError& predictionError,
                                       const std::vector<float>& socialInput) {
    // Generate phasic response to social and reward events
    // Rapid, transient increases in serotonin signaling for significant events
    
    // Compute event salience from inputs
    float rewardSalience = reward.getLevel();
    float noveltySalience = novelty.getLevel();
    float predictionErrorSalience = predictionError.getLevel();
    
    // Compute average social input salience
    float socialSalience = 0.0f;
    if (!socialInput.empty()) {
        for (float input : socialInput) {
            socialSalience += input;
        }
        socialSalience /= socialInput.size();
    }
    
    // Determine if phasic response should be triggered
    float combinedSalience = rewardSalience * 0.3f + 
                            noveltySalience * 0.3f + 
                            predictionErrorSalience * 0.2f + 
                            socialSalience * 0.2f;
    
    bool shouldTrigger = combinedSalience > 0.4f; // Threshold for phasic response
    
    if (shouldTrigger) {
        pImpl->isPhasicActive = true;
        pImpl->phasicStartTime = 0.0f;
        
        // Compute phasic amplitude based on event properties
        float noveltyContribution = noveltySalience * 0.3f;
        float rewardContribution = rewardSalience * 0.4f;
        float surpriseContribution = predictionErrorSalience * 0.2f;
        float socialContribution = socialSalience * 0.1f;
        
        // Compute phasic amplitude with saturation
        float phasicAmplitude = std::min(1.0f, 
                                        rewardSalience * 1.5f + 
                                        noveltyContribution + 
                                        surpriseContribution + 
                                        socialContribution);
        
        pImpl->phasicPeak = phasicAmplitude;
        
        // Update level with phasic response
        setLevel(pImpl->phasicPeak + pImpl->baselineLevel);
        
        // Increment phasic event counter
        pImpl->totalPhasicEvents++;
        
        NLM_LOG_INFO("Serotonin: Phasic response triggered (salience=" + 
                     std::to_string(combinedSalience) + 
                     ", reward=" + std::to_string(rewardSalience) + 
                     ", novelty=" + std::to_string(noveltySalience) + ", 
                     "prediction_error=" + std::to_string(predictionErrorSalience) + 
                     ", social=" + std::to_string(socialSalience) + ", 
                     "amplitude=" + std::to_string(phasicAmplitude) + ")");
    }
}

void Serotonin::applyStateDependentModulation(Dopamine& dopamine,
                                              Norepinephrine& norepinephrine,
                                              Acetylcholine& acetylcholine,
                                              Novelty& novelty) {
    // Apply state-dependent modulation of other neuromodulators
    // Serotonergic system coordinates with other systems for balanced behavior
    
    float daLevel = dopamine.getLevel();
    float neLevel = norepinephrine.getLevel();
    float achLevel = acetylcholine.getLevel();
    float noveltyLevel = novelty.getLevel();
    
    // Compute reward-motivation balance (Serotonin promotes balance)
    // High 5-HT reduces excessive reward seeking while maintaining motivation
    float serotoninRewardBalance = 1.0f - pImpl->level * 0.3f * (daLevel - 0.5f);
    serotoninRewardBalance = std::clamp(serotoninRewardBalance, 0.3f, 1.0f);
    
    // Compute stress-anxiety balance (Serotonin reduces anxiety)
    // High 5-HT reduces norepinephrine-driven stress responses
    float serotoninStressBalance = 1.0f - pImpl->level * 0.4f * (neLevel - 0.3f);
    serotoninStressBalance = std::clamp(serotoninStressBalance, 0.3f, 1.0f);
    
    // Compute attention-mood balance (Serotonin moderates ACh effects)
    // 5-HT modulates attention based on mood state
    float serotoninAttentionBalance = 0.5f + pImpl->level * 0.3f * (neLevel - 0.5f) -
                                     0.2f * (achLevel - 0.5f);
    serotoninAttentionBalance = std::clamp(serotoninAttentionBalance, 0.2f, 0.8f);
    
    // Compute exploration-inhibition balance (Serotonin modulates novelty)
    // High 5-HT promotes inhibition of novel but potentially dangerous stimuli
    float serotoninExplorationBalance = 1.0f - pImpl->level * 0.5f * noveltyLevel;
    serotoninExplorationBalance = std::clamp(serotoninExplorationBalance, 0.2f, 1.0f);
    
    // Store integration balances for analysis
    pImpl->rewardMotivationBalance = serotoninRewardBalance;
    pImpl->stressAnxietyBalance = serotoninStressBalance;
    pImpl->attentionMoodBalance = serotoninAttentionBalance;
    pImpl->explorationInhibitionBalance = serotoninExplorationBalance;
    
    NLM_LOG_DEBUG("Serotonin state-dependent modulation: "
                  "reward_motivation_balance=" + std::to_string(serotoninRewardBalance) + 
                  ", stress_anxiety_balance=" + std::to_string(serotoninStressBalance) + ", 
                  "attention_mood_balance=" + std::to_string(serotoninAttentionBalance) + 
                  ", exploration_inhibition_balance=" + std::to_string(serotoninExplorationBalance));
}

void Serotonin::homeostaticRegulation(const std::vector<float>& serotoninHistory,
                                      float targetLevel) {
    // Homeostatic regulation to maintain balance
    // 5-HT system adjusts its baseline to maintain optimal level
    
    if (serotoninHistory.size() >= 10) {
        // Compute recent average serotonin level
        float recentAverage = 0.0f;
        size_t count = std::min(size_t(10), serotoninHistory.size());
        
        for (size_t i = 0; i < count; ++i) {
            recentAverage += serotoninHistory[serotoninHistory.size() - 1 - i];
        }
        recentAverage /= count;
        
        // Compute homeostatic error
        pImpl->homeostaticError = std::abs(recentAverage - targetLevel);
        
        // Adapt baseline level to reduce error
        float adaptationAmount = pImpl->adaptationRate * pImpl->homeostaticError;
        
        if (recentAverage < targetLevel) {
            // Need to increase baseline - enhance serotonergic tone
            pImpl->baselineLevel += adaptationAmount;
        } else {
            // Need to decrease baseline - reduce serotonergic tone
            pImpl->baselineLevel -= adaptationAmount;
        }
        
        // Constrain baseline level
        pImpl->baselineLevel = std::clamp(pImpl->baselineLevel, 0.0f, 1.0f);
        
        // Update homeostatic target
        pImpl->homeostaticTarget = targetLevel;
        
        NLM_LOG_DEBUG("Serotonin homeostatic regulation: recent_avg=" + std::to_string(recentAverage) + 
                      ", target=" + std::to_string(targetLevel) + 
                      ", error=" + std::to_string(pImpl->homeostaticError) + 
                      ", baseline_adjustment=" + std::to_string(adaptationAmount));
    }
}

void Serotonin::applyTonicLevel(float baselineLevel) {
    // Apply tonic background level
    pImpl->baselineLevel = baselineLevel;
    
    // Smooth transition to new tonic level
    float targetLevel = pImpl->phasicLevel + pImpl->baselineLevel;
    if (std::abs(pImpl->level - targetLevel) > 0.01f) {
        float transitionAmount = (targetLevel - pImpl->level) * 0.1f;
        pImpl->level += transitionAmount;
    }
}

void Serotonin::applyDecay(float decayRate, TimestepDuration dt) {
    // Decay between phasic events
    float dtFloat = static_cast<float>(dt);
    float decayAmount = decayRate * dtFloat;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineLevel, 
                           pImpl->level - decayAmount);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->phasicLevel = 0.0f;
    pImpl->isPhasicActive = false;
    pImpl->phasicStartTime = 0.0f;
    
    // Reset homeostatic state
    pImpl->homeostaticError = 0.0f;
    pImpl->homeostaticTarget = 0.5f;
    
    // Reset integration balances
    pImpl->rewardMotivationBalance = 0.5f;
    pImpl->stressAnxietyBalance = 0.5f;
    pImpl->attentionMoodBalance = 0.5f;
    pImpl->explorationInhibitionBalance = 0.5f;
    
    // Clear history
    pImpl->serotoninHistory.clear();
    pImpl->moodHistory.clear();
    pImpl->inhibitionHistory.clear();
    pImpl->socialHistory.clear();
    pImpl->sleepHistory.clear();
    pImpl->rewardHistory.clear();
    pImpl->cognitiveHistory.clear();
    pImpl->painHistory.clear();
    
    // Reset statistics
    pImpl->totalPhasicEvents = 0;
    pImpl->successfulInhibitions = 0;
    pImpl->socialModulations = 0;
    pImpl->sleepRegulations = 0;
    pImpl->rewardModulations = 0;
    pImpl->cognitiveEnhancements = 0;
    pImpl->painModulations = 0;
    
    NLM_LOG_INFO("Serotonin neuromodulation system reset");
}

} // namespace nlm
