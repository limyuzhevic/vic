#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

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

} // namespace nlm
