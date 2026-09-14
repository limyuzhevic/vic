#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Oxytocin.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Initialize acetylcholine implementation with proper implementation
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float releaseRate;
    float decayRate;
    float diffusionRate;
    float reuptakeRate;
    float workingMemoryGain;
    float attentionGain;
    float cholinergicReceptorDensity;
    float temporalHalfLife;
    float acetylcholineHistory[10];  // Rolling window for history
    int historyIndex;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , releaseRate(1.5f)
        , decayRate(0.15f)
        , diffusionRate(0.2f)
        , reuptakeRate(0.25f)
        , workingMemoryGain(0.8f)
        , attentionGain(0.9f)
        , cholinergicReceptorDensity(1.0f)
        , temporalHalfLife(20.0f)
        , historyIndex(0) {
        for (int i = 0; i < 10; ++i) {
            acetylcholineHistory[i] = 0.0f;
        }
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized for attention and working memory");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    // Update history for temporal dynamics analysis
    pImpl->acetylcholineHistory[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related learning
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Diffusion - spreading to working memory systems
    if (pImpl->brain && pImpl->diffusionRate > 0.0f) {
        if (pImpl->brain->getWorkingMemory()) {
            float wmEffect = pImpl->level * pImpl->workingMemoryGain;
            pImpl->brain->getWorkingMemory()->setAChLevel(wmEffect);
        }
    }
    
    // Reuptake - clearing from synaptic cleft
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->reuptakeRate * timeStep);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
    
    // Apply attention modulation to neural excitability
    if (pImpl->brain && pImpl->attentionGain > 0.0f) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // ACh modulates attention by adjusting membrane potential thresholds
                    float attentionMod = pImpl->level * pImpl->attentionGain * 0.3f;
                    if (attentionMod > 0.0f) {
                        neuron->adjustAttention(attentionMod);
                    }
                }
            }
        }
    }
}

void Acetylcholine::signalAttention(float attentionLevel) {
    float releaseAmount = attentionLevel * pImpl->releaseRate;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    NLM_LOG_INFO("Acetylcholine released: " + std::to_string(getLevel()) + " (attention: " + std::to_string(attentionLevel) + ")");
}

float Acetylcholine::getWorkingMemoryGain() const {
    return pImpl->workingMemoryGain;
}

void Acetylcholine::setWorkingMemoryGain(float gain) {
    pImpl->workingMemoryGain = gain;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = gain;
}

// Initialize norepinephrine implementation with complete functionality
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float releaseRate;
    float decayRate;
    float diffusionRate;
    float reuptakeRate;
    float arousalGain;
    float signalDetectionGain;
    float locusCoeruleusDensity;
    float temporalHalfLife;
    float noradrenalineHistory[10];
    int historyIndex;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , releaseRate(1.8f)
        , decayRate(0.12f)
        , diffusionRate(0.25f)
        , reuptakeRate(0.35f)
        , arousalGain(0.85f)
        , signalDetectionGain(0.8f)
        , locusCoeruleusDensity(1.0f)
        , temporalHalfLife(15.0f)
        , historyIndex(0) {
        for (int i = 0; i < 10; ++i) {
            noradrenalineHistory[i] = 0.0f;
        }
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized for arousal and vigilance");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    pImpl->noradrenalineHistory[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal-dependent learning
    return 0.2f + 0.8f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Diffusion - spreading to salience networks
    if (pImpl->brain && pImpl->diffusionRate > 0.0f) {
        // NE enhances signal detection in working memory
        if (pImpl->brain->getWorkingMemory()) {
            float sdEffect = pImpl->level * pImpl->signalDetectionGain;
            pImpl->brain->getWorkingMemory()->setArousalLevel(sdEffect);
        }
    }
    
    // Reuptake
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->reuptakeRate * timeStep);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
    
    // Apply arousal modulation to neural excitability
    if (pImpl->brain && pImpl->arousalGain > 0.0f) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE modulates arousal by adjusting membrane potential thresholds
                    float arousalMod = pImpl->level * pImpl->arousalGain * 0.4f;
                    if (arousalMod > 0.0f) {
                        neuron->injectCurrent(arousalMod);
                    }
                }
            }
        }
    }
}

