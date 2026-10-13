#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float targetLevel;
    float adaptRate;
    float maxLevel;
    
    // Curiosity state tracking
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    uint32_t historySize;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , targetLevel(0.0f)
        , adaptRate(0.1f)
        , maxLevel(1.0f)
        , historySize(100) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize history buffers
    pImpl->noveltyHistory.reserve(pImpl->historySize);
    pImpl->predictionErrorHistory.reserve(pImpl->historySize);
    
    NLM_LOG_INFO("Curiosity system initialized with brain reference");
    NLM_LOG_DEBUG(std::string("Novelty weight: ") + std::to_string(pImpl->noveltyWeight));
    NLM_LOG_DEBUG(std::string("Prediction error weight: ") + std::to_string(pImpl->predictionErrorWeight));
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Store history for adaptive weighting
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    // Maintain history size
    if (pImpl->noveltyHistory.size() > pImpl->historySize) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Calculate target curiosity level from recent inputs
    float targetNovelty = 0.0f, targetPredictionError = 0.0f;
    if (!pImpl->noveltyHistory.empty()) {
        // Use average of recent history for smoother updates
        float sumNovelty = std::accumulate(pImpl->noveltyHistory.begin(), 
                                         pImpl->noveltyHistory.end(), 0.0f);
        float sumPredictionError = std::accumulate(pImpl->predictionErrorHistory.begin(), 
                                                  pImpl->predictionErrorHistory.end(), 0.0f);
        targetNovelty = sumNovelty / pImpl->noveltyHistory.size();
        targetPredictionError = sumPredictionError / pImpl->predictionErrorHistory.size();
    }
    
    pImpl->targetLevel = pImpl->noveltyWeight * targetNovelty + 
                         pImpl->predictionErrorWeight * targetPredictionError;
    
    // Clamp target to reasonable range
    pImpl->targetLevel = std::clamp(pImpl->targetLevel, 0.0f, pImpl->maxLevel);
    
    // Adaptive learning: smooth transition to target level
    if (std::abs(pImpl->level - pImpl->targetLevel) > 0.01f) {
        float adapt = pImpl->adaptRate * deltaTime;
        pImpl->level += (pImpl->targetLevel - pImpl->level) * adapt;
    }
    
    // Natural decay when not stimulated
    float decay = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Log significant changes
    if (std::abs(novelty) > 0.5f || std::abs(predictionError) > 0.5f) {
        NLM_LOG_DEBUG(std::string("Curiosity update: novelty=") + std::to_string(novelty) +
                     ", pred_error=" + std::to_string(predictionError) +
                     ", level=" + std::to_string(pImpl->level));
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_INFO(std::string("Curiosity novelty weight set to ") + std::to_string(weight));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_INFO(std::string("Curiosity prediction error weight set to ") + std::to_string(weight));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->targetLevel = 0.0f;
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
    NLM_LOG_INFO("Curiosity system reset");
}

} // namespace nlm
