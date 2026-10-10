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
    // Enhanced curiosity dynamics with more realistic integration
    
    // Compute curiosity drive from multiple sources
    float noveltyComponent = pImpl->noveltyWeight * std::tanh(novelty);  // Bounded response
    float predictionErrorComponent = pImpl->predictionErrorWeight * std::tanh(predictionError);
    
    float target = noveltyComponent + predictionErrorComponent;
    
    // Smooth but responsive integration
    float learningRate = 0.15f;  // Faster learning
    float maxChange = 0.1f;  // Limit per timestep
    
    float change = std::clamp((target - pImpl->level) * learningRate, -maxChange, maxChange);
    pImpl->level += change;
    
    // Apply decay with memory retention
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = std::max(0.0f, pImpl->level * decayFactor);
    
    // Ensure within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Curiosity::getExplorationDrive() const {
    // Enhanced exploration drive calculation
    // Incorporate both immediate and accumulated curiosity
    
    // Base exploration on current curiosity level
    float drive = pImpl->level;
    
    // Add memory factor for persistent exploration tendencies
    static float accumulatedCuriosity = 0.0f;
    accumulatedCuriosity = std::max(0.0f, accumulatedCuriosity - 0.05f * pImpl->decayRate);
    accumulatedCuriosity = std::min(1.0f, accumulatedCuriosity + pImpl->level * 0.02f);
    
    // Combine current and accumulated drive
    drive = std::sqrt(drive * drive + accumulatedCuriosity * accumulatedCuriosity * 0.3f);
    
    return drive;
}

void Curiosity::setNoveltyWeight(float weight) {
    // Ensure weight stays within reasonable bounds
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    // Ensure weight stays within reasonable bounds
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::reset() {
    // Reset with memory of previous curiosity levels
    pImpl->level = 0.0f;
    
    // Keep some accumulated curiosity for behavioral consistency
    static float memory = 0.0f;
    memory = std::max(0.0f, memory - 0.1f);
}

} // namespace nlm