void Norepinephrine::signalArousal(float arousalLevel) {
    float releaseAmount = arousalLevel * pImpl->releaseRate;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    NLM_LOG_INFO("Norepinephrine released: " + std::to_string(getLevel()) + " (arousal: " + std::to_string(arousalLevel) + ")");
}

float Norepinephrine::getArousalGain() const {
    return pImpl->arousalGain;
}

void Norepinephrine::setArousalGain(float gain) {
    pImpl->arousalGain = gain;
}

float Norepinephrine::getSignalDetectionGain() const {
    return pImpl->signalDetectionGain;
}

void Norepinephrine::setSignalDetectionGain(float gain) {
    pImpl->signalDetectionGain = gain;
}

// Initialize serotonin implementation with mood and emotional regulation
struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float releaseRate;
    float decayRate;
    float diffusionRate;
    float reuptakeRate;
    float moodModulationGain;
    float emotionalMemoryGain;
    float rapheNucleiDensity;
    float temporalHalfLife;
    float serotoninHistory[10];
    int historyIndex;
    float emotionalValence;
    float impulsivity;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , releaseRate(1.2f)
        , decayRate(0.18f)
        , diffusionRate(0.3f)
        , reuptakeRate(0.4f)
        , moodModulationGain(0.7f)
        , emotionalMemoryGain(0.6f)
        , rapheNucleiDensity(1.0f)
        , temporalHalfLife(25.0f)
        , emotionalValence(0.5f)
        , impulsivity(0.5f)
        , historyIndex(0) {
        for (int i = 0; i < 10; ++i) {
            serotoninHistory[i] = 0.0f;
        }
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized for mood, emotion, and sleep");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    pImpl->serotoninHistory[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for emotional memory and mood-dependent learning
    float moodFactor = 0.5f + pImpl->emotionalValence * 0.5f;
    return 0.3f + 0.7f * pImpl->level * moodFactor;
}

void Serotonin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Diffusion - spreading to limbic system and mood regulation
    if (pImpl->brain && pImpl->diffusionRate > 0.0f) {
        // 5-HT modulates emotional memory consolidation
        if (pImpl->brain->getEpisodicMemory()) {
            float emEffect = pImpl->level * pImpl->emotionalMemoryGain;
            pImpl->brain->getEpisodicMemory()->setSerotoninModulation(emEffect);
        }
    }
    
    // Reuptake
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->reuptakeRate * timeStep);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
    
    // Apply mood modulation to neural systems
    if (pImpl->brain && pImpl->moodModulationGain > 0.0f) {
        float moodMod = pImpl->level * pImpl->moodModulationGain * 0.3f;
        if (pImpl->brain->getWorkingMemory()) {
            pImpl->brain->getWorkingMemory()->setMoodModulation(moodMod);
        }
        
        // Sleep/wake cycle regulation
        pImpl->brain->updateSleepWakeCycle(pImpl->level);
    }
}

void Serotonin::signalMood(float moodLevel, float valence, float impulsivity) {
    float releaseAmount = moodLevel * pImpl->releaseRate;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Update emotional state
    pImpl->emotionalValence = std::clamp(valence, 0.0f, 1.0f);
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Serotonin released: " + std::to_string(getLevel()) + 
                 " (mood: " + std::to_string(moodLevel) + ", valence: " + std::to_string(pImpl->emotionalValence) + ")");
}

float Serotonin::getEmotionalValence() const {
    return pImpl->emotionalValence;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::setEmotionalValence(float valence) {
    pImpl->emotionalValence = std::clamp(valence, 0.0f, 1.0f);
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
}

// Initialize dopamine implementation with complete reward and prediction error processing
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float workingMemoryGain;
    float predictionErrorIntegration;
    float temporalHalfLife;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , workingMemoryGain(0.5f)
        , predictionErrorIntegration(0.7f)
        , temporalHalfLife(10.0f) {}
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
    // Enhanced dopamine-modulated plasticity with prediction error integration
    float peModulation = 1.0f + pImpl->predictionErrorIntegration * 0.5f;
    return 0.5f + 0.5f * pImpl->level * peModulation;
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
}

