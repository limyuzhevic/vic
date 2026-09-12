// Prediction system implementation - handles prediction, error computation, and action selection
#include "PredictionSystem.hpp"
#include "Brain.hpp"
#include <memory>

namespace nlm {

PredictionSystem::PredictionSystem(Brain& brainRef) : brain(brainRef) {
    currentPrediction = std::make_unique<Prediction>();
    predictionError = std::make_unique<PredictionError>();
}

PredictionSystem::~PredictionSystem() = default;

bool PredictionSystem::initialize() {
    if (!currentPrediction || !predictionError) {
        return false;
    }
    
    return true;
}

void PredictionSystem::update(TimestepDuration dt) {
    if (currentPrediction) {
        currentPrediction->update(dt);
    }
}

std::unique_ptr<Prediction> PredictionSystem::predict() {
    if (currentPrediction) {
        return std::make_unique<Prediction>(*currentPrediction);
    }
    return nullptr;
}

void PredictionSystem::updateWithError(const class PredictionError& error) {
    if (currentPrediction && predictionError) {
        currentPrediction->updateWithError(error);
        predictionError->update(error);
    }
}

} // namespace nlm
