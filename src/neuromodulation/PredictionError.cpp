#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionError::Impl {
    Brain* brain;
    float level;
    float prediction;
    
    Impl() : brain(nullptr), level(0.0f), prediction(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {
    NLM_LOG_INFO("Prediction error system initialized");
}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    reset();
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::update(TimestepDuration dt) {
    // Simple decay over time
    pImpl->level *= (1.0f - 0.01f * dt);
}

void PredictionError::setPredicted(float prediction) {
    pImpl->prediction = prediction;
}

void PredictionError::updateWithActual(float actual) {
    // Compute prediction error (absolute difference)
    pImpl->level = std::abs(actual - pImpl->prediction);
    
    // Scale error to reasonable range
    pImpl->level = std::min(pImpl->level, 1.0f);
}

void PredictionError::reset() {
    pImpl->level = 0.0f;
    pImpl->prediction = 0.0f;
}

} // namespace nlm
