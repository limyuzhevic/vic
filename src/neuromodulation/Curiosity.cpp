#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float recentDrive;  // Track recent curiosity drive for exploration
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , recentDrive(0.0f) {}
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
    // More biologically realistic curiosity system
    // Based on intrinsic motivation theory: curiosity driven by both novelty and prediction error
    
    // Novelty component (sensitivity to new experiences)
    float noveltyDrive = std::tanh(novelty * pImpl->noveltyWeight);
    
    // Prediction error component (uncertainty-driven exploration)
    float uncertaintyDrive = std::tanh(std::abs(predictionError) * pImpl->predictionErrorWeight);
    
    // Combined drive with multiplicative interaction
    float target = (noveltyDrive * 0.7f + uncertaintyDrive * 0.3f) * (1.0f + pImpl->predictionErrorWeight);
    
    // More sophisticated update rule with adaptive learning rate
    float learningRate = 0.2f + 0.3f * pImpl->predictionErrorWeight;
    pImpl->level += (target - pImpl->level) * learningRate;
    
    // Biological constraints and decay
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Store recent drive for exploration decisions
    pImpl->recentDrive = pImpl->level;
}

float Curiosity::getRecentDrive() const {
    return pImpl->recentDrive;
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
