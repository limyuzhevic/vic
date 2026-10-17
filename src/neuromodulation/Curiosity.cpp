#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float adaptationRate;
    float explorationDrive;
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    Timestamp lastNoveltyTime;
    Timestamp lastPredictionErrorTime;
    float homeostaticLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.6f)
        , predictionErrorWeight(0.4f)
        , decayRate(0.05f)
        , adaptationRate(0.1f)
        , explorationDrive(0.0f)
        , lastNoveltyTime(-1.0f)
        , lastPredictionErrorTime(-1.0f)
        , homeostaticLevel(0.0f) {
        noveltyHistory.reserve(1000);
        predictionErrorHistory.reserve(1000);
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    if (!brain) {
        return;  // Should log error but not crash
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    if (!pImpl->brain) {
        return;
    }
    
    // Store history for analysis
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    if (pImpl->noveltyHistory.size() > 1000) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    pImpl->lastNoveltyTime = std::chrono::system_clock::now();
    pImpl->lastPredictionErrorTime = std::chrono::system_clock::now();
    
    // Target level based on weighted combination of novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with biological time constants
    float timeConstant = 100.0f / pImpl->decayRate;  // ms
    float decayFactor = std::exp(-dt / timeConstant);
    
    // Apply homeostatic regulation
    pImpl->homeostaticLevel = std::max(0.0f, pImpl->homeostaticLevel + 
        pImpl->adaptationRate * (target - pImpl->homeostaticLevel));
    
    // Update level with homeostatic bias
    pImpl->level += (pImpl->homeostaticLevel - pImpl->level) * 0.1f;
    
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level * decayFactor);
    
    // Exploration drive is the current curiosity level
    pImpl->explorationDrive = pImpl->level;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(weight, 1.0f));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(weight, 1.0f));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationDrive = 0.0f;
    pImpl->homeostaticLevel = 0.0f;
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
}

} // namespace nlm
