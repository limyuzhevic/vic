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
    float decayRate;
    float explorationBonus;
    float learningRate;
    float temporalDifference;
    float motivationThreshold;
    float attentionBias;
    
    // Internal state for better curiosity dynamics
    float recentNoveltyHistory[5];  // Track recent novelty values
    float predictionErrorHistory[5];  // Track recent prediction errors
    size_t historyIndex;
    
    // Random number generator for exploration
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<float> uniformDist;
    mutable std::normal_distribution<float> normalDist;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.4f)
        , predictionErrorWeight(0.4f)
        , decayRate(0.05f)
        , explorationBonus(0.0f)
        , learningRate(0.01f)
        , temporalDifference(0.0f)
        , motivationThreshold(0.5f)
        , attentionBias(0.5f)
        , historyIndex(0)
        , rng(std::random_device()())
        , uniformDist(0.0f, 1.0f)
        , normalDist(0.0f, 0.1f) {
        // Initialize history arrays
        for (int i = 0; i < 5; ++i) {
            recentNoveltyHistory[i] = 0.0f;
            predictionErrorHistory[i] = 0.0f;
        }
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with enhanced prediction error integration");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Update history buffers (circular buffer)
    pImpl->recentNoveltyHistory[pImpl->historyIndex] = novelty;
    pImpl->predictionErrorHistory[pImpl->historyIndex] = predictionError;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
    
    // Compute moving averages for better dynamics
    float noveltyMA = 0.0f, errorMA = 0.0f;
    for (int i = 0; i < 5; ++i) {
        noveltyMA += pImpl->recentNoveltyHistory[i];
        errorMA += pImpl->predictionErrorHistory[i];
    }
    noveltyMA /= 5.0f;
    errorMA /= 5.0f;
    
    // Enhanced curiosity dynamics with prediction error integration:
    // 1. Novelty-driven curiosity (exploration of unknown)
    // 2. Prediction error-driven curiosity (learning about predictions)
    // 3. Temporal difference for sustained motivation
    
    // Biological model: curiosity as sum of state novelty and prediction error
    float noveltyComponent = noveltyWeight * novelty;
    float errorComponent = predictionErrorWeight * std::abs(predictionError);
    
    // Add temporal integration for sustained motivation
    pImpl->temporalDifference = pImpl->temporalDifference * 0.9f + 
                               (noveltyComponent + errorComponent) * 0.1f * learningRate;
    
    // Smooth update with attention mechanism
    float attention = pImpl->attentionBias + normalDist(pImpl->rng) * 0.1f;
    attention = std::clamp(attention, 0.1f, 1.0f);
    
    float target = pImpl->temporalDifference * attention;
    
    // Enhanced update dynamics with exploration bonus
    float learningMod = 1.0f + pImpl->explorationBonus * 0.5f;
    pImpl->level += (target - pImpl->level) * 0.15f * learningMod;
    
    // Biological noise for exploration
    float noise = normalDist(pImpl->rng) * 0.02f;
    pImpl->level = std::max(0.0f, pImpl->level + noise);
    
    // Decay with exponential time constant
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Update exploration bonus based on novelty and prediction error
    if (novelty > 0.7f && std::abs(predictionError) > 0.5f) {
        // Strong novelty + high prediction error = exploration bonus
        pImpl->explorationBonus = std::min(1.0f, pImpl->explorationBonus + 0.2f * deltaTime);
    }
    
    // Exploration bonus decays naturally
    if (pImpl->explorationBonus > 0.0f) {
        pImpl->explorationBonus *= std::exp(-pImpl->decayRate * deltaTime * 2.0f);
    }
}

float Curiosity::getExplorationDrive() const {
    // Enhanced exploration drive includes both curiosity level and bonus
    float baseDrive = pImpl->level;
    float bonusDrive = pImpl->explorationBonus * 0.5f;
    return baseDrive + bonusDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->temporalDifference = 0.0f;
    // Reset history
    for (int i = 0; i < 5; ++i) {
        pImpl->recentNoveltyHistory[i] = 0.0f;
        pImpl->predictionErrorHistory[i] = 0.0f;
    }
    pImpl->historyIndex = 0;
}

bool Curiosity::shouldExplore() const {
    // Decision criterion for exploration vs exploitation
    return pImpl->level > pImpl->motivationThreshold || 
           pImpl->explorationBonus > 0.3f;
}

float Curiosity::getMotivation() const {
    // Compute overall motivation level
    return pImpl->level * 0.7f + pImpl->explorationBonus * 0.3f;
}

} // namespace nlm