void Dopamine::signalReward(float reward) {
    float releaseAmount = reward * pImpl->releaseRate;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Enhance working memory when dopamine increases
    if (pImpl->brain && getLevel() > 0.5f) {
        pImpl->brain->getWorkingMemory()->setDopamineLevel(getLevel() * pImpl->workingMemoryGain);
    }
    
    NLM_LOG_INFO("Dopamine released: " + std::to_string(getLevel()) + " (reward: " + std::to_string(reward) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced prediction error signaling with working memory integration
    float releaseAmount = std::abs(error) * pImpl->releaseRate * pImpl->predictionErrorIntegration;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Update working memory with prediction error information
    if (pImpl->brain && getLevel() > 0.3f) {
        float peModulation = getLevel() * 0.3f;
        pImpl->brain->getWorkingMemory()->setPredictionErrorModulation(peModulation);
    }
    
    NLM_LOG_INFO("Dopamine prediction error: " + std::to_string(getLevel()) + " (error: " + std::to_string(error) + ")");
}

// Initialize curiosity implementation with sophisticated learning-based exploration
struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float noveltyWeight;
    float predictionErrorWeight;
    float memoryIntegrationWeight;
    float temporalDecayRate;
    float refractoryPeriod;
    float explorationBonus;
    float learningRate;
    float confidenceWeight;
    float uncertaintyWeight;
    float temporalHalfLife;
    
    // Time tracking
    float lastUpdateTime;
    float refractoryTimer;
    float timeToNextExploration;
    
    // History for learning
    std::vector<float> predictionErrorHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> learningOutcomeHistory;
    std::vector<float> curiosityHistory;
    
    // Stochastic elements
    std::mt19937 rng;
    std::normal_distribution<float> explorationNoise;
    std::uniform_real_distribution<float> outcomeDistribution;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , noveltyWeight(0.4f)
        , predictionErrorWeight(0.4f)
        , memoryIntegrationWeight(0.2f)
        , temporalDecayRate(0.05f)
        , refractoryPeriod(5.0f)
        , explorationBonus(0.0f)
        , learningRate(0.1f)
        , confidenceWeight(0.3f)
        , uncertaintyWeight(0.7f)
        , temporalHalfLife(10.0f)
        , lastUpdateTime(0.0f)
        , refractoryTimer(0.0f)
        , timeToNextExploration(1.0f)
        , rng(std::random_device{}())
        , explorationNoise(0.0f, 0.1f)
        , outcomeDistribution(0.0f, 1.0f) {
        
        // Reserve space for history
        predictionErrorHistory.reserve(100);
        noveltyHistory.reserve(100);
        learningOutcomeHistory.reserve(100);
        curiosityHistory.reserve(100);
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->lastUpdateTime = 0.0f;
    NLM_LOG_INFO("Curiosity system initialized with sophisticated prediction error integration");
}

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    pImpl->lastUpdateTime = timeStep;
    
    // Check if in refractory period
    if (pImpl->refractoryTimer > 0.0f) {
        pImpl->refractoryTimer -= timeStep;
        if (pImpl->refractoryTimer <= 0.0f) {
            pImpl->refractoryTimer = 0.0f;
        }
    }
    
    // Apply temporal dynamics (exponential decay)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Check if we can increase curiosity
    bool canIncrease = (pImpl->refractoryTimer <= 0.0f) && !std::isnan(novelty) && !std::isnan(predictionError);
    
    if (canIncrease) {
        // Sophisticated curiosity calculation
        float weightedPredictionError = std::abs(predictionError) * pImpl->predictionErrorWeight;
        float weightedNovelty = novelty * pImpl->noveltyWeight;
        
        // Memory integration component - based on recent learning outcomes
        float recentLearningOutcome = 0.0f;
        if (!pImpl->learningOutcomeHistory.empty()) {
            size_t historySize = std::min(size_t(10), pImpl->learningOutcomeHistory.size());
            float totalOutcome = 0.0f;
            for (size_t i = 0; i < historySize; ++i) {
                totalOutcome += pImpl->learningOutcomeHistory[pImpl->learningOutcomeHistory.size() - 1 - i];
            }
            recentLearningOutcome = totalOutcome / historySize;
        }
        
        float memoryIntegration = recentLearningOutcome * pImpl->memoryIntegrationWeight;
        
        // Uncertainty component - higher when prediction errors are inconsistent
        float uncertainty = 0.0f;
        if (!pImpl->predictionErrorHistory.empty()) {
            float errorVariance = 0.0f;
            size_t historySize = std::min(size_t(20), pImpl->predictionErrorHistory.size());
            float meanError = 0.0f;
            for (size_t i = 0; i < historySize; ++i) {
                meanError += std::abs(pImpl->predictionErrorHistory[i]);
            }
            meanError /= historySize;
            
            for (size_t i = 0; i < historySize; ++i) {
                float diff = std::abs(pImpl->predictionErrorHistory[i]) - meanError;
                errorVariance += diff * diff;
            }
            errorVariance /= historySize;
            uncertainty = std::sqrt(errorVariance) * pImpl->uncertaintyWeight;
        }
        
        // Confidence component - based on recent learning success
        float confidence = 0.0f;
        if (!pImpl->learningOutcomeHistory.empty()) {
            size_t historySize = std::min(size_t(20), pImpl->learningOutcomeHistory.size());
            float totalOutcome = 0.0f;
            for (size_t i = 0; i < historySize; ++i) {
                totalOutcome += pImpl->learningOutcomeHistory[pImpl->learningOutcomeHistory.size() - 1 - i];
            }
            confidence = (totalOutcome / historySize) * pImpl->confidenceWeight;
        }
        
        // Combined curiosity drive
        float targetLevel = weightedPredictionError + weightedNovelty + memoryIntegration + uncertainty + (1.0f - confidence);
        targetLevel = std::clamp(targetLevel, 0.0f, pImpl->peak);
        
        // Apply exploration bonus if learning was successful
        if (pImpl->explorationBonus > 0.0f) {
            targetLevel += pImpl->explorationBonus;
        }
        
        // Apply learning rate to smooth update
        float learningIncrement = (targetLevel - pImpl->level) * pImpl->learningRate;
        pImpl->level += learningIncrement;
        
        // Record history
        pImpl->predictionErrorHistory.push_back(predictionError);
        pImpl->noveltyHistory.push_back(novelty);
        pImpl->curiosityHistory.push_back(pImpl->level);
        
        // Maintain history size
        const size_t maxHistorySize = 100;
        if (pImpl->predictionErrorHistory.size() > maxHistorySize) {
            pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
        }
        if (pImpl->noveltyHistory.size() > maxHistorySize) {
            pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        }
        if (pImpl->curiosityHistory.size() > maxHistorySize) {
            pImpl->curiosityHistory.erase(pImpl->curiosityHistory.begin());
        }
        
        // Enter refractory period after significant curiosity change
        if (std::abs(learningIncrement) > 0.1f) {
            pImpl->refractoryTimer = pImpl->refractoryPeriod;
        }
        
        // Apply curiosity effects on brain systems
        applyCuriosityEffects(timeStep);
    }
    
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->temporalDecayRate * timeStep);
}

