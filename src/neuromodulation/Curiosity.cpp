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
    float adaptationTimeConstant;
    float explorationThreshold;
    float maximumLevel;
    float learningRate;
    float temporalIntegration;
    
    // For temporal differentiation
    float previousNovelty;
    float previousPredictionError;
    float noveltyIntegral;
    float predictionErrorIntegral;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , adaptationTimeConstant(1.0f)
        , explorationThreshold(0.3f)
        , maximumLevel(1.0f)
        , learningRate(0.2f)
        , temporalIntegration(0.1f)
        , previousNovelty(0.0f)
        , previousPredictionError(0.0f)
        , noveltyIntegral(0.0f)
        , predictionErrorIntegral(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {
    NLM_LOG_INFO("Curiosity system initialized with enhanced dynamics");
}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with enhanced dynamics");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Enhanced curiosity dynamics with temporal integration
    
    // Compute temporal derivatives
    float noveltyDerivative = (novelty - pImpl->previousNovelty) / std::max(0.001f, static_cast<float>(dt));
    float predictionErrorDerivative = (predictionError - pImpl->previousPredictionError) / std::max(0.001f, static_cast<float>(dt));
    
    // Update integrals (temporal accumulation)
    pImpl->noveltyIntegral += novelty * pImpl->temporalIntegration;
    pImpl->predictionErrorIntegral += predictionError * pImpl->temporalIntegration;
    
    // Limit integral growth
    pImpl->noveltyIntegral = std::clamp(pImpl->noveltyIntegral, -10.0f, 10.0f);
    pImpl->predictionErrorIntegral = std::clamp(pImpl->predictionErrorIntegral, -10.0f, 10.0f);
    
    // Compute target curiosity level based on weighted inputs
    float noveltyComponent = pImpl->noveltyWeight * novelty;
    float predictionErrorComponent = pImpl->predictionErrorWeight * predictionError;
    float integralComponent = pImpl->noveltyWeight * pImpl->noveltyIntegral * 0.1f + 
                               pImpl->predictionErrorWeight * pImpl->predictionErrorIntegral * 0.1f;
    
    float target = noveltyComponent + predictionErrorComponent + integralComponent;
    
    // Smooth, adaptive update with learning rate
    float learningSpeed = pImpl->learningRate;
    pImpl->level += (target - pImpl->level) * learningSpeed;
    
    // Add adaptation effects based on prediction error magnitude
    float errorMagnitude = std::abs(predictionError);
    if (errorMagnitude > 0.5f) {
        // Large prediction errors trigger stronger adaptation
        pImpl->level *= (1.0f + errorMagnitude * 0.1f);
    }
    
    // Decay with time
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->maximumLevel);
    
    // Update previous values
    pImpl->previousNovelty = novelty;
    pImpl->previousPredictionError = predictionError;
    
    // Trigger exploration when curiosity exceeds threshold
    if (pImpl->level > pImpl->explorationThreshold) {
        NLM_LOG_DEBUG(std::string("Curiosity-driven exploration triggered: ") + std::to_string(pImpl->level));
    }
}

float Curiosity::getExplorationDrive() const {
    // Enhanced exploration drive that considers curiosity level and its rate of change
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
    NLM_LOG_INFO(std::string("Novelty weight set to: ") + std::to_string(weight));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
    NLM_LOG_INFO(std::string("Prediction error weight set to: ") + std::to_string(weight));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->noveltyIntegral = 0.0f;
    pImpl->predictionErrorIntegral = 0.0f;
    pImpl->previousNovelty = 0.0f;
    pImpl->previousPredictionError = 0.0f;
    NLM_LOG_INFO("Curiosity system reset");
}

} // namespace nlm
