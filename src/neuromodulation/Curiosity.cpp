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
    float adaptationRate;
    float learningCapacity;
    float explorationBonus;
    float predictionErrorTrace;
    std::vector<float> noveltyHistory;
    std::vector<float> errorHistory;
    float maxExploration;
    SimulationStep lastUpdate;
    
    Impl() : brain(nullptr), level(0.0f), noveltyWeight(0.5f), predictionErrorWeight(0.5f),
             decayRate(0.05f), adaptationRate(0.03f), learningCapacity(1.0f),
             explorationBonus(0.0f), predictionErrorTrace(0.0f), maxExploration(0.0f),
             lastUpdate(0) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {
    NLM_LOG_INFO("Curiosity system enhanced initialized");
}

Curiosity::~Curiosity() {
    NLM_LOG_INFO("Curiosity system enhanced destroyed");
}

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system enhanced initialized with learning capacity %.2f", pImpl->learningCapacity);
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Enhanced curiosity dynamics with traces and learning capacity
    float timeFactor = static_cast<float>(dt) / 100.0f;
    
    // Store history
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->errorHistory.push_back(predictionError);
    
    // Cap history size
    if (pImpl->noveltyHistory.size() > 50) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Compute weighted target with learning capacity
    float rewardNovelty = std::min(1.0f, novelty * pImpl->noveltyWeight);
    float rewardError = std::min(1.0f, std::abs(predictionError) * pImpl->predictionErrorWeight);
    
    float target = rewardNovelty + rewardError;
    
    // Smooth update with exploration bonus
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Adaptation based on history
    if (!pImpl->noveltyHistory.empty()) {
        float avgNovelty = 0.0f;
        float avgError = 0.0f;
        for (size_t i = 0; i < pImpl->noveltyHistory.size(); ++i) {
            avgNovelty += pImpl->noveltyHistory[i];
            avgError += pImpl->errorHistory[i];
        }
        avgNovelty /= pImpl->noveltyHistory.size();
        avgError /= pImpl->errorHistory.size();
        
        // Adjust weights based on environment statistics
        if (avgNovelty > 0.5f) {
            pImpl->noveltyWeight = std::min(1.0f, pImpl->noveltyWeight + 0.01f * timeFactor);
        }
        if (avgError > 0.5f) {
            pImpl->predictionErrorWeight = std::min(1.0f, pImpl->predictionErrorWeight + 0.01f * timeFactor);
        }
    }
    
    // Decay with learning capacity bonus
    float effectiveDecay = pImpl->decayRate / pImpl->learningCapacity;
    pImpl->level = std::max(0.0f, pImpl->level - effectiveDecay * timeFactor);
    
    // Update prediction error trace
    pImpl->predictionErrorTrace = std::max(0.0f, pImpl->predictionErrorTrace - 0.02f * timeFactor);
    if (predictionError != 0.0f) {
        pImpl->predictionErrorTrace = std::min(pImpl->predictionErrorTrace + std::abs(predictionError) * 0.3f, 1.0f);
    }
    
    // Update exploration bonus
    pImpl->explorationBonus = std::min(1.0f, pImpl->explorationBonus + pImpl->level * 0.01f * timeFactor);
    
    // Update max exploration
    pImpl->maxExploration = std::max(pImpl->maxExploration, pImpl->level);
}

float Curiosity::getExplorationDrive() const {
    // Enhanced exploration drive that considers exploration bonus
    float drive = pImpl->level * pImpl->learningCapacity * (1.0f + pImpl->explorationBonus);
    return std::min(1.0f, drive);
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setLearningCapacity(float capacity) {
    pImpl->learningCapacity = std::clamp(capacity, 0.1f, 5.0f);
}

void Curiosity::enhanceLearning(float learningMultiplier) {
    // Enhanced curiosity specifically for learning acceleration
    pImpl->learningCapacity = std::min(pImpl->learningCapacity + learningMultiplier * 0.2f, 5.0f);
    pImpl->level = std::min(pImpl->level + learningMultiplier * 0.3f, 1.0f);
    
    // Stronger exploration drive
    pImpl->explorationBonus = std::min(1.0f, pImpl->explorationBonus + learningMultiplier * 0.4f);
}

float Curiosity::getLearningEfficiency() const {
    // Compute learning efficiency based on exploration and prediction error
    float noveltyEfficiency = pImpl->noveltyWeight * getExplorationDrive();
    float errorEfficiency = pImpl->predictionErrorWeight * pImpl->predictionErrorTrace;
    float totalEfficiency = noveltyEfficiency + errorEfficiency;
    
    return std::min(1.0f, totalEfficiency / 2.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->noveltyWeight = 0.5f;
    pImpl->predictionErrorWeight = 0.5f;
    pImpl->learningCapacity = 1.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->predictionErrorTrace = 0.0f;
    pImpl->noveltyHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->maxExploration = 0.0f;
}

float Curiosity::getMaxExploration() const {
    return pImpl->maxExploration;
}

} // namespace nlm