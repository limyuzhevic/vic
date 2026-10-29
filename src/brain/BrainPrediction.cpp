#include "BrainHeaders.hpp"

namespace nlm {

void BrainPrediction::updatePredictionSystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* predictionSys = brain->getPredictionSystem();
    if (predictionSys) {
        predictionSys->update(timestep);
    }
}

void BrainPrediction::computePredictionError(Brain* brain) {
    if (!brain) return;
    auto* predictionSys = brain->getPredictionSystem();
    auto* predictionError = brain->getPredictionErrorSignal();
    if (predictionSys && predictionError) {
        predictionError->update(predictionSys->getPredictionError());
    }
}

void BrainPrediction::updateForwardModel(Brain* brain) {
    if (!brain) return;
    auto* predictionSys = brain->getPredictionSystem();
    if (predictionSys) {
        predictionSys->updateForwardModel();
    }
}

void BrainPrediction::integratePredictionWithAction(Brain* brain) {
    if (!brain) return;
    auto* predictionSys = brain->getPredictionSystem();
    auto* planner = brain->getPlanner();
    if (predictionSys && planner) {
        planner->setPredictionSignal(predictionSys->getPredictionStrength());
    }
}

} // namespace nlm