void Curiosity::applyCuriosityEffects(float timeStep) {
    if (!pImpl->brain) return;
    
    float curiosityLevel = pImpl->level;
    
    // Enhance exploration drive in working memory
    if (pImpl->brain->getWorkingMemory()) {
        // Higher curiosity increases exploration bias
        float explorationBias = curiosityLevel * 0.3f;
        pImpl->brain->getWorkingMemory()->setExplorationBias(explorationBias);
        
        // Store curiosity-enhanced memory traces
        pImpl->brain->getWorkingMemory()->storeCuriosityLevel(curiosityLevel);
    }
    
    // Modulate prediction system sensitivity
    if (pImpl->brain->getPredictionSystem()) {
        // Higher curiosity increases prediction system plasticity
        float predictionSensitivity = curiosityLevel * 0.5f;
        pImpl->brain->getPredictionSystem()->setCuriosityModulation(predictionSensitivity);
    }
    
    // Enhance concept formation
    if (pImpl->brain->getConceptFormation()) {
        // Curiosity promotes novel concept discovery
        float conceptFormationBoost = curiosityLevel * 0.4f;
        pImpl->brain->getConceptFormation()->setCuriosityBoost(conceptFormationBoost);
    }
    
    // Influence action selection (promote exploration)
    if (pImpl->brain->getPlanner()) {
        float planningExploration = curiosityLevel * 0.6f;
        pImpl->brain->getPlanner()->setExplorationDrive(planningExploration);
        
        // Sometimes trigger exploration action even without clear goal
        if (curiosityLevel > 0.7f && pImpl->rng() % 100 < 15) { // 15% chance
            pImpl->timeToNextExploration = 0.0f;
        }
    }
    
    // Store learning outcome for future curiosity updates
    pImpl->learningOutcomeHistory.push_back(calculateLearningOutcome());
    
    // Maintain history size
    const size_t maxHistorySize = 100;
    if (pImpl->learningOutcomeHistory.size() > maxHistorySize) {
        pImpl->learningOutcomeHistory.erase(pImpl->learningOutcomeHistory.begin());
    }
}

