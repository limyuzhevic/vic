#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Enhanced curiosity system based on exploration-exploitation trade-off
    // Combines active inference models with prediction error learning
    // Implements intelligent exploration based on uncertainty and novelty
    
    float dtf = static_cast<float>(dt);
    
    // Compute composite curiosity signal using precision-weighted prediction errors
    // Active inference: minimize free energy through novel experiences
    float weightedNovelty = pImpl->noveltyWeight * novelty;
    float weightedPredictionError = pImpl->predictionErrorWeight * predictionError;
    
    // Precision-weighted prediction error (Bayesian approach)
    float predictionErrorPrecision = 1.0f / (1.0f + std::abs(predictionError));
    float precisionWeightedPE = predictionErrorPrecision * weightedPredictionError;
    
    // Novelty detection with temporal smoothing
    float noveltyFilter = 0.95f; // Temporal smoothing
    float target = noveltyFilter * (weightedNovelty + precisionWeightedPE);
    
    // Adaptive learning rate for curiosity
    float learningRate = 0.2f * (1.0f + getLevel()); // Higher curiosity = faster learning
    
    // Smooth update with biological plausibility
    pImpl->level += (target - pImpl->level) * learningRate;
    
    // Biological decay mechanism (consumption of exploration energy)
    float decayAmount = pImpl->decayRate * dtf;
    pImpl->level = std::max(0.0f, pImpl->level - decayAmount);
    
    // Intrinsic motivation bonus for high uncertainty
    float uncertainty = std::sqrt(novelty * predictionError);
    if (uncertainty > 0.5f) {
        pImpl->level += uncertainty * 0.1f; // Spontaneous exploration drive
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(2.0f, weight)); // Constrain to reasonable range
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(2.0f, weight)); // Constrain to reasonable range
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
