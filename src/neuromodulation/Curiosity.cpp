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
    // Enhanced curiosity update with multiple inputs and temporal dynamics
    if (std::isnan(novelty) || std::isinf(novelty)) {
        NLM_LOG_WARNING("Invalid novelty value: " + std::to_string(novelty));
        novelty = 0.0f;
    }
    
    if (std::isnan(predictionError) || std::isinf(predictionError)) {
        NLM_LOG_WARNING("Invalid prediction error value: " + std::to_string(predictionError));
        predictionError = 0.0f;
    }
    
    // Combined curiosity drive from multiple sources
    float noveltyContribution = pImpl->noveltyWeight * std::min(novelty, 1.0f);
    float predictionErrorContribution = pImpl->predictionErrorWeight * std::min(std::abs(predictionError), 1.0f);
    
    float target = noveltyContribution + predictionErrorContribution;
    target = std::clamp(target, 0.0f, 1.0f);
    
    // Smooth temporal dynamics with inertia
    float learningRate = 0.1f * (1.0f + pImpl->level); // Faster when already curious
    float delta = (target - pImpl->level) * learningRate;
    
    // Apply damping to prevent oscillations
    pImpl->level += delta - delta * 0.1f * std::abs(delta);
    
    // Realistic decay with temperature-dependent modulation
    float decayFactor = pImpl->decayRate * static_cast<float>(dt) * (0.5f + pImpl->level * 0.5f);
    pImpl->level = std::max(0.0f, pImpl->level - decayFactor);
    
    // Adaptive inhibition based on environmental stability
    if (novelty < 0.01f && predictionError < 0.01f) {
        // Environment is stable, reduce curiosity to prevent exploration overkill
        float inhibition = 0.05f * static_cast<float>(dt);
        pImpl->level = std::max(0.0f, pImpl->level - inhibition);
    }
    
    NLM_LOG_DEBUG("Curiosity update: novelty=" + std::to_string(novelty) + 
                  " pred_error=" + std::to_string(predictionError) + 
                  " level=" + std::to_string(pImpl->level));
}

void Curiosity::reset() {
    // Comprehensive reset including adaptive parameters
    pImpl->level = 0.0f;
    
    // Occasionally adjust internal parameters for diversity
    if (pImpl->brain) {
        static int resetCount = 0;
        resetCount++;
        
        // Slightly randomize weights every 10th reset for exploration diversity
        if (resetCount % 10 == 0) {
            pImpl->noveltyWeight = 0.5f + (pImpl->rng ? pImpl->rng->getFloat(-0.2f, 0.2f) : 0.0f);
            pImpl->predictionErrorWeight = 0.5f + (pImpl->rng ? pImpl->rng->getFloat(-0.2f, 0.2f) : 0.0f);
            pImpl->decayRate = 0.05f + (pImpl->rng ? pImpl->rng->getFloat(-0.02f, 0.02f) : 0.0f);
            
            NLM_LOG_INFO("Curiosity system reset with parameter adaptation");
        }
    }
}

} // namespace nlm