float Curiosity::calculateLearningOutcome() const {
    if (pImpl->predictionErrorHistory.empty()) {
        return 0.0f;
    }
    
    // Calculate learning outcome based on prediction error reduction
    float recentError = std::abs(pImpl->predictionErrorHistory.back());
    
    // Compare with historical average
    float historicalAverage = 0.0f;
    size_t historySize = std::min(size_t(20), pImpl->predictionErrorHistory.size());
    if (historySize > 0) {
        for (size_t i = 0; i < historySize; ++i) {
            historicalAverage += std::abs(pImpl->predictionErrorHistory[pImpl->predictionErrorHistory.size() - 1 - i]);
        }
        historicalAverage /= historySize;
    }
    
    // Learning is successful if error is below average
    float outcome = 0.0f;
    if (recentError < historicalAverage) {
        outcome = 1.0f - (recentError / historicalAverage); // Higher when error is much lower
    } else {
        outcome = -0.5f * (recentError / historicalAverage - 1.0f); // Lower when error is higher
    }
    
    return std::clamp(outcome, -1.0f, 1.0f);
}

float Curiosity::getExplorationDrive() const {
    // Exploration drive increases with curiosity level
    return pImpl->level * (1.0f + pImpl->explorationBonus);
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
    // Renormalize weights to sum to 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight + pImpl->memoryIntegrationWeight;
    pImpl->noveltyWeight /= totalWeight;
    pImpl->predictionErrorWeight /= totalWeight;
    pImpl->memoryIntegrationWeight /= totalWeight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
    // Renormalize weights to sum to 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight + pImpl->memoryIntegrationWeight;
    pImpl->noveltyWeight /= totalWeight;
    pImpl->predictionErrorWeight /= totalWeight;
    pImpl->memoryIntegrationWeight /= totalWeight;
}

void Curiosity::setMemoryIntegrationWeight(float weight) {
    pImpl->memoryIntegrationWeight = std::clamp(weight, 0.0f, 1.0f);
    // Renormalize weights to sum to 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight + pImpl->memoryIntegrationWeight;
    pImpl->noveltyWeight /= totalWeight;
    pImpl->predictionErrorWeight /= totalWeight;
    pImpl->memoryIntegrationWeight /= totalWeight;
}

void Curiosity::setTemporalHalfLife(float halfLife) {
    pImpl->temporalHalfLife = std::max(0.1f, halfLife);
}

void Curiosity::setRefractoryPeriod(float period) {
    pImpl->refractoryPeriod = std::max(0.0f, period);
}

void Curiosity::addLearningOutcome(float outcome) {
    pImpl->learningOutcomeHistory.push_back(std::clamp(outcome, -1.0f, 1.0f));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->refractoryTimer = 0.0f;
    pImpl->predictionErrorHistory.clear();
    pImpl->noveltyHistory.clear();
    pImpl->learningOutcomeHistory.clear();
    pImpl->curiosityHistory.clear();
    pImpl->explorationBonus = 0.0f;
}

const std::vector<float>& Curiosity::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

const std::vector<float>& Curiosity::getNoveltyHistory() const {
    return pImpl->noveltyHistory;
}

