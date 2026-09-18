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
    float learningRate;
    
    // Integration with dopamine system
    float dopamineLevel;
    
    // Temporal dynamics
    float noveltyHistory[3];
    float predictionErrorHistory[3];
    
    // State tracking
    float lastPredictionError;
    float lastNovelty;
    SimulationStep lastUpdateStep;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , dopamineLevel(0.0f)
        , lastPredictionError(0.0f)
        , lastNovelty(0.0f)
        , lastUpdateStep(0)
    {
        for (int i = 0; i < 3; ++i) {
            noveltyHistory[i] = 0.0f;
            predictionErrorHistory[i] = 0.0f;
        }
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize with brain reference for better integration
    if (brain) {
        // Connect to dopamine system for better curiosity regulation
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            // Future enhancement: add callback for dopamine level changes
        }
    }
    
    NLM_LOG_INFO("Curiosity system initialized with prediction error integration");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Store previous state for temporal dynamics
    pImpl->lastNovelty = pImpl->noveltyHistory[0];
    pImpl->lastPredictionError = pImpl->predictionErrorHistory[0];
    
    // Update history (shift and add new values)
    for (int i = 2; i >= 0; --i) {
        pImpl->noveltyHistory[i] = pImpl->noveltyHistory[i-1];
        pImpl->predictionErrorHistory[i] = pImpl->predictionErrorHistory[i-1];
    }
    pImpl->noveltyHistory[0] = novelty;
    pImpl->predictionErrorHistory[0] = predictionError;
    
    // Compute weighted curiosity signal
    float weightedNovelty = 0.0f;
    float weightedPredictionError = 0.0f;
    
    // Apply temporal weighting - more recent events have more influence
    for (int i = 0; i < 3; ++i) {
        float weight = 1.0f / (i + 1);  // 1.0, 0.5, 0.33
        weightedNovelty += pImpl->noveltyHistory[i] * weight;
        weightedPredictionError += pImpl->predictionErrorHistory[i] * weight;
    }
    
    // Scale weights
    weightedNovelty /= 1.833f;  // Sum of weights
    weightedPredictionError /= 1.833f;
    
    // Enhanced curiosity computation
    float target = pImpl->noveltyWeight * weightedNovelty + 
                   pImpl->predictionErrorWeight * weightedPredictionError;
    
    // Learning rate modulated by dopamine for adaptive curiosity
    float effectiveLearningRate = pImpl->learningRate;
    if (pImpl->brain) {
        auto* dopamine = pImpl->brain->getDopamine();
        if (dopamine) {
            // Dopamine modulates learning rate - optimal at moderate levels
            float dopamineInfluence = std::abs(dopamine->getLevel());
            float optimalDopamine = 0.5f;  // Mid-range dopamine is optimal
            float dopamineEffect = std::exp(-std::pow(dopamineInfluence - optimalDopamine, 2.0f) / 0.1f);
            effectiveLearningRate = pImpl->learningRate * dopamineEffect;
        }
    }
    
    // Smooth update with temporal integration
    float delta = (target - pImpl->level) * effectiveLearningRate;
    
    // Time-dependent modulation - curiosity builds up over time
    SimulationStep timeSinceUpdate = static_cast<SimulationStep>(dt) - pImpl->lastUpdateStep;
    if (timeSinceUpdate > 0) {
        // More time allows for greater learning
        delta *= std::min(1.0f, static_cast<float>(timeSinceUpdate) / 10.0f);
    }
    
    pImpl->level += delta;
    
    // Decay with temporal dynamics
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Cap maximum curiosity level
    pImpl->level = std::min(pImpl->level, 1.0f);
    
    // Log significant curiosity changes
    if (std::abs(delta) > 0.1f) {
        NLM_LOG_INFO("Curiosity update: " + std::to_string(pImpl->level) + 
                    " (novelty: " + std::to_string(weightedNovelty) + 
                    ", pred_error: " + std::to_string(weightedPredictionError) + ")");
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
    // Renormalize to keep total weight balanced
    float total = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    pImpl->noveltyWeight /= total;
    pImpl->predictionErrorWeight /= total;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
    // Renormalize to keep total weight balanced
    float total = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    pImpl->noveltyWeight /= total;
    pImpl->predictionErrorWeight /= total;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    
    // Reset history
    for (int i = 0; i < 3; ++i) {
        pImpl->noveltyHistory[i] = 0.0f;
        pImpl->predictionErrorHistory[i] = 0.0f;
    }
    
    pImpl->lastPredictionError = 0.0f;
    pImpl->lastNovelty = 0.0f;
    pImpl->lastUpdateStep = 0;
}

} // namespace nlm