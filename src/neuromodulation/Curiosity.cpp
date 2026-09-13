#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float explorationWeight;
    float exploitationWeight;
    float temporalMemoryDecay;
    float memoryCapacity;
    float learningRate;
    float maxLevel;
    float minLevel;
    
    // Temporal dynamics
    float lastNovelty;
    float lastPredictionError;
    float temporalIntegration;
    float temporalDerivative;
    
    // Memory integration
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> temporalContext;
    
    // Exploration-exploitation balance
    float explorationThreshold;
    float exploitationBoost;
    float resetCooldown;
    float currentCooldown;
    
    // Random number generator for exploration
    std::mt19937 rng;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.3f)
        , explorationWeight(0.2f)
        , exploitationWeight(0.8f)
        , temporalMemoryDecay(0.95f)
        , memoryCapacity(100)
        , learningRate(0.1f)
        , maxLevel(1.0f)
        , minLevel(0.0f)
        , lastNovelty(0.0f)
        , lastPredictionError(0.0f)
        , temporalIntegration(0.0f)
        , temporalDerivative(0.0f)
        , explorationThreshold(0.5f)
        , exploitationBoost(1.2f)
        , resetCooldown(10.0f)
        , currentCooldown(0.0f)
    {
        std::random_device rd;
        rng.seed(rd());
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
    pImpl->temporalContext.clear();
    
    // Initialize temporal context vector
    pImpl->temporalContext.reserve(pImpl->memoryCapacity);
    
    NLM_LOG_INFO("Advanced Curiosity system initialized with temporal dynamics and memory integration");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Store previous values for temporal dynamics
    pImpl->lastNovelty = pImpl->noveltyHistory.empty() ? 0.0f : pImpl->noveltyHistory.back();
    pImpl->lastPredictionError = pImpl->predictionErrorHistory.empty() ? 0.0f : pImpl->predictionErrorHistory.back();
    
    // Update history
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    // Maintain memory capacity
    if (pImpl->noveltyHistory.size() > pImpl->memoryCapacity) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Compute temporal dynamics
    computeTemporalDynamics(timeStep);
    
    // Calculate weighted curiosity signal
    float noveltyComponent = pImpl->noveltyWeight * novelty;
    float predictionErrorComponent = pImpl->predictionErrorWeight * predictionError;
    float temporalComponent = pImpl->explorationWeight * pImpl->temporalIntegration;
    
    float target = noveltyComponent + predictionErrorComponent + temporalComponent;
    
    // Adaptive learning rate based on current level
    float adaptiveLearningRate = pImpl->learningRate * (1.0f - std::abs(pImpl->level - 0.5f) * 2.0f);
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * adaptiveLearningRate;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, pImpl->minLevel, pImpl->maxLevel);
    
    // Decay temporal integration
    pImpl->temporalIntegration *= pImpl->temporalMemoryDecay;
    
    // Update cooldown
    if (pImpl->currentCooldown > 0.0f) {
        pImpl->currentCooldown -= timeStep;
    }
}

void Curiosity::computeTemporalDynamics(float dt) {
    if (pImpl->noveltyHistory.size() < 2) {
        pImpl->temporalIntegration = 0.0f;
        pImpl->temporalDerivative = 0.0f;
        return;
    }
    
    // Compute temporal derivative (rate of change)
    float currentNovelty = pImpl->noveltyHistory.back();
    float previousNovelty = pImpl->noveltyHistory[pImpl->noveltyHistory.size() - 2];
    pImpl->temporalDerivative = (currentNovelty - previousNovelty) / dt;
    
    // Compute temporal integration (accumulation)
    float currentPredError = pImpl->predictionErrorHistory.back();
    float previousPredError = pImpl->predictionErrorHistory[pImpl->predictionErrorHistory.size() - 2];
    pImpl->temporalIntegration += (currentPredError - previousPredError) * dt;
    
    // Apply exploration-exploitation balance
    if (pImpl->level > pImpl->explorationThreshold && pImpl->currentCooldown <= 0.0f) {
        // High curiosity -> more exploration
        pImpl->temporalIntegration *= pImpl->exploitationBoost;
        pImpl->currentCooldown = pImpl->resetCooldown;
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setExplorationWeight(float weight) {
    pImpl->explorationWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setExploitationWeight(float weight) {
    pImpl->exploitationWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setTemporalMemoryDecay(float decay) {
    pImpl->temporalMemoryDecay = std::clamp(decay, 0.5f, 0.99f);
}

void Curiosity::setMemoryCapacity(size_t capacity) {
    pImpl->memoryCapacity = static_cast<float>(std::max<size_t>(capacity, 10));
}

float Curiosity::getTemporalDerivative() const {
    return pImpl->temporalDerivative;
}

const std::vector<float>& Curiosity::getNoveltyHistory() const {
    return pImpl->noveltyHistory;
}

const std::vector<float>& Curiosity::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

const std::vector<float>& Curiosity::getTemporalContext() const {
    return pImpl->temporalContext;
}

void Curiosity::addTemporalContext(float context) {
    pImpl->temporalContext.push_back(context);
    if (pImpl->temporalContext.size() > pImpl->memoryCapacity) {
        pImpl->temporalContext.erase(pImpl->temporalContext.begin());
    }
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
    pImpl->temporalContext.clear();
    pImpl->currentCooldown = 0.0f;
    NLM_LOG_INFO("Curiosity system reset");
}

} // namespace nlm