const std::vector<float>& Curiosity::getCuriosityHistory() const {
    return pImpl->curiosityHistory;
}

const std::vector<float>& Curiosity::getLearningOutcomeHistory() const {
    return pImpl->learningOutcomeHistory;
}

// Initialize novelty implementation with temporal dynamics and adaptive thresholds
struct Novelty::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float recoveryRate;
    float temporalHalfLife;
    float noveltyThreshold;
    float inhibitionRate;
    float spontaneousDecayRate;
    float feedbackGain;
    float patternMemorySize;
    float memoryDecayRate;
    
    // Time tracking
    float lastNoveltyTime;
    float noveltyBurstTimer;
    float habituationCounter;
    float noveltyResetTimer;
    
    // History and memory
    std::vector<float> history;
    std::vector<std::vector<float>> patternMemory;
    std::vector<float> lastPattern;
    std::vector<float> noveltyThresholdHistory;
    
    // Stochastic elements
    std::mt19937 rng;
    std::uniform_real_distribution<float> noveltyNoise;
    std::normal_distribution<float> spontaneousNovelty;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , recoveryRate(0.02f)
        , temporalHalfLife(20.0f)
        , noveltyThreshold(0.3f)
        , inhibitionRate(0.15f)
        , spontaneousDecayRate(0.05f)
        , feedbackGain(0.1f)
        , patternMemorySize(100)
        , memoryDecayRate(0.03f)
        , lastNoveltyTime(0.0f)
        , noveltyBurstTimer(0.0f)
        , habituationCounter(0.0f)
        , noveltyResetTimer(0.0f)
        , rng(std::random_device{}())
        , noveltyNoise(0.0f, 0.1f)
        , spontaneousNovelty(0.0f, 0.02f) {
        patternMemory.reserve(patternMemorySize);
    }
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->lastNoveltyTime = 0.0f;
    NLM_LOG_INFO("Novelty detection initialized with temporal dynamics");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    pImpl->lastNoveltyTime = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0f;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations
    std::vector<float> currentPattern;
    
    // Simple feature extraction from observation
    if (!observation.getData().empty()) {
        currentPattern = observation.getData();
    } else {
        currentPattern = std::vector<float>(10, 0.1f);  // Default pattern
    }
    
    detectNovelty(currentPattern, previousObservation.getData());
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Apply adaptive threshold based on history
    float adaptiveThreshold = pImpl->noveltyThreshold;
    if (!pImpl->noveltyThresholdHistory.empty()) {
        size_t historySize = std::min(size_t(10), pImpl->noveltyThresholdHistory.size());
        float totalThreshold = 0.0f;
        for (size_t i = 0; i < historySize; ++i) {
            totalThreshold += pImpl->noveltyThresholdHistory[historySize - 1 - i];
        }
        adaptiveThreshold = totalThreshold / historySize;
    }
    
    // Update novelty level with temporal dynamics
    float novelty = std::min(1.0f, avgDiff / adaptiveThreshold);
    
    // Apply noise for biological realism
    novelty += pImpl->noveltyNoise(pImpl->rng);
    novelty = std::max(0.0f, std::min(1.0f, novelty));
    
    // Update level with temporal smoothing
    float timeStep = 0.001f;  // Default timestep
    float temporalSmoothing = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level = pImpl->level * temporalSmoothing + novelty * (1.0f - temporalSmoothing);
    
    // Record history
    pImpl->history.push_back(pImpl->level);
    pImpl->noveltyThresholdHistory.push_back(adaptiveThreshold);
    
    // Update pattern memory (store recent patterns)
    pImpl->patternMemory.push_back(currentPattern);
    if (pImpl->patternMemory.size() > pImpl->patternMemorySize) {
        pImpl->patternMemory.erase(pImpl->patternMemory.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Update habituation counter
    pImpl->habituationCounter += novelty * 0.1f;
    
    // Reset novelty threshold if high novelty is detected
    if (novelty > 0.8f) {
        pImpl->noveltyResetTimer = 5.0f;  // Reset after 5 time units
    }
    
    // Apply inhibition to prevent runaway novelty
    if (pImpl->level > 0.9f) {
        pImpl->level *= (1.0f - pImpl->inhibitionRate * timeStep);
    }
    
    NLM_LOG_INFO("Novelty detected: " + std::to_string(pImpl->level) + " (threshold: " + std::to_string(adaptiveThreshold) + ")");
}

void Novelty::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Temporal decay towards baseline
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Spontaneous decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->spontaneousDecayRate * timeStep);
    
    // Recovery towards peak
    if (pImpl->level < 0.5f) {
        pImpl->level += pImpl->recoveryRate * timeStep;
    }
    
    // Novelty burst decay
    if (pImpl->noveltyBurstTimer > 0.0f) {
        pImpl->noveltyBurstTimer -= timeStep;
        if (pImpl->noveltyBurstTimer <= 0.0f) {
            // Apply burst effects
            pImpl->level *= 0.5f;  // Reduce burst effect
        }
    }
    
    // Habituation effect
    pImpl->noveltyThreshold *= (1.0f + pImpl->habituationCounter * 0.01f);
    
    // Novelty reset timer
    if (pImpl->noveltyResetTimer > 0.0f) {
        pImpl->noveltyResetTimer -= timeStep;
        if (pImpl->noveltyResetTimer <= 0.0f) {
            pImpl->noveltyThreshold = 0.3f;  // Reset threshold
            pImpl->habituationCounter = 0.0f;
        }
    }
    
    // Pattern memory decay
    for (size_t i = 0; i < pImpl->patternMemory.size(); ++i) {
        for (float& value : pImpl->patternMemory[i]) {
            value *= (1.0f - pImpl->memoryDecayRate * timeStep);
        }
    }
    
    // Update novelty history
    pImpl->history.push_back(pImpl->level);
    
    // Maintain history size
    const size_t maxHistorySize = 1000;
    if (pImpl->history.size() > maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    if (pImpl->noveltyThresholdHistory.size() > maxHistorySize) {
        pImpl->noveltyThresholdHistory.erase(pImpl->noveltyThresholdHistory.begin());
    }
}

