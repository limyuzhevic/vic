// Brain prediction implementation - handles prediction system integration
#include "BrainPrediction.hpp"
#include "Brain.hpp"
#include "PredictionSystem.hpp"
#include <stdexcept>

namespace nlm {

BrainPrediction::BrainPrediction(std::shared_ptr<Config> config) : Brain(config) {
    predictionSystem = std::make_unique<PredictionSystem>(*this);
}

BrainPrediction::~BrainPrediction() = default;

bool BrainPrediction::initializePredictionSystems() {
    if (!predictionSystem) {
        return false;
    }
    
    return predictionSystem->initialize();
}

void BrainPrediction::updatePredictionSystems(TimestepDuration dt) {
    if (predictionSystem) {
        predictionSystem->update(dt);
    }
}

bool BrainPrediction::initialize() {
    if (!Brain::initialize()) {
        return false;
    }
    
    if (!initializePredictionSystems()) {
        return false;
    }
    
    return true;
}

void BrainPrediction::step(SimulationStep currentStep) {
    Brain::step(currentStep);
}

void BrainPrediction::step(SimulationStep currentStep, Timestamp currentTime) {
    Brain::step(currentStep, currentTime);
    
    if (predictionSystem) {
        predictionSystem->update(static_cast<TimestepDuration>(currentTime));
    }
}

PredictionSystem* BrainPrediction::getPredictionSystem() {
    return predictionSystem.get();
}

} // namespace nlm
