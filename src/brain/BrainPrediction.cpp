// Brain prediction implementation
#include "BrainPrediction.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainPrediction::updatePredictionSystem(Brain* brain) {
    if (!brain || !brain->getPredictionSystem()) return;
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

float BrainPrediction::getPredictionError(Brain* brain) {
    if (!brain || !brain->getPredictionErrorSignal()) return 0.0f;
    return brain->getPredictionErrorSignal()->getLevel();
}

void BrainPrediction::reset(Brain* brain) {
    if (brain && brain->getPredictionSystem()) {
        // TODO: Implement prediction system reset
    }
}

void BrainPrediction::enable(Brain* brain, bool enable) {
    if (brain && brain->getPredictionSystem()) {
        // TODO: Implement prediction system enable/disable
    }
}

bool BrainPrediction::isEnabled(Brain* brain) {
    if (!brain || !brain->getPredictionSystem()) return false;
    return true; // Simplified - prediction system is always enabled
}

} // namespace nlm