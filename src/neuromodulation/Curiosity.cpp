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
    // Compute curiosity as weighted combination of novelty and prediction error
    // This drives exploration behavior when internal models are inaccurate
    
    // Calculate combined drive from novelty and prediction error
    float noveltyDrive = novelty * pImpl->noveltyWeight;
    float errorDrive = predictionError * pImpl->predictionErrorWeight;
    float combinedDrive = noveltyDrive + errorDrive;
    
    // Apply adaptive filtering based on recent curiosity levels (short-term memory)
    float recentAverage = 0.0f;
    if (pImpl->memorySize > 0) {
        float sum = 0.0f;
        for (size_t i = 0; i < pImpl->memorySize; ++i) {
            sum += pImpl->memoryTrace[i];
        }
        recentAverage = sum / static_cast<float>(pImpl->memorySize);
    }
    
    // Adaptive scaling: reduce response if recent curiosity has been high
    float adaptationFactor = 1.0f - 0.1f * std::min(1.0f, recentAverage / pImpl->saturationLevel);
    
    // Compute target level with adaptive gain
    float targetLevel = std::min(1.0f, combinedDrive * adaptationFactor);
    
    // Multi-timescale dynamics: fast decay for quick responses, slow recovery to baseline
    float fastDecay = pImpl->decayRate * 2.0f;  // Faster decay for immediate curiosity
    float slowDecay = pImpl->decayRate;        // Slower recovery to baseline
    
    // Smooth transition to target level
    pImpl->level = pImpl->level * (1.0f - fastDecay * static_cast<float>(dt)) + 
                   targetLevel * (fastDecay * static_cast<float>(dt));
    
    // Natural decay back to baseline
    pImpl->level = pImpl->level * (1.0f - slowDecay * static_cast<float>(dt)) + 
                   pImpl->baselineLevel * (slowDecay * static_cast<float>(dt));
    
    // Ensure level stays within bounds
    pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level));
    
    // Update memory trace
    pImpl->memoryTrace[pImpl->memoryIndex] = pImpl->level;
    pImpl->memoryIndex = (pImpl->memoryIndex + 1) % 50;
    if (pImpl->memorySize < 50) pImpl->memorySize++;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
