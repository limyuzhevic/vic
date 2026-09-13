#include "BrainPrediction.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

void PredictionSystem::initialize(class Brain* brain) {
    brainRef = brain;
    predictionError = 0.0f;
    predictionAccuracy = 1.0f;
}

void PredictionSystem::update(float timestep) {
    // Simple prediction update - in a real system this would use
    // sensory input and memory to predict next states
    predictionError *= 0.95f; // Decay prediction error over time
    predictionAccuracy = std::max(0.0f, predictionAccuracy - 0.01f * timestep);
}

float PredictionSystem::getPredictionError() const {
    return predictionError;
}

void PredictionSystem::reset() {
    predictionError = 0.0f;
    predictionAccuracy = 1.0f;
}

} // namespace nlm