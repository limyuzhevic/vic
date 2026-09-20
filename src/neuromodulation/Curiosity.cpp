#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

Curiosity::Curiosity() : pImpl(std::make_unique<Impl>()) {}

Curiosity::~Curiosity() = default;

Curiosity::Curiosity(Curiosity&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Curiosity& Curiosity::operator=(Curiosity&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Curiosity::initialize(Brain* brain) {
    if (pImpl) {
        pImpl->brain = brain;
    }
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Implement curiosity-driven exploration
    // Two main drivers:
    // 1. Novelty detection: how new or different is this experience?
    // 2. Prediction error: how unexpected is this outcome?
    
    // Compute weighted combination of novelty and prediction error
    float targetLevel = pImpl->noveltyWeight * novelty + 
                       pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update to avoid sudden changes
    // Curiosity changes gradually based on current driving forces
    float timeDelta = static_cast<float>(dt);
    float updateRate = 0.1f;  // Adjust based on timestep for stability
    
    pImpl->level += (targetLevel - pImpl->level) * updateRate * timeDelta;
    
    // Apply biologically plausible constraints
    // Curiosity is bounded between 0 and 1 (or higher for exploration)
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);  // Allow up to 2.0 for high exploration
    
    // Implement decay towards baseline
    // Without stimulation, curiosity naturally decreases
    // This prevents runaway exploration
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * timeDelta);
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
