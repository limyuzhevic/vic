#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    // Real curiosity dynamics parameters
    float baselineLevel;
    float explorationDrive;
    float uncertaintySensitivity;
    float attentionModulation;
    float plasticityEnhancement;
    
    // History tracking for learning
    std::vector<float> curiosityHistory;
    std::vector<float> explorationHistory;
    std::vector<std::string> explorationTargets;
    
    // Random exploration parameters
    float explorationRate;
    float stochasticity;
    float goalDiversity;
    
    // Integration with other systems
    float memoryInfluence;
    float attentionInfluence;
    float plasticityInfluence;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , baselineLevel(0.1f)
        , explorationDrive(0.0f)
        , uncertaintySensitivity(0.5f)
        , attentionModulation(0.3f)
        , plasticityEnhancement(0.2f)
        , explorationRate(0.3f)
        , stochasticity(0.2f)
        , goalDiversity(0.4f)
        , memoryInfluence(0.3f)
        , attentionInfluence(0.4f)
        , plasticityInfluence(0.3f)
    {
        curiosityHistory.reserve(100);
        explorationHistory.reserve(100);
        explorationTargets.reserve(50);
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {
    NLM_LOG_INFO("Curiosity system initialized with real exploration dynamics");
}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with brain integration");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Compute curiosity based on novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Apply non-linear integration (like real neural systems)
    // Novelty detection uses sigmoidal response
    target = 1.0f / (1.0f + std::exp(-3.0f * (target - 0.5f)));
    
    // Smooth update with inertia
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Add baseline curiosity for spontaneous exploration
    pImpl->level = std::max(pImpl->level, pImpl->baselineLevel);
    
    // Decay toward baseline
    pImpl->level = std::max(pImpl->baselineLevel, 
                            pImpl->level - pImpl->decayRate * timeStep);
    
    // Compute exploration drive (level + stochastic component)
    float stochastic = (pImpl->brain ? pImpl->brain->getRandomGenerator()->generateFloat(0.0f, 1.0f) : 0.5f) - 0.5f;
    pImpl->explorationDrive = pImpl->level + stochastic * pImpl->stochasticity;
    pImpl->explorationDrive = std::clamp(pImpl->explorationDrive, 0.0f, 1.0f);
    
    // Maintain history
    pImpl->curiosityHistory.push_back(pImpl->level);
    pImpl->explorationHistory.push_back(pImpl->explorationDrive);
    
    if (pImpl->curiosityHistory.size() > 100) {
        pImpl->curiosityHistory.erase(pImpl->curiosityHistory.begin());
        pImpl->explorationHistory.erase(pImpl->explorationHistory.begin());
    }
    
    // Log for debugging
    if (pImpl->level > 0.5f) {
        NLM_LOG_DEBUG("Curiosity elevated: novelty={}, pred_error={}, level={}", 
                     novelty, predictionError, pImpl->level);
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->explorationDrive = 0.0f;
    pImpl->curiosityHistory.clear();
    pImpl->explorationHistory.clear();
    pImpl->explorationTargets.clear();
}

// New methods for real curiosity-driven behavior
void Curiosity::setUncertaintySensitivity(float sensitivity) {
    pImpl->uncertaintySensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

void Curiosity::setGoalDiversity(float diversity) {
    pImpl->goalDiversity = std::clamp(diversity, 0.0f, 1.0f);
}

void Curiosity::addExplorationTarget(const std::string& target) {
    pImpl->explorationTargets.push_back(target);
    if (pImpl->explorationTargets.size() > 50) {
        pImpl->explorationTargets.erase(pImpl->explorationTargets.begin());
    }
}

const std::vector<std::string>& Curiosity::getExplorationTargets() const {
    return pImpl->explorationTargets;
}

float Curiosity::getUncertaintySensitivity() const {
    return pImpl->uncertaintySensitivity;
}

float Curiosity::getGoalDiversity() const {
    return pImpl->goalDiversity;
}

void Curiosity::updateAttentionModulation(float &attentionGain) {
    // Curiosity enhances attentional breadth and flexibility
    float currentLevel = getLevel();
    
    // At low curiosity, attention is focused
    if (currentLevel < 0.3f) {
        attentionGain *= 0.7f;
    }
    // At medium curiosity, attention is balanced
    else if (currentLevel < 0.7f) {
        attentionGain *= 0.9f;
    }
    // At high curiosity, attention is broad but selective
    else {
        attentionGain *= 1.3f;
    }
    
    // Add stochastic component for exploratory attention
    float stochastic = (pImpl->brain ? pImpl->brain->getRandomGenerator()->generateFloat(0.0f, 1.0f) : 0.5f) - 0.5f;
    attentionGain += stochastic * 0.2f * pImpl->attentionModulation;
    
    attentionGain = std::clamp(attentionGain, 0.1f, 2.0f);
}

void Curiosity::updatePlasticity(float &plasticityFactor) {
    // Curiosity enhances neural plasticity
    float currentLevel = getLevel();
    
    // Exponential relationship with curiosity
    plasticityFactor *= (1.0f + currentLevel * pImpl->plasticityEnhancement);
    
    // Higher curiosity leads to more rapid learning
    if (currentLevel > 0.5f) {
        plasticityFactor *= (1.0f + (currentLevel - 0.5f) * 0.5f);
    }
}

void Curiosity::updateMemory(float &memoryWeight) {
    // Curiosity influences memory encoding and consolidation
    float currentLevel = getLevel();
    
    // Moderate curiosity optimizes memory encoding
    if (currentLevel > 0.3f && currentLevel < 0.8f) {
        memoryWeight = 1.2f;  // Enhanced memory encoding
    } else {
        memoryWeight = 1.0f;  // Baseline memory weight
    }
}

} // namespace nlm