void Novelty::setTemporalHalfLife(float halfLife) {
    pImpl->temporalHalfLife = std::max(0.1f, halfLife);
}

void Novelty::setInhibitionRate(float rate) {
    pImpl->inhibitionRate = std::clamp(rate, 0.0f, 1.0f);
}

void Novelty::setSpontaneousDecayRate(float rate) {
    pImpl->spontaneousDecayRate = std::clamp(rate, 0.0f, 1.0f);
}

void Novelty::setPatternMemorySize(size_t size) {
    pImpl->patternMemorySize = static_cast<float>(std::max(size_t(10), size));
    pImpl->patternMemory.reserve(pImpl->patternMemorySize);
}

void Novelty::setMemoryDecayRate(float rate) {
    pImpl->memoryDecayRate = std::clamp(rate, 0.0f, 0.1f);
}

float Novelty::getTemporalHalfLife() const {
    return pImpl->temporalHalfLife;
}

float Novelty::getInhibitionRate() const {
    return pImpl->inhibitionRate;
}

float Novelty::getSpontaneousDecayRate() const {
    return pImpl->spontaneousDecayRate;
}

float Novelty::getHabituationCounter() const {
    return pImpl->habituationCounter;
}

const std::vector<std::vector<float>>& Novelty::getPatternMemory() const {
    return pImpl->patternMemory;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->noveltyThresholdHistory.clear();
    pImpl->patternMemory.clear();
}

// Initialize oxytocin implementation with social bonding dynamics
struct Oxytocin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float releaseRate;
    float decayRate;
    float diffusionRate;
    float reuptakeRate;
    float socialBondingWeight;
    float trustWeight;
    float memoryIntegrationRate;
    float temporalHalfLife;
    float socialHistoryDecayRate;
    std::vector<float> socialBondHistory;
    float lastSocialEventTime;
    float accumulatedSocialCohesion;
    float trustScore;
    
    // Random generator for stochastic release
    std::mt19937 rng;
    std::uniform_real_distribution<float> releaseDistribution;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , releaseRate(2.0f)
        , decayRate(0.2f)
        , diffusionRate(0.15f)
        , reuptakeRate(0.3f)
        , socialBondingWeight(0.7f)
        , trustWeight(0.3f)
        , memoryIntegrationRate(0.1f)
        , temporalHalfLife(30.0f)
        , socialHistoryDecayRate(0.05f)
        , lastSocialEventTime(0.0f)
        , accumulatedSocialCohesion(0.0f)
        , trustScore(0.5f)
        , rng(std::random_device{}())
        , releaseDistribution(0.0f, 1.0f) {}
};

