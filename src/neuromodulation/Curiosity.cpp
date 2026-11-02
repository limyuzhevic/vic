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
    
    // Enhanced curiosity parameters
    float explorationHistory[20];     // Rolling window of exploration levels
    size_t historyIndex;
    float learningRate;               // For curiosity learning
    float uncertainty;                // For epistemic uncertainty
    float persistence;                // For behavioral persistence
    float socialCuriosityFactor;      // For social learning
    float habituationRate;            // For curiosity decay
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , historyIndex(0)
        , learningRate(0.01f)
        , uncertainty(0.5f)
        , persistence(0.8f)
        , socialCuriosityFactor(0.3f)
        , habituationRate(0.001f) {
        // Initialize exploration history
        for (int i = 0; i < 20; ++i) {
            explorationHistory[i] = 0.0f;
        }
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Enhanced Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Enhanced curiosity update with adaptive weights
    float timeFactor = static_cast<float>(dt) * 0.001f;
    
    // Adaptive weighting based on environmental statistics
    float avgNovelty = getAverageNovelty();
    float avgPredictionError = getAveragePredictionError();
    
    pImpl->noveltyWeight = std::clamp(0.5f + avgNovelty * 0.5f, 0.1f, 1.0f);
    pImpl->predictionErrorWeight = std::clamp(0.5f + avgPredictionError * 0.5f, 0.1f, 1.0f);
    
    // Target curiosity level based on weighted inputs
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Enhanced learning dynamics with persistence
    float learningRate = pImpl->learningRate * (1.0f + pImpl->uncertainty * 0.5f);
    float persistenceFactor = std::pow(pImpl->persistence, timeFactor);
    
    // Smooth update with momentum
    pImpl->level = pImpl->level * persistenceFactor + 
                   (target - pImpl->level) * (1.0f - persistenceFactor) * learningRate;
    
    // Bounded to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Decay with habituation
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * timeFactor * pImpl->habituationRate);
    
    // Record exploration level
    pImpl->explorationHistory[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    
    // Update uncertainty based on prediction error
    pImpl->uncertainty = std::clamp(pImpl->uncertainty + std::abs(predictionError) * 0.1f - 
                                   0.05f * timeFactor, 0.1f, 1.0f);
}

float Curiosity::getExplorationDrive() const {
    // Enhanced exploration drive with social curiosity component
    float baseDrive = pImpl->level;
    float socialComponent = getSocialCuriosity() * pImpl->socialCuriosityFactor;
    
    // Combine with persistence and uncertainty
    float enhancedDrive = baseDrive * (1.0f + socialComponent) * 
                         (1.0f + pImpl->uncertainty * 0.2f) * 
                         pImpl->persistence;
    
    return std::clamp(enhancedDrive, 0.0f, 1.0f);
}

float Curiosity::getAverageNovelty() const {
    // Calculate average novelty from exploration history
    float total = 0.0f;
    for (size_t i = 0; i < 20; ++i) {
        total += pImpl->explorationHistory[i];
    }
    return total / 20.0f;
}

float Curiosity::getAveragePredictionError() const {
    if (!pImpl->brain) return 0.0f;
    
    // This would be more sophisticated in a full implementation
    // For now, return a reasonable estimate based on current curiosity level
    return pImpl->level * 0.3f;
}

float Curiosity::getSocialCuriosity() const {
    // Calculate social curiosity based on observation count and memory
    if (!pImpl->brain) return 0.0f;
    
    // This would integrate with social learning systems
    // For now, return a base value
    return 0.2f;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

void Curiosity::setPersistence(float persistence) {
    pImpl->persistence = std::clamp(persistence, 0.5f, 0.99f);
}

void Curiosity::setSocialCuriosityFactor(float factor) {
    pImpl->socialCuriosityFactor = std::clamp(factor, 0.0f, 1.0f);
}

void Curiosity::setHabituationRate(float rate) {
    pImpl->habituationRate = std::clamp(rate, 0.0001f, 0.01f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->learningRate = 0.01f;
    pImpl->uncertainty = 0.5f;
    pImpl->persistence = 0.8f;
    
    // Clear exploration history
    for (int i = 0; i < 20; ++i) {
        pImpl->explorationHistory[i] = 0.0f;
    }
}

} // namespace nlm
