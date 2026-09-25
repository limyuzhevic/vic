#include "CuriosityManager.hpp"
#include "../core/Constants.hpp"

namespace nlm {

CuriosityManager::CuriosityManager() 
    : curiosityLevel_(0.0f), curiosityEnabled_(true), explorationChance_(0.3f) {}

CuriosityManager::~CuriosityManager() {}

float CuriosityManager::updateCuriosityLevel(float novelty, float predictionError,
                                            float decayFactor, float errorWeight) {
    // Update curiosity based on novelty and prediction error
    curiosityLevel_ = novelty * decayFactor + std::abs(predictionError) * errorWeight;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    return curiosityLevel_;
}

bool CuriosityManager::shouldExplore(float curiosityLevel, float threshold) {
    return curiosityEnabled_ && curiosityLevel > threshold;
}

int CuriosityManager::selectExplorationAction(float curiosityLevel) {
    // Higher curiosity = more exploration
    float exploreChance = curiosityLevel * explorationChance_; // Up to 30% random
    
    float r = curiosityLevel; // Simplified for demo - should use proper random generator
    if (r < exploreChance) {
        // Random motor command selection
        // In real implementation, this would use brain_->getRandomGenerator()
        // For now, return a deterministic value based on curiosity level
        return static_cast<int>(curiosityLevel * 10.0f) % 8;
    }
    return -1; // No exploration
}

} // namespace nlm