Oxytocin::Oxytocin() : pImpl(new Impl) {}

Oxytocin::~Oxytocin() = default;

void Oxytocin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Oxytocin system initialized for social bonding");
}

const char* Oxytocin::getName() const {
    return "OT";
}

float Oxytocin::getLevel() const {
    return pImpl->level;
}

void Oxytocin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Oxytocin::getPlasticityFactor() const {
    // Oxytocin enhances social memory plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Oxytocin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay based on half-life)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Diffusion - spreading effect to connected systems
    if (pImpl->brain && pImpl->diffusionRate > 0.0f) {
        // Oxytocin diffuses to influence working memory
        if (pImpl->brain->getWorkingMemory() && pImpl->brain->getWorkingMemory()->getMemoryNeurons().size() > 0) {
            float memoryLevel = pImpl->level * pImpl->diffusionRate;
            pImpl->brain->getWorkingMemory()->setSocialCohesionFactor(memoryLevel);
        }
    }
    
    // Reuptake - clearing from synaptic cleft
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->reuptakeRate * timeStep);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
    
    // Social history decay
    pImpl->socialBondHistory.clear();
    for (size_t i = 0; i < pImpl->socialBondHistory.size(); ++i) {
        pImpl->socialBondHistory[i] *= (1.0f - pImpl->socialHistoryDecayRate * timeStep);
    }
}

void Oxytocin::signalSocialBonding(float socialCohesion) {
    // Stochastic release with social context
    float releaseAmount = socialCohesion * pImpl->releaseRate;
    
    // Add random noise for biological realism
    float noise = pImpl->releaseDistribution(pImpl->rng) * 0.1f;
    releaseAmount += noise;
    
    // Cap at peak level
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Update social history
    pImpl->socialBondHistory.push_back(socialCohesion);
    pImpl->lastSocialEventTime = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0f;
    
    // Integrate with memory systems
    if (pImpl->brain && pImpl->brain->getEpisodicMemory()) {
        pImpl->accumulatedSocialCohesion = (pImpl->accumulatedSocialCohesion * 0.9f) + (socialCohesion * 0.1f);
        
        // Enhance social memory formation
        if (pImpl->accumulatedSocialCohesion > 0.7f) {
            pImpl->brain->getEpisodicMemory()->enhanceSocialMemoryTrace(pImpl->accumulatedSocialCohesion);
        }
    }
    
    NLM_LOG_INFO("Oxytocin released: " + std::to_string(getLevel()) + " (social cohesion: " + std::to_string(socialCohesion) + ")");
}

void Oxytocin::signalTrust(float trustLevel) {
    // Trust signaling affects oxytocin level and social learning
    float trustSignal = trustLevel * pImpl->trustWeight;
    setLevel(std::min(pImpl->peak, getLevel() + trustSignal * 0.5f));
    
    // Update trust score
    pImpl->trustScore = (pImpl->trustScore * 0.9f) + (trustLevel * 0.1f);
    
    // Enhance social learning capacity
    if (pImpl->brain && pImpl->trustScore > 0.6f) {
        pImpl->brain->getWorkingMemory()->setTrustLevel(pImpl->trustScore);
    }
    
    NLM_LOG_INFO("Oxytocin trust modulation: " + std::to_string(getLevel()) + " (trust: " + std::to_string(pImpl->trustScore) + ")");
}

float Oxytocin::getSocialBondHistory(size_t index) const {
    if (index < pImpl->socialBondHistory.size()) {
        return pImpl->socialBondHistory[index];
    }
    return 0.0f;
}

float Oxytocin::getTrustScore() const {
    return pImpl->trustScore;
}

const char* Oxytocin::Impl::getName() const {
    return "OT";
}

} // namespace